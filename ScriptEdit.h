#if !defined(AFX_SCRIPTEDIT_H__1216F756_9F12_4ABB_8F6F_7FC1A925260D__INCLUDED_)
#define AFX_SCRIPTEDIT_H__1216F756_9F12_4ABB_8F6F_7FC1A925260D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ScriptEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CScriptEdit dialog

class CScriptEdit : public CDialog
{
// Construction
public:
	CScriptEdit(CWnd* pParent = NULL);   // standard constructor
  void NewScript();

// Dialog Data
	//{{AFX_DATA(CScriptEdit)
	enum { IDD = IDD_SCRIPTEDIT };
	CRichEditCtrl	m_text_control;
	//}}AFX_DATA
  CString m_idsname;
  int m_indent;
  int m_bcs;
  int m_count, m_firsterror;
  int *m_errors;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScriptEdit)
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
  CFindReplaceDialog *m_searchdlg;
  int m_infind;
  
  long OnFindReplace(WPARAM wParam, LPARAM lParam);
  int perform_search_and_replace(int idx, int mode, int match, CString search, CString replace);
  void do_search_and_replace(int direction,int mode,int match,CString search,CString replace);
  int decompile(CString &filepath, CString tmpname);
  int compile(CString filepath);
  CString GetLine(int idx);
  void CheckScript(int messages);
  void RefreshDialog();
  void RefreshMenu();
  int RunWeidu(CString syscommand);
  void OnSaveasGeneral(int type);
  void OnSaveasBCS();
  void OnSaveasBAF();
	// Generated message map functions
	//{{AFX_MSG(CScriptEdit)
	virtual BOOL OnInitDialog();
	afx_msg void OnLoad();
	afx_msg void OnLoadex();
	afx_msg void OnSaveas();
	afx_msg void OnNew();
	afx_msg void OnSetfocusText();
	afx_msg void OnCheck();
	afx_msg void OnSaveAsBAF();
	afx_msg void OnSaveAsBCS();
	afx_msg void OnFileLoadsource();
	afx_msg void OnSearch();
	afx_msg void OnKillfocusText();
	afx_msg void OnMaxtextText();
	afx_msg void OnOptionsAutocheck();
	afx_msg void OnOptionsLargeindent();
	afx_msg void OnEditSelectall();
	afx_msg void OnEditUndo();
	afx_msg void OnEditCut();
	afx_msg void OnEditCopy();
	afx_msg void OnEditPaste();
	afx_msg void OnEditDelete();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnIDS();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnChangeText();
	afx_msg void OnUpdateText();
	virtual void OnCancel();
	afx_msg void OnToolsLookupstrref();
	afx_msg void OnOptionsUseinternalcompiler();
	afx_msg void OnOptionsUseinternaldecompiler();
	afx_msg void OnOptionsLogging();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCRIPTEDIT_H__1216F756_9F12_4ABB_8F6F_7FC1A925260D__INCLUDED_)
