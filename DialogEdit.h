#if !defined(AFX_DIALOGEDIT_H__8ABD4F86_1932_4170_98B1_90F9190C47DC__INCLUDED_)
#define AFX_DIALOGEDIT_H__8ABD4F86_1932_4170_98B1_90F9190C47DC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogEdit.h : header file
//

#include "SearchNode.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogEdit dialog

//typedef HTREEITEM TRANSSTATE[2]; //reply/text

class leaf_data : public CMap<HTREEITEM, HTREEITEM, int, int&>
{
public:
  HTREEITEM FindStTrigger(int findvalue)
  {
    HTREEITEM key;
    int value;
    POSITION pos;

    pos=GetStartPosition();
    while(pos)
    {
      GetNextAssoc(pos,key,value);
      if(value<0 || value>=the_dialog.statecount) continue;
      if(the_dialog.dlgstates[value].stindex==findvalue) return key;
    }
    return (HTREEITEM) 0;
  }

  HTREEITEM FindTrTrigger(int findvalue)
  {
    HTREEITEM key;
    int value;
    POSITION pos;

    pos=GetStartPosition();
    while(pos)
    {
      GetNextAssoc(pos,key,value);
      if(value<0 || value>=the_dialog.transcount) continue;
      if(the_dialog.dlgtrans[value].trtrindex==findvalue) return key;
    }
    return (HTREEITEM) 0;
  }

  HTREEITEM FindAction(int findvalue)
  {
    HTREEITEM key;
    int value;
    POSITION pos;

    pos=GetStartPosition();
    while(pos)
    {
      GetNextAssoc(pos,key,value);
      if(value<0 || value>=the_dialog.transcount) continue;
      if(the_dialog.dlgtrans[value].actionindex==findvalue) return key;
    }
    return (HTREEITEM) 0;
  }

  HTREEITEM FindElement(int findvalue)
  {
    HTREEITEM key;
    int value;
    POSITION pos;

    pos=GetStartPosition();
    while(pos)
    {
      GetNextAssoc(pos,key,value);
      if(findvalue==value) return key;
    }
    return (HTREEITEM) 0;
  }
};

class CDialogEdit : public CDialog
{
// Construction
public:
	CDialogEdit(CWnd* pParent = NULL);   // standard constructor
  void NewDialog();
// Dialog Data
	//{{AFX_DATA(CDialogEdit)
	enum { IDD = IDD_DIALOGEDIT };
	CEdit	m_text_control;
	CButton	m_version_control;
	CEdit	m_freeze_control;
	CButton	m_journal_control;
	CButton	m_condition_control;
	CButton	m_action_control;
	CButton	m_hasjournal;
	CButton	m_hascondition;
	CButton	m_hasaction;
	CButton	m_hasoption;
	CComboBox	m_jourtype_control;
	CButton	m_tagcontrol;
	CTreeCtrl	m_dialogtree;
	CString	m_label;
	CString	m_text;
	int		m_strref;
	CString	m_sound;
	CString	m_dialog;
	int		m_value;
	//}}AFX_DATA
  //BOOL m_syntaxerror;
  HTREEITEM m_currentselection;
  HTREEITEM m_currentselection_new;
  int m_activesection;
  leaf_data m_treestates;  //the state ID's of the dialog tree
  leaf_data m_transstates; //the transitions (PC responses)

protected:
   CToolTipCtrl m_tooltip;
   CSearchNode m_searchdlg;
   char *m_treeflags;
   char *m_transflags;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  int SaveDialog(CString filepath, CString newname);
  void RefreshDialog(int stateidx, int expandselection=0);
  int AddTransBranch(HTREEITEM ht, int transitionidx, int transitioncount, int level);
  void DisplayText();
  void RetrieveText();
  void EnableWindows(int enableflags, int actualflags);
  void RefreshMenu();
  void RefreshLeaf();
  void CheckTriggerOrAction(int messages);
  void WalkOnTree(int command);
  void SearchNode();
  int SeekTo(int startnode, int type);
  int RunWeidu(CString syscommand);
  int pstize(int reference);
	// Generated message map functions
	//{{AFX_MSG(CDialogEdit)
	afx_msg void OnSelchangedDialogtree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTagged();
	afx_msg void OnKillfocusJourtype();
	afx_msg void OnHasoption();
	afx_msg void OnHascondition();
	afx_msg void OnHasaction();
	afx_msg void OnHasjournal();
	afx_msg void OnKillfocusFlags();
	afx_msg void OnOption();
	afx_msg void OnCondition();
	afx_msg void OnAction();
	afx_msg void OnJournal();
	afx_msg void OnLoad();
	afx_msg void OnLoadex();
	afx_msg void OnSaveas();
	virtual BOOL OnInitDialog();
	afx_msg void OnVersion();
	afx_msg void OnKillfocusText();
	afx_msg void OnKillfocusStrref();
	afx_msg void OnKillfocusValue();
	afx_msg void OnVscrollText();
	afx_msg void OnKillfocusSound();
	afx_msg void OnBrowse();
	afx_msg void OnPlay();
	afx_msg void OnKillfocusDialog();
	afx_msg void OnSetfocusText();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnFollowlink();
	afx_msg void OnDblclkDialogtree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGetinfotipDialogtree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRefresh();
	afx_msg void OnExpand();
	afx_msg void OnCollapse();
	afx_msg void OnSearchnode();
	afx_msg void OnRemove();
	afx_msg void OnAddnode();
	afx_msg void OnConvert();
	afx_msg void OnNew();
	afx_msg void OnSwapnodes();
	afx_msg void OnIncreaseweight();
	afx_msg void OnToolsorder();
	afx_msg void OnToolsorder2();
	afx_msg void OnAddtop();
	afx_msg void OnExportWeidu();
	afx_msg void OnImportWeidu();
	afx_msg void OnImportTbg();
	afx_msg void OnExportTbg();
	afx_msg void OnSave();
	afx_msg void OnCheck();
	afx_msg void OnToolspst();
	virtual void OnCancel();
	afx_msg void OnNewvalue();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGEDIT_H__8ABD4F86_1932_4170_98B1_90F9190C47DC__INCLUDED_)
