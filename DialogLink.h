#if !defined(AFX_DIALOGLINK_H__9991A6B8_8CEB_4AC1_ABE9_EB491E1DBB38__INCLUDED_)
#define AFX_DIALOGLINK_H__9991A6B8_8CEB_4AC1_ABE9_EB491E1DBB38__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogLink.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogLink dialog

class CDialogLink : public CDialog
{
// Construction
public:
	CDialogLink(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogLink)
	enum { IDD = IDD_ADDLINK };
	CEdit	m_dialogres_control;
	CString	m_dialogres;
	int		m_state;
	BOOL	m_internal;
	BOOL	m_leafnode;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogLink)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  void RefreshDialog();
	// Generated message map functions
	//{{AFX_MSG(CDialogLink)
	afx_msg void OnInternal();
	afx_msg void OnKillfocusDialog();
	afx_msg void OnKillfocusState();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnBrowse();
	afx_msg void OnRemove();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGLINK_H__9991A6B8_8CEB_4AC1_ABE9_EB491E1DBB38__INCLUDED_)
