// src.h: interface for the Csrc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SRC_H__3CB9CB45_30FE_4026_A800_0588B9F07FA5__INCLUDED_)
#define AFX_SRC_H__3CB9CB45_30FE_4026_A800_0588B9F07FA5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Csrc  
{
public:
	Csrc();
	virtual ~Csrc();

  long *m_slots;
  long m_cnt;

  int WriteStringToFile(int fhandle, int calculate);
  int ReadStringFromFile(int fhandle, int ml);

  inline void KillSlots()
  {
    if(m_slots) delete [] m_slots;
    m_slots=NULL;
    m_cnt=0;
  }
};

#endif // !defined(AFX_SRC_H__3CB9CB45_30FE_4026_A800_0588B9F07FA5__INCLUDED_)
