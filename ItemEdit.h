#if !defined(AFX_ITEMEDIT_H__A8674F8D_B979_4A4E_87F6_976985F68A8B__INCLUDED_)
#define AFX_ITEMEDIT_H__A8674F8D_B979_4A4E_87F6_976985F68A8B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ItemEdit.h : header file
//

#include "ItemProperties.h"

/////////////////////////////////////////////////////////////////////////////
// CItemEdit dialog
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
class CItemEdit : public CDialog
{
// Construction
public:
	CItemEdit(CWnd* pParent = NULL);   // standard constructor
  void NewItem();

// Dialog Data
	//{{AFX_DATA(CItemEdit)
	enum { IDD = IDD_ITEMEDIT };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CItemEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL
// Implementation
protected:
	CItemPropertySheet* m_pModelessPropSheet;
  CToolTipCtrl m_tooltip;

  void SaveItem(int save);
	// Generated message map functions
	//{{AFX_MSG(CItemEdit)
	afx_msg void OnLoad();
	afx_msg void OnLoadex();
	afx_msg void OnSaveas();
	virtual BOOL OnInitDialog();
	afx_msg void OnNew();
	afx_msg void OnCheck();
	afx_msg void OnSave();
	afx_msg void OnFileTbg();
	afx_msg void OnFileTp2();
	afx_msg void OnToolsLookupstrref();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ITEMEDIT_H__A8674F8D_B979_4A4E_87F6_976985F68A8B__INCLUDED_)
