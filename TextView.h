#if !defined(AFX_TEXTVIEW_H__AE5DE80F_97AC_40DF_A901_4339FCBDDF31__INCLUDED_)
#define AFX_TEXTVIEW_H__AE5DE80F_97AC_40DF_A901_4339FCBDDF31__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TextView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTextView dialog

class CTextView : public CDialog
{
// Construction
public:
	CTextView(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTextView)
	enum { IDD = IDD_TEXTVIEW };
	CRichEditCtrl	m_text_control;
	//}}AFX_DATA
  CString m_text, m_file;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTextView)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTextView)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEXTVIEW_H__AE5DE80F_97AC_40DF_A901_4339FCBDDF31__INCLUDED_)
