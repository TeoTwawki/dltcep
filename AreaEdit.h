#if !defined(AFX_AREAEDIT_H__A8674F8D_B979_4A4E_87F6_976985F68A8B__INCLUDED_)
#define AFX_AREAEDIT_H__A8674F8D_B979_4A4E_87F6_976985F68A8B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AreaEdit.h : header file
//

#include "AreaProperties.h"

/////////////////////////////////////////////////////////////////////////////
// CAreaEdit dialog

class CAreaEdit : public CDialog
{
// Construction
public:
	CAreaEdit(CWnd* pParent = NULL);   // standard constructor
  void NewArea();

// Dialog Data
	//{{AFX_DATA(CAreaEdit)
	enum { IDD = IDD_AREAEDIT };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAreaEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL
// Implementation
protected:
	CAreaPropertySheet* m_pModelessPropSheet;
  CToolTipCtrl m_tooltip;

  void SaveArea(int save);
	// Generated message map functions
	//{{AFX_MSG(CAreaEdit)
	afx_msg void OnLoad();
	afx_msg void OnLoadex();
	afx_msg void OnSaveas();
	virtual BOOL OnInitDialog();
	afx_msg void OnNew();
	afx_msg void OnCheck();
	afx_msg void OnSave();
	afx_msg void OnFileTbg();
	afx_msg void OnToolsMirrorareavertically();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AREAEDIT_H__A8674F8D_B979_4A4E_87F6_976985F68A8B__INCLUDED_)
