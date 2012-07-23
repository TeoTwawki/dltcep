#if !defined(AFX_PROGRESSBAR_H__59E9C988_BFE7_4E0E_97A5_52F670E63924__INCLUDED_)
#define AFX_PROGRESSBAR_H__59E9C988_BFE7_4E0E_97A5_52F670E63924__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// progressbar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// progressbar dialog

#define PROGRESS_GRANULARITY    1023
#define PROGRESS_GRANULARITY_SM 63

class progressbar : public CDialog
{
// Construction
public:
	progressbar(CWnd* pParent = NULL);   // standard constructor
  void SetRange(int range, CString title);
  void SetProgress(int progr);

protected:
  int m_range;
  int m_gran;
// Dialog Data
	//{{AFX_DATA(progressbar)
	enum { IDD = IDD_PROGRESS };
	CProgressCtrl	m_progress_control;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(progressbar)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(progressbar)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// panicbutton dialog

class panicbutton : public CDialog
{
// Construction
public:
	panicbutton(CWnd* pParent = NULL);   // standard constructor
  bool waitsound;
  bool abort;

// Dialog Data
	//{{AFX_DATA(panicbutton)
	enum { IDD = IDD_PANICBUTTON };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(panicbutton)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(panicbutton)
	afx_msg void OnTimer(UINT nIDEvent);
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROGRESSBAR_H__59E9C988_BFE7_4E0E_97A5_52F670E63924__INCLUDED_)
