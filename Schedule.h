#if !defined(AFX_SCHEDULE_H__93FB8CBA_B2C3_4F11_AD99_175E014D57B1__INCLUDED_)
#define AFX_SCHEDULE_H__93FB8CBA_B2C3_4F11_AD99_175E014D57B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Schedule.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSchedule dialog

class CSchedule : public CDialog
{
// Construction
public:
	CSchedule(CWnd* pParent = NULL);   // standard constructor
  CString GetCaption();
  CString GetTimeOfDay();
// Dialog Data
	//{{AFX_DATA(CSchedule)
	enum { IDD = IDD_SCHEDULE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSchedule)
protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL
  
public:
  unsigned long m_schedule;
  unsigned long m_default;

  // Implementation
protected:
  
	// Generated message map functions
	//{{AFX_MSG(CSchedule)
	afx_msg void OnNight();
	afx_msg void OnTwilight();
	afx_msg void OnNone();
	afx_msg void OnAll();
	afx_msg void OnDawn();
	afx_msg void OnDay();
	afx_msg void OnMorning();
	afx_msg void OnAfternoon();
	afx_msg void OnCopy();
	afx_msg void OnPaste();
	afx_msg void OnHour1();
	afx_msg void OnHour2();
	afx_msg void OnHour3();
	afx_msg void OnHour4();
	afx_msg void OnHour5();
	afx_msg void OnHour6();
	afx_msg void OnHour7();
	afx_msg void OnHour8();
	afx_msg void OnHour9();
	afx_msg void OnHour10();
	afx_msg void OnHour11();
	afx_msg void OnHour12();
	afx_msg void OnHour13();
	afx_msg void OnHour14();
	afx_msg void OnHour15();
	afx_msg void OnHour16();
	afx_msg void OnHour17();
	afx_msg void OnHour18();
	afx_msg void OnHour19();
	afx_msg void OnHour20();
	afx_msg void OnHour21();
	afx_msg void OnHour22();
	afx_msg void OnHour23();
	afx_msg void OnHour24();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCHEDULE_H__93FB8CBA_B2C3_4F11_AD99_175E014D57B1__INCLUDED_)
