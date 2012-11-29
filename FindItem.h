#if !defined(AFX_FINDITEM_H__CA281E21_DCE6_4B99_A0E7_64386CDB72D3__INCLUDED_)
#define AFX_FINDITEM_H__CA281E21_DCE6_4B99_A0E7_64386CDB72D3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FindItem.h : header file
//

#include "structs.h"

/////////////////////////////////////////////////////////////////////////////
// CFindItem dialog

class CFindItem : public CDialog
{
// Construction
public:
	CFindItem(CWnd* pParent = NULL);   // standard constructor

  unsigned long mask; //enable controls
  unsigned long flags;
  search_data searchdata;
  CString title;
  CString mtype_title;
  CString proj_title;
  CString item_title;
// Dialog Data
	//{{AFX_DATA(CFindItem)
	enum { IDD = IDD_FINDITEM };
	CButton	m_item_control;
	CButton	m_proj_control;
	CComboBox	m_change_control;
	CButton	m_mtype_control;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFindItem)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
  CString m_resource;
  CString m_itemname;
  CString m_variable;
  CString m_newres;

// Implementation
protected:
  void Refresh();
	// Generated message map functions
	//{{AFX_MSG(CFindItem)
	afx_msg void OnMproj();
	virtual BOOL OnInitDialog();
	afx_msg void OnMtype();
	afx_msg void OnKillfocusItemtype();
	afx_msg void OnKillfocusItemtype2();
	afx_msg void OnKillfocusProj();
	afx_msg void OnKillfocusProj2();
	afx_msg void OnKillfocusEffect();
	afx_msg void OnKillfocusEffect2();
	afx_msg void OnMfeat();
	afx_msg void OnMres();
	afx_msg void OnKillfocusResource();
	afx_msg void OnMitem();
	afx_msg void OnKillfocusItemname();
	afx_msg void OnKillfocusEa();
	afx_msg void OnKillfocusEa2();
	afx_msg void OnKillfocusGeneral();
	afx_msg void OnKillfocusGeneral2();
	afx_msg void OnKillfocusRace();
	afx_msg void OnKillfocusRace2();
	afx_msg void OnKillfocusClass();
	afx_msg void OnKillfocusClass2();
	afx_msg void OnKillfocusSpecific();
	afx_msg void OnKillfocusSpecific2();
	afx_msg void OnKillfocusGender();
	afx_msg void OnKillfocusGender2();
	afx_msg void OnKillfocusAlign();
	afx_msg void OnKillfocusAlign2();
	afx_msg void OnMea();
	afx_msg void OnMgeneral();
	afx_msg void OnMrace();
	afx_msg void OnMclass();
	afx_msg void OnMspecific();
	afx_msg void OnMgender();
	afx_msg void OnMalign();
	afx_msg void OnKillfocusVariable();
	afx_msg void OnMvar();
	afx_msg void OnKillfocusChange();
	afx_msg void OnKillfocusNewvalue();
	afx_msg void OnReplace();
	afx_msg void OnKillfocusStrref();
	afx_msg void OnMstrref();
	afx_msg void OnKillfocusNewresource();
	afx_msg void OnSelchangeChange();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FINDITEM_H__CA281E21_DCE6_4B99_A0E7_64386CDB72D3__INCLUDED_)
