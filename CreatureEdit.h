#if !defined(AFX_CREATUREEDIT_H__959B5420_951D_452A_A950_3BE3B9A1257D__INCLUDED_)
#define AFX_CREATUREEDIT_H__959B5420_951D_452A_A950_3BE3B9A1257D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CreatureEdit.h : header file
//

#include "CreatureProperties.h"

/////////////////////////////////////////////////////////////////////////////
// CCreatureEdit dialog

class CCreatureEdit : public CDialog
{
// Construction
public:
	CCreatureEdit(CWnd* pParent = NULL);   // standard constructor
  void NewCreature();

// Dialog Data
	//{{AFX_DATA(CCreatureEdit)
	enum { IDD = IDD_CREATUREEDIT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCreatureEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	CCreaturePropertySheet* m_pModelessPropSheet;
  CToolTipCtrl m_tooltip;

  void SaveCreature(int save);
	// Generated message map functions
	//{{AFX_MSG(CCreatureEdit)
	afx_msg void OnLoad();
	afx_msg void OnLoadex();
	afx_msg void OnSaveas();
	virtual BOOL OnInitDialog();
	afx_msg void OnNew();
	afx_msg void OnCheck();
	afx_msg void OnSave();
	afx_msg void OnFileTbg();
	afx_msg void OnExportsoundset();
	afx_msg void OnImportsoundset();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CREATUREEDIT_H__959B5420_951D_452A_A950_3BE3B9A1257D__INCLUDED_)
