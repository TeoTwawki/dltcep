// VVC.h: interface for the CVVC class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VVC_H__9EF7094C_4DCB_46D0_8A8F_1177EAE3AE70__INCLUDED_)
#define AFX_VVC_H__9EF7094C_4DCB_46D0_8A8F_1177EAE3AE70__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CVVC  
{
public:
  vvc_header header;
  bool m_changed;

	CVVC();
	virtual ~CVVC();
	int ReadVVCFromFile(int fhandle, long maxlen);
  int WriteVVCToFile(int fhandle);
};

#endif // !defined(AFX_VVC_H__9EF7094C_4DCB_46D0_8A8F_1177EAE3AE70__INCLUDED_)
