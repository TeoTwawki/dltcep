#if !defined(AFX_VVCEDIT_H__EA7AC045_CA98_43DA_A52A_6E076FCF2EDE__INCLUDED_)
#define AFX_VVCEDIT_H__EA7AC045_CA98_43DA_A52A_6E076FCF2EDE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VVCEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CVVCEdit dialog

class CVVCEdit : public CDialog
{
// Construction
public:
	CVVCEdit(CWnd* pParent = NULL);   // standard constructor
  void NewVVC();

// Dialog Data
	//{{AFX_DATA(CVVCEdit)
	enum { IDD = IDD_VVCEDIT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVVCEdit)
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
  void SaveVVC(int save);

	afx_msg void DefaultKillfocus();
	// Generated message map functions
	//{{AFX_MSG(CVVCEdit)
	afx_msg void OnLoad();
	afx_msg void OnLoadex();
	afx_msg void OnSaveas();
	afx_msg void OnKillfocusSequencing();
	afx_msg void OnLoop();
	virtual BOOL OnInitDialog();
	afx_msg void OnPlay1();
	afx_msg void OnPlay2();
	afx_msg void OnBrowse();
	afx_msg void OnBrowse2();
	afx_msg void OnBrowse3();
	afx_msg void OnNew();
	afx_msg void OnTransparent();
	afx_msg void OnTint();
	afx_msg void OnGlow();
	afx_msg void OnGrey();
	afx_msg void OnUnknown();
	afx_msg void OnBrightest();
	afx_msg void OnCheck();
	afx_msg void OnSave();
	afx_msg void OnPos1();
	afx_msg void OnPos2();
	afx_msg void OnUsebam();
	afx_msg void OnU2();
	afx_msg void OnU3();
	afx_msg void OnMirror();
	afx_msg void OnMirror2();
	afx_msg void OnBlend();
	virtual void OnCancel();
	afx_msg void OnWall();
	afx_msg void OnRed();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VVCEDIT_H__EA7AC045_CA98_43DA_A52A_6E076FCF2EDE__INCLUDED_)
