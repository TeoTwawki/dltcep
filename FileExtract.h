#if !defined(AFX_FILEEXTRACT_H__AD932AED_3D7E_4320_B66E_222C14765E86__INCLUDED_)
#define AFX_FILEEXTRACT_H__AD932AED_3D7E_4320_B66E_222C14765E86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FileExtract.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFileExtract dialog

class CFileExtract : public CDialog
{
// Construction
public:
	CFileExtract(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFileExtract)
	enum { IDD = IDD_EXTRACT };
	CComboBox	m_bifname_control;
	CComboBox	m_filetype_control;
	CString	m_filemask;
	BOOL	m_override;
	//}}AFX_DATA
  unsigned short m_filetype;
	unsigned short m_bifidx;
  int m_num_extract;
  bool extract_or_search;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileExtract)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  CToolTipCtrl m_tooltip;

  void do_copy_file(CString key, CString ext, loc_entry fileloc);
  int extract_files(int restype, CString extension, CStringMapLocEntry &refs);

	// Generated message map functions
	//{{AFX_MSG(CFileExtract)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnCloseupBifname();
	afx_msg void OnKillfocusFilemask();
	afx_msg void OnOverride();
	afx_msg void OnSelchangeFiletype();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CFileExtract2 dialog

class CFileExtract2 : public CDialog
{
// Construction
public:
	CFileExtract2(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFileExtract2)
	enum { IDD = IDD_EXTRACT2 };
	CString	m_text;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileExtract2)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFileExtract2)
	afx_msg void OnYes();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CFileExtract3 dialog

class CFileExtract3 : public CDialog
{
// Construction
public:
	CFileExtract3(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFileExtract3)
	enum { IDD = IDD_SAVHANDLE };
	CButton	m_openfile;
	CComboBox	m_filetype_control;
	BOOL	m_override;
	CString	m_filename;
	CString	m_filemask;
	//}}AFX_DATA
  CString m_filetype;
	CString m_savname;
  int m_num_extract;
  bool cbf_or_sav;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileExtract3)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

//  int do_copy_file(CString key, CString ext, int finput, int &maxlen);
	// Generated message map functions
	//{{AFX_MSG(CFileExtract3)
	afx_msg void OnOpenfile();
	afx_msg void OnKillfocusFilemask();
	afx_msg void OnOverride();
	afx_msg void OnSelchangeFiletype();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILEEXTRACT_H__AD932AED_3D7E_4320_B66E_222C14765E86__INCLUDED_)
