#if !defined(AFX_AREATRIGGERSTRING_H__1C2BC1DE_1BFC_4418_AFBA_0866EF06179C__INCLUDED_)
#define AFX_AREATRIGGERSTRING_H__1C2BC1DE_1BFC_4418_AFBA_0866EF06179C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AreaTriggerString.h : header file
//

#include "AreaProperties.h"
/////////////////////////////////////////////////////////////////////////////
// CAreaTriggerString dialog

class CAreaTriggerString : public CDialog
{
// Construction
public:
	CAreaTriggerString(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAreaTriggerString)
	enum { IDD = IDD_AREATRIGGERSTRINGS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
  CAreaTrigger *parent;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAreaTriggerString)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAreaTriggerString)
	afx_msg void OnDefaultKillfocus();
	afx_msg void OnKillfocusInfostr();
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusInfostr2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AREATRIGGERSTRING_H__1C2BC1DE_1BFC_4418_AFBA_0866EF06179C__INCLUDED_)
