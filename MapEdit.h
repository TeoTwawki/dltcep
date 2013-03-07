#if !defined(AFX_MAPEDIT_H__F6926E07_81F6_4BF2_ACAB_8574C3473F8C__INCLUDED_)
#define AFX_MAPEDIT_H__F6926E07_81F6_4BF2_ACAB_8574C3473F8C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MapEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMapEdit dialog

class CMapEdit : public CDialog
{
// Construction
public:
	CMapEdit(CWnd* pParent = NULL);   // standard constructor
  void NewMap();

// Dialog Data
	//{{AFX_DATA(CMapEdit)
	enum { IDD = IDD_MAPEDIT };
	CStatic	m_iconres;
	CComboBox	m_areapicker;
	CComboBox	m_mappicker;
	CString	m_maxmap;
	CString	m_caption;
	CString	m_tooltiptext;
	CString	m_maxarea;
	CString	m_north;
	CString	m_east;
	CString	m_south;
	CString	m_west;
	CString	m_text;
	CString	m_maxlink;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMapEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  CToolTipCtrl m_tooltip;
  CString filepath;

  void RefreshDialog();
  void SaveMap(int save);
  int GetMapPoint(POINT &pos);

	// Generated message map functions
	//{{AFX_MSG(CMapEdit)
	afx_msg void OnKillfocusMapcount();
	afx_msg void OnSelchangeMapcount();
	afx_msg void OnKillfocusMos();
	afx_msg void OnKillfocusStrref();
	afx_msg void OnKillfocusText();
	afx_msg void OnKillfocusMapicons();
	afx_msg void OnKillfocusWidth();
	afx_msg void OnKillfocusHeight();
	afx_msg void OnKillfocusUnknown10();
	afx_msg void OnKillfocusUnknown18();
	afx_msg void OnKillfocusUnknown1c();
	afx_msg void OnKillfocusAreapicker();
	afx_msg void OnSelchangeAreapicker();
	virtual BOOL OnInitDialog();
	afx_msg void OnLoad();
	afx_msg void OnLoadex();
	afx_msg void OnSaveas();
	afx_msg void OnNew();
	afx_msg void OnKillfocusName();
	afx_msg void OnKillfocusArea();
	afx_msg void OnKillfocusMapnumber();
	afx_msg void OnBrowse1();
	afx_msg void OnBrowse2();
	afx_msg void OnKillfocusAreaflag();
	afx_msg void OnFlag2();
	afx_msg void OnFlag3();
	afx_msg void OnFlag4();
	afx_msg void OnFlag5();
	afx_msg void OnFlag6();
	afx_msg void OnFlag7();
	afx_msg void OnFlag8();
	afx_msg void OnKillfocusIconidx();
	afx_msg void OnKillfocusXpos();
	afx_msg void OnKillfocusYpos();
	afx_msg void OnKillfocusCaption();
	afx_msg void OnKillfocusTooltip();
	afx_msg void OnFlag1();
	afx_msg void OnKillfocusLoadscreen();
	afx_msg void OnBrowse3();
	afx_msg void OnKillfocusLongname();
	afx_msg void OnKillfocusCaptiontext();
	afx_msg void OnKillfocusTooltiptext();
	afx_msg void OnEditlink();
	afx_msg void OnAddarea();
	afx_msg void OnDelarea();
	afx_msg void OnAddmap();
	afx_msg void OnDelmap();
	afx_msg void OnBrowse4();
	afx_msg void OnCheck();
	afx_msg void OnSave();
	afx_msg void OnFileTbg();
	afx_msg void OnFileTp2();
	afx_msg void OnSet();
	afx_msg void OnSelection();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CMapLink dialog

class CMapLink : public CDialog
{
// Construction
public:
	CMapLink(CWnd* pParent = NULL);   // standard constructor
  inline void SetLink(int map, int area)
  {
    m_map=map;
    m_area=area;
  }

  int m_map;
  int m_area;
// Dialog Data
	//{{AFX_DATA(CMapLink)
	enum { IDD = IDD_MAPLINK };
	CComboBox	m_entrancenamepicker;
	CComboBox	m_areapicker;
	int		m_direction;
	CString	m_maxdir;
	int		m_linkpicker;
	int		m_arealink;
	//}}AFX_DATA
  int m_first;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMapLink)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  CToolTipCtrl m_tooltip;

  void RefreshLink();
  void Encounters(int idx);
	// Generated message map functions
	//{{AFX_MSG(CMapLink)
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusDirection();
	afx_msg void OnSelchangeDirection();
	afx_msg void OnKillfocusArealink();
	afx_msg void OnSelchangeArealink();
	afx_msg void OnKillfocusLinkpicker();
	afx_msg void OnSelchangeLinkpicker();
	afx_msg void OnKillfocusEntrance();
	afx_msg void OnKillfocusDistance();
	afx_msg void OnKillfocusFlags();
	afx_msg void OnKillfocusU1();
	afx_msg void OnKillfocusU2();
	afx_msg void OnKillfocusU3();
	afx_msg void OnKillfocusU4();
	afx_msg void OnKillfocusU5();
	afx_msg void OnKillfocusChance();
	afx_msg void OnBrowse1();
	afx_msg void OnBrowse2();
	afx_msg void OnBrowse3();
	afx_msg void OnBrowse4();
	afx_msg void OnBrowse5();
	afx_msg void OnSelchangeEntrance();
	afx_msg void OnAdd();
	afx_msg void OnRemove();
	afx_msg void OnFlag1();
	afx_msg void OnFlag2();
	afx_msg void OnFlag3();
	afx_msg void OnFlag4();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAPEDIT_H__F6926E07_81F6_4BF2_ACAB_8574C3473F8C__INCLUDED_)
