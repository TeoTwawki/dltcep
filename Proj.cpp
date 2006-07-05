// proj.cpp: implementation of the Cproj class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <fcntl.h>

#include "chitem.h"
#include "proj.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Cproj::Cproj()
{
  hasextension=0;
}

Cproj::~Cproj()
{

}

int Cproj::WriteProjectileToFile(int fhandle, int calculate)
{
  if(calculate)
  {
    if(header.type!=3) return sizeof(proj_header);
    return sizeof(proj_header)+sizeof(proj_extension);
  }
  memcpy(header.filetype,"PRO V1.0",8);
  if(write(fhandle,&header,sizeof(proj_header) )!=sizeof(proj_header) )
  {	
  	return -2; // short file, invalid item
  }
  if(header.type!=3)
  {
    m_changed=false;
    return 0;
  }
  if(write(fhandle,&extension,sizeof(proj_extension) )!=sizeof(proj_extension) )
  {	
  	return -2; // short file, invalid item
  }
  m_changed=false;
  return 0;
}

int Cproj::ReadProjectileFromFile(int fhandle, long maxlen)
{
  int fullsize;

  m_changed=false;
  if(maxlen==-1) maxlen=filelength(fhandle);
  if(maxlen<1) return -1; //short file, invalid item
  if(read(fhandle,&header,sizeof(proj_header) )!=sizeof(proj_header) )
  {	
  	return -2; // short file, invalid item
  }
  if(memcmp(header.filetype,"PRO ",4) )
  {
	  return -2;
  }
  if(header.revision[0]!='V')
  {
	  return -2;
  }
  fullsize=sizeof(proj_header);
  memset(&extension,0,sizeof(proj_extension));
  hasextension=0;
  if(maxlen==fullsize) return 0;
  hasextension=1;
  if(read(fhandle,&extension,sizeof(proj_extension))!=sizeof(proj_extension))
  {
    return -2;
  }
  fullsize+=sizeof(proj_extension);
  if(fullsize!=maxlen)
  {
    return -3;
  }
  return 0;
}
