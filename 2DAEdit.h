#if !defined(AFX_2DAEDIT_H__A659E223_F381_47CD_9D75_C923A23E39C6__INCLUDED_)
#define AFX_2DAEDIT_H__A659E223_F381_47CD_9D75_C923A23E39C6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// 2DAEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// C2DAEdit dialog

class C2DAEdit : public CDialog
{
// Construction
public:
	C2DAEdit(CWnd* pParent = NULL);   // standard constructor
  void New2DA();

// Dialog Data
	//{{AFX_DATA(C2DAEdit)
	enum { IDD = IDD_2DAEDIT };
	CListCtrl	m_2da_control;
	BOOL	m_integer;
	//}}AFX_DATA
  CEdit m_edit;
  int m_inedit;
  int m_item, m_subitem;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(C2DAEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  CToolTipCtrl m_tooltip;

  void RefreshDialog();
  void RClick();
  void Click(int flg);
  void SaveTable(int save);
	afx_msg void DefaultKillfocus();
	// Generated message map functions
	//{{AFX_MSG(C2DAEdit)
	afx_msg void OnLoad();
	afx_msg void OnLoadex();
	afx_msg void OnSaveas();
	afx_msg void OnNew();
	virtual BOOL OnInitDialog();
	afx_msg void OnInt();
	afx_msg void OnEndlabeledit2da(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClick2da(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclick2da(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRow();
	afx_msg void OnColumn();
	afx_msg void OnRemove();
	afx_msg void OnRemove2();
	afx_msg void OnRclick2da(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCopy2();
	afx_msg void OnCopy();
	afx_msg void OnPaste2();
	afx_msg void OnPaste();
	afx_msg void OnColumn2();
	afx_msg void OnRow2();
	afx_msg void OnSave();
	afx_msg void OnToolsCapitalize();
	afx_msg void OnToolsLookupstrref();
	//}}AFX_MSG
	afx_msg void OnKillfocusEditlink();
  afx_msg void OnCustomdrawMyList ( NMHDR* pNMHDR, LRESULT* pResult );
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CIDSEdit dialog

class CIDSEdit : public CDialog
{
// Construction
public:
	CIDSEdit(CWnd* pParent = NULL);   // standard constructor
  void NewIDS();
  void SetReadOnly(int arg) { m_readonly=arg; }

// Dialog Data
	//{{AFX_DATA(CIDSEdit)
	enum { IDD = IDD_IDSEDIT };
	CListCtrl	m_ids_control;
	BOOL	m_hex;
	BOOL	m_four;
	//}}AFX_DATA
  CEdit m_edit;
  BOOL m_inedit;
  int m_item, m_subitem;
  int m_readonly;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIDSEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  CToolTipCtrl m_tooltip;

  void RefreshDialog();
  void SetHex();
  void SaveIDS(int save);

	// Generated message map functions
	//{{AFX_MSG(CIDSEdit)
	afx_msg void OnLoad();
	afx_msg void OnLoadex();
	afx_msg void OnNew();
	afx_msg void OnSaveas();
	afx_msg void OnRow();
	virtual BOOL OnInitDialog();
	afx_msg void OnClickIds(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeleditIds(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnInt();
	afx_msg void OnOrder();
	afx_msg void OnRemove();
	afx_msg void OnFour();
	afx_msg void OnSave();
	//}}AFX_MSG
	afx_msg void OnKillfocusEditlink();
  afx_msg void OnCustomdrawMyList ( NMHDR* pNMHDR, LRESULT* pResult );
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CMUSEdit dialog

class CMUSEdit : public CDialog
{
// Construction
public:
	CMUSEdit(CWnd* pParent = NULL);   // standard constructor
  void NewMUS();

// Dialog Data
	//{{AFX_DATA(CMUSEdit)
	enum { IDD = IDD_MUSEDIT };
	CListCtrl	m_mus_control;
	//}}AFX_DATA
  CEdit m_edit;
  BOOL m_inedit;
  int m_item, m_subitem;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMUSEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  CToolTipCtrl m_tooltip;

  void RefreshDialog();
  int RClick();
  void Click();

	// Generated message map functions
	//{{AFX_MSG(CMUSEdit)
	afx_msg void OnRemove();
	afx_msg void OnRow();
	afx_msg void OnLoad();
	afx_msg void OnLoadex();
	afx_msg void OnSaveas();
	afx_msg void OnNew();
	virtual BOOL OnInitDialog();
	afx_msg void OnClickMus(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeleditMus(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickMus(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPlaywhole();
	//}}AFX_MSG
	afx_msg void OnKillfocusEditlink();
  afx_msg void OnCustomdrawMyList ( NMHDR* pNMHDR, LRESULT* pResult );
	DECLARE_MESSAGE_MAP()
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_2DAEDIT_H__A659E223_F381_47CD_9D75_C923A23E39C6__INCLUDED_)
