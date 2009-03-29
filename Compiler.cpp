// Compiler.cpp: implementation of the Compiler class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "chitem.h"
#include "Compiler.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <assert.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Compiler::Compiler(int arg_log)
{
	logging = arg_log;
	flog = 0;
	fhandle = 0;
	fout = 0;
	buffer = 0;
	bpoi = 0;
	valid = 0;
  state = 0;
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

Compiler::~Compiler()
{
	CloseAll();
	if (buffer) free(buffer);
}

void Compiler::CloseAll()
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

bool Compiler::Compile(CString filepath, CString output)
{
  linecount=1;
	bpoi = 0;
	valid = 0;
	if (logging && !flog)
	{
		unlink(WEIDU_LOG); //removing the logfile
		flog = open(WEIDU_LOG, O_RDWR|O_CREAT|O_TRUNC, S_IWRITE|S_IREAD);
		if (flog<0) flog = 0;
	}

	if (fhandle) abort();

	fhandle=open(filepath,O_RDONLY|O_BINARY,S_IREAD);
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

  //output the script
  state = TA_IF;
  WriteString("SC\n");
	//ok we have all the files now
  while (!ProcessLine());
  WriteString("SC\n");

  CloseAll();
  return false;
}

//different from the decompiler readline, this one ignores // characters
void Compiler::ReadLine(int maxlength)
{
	int i;
  bool comment;

	if (!buffer)
	{
		buffer = (char *) malloc(READ_BUFFERLENGTH);
		valid = 0;
		bpoi = 0;
	}
  	
	assert(maxlength<=READ_BUFFERLENGTH);
	line.Empty();
	if (!buffer)
	{
		return;
	}  
retry:
  if(valid<bpoi)
  {
    return;
  }
	if (bpoi+maxlength>valid)
	{
		memmove(buffer,buffer+bpoi, valid-bpoi);
		valid-=bpoi;
    buffer[valid]=0;
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
  comment=false;
	for(i=bpoi;i<bpoi+valid;i++)
	{
		if (buffer[i]=='\r' || buffer[i]=='\n')
		{
			buffer[i]=0;
      comment=false;
			break;
		}
    if (buffer[i]=='/' && (i+1<bpoi+valid) && buffer[i+1]=='/')
    {
      buffer[i]=0;
      buffer[i+1]=0;
      comment=true;
      continue;
    }
    if(comment) buffer[i]=0;
	}

  //This conversion always sets the linelength to maxlength, which is wrong
	line = CString(buffer+bpoi, maxlength);  
  line.ReleaseBuffer();//correcting the line value
	bpoi=i+1;
  line.TrimLeft();
  line.TrimRight();

	if (line.IsEmpty())
	{
		goto retry;
	}
}

void Compiler::WriteString(CString tmp)
{
	write(fout,tmp, tmp.GetLength());
}

void Compiler::WriteError(CString out)
{
  CString tmp;

  if(flog)
  {
    tmp.Format("%d: %s\r\n", linecount, out);
	  write(flog, tmp, tmp.GetLength());
  }
}

void Compiler::WriteObject(object &ob)
{
  int i;
  CString tmp;

  WriteString("OB\n");
  for(i=0;i<ObjectFieldsCount+MaxObjectNesting;i++)
  {
    tmp.Format("%d ", ob.bytes[i]);
    WriteString(tmp);
  }

  if(HasAdditionalRect)
  {
    tmp.Format("%d.%d.%d.%d ",ob.area[0],ob.area[1],ob.area[2],ob.area[3]);
    WriteString(tmp);
  }

  tmp.Format("\"%s\"",ob.var);
  WriteString(tmp);

  for(i=0;i<ExtraParametersCount;i++)
  {
    tmp.Format("%d ", ob.bytes2[i]);
    WriteString(tmp);
  }
  //TODO write object
  WriteString("OB\n");
}

bool Compiler::HandleAction()
{
  int i;
  int err;
  CString tmp;
  action action;

  ReadLine(1024);
  if (line.CompareNoCase("END"))
  {
    err = compile_action(line, action, false);
    
    tmp.Format("AC\n%d", action.opcode);
    WriteString(tmp);

    for(i=0;i<3;i++)
    {
      WriteObject(action.obj[i]);
    }
    for(i=0;i<5;i++)
    {
      //no space before var1
      if(i!=4)
      {
        tmp.Format("%d ", action.bytes[i]);
      }
      else
      {
        tmp.Format("%d", action.bytes[i]);
      }
      WriteString(tmp);
    }
    tmp.Format("\"%s\" \"%s\" ", action.var1, action.var2);
    WriteString(tmp);
    
    //TODO output the action
    //
    WriteString("AC\n");
    return false;
  }
  WriteString("RE\nRS\nCR\n");
  state = TA_IF;
  return false;
}

bool Compiler::HandleResponse()
{
  CString num;
  int value;

  ReadLine(1024);
  if(num_or==-1)
  {
    num_or=0;
    WriteString("RS\n");
  }
  if(line.Left(10).CompareNoCase("RESPONSE #"))
  {
    return true;
  }
  num = line.Mid(10);
  value=-1;
  sscanf(num,"%d", &value);
  if (value<1 || value>100)
  {
    WriteError("Invalid response #");
  }
  WriteString("RE\n"+num);
  state=TA_ACTION;
  return false;
}

bool Compiler::HandleTrigger()
{
  int err;
  CString tmp;
  trigger trigger;

  ReadLine(1024);
  if(line.CompareNoCase("THEN"))
  {
    if(num_or==-1)
    {
      WriteString("CO\n");
      num_or=0;
    }
    err = compile_trigger(line, trigger);
    if (err)
    {
      //TODO use WriteError to explain the error
      //
    }

    if((trigger.opcode&0x3fff)==TR_OR)
    {
      if (num_or)
      {
        WriteError("Nested OR in trigger block");
      }
      num_or = trigger.bytes[0];
    } else if (num_or) num_or--;
        
    WriteString("TR\n");
    //TODO output the trigger
    //
    tmp.Format("%d %d %d %d %d ", 
        trigger.opcode, trigger.bytes[0], trigger.negate,
        trigger.bytes[1], trigger.bytes[2]);
    WriteString(tmp);

    if(HasTriggerPoint)
    {
      tmp.Format("[%d,%d] ", trigger.bytes[3],trigger.bytes[4]);
      WriteString(tmp);
    }
    tmp.Format("\"%s\" \"%s\" ", trigger.var1, trigger.var2);
    WriteString(tmp);
    WriteObject(trigger.trobj);
    WriteString("TR\n");
    return false;
  }

  if (num_or<0)
  {
    //there was nothing in the trigger block (IF/THEN without any triggers)
    WriteError("Missing trigger block");
    state = TA_RESPONSE; //this is just a guessed state we are already off track
    return true;
  }
  //at the end of the trigger block, we got incomplete or
  if (num_or)
  {
    WriteError("Incomplete OR in trigger block");
  }
  //output the condition closer marker
  WriteString("CO\n");
  state = TA_RESPONSE;
  num_or=-1;
  return false;
}

bool Compiler::ProcessLine()
{
  switch(state)
  {
  case TA_IF:
    ReadLine(10);
    if(line.CompareNoCase("IF")) return true;
    state=TA_TRIGGER;
    WriteString("CR\n");
    num_or=-1;
    break;
  case TA_TRIGGER:
    if (HandleTrigger()) return true;
    break;
  case TA_ACTION:
    if (HandleAction()) return true;
    break;
  case TA_RESPONSE:
    if (HandleResponse())
    {
      if(line.CompareNoCase("END"))
      {
        WriteString("RE\nCR\n");
        state = TA_IF;
      } else return true;
    }
    break;
  }

  return false;
}
