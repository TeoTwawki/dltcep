#if !defined(AFX_VARIABLE_H__158B8F43_B146_4713_89BF_3424CA56E94A__INCLUDED_)
#define AFX_VARIABLE_H__158B8F43_B146_4713_89BF_3424CA56E94A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// variable.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// Cvariable dialog

void save_variables(HWND hWnd, CStringMapInt &vars);
void load_variables(HWND hWnd, int verbose, int novalue, CStringMapInt &vars);

class Cvariable : public CDialog
{
// Construction
public:
	Cvariable(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(Cvariable)
	enum { IDD = IDD_VARIABLES };
	CEdit	m_area_control;
	CComboBox	m_varpicker_control;
	int	m_value;
	CString	m_varpicker;
	CString	m_area;
	//}}AFX_DATA
  CString m_prefix;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Cvariable)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  CStringMapInt localvars;

  void RefreshVarpicker();
  void PositionHead();
	// Generated message map functions
	//{{AFX_MSG(Cvariable)
	afx_msg void OnKillfocusValue();
	afx_msg void OnRemove();
	afx_msg void OnAdd();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnVtype();
	afx_msg void OnVtype2();
	afx_msg void OnVType3();
	afx_msg void OnKillfocusVarpicker();
	afx_msg void OnSelchangeVarpicker();
	afx_msg void OnKillfocusArea();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VARIABLE_H__158B8F43_B146_4713_89BF_3424CA56E94A__INCLUDED_)
