#if !defined(AFX_WFXEDIT_H__32A26AD9_A1D0_4620_B3F3_2957C4C5D5CD__INCLUDED_)
#define AFX_WFXEDIT_H__32A26AD9_A1D0_4620_B3F3_2957C4C5D5CD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WFXEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// WFXEdit dialog

class WFXEdit : public CDialog
{
// Construction
public:
	WFXEdit(CWnd* pParent = NULL);   // standard constructor
  void NewWFX();

// Dialog Data
	//{{AFX_DATA(WFXEdit)
	enum { IDD = IDD_WFXEDIT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(WFXEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  CToolTipCtrl m_tooltip;

  void RefreshDialog();
  void SaveWFX(int save);

	afx_msg void DefaultKillfocus();
	// Generated message map functions
	//{{AFX_MSG(WFXEdit)
	virtual BOOL OnInitDialog();
	afx_msg void OnFlag1();
	afx_msg void OnFlag2();
	afx_msg void OnFlag3();
	afx_msg void OnFlag4();
	afx_msg void OnFlag5();
	afx_msg void OnLoad();
	afx_msg void OnLoadex();
	afx_msg void OnSaveas();
	afx_msg void OnNew();
	afx_msg void OnCheck();
	virtual void OnCancel();
	afx_msg void OnSave();
	afx_msg void OnSaveAs();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WFXEDIT_H__32A26AD9_A1D0_4620_B3F3_2957C4C5D5CD__INCLUDED_)
