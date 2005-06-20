// chui.cpp: implementation of the Cchui class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "chitem.h"
#include "chui.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Cchui::Cchui()
{
  fhandle=-1;
  startpoint=-1;
  maxlen=-1;
  windows=NULL;
  controltable=NULL;
  controls=NULL;
  extensions=NULL;
  revision=10;
  memset(&header,0,sizeof(header));
}

Cchui::~Cchui()
{
  KillWindows();
  KillControls();
}

int Cchui::adjust_actpoint(long offset)
{
  if(actpoint()==offset) return 0;
  if(maxlen<offset) return -2;
  if(myseek(offset)!=offset) return -2;
  return 1;
}

int Cchui::WriteChuiToFile(int fh, int calculate)
{
  int esize;
  int i;

  memcpy(header.filetype,"CHUIV1  ",8);
  fullsize=sizeof(header);
  header.windowoffs=fullsize;
  fullsize+=header.windowcnt*sizeof(chui_window);
  header.controloffs=fullsize;
  fullsize+=controlcnt*sizeof(POINT);
  for(i=0;i<controlcnt;i++)
  {
    controltable[i].x=fullsize; //recalculates control offsets
    fullsize+=controltable[i].y;
  }
  if(calculate) return fullsize;
  if(fh<1) return -1;

  if(write(fh,&header,sizeof(header))!=sizeof(header))
  {
    return -2;
  }
  esize=header.windowcnt*sizeof(chui_window);
  if(write(fh,windows,esize)!=esize)
  {
    return -2;
  }
  esize=controlcnt*sizeof(POINT);
  if(write(fh,controltable,esize)!=esize)
  {
    return -2;
  }
  for(i=0;i<controlcnt;i++)
  {
    if(write(fh,controls+i,sizeof(chui_control_common))!=sizeof(chui_control_common))
    {
      return -2;
    }
    esize=ChuiControlSize(controls[i].controltype);
    if(esize==-1) return -1;
    if(write(fh,extensions[i],esize)!=esize)
    {
      return -2;
    }
  }
  return 0;
}

int Cchui::ReadChuiFromFile(int fh, long ml)
{
  int flg, ret;
  int esize, minsize;
  int i;
  int ccnt;

  if(fh<1) return -1;
  fhandle=fh; //for safe string reads
  if(ml==-1) maxlen=filelength(fhandle);
  else maxlen=ml;
  if(maxlen<1) return -1; //short file, invalid item
  startpoint=tell(fhandle);
  fullsize=sizeof(header);
  if(read(fhandle,&header, fullsize)!=fullsize )
  {	
  	return -2; // short file, invalid item
  }
  if(memcmp(header.filetype,"CHUIV1  ",8) )
  {
    return -2;
  }

  //chui windows
  flg=adjust_actpoint(header.windowoffs);
  if(flg<0) return flg;
  ret=flg;
  if(header.windowcnt!=windowcnt)
  {
    KillWindows();
    windows=new chui_window[header.windowcnt];
    if(!windows) return -3;
    windowcnt=header.windowcnt;
  }
  esize=windowcnt*sizeof(chui_window);
  if(read(fhandle,windows,esize)!=esize)
  {
    return -2;
  }
  fullsize+=esize;

  //pre calculating the size of the control table
  ccnt=0;
  for(i=0;i<header.windowcnt;i++)
  {
    esize=windows[i].controlidx+windows[i].controlcount;
    if(esize>ccnt) ccnt=esize;
  }

  flg=adjust_actpoint(header.controloffs);
  if(flg<0) return flg;
  ret=flg;

  //remove controls always, it doesn't worth to bother with
  KillControls();
  controltable=new POINT[ccnt];
  if(!controltable) return -3;
  controls=new chui_control_common[ccnt];
  if(!controls) {
    return -3;
  }
  extensions=new BYTE*[ccnt];
  if(!extensions)
  {
    return -3;
  }
  memset(extensions,0,sizeof(BYTE *)*ccnt);
  memset(controltable,0,sizeof(POINT)*ccnt);
  memset(controls,-1,sizeof(chui_control_common)*ccnt);//precaution
  controlcnt=ccnt;

  esize=sizeof(POINT)*ccnt;
  if(read(fhandle,controltable,esize)!=esize)
  {
    return -2;
  }
  fullsize+=esize;

  for(i=0;i<ccnt;i++)
  {
    flg=adjust_actpoint(controltable[i].x);
    if(flg<0) return flg;
    ret|=flg;

    esize=sizeof(chui_control_common);
    if(read(fhandle,controls+i,esize)!=esize)      
    {
      return -2;
    }
    fullsize+=esize;
    
    esize=controltable[i].y-sizeof(chui_control_common);
    minsize=ChuiControlSize(controls[i].controltype);
    if(esize!=minsize)
    {//unknown control or bad length stored
      controltable[i].y=minsize+sizeof(chui_control_common);
      ret|=2;
    }
    
    minsize=max(esize,minsize);
    extensions[i]=new BYTE[minsize];
    if(!extensions[i])
    {
      return -3;
    }

    if(read(fhandle,extensions[i],esize)!=esize)
    {
      return -2;
    }

    fullsize+=esize;
  }

  if(maxlen!=fullsize)
  {
    return -4; //incorrect length
  }
  return ret;
}

CString Cchui::RetrieveMosRef(int fh)
{
  CString tmpstr;
  int flg;
  int required;
  chui_window tmpwin;

  if(fh<1) return "";
  fhandle=fh;
  startpoint=tell(fhandle);
  required=sizeof(header);
  if(read(fh,&header, required)!=required )
  {	
    close(fh);
  	return ""; // short file, invalid item
  }
  if(memcmp(header.filetype,"CHUIV1  ",8) )
  {
    close(fh);
    return "";
  }
  if(header.windowcnt<1)
  {
    close(fh);
    return "";
  }
  flg=adjust_actpoint(header.windowoffs);
  if(flg<0)
  {
    close(fh);
    return "";
  }
  if(read(fh, &tmpwin, sizeof(tmpwin))!=sizeof(tmpwin))
  {
    close(fh);
    return "";
  }
  close(fh);
  RetrieveResref(tmpstr,tmpwin.mos);
  return tmpstr;
}
