#if !defined(AFX_PCINFO_H__076EE811_28B9_4EF1_B01D_DDE6B53D5E08__INCLUDED_)
#define AFX_PCINFO_H__076EE811_28B9_4EF1_B01D_DDE6B53D5E08__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PCInfo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPCInfo dialog

class CPCInfo : public CDialog
{
// Construction
public:
	CPCInfo(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPCInfo)
	enum { IDD = IDD_GAMEPCINFO };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
	gam_npc *m_data;
	gam_npc_extension *m_stat;
  char (*quickspells)[8];

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPCInfo)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CToolTipCtrl m_tooltip;

  void BrowseSpell(int x);
  void Wslot(int x);

	// Generated message map functions
	//{{AFX_MSG(CPCInfo)
	virtual BOOL OnInitDialog();
	afx_msg void OnDefaultKillfocus();
	afx_msg void OnBrowse1();
	afx_msg void OnBrowse2();
	afx_msg void OnBrowse3();
	afx_msg void OnBrowse4();
	afx_msg void OnBrowse5();
	afx_msg void OnBrowse6();
	afx_msg void OnBrowse7();
	afx_msg void OnBrowse8();
	afx_msg void OnBrowse9();
	afx_msg void OnBrowse10();
	afx_msg void OnBrowse11();
	afx_msg void OnFlag11();
	afx_msg void OnFlag12();
	afx_msg void OnFlag13();
	afx_msg void OnFlag14();
	afx_msg void OnFlag15();
	afx_msg void OnFlag16();
	afx_msg void OnFlag17();
	afx_msg void OnFlag18();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PCINFO_H__076EE811_28B9_4EF1_B01D_DDE6B53D5E08__INCLUDED_)
