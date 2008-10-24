// script.cpp: implementation of the script class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <ctype.h>

#include "chitem.h"
#include "script.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static CString oidPST[9]={"FACTION","TEAM","EA","GENERAL","RACE","CLASS","SPECIFIC","GENDER","ALIGN"};
static CString oidIWD[8]={"EA","GENERAL","RACE","CLASS","SPECIFIC","GENDER","ALIGNMEN"};
static CString oidIWD2[10]={"EA","GENERAL","RACE","SUBRAC","CLASS","SPECIFIC","GENDER","ALIGN","",""};
static CString oidBG[7]={"EA","GENERAL","RACE","CLASS","SPECIFIC","GENDER","ALIGNMEN"};
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Cscript::Cscript()
{
  blocks=NULL;
  blockcount=0;
  scrdata=NULL;
  maxlen=0;
  cnt=0;
  lineno=1;
  lastln=0;
}

void Cscript::NukeBlocks()
{
  int i,j;

  if(blocks)
  {
    for(i=0;i<blockcount;i++)
    {
      for(j=0;j<blocks[i].responsecount;j++)
      {
        if(blocks[i].responses[j].actions)
        {
          delete blocks[i].responses[j].actions;
          blocks[i].responses[j].actions=NULL;
        }
      }
      if(blocks[i].triggers)
      {
        delete blocks[i].triggers;
        blocks[i].triggers=NULL;
      }
      if(blocks[i].responses)
      {
        delete blocks[i].responses;
        blocks[i].responses=NULL;
      }
    }
    delete blocks;
    blocks=NULL;
  }
}

Cscript::~Cscript()
{
  NukeBlocks();
  if(scrdata)
  {
    delete scrdata;
    scrdata=NULL;
  }
}

int Cscript::SkipChar(char chr)
{
  if(scrdata[cnt]==chr)
  {
    cnt++;
    return 0;
  }
  return -1;
}

int Cscript::ReadToken(char *token, bool crlf)
{
  if(memcmp(scrdata+cnt, token, 2) ) return -1;
  if(crlf)
  {
    if(scrdata[cnt+2]!='\n') return -1;
    cnt+=3;
    lineno++;
    lastln=cnt;
  }
  else cnt+=2;
  return 0;
}

#define RNB_SCAN       0
#define RNB_STORE      1

int Cscript::WriteString(FILE *fpoi, char *where)
{
  return fprintf(fpoi,"%s",where);
}

int Cscript::ReadString(int store, char *where)
{
  int i;

  if(scrdata[cnt]!='"') return -1; //not at a string
  cnt++;
  i=0;
  if(store==RNB_STORE)
  {
    while(cnt<maxlen && scrdata[cnt]!='"') where[i++]=(char) toupper(scrdata[cnt++]);
  }
  else
  {
    while(cnt<maxlen && scrdata[cnt]!='"') cnt++;
  }
  if(cnt<maxlen && scrdata[cnt]!='"')
  {
    return -1; //not at a string
  }
  if(store==RNB_STORE) where[i]=0;
  cnt++;
  return 0;
}

int Cscript::WriteNumber(FILE *fpoi, int number)
{
  return fprintf(fpoi,"%d",number);
}

int Cscript::ReadNumber(int store, int &where)
{
  char tmp[13];
  int tmplen;

  for(tmplen=0;(scrdata[cnt]=='-' || isdigit(scrdata[cnt]))&&tmplen<12&&cnt<maxlen;tmplen++)
  {
    tmp[tmplen]=scrdata[cnt++];
  }
  tmp[tmplen]=0;
  if(tmplen)
  {
    if(store==RNB_STORE) where=atoi(tmp);
    return 0;
  }
  return -1;
}

int Cscript::WriteObjectToB(FILE *fpoi, object &object)
{
  int i;
  int res;

  res=fprintf(fpoi,"OB\n");
  if(res<=0) return res;
  for(i=0;i<12;i++)
  {
    res=WriteNumber(fpoi, object.bytes[i]);
    if(res<=0) return res;
    res=fprintf(fpoi," ");
    if(res<=0) return res;
  }
  res=WriteString(fpoi, object.var);
  if(res<=0) return res;
  return fprintf(fpoi,"OB\n");
}

int Cscript::WriteArea(FILE *fpoi, int *area)
{
  int i;
  int res;

  res=fprintf(fpoi,"[");
  if(res<=0) return res;
  for(i=0;i<3;i++)
  {
    res=WriteNumber(fpoi, area[i]);
    if(res<=0) return res;
    res=fprintf(fpoi,",");
    if(res<=0) return res;
  }
  res=WriteNumber(fpoi, area[i]);
  if(res<=0) return res;
  res=fprintf(fpoi,"]");
  return res;
}

int Cscript::WriteObjectPST(FILE *fpoi, object &object)
{
  int i;
  int res;

  res=fprintf(fpoi,"OB\n");
  if(res<=0) return res;
  for(i=0;i<14;i++)
  {
    res=WriteNumber(fpoi, object.bytes[i]);
    if(res<=0) return res;
    res=fprintf(fpoi," ");
    if(res<=0) return res;
  }
  res=WriteArea(fpoi, object.area);
  if(res<=0) return res;
  res=WriteString(fpoi, object.var);
  if(res<=0) return res;
  res=fprintf(fpoi,"OB\n");
  if(res<=0) return res;
  return res;
}

int Cscript::WriteObjectIWD(FILE *fpoi, object &object)
{
  int i;
  int res;

  res=fprintf(fpoi,"OB\n");
  if(res<=0) return res;
  for(i=0;i<12;i++)
  {
    res=WriteNumber(fpoi, object.bytes[i]);
    if(res<=0) return res;
    res=fprintf(fpoi," ");
    if(res<=0) return res;
  }
  res=WriteArea(fpoi, object.area);
  if(res<=0) return res;
  res=WriteString(fpoi, object.var);
  if(res<=0) return res;
  res=fprintf(fpoi,"OB\n");
  if(res<=0) return res;
  return res;
}

int Cscript::WriteObjectIWD2(FILE *fpoi, object &object)
{
  int i;
  int res;

  res=fprintf(fpoi,"OB\n");
  if(res<=0) return res;
  for(i=0;i<13;i++)
  {
    res=WriteNumber(fpoi, object.bytes[i]);
    if(res<=0) return res;
    res=fprintf(fpoi," ");
    if(res<=0) return res;
  }
  res=WriteArea(fpoi, object.area);
  if(res<=0) return res;
  res=WriteString(fpoi, object.var);
  if(res<=0) return res;
  for(i=0;i<2;i++)
  {
    res=WriteNumber(fpoi, object.bytes2[i]);
    if(res<=0) return res;
    res=fprintf(fpoi," ");
    if(res<=0) return res;
  }
  res=fprintf(fpoi,"OB\n");
  if(res<=0) return res;
  return res;
}

int Cscript::WriteObject(FILE *fpoi, object &object)
{
  if(iwd2_structures()) return WriteObjectIWD2(fpoi,object);
  if(has_xpvar()) return WriteObjectIWD(fpoi,object);
  if(pst_compatible_var()) return WriteObjectPST(fpoi,object);
  return WriteObjectToB(fpoi,object);
}

int Cscript::ReadObjectToB(int store, object &object)
{
  int i;

  if(ReadToken("OB",true)) return -1;
  for(i=0;i<12;i++)
  {
    if(ReadNumber(store, object.bytes[i])) return -1;
    if(SkipChar(' ')) return -1;
  }
  if(ReadString(store, object.var)) return -1;
  return ReadToken("OB",true); //end of object
}

int Cscript::ReadArea(int store, int *area)
{
  int i;

  if(SkipChar('[')) return -1;
  for(i=0;i<3;i++)
  {
    if(ReadNumber(store, area[i]) ) return -1;
    if(SkipChar('.')) return -1;
  }
  if(ReadNumber(store, area[i]) ) return -1;
  if(SkipChar(']')) return -1;
  return 0;
}

int Cscript::ReadObjectPST(int store, object &object)
{
  int i;

  if(ReadToken("OB",true)) return -1;
  for(i=0;i<14;i++)
  {
    if(ReadNumber(store, object.bytes[i]))
    {
      if(i==12) break; //no faction/team ???
      return -1; //allow crippled pst bcs
    }
    if(SkipChar(' ')) return -1;
  }
  if(ReadArea(store, object.area)) return -1;
  if(SkipChar(' ')) return -1;
  if(ReadString(store, object.var)) return -1;
  return ReadToken("OB",true); //end of object
}

int Cscript::ReadObjectIWD(int store, object &object)
{
  int i;

  if(ReadToken("OB",true)) return -1;
  for(i=0;i<12;i++)
  {
    if(ReadNumber(store, object.bytes[i])) return -1;
    if(SkipChar(' ')) return -1;
  }
  if(ReadArea(store, object.area)) return -1;
  if(SkipChar(' ')) return -1;
  if(ReadString(store, object.var)) return -1;
  return ReadToken("OB",true); //end of object
}

int Cscript::ReadObjectIWD2(int store, object &object)
{
  int i;

  if(ReadToken("OB",true)) return -1;
  for(i=0;i<13;i++)
  {
    if(ReadNumber(store, object.bytes[i])) return -1;
    if(SkipChar(' ')) return -1;
  }
  if(ReadArea(store, object.area)) return -1;
  if(SkipChar(' ')) return -1;
  if(ReadString(store, object.var)) return -1;
  if(SkipChar(' ')) return -1;
  for(i=0;i<2;i++)
  {
    if(ReadNumber(store, object.bytes2[i])) return -1;
    if(SkipChar(' ')) return -1;
  }
  return ReadToken("OB",true); //end of object
}

int Cscript::ReadObject(int store, object &object)
{
  if(iwd2_structures()) return ReadObjectIWD2(store,object);
  if(has_xpvar()) return ReadObjectIWD(store,object);
  if(pst_compatible_var()) return ReadObjectPST(store,object);
  return ReadObjectToB(store,object);
}

int Cscript::WriteTrigger(FILE *fpoi, trigger &trigger)
{
  int i;
  int res;

  if(trigger.opcode==-1) return -1;
  res=fprintf(fpoi,"TR\n");
  if(res<=0) return res;
  res=WriteNumber(fpoi, trigger.opcode);
  if(res<=0) return res;
  res=fprintf(fpoi," ");
  if(res<=0) return res;
  for(i=0;i<4;i++)
  {
    res=WriteNumber(fpoi, trigger.bytes[i]);
    if(res<=0) return res;
    res=fprintf(fpoi," ");
    if(res<=0) return res;
  }
  res=WriteString(fpoi, trigger.var1);
  if(res<=0) return res;
  res=fprintf(fpoi," ");
  if(res<=0) return res;
  res=WriteString(fpoi, trigger.var2);
  if(res<=0) return res;
  res=fprintf(fpoi," ");
  if(res<=0) return res;
  res=WriteObject(fpoi, trigger.trobj);
  if(res<=0) return res;
  res=fprintf(fpoi,"TR\n");
  if(res<=0) return res;
  return res;
}

int Cscript::ReadPSTPoint(int store, int *where)
{
  if(SkipChar('[') ) return -1;
  if(ReadNumber(store,where[4]) ) return -1;
  if(SkipChar(',') ) return -1;
  if(ReadNumber(store,where[5]) ) return -1;
  if(SkipChar(']') ) return -1;
  return SkipChar(' ');
}

int Cscript::ReadTrigger(int store, trigger &trigger)
{
  int i;

  if(ReadToken("TR",true))
  {
    if(store==RNB_STORE) return -2; //heh, we expected a trigger here
    return 1;  //end of triggers
  }
  else
  {
    if(ReadNumber(store, trigger.opcode)) return -1;
    if(SkipChar(' ')) return -1;
    for(i=0;i<4;i++)
    {
      if(ReadNumber(store, trigger.bytes[i])) return -1;
      if(SkipChar(' ')) return -1;
    }
    if(pst_compatible_var())
    {
      if(ReadPSTPoint(store, trigger.bytes) ) return -1;
    }
    if(ReadString(store, trigger.var1)) return -1;
    if(SkipChar(' ')) return -1;
    if(ReadString(store, trigger.var2)) return -1;
    if(SkipChar(' ')) return -1;
    if(ReadObject(store, trigger.trobj)) return -1;
  }
  return ReadToken("TR",true); //end of trigger
}

int Cscript::WriteAction(FILE *fpoi, action &action)
{
  int i;
  int res;

  if(action.opcode==-1) return -1;
  res=fprintf(fpoi,"AC\n");
  if(res<=0) return res;
  res=WriteNumber(fpoi,action.opcode);
  if(res<=0) return res;
  for(i=0;i<3;i++)
  {
    res=WriteObject(fpoi, action.obj[i]);
    if(res<=0) return res;
  }
  for(i=0;i<4;i++)
  {
    res=WriteNumber(fpoi, action.bytes[i]);
    if(res<=0) return res;
    res=fprintf(fpoi," ");
    if(res<=0) return res;
  }
  res=WriteNumber(fpoi, action.bytes[i]);
  if(res<=0) return res;
  res=WriteString(fpoi, action.var1);
  if(res<=0) return res;
  res=fprintf(fpoi," ");
  if(res<=0) return res;
  res=WriteString(fpoi, action.var2);
  if(res<=0) return res;
  res=fprintf(fpoi," AC\n");
  if(res<=0) return res;
  return res;
}

int Cscript::ReadAction(int store, action &action)
{
  int i;

  if(store==RNB_STORE) action.opcode=-1;
  if(ReadToken("AC",true)) 
  {
    if(store==RNB_STORE) return -2; //heh, we expected a response here
    return 1;
  }
  if(ReadNumber(store, action.opcode) ) return -1;
  for(i=0;i<3;i++)
  {
    if(ReadObject(store, action.obj[i]) ) return -1;
  }
  for(i=0;i<4;i++)
  {
    if(ReadNumber(store, action.bytes[i]) ) return -1;
    if(SkipChar(' ')) return -1;
  }
  if(ReadNumber(store, action.bytes[i]) ) return -1;
  if(ReadString(store, action.var1) ) return -1;
  if(SkipChar(' ')) return -1;
  if(ReadString(store, action.var2) ) return -1;
  if(SkipChar(' ')) return -1;
  return ReadToken("AC",true); //end of action
}

int Cscript::WriteResponse(FILE *fpoi, response &response)
{
  int i;
  int res;

  if(!response.actions) return -1;
  res=fprintf(fpoi,"RE\n");
  if(res<=0) return res;
  res=WriteNumber(fpoi, response.weight);
  if(res<=0) return res;
  for(i=0;i<response.actioncount;i++)
  {
    res=WriteAction(fpoi,response.actions[i]);
    if(res<=0) return res;
  }
  res=fprintf(fpoi,"RE\n");
  if(res<=0) return res;
  return res;
}

int Cscript::ReadResponse(int store, response &response)
{
  int i;
  int actioncount;
  int res;
  action dummy;
  int cnt_bak;
  int lineno_bak;

  if(store==RNB_STORE) response.weight=-1;
  if(ReadToken("RE",true)) 
  {
    if(store==RNB_STORE) return -2; //heh, we expected a response here
    return 1;
  }
  ReadNumber(store, response.weight);

  StorePosition();  
  actioncount=-1;
  do
  {
    res=ReadAction(RNB_SCAN, dummy);
    if(res<0) return res; //error while reading trigger
    actioncount++;
  }
  while(!res);
  if(actioncount<0) return -1; //0 action

  if(store==RNB_SCAN) return ReadToken("RE",true); //end of condition/response block

  response.actioncount=actioncount;
  response.actions=new action[actioncount];
  if(!response.actions) return -3;

  RestorePosition();  

  for(i=0;i<actioncount;i++)
  {
    res=ReadAction(RNB_STORE, response.actions[i]);
    if(res<0) return res; //error while reading trigger
  }

  return ReadToken("RE",true);
}

int Cscript::WriteBlock(FILE *fpoi, block &block)
{
  int i;
  int res;

  if(!block.triggers) return -1;
  if(!block.responses) return -1;
  res=fprintf(fpoi,"CR\nCO\n");
  if(res<=0) return res;
  for(i=0;i<block.triggercount;i++)
  {
    res=WriteTrigger(fpoi,block.triggers[i]);
    if(res<=0) return res;
  }
  res=fprintf(fpoi,"CO\nRS\n");
  if(res<=0) return res;
  for(i=0;i<block.responsecount;i++)
  {
    res=WriteResponse(fpoi,block.responses[i]);
    if(res<=0) return res;
  }
  res=fprintf(fpoi,"RS\nCR\n");
  if(res<=0) return res;
  return res;
}

int Cscript::ReadBlock(int store, block &block)
{
  int i;
  int triggercount;
  int responsecount;
  int res;
  trigger dummy1;
  response dummy2;
  int cnt_bak;
  int lineno_bak;

  if(ReadToken("CR",true))
  {
    if(store==RNB_STORE) return -2; //heh, we expected a trigger here
    return 1; 
  }
  if(ReadToken("CO",true)) return -1; //we always have a condition block
  StorePosition();  

  triggercount=-1;
  do
  {
    res=ReadTrigger(RNB_SCAN, dummy1);
    if(res<0) return res; //error while reading trigger
    triggercount++;
  }
  while(!res);
  if(triggercount<0) return -1; //0 trigger

  if(ReadToken("CO",true)) return -1; //condition block end

  responsecount=-1;
  if(ReadToken("RS",true)) return -1; //response block start
  do
  {
    res=ReadResponse(RNB_SCAN, dummy2);
    if(res<0) return res; //error while reading trigger
    responsecount++;
  }
  while(!res);
  if(responsecount<0) return -1; // 0 response
  if(ReadToken("RS",true)) return -1; //response block end

  if(store==RNB_SCAN) return ReadToken("CR",true); //end of condition/response block

  //reading a trigger block
  block.triggercount=triggercount;
  block.triggers=new trigger[triggercount];
  if(!block.triggers) return -3;

  block.responsecount=responsecount;
  block.responses=new response[responsecount];
  if(!block.responses) return -3;

  RestorePosition();  

  for(i=0;i<triggercount;i++)
  {
    res=ReadTrigger(RNB_STORE, block.triggers[i]);
    if(res<0) return res; //error while reading trigger
  }
  
  if(ReadToken("CO",true)) return -2; //condition block end
  if(ReadToken("RS",true)) return -2; //condition block end

  for(i=0;i<responsecount;i++)
  {
    res=ReadResponse(RNB_STORE, block.responses[i]);
    if(res<0) return res; //error while reading trigger
  }

  if(ReadToken("RS",true)) return -2; //condition block end

  return ReadToken("CR",true); //end of block
}

int Cscript::WriteScriptToFile(FILE *fpoi)
{
  int i;
  int res;

  if(!blocks) return -1;
  res=fprintf(fpoi,"SC\n");
  if(res<=0) return res;
  for(i=0;i<blockcount;i++)
  {
    res=WriteBlock(fpoi,blocks[i]);
    if(res<=0) return res;
  }
  res=fprintf(fpoi,"SC\n");
  if(res<=0) return res;
  return res;
}

int Cscript::BuildScriptTree()
{
  int i;
  int res;
  block dummy;

//first pass, counting trigger blocks
  RollBack();
  blockcount=-1;
  if(ReadToken("SC",true)) return -1;
  do
  {
    res=ReadBlock(RNB_SCAN,dummy);
    if(res<0) return res; //error while reading trigger
    blockcount++;
  }
  while(!res);
  if(ReadToken("SC",false)) return -1;
//second pass, counting triggers, counting response blocks
  blocks=new block[blockcount];
  if(!blocks) return -3;

  RollBack();
  if(ReadToken("SC",true)) return -2; //we already read this in the previous pass
  for(i=0;i<blockcount;i++)
  {
    res=ReadBlock(RNB_STORE, blocks[i]);
    if(res<0) return res;
  }
  if(ReadToken("SC",false)) return -2;
  return 0;
}

int Cscript::ReadScriptFile(int fhandle, int ml)
{
  char *tmpscrdata;
  int i,j;

  if(blocks) return -2;
  if(scrdata) return -2; //already open
  if(maxlen) return -2; //already open
  if(fhandle<1) return -2;
  if(ml==-1)
  {
    ml=filelength(fhandle);
  }
  if(ml<1) return -4;
  tmpscrdata=new char[ml];
  cnt=0;
  if(tmpscrdata)
  {
    read(fhandle,tmpscrdata,ml);
    j=0;
    for(i=0;i<ml;i++)
    {
      if(tmpscrdata[i]!=13)
      {
        tmpscrdata[j++]=tmpscrdata[i];
      }
    }
    maxlen=j;
    scrdata=new char[maxlen];
    if(scrdata)
    {
      memcpy(scrdata, tmpscrdata, maxlen);
      delete tmpscrdata;
      return BuildScriptTree();
    }
    delete tmpscrdata;
  }
  return -3; //not enough memory
}

int Cscript::CloseScriptFile()
{
  if(!scrdata) return -1;
  delete scrdata;
  scrdata=NULL;
  maxlen=0;
  NukeBlocks();
  return 0;
}

#define AC_CREATEITEM        82
#define AC_GIVEITEMCREATE    140
#define AC_CREATEITEMGLOBAL  256
#define AC_TAKEITEMREPLACE   278 //second

#define AC_STARTSTORE 150

int CREATEITEM_ACTIONS1[]={AC_CREATEITEM,AC_GIVEITEMCREATE,
-1};
int CREATEITEM_ACTIONS2[]={AC_TAKEITEMREPLACE,AC_CREATEITEMGLOBAL,
-1};

int Cscript::get_next_scriptitem(CString &item, int function)
{
  int action;
  int *actionpoi1, *actionpoi2;

  switch(function)
  {
  case 0: //createitem
    actionpoi1=CREATEITEM_ACTIONS1;
    actionpoi2=CREATEITEM_ACTIONS2;
    break;
  default:
    return -1;
  }
  if(!scrdata)
  {
    return -1; //invalid scan object
  }

  for(;bcnt<blockcount;bcnt++)
  {
    for(;rcnt<blocks[bcnt].responsecount;rcnt++)
    {
      for(;acnt<blocks[bcnt].responses[rcnt].actioncount;acnt++)
      {
        action=blocks[bcnt].responses[rcnt].actions[acnt].opcode;
        if(member_array(action,actionpoi1)!=-1)
        {
          item=blocks[bcnt].responses[rcnt].actions[acnt].var1;
          acnt++;
          if(item.GetLength()>8) return 2; //invalid item reference
          return 1;
        }
        if(member_array(action,actionpoi2)!=-1)
        {
          item=blocks[bcnt].responses[rcnt].actions[acnt].var2;
          acnt++;
          if(item.GetLength()>8) return 2; //invalid item reference
          return 1;
        }
      }
    }
  }
  return 0; //not found any more
}

CString Cscript::GetErrLine()
{
  char tmp[130];
  int len;

  if(!scrdata) return "";
  if(lastln>cnt) return "";
  len=sizeof(tmp)-2;
  if(lastln+len>maxlen) len=maxlen-lastln;
  memcpy(tmp,scrdata+lastln, len);
  tmp[len]=0;
  while(len--)
  {
    if(tmp[len]=='\n') tmp[len]=0;
  }
  return tmp;
}

int Cscript::find_itemtype(search_data &searchdata, search_data &finddata, int itemtype)
{
  long item;
  trigger *trigger;
  action *action;

  if(!scrdata)
  {
    return -1; //invalid scan object
  }

  for(;bcnt<blockcount;bcnt++)
  {
    if(itemtype==FLG_MTYPE || itemtype==FLG_MRES || itemtype==FLG_MVAR)
    {
      for(;tcnt<blocks[bcnt].triggercount;tcnt++)
      {
        trigger=&blocks[bcnt].triggers[tcnt];
        switch(itemtype)
        {
        case FLG_MVAR:
          memcpy(finddata.variable,trigger->var1+6,32);
          finddata.param1=bcnt;
          if(searchdata.variable[0])
          {
            if(!strnicmp(finddata.variable,searchdata.variable,32))
            {
              return 1;
            }
          }
          memcpy(finddata.variable,trigger->var2+6,32);
          finddata.param1=bcnt;
          if(searchdata.variable[0])
          {
            if(!strnicmp(finddata.variable,searchdata.variable,32))
            {
              return 1;
            }
          }
          break;
        case FLG_MRES:
          if(strlen(trigger->var1)<=8)
          {
            memcpy(finddata.resource,trigger->var1,8);
            finddata.param1=bcnt;
            if(searchdata.resource[0])
            {
              if(!strnicmp(finddata.resource,searchdata.resource,8))
              {
                return 1;
              }
            }
          }

          if(strlen(trigger->var2)<=8)
          {
            memcpy(finddata.resource,trigger->var2,8);
            finddata.param1=bcnt;
            if(searchdata.resource[0])
            {
              if(!strnicmp(finddata.resource,searchdata.resource,8))
              {
                return 1;
              }
            }
          }
          break;
        case FLG_MTYPE:
          item=trigger->opcode&0x3fff;
          if(item>=searchdata.itemtype && item<=searchdata.itemtype2)
          {
            finddata.itemtype=(short) item;
            finddata.itemtype2=(short) bcnt;
            return 1;
          }
          break;
        }
      }
      tcnt=0;
    }    
    if(itemtype==FLG_MTYPE) continue; //skip responses for trigger

    for(;rcnt<blocks[bcnt].responsecount;rcnt++)
    {
      for(;acnt<blocks[bcnt].responses[rcnt].actioncount;acnt++)
      {
        action=&blocks[bcnt].responses[rcnt].actions[acnt];
        switch(itemtype)
        {
        case FLG_MVAR:
          memcpy(finddata.variable,action->var1+6,32);
          finddata.param1=bcnt;
          if(searchdata.variable[0])
          {
            if(!strnicmp(finddata.variable,searchdata.variable,32))
            {
              return 1;
            }
          }
          memcpy(finddata.variable,action->var2+6,32);
          finddata.param1=bcnt;
          if(searchdata.variable[0])
          {
            if(!strnicmp(finddata.variable,searchdata.variable,32))
            {
              return 1;
            }
          }
          break;
        case FLG_MRES:
          if(strlen(action->var1)<=8)
          {
            memcpy(finddata.resource,action->var1,8);
            finddata.param1=bcnt;
            if(searchdata.resource[0])
            {
              if(!strnicmp(finddata.resource,searchdata.resource,8))
              {
                return 1;
              }
            }
          }

          if(strlen(action->var2)<=8)
          {
            memcpy(finddata.resource,action->var2,8);
            finddata.param1=bcnt;
            if(searchdata.resource[0])
            {
              if(!strnicmp(finddata.resource,searchdata.resource,8))
              {
                return 1;
              }
            }
          }
          break;
        case FLG_MFEAT:
          item=action->opcode;
          if(item>=searchdata.feature && item<=searchdata.feature2)
          {
            finddata.feature=(short) item;
            finddata.feature2=(short) bcnt;
            return 1;
          }
          break;
        }
      }
      acnt=0;
    }
    rcnt=0;
  }
  bcnt=0;
  return 0;
}
