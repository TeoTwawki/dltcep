#if !defined(AFX_CHUIEDIT_H__EDEBF614_15F6_4FD2_8173_617760A08A05__INCLUDED_)
#define AFX_CHUIEDIT_H__EDEBF614_15F6_4FD2_8173_617760A08A05__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChuiEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CChuiEdit dialog

class CChuiEdit : public CDialog
{
// Construction
public:
	CChuiEdit(CWnd* pParent = NULL);   // standard constructor
  void NewChui();

// Dialog Data
	//{{AFX_DATA(CChuiEdit)
	enum { IDD = IDD_CHUIEDIT };
	CComboBox	m_controlpicker;
	CComboBox	m_windowpicker;
	int		m_x;
	CString	m_text;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChuiEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  CToolTipCtrl m_tooltip;
  CString filepath;

  //refreshes control type specific edit controls
  void DisplayControls(int type);
  void RefreshControls(CDataExchange* pDX, int type, int position);
  //refreshes generic edit controls
  void RefreshDialog();
  void SaveChui(int save);  
  int GetCurrentControlType(int &pos2, char *&oldtype);
  int DeleteControls(int firstcontrol, int controlcount);
  void OnLabelFlag(int flag);
  short GetWindowId();

	afx_msg void DefaultKillfocus();
	// Generated message map functions
	//{{AFX_MSG(CChuiEdit)
	afx_msg void OnLoad();
	afx_msg void OnLoadex();
	afx_msg void OnSaveas();
	afx_msg void OnNew();
	afx_msg void OnCheck();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeWindowpicker();
	afx_msg void OnAddwin();
	afx_msg void OnDelwin();
	afx_msg void OnCopywin();
	afx_msg void OnSave();
	afx_msg void OnBrowse();
	afx_msg void OnBackground();
	afx_msg void OnSelchangeControlpicker();
	afx_msg void OnKillfocusType();
	afx_msg void OnAddctrl();
	afx_msg void OnDelctrl();
	afx_msg void OnBrowse2();
	afx_msg void OnBrowse3();
	afx_msg void OnPreview();
	afx_msg void OnBrowse4();
	afx_msg void OnBrowse5();
	afx_msg void OnBrowse6();
	afx_msg void OnKillfocusText();
	afx_msg void OnFileTbg();
	afx_msg void OnFileTp2();
	afx_msg void OnFlag1();
	afx_msg void OnFlag2();
	afx_msg void OnFlag3();
	afx_msg void OnFlag4();
	afx_msg void OnFlag5();
	afx_msg void OnFlag6();
	afx_msg void OnFlag7();
	afx_msg void OnFlag8();
	afx_msg void OnFlag9();
	afx_msg void OnKillfocusId2();
	afx_msg void OnShadow();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHUIEDIT_H__EDEBF614_15F6_4FD2_8173_617760A08A05__INCLUDED_)
