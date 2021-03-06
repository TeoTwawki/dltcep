#if !defined(AFX_CREATUREOVERLAY_H__1D2FC922_89E8_473B_A9A6_F105E4B60E8E__INCLUDED_)
#define AFX_CREATUREOVERLAY_H__1D2FC922_89E8_473B_A9A6_F105E4B60E8E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CreatureOverlay.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCreatureOverlay dialog

class CCreatureOverlay : public CDialog
{
// Construction
public:
	CCreatureOverlay(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCreatureOverlay)
	enum { IDD = IDD_CREATUREOVR };
	CString	m_bamres;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCreatureOverlay)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  CToolTipCtrl m_tooltip;
  int m_itemcount;
  int m_curritem;

  void ResetCombo();

	// Generated message map functions
	//{{AFX_MSG(CCreatureOverlay)
	virtual BOOL OnInitDialog();
	afx_msg void OnBrowse();
	afx_msg void OnClear();
	afx_msg void OnKillfocusOverlaypicker();
	afx_msg void OnSelchangeOverlaypicker();
	afx_msg void OnDefaultKillfocus();
	afx_msg void OnKillfocusBam();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CREATUREOVERLAY_H__1D2FC922_89E8_473B_A9A6_F105E4B60E8E__INCLUDED_)
