#if !defined(AFX_ANIMDIALOG_H__D73F6F71_91CC_45C1_8CC2_88B34477EB5E__INCLUDED_)
#define AFX_ANIMDIALOG_H__D73F6F71_91CC_45C1_8CC2_88B34477EB5E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AnimDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAnimDialog dialog

class CAnimDialog : public CDialog
{
// Construction
public:
	CAnimDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAnimDialog)
	enum { IDD = IDD_ANIMDIALOG };
	CListCtrl	m_list_control;
	CString	m_filter;
	//}}AFX_DATA
  CString m_picked;
  int m_restype;
  int m_item, m_subitem;
  CImageList imagelist;
  int m_cnt;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAnimDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  void ResolveKey(CString &key, loc_entry &fileloc, int restype);
  void FillList();
  void GetItemImage(LV_DISPINFO *pDispInfo);
  void RemoveItemImage(unsigned int item);

	// Generated message map functions
	//{{AFX_MSG(CAnimDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnClickIconres(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGetdispinfoIconres(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusFilter();
	//}}AFX_MSG
  afx_msg void OnCustomdrawMyList(NMHDR *pNMHDR, LRESULT *pResult);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ANIMDIALOG_H__D73F6F71_91CC_45C1_8CC2_88B34477EB5E__INCLUDED_)
