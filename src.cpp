// src.cpp: implementation of the Csrc class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "chitem.h"
#include "src.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Csrc::Csrc()
{
  m_slots=NULL;
  m_cnt=0;
}

Csrc::~Csrc()
{
  KillSlots();
}

int Csrc::WriteStringToFile(int fhandle, int calculate)
{
  long tmp[2];
  int i;

  if(calculate) return m_cnt*sizeof(long)*2+sizeof(long);
  if(write(fhandle, &m_cnt,sizeof(long) )!=sizeof(long)) return -2;
  tmp[1]=1;
  for(i=0; i<m_cnt;i++)
  {
    tmp[0]=m_slots[i];
    if(write(fhandle,tmp,sizeof(tmp))!=sizeof(tmp)) return -2;
  }
  return 0;
}

int Csrc::ReadStringFromFile(int fhandle, int ml)
{
  long tmp[2];
  int i;

  KillSlots();
  if(fhandle<1) return -1;
  if(ml<0) ml=filelength(fhandle);
  if(ml<0) return -2;
  if(read(fhandle, &m_cnt,sizeof(long))!=sizeof(long)) return -2;
  if(m_cnt*sizeof(long)*2+sizeof(long)!=(unsigned long) ml) return -1;
  m_slots=new long[m_cnt];
  if(!m_slots) return -3;
  for(i=0;i<m_cnt;i++)
  {
    if(read(fhandle, tmp, sizeof(tmp))!=sizeof(tmp)) return -2;
    m_slots[i]=tmp[0];
  }
  return 0;
}
