// item.cpp: implementation of the Citem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <fcntl.h>

#include "chitem.h"
#include "item.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Citem::Citem()
{
  extheaders=NULL;
  featblocks=NULL;
  extheadcount=0;
  featblkcount=0;
  revision=1;
}

Citem::~Citem()
{
  KillExtHeaders();
  KillFeatHeaders();
}

int Citem::WriteItemToFile(int fhandle, int calculate)
{
  int fbc;
  int i;

  header.extheadoffs=sizeof(item_header);
  switch(revision)
  {
  case 20:
    header.extheadoffs+=sizeof(iwd2_header);
    memcpy(header.filetype,"ITM V2.0",8); //iwd2
    break;
  case 11:
    header.extheadoffs+=sizeof(pst_header);
    memcpy(header.filetype,"ITM V1.1",8); //pst
    break;
  default:
    memcpy(header.filetype,"ITM V1  ",8); //bg1, bg2
    break;
  }
  //as a rule, we use the internal counter (header.extheadcount instead of extheadcount)
  //actually, the reason is that the internal counter is stored in the file.
  //thus the item is more consistent
  header.featureoffs=header.extheadoffs+header.extheadcount*sizeof(ext_header);
  fbc=header.featblkcount;

  if(calculate)
  {
    for(i=0;i<extheadcount;i++)
    {
      fbc+=extheaders[i].fbcount;
    }
    return header.featureoffs+fbc*sizeof(feat_block);
  }
  if(fhandle<1) return -1;

  if(write(fhandle,&header,sizeof(item_header) )!=sizeof(item_header) )
  {
    return -2;
  }
  if(revision==20)
  {
    if(write(fhandle,&iwd2header,sizeof(iwd2_header) )!=sizeof(iwd2_header) )
    {
      return -2;
    }
  }
  else if(revision==11)
  {
    if(write(fhandle,&pstheader,sizeof(pst_header) )!=sizeof(pst_header) )
    {
      return -2;
    }
  }
  for(i=0;i<extheadcount;i++)
  {
    extheaders[i].fboffs=(unsigned short) fbc;
    if(write(fhandle, &extheaders[i], sizeof(ext_header) )!=sizeof(ext_header) )
    {
      return -2;
    }
    fbc+=extheaders[i].fbcount;
  }
  if(fbc!=featblkcount)
  {
    return 1;
  }
  fbc*=sizeof(feat_block);

  if(write(fhandle, featblocks, fbc )!=fbc )
  {
    return -2;
  }
  m_changed=false;
  return 0;
}

int Citem::adjust_actpoint(long offset)
{
  if(actpoint()==offset) return 0;
  if(maxlen<offset) return -2;
  if(myseek(offset)!=offset) return -2;
  return 1;
}

CString Citem::RetrieveResourceRef(int fh)
{
  CString tmpstr;
  int required;

  if(fh<1) return "";
  required=(BYTE *) &header.invicon-(BYTE *) &header+8;
  if(read(fh, &header, required)!=required)
  {
    close(fh);
    return "";
  }
  close(fh);
  RetrieveResref(tmpstr,header.invicon);
  return tmpstr;
}

long Citem::RetrieveNameRef(int fh)
{
  int required;

  if(fh<1) return -1;
  required=(BYTE *) &header.idref-(BYTE *) &header+sizeof(long);
  if(read(fh, &header, required)!=required)
  {
    close(fh);
    return -1;
  }
  close(fh);
  if(header.idref>0) return header.idref;
  return header.unidref;
}

int Citem::RetrieveItemType(int fh)
{
  int required;

  if(fh<1) return -1;
  required=(BYTE *) &header.itemtype-(BYTE *) &header+sizeof(long);
  if(read(fh, &header, required)!=required)
  {
    close(fh);
    return -1;
  }
  close(fh);
  return header.itemtype;
}

int Citem::ReadItemFromFile(int fh, long ml)
{
  int fullsize;
  int fbc;
  int i;
  int ret;

  m_changed=false;
  if(fh<1) return -1;
  fhandle=fh; //for safe string reads
  if(ml==-1) maxlen=filelength(fhandle);
  else maxlen=ml;
  if(maxlen<1) return -1; //short file, invalid item
  startpoint=tell(fhandle);
  if(read(fhandle,&header,sizeof(item_header) )!=sizeof(item_header) )
  {	
  	return -2; // short file, invalid item
  }
  if(memcmp(header.filetype,"ITM V",5) )
  {
	  return -2;
  }
  if(header.revision[1]=='2')
  {
    if(read(fhandle,&iwd2header,sizeof(iwd2_header) )!=sizeof(iwd2_header) )
    {	
  	  return -2; // short file, invalid item
    }
    fullsize=sizeof(header)+sizeof(iwd2header);
    revision=20;
  }
  else
  {
    switch(header.revision[3])
    {
    case '1':
      if(read(fhandle,&pstheader,sizeof(pst_header) )!=sizeof(pst_header) )
      {	
        return -2; // short file, invalid item
      }
      fullsize=sizeof(header)+sizeof(pstheader);
      revision=11; //pst
      break;
    default:
      revision=1;  //bg1, bg2
      fullsize=sizeof(header);
    }
  }
  if(revision!=11)
  {
    //filling these pst unknowns (if the item isn't from PST), making conversion easier
    pstheader.nameref=header.unidref;
    StoreResref(itemname,pstheader.dialog); 
  }
  ret = 0;
  if(header.extheadoffs!=fullsize )
  {
    //this is a small hack to fix some tutu screwups
    if ((header.featureoffs==fullsize) && !header.extheadcount)
    {
      header.extheadoffs=fullsize;
    }
    ret = 1;
  }
  if(header.extheadcount!=extheadcount)
  {
    KillExtHeaders();
    extheaders=new ext_header[header.extheadcount];
    if(!extheaders) return -3;
    extheadcount=header.extheadcount;
  }
  fbc=header.featblkcount;
  if(fbc || header.extheadcount)
  {
    if(header.featureoffs!=(int) (header.extheadoffs+extheadcount*sizeof(ext_header)) )
    {
      ret = 2;
    }
    if(header.featblkoffs)
    {
      ret = 2;
    }
  }
  else
  { //item isn't seriously crippled
    if(header.featureoffs!=(int) (header.extheadoffs+extheadcount*sizeof(ext_header)) )
    {
      ret = 1;
    }
    if(header.featblkoffs)
    {
      ret = 1;
    }
  }
  for(i=0;i<extheadcount;i++)
  {
    if(read(fhandle, &extheaders[i], sizeof(ext_header) )!=sizeof(ext_header) )
    {
      return -2;
    }
    fbc+=extheaders[i].fbcount;
    fullsize+=sizeof(ext_header);
  }
  if(fbc!=featblkcount)
  {
    KillFeatHeaders();
    featblocks=new feat_block[fbc];
    if(!featblocks) return -3;
    featblkcount=fbc;
  }
  if (fullsize<header.featblkoffs)
  {
    lseek(fhandle, header.featblkoffs-fullsize, SEEK_CUR);
    ret = 1;
  }
  for(i=0;i<featblkcount;i++)
  {
    if(read(fhandle, &featblocks[i], sizeof(feat_block) )!=sizeof(feat_block) )
    {
      return -2;
    }
    fullsize+=sizeof(feat_block);
  }
  if(maxlen!=fullsize)
  {
    ret = 1; //incorrect length
  }
  return ret;
}
