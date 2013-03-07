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
  extlinks=NULL;
  statecount=0;
  transcount=0;
  sttriggercount=0;
  trtriggercount=0;
  actioncount=0;
  extlinkcnt=0;
  changed=0;
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
  KillExternalLinks();
}

int Cdialog::WriteDialogToFile(int fh, int calculate)
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
  if(calculate)
  {
    for(i=0;i<header.numsttrigger;i++)
    {
      fullsize+=sttriggers[i].GetLength();
    }
    for(i=0;i<header.numtrtrigger;i++)
    {
      fullsize+=trtriggers[i].GetLength();
    }
    for(i=0;i<header.numaction;i++)
    {
      fullsize+=actions[i].GetLength();
    }
    return fullsize;
  }

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

CString Cdialog::GetPartName(int stateindex)
{
  CString ret = itemname;
  int i;

  for(i=0;i<extlinkcnt;i++)
  {
    if (stateindex<extlinks[i].firststate) return ret;
    RetrieveResref(ret, extlinks[i].dlgname);
  }
  return ret;
}

int Cdialog::AddExternals()
{
  loc_entry fileloc;
  CString key;
  Cdialog ext;
  dlg_trans extlink;
  int fhandle;
  int ret;

  while(GetExternalLink(extlink))
  {
    Cdialog ext;
    
    RetrieveResref(key, extlink.nextdlg);
    if(dialogs.Lookup(key,fileloc))
    {
      fhandle=locate_file(fileloc, 0);
      if(fhandle<1)
      {
        MessageBox(0,"Invalid file:"+key,"Error", MB_ICONSTOP|MB_OK);
        return -2;
      }
      ret = ext.ReadDialogFromFile(fhandle, fileloc.size, key);
      close(fhandle);
      if (ret<0)
      {
        MessageBox(0,"Invalid file:"+key,"Error", MB_ICONSTOP|MB_OK);
        return ret;
      }
      ret = MergeDialog(ext, key);
      if (ret)
      {
        MessageBox(0,"Invalid file:"+key,"Error", MB_ICONSTOP|MB_OK);
        return ret;
      }
    }
  }
  return 0;
}

int Cdialog::ResolveLinks(dlg_extlink *baselinks, int baselinkcount, CString basename)
{
  CString tmpstr, loaded;
  int i, j;

  int firststate = baselinks[baselinkcount].firststate;
  int firsttrans = baselinks[baselinkcount].firsttrans;
  int firststtrigger = baselinks[baselinkcount].firststtrigger;
  int firsttrtrigger = baselinks[baselinkcount].firsttrtrigger;
  int firstaction = baselinks[baselinkcount].firstaction;

  for(i=0;i<header.numstates;i++)
  {
    if (dlgstates[i].stindex>=0)
    {
      dlgstates[i].stindex+=firststtrigger;
    }
    dlgstates[i].trindex+=firsttrans;
  }
  for(i=0;i<header.numtrans;i++)
  {
    if (dlgtrans[i].actionindex>=0)
    {
      dlgtrans[i].actionindex += firstaction;
    }
    if (dlgtrans[i].trtrindex>=0)
    {
      dlgtrans[i].trtrindex += firsttrtrigger;
    }
    if (dlgtrans[i].nextdlg[0]=='*')
    {
      if (dlgtrans[i].stateindex>=statecount)
      {
        //probably already checked
        return -2;
      }
      dlgtrans[i].stateindex += firststate;
    }
    else
    {
      RetrieveResref(tmpstr, dlgtrans[i].nextdlg);
      if (tmpstr==basename)
      {
        StoreResref(SELF_REFERENCE,dlgtrans[i].nextdlg);
        if(dlgtrans[i].stateindex>=baselinks[0].firststate)
        {
          return -2;
        }
        //state is matching because this is the base dialog
      }
      else
      {
        //resolve already loaded links
        for(j=0;j<baselinkcount;j++)
        {
          RetrieveResref(loaded, baselinks[j].dlgname);
          if(tmpstr == loaded)
          {
            StoreResref(SELF_REFERENCE,dlgtrans[i].nextdlg);
            if (dlgtrans[i].stateindex>=baselinks[j].numstates)
            {
              return -2;
            }
            dlgtrans[i].stateindex += baselinks[j].firststate;
            break;
          }
        }
      }
    }
  }
  return 0;
}

int Cdialog::MergeDialog(Cdialog &add, CString key)
{
  CString tmpstr;
  int ret;
  int i;

  dlg_extlink *newextlink = new dlg_extlink[extlinkcnt+1];
  if (!newextlink) return -3;
  dlg_state *newstates = new dlg_state[statecount+add.statecount];
  if (!newstates)
  {
    delete [] newextlink;
    return -3;
  }
  dlg_trans *newtrans = new dlg_trans[transcount+add.transcount];
  if (!newtrans)
  {
    delete [] newextlink;
    delete [] newstates;
    return -3;
  }
  CString *newtrtriggers = new CString[trtriggercount+add.trtriggercount];
  if (!newtrtriggers)
  {
    delete [] newextlink;
    delete [] newstates;
    delete [] newtrans;
    return -3;
  }
  CString *newsttriggers = new CString[sttriggercount+add.sttriggercount];
  if (!newsttriggers)
  {
    delete [] newextlink;
    delete [] newstates;
    delete [] newtrans;
    delete [] newtrtriggers;
    return -3;
  }
  CString *newactions = new CString[actioncount+add.actioncount];
  if (!newactions)
  {
    delete [] newextlink;
    delete [] newstates;
    delete [] newtrans;
    delete [] newtrtriggers;
    delete [] newsttriggers;
    return -3;
  }

  memcpy(newextlink, extlinks, extlinkcnt*sizeof(dlg_extlink));
  StoreResref(key, newextlink[extlinkcnt].dlgname);
  newextlink[extlinkcnt].firststate = header.numstates;
  newextlink[extlinkcnt].firsttrans = header.numtrans;
  newextlink[extlinkcnt].firststtrigger = header.numsttrigger;
  newextlink[extlinkcnt].firsttrtrigger = header.numtrtrigger;
  newextlink[extlinkcnt].firstaction = header.numaction;
  newextlink[extlinkcnt].numstates = add.header.numstates;
  newextlink[extlinkcnt].numtrans = add.header.numtrans;
  newextlink[extlinkcnt].numsttrigger = add.header.numsttrigger;
  newextlink[extlinkcnt].numtrtrigger = add.header.numtrtrigger;
  newextlink[extlinkcnt].numaction = add.header.numaction;
  delete[] extlinks;
  extlinks = newextlink;
  //don't increase extlinkcnt, because the last one points to the currently added dialog
  ret = add.ResolveLinks(extlinks, extlinkcnt, itemname);
  extlinkcnt++;

  //
  memcpy(newstates, dlgstates, statecount*sizeof(dlg_state));
  memcpy(newstates+statecount, add.dlgstates, add.statecount*sizeof(dlg_state));
  delete [] dlgstates;
  dlgstates = newstates;

  //
  memcpy(newtrans, dlgtrans, transcount*sizeof(dlg_trans));
  //resolve references to the merged dialog
  for(i=0;i<transcount;i++)
  {
    RetrieveResref(tmpstr,newtrans[i].nextdlg);
    if(tmpstr==key)
    {
      newtrans[i].stateindex+=statecount;
      StoreResref(SELF_REFERENCE,newtrans[i].nextdlg);
    }
  }
  memcpy(newtrans+transcount, add.dlgtrans, add.transcount*sizeof(dlg_trans));
  delete [] dlgtrans;
  dlgtrans = newtrans;
  //
  for(i=0;i<trtriggercount;i++)
  {
    newtrtriggers[i]=trtriggers[i];
  }
  //memcpy(newtrtriggers, trtriggers, trtriggercount*sizeof(CString *));
  for(i=0;i<add.trtriggercount;i++)
  {
    newtrtriggers[trtriggercount+i]=add.trtriggers[i];
  }
  //memcpy(newtrtriggers+trtriggercount, add.trtriggers, add.trtriggercount*sizeof(CString *));
  delete [] trtriggers;
  trtriggers = newtrtriggers;
  //
  for(i=0;i<sttriggercount;i++)
  {
    newsttriggers[i]=sttriggers[i];
  }
  //memcpy(newsttriggers, sttriggers, sttriggercount*sizeof(CString *));
  for(i=0;i<add.sttriggercount;i++)
  {
    newsttriggers[sttriggercount+i]=add.sttriggers[i];
  }
  //memcpy(newsttriggers+sttriggercount, add.sttriggers, add.sttriggercount*sizeof(CString *));
  delete [] sttriggers;
  sttriggers = newsttriggers;
  //
  for(i=0;i<actioncount;i++)
  {
    newactions[i]=actions[i];
  }
  //memcpy(newactions, actions, actioncount*sizeof(CString *));
  for(i=0;i<add.actioncount;i++)
  {
    newactions[actioncount+i]=add.actions[i];
  }
  //memcpy(newactions+actioncount, add.actions, add.actioncount*sizeof(CString *));
  delete [] actions;
  actions = newactions;
  //
  statecount = header.numstates+=add.header.numstates;
  transcount = header.numtrans+=add.header.numtrans;
  sttriggercount = header.numsttrigger+=add.header.numsttrigger;
  trtriggercount = header.numtrtrigger+=add.header.numtrtrigger;
  actioncount = header.numaction+=add.header.numaction;
  return ret;
}

bool Cdialog::GetExternalLink(dlg_trans &extlink)
{
  CString tmp;
  int i;

  for(i=0;i<transcount;i++)
  {
    //self reference
    if(dlgtrans[i].nextdlg[0] && dlgtrans[i].nextdlg[0]!='*')
    {
      memcpy(&extlink, dlgtrans+i, sizeof(dlg_trans));
      return true;
    }
  }
  return false;
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

int Cdialog::ReadDialogFromFile(int fh, long ml, CString self)
{
  CString tmpstr;
  long fullsize;
  int i;
  offset_length oldata;
  unsigned long tmptext;
  char *poi;
  long oldpos;
  int ret, gret;

  KillExternalLinks();
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
    if(tmpstr==self) StoreResref(SELF_REFERENCE,dlgtrans[i].nextdlg);
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

int Cdialog::findtrigger(int sttrigger)
{
  int i;

  for(i=0;i<statecount;i++)
  {
    if(sttrigger==dlgstates[i].stindex) return i;
  }
  return -1;
}
