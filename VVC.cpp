// VVC.cpp: implementation of the CVVC class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "chitem.h"
#include "VVC.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVVC::CVVC()
{

}

CVVC::~CVVC()
{

}

int CVVC::WriteVVCToFile(int fhandle)
{
  memcpy(header.filetype,"VVC V1.0",8);
  if(write(fhandle,&header,sizeof(vvc_header) )!=sizeof(vvc_header) )
  {
    return -2;
  }
  return 0;
}

int CVVC::ReadVVCFromFile(int fhandle, long maxlen)
{
  if(maxlen==-1) maxlen=filelength(fhandle);
  if(maxlen!=sizeof(vvc_header) )
  {
    return -2;
  }
  if(read(fhandle,&header,sizeof(vvc_header) )!=sizeof(vvc_header) )
  {
    return -2;
  }
  return 0;
}
