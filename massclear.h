#if !defined(AFX_MASSCLEAR_H__272DFBFE_EE3F_41C6_BBEC_6DD8DAE7F90E__INCLUDED_)
#define AFX_MASSCLEAR_H__272DFBFE_EE3F_41C6_BBEC_6DD8DAE7F90E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// massclear.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// massclear dialog

class massclear : public CDialog
{
// Construction
public:
	massclear(CWnd* pParent = NULL);   // standard constructor
  void SetRange(int minval, int maxval, int curval);
  void SetText(CString str);
// Dialog Data
	//{{AFX_DATA(massclear)
	enum { IDD = IDD_MASSCLEAR };
	CEdit	m_from_control;
	CSpinButtonCtrl	m_tosp_control;
	CSpinButtonCtrl	m_fromsp_control;
	int		m_from;
	int		m_to;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(massclear)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

  int m_minval, m_maxval;
  CString m_caption;

	// Generated message map functions
	//{{AFX_MSG(massclear)
	afx_msg void OnKillfocusFrom();
	afx_msg void OnKillfocusTo();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MASSCLEAR_H__272DFBFE_EE3F_41C6_BBEC_6DD8DAE7F90E__INCLUDED_)
