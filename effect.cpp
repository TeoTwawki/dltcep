// effect.cpp: implementation of the Ceffect class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <fcntl.h>

#include "chitem.h"
#include "effect.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Ceffect::Ceffect()
{

}

Ceffect::~Ceffect()
{

}

int Ceffect::WriteEffectToFile(int fhandle, int calculate)
{
  if(calculate) return sizeof(eff_header);

  memcpy(header.filetype,"EFF V2.0",8);
  memcpy(header.signature,"EFF V2.0",8);
  if(write(fhandle,&header,sizeof(eff_header) )!=sizeof(eff_header) )
  {
    return -2;
  }
  return 0;
}

int Ceffect::ReadEffectFromFile(int fhandle, int maxlen)
{
  if(maxlen==-1) maxlen=filelength(fhandle);
  if(!maxlen) return -2; //short file, invalid item

  if(read(fhandle,&header,sizeof(eff_header) )!=sizeof(eff_header) )
  {	
  	return -2; // short file, invalid item
  }
  if(memcmp(header.filetype,"EFF ",4) )
  {
	  return -2;
  }
  if(header.revision[0]!='V')
  {
	  return -2;
  }
  return 0;
}
