#if !defined(AFX_GAMEEDIT_H__4D7C0663_22AE_4A5C_AC79_31D7E9CE7A33__INCLUDED_)
#define AFX_GAMEEDIT_H__4D7C0663_22AE_4A5C_AC79_31D7E9CE7A33__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GameEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGameEdit dialog

class CGameEdit : public CDialog
{
// Construction
public:
	CGameEdit(CWnd* pParent = NULL);   // standard constructor
  void NewGame();

// Dialog Data
	//{{AFX_DATA(CGameEdit)
	enum { IDD = IDD_GAMEEDIT };
	CComboBox	m_creaturenum_control;
	CComboBox	m_facepicker;
	CComboBox	m_familiarpicker;
	CComboBox	m_journalpicker;
	CComboBox	m_dvarpicker;
	CComboBox	m_variablepicker;
	CComboBox	m_npcpicker;
	CString	m_original;
	CString	m_revision;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGameEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  CToolTipCtrl m_tooltip;
  CString filepath;

  void RefreshDialogPartial();
  void RefreshDialog();
  void SaveGame(int save);
  int GetActualPosition(CComboBox &cb);
  void DeleteVariable(int pos);
  void DeleteVariable2(int pos);

	// Generated message map functions
	//{{AFX_MSG(CGameEdit)
	afx_msg void OnLoad();
	afx_msg void OnLoadex();
	afx_msg void OnSaveas();
	virtual BOOL OnInitDialog();
	afx_msg void OnNew();
	afx_msg void OnKillfocusNpcpicker();
	afx_msg void OnSelchangeNpcpicker();
	afx_msg void OnKillfocusArea();
	afx_msg void OnKillfocusCreres();
	afx_msg void OnKillfocusXpos();
	afx_msg void OnKillfocusYpos();
	afx_msg void OnBrowse1();
	afx_msg void OnBrowse2();
	afx_msg void OnAddnpc();
	afx_msg void OnDelnpc();
	afx_msg void OnKillfocusVariablepicker();
	afx_msg void OnSelchangeVariablepicker();
	afx_msg void OnKillfocusValue();
	afx_msg void OnDelvar();
	afx_msg void OnAddvar();
	afx_msg void OnKillfocusVariablepicker2();
	afx_msg void OnSelchangeVariablepicker2();
	afx_msg void OnKillfocusValue2();
	afx_msg void OnDelvar2();
	afx_msg void OnAddvar2();
	afx_msg void OnKillfocusFamiliarpicker();
	afx_msg void OnSelchangeFamiliarpicker();
	afx_msg void OnKillfocusFamiliar();
	afx_msg void OnKillfocusJournalpicker();
	afx_msg void OnSelchangeJournalpicker();
	afx_msg void OnKillfocusStrref();
	afx_msg void OnAddjournal();
	afx_msg void OnDeljournal();
	afx_msg void OnJoinable();
	afx_msg void OnParty();
	afx_msg void OnVariables();
	afx_msg void OnDvar();
	afx_msg void OnJournal();
	afx_msg void OnKillfocusVarname();
	afx_msg void OnKillfocusVarname2();
	afx_msg void OnKillfocusFace();
	afx_msg void OnKillfocusChapter();
	afx_msg void OnKillfocusTime();
	afx_msg void OnKillfocusSection();
	afx_msg void OnCheck();
	afx_msg void OnEdit();
	afx_msg void OnKillfocusCreaturenum();
	afx_msg void OnSave();
	afx_msg void OnEditlink();
	afx_msg void OnEdit2();
	afx_msg void OnEditlink2();
	afx_msg void OnGeneral();
	afx_msg void OnPCData();
	afx_msg void OnEditblock2();
	afx_msg void OnFix();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GAMEEDIT_H__4D7C0663_22AE_4A5C_AC79_31D7E9CE7A33__INCLUDED_)
