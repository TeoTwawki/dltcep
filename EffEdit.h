#if !defined(AFX_EFFEDIT_H__26DB259A_FAB2_4B68_A981_6EDCD5FB29F3__INCLUDED_)
#define AFX_EFFEDIT_H__26DB259A_FAB2_4B68_A981_6EDCD5FB29F3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EffEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEffEdit dialog

class CEffEdit : public CDialog
{
// Construction
public:
	CEffEdit(CWnd* pParent = NULL);   // standard constructor
  void NewEff();
  void SetLimitedEffect(int arg);
  void SetDefaultDuration(int arg);

// Dialog Data
	//{{AFX_DATA(CEffEdit)
	enum { IDD = IDD_EFFEDIT };
	CEdit	m_text2_control;
	CComboBox	m_sectype_control;
	CComboBox	m_school_control;
	CComboBox	m_resist_control;
	CEdit	m_text_control;
	CComboBox	m_efftarget_control;
	CComboBox	m_timing_control;
	CComboBox	m_stype_control;
	CComboBox	m_effopcode_control;
	CString	m_durationlabel;
	CString	m_param1;
	CString	m_param2;
	BYTE	m_param1b1;
	BYTE	m_param1b2;
	BYTE	m_param1b3;
	BYTE	m_param1b4;
	BYTE	m_param2b1;
	BYTE	m_param2b2;
	BYTE	m_param2b3;
	BYTE	m_param2b4;
	CString	m_text;
	CString	m_text2;
	//}}AFX_DATA
  CString m_idsname;
  CString m_opcode;
  int m_par_type;
  int m_hexadecimal;
  int m_limitedeffect;
  int m_defduration;
  CIntMapInt m_efftext;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEffEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  CToolTipCtrl m_tooltip;
  CString filepath;

  void RefreshDialog();
  void UpdateTooltip();
  void Explode(int *boxids, int parid, int buttonid);
  void SaveEff(int save);
 	afx_msg void DefaultKillfocus();
	// Generated message map functions
	//{{AFX_MSG(CEffEdit)
	virtual BOOL OnInitDialog();
	afx_msg void OnLoad();
	afx_msg void OnLoadex();
	afx_msg void OnSaveas();
	afx_msg void OnKillfocusEffopcode();
	afx_msg void OnKillfocusPar1();
	afx_msg void OnKillfocusPar2();
	afx_msg void OnSelchangeEffopcode();
	afx_msg void OnParUndefined();
	afx_msg void OnParColor();
	afx_msg void OnParIds();
	afx_msg void OnParDamage();
	afx_msg void OnExplode1();
	afx_msg void OnExplode2();
	afx_msg void OnKillfocusPar1b1();
	afx_msg void OnKillfocusPar1b2();
	afx_msg void OnKillfocusPar1b3();
	afx_msg void OnKillfocusPar1b4();
	afx_msg void OnKillfocusPar2b1();
	afx_msg void OnKillfocusPar2b2();
	afx_msg void OnKillfocusPar2b3();
	afx_msg void OnKillfocusPar2b4();
	afx_msg void OnBrowse();
	afx_msg void OnBrowse2();
	afx_msg void OnNew();
	afx_msg void OnCheck();
	afx_msg void OnSave();
	afx_msg void OnFileTbg();
	afx_msg void OnFileTp2();
	afx_msg void OnKillfocusText();
	afx_msg void OnPlay();
	afx_msg void OnBrowse3();
	afx_msg void OnBrowse4();
	afx_msg void OnToolsDuration();
	afx_msg void OnKillfocusText2();
	afx_msg void OnToolsIdsbrowser();
	afx_msg void OnDefaultKillfocus();
	afx_msg void OnHexadecimal();
	afx_msg void OnMirror();
	afx_msg void OnDiff();
	afx_msg void OnParAnim();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EFFEDIT_H__26DB259A_FAB2_4B68_A981_6EDCD5FB29F3__INCLUDED_)
