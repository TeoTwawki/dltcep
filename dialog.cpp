// dialog.cpp: implementation of the Cdialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <fcntl.h>

#include "chitem.h"
#include "dialog.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Cdialog::Cdialog()
{
  dlgstates=NULL;
  dlgtrans=NULL;
  sttriggers=NULL;
  trtriggers=NULL;
  actions=NULL;
  statecount=0;
  transcount=0;
  sttriggercount=0;
  trtriggercount=0;
  actioncount=0;
}

Cdialog::~Cdialog()
{
  new_dialog();
}

void Cdialog::new_dialog()
{
  KillStates();
  KillTrans();
  KillStateTriggers();
  KillTransTriggers();
  KillActions();
}

int Cdialog::WriteDialogToFile(int fh)
{
  int fullsize;
  int esize;
  int oldpos;
  offset_length *tmpoffs;
  int i;
 
  changed=0;
  fullsize=sizeof(dlg_header);
  if(header.offstates==0x30) fullsize-=4;
  header.offstates=fullsize;
  oldpos=fullsize;
  //we use the internal counters (see item.cpp for reasons)
  fullsize+=sizeof(dlg_state)*header.numstates;
  header.offtrans=fullsize;
  fullsize+=sizeof(dlg_trans)*header.numtrans;
  header.offsttrigger=fullsize;
  fullsize+=sizeof(offset_length)*header.numsttrigger;
  header.offtrtrigger=fullsize;
  fullsize+=sizeof(offset_length)*header.numtrtrigger;
  header.offaction=fullsize;
  fullsize+=sizeof(offset_length)*header.numaction;

  memcpy(&header,"DLG V1.0",8);
  if(write(fh,&header,oldpos )!=oldpos)
  {
    return -2;
  }
  oldpos=fullsize; //this holds now the offset of the strings

  esize=sizeof(dlg_state)*header.numstates;
  if(write(fh,dlgstates,esize)!=esize)
  {
    return -2;
  }

  esize=sizeof(dlg_trans)*header.numtrans;
  for(i=0;i<header.numtrans;i++)
  {
    if(!strnicmp(dlgtrans[i].nextdlg,SELF_REFERENCE,8) )
    {
      StoreResref(itemname,dlgtrans[i].nextdlg);
    }
  }
  if(write(fh,dlgtrans,esize)!=esize)
  {
    return -2;
  }
  for(i=0;i<header.numtrans;i++)
  {
    if(!strnicmp(dlgtrans[i].nextdlg,itemname,8) )
    {
      StoreResref(SELF_REFERENCE,dlgtrans[i].nextdlg);
    }
  }

//assembling the state trigger table
  tmpoffs= new offset_length[header.numsttrigger];
  if(!tmpoffs) return -3;
  for(i=0;i<header.numsttrigger;i++)
  {
    tmpoffs[i].textoffset=oldpos;
    tmpoffs[i].textlength=sttriggers[i].GetLength();
    oldpos+=tmpoffs[i].textlength;
  }
  esize=sizeof(offset_length)*header.numsttrigger;
  if(write(fh,tmpoffs,esize)!=esize)
  {
    delete tmpoffs;
    return -2;
  }
  delete tmpoffs;

//assembling the transition trigger table
  tmpoffs= new offset_length[header.numtrtrigger];
  if(!tmpoffs) return -3;
  for(i=0;i<header.numtrtrigger;i++)
  {
    tmpoffs[i].textoffset=oldpos;
    tmpoffs[i].textlength=trtriggers[i].GetLength();
    oldpos+=tmpoffs[i].textlength;
  }
  esize=sizeof(offset_length)*header.numtrtrigger;
  if(write(fh,tmpoffs,esize)!=esize)
  {
    delete tmpoffs;
    return -2;
  }
  delete tmpoffs;

//assembling the action table
  tmpoffs= new offset_length[header.numaction];
  if(!tmpoffs) return -3;
  for(i=0;i<header.numaction;i++)
  {
    tmpoffs[i].textoffset=oldpos;
    tmpoffs[i].textlength=actions[i].GetLength();
    oldpos+=tmpoffs[i].textlength;
  }
  esize=sizeof(offset_length)*header.numaction;
  if(write(fh,tmpoffs,esize)!=esize)
  {
    delete tmpoffs;
    return -2;
  }
  delete tmpoffs;

  for(i=0;i<header.numsttrigger;i++)
  {
    esize=sttriggers[i].GetLength();
    if(write(fh,sttriggers[i],esize )!=esize )
    {
      return -2;
    }
  }

  for(i=0;i<header.numtrtrigger;i++)
  {
    esize=trtriggers[i].GetLength();
    if(write(fh,trtriggers[i], esize)!=esize )
    {
      return -2;
    }
  }

  for(i=0;i<header.numaction;i++)
  {
    esize=actions[i].GetLength();
    if(write(fh,actions[i],esize)!=esize )
    {
      return -2;
    }
  }
  return 0;
}

int Cdialog::adjust_actpoint(long offset)
{
  if(actpoint()==offset) return 0;
  if(maxlen<offset) return -2;
  if(myseek(offset)!=offset) return -2;
  return 1;
}

int Cdialog::CheckExternalLink(int fh, int stateidx)
{
  dlg_header tmpheader;

  if(read(fh,&tmpheader,sizeof(dlg_header) )!=sizeof(dlg_header) )
  {
    return -2;
  }
  return tmpheader.numstates<=stateidx;
}

int Cdialog::ReadDialogFromFile(int fh, long ml)
{
  CString tmpstr;
  long fullsize;
  int i;
  offset_length oldata;
  unsigned long tmptext;
  char *poi;
  long oldpos;
  int ret, gret;

  changed=0;
  fhandle=fh; //for safe string reads
  if(ml==-1) maxlen=filelength(fhandle);
  else maxlen=ml;
  if(maxlen<1) return -2; //short file, invalid item
  startpoint=tell(fhandle);
  if(read(fhandle,&header,sizeof(dlg_header) )!=sizeof(dlg_header) )
  {
    return -2;
  }
  if(memcmp(header.filetype,"DLG ",4) )
  {
	  return -2;
  }
  if(header.revision[0]!='V')
  {
	  return -2;
  }
  fullsize=sizeof(header);
  if(header.offstates==0x30) //hack for old version dialogs
  {
    header.flags=0;          //removing crap from freeze flag
    fullsize-=4;
    lseek(fhandle,-4,SEEK_CUR);
    gret=1;
  }
  else gret=0;

  //states
  if(header.offstates!=fullsize)
  {
    return -2;
  }
  if(header.numstates!=statecount)
  {
    KillStates();
    dlgstates=new dlg_state[header.numstates];
    if(!dlgstates) return -3;
    statecount=header.numstates;
  }
  if(read(fhandle,dlgstates,sizeof(dlg_state)*statecount)!=(long) (statecount*sizeof(dlg_state)))
  {
    return -2;
  }
  fullsize+=sizeof(dlg_state)*statecount;

  //trans
  if(header.offtrans!=fullsize)
  {
    return -2;
  }
  if(header.numtrans!=transcount)
  {
    KillTrans();
    dlgtrans=new dlg_trans[header.numtrans];
    if(!dlgtrans) return -3;
    transcount=header.numtrans;
  }
  if(read(fhandle,dlgtrans,sizeof(dlg_trans)*transcount)!=(long) (transcount*sizeof(dlg_trans)))
  {
    return -2;
  }
  for(i=0;i<transcount;i++)
  {
    RetrieveResref(tmpstr,dlgtrans[i].nextdlg);
    if(tmpstr==itemname) StoreResref(SELF_REFERENCE,dlgtrans[i].nextdlg);
  }
  fullsize+=sizeof(dlg_trans)*transcount;
  
  //state triggers
  if(header.offsttrigger!=fullsize)
  {
    return -2;
  }
  if(header.numsttrigger!=sttriggercount)
  {
    KillStateTriggers();
    sttriggers=new CString[header.numsttrigger];
    if(!sttriggers) return -3;
    sttriggercount=header.numsttrigger;
  }
  tmptext=0;
  for(i=0;i<sttriggercount;i++)
  {
    if(read(fhandle, &oldata, sizeof(offset_length) )!=sizeof(offset_length) )
    {
      return -2;
    }
    oldpos=tell(fhandle);
    ret=adjust_actpoint(oldata.textoffset);
    if(ret<0) return ret;
    poi=sttriggers[i].GetBufferSetLength(oldata.textlength);
    if(read(fhandle, poi, oldata.textlength)!=oldata.textlength)
    {
      return -2;
    }
    lseek(fhandle,oldpos,SEEK_SET);
    sttriggers[i]=consolidate(poi, oldata.textlength);
//    sttriggers[i].ReleaseBuffer(-1);
    tmptext+=oldata.textlength;
  }
  fullsize+=sizeof(offset_length)*sttriggercount;

  //transition triggers
  if(header.offtrtrigger!=fullsize)
  {
    return -2;
  }
  if(header.numtrtrigger!=trtriggercount)
  {
    KillTransTriggers();
    trtriggers=new CString[header.numtrtrigger];
    if(!trtriggers) return -3;
    trtriggercount=header.numtrtrigger;
  }

  for(i=0;i<trtriggercount;i++)
  {
    if(read(fhandle, &oldata, sizeof(offset_length) )!=sizeof(offset_length) )
    {
      return -2;
    }
    oldpos=tell(fhandle);
    ret=adjust_actpoint(oldata.textoffset);
    if(ret<0) return ret;
    poi=trtriggers[i].GetBufferSetLength(oldata.textlength);
    if(read(fhandle, poi, oldata.textlength)!=oldata.textlength)
    {
      return -2;
    }
    lseek(fhandle,oldpos,SEEK_SET);
    trtriggers[i]=consolidate(poi,oldata.textlength);
//    trtriggers[i].ReleaseBuffer(-1);
    tmptext+=oldata.textlength;
  }
  fullsize+=sizeof(offset_length)*trtriggercount;

  //actions
  if(header.offaction!=fullsize)
  {
    return -2;
  }
  if(header.numaction!=actioncount)
  {
    KillActions();
    actions=new CString[header.numaction];
    if(!actions) return -3;
    actioncount=header.numaction;
  }
  for(i=0;i<actioncount;i++)
  {
    if(read(fhandle, &oldata, sizeof(offset_length) )!=sizeof(offset_length) )
    {
      return -2;
    }
    oldpos=tell(fhandle);
    ret=adjust_actpoint(oldata.textoffset);
    if(ret<0) return ret;
    poi=actions[i].GetBufferSetLength(oldata.textlength);
    if(read(fhandle, poi, oldata.textlength)!=oldata.textlength)
    {
      return -2;
    }
    lseek(fhandle,oldpos,SEEK_SET);
    actions[i]=consolidate(poi, oldata.textlength);
//    actions[i].ReleaseBuffer(-1);
    tmptext+=oldata.textlength; //add up the original length
  }
  fullsize+=sizeof(offset_length)*actioncount;
  fullsize+=tmptext;
  if(fullsize!=maxlen)
  {
    return -1;
  }
  return gret;
}
