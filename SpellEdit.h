#if !defined(AFX_ITEMEDIT_H__A8674F8D_B979_4A4E_87F6_976985F68A8C__INCLUDED_)
#define AFX_ITEMEDIT_H__A8674F8D_B979_4A4E_87F6_976985F68A8C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SpellEdit.h : header file
//

#include "SpellProperties.h"

/////////////////////////////////////////////////////////////////////////////
// CSpellEdit dialog
/*
#define REF_BAM   0x03e8
#define REF_ITM   0x03ed
#define REF_SPL   0x03ee
#define REF_BCS   0x03ef
#define REF_IDS   0x03f0
#define REF_CRE   0x03f1
#define REF_DLG   0x03f3
#define REF_2DA   0x03f4
#define REF_STO   0x03f6
#define REF_EFF   0x03f8
#define REF_PRO   0x03fd
*/
class CSpellEdit : public CDialog
{
// Construction
public:
	CSpellEdit(CWnd* pParent = NULL);   // standard constructor
  void NewSpell();

// Dialog Data
	//{{AFX_DATA(CSpellEdit)
	enum { IDD = IDD_SPELLEDIT };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpellEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL
// Implementation
protected:
	CSpellPropertySheet* m_pModelessPropSheet;
  CToolTipCtrl m_tooltip;

  void SaveSpell(int save);
	// Generated message map functions
	//{{AFX_MSG(CSpellEdit)
	afx_msg void OnLoad();
	afx_msg void OnLoadex();
	afx_msg void OnSaveas();
	virtual BOOL OnInitDialog();
	afx_msg void OnNew();
	afx_msg void OnCheck();
	afx_msg void OnSave();
	afx_msg void OnFileTbg();
	afx_msg void OnAddcfb();
	afx_msg void OnRemovecfb();
	afx_msg void OnSavecfb();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ITEMEDIT_H__A8674F8D_B979_4A4E_87F6_976985F68A8C__INCLUDED_)
