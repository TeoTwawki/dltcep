// map.cpp: implementation of the Cmap class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "chitem.h"
#include "map.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Cmap::Cmap()
{
  fhandle=-1;
  startpoint=-1;
  maxlen=-1;

  headers=NULL;
  areas=NULL;
  arealinks=NULL;
  areacount=NULL;
  linkcount=NULL;
  
  mapcount=0;
}

Cmap::~Cmap()
{
  KillMaps();
  KillAreas();
  KillAreaLinks();
}

int Cmap::adjust_actpoint(long offset)
{
  if(actpoint()==offset) return 0;
  if(maxlen<offset) return -2;
  if(myseek(offset)!=offset) return -2;
  return 1;
}

int Cmap::WriteMapToFile(int fh, int calculate)
{
  int esize;
  int i;

  fullsize=sizeof(map_mainheader);
  mainheader.wmapoffset=fullsize;
  fullsize+=mainheader.wmapcount*sizeof(map_header);
  for(i=0;i<mainheader.wmapcount;i++)
  {
    headers[i].areaoffset=fullsize;
    fullsize+=headers[i].areacount*sizeof(map_area);
  }
  for(i=0;i<mainheader.wmapcount;i++)
  {
    headers[i].arealinkoffset=fullsize;
    fullsize+=headers[i].arealinkcount*sizeof(map_arealink);
  }
  if(calculate)
  {
    return fullsize;
  }
  if(fh<1) return -1;
  memcpy(mainheader.filetype,"WMAPV1.0",8);
  if(write(fh,&mainheader,sizeof(map_mainheader) )!=sizeof(map_mainheader) )
  {
    return -2;
  }
  esize=mainheader.wmapcount*sizeof(map_header);
  if(write(fh,headers,esize)!=esize)
  {
    return -2;
  }
  for(i=0;i<mainheader.wmapcount;i++)
  {
    esize=headers[i].areacount*sizeof(map_area);
    if(write(fh,areas[i],esize)!=esize)
    {
      return -2;
    }
  }
  for(i=0;i<mainheader.wmapcount;i++)
  {
    esize=headers[i].arealinkcount*sizeof(map_arealink);
    if(write(fh,arealinks[i],esize)!=esize)
    {
      return -2;
    }
  }
  return 0;
}

int Cmap::ReadMapFromFile(int fh, long ml)
{
  int flg, ret;
  int esize, fsize, baseposition, pos;
  int i, j;

  ret=0;
  fhandle=fh; //for safe string reads
  if(ml==-1) maxlen=filelength(fhandle);
  else maxlen=ml;
  if(maxlen<1) return -1; //short file, invalid item
  startpoint=tell(fhandle);
  fullsize=sizeof(mainheader);
  if(read(fhandle,&mainheader,fullsize )!=fullsize )
  {	
  	return -2; // short file
  }
  if(memcmp(mainheader.filetype,"WMAP",4) )
  {
    return -2;
  }
  if(mainheader.revision[0]!='V')
  {
	  return -2;
  }
  if(mainheader.wmapoffset!=fullsize)
  {
    return -2;
  }
  if(mainheader.wmapcount!=mapcount)
  {
    KillMaps();
    headers=new map_header[mainheader.wmapcount];
    if(!headers) return -3;

    mapcount=mainheader.wmapcount;
    areas=new map_area*[mapcount];
    if(!areas) return -3;
    memset(areas,0,mapcount*sizeof(map_area *) );
    areacount=new int[mapcount];    
    if(!areacount) return -3;
    memset(areacount,0,mapcount*sizeof(int) );

    arealinks=new map_arealink*[mapcount];
    if(!arealinks) return -3;
    memset(arealinks,0,mapcount*sizeof(map_arealink *) );
    linkcount=new int[mapcount];
    if(!linkcount) return -3;
    memset(linkcount,0,mapcount*sizeof(int) );
  }
  esize=mainheader.wmapcount*sizeof(map_header);
  if(read(fhandle,headers,esize)!=esize)
  {
    return -2;
  }
  fullsize+=esize;
  for(i=0;i<mapcount;i++)
  {//seeking to the proper area block
    if(headers[i].areacount)
    {
      flg=adjust_actpoint(headers[i].areaoffset);
      if(flg<0) return flg;
      ret|=flg;
    }

    //reading areas
    if(headers[i].areacount!=areacount[i])
    {
      KillAreas(i);
      areas[i]=new map_area[headers[i].areacount];
      if(!areas[i]) return -3;
      areacount[i]=headers[i].areacount;
    }
    esize=headers[i].areacount*sizeof(map_area);
    if(read(fhandle,areas[i],esize)!=esize)
    {
      return -2;
    }
    fullsize+=esize;
  }

  for(i=0;i<mapcount;i++)
  {
    baseposition=headers[i].arealinkoffset;
    esize=0;
    for(j=0;j<headers[i].areacount;j++)
    {
      esize+=areas[i][j].eastcnt+areas[i][j].westcnt+areas[i][j].northcnt+areas[i][j].southcnt;
    }
    if(headers[i].arealinkcount!=esize)
    {
      headers[i].arealinkcount=esize;
      ret|=2;
    }
    if(esize!=linkcount[i])
    {
      KillAreaLinks(i);
      arealinks[i]=new map_arealink[esize];
      if(!arealinks[i]) return -3;
      linkcount[i]=headers[i].arealinkcount;
    }   
    fullsize+=esize*sizeof(map_arealink);

    //seeking to the link blocks in the proper order, reading them in an ordered way
    //order is north, east, south ,west, though the links are ordered in a
    //north, west, south, east way in the header
    pos=0;
    for(j=0;j<headers[i].areacount;j++)
    {
      fsize=areas[i][j].northidx*sizeof(map_arealink);
      if(areas[i][j].northidx!=pos)
      {
        areas[i][j].northidx=pos;
        ret|=1;
      }
      flg=adjust_actpoint(baseposition+fsize);
      if(flg<0) return flg;
      ret|=flg;
      esize=sizeof(map_arealink)*areas[i][j].northcnt;
      if(read(fhandle,arealinks[i]+pos,esize)!=esize)
      {
        return -2;
      }
      pos+=areas[i][j].northcnt;

      fsize=sizeof(map_arealink)*areas[i][j].eastidx;
      if(areas[i][j].eastidx!=pos)
      {
        areas[i][j].eastidx=pos;
        ret|=1;
      }
      flg=adjust_actpoint(baseposition+fsize);
      if(flg<0) return flg;
      ret|=flg;
      esize=sizeof(map_arealink)*areas[i][j].eastcnt;
      if(read(fhandle,arealinks[i]+pos,esize)!=esize)
      {
        return -2;
      }
      pos+=areas[i][j].eastcnt;

      fsize=sizeof(map_arealink)*areas[i][j].southidx;
      if(areas[i][j].southidx!=pos)
      {
        areas[i][j].southidx=pos;
        ret|=1;
      }
      flg=adjust_actpoint(baseposition+fsize);
      if(flg<0) return flg;
      ret|=flg;
      esize=sizeof(map_arealink)*areas[i][j].southcnt;
      if(read(fhandle,arealinks[i]+pos,esize)!=esize)
      {
        return -2;
      }
      pos+=areas[i][j].southcnt;

      fsize=sizeof(map_arealink)*areas[i][j].westidx;
      if(areas[i][j].westidx!=pos)
      {
        areas[i][j].westidx=pos;
        ret|=1;
      }
      flg=adjust_actpoint(baseposition+fsize);
      if(flg<0) return flg;
      ret|=flg;
      esize=sizeof(map_arealink)*areas[i][j].westcnt;
      if(read(fhandle,arealinks[i]+pos,esize)!=esize)
      {
        return -2;
      }
      pos+=areas[i][j].westcnt;
    }
  }

  if(fullsize!=maxlen) return -1;
  return ret;
}
