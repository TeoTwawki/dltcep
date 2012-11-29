#if !defined(AFX_STRREFDLG_H__B5E0D187_9E65_4566_843F_C649D01EB2F2__INCLUDED_)
#define AFX_STRREFDLG_H__B5E0D187_9E65_4566_843F_C649D01EB2F2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StrRefDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CStrRefDlg dialog

class CStrRefDlg : public CDialog
{
  // Construction
public:
  CStrRefDlg(CWnd* pParent = NULL);   // standard constructor
  
  // Dialog Data
  //{{AFX_DATA(CStrRefDlg)
	enum { IDD = IDD_STRREFDLG };
	CSpinButtonCtrl	m_spin_control;
	CEdit	m_strref_control;
  CEdit	m_text_control;
  CString	m_text;
  CString	m_sound;
  BOOL	m_tag;
	CString	m_maxstr;
	//}}AFX_DATA
  
  long OnFindReplace(WPARAM wParam, LPARAM lParam);
  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CStrRefDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
  
  // Implementation
protected:
  int m_refresh;
  long m_strref;
  int m_mode;
  CFindReplaceDialog *m_searchdlg;
  CToolTipCtrl m_tooltip;

  int do_search_and_replace(int direction,int mode,int match,CString search,CString replace);
  void StartFindReplace(int mode);
	// Generated message map functions
	//{{AFX_MSG(CStrRefDlg)
	afx_msg void OnUpdateStrref();
	afx_msg void OnKillfocusText();
	afx_msg void OnBrowse();
	afx_msg void OnKillfocusSound();
	afx_msg void OnTag();
	afx_msg void OnSearch();
	afx_msg void OnPlay();
	virtual BOOL OnInitDialog();
	afx_msg void OnFind();
	afx_msg void OnSave();
	afx_msg void OnReload();
	afx_msg void OnTrim();
	afx_msg void OnClear();
	afx_msg void OnKillfocusStrref();
	afx_msg void OnChecksound();
	afx_msg void OnChecktag();
	afx_msg void OnToolsSynchronisetlks();
	afx_msg void OnCheckSpecialstrings();
	afx_msg void OnSearchWeirdcharacter();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STRREFDLG_H__B5E0D187_9E65_4566_843F_C649D01EB2F2__INCLUDED_)
