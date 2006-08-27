// Decompiler.cpp: implementation of the Decompiler class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "chitem.h"
#include "Decompiler.h"
#include "chitem.h"
#include "2da.h"
#include "options.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <assert.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Helper classes
//////////////////////////////////////////////////////////////////////
/*************** point ****************************/
Point::Point(void)
{
	x = y = 0;
	//memset(this, 0, sizeof(*this));
}

Point::~Point(void)
{
}

Point::Point(const Point& pnt)
{
	x=pnt.x;
	y=pnt.y;
	//memcpy(this, &pnt, sizeof(*this));
}

Point& Point::operator=(const Point& pnt)
{
	x = pnt.x;
	y = pnt.y;
	//memcpy(this, &pnt, sizeof(*this));
	return *this;
}

bool Point::operator==(const Point& pnt)
{
	if (( x == pnt.x ) && ( y == pnt.y ))
	{
		return true;
	}
	return false;
}

Point::Point(short x, short y)
{
	this->x = x;
	this->y = y;
}

bool Point::isnull() const
{
	if (x==0 && y==0)
	{
		return true;
	}
	return false;
}

bool Point::isempty() const
{
	if (x==-1 && y==-1)
	{
		return true;
	}
	return false;
} 

/** Self-destructing object if it is empty */
bool Object::ReadyToDie(int ObjectFieldsCount)
{
	if (objectName[0]!=0)
	{
		return false;
	}
	if (objectFilters[0])
	{
		return false;
	}
	for (int i=0;i<ObjectFieldsCount;i++)
	{
		if (objectFields[i])
		{
			return false;
		}
	}
	return true;
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Decompiler::Decompiler(int arg_log)
{
	logging = arg_log;
	flog = 0;
	fhandle = 0;
	fout = 0;
	buffer = 0;
	bpoi = 0;
	valid = 0;
	ObjectFieldsCount = 7;
	MaxObjectNesting = 5;
	ExtraParametersCount = 0;
	HasAdditionalRect = false;
	HasTriggerPoint = false;
	if (pst_compatible_var())
	{
		ObjectFieldsCount += 2;
		HasTriggerPoint = true;
		HasAdditionalRect = true;
	}
	if (has_xpvar())
	{
		HasAdditionalRect = true;
	}
	if (iwd2_structures())
	{
		ObjectFieldsCount++;
		ExtraParametersCount += 2;
	}
}

Decompiler::~Decompiler()
{
	CloseAll();
	if (buffer) free(buffer);
}

void Decompiler::CloseAll()
{
	if (flog) close(flog);
	flog = 0;
	if (fhandle) close(fhandle);
	fhandle = 0;
	if (fout) close(fout);
	fout = 0;
	bpoi = 0;
	valid = 0;
}

void Decompiler::ReadLine(int maxlength)
{
	int i;

	if (!buffer)
	{
		buffer = (char *) malloc(READ_BUFFERLENGTH);
		valid = 0;
		bpoi = 0;
	}
	assert(valid>=bpoi);
	assert(maxlength<=READ_BUFFERLENGTH);
	line.Empty();
	if (!buffer)
	{
		return;
	}
retry:
	if (bpoi+maxlength>valid)
	{
		memmove(buffer,buffer+bpoi, valid-bpoi);
		valid-=bpoi;
		bpoi=0;
	}
	valid += read(fhandle, buffer+valid, READ_BUFFERLENGTH-valid);
	if (valid<1)
	{
		return;
	}
	if (valid<maxlength)
	{
		maxlength=valid;
	}
	for(i=bpoi;i<bpoi+maxlength;i++)
	{
		if (buffer[i]=='\r' || buffer[i]=='\n')
		{
			buffer[i]=0;
			break;
		}
	}
	line = CString(buffer+bpoi);
	bpoi=i+1;
	if (line.IsEmpty())
	{
		goto retry;
	}
}

void Decompiler::WriteString(CString tmp)
{
	write(fout,tmp, tmp.GetLength());
}

int Decompiler::ParseInt(int &pos)
{
	char number[33];

	char* tmp = number;
	while (isdigit(line[pos]) || line[pos]=='-')
	{
		*tmp = line[pos];
		tmp++;
		pos++;
	}
	*tmp = 0;
	pos++;
	return atoi( number );
}

void Decompiler::ParseString(int &pos, char* tmp)
{
	while (line[pos] != '"')
	{
		*tmp = line[pos];
		if (!*tmp) break;
		tmp++;
		pos++;
	}
	*tmp = 0;
} 

CString Decompiler::PrintObject(Object *oB)
{
	CString ret;
	CString tmpstr;
	int i,j;
	bool flg = false;

	if (oB->objectName[0])
	{
		ret.Format("\"%s\"",oB->objectName);
	}
	else
	{
		ret="[";
		j = ObjectFieldsCount;
		while(j--)
		{
			if(oB->objectFields[j]) break;
		}
		for (i = 0; i <= j; i++)
		{
			tmpstr=IDSToken(IDSName(i,false), oB->objectFields[i], false);
			if (tmpstr.IsEmpty()) tmpstr.Format("%d",oB->objectFields[i]);
			tmpstr.MakeUpper();
			if (flg) ret+=".";
			ret+=tmpstr;
			flg=true;
		}
		ret+="]";
	}
	//
	for (i = 0; i < MaxObjectNesting; i++)
	{
		if (oB->objectFilters[i])
		{
			tmpstr=IDSToken("OBJECT",oB->objectFilters[i], false);
			if (tmpstr.IsEmpty()) tmpstr.Format("unknown");
			tmpstr.SetAt(0,(char) toupper(tmpstr[0]) );
			if (!flg)
			{
				ret = tmpstr;
				flg=true;
			}
			else
			{
				ret = tmpstr+"("+ret+")";
			}
		}
	}

	if (HasAdditionalRect)
	{
		ret+=PrintRegion(oB->objectRect);
	}
	return ret;
}

Object* Decompiler::DecodeObject(int &pos)
{
	int i;
	pos = 0;

	Object* oB = new Object();
	for (i = 0; i < ObjectFieldsCount; i++)
	{
		oB->objectFields[i] = ParseInt( pos );
	}
	for (i = 0; i < MaxObjectNesting; i++)
	{
		oB->objectFilters[i] = ParseInt( pos );
	}
	//iwd tolerates the missing rectangle, so we do so too
	if (HasAdditionalRect && (line[pos]=='[') )
	{
		pos++; //Skip [
		for (i = 0; i < 4; i++)
		{
			oB->objectRect[i] = ParseInt( pos );
		}
		if(line[pos]!=' ') goto die;
		pos++; //Skip a ' ', the ] was skipped by the ParseInt function
  } else {
    for (i = 0; i < 4; i++)
    {
      oB->objectRect[i]=-1;
    }
  }
	if (line[pos]!='"') goto die;
	pos++; //Skip "
	ParseString( pos, oB->objectName );
	if (line[pos]!='"') goto die;
	pos++; //Skip " (the same as above)
	if (ExtraParametersCount)
	{
		if (line[pos]!=' ') goto die;
		pos++;
	}
	for (i = 0; i < ExtraParametersCount; i++)
	{
		oB->objectFields[i + ObjectFieldsCount] = ParseInt( pos );
	}
	if (line[pos]!='O' || line[pos+1]!='B' || line.GetLength()!=pos+2)
	{
		goto die;
	}
	//let the object realize it has no future (in case of null objects)
	if (oB->ReadyToDie(ObjectFieldsCount+ExtraParametersCount))
	{
die:
		delete oB;
		oB = NULL;
	}
	return oB;
}

CString Decompiler::PrintPoint(Point &pO)
{
	CString tmp;

	tmp.Format("[%d.%d]",pO.x,pO.y);
	return tmp;
}

CString Decompiler::PrintRegion(int *region)
{
	CString tmp;
	int i;
	bool flag = false;

	for (i=0;i<4;i++)
	{
		if (region[i]!=-1) flag=true;
	}
	if (flag)
	{
		tmp.Format("[%d,%d,%d,%d]",region[0],region[1],region[2],region[3]);
	}
	return tmp;
}

int Decompiler::FindColon(const char *variable, int add)
{
	for(int i=0;i<65;i++)
	{
		if (variable[i]=='\0') return i;
		if (variable[i]==':') return i+add;
	}
  if (add) {
    return 65;
  }
	return 0;
}

int Decompiler::IsAreaUsed(compiler_data &value, int nextpar)
{
	if (value.parnum<=nextpar) return 0;
	if (value.parameters[nextpar].type==SPT_AREA1) return 6;
	if (value.parameters[nextpar].type==SPT_AREA2) return 6;
	//
	return 0;
}

int Decompiler::guess_paramcount(Trigger *tR)
{
	int ret;

	if (tR->int2Parameter) ret = 3;
	else if(tR->int1Parameter) ret = 2;
	else if(tR->int0Parameter) ret = 1;
	else ret = 0;
	if (tR->string1Parameter[0]) ret+=0x20;
	else if(tR->string0Parameter[0]) ret+=0x10;
	if (!tR->pointParameter.isnull()) ret++;
	if (tR->objectParameter) ret++;
	return ret;
}

CString Decompiler::PrintTrigger(Trigger *tR)
{
	CString tmp, key, ob, params, comment;
	compiler_data value;
	int i;
	int areaused;

	i = guess_paramcount(tR);
  if (!trigger_data.ReverseLookup(tR->triggerID, i, key, value))
  {
    if (!trigger_data.ReverseLookup(tR->triggerID, key, value))
    {
      key.Format("unknown_%d",tR->triggerID);
    }
  }
	for (i=0;i<value.parnum;i++)
	{
		if (i) params = params+",";
		switch(value.parameters[i].type)
		{
		case SPT_ACTION:   //actionoverride in trigger, no way!
		case SPT_OVERRIDE: //actionoverrided object
		case SPT_SECONDOB: //second object in trigger
		case SPT_STRREF:   //no strref in triggers
		case SPT_STRREF2:
		case SPT_STRREF3:
			ob ="invalid";
			break;
		case '0':  //null argument, simply skip it (not in the original compiler)
			break;
		default:
			ob = "internal_error";
			break;
		case SPT_POINT:
			ob = PrintPoint(tR->pointParameter);
			break;
		case SPT_INTEGER:
			//lookup IDS, or simple number
			ob=IDSToken(value.parameters[i].idsfile, tR->int0Parameter, false);
			if (ob.IsEmpty()) ob.Format("%d",tR->int0Parameter);
			ob.MakeUpper();
			break;
		case SPT_INTEGER2:
			ob=IDSToken(value.parameters[i].idsfile, tR->int1Parameter, false);
			if (ob.IsEmpty()) ob.Format("%d",tR->int1Parameter);
			ob.MakeUpper();
			break;
		case SPT_INTEGER3:
			ob=IDSToken(value.parameters[i].idsfile, tR->int2Parameter, false);
			if (ob.IsEmpty()) ob.Format("%d",tR->int2Parameter);
			ob.MakeUpper();
			break;
		case SPT_DEAD1:
		case SPT_VAR1:
			areaused = IsAreaUsed(value,i+1);
			ob.Format("\"%s\"",CString(tR->string0Parameter+areaused) );
			break;
		case SPT_COLUMN1:
			ob.Format("\"%s\"",CString(tR->string0Parameter) );
			break;
		case SPT_TOKEN1:
			ob.Format("\"%s\"",CString(tR->string0Parameter) );
			break;
		case SPT_RESREF1: //resource
			ob.Format("\"%s\"",CString(tR->string0Parameter) );
			ob.MakeUpper();
			break;
    case SPT_1AREA:
		case SPT_AREA1: //area (only for var1)
			ob.Format("\"%s\"",CString(tR->string0Parameter,6) );
			ob.MakeUpper();
			break;
		case SPT_VAR4:
			areaused = FindColon(tR->string1Parameter, 1);
			ob.Format("\"%s\"",CString(tR->string1Parameter+areaused) );
			break;
		case SPT_VAR3:
			areaused = FindColon(tR->string0Parameter, 1);
			ob.Format("\"%s\"",CString(tR->string0Parameter+areaused) );
			break;
		case SPT_AREA4:
			areaused = FindColon(tR->string1Parameter, 0);
			ob.Format("\"%s\"",CString(tR->string1Parameter,areaused) );
			break;
		case SPT_AREA3:
			areaused = FindColon(tR->string0Parameter, 0);
			ob.Format("\"%s\"",CString(tR->string0Parameter,areaused) );
			break;
		case SPT_VAR2:
			areaused = IsAreaUsed(value,i+1);
			ob.Format("\"%s\"",CString(tR->string1Parameter+areaused) );
			break;
		case SPT_COLUMN2:
			ob.Format("\"%s\"",CString(tR->string1Parameter) );
			break;
		case SPT_TOKEN2:
			ob.Format("\"%s\"",CString(tR->string1Parameter) );
			break;
		case SPT_RESREF2: //resource
			ob.Format("\"%s\"",CString(tR->string1Parameter) );
			ob.MakeUpper();
			break;
    case SPT_2AREA:
		case SPT_AREA2: //area (only for var2)
			ob.Format("\"%s\"",CString(tR->string1Parameter,6) );
			ob.MakeUpper();
			break;
		case SPT_OBJECT: //only O exists in triggers
			if (tR->objectParameter)
			{
				ob = PrintObject(tR->objectParameter);
			}
			else
			{
				tmp = IDSToken("ea", 0, false);
				if (tmp.IsEmpty()) tmp="0";
				ob.Format("[%s]", tmp);
			}
			break;
		}
		params=params+ob;
	}
	key.SetAt(0,(char) toupper(key[0]) );
	if (tR->flags&NEGATE_TRIGGER)
	{
		key = CString("!")+key;
	}
	tmp.Format("  %s(%s)",key, params);
	if (num_or)
	{
		tmp=CString("  ")+tmp;
		num_or--;
	}
	if((tR->triggerID&0x3fff)==TR_OR)
	{
		num_or=tR->int0Parameter;
	}

	if (comment.GetLength())
	{
		tmp=tmp+"   // "+comment;
	}
	return tmp+"\r\n";
}

int Decompiler::ReadTrigger()
{
	int pos;
	CString tmp;

	if (strncmp( line, "TR", 2 ) != 0)
	{
		return 1;
	}
	ReadLine( 1024 );
	Trigger* tR = new Trigger();
	//this exists only in PST, but not always?
	if (HasTriggerPoint)
	{
		pos=sscanf( line, "%hu %d %d %d %d [%hd,%hd] \"%[^\"]\" \"%[^\"]\" OB",
			&tR->triggerID, &tR->int0Parameter, &tR->flags,
			&tR->int1Parameter, &tR->int2Parameter, &tR->pointParameter.x,
			&tR->pointParameter.y, tR->string0Parameter, tR->string1Parameter );
	}
	else
	{
		pos=0;
	}
	if (pos<7)
	{
		sscanf( line, "%hu %d %d %d %d \"%[^\"]\" \"%[^\"]\" OB",
			&tR->triggerID, &tR->int0Parameter, &tR->flags,
			&tR->int1Parameter, &tR->int2Parameter, tR->string0Parameter,
			tR->string1Parameter );
	}
	ReadLine( 1024 );
	tR->objectParameter = DecodeObject(pos);
	ReadLine( 10 );
	if (strncmp( line, "TR", 2 ) != 0)
	{
		delete tR;
		return 1;
	}

	//
	//write trigger
	//
	tmp = PrintTrigger(tR);
	WriteString(tmp);
	delete tR;
	return 0;
}

int Decompiler::ReadCondition()
{
	ReadLine( 10 );
	if (strncmp( line, "CO", 2 ) != 0)
	{
		return 1;
	}
	WriteString("IF\r\n");
	num_or = 0;
	while (true)
	{
		ReadLine( 10 );
		if (strncmp( line, "CO", 2 ) == 0)
		{
			break;
		}
		if(ReadTrigger( )) {
			return 1;
		}
	}
	return 0;
}

int Decompiler::guess_paramcount(Action *aC)
{
	int ret;

	if (aC->int2Parameter) ret = 3;
	else if(aC->int1Parameter) ret = 2;
	else if(aC->int0Parameter) ret = 1;
	else ret = 0;
	if (aC->string1Parameter[0]) ret+=0x20;
	else if(aC->string0Parameter[0]) ret+=0x10;
	if (!aC->pointParameter.isnull()) ret++;
	if (aC->objects[2]) ret+=2;
	else if(aC->objects[1]) ret++;
	return ret;
}

CString Decompiler::GetComment(int strref)
{
	CString tmp, comment;

	if (!(weiduflg&WEI_TEXT)) goto end_of_comment;
	if (weiduflg&WEI_NOCOM) goto end_of_comment;
	comment.Format("~%s~", resolve_tlk_text(strref, 0) );
	comment.Replace("\r\n","");
	if (comment.GetLength()>50) comment=comment.Left(50)+"...";
	tmp = resolve_tlk_soundref(strref, 0);
	if (tmp.GetLength()) comment+=CString("  [")+tmp+"]";

end_of_comment:
	return comment;
}

CString Decompiler::PrintAction(Action *aC)
{
	CString tmp, key, ob, params, comment;
	compiler_data value;
	int i;
	int areaused;

  //strings mean +0x10
	i = guess_paramcount(aC);
	if (!action_data.ReverseLookup(aC->actionID, i, key, value))
	{
		if (!action_data.ReverseLookup(aC->actionID, key, value))
		{
			key.Format("unknown_%d",aC->actionID);
		}
	}
	for (i=0;i<value.parnum;i++)
	{
		if (i) params = params+",";
		switch(value.parameters[i].type)
		{
		case 0:  //null argument
			break;
		default:
			ob = "invalid";
			break;
		case SPT_ACTION:
			ob = "invalid";
			break;
		case SPT_INTEGER:
			ob=IDSToken(value.parameters[i].idsfile, aC->int0Parameter, false);
			if (ob.IsEmpty()) ob.Format("%d",aC->int0Parameter);
			ob.MakeUpper();
			break;
		case SPT_STRREF:
			ob.Format("%d",aC->int0Parameter);
			comment = GetComment(aC->int0Parameter);
			break;
		case SPT_INTEGER2:
			ob=IDSToken(value.parameters[i].idsfile, aC->int1Parameter, false);
			if (ob.IsEmpty()) ob.Format("%d",aC->int1Parameter);
			ob.MakeUpper();
			break;
		case SPT_STRREF2:
			ob.Format("%d",aC->int1Parameter);
			comment = GetComment(aC->int1Parameter);
			break;
		case SPT_INTEGER3:
			ob=IDSToken(value.parameters[i].idsfile, aC->int2Parameter, false);
			if (ob.IsEmpty()) ob.Format("%d",aC->int2Parameter);
			ob.MakeUpper();
			break;
		case SPT_STRREF3:
			ob.Format("%d",aC->int2Parameter);
			comment = GetComment(aC->int2Parameter);
			break;
		case SPT_VAR1:
			areaused = IsAreaUsed(value,i+1);
			ob.Format("\"%s\"",CString(aC->string0Parameter+areaused) );
			break;
		case SPT_COLUMN1:
			ob.Format("\"%s\"",CString(aC->string0Parameter) );
			break;
		case SPT_NUMLIST1:
			ob.Format("\"%s\"",CString(aC->string0Parameter) );
			break;
		case SPT_TOKEN1:
			ob.Format("\"%s\"",CString(aC->string0Parameter) );
			break;
		case SPT_RESREF1: //resource
			ob.Format("\"%s\"",CString(aC->string0Parameter) );
			ob.MakeUpper();
			break;
    case SPT_1AREA:
		case SPT_AREA1: //area (only for var1)
			ob.Format("\"%s\"",CString(aC->string0Parameter,6) );
			ob.MakeUpper();
			break;
		case SPT_VAR2:
			areaused = IsAreaUsed(value,i+1);
			ob.Format("\"%s\"",CString(aC->string1Parameter+areaused) );
			break;
		case SPT_VAR4:
    case SPT_RES4:
			areaused = FindColon(aC->string1Parameter, 1);
			ob.Format("\"%s\"",CString(aC->string1Parameter+areaused) );
			break;
		case SPT_VAR3:
    case SPT_RES2:
			areaused = FindColon(aC->string0Parameter, 1);
			ob.Format("\"%s\"",CString(aC->string0Parameter+areaused) );
			break;
		case SPT_AREA4:
    case SPT_RES3:
			areaused = FindColon(aC->string1Parameter, 0);
			ob.Format("\"%s\"",CString(aC->string1Parameter,areaused) );
			break;
		case SPT_AREA3:
    case SPT_RES1:
			areaused = FindColon(aC->string0Parameter, 0);
			ob.Format("\"%s\"",CString(aC->string0Parameter,areaused) );
			break;
		case SPT_COLUMN2:
			ob.Format("\"%s\"",CString(aC->string1Parameter) );
			break;
		case SPT_NUMLIST2:
			ob.Format("\"%s\"",CString(aC->string1Parameter) );
			break;
		case SPT_TOKEN2:
			ob.Format("\"%s\"",CString(aC->string1Parameter) );
			break;
		case SPT_RESREF2: //resource
			ob.Format("\"%s\"",CString(aC->string1Parameter) );
			ob.MakeUpper();
			break;
    case SPT_2AREA:
		case SPT_AREA2:
			ob.Format("\"%s\"",CString(aC->string1Parameter,6) );
			ob.MakeUpper();
			break;
		case SPT_POINT: //only one point allowed
			ob = PrintPoint(aC->pointParameter);
			break;
		case SPT_SECONDOB: //param2 object (3. place)
			if (aC->objects[2])
			{
				ob = PrintObject(aC->objects[2]);
			}
			else
			{
				ob = "anything";
			}
			break;
		case SPT_OBJECT: //regular param1 object (2. place)
			if (aC->objects[1])
			{
				ob = PrintObject(aC->objects[1]);
			}
			else
			{
				ob = "anything";
			}
			break;
		case SPT_OVERRIDE: //actionoverride object (1. place)
			ob = "invalid";
			break;
		}
		params=params+ob;
	}
	key.SetAt(0,(char) toupper(key[0]) );
	tmp.Format("%s(%s)",key, params);
	if (aC->objects[0])
	{
			ob.Format("ActionOverride(%s,%s)",PrintObject(aC->objects[0]), tmp );
			tmp=ob;
	}
	if (comment.GetLength())
	{
		tmp=tmp+"   // "+comment;
	}
	return CString("    ")+tmp+"\r\n";
}

int Decompiler::ReadResponse()
{
	CString tmp;
	int pos;
	char *poi;

	if (strncmp( line, "RE", 2 ) != 0)
	{
		return 1;
	}
	ReadLine( 10 );
	int weight = (int) strtoul(line,&poi,10);

  //empty response block
  if (strncmp(poi,"RE",2)==0)
  {
  	tmp.Format("  RESPONSE #%d         //empty response block!!!\r\n",weight);
	  WriteString(tmp);
    return 0;
  }

	if (strncmp(poi,"AC",2)!=0)
	{
		return 1;
	}

	tmp.Format("  RESPONSE #%d\r\n",weight);
	WriteString(tmp);

	while (true)
	{
		ReadLine( 10 );
		int actionid = strtoul(line, &poi,10);
		Action* aC = new Action;

		for (int i = 0; i < 3; i++)
		{
			if (strncmp(poi,"OB",2)!=0)
			{
				delete aC;
				return 1;
			}
			ReadLine( 1024 );
			aC->objects[i] = DecodeObject(pos);
			ReadLine( 1024 );
			poi=(char *) (const char*) line;
		}
		aC->actionID = (unsigned short) actionid;

		sscanf( line, "%d %hd %hd %d %d\"%[^\"]\" \"%[^\"]\" AC",
			&aC->int0Parameter, &aC->pointParameter.x, &aC->pointParameter.y,
			&aC->int1Parameter, &aC->int2Parameter, aC->string0Parameter,
			aC->string1Parameter );

		//
		//print action
		//
		tmp = PrintAction(aC);
		WriteString(tmp);

		delete aC;

		ReadLine( 10 );
		if (strncmp( line, "RE", 2 ) == 0)
		{
			break;
		}
		if (strncmp( line, "AC", 2 ) != 0)
		{
			return 1;
		}
	}
	return 0;
}

int Decompiler::ReadResponseSet()
{
	ReadLine( 10 );
	if (strncmp( line, "RS", 2 ) != 0)
	{
		return 1;
	}
	while (true)
	{
		ReadLine (10 );
		if (strncmp (line, "RS",2) == 0)
		{
			break;
		}
		if (ReadResponse())
		{
			return 1;
		}
	}
	return 0;
}

int Decompiler::ReadResponseBlock()
{
	ReadLine( 10 );
	if (strncmp( line, "CR", 2 ) != 0)
	{
		return 1;
	}
	if (ReadCondition( ))
	{
		return 1;
	}
	WriteString("THEN\r\n");
	if (ReadResponseSet())
	{
		return 1;
	}
	WriteString("END\r\n\r\n");
	ReadLine( 10 );
	if (strncmp( line, "CR", 2 ) != 0)
	{
		return 1;
	}
	return 0;
}

bool Decompiler::Decompile(CString filepath, CString output)
{
	bpoi = 0;
	valid = 0;
	if (logging && !flog)
	{
		unlink(WEIDU_LOG); //removing the logfile
		flog = open(WEIDU_LOG, O_RDWR|O_CREAT|O_TRUNC, S_IWRITE|S_IREAD);
		if (flog<0) flog = 0;
	}

	if (fhandle) abort();

	fhandle=get_script_handle(filepath);
	if (fhandle<=0)
	{
		return true;
	}

	if (fout) abort();

	fout = open(output, O_RDWR|O_CREAT|O_TRUNC, S_IWRITE|S_IREAD);
	if (fout<=0)
	{
		return true;
	}
	//ok we have all the files now
	
	ReadLine( 10 );
	if (strncmp( line, "SC", 2 ) != 0)
	{
		return true;
	}

	while (true)
	{
		if (ReadResponseBlock( ))
			break;
	}
	if (strncmp( line, "SC", 2 ) != 0)
	{
		return true;
	}
	//ok
	return false;
}
