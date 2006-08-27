#if !defined(AFX_STOREEDIT_H__74572A68_4088_4AD6_B718_A8526E86C000__INCLUDED_)
#define AFX_STOREEDIT_H__74572A68_4088_4AD6_B718_A8526E86C000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// StoreEdit.h : header file
//

#include "StoreProperties.h"

/////////////////////////////////////////////////////////////////////////////
// CStoreEdit dialog

class CStoreEdit : public CDialog
{
// Construction
public:
	CStoreEdit(CWnd* pParent = NULL);   // standard constructor
  void NewStore();

// Dialog Data
	//{{AFX_DATA(CStoreEdit)
	enum { IDD = IDD_STOREEDIT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStoreEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	CStorePropertySheet* m_pModelessPropSheet;
  CToolTipCtrl m_tooltip;

  void OnProperties();
  void SaveStore(int save);

	// Generated message map functions
	//{{AFX_MSG(CStoreEdit)
	afx_msg void OnLoad();
	afx_msg void OnLoadex();
	afx_msg void OnSaveas();
	virtual BOOL OnInitDialog();
	afx_msg void OnNew();
	afx_msg void OnCheck();
	afx_msg void OnFileTbg();
	afx_msg void OnFileTp2();
	afx_msg void OnSave();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STOREEDIT_H__74572A68_4088_4AD6_B718_A8526E86C000__INCLUDED_)
