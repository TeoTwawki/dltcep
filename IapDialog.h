#if !defined(AFX_IAPDIALOG_H__5DD8C81E_E14D_4748_9FD0_7CCE878092C1__INCLUDED_)
#define AFX_IAPDIALOG_H__5DD8C81E_E14D_4748_9FD0_7CCE878092C1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IapDialog.h : header file
//

#include "tbg.h"

/////////////////////////////////////////////////////////////////////////////
// CIapDialog dialog

class CIapDialog : public CDialog
{
// Construction
public:
	CIapDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CIapDialog)
	enum { IDD = IDD_IAPDIALOG };
	CButton	m_launch_control;
	CButton	m_opentbg;
	CButton	m_openother;
	CComboBox	m_tbgpicker;
	CComboBox	m_otherpicker;
	CString	m_newother;
	CString	m_newtbg;
	//}}AFX_DATA
  Ctbg the_iap;
//  CBitmap m_bbmp;

  void NewIAP();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIapDialog)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  CToolTipCtrl m_tooltip;

  void UpdateControls();
  void RefreshDialog();
  void Refresh();
  void AddOther();
  void AddTbg();
  int WriteIap(int fhandle);
	// Generated message map functions
	//{{AFX_MSG(CIapDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnNew();
	afx_msg void OnSaveas();
	afx_msg void OnKillfocusOther();
	afx_msg void OnKillfocusTbg();
	afx_msg void OnOpenother();
	afx_msg void OnOpentbg();
	afx_msg void OnDblclkOther();
	afx_msg void OnDblclkTbg();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnSelchangeOther();
	afx_msg void OnLaunch();
	afx_msg void OnLoad();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IAPDIALOG_H__5DD8C81E_E14D_4748_9FD0_7CCE878092C1__INCLUDED_)
