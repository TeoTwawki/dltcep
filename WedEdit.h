#if !defined(AFX_WEDEDIT_H__2CDF2725_6846_49AC_912D_3245A293816F__INCLUDED_)
#define AFX_WEDEDIT_H__2CDF2725_6846_49AC_912D_3245A293816F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WedEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWedEdit dialog

class CWedEdit : public CDialog
{
// Construction
public:
	CWedEdit(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWedEdit)
	enum { IDD = IDD_WEDEDIT };
	CComboBox	m_wallpolypicker_control;
	CComboBox	m_doorpolypicker_control;
	CComboBox	m_wallgrouppicker_control;
	CComboBox	m_overlaypicker_control;
	CComboBox	m_doorpicker_control;
	CString	m_maxoverlay;
	CString	m_maxdoor;
	CString	m_maxwallgroup;
	CString	m_maxdoorpoly;
	BOOL	m_open;
	CString	m_polygontype;
	CString	m_problem;
	BOOL	m_whole;
	//}}AFX_DATA
  int m_overlaynum;
  int m_doornum;
  int m_wallgroupnum;
  int m_doorpolynum;
  int m_wallpolynum;
  bool m_repair;
  area_vertex *m_polygoncopy;
  int m_polygonsize;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWedEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
  CToolTipCtrl m_tooltip;

  void RefreshOverlay();
  void RefreshWed();
  //returns true if tile is fully transparent
  int HandleOverlay(int x, int y, area_vertex *polygon, int size, int tile);
  //returns true if tile is fully opaque
  int RemoveOverlay(int x, int y, area_vertex *polygon, int size, int tile, int original);

	// Generated message map functions
	//{{AFX_MSG(CWedEdit)
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusOverlaypicker();
	afx_msg void OnSelchangeOverlaypicker();
	afx_msg void OnKillfocusTileset();
	afx_msg void OnKillfocusWidth();
	afx_msg void OnKillfocusHeight();
	afx_msg void OnKillfocusUnknown0c();
	afx_msg void OnKillfocusDoorpicker();
	afx_msg void OnSelchangeDoorpicker();
	afx_msg void OnKillfocusDoorid();
	afx_msg void OnKillfocusClosed();
	afx_msg void OnKillfocusWallgrouppicker();
	afx_msg void OnSelchangeWallgrouppicker();
	afx_msg void OnEditdoorpoly();
	afx_msg void OnEditwallpoly();
	afx_msg void OnKillfocusDoorpolypicker();
	afx_msg void OnSelchangeDoorpolypicker();
	afx_msg void OnOpen();
	afx_msg void OnOrder();
	afx_msg void OnClear();
	afx_msg void OnEdit();
	afx_msg void OnNew();
	afx_msg void OnEdittile();
	afx_msg void OnRemove();
	afx_msg void OnOverlay();
	afx_msg void OnSelection();
	afx_msg void OnSelection2();
	afx_msg void OnAdd3();
	afx_msg void OnAdd2();
	afx_msg void OnRemove2();
	afx_msg void OnRemove3();
	afx_msg void OnCopy();
	afx_msg void OnPaste();
	afx_msg void OnCopy2();
	afx_msg void OnPaste2();
	afx_msg void OnTransparent();
	afx_msg void OnCleanup();
	afx_msg void OnEdittile2();
	afx_msg void OnDrop();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WEDEDIT_H__2CDF2725_6846_49AC_912D_3245A293816F__INCLUDED_)
