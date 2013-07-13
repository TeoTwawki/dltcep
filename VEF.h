// VEF.h: interface for the CVEF class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VEF_H__9EF7094C_4DCB_46D0_8A8F_1177EAE3AE70__INCLUDED_)
#define AFX_VEF_H__9EF7094C_4DCB_46D0_8A8F_1177EAE3AE70__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CVEF
{
public:
  vef_header header;
  vef_component *comp1;
  vef_component *comp2;
  int comp1count;
  int comp2count;
  bool m_changed;

	CVEF();
	virtual ~CVEF();
	int ReadVEFFromFile(int fhandle, long maxlen);
  int WriteVEFToFile(int fhandle, int calculate=0);
  inline void KillComp1Headers()
  {
    if(comp1)
    {
      delete[] comp1;
      comp1=NULL;
      comp1count=0;
    }
  }
  inline void KillComp2Headers()
  {
    if(comp2)
    {
      delete[] comp2;
      comp2=NULL;
      comp2count=0;
    }
  }
};

#endif // !defined(AFX_VEF_H__9EF7094C_4DCB_46D0_8A8F_1177EAE3AE70__INCLUDED_)
