#if !defined(AFX_COMPAT_H__78B49F8C_9208_4189_BEE4_E531575D5D9D__INCLUDED_)
#define AFX_COMPAT_H__78B49F8C_9208_4189_BEE4_E531575D5D9D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// compat.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// Ccompat dialog

#define COM_DLG   1   //old style dlg
#define COM_PST   2   //planescape torment var
#define COM_BG1   4   //bg1 area
#define COM_IWD2  8   //iwd2 structures
#define COM_ITEM  16
#define COM_NOZIP 32
#define COM_NOWP  64
#define COM_XPL   128 //xplist (in iwd/iwd2)
#define COM_DLTC  256
#define COM_TOB   512

#define BOTHDIALOG 0x10000

class Ccompat : public CDialog
{
// Construction
public:
	Ccompat(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(Ccompat)
	enum { IDD = IDD_COMPAT };
	CComboBox	m_logtype_control;
	CComboBox	m_setupname_control;
	CComboBox	m_gametype_control;
	CEdit	m_filename_control;
	CEdit	m_descpath_control;
	CButton	m_openfile;
	CButton	m_descopen;
	CString	m_descpath;
	CString	m_filename;
	CString	m_setupname;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Ccompat)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  CToolTipCtrl m_tooltip;

  void Refresh();
  void Transfer();
	// Generated message map functions
	//{{AFX_MSG(Ccompat)
	afx_msg void OnOpenfile();
	virtual BOOL OnInitDialog();
	afx_msg void OnDescopen();
	afx_msg void OnSave();
	afx_msg void OnPstvar();
	afx_msg void OnDlg30();
	afx_msg void OnSelchangeGametype();
	afx_msg void OnBg1area();
	afx_msg void OnIwd2();
	afx_msg void OnAdd();
	afx_msg void OnRemove();
	afx_msg void OnKillfocusSetupname();
	afx_msg void OnSelchangeSetupname();
	afx_msg void OnTooltip();
	afx_msg void OnProgress();
	afx_msg void OnKillfocusLogtype();
	afx_msg void OnOptions();
	afx_msg void OnReadonly();
	afx_msg void OnUnlimit();
	afx_msg void OnNozip();
	afx_msg void OnWeapon();
	afx_msg void OnXplist();
	afx_msg void OnOptions2();
	afx_msg void OnOptions1();
	afx_msg void OnDialog();
	afx_msg void OnTob();
	afx_msg void OnDltc();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMPAT_H__78B49F8C_9208_4189_BEE4_E531575D5D9D__INCLUDED_)
