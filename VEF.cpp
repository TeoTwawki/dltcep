// VEF.cpp: implementation of the CVEF class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "chitem.h"
#include "VEF.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVEF::CVEF()
{
  comp1 = NULL;
  comp2 = NULL;
  comp1count = 0;
  comp2count = 0;
}

CVEF::~CVEF()
{
  KillComp1Headers();
  KillComp2Headers();
}

int CVEF::WriteVEFToFile(int fhandle, int calculate)
{
  int bc1, bc2;

  bc1 = comp1count*sizeof(vef_component);
  bc2 = comp2count*sizeof(vef_component);
  memcpy(header.filetype,"VEF \0\0\0",8);
  header.offset1 = sizeof(vef_header);
  header.count1 = comp1count;
  header.offset2 = header.offset1 + bc1;
  header.count2 = comp2count;

  if(write(fhandle,&header,sizeof(vef_header) )!=sizeof(vef_header) )
  {
    return -2;
  }
  if (calculate)
  {
    return header.offset2+bc2;
  }

  if (write(fhandle, comp1, bc1)!=bc1)
  {
    return -2;
  }
  if (write(fhandle, comp2, bc2)!=bc2)
  {
    return -2;
  }
  m_changed=false;
  return 0;
}

int CVEF::ReadVEFFromFile(int fhandle, long maxlen)
{
  int bc1, bc2;

  m_changed=false;
  if(maxlen==-1) maxlen=filelength(fhandle);
  if(maxlen<sizeof(vef_header) )
  {
    return -2;
  }
  if(read(fhandle,&header,sizeof(vef_header) )!=sizeof(vef_header) )
  {
    return -2;
  }
  if(header.count1!=comp1count)
  {
    KillComp1Headers();
    comp1 = new vef_component[header.count1];
    if (!comp1) return -3;
    comp1count = header.count1;
  }

  bc1 = comp1count*sizeof(vef_component);
  if (read(fhandle,comp1,bc1)!=bc1)
  {
    return -2;
  }

  if(header.count2!=comp2count)
  {
    KillComp2Headers();
    comp2 = new vef_component[header.count2];
    if (!comp2) return -3;
    comp2count = header.count2;
  }

  bc2 = comp2count*sizeof(vef_component);
  if (read(fhandle,comp2,bc2)!=bc2)
  {
    return -2;
  }

  return 0;
}
