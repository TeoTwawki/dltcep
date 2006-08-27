#if !defined(AFX_CFBDIALOG_H__7A151B68_7A6C_4C09_B15A_2F5E5EFC0ECA__INCLUDED_)
#define AFX_CFBDIALOG_H__7A151B68_7A6C_4C09_B15A_2F5E5EFC0ECA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CFBDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCFBDialog dialog

class CCFBDialog : public CDialog
{
// Construction
public:
  CString filename;
  bool newitem;
// Dialog Data
	//{{AFX_DATA(CCFBDialog)
	enum { IDD = IDD_CFB };
	CButton	m_outputbutton;
	CEdit	m_event_control;
	CString	m_etitle;
	CString	m_event;
	CString	m_output;
	//}}AFX_DATA
  int mode;

	CCFBDialog(CWnd* pParent = NULL);   // standard constructor
  void log(CString format, ...);
  void SetMode(int arg)
  {
    mode=arg;
  }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCFBDialog)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  void changeitemname(CString arg)
  {
    itemname=arg;
    newitem=TRUE;
  }

  CToolTipCtrl m_tooltip;

	// Generated message map functions
	//{{AFX_MSG(CCFBDialog)
	afx_msg void OnOpenfile();
	virtual BOOL OnInitDialog();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnKillfocusOutfolder();
	afx_msg void OnOutput();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CFBDIALOG_H__7A151B68_7A6C_4C09_B15A_2F5E5EFC0ECA__INCLUDED_)
