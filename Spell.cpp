// spell.cpp: implementation of the Cspell class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <fcntl.h>

#include "chitem.h"
#include "spell.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Cspell::Cspell()
{
  extheaders=NULL;
  featblocks=NULL;
  extheadcount=0;
  featblkcount=0;
  revision=1;
}

Cspell::~Cspell()
{
  KillExtHeaders();
  KillFeatHeaders();
}

int Cspell::WriteSpellToFile(int fhandle, int calculate)
{
  int fbc,i;

  header.extheadoffs=sizeof(spell_header);
  switch(revision)
  {
  case 20:
    header.extheadoffs+=sizeof(iwd2_header);
    memcpy(header.filetype,"SPL V2.0",8);
    break;
  default:
    memcpy(header.filetype,"SPL V1  ",8); //bg1, bg2
    break;
  }
  header.featureoffs=header.extheadoffs+header.extheadcount*sizeof(spl_ext_header);
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

  if(write(fhandle,&header,sizeof(spell_header) )!=sizeof(spell_header) )
  {	
  	return -2; // short file, invalid item
  }
  if(revision==20)
  {
    if(write(fhandle,&iwd2header,sizeof(iwd2_header) )!=sizeof(iwd2_header) )
    {
      return -2;
    }
  }
  for(i=0;i<extheadcount;i++)
  {
    extheaders[i].fboffs=(unsigned short) fbc;
    if(write(fhandle, &extheaders[i], sizeof(spl_ext_header) )!=sizeof(spl_ext_header) )
    {
      return -2;
    }
    fbc+=extheaders[i].fbcount;
  }
  if(fbc!=featblkcount)
  {
    return -2;
  }
  for(i=0;i<featblkcount;i++)
  {
    if(write(fhandle, &featblocks[i], sizeof(feat_block) )!=sizeof(feat_block) )
    {
      return -2;
    }
  }
  return 0;
}

CString Cspell::RetrieveResourceRef(int fh)
{
  CString tmpstr;
  int required;

  if(fh<1) return "";
  required=(BYTE *) &header.icon-(BYTE *) &header+8;
  if(read(fh, &header, required)!=required)
  {
    close(fh);
    return "";
  }
  close(fh);
  RetrieveResref(tmpstr,header.icon);
  return tmpstr;
}

long Cspell::RetrieveNameRef(int fh)
{
  int required;

  if(fh<1) return -1;
  required=(BYTE *) &header.spellname-(BYTE *) &header+sizeof(long);
  if(read(fh, &header, required)!=required)
  {
    close(fh);
    return -1;
  }
  close(fh);
  return header.spellname;
}

//does no apply for IWD2
long Cspell::RetrieveTypeAndLevel(int fh)
{
  int required;

  if(fh<1) return -1;
  required=(BYTE *) &header.level-(BYTE *) &header+sizeof(long);
  if(read(fh, &header, required)!=required)
  {
    close(fh);
    return -1;
  }
  close(fh);
  if(iwd2_structures()) return header.level;
  switch(header.spelltype)
  {
  case 2:
    return (header.level>7)?6:(header.level-1);
  case 1:
    return (header.level>9)?15:(header.level+6);
  case 4:
    return 16;
  default:
    return -1;
  }  
}

int Cspell::ReadSpellFromFile(int fhandle, long maxlen)
{
  int fullsize;
  int fbc;
  int i;

  if(fhandle<1) return -1;
  if(maxlen==-1) maxlen=filelength(fhandle);
  if(!maxlen) return -2; //short file, invalid item
  if(read(fhandle,&header,sizeof(spell_header) )!=sizeof(spell_header) )
  {	
  	return -2; // short file, invalid item
  }
  if(memcmp(header.filetype,"SPL ",4) )
  {
	  return -2;
  }
  if(header.revision[0]!='V')
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
    fullsize=sizeof(header);
    revision=1;
  }
  if(header.extheadoffs!=fullsize )
  {
    return -2;
  }
  if(header.extheadcount!=extheadcount)
  {
    KillExtHeaders();
    extheaders=new spl_ext_header[header.extheadcount];
    if(!extheaders) return -3;
    extheadcount=header.extheadcount;
  }
  fbc=header.featblkcount;
  for(i=0;i<extheadcount;i++)
  {
    if(read(fhandle, &extheaders[i], sizeof(spl_ext_header) )!=sizeof(spl_ext_header) )
    {
      return -2;
    }
    fbc+=extheaders[i].fbcount;
    fullsize+=sizeof(spl_ext_header);
  }
  if(fbc!=featblkcount)
  {
    KillFeatHeaders();
    featblocks=new feat_block[fbc];
    if(!featblocks) return -3;
    featblkcount=fbc;
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
    return 1; //incorrect length
  }
  return 0;
}
