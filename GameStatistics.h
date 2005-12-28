#if !defined(AFX_GAMESTATISTICS_H__4C10B274_99C2_43EC_9BBB_A5A51C499FBF__INCLUDED_)
#define AFX_GAMESTATISTICS_H__4C10B274_99C2_43EC_9BBB_A5A51C499FBF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GameStatistics.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGameStatistics dialog

class CGameStatistics : public CDialog
{
// Construction
public:
	CGameStatistics(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGameStatistics)
	enum { IDD = IDD_GAMESTATS };
	CComboBox	m_spellpicker;
	CComboBox	m_weaponpicker;
	CString	m_text;
	//}}AFX_DATA
  gam_npc_extension *m_stats;
  char (*m_wstr)[8];
  short *m_wcnt;
  char (*m_sstr)[8];
  short *m_scnt;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGameStatistics)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  CToolTipCtrl m_tooltip;

  void RefreshDialog();
	// Generated message map functions
	//{{AFX_MSG(CGameStatistics)
	afx_msg void OnDefaultKillfocus();
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusStrref();
	afx_msg void OnSelchangeWeapon();
	afx_msg void OnSelchangeSpellpicker();
	afx_msg void OnKillfocusPicker();
	afx_msg void OnBrowse();
	afx_msg void OnBrowse2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GAMESTATISTICS_H__4C10B274_99C2_43EC_9BBB_A5A51C499FBF__INCLUDED_)
