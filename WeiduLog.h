#if !defined(AFX_WEIDULOG_H__7902E117_F285_49C5_BB4D_89954068E314__INCLUDED_)
#define AFX_WEIDULOG_H__7902E117_F285_49C5_BB4D_89954068E314__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WeiduLog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWeiduLog dialog

class CWeiduLog : public CDialog
{
// Construction
public:
	CWeiduLog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWeiduLog)
	enum { IDD = IDD_SHOWLOG };
	CString	m_log;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWeiduLog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWeiduLog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WEIDULOG_H__7902E117_F285_49C5_BB4D_89954068E314__INCLUDED_)
