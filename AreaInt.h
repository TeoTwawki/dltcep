#if !defined(AFX_AREAINT_H__CB227BA8_E0D5_4D98_A227_A67BE76C5DC4__INCLUDED_)
#define AFX_AREAINT_H__CB227BA8_E0D5_4D98_A227_A67BE76C5DC4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AreaInt.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAreaInt dialog

class CAreaInt : public CDialog
{
// Construction
public:
	CAreaInt(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAreaInt)
	enum { IDD = IDD_AREAINT };
	CComboBox	m_spawnnumpicker;
	CString	m_text;
	//}}AFX_DATA

  int m_crenum;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAreaInt)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

  void RefreshInt();
	// Generated message map functions
	//{{AFX_MSG(CAreaInt)
	afx_msg void OnKillfocusSection();
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusMin();
	afx_msg void OnKillfocusMax();
	afx_msg void OnKillfocusCreres();
	afx_msg void OnKillfocusStrref();
	afx_msg void OnKillfocusText();
	afx_msg void OnKillfocusUnknown9a();
	afx_msg void OnKillfocusUnknown9c();
	afx_msg void OnKillfocusUnknowna0();
	afx_msg void OnKillfocusUnknowna2();
	afx_msg void OnBrowse();
	afx_msg void OnAddcre();
	afx_msg void OnDelcre();
	afx_msg void OnKillfocusUnknowna8();
	afx_msg void OnKillfocusUnknownaa();
	afx_msg void OnKillfocusSpawnnumpicker();
	afx_msg void OnSelchangeSpawnnumpicker();
	afx_msg void OnClear();
	afx_msg void OnKillfocusUnknownac();
	afx_msg void OnKillfocusUnknownae();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AREAINT_H__CB227BA8_E0D5_4D98_A227_A67BE76C5DC4__INCLUDED_)
