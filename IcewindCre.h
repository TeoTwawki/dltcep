#if !defined(AFX_ICEWINDCRE_H__5BA36393_1E87_40D8_BDC8_C70E412AA0CE__INCLUDED_)
#define AFX_ICEWINDCRE_H__5BA36393_1E87_40D8_BDC8_C70E412AA0CE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IcewindCre.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// IcewindCre dialog

class IcewindCre : public CDialog
{
// Construction
public:
	IcewindCre(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(IcewindCre)
	enum { IDD = IDD_CREATUREIWD };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(IcewindCre)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  CToolTipCtrl m_tooltip;

	// Generated message map functions
	//{{AFX_MSG(IcewindCre)
	afx_msg void DefaultKillfocus();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ICEWINDCRE_H__5BA36393_1E87_40D8_BDC8_C70E412AA0CE__INCLUDED_)
