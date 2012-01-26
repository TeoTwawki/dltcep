// WFX.h: interface for the CVVC class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WFX_H__9EF7094C_4DCB_46D0_8A8F_1177EAE3AE70__INCLUDED_)
#define AFX_WFX_H__9EF7094C_4DCB_46D0_8A8F_1177EAE3AE70__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WFX_CUTSCENE 1
#define WFX_SRCURVE  2
#define WFX_FREQUENCY 4
#define WFX_VOLUME 8
#define WFX_NO_EAX 16

class CWFX
{
public:
  wfx_header header;
  bool m_changed;

	CWFX();
	virtual ~CWFX();
	int ReadWFXFromFile(int fhandle, long maxlen);
  int WriteWFXToFile(int fhandle);
};

#endif // !defined(AFX_WFX_H__9EF7094C_4DCB_46D0_8A8F_1177EAE3AE70__INCLUDED_)
