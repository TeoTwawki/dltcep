#if !defined(AFX_KEYEDIT_H__FD911D57_3F77_42E9_B2AA_BF3C9BC6D58C__INCLUDED_)
#define AFX_KEYEDIT_H__FD911D57_3F77_42E9_B2AA_BF3C9BC6D58C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KeyEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CKeyEdit dialog

class CKeyEdit : public CDialog
{
// Construction
public:
	CKeyEdit(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CKeyEdit)
	enum { IDD = IDD_KEYEDIT };
	CListCtrl	m_filelist_control;
	CListBox	m_biflist_control;
	CString	m_bifpos;
	CString	m_filepos;
	//}}AFX_DATA
  int m_bifindex;
  int m_biflocation;
  long m_bifsize;

  long OnFindReplace(WPARAM wParam, LPARAM lParam);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKeyEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  CFindReplaceDialog *m_searchdlg;
  CToolTipCtrl m_tooltip;

  void do_search_and_replace(CString filename);
  void StartFindReplace();
  void RefreshDialog();
  void RefreshBiflist();
  int GetCurSel();
  void Clicked(int mask); //clicked on a checkbox
  void AdjustBifindex(int index, int value);
  int WriteChitin(int fhandle);
  void CKeyEdit::AddResource(int type, int bifidx, int locidx, CString ref);
  int write_biff(int fhandle, int bifindex, CString folder);
  void refresh_list(int list_deleted);
  int has_change(int bifidx);
  void do_copy_file(CString dest, CString key, CString ext, loc_entry fileloc);
  int extract_files(CString dest, CString extension, CStringMapLocEntry &refs);
  void cleanup();
  void get_bifs(CString folder);
  void write_chitin(int fhandle);
  void RefreshEntryInfo();
	// Generated message map functions
	//{{AFX_MSG(CKeyEdit)
	afx_msg void OnSelchangeBiflist();
	afx_msg void OnSelchangeFilelist();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDeleteall();
	afx_msg void OnRemovebif();
	afx_msg void OnMark();
	afx_msg void OnFlag1();
	afx_msg void OnFlag2();
	afx_msg void OnFlag3();
	afx_msg void OnFlag4();
	afx_msg void OnFlag5();
	afx_msg void OnFlag6();
	afx_msg void OnFlag7();
	afx_msg void OnFlag8();
	afx_msg void OnReadd();
	afx_msg void OnReload();
	afx_msg void OnToolsExplode();
	afx_msg void OnToolsImplode();
	afx_msg void OnCheck();
	afx_msg void OnItemchangedFilelist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSearch();
	//}}AFX_MSG
  afx_msg void OnCustomdrawFilelist ( NMHDR* pNMHDR, LRESULT* pResult );  
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KEYEDIT_H__FD911D57_3F77_42E9_B2AA_BF3C9BC6D58C__INCLUDED_)
