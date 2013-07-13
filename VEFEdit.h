#if !defined(AFX_VEFEDIT_H__350FC80E_595A_43EE_B34F_161853D5FF72__INCLUDED_)
#define AFX_VEFEDIT_H__350FC80E_595A_43EE_B34F_161853D5FF72__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VEFEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CVEFEdit dialog

class CVEFEdit : public CDialog
{
// Construction
public:
	CVEFEdit(CWnd* pParent = NULL);   // standard constructor
  void NewVEF();

// Dialog Data
	//{{AFX_DATA(CVEFEdit)
	enum { IDD = IDD_VEFEDIT };
	CComboBox	m_type2_control;
	CComboBox	m_type1_control;
	CComboBox	m_group2_control;
	CComboBox	m_group1_control;
	//}}AFX_DATA
  int group1num;
  int group2num;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVEFEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  CToolTipCtrl m_tooltip;

  void RefreshDialog();
  void checkflags(int *boxids, int value);
  void SaveVEF(int save);
  int DeleteComponent(vef_component *&oldcomp, int pos, int &max);
  int InsertComponent(vef_component *&oldcomp, int pos, int &max);
	// Generated message map functions
	//{{AFX_MSG(CVEFEdit)
	afx_msg void OnLoad();
	afx_msg void OnLoadex();
	afx_msg void OnSaveas();
	virtual BOOL OnInitDialog();
	afx_msg void OnNew();
	afx_msg void OnCheck();
	afx_msg void OnSave();
	virtual void OnCancel();
	afx_msg void OnRemove();
	afx_msg void OnAdd();
	afx_msg void OnRemove2();
	afx_msg void OnAdd2();
	afx_msg void OnDefaultKillfocus();
	afx_msg void OnKillfocusGroup1();
	afx_msg void OnSelchangeGroup1();
	afx_msg void OnKillfocusGroup2();
	afx_msg void OnSelchangeGroup2();
	afx_msg void OnContinuous();
	afx_msg void OnContinuous2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VEFEDIT_H__350FC80E_595A_43EE_B34F_161853D5FF72__INCLUDED_)
