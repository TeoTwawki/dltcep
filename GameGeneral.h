#if !defined(AFX_GAMEGENERAL_H__FA8C3431_A511_4A4F_8E87_3A6B970A4E9C__INCLUDED_)
#define AFX_GAMEGENERAL_H__FA8C3431_A511_4A4F_8E87_3A6B970A4E9C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GameGeneral.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGameGeneral dialog

class CGameGeneral : public CDialog
{
// Construction
public:
	CGameGeneral(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGameGeneral)
	enum { IDD = IDD_GAMEGENERAL };
	CSliderCtrl	m_slider_control;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGameGeneral)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CToolTipCtrl m_tooltip;

	// Generated message map functions
	//{{AFX_MSG(CGameGeneral)
	afx_msg void OnDefaultKillfocus();
	afx_msg void OnFlag1();
	afx_msg void OnFlag2();
	afx_msg void OnFlag3();
	afx_msg void OnFlag4();
	afx_msg void OnFlag5();
	afx_msg void OnFlag6();
	afx_msg void OnFlag7();
	afx_msg void OnFlag8();
	afx_msg void OnBrowse();
	virtual BOOL OnInitDialog();
	afx_msg void OnBrowse2();
	//}}AFX_MSG
	afx_msg void OnDefaultKillfocusX(NMHDR *, LRESULT *);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GAMEGENERAL_H__FA8C3431_A511_4A4F_8E87_3A6B970A4E9C__INCLUDED_)
