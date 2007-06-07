#if !defined(AFX_TORMENTCRE_H__DEDF7016_143E_43A9_8843_9FD37E7D46C5__INCLUDED_)
#define AFX_TORMENTCRE_H__DEDF7016_143E_43A9_8843_9FD37E7D46C5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TormentCre.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTormentCre dialog

class CTormentCre : public CDialog
{
// Construction
public:
	CTormentCre(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTormentCre)
	enum { IDD = IDD_CREATUREPST };
	CString	m_beast;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTormentCre)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  CToolTipCtrl m_tooltip;

  void ColorDlg(int idx);

	// Generated message map functions
	//{{AFX_MSG(CTormentCre)
	virtual BOOL OnInitDialog();
	afx_msg void OnDefaultKillfocus();
	afx_msg void OnBrowse1();
	afx_msg void OnBrowse2();
	afx_msg void OnBrowse3();
	afx_msg void OnBrowse4();
	afx_msg void OnBrowse5();
	afx_msg void OnBrowse6();
	afx_msg void OnBrowse7();
	afx_msg void OnOverlay();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TORMENTCRE_H__DEDF7016_143E_43A9_8843_9FD37E7D46C5__INCLUDED_)
