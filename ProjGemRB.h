#if !defined(AFX_PROJGEMRB_H__BD52A523_DB9E_4DDA_9A9A_8D22A2981094__INCLUDED_)
#define AFX_PROJGEMRB_H__BD52A523_DB9E_4DDA_9A9A_8D22A2981094__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProjGemRB.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProjGemRB dialog

class CProjGemRB : public CDialog
{
// Construction
public:
	CProjGemRB(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CProjGemRB)
	enum { IDD = IDD_PROJGEMRB };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProjGemRB)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CProjGemRB)
	afx_msg void OnFlag1();
	afx_msg void OnFlag2();
	afx_msg void OnFlag3();
	afx_msg void OnFlag4();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROJGEMRB_H__BD52A523_DB9E_4DDA_9A9A_8D22A2981094__INCLUDED_)
