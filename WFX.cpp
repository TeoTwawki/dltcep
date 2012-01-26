// WFX.cpp: implementation of the CWFX class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "chitem.h"
#include "WFX.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWFX::CWFX()
{

}

CWFX::~CWFX()
{

}

int CWFX::WriteWFXToFile(int fhandle)
{
  memcpy(header.filetype,"WFX V1.0",8);
  if(write(fhandle,&header,sizeof(wfx_header) )!=sizeof(wfx_header) )
  {
    return -2;
  }
  m_changed=false;
  return 0;
}

int CWFX::ReadWFXFromFile(int fhandle, long maxlen)
{
  m_changed=false;
  if(maxlen==-1) maxlen=filelength(fhandle);
  if(maxlen!=sizeof(wfx_header) )
  {
    return -2;
  }
  if(read(fhandle,&header,sizeof(wfx_header) )!=sizeof(wfx_header) )
  {
    return -2;
  }
  return 0;
}
