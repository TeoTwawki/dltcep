#if !defined(AFX_SRCEDIT_H__0B02863B_E61C_4175_A590_7E106B6DBF3D__INCLUDED_)
#define AFX_SRCEDIT_H__0B02863B_E61C_4175_A590_7E106B6DBF3D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SRCEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSRCEdit dialog

class CSRCEdit : public CDialog
{
// Construction
public:
	CSRCEdit(CWnd* pParent = NULL);   // standard constructor
  void NewSRC();

// Dialog Data
	//{{AFX_DATA(CSRCEdit)
	enum { IDD = IDD_SRCEDIT };
	CSpinButtonCtrl	m_spincontrol;
	CEdit	m_buddycontrol;
	CComboBox	m_slotpicker;
	CString	m_text;
	CString	m_cntstr;
	CString	m_soundref;
	long	m_ref;
	BOOL	m_tagged;
	//}}AFX_DATA
  int m_srccnt;
  int m_stringnum;
//  int *m_srcslots;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSRCEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
  CToolTipCtrl m_tooltip;
  CString filepath;

  void RefreshControl();
  void SaveSrc(int save);

	// Generated message map functions
	//{{AFX_MSG(CSRCEdit)
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
	afx_msg void OnRemove();
	afx_msg void OnPlay();
	afx_msg void OnLoad();
	afx_msg void OnLoadex();
	afx_msg void OnSaveas();
	afx_msg void OnNew();
	afx_msg void OnCheck();
	afx_msg void OnKillfocusRef();
	afx_msg void OnKillfocusSoundref();
	afx_msg void OnSave();
	afx_msg void OnKillfocusText();
	afx_msg void OnTagged();
	afx_msg void OnKillfocusSlotpicker();
	afx_msg void OnSelchangeSlotpicker();
	afx_msg void OnChangeBuddy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SRCEDIT_H__0B02863B_E61C_4175_A590_7E106B6DBF3D__INCLUDED_)
