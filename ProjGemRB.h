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
	CString	m_text;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProjGemRB)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:  
  void RefreshStrings();

	// Generated message map functions
	//{{AFX_MSG(CProjGemRB)
	afx_msg void OnFlag1();
	afx_msg void OnFlag2();
	afx_msg void OnFlag3();
	afx_msg void OnFlag4();
	afx_msg void OnFlag5();
	afx_msg void OnFlag6();
	afx_msg void OnFlag7();
	afx_msg void OnFlag8();
	afx_msg void OnFlag9();
	afx_msg void OnFlag10();
	afx_msg void OnFlag11();
	afx_msg void OnFlag12();
	afx_msg void OnFlag13();
	afx_msg void OnFlag14();
	afx_msg void OnFlag15();
	afx_msg void OnFlag16();
	afx_msg void OnFlag17();
	afx_msg void OnFlag18();
	afx_msg void OnKillfocus();
	afx_msg void OnFlag19();
	afx_msg void OnKillfocusString();
	virtual BOOL OnInitDialog();
	afx_msg void OnFlag20();
	afx_msg void OnFlag21();
	afx_msg void OnFlag22();
	afx_msg void OnFlag23();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CProjAreaGemRB dialog

class CProjAreaGemRB : public CDialog
{
// Construction
public:
	CProjAreaGemRB(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CProjAreaGemRB)
	enum { IDD = IDD_PROJAREAGEMRB };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProjAreaGemRB)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CProjAreaGemRB)
	afx_msg void OnFlag1();
	afx_msg void OnFlag2();
	afx_msg void OnFlag3();
	afx_msg void OnFlag4();
	afx_msg void OnFlag5();
	afx_msg void OnFlag6();
	afx_msg void OnFlag7();
	afx_msg void OnFlag8();
	afx_msg void OnKillfocus();
	afx_msg void OnBrowse1();
	afx_msg void OnBrowse2();
	afx_msg void OnBrowse3();
	afx_msg void OnPlay1();
	afx_msg void OnFlag9();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROJGEMRB_H__BD52A523_DB9E_4DDA_9A9A_8D22A2981094__INCLUDED_)
