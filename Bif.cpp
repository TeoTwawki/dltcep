// Bif.cpp: implementation of the CBif class.
// This is a class for bif manipulations
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <fcntl.h>
#include <sys/stat.h>
#include "chitem.h"
#include "Bif.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBif::CBif()
{
  entries=NULL;
  tisentries=NULL;
}

CBif::~CBif()
{
  if(entries) delete [] entries;
  if(tisentries) delete [] tisentries;
}

int CBif::FindResType(int restype, int start)
{
  int i;

  for(i=start;i<header.file_entries;i++)
  {
    if(entries[i].restype==restype) return i;
  }
  return -1;
}

//returns 1 if ordering is required
int CBif::ReadBifHeader(CString filename)
{
//  bif_entry *tmpentries=NULL; // unsorted entries
  int i;
  int esize;
  int fhandle;
  int ret;

  fhandle=open(filename,O_RDONLY|O_BINARY);
  if(fhandle<1) return -2;
  ret=0;
  if(read(fhandle,&header,sizeof(header))!=sizeof(header))
  {
    ret=-2;
    goto endofquest;
  }
  if(memcmp(header.signature,"BIFF",4))
  {
    if(memcmp(header.signature,"BIFC",4))
    {
      ret=-2;      //corrupted file
      goto endofquest;
    }
    return -1; //BIFC not supported
  }
  if(header.file_offset!=sizeof(header))
  {
    ret=-2;      //corrupted file
    goto endofquest;
  }

  esize=header.file_entries*sizeof(bif_entry);
  startpos=esize+header.tile_entries*sizeof(tis_entry)+header.file_offset;
  if(filelength(fhandle)<startpos)
  {
    ret=-2;      //corrupted file
    goto endofquest;
  }
  if(entries) delete [] entries;
  entries=new bif_entry[header.file_entries];
  if(!entries)
  {
    ret=-3;
    goto endofquest;
  }
  if(read(fhandle,entries,esize)!=esize)
  {
    ret=-2;      //corrupted file
    goto endofquest;
  }
  esize=header.tile_entries*sizeof(tis_entry);
  if(tisentries) delete [] tisentries;
  tisentries=new tis_entry[header.tile_entries];
  if(!tisentries)
  {
    ret=-3;      //out of memory
    goto endofquest;
  }
  if(read(fhandle,tisentries,esize)!=esize)
  {
    ret=-2;      //corrupted file
    goto endofquest;
  }
  //reordering bif entries according to res_loc
  for(i=0;i<header.file_entries;i++)
  {
    if(entries[i].res_loc!=i)
    {
      entries[i].res_loc=i;
      ret=1;
    }
  }
  for(i=0;i<header.tile_entries;i++)
  {
    int ii=(i+1)<<14;
    if(tisentries[i].res_loc!=ii)
    {
      tisentries[i].res_loc=ii;
      ret=1;
    }
  }
endofquest:
  close(fhandle);
  return ret;
}

int CBif::WriteBifHeader(CString filename)
{
  int esize;
  int fhandle;
  int ret;

  fhandle=open(filename,O_RDWR|O_BINARY|O_TRUNC|O_CREAT,S_IREAD|S_IWRITE);
  if(fhandle<1) return -2;
  if(write(fhandle,&header,sizeof(header))!=sizeof(header))
  {
    ret=-2;
    goto endofquest;
  }
  esize=header.file_entries*sizeof(bif_entry);
  if(write(fhandle,entries,esize)!=esize)
  {
    ret=-2;
    goto endofquest;
  }

  esize=header.tile_entries*sizeof(tis_entry);
  if(write(fhandle,tisentries,esize)!=esize)
  {
    ret=-2;
    goto endofquest;
  }
  
  ret=0;
endofquest:
  close(fhandle);
  return ret;
}

//copying BIF data from a complete bif to a bif stub
int CBif::CopyData(CString infile, CString outfile)
{
  int fin, fout;
  int ret;

  fin=open(infile,O_RDONLY|O_BINARY);
  if(fin<1) return -2;
  fout=open(outfile,O_RDWR|O_BINARY|O_TRUNC|O_CREAT,S_IREAD|S_IWRITE);
  if(fout<1)
  {
    close(fin);
    return -2;
  }
  ret=0;
  
  if(lseek(fin,startpos,SEEK_SET)!=startpos)
  {
    ret=-2;
    goto endofquest;
  }

  if(lseek(fout,startpos,SEEK_SET)!=startpos)
  {
    ret=-2;
    goto endofquest;
  }
  //the remaining data is transferred by the original copy_file
  //remaining filesize is whole length-startpos,
  //decryption is set to 0
  ret=copy_file(fin, fout, filelength(fin)-startpos,0);
endofquest:
  close(fin);
  close(fout);
  return ret;
}
