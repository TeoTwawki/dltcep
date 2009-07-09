#if !defined(AFX_PROJEDIT_H__B5EA5F3F_D2B0_4020_A9AA_D7D582149246__INCLUDED_)
#define AFX_PROJEDIT_H__B5EA5F3F_D2B0_4020_A9AA_D7D582149246__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProjEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProjEdit dialog

class CProjEdit : public CDialog
{
  // Construction
public:
  CProjEdit(CWnd* pParent = NULL);   // standard constructor
  void NewProjectile();
  // Dialog Data
  //{{AFX_DATA(CProjEdit)
	enum { IDD = IDD_PROJEDIT };
	CStatic	m_mpsmoke;
	CStatic	m_mptravel;
	//}}AFX_DATA
  int m_smokecoloridx, m_bamcoloridx;
  HBITMAP hbs, hbb;
  
  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CProjEdit)
	public:
  virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL
//  CPoint mousepoint;
//  CPoint mypoint;
  
  // Implementation
protected:
  CToolTipCtrl m_tooltip;
  
  void SaveProj(int save);
  void RefreshDialog();
  void InitPaletteBitmap(int paletteid, char *gradients);
  void RefreshSmoke(int idx);
  void RefreshTravel(int idx);
  
	afx_msg void DefaultKillfocus();
	// Generated message map functions
	//{{AFX_MSG(CProjEdit)
	afx_msg void OnLoad();
	afx_msg void OnLoadex();
	afx_msg void OnSaveas();
	virtual BOOL OnInitDialog();
	afx_msg void OnNew();
	afx_msg void OnFlag1();
	afx_msg void OnFlag2();
	afx_msg void OnBrowse1();
	afx_msg void OnPlay1();
	afx_msg void OnPlay2();
	afx_msg void OnPlay3();
	afx_msg void OnBrowse2();
	afx_msg void OnBrowse3();
	afx_msg void OnBrowse4();
	afx_msg void OnBrowse5();
	afx_msg void OnBrowse6();
	afx_msg void OnAttr1();
	afx_msg void OnAttr2();
	afx_msg void OnAttr3();
	afx_msg void OnAttr4();
	afx_msg void OnAttr5();
	afx_msg void OnAttr6();
	afx_msg void OnAttr7();
	afx_msg void OnAttr8();
	afx_msg void OnMpsmoke();
	afx_msg void OnMptravel();
	afx_msg void OnAttr9();
	afx_msg void OnAttr10();
	afx_msg void OnAttr11();
	afx_msg void OnAttr12();
	afx_msg void OnAttr13();
	afx_msg void OnAttr14();
	afx_msg void OnAttr15();
	afx_msg void OnAttr16();
	afx_msg void OnSelchangeType();
	afx_msg void OnExtension();
	afx_msg void OnUnknown();
	afx_msg void OnUnknown9();
	afx_msg void OnUnknown154();
	afx_msg void OnFlag3();
	afx_msg void OnCheck();
	afx_msg void OnSave();
	afx_msg void OnUnknown2c();
	afx_msg void OnSetfocusU1();
	afx_msg void OnSetfocusU2();
	afx_msg void OnSetfocusU3();
	afx_msg void OnSetfocusU4();
	afx_msg void OnSetfocusU5();
	afx_msg void OnSetfocusU6();
	afx_msg void OnSetfocusU7();
	afx_msg void OnSetfocusU11();
	afx_msg void OnSetfocusU12();
	afx_msg void OnSetfocusU13();
	afx_msg void OnSetfocusU14();
	afx_msg void OnSetfocusU15();
	afx_msg void OnSetfocusU16();
	afx_msg void OnSetfocusU17();
	afx_msg void OnFlag5();
	afx_msg void OnBrowse7();
	afx_msg void OnBrowse8();
	afx_msg void OnBrowse9();
	virtual void OnCancel();
	afx_msg void OnFlag4();
	afx_msg void OnFileTbg();
	afx_msg void OnFileTp2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CProjExt dialog

class CProjExt : public CDialog
{
// Construction
public:
	CProjExt(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CProjExt)
	enum { IDD = IDD_PROJEXT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProjExt)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  CToolTipCtrl m_tooltip;

	afx_msg void DefaultKillfocus();
	// Generated message map functions
	//{{AFX_MSG(CProjExt)
	afx_msg void OnAttr1();
	afx_msg void OnAttr2();
	afx_msg void OnAttr3();
	afx_msg void OnAttr4();
	afx_msg void OnAttr5();
	afx_msg void OnAttr6();
	afx_msg void OnAttr7();
	afx_msg void OnAttr8();
	afx_msg void OnAttr9();
	afx_msg void OnAttr10();
	afx_msg void OnAttr11();
	afx_msg void OnAttr12();
	afx_msg void OnAttr13();
	afx_msg void OnAttr14();
	afx_msg void OnAttr15();
	afx_msg void OnAttr16();
	virtual BOOL OnInitDialog();
	afx_msg void OnBrowse1();
	afx_msg void OnPlay1();
	afx_msg void OnBrowse();
	afx_msg void OnBrowse2();
	afx_msg void OnUnknown();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROJEDIT_H__B5EA5F3F_D2B0_4020_A9AA_D7D582149246__INCLUDED_)
