// SpellProperties.h : header file
//

#ifndef __SPELLPROPERTIES_H__
#define __SPELLPROPERTIES_H__

/////////////////////////////////////////////////////////////////////////////
// CSpellGeneral dialog

class CSpellGeneral : public CPropertyPage
{
	DECLARE_DYNCREATE(CSpellGeneral)

// Construction
public:
	CSpellGeneral();
	~CSpellGeneral();
  void RefreshGeneral();
  CString m_bmb;
  HBITMAP m_hbmb;

protected:
  CToolTipCtrl m_tooltip;

// Dialog Data
	//{{AFX_DATA(CSpellGeneral)
	enum { IDD = IDD_SPELLGENERAL };
	CStatic	m_bookicon_control;
	CComboBox	m_sectype_control;
	CComboBox	m_school_control;
	CComboBox	m_spelltypepicker_control;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSpellGeneral)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
  protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSpellGeneral)
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusWav();
	afx_msg void OnKillfocusSpelltype();
	afx_msg void OnKillfocusLevel();
	afx_msg void OnKillfocusSchool();
	afx_msg void OnKillfocusSectype();
	afx_msg void OnKillfocusSelection();
	afx_msg void OnKillfocusUnknown0c();
	afx_msg void OnKillfocusUnknown23();
	afx_msg void OnKillfocusUnknown28();
	afx_msg void OnKillfocusUnknown2c();
	afx_msg void OnKillfocusUnknown30();
	afx_msg void OnKillfocusUnknown38();
	afx_msg void OnKillfocusUnknown44();
	afx_msg void OnKillfocusUnknown48();
	afx_msg void OnKillfocusUnknown4c();
	afx_msg void OnKillfocusUnknown54();
	afx_msg void OnKillfocusUnknown58();
	afx_msg void OnKillfocusUnknown5c();
	afx_msg void OnKillfocusUnknown60();
	afx_msg void OnAttr1();
	afx_msg void OnAttr2();
	afx_msg void OnAttr3();
	afx_msg void OnAttr4();
	afx_msg void OnAttr5();
	afx_msg void OnAttr6();
	afx_msg void OnAttr7();
	afx_msg void OnAttr8();
	afx_msg void OnKillfocusExclude();
	afx_msg void OnKillfocusBookicon();
	afx_msg void OnKillfocusByte1();
	afx_msg void OnKillfocusByte34();
	afx_msg void OnV1();
	afx_msg void OnV20();
	afx_msg void OnBrowse();
	afx_msg void OnInvicon1();
	afx_msg void OnPlaysound();
	afx_msg void OnKillfocusPriesttype();
	afx_msg void OnKillfocusU1();
	afx_msg void OnKillfocusU2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};


/////////////////////////////////////////////////////////////////////////////
// CSpellDescription dialog

class CSpellDescription : public CPropertyPage
{
	DECLARE_DYNCREATE(CSpellDescription)

// Construction
public:
	CSpellDescription();
	~CSpellDescription();
  void RefreshDescription();

// Dialog Data
	//{{AFX_DATA(CSpellDescription)
	enum { IDD = IDD_SPELLDESC };
	CString	m_longdesc;
	CString	m_longname;
	CString	m_shortdesc;
	CString	m_shortname;
	BOOL	m_longdesctag;
	BOOL	m_longnametag;
	BOOL	m_shortdesctag;
	BOOL	m_shortnametag;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSpellDescription)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  CToolTipCtrl m_tooltip;

	// Generated message map functions
	//{{AFX_MSG(CSpellDescription)
	afx_msg void OnKillfocusLongdescref();
	afx_msg void OnKillfocusLongref();
	afx_msg void OnKillfocusShortdescref();
	afx_msg void OnKillfocusShortref();
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusShortdesc();
	afx_msg void OnKillfocusLongname();
	afx_msg void OnKillfocusLongdesc();
	afx_msg void OnLongnametag();
	afx_msg void OnLongdesctag();
	afx_msg void OnNew1();
	afx_msg void OnNew2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////
// CSpellEquip dialog

class CSpellEquip : public CPropertyPage
{
	DECLARE_DYNCREATE(CSpellEquip)

// Construction
public:
	CSpellEquip();
	~CSpellEquip();
  void DoDataExchangeEquip(CDataExchange* pDX);
  void EnableWindow_Equip(bool value);
  void RefreshEquip();

// Dialog Data
	//{{AFX_DATA(CSpellEquip)
	enum { IDD = IDD_SPELLEQUIP };
	CListBox	m_equipnum_control;
	//}}AFX_DATA
	int	equipnum;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSpellEquip)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  CToolTipCtrl m_tooltip;

	// Generated message map functions
	//{{AFX_MSG(CSpellEquip)
	afx_msg void OnKillfocusEquipnum();
	virtual BOOL OnInitDialog();
	afx_msg void OnEdit();
	afx_msg void OnEquipremove();
	afx_msg void OnEquipadd();
	afx_msg void OnEquipcopy();
	afx_msg void OnEquippaste();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CSpellExtended dialog

class CSpellExtended : public CPropertyPage
{
	DECLARE_DYNCREATE(CSpellExtended)

// Construction
public:
	CSpellExtended();
	~CSpellExtended();
  void RefreshExtended();
// Dialog Data
	//{{AFX_DATA(CSpellExtended)
	enum { IDD = IDD_SPELLEFFECT };
	CListBox	m_exteffnum_control;
  CStatic m_useicon_control;
	CComboBox	m_projid_control;
	CComboBox	m_chargetype_control;
	CComboBox	m_damagetype_control;
	CComboBox	m_extheadnum_control;
	CComboBox	m_projframe_control;
	CComboBox	m_target_control;
	CComboBox	m_loc_control;
	CComboBox	m_exttype_control;
	CString	m_headnum;
	//}}AFX_DATA
  // real counters
	int	m_extheadnum;
  int extheadnum;
  int m_exteffnum;
  CString m_bmu;
  HBITMAP m_hbmu;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSpellExtended)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  CToolTipCtrl m_tooltip;

  void DoDataExchangeEffects(CDataExchange* pDX);
  void DoDataExchangeExtended(CDataExchange* pDX);
  void EnableWindow_ExtEffect(bool value);
  void EnableWindow_Extended(bool value);
	afx_msg void OnDefaultKillfocus();

	//{{AFX_MSG(CSpellExtended)
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusExtheadnum();
	afx_msg void OnKillfocusExteffnum();
	afx_msg void OnExtpaste();
	afx_msg void OnExteffadd();
	afx_msg void OnExteffcopy();
	afx_msg void OnExteffpaste();
	afx_msg void OnExteffremove();
	afx_msg void OnExtcopy();
	afx_msg void OnExtadd();
	afx_msg void OnExtremove();
	afx_msg void OnSelchangeExtheadnum();
	afx_msg void OnOrder();
	afx_msg void OnUseicon();
	afx_msg void OnEdit();
	afx_msg void OnNext();
	afx_msg void OnPrev();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////
// CSpellGeneral dialog

class CSpellExtra : public CPropertyPage
{
	DECLARE_DYNCREATE(CSpellExtra)

// Construction
public:
	CSpellExtra();
	~CSpellExtra();
  void RefreshExtra();

protected:
// Dialog Data
	//{{AFX_DATA(CSpellExtra)
	enum { IDD = IDD_SPELLEXTRA };
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSpellExtra)
	public:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
protected:
  CToolTipCtrl m_tooltip;

	// Generated message map functions
	//{{AFX_MSG(CSpellExtra)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CSpellTool : public CPropertyPage
{
	DECLARE_DYNCREATE(CSpellTool)

// Construction
public:
	CSpellTool();
	~CSpellTool();
  void RefreshTool();

// Dialog Data
	//{{AFX_DATA(CSpellTool)
	enum { IDD = IDD_SPELLTOOL };
	int		m_rangeinc;
	int		m_range;
	int		m_duration;
	int		m_durationinc;
	int		m_levelinc;
	int		m_level;
	int		m_which;
	int		m_damage;
	int		m_damageinc;
	BOOL	m_half;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpellTool)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  CToolTipCtrl m_tooltip;

	// Generated message map functions
	//{{AFX_MSG(CSpellTool)
	afx_msg void OnKillfocusRange();
	afx_msg void OnKillfocusRangeinc();
	afx_msg void OnKillfocusDuration();
	afx_msg void OnKillfocusDurationinc();
	afx_msg void OnDoranges();
	afx_msg void OnDodurations();
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusLevel();
	afx_msg void OnKillfocusLevelinc();
	afx_msg void OnDolevels();
	afx_msg void OnKillfocusDamage();
	afx_msg void OnKillfocusDamageinc();
	afx_msg void OnDodamages();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CSpellPropertySheet

class CSpellPropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CSpellPropertySheet)

// Construction
public:
	CSpellPropertySheet(CWnd* pWndParent = NULL);

// Attributes
public:
	CSpellGeneral m_PageGeneral;
	CSpellDescription m_PageDescription;
	CSpellEquip m_PageEquip;
	CSpellExtended m_PageExtended;
	CSpellExtra m_PageExtra;
	CSpellTool m_PageTool;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpellPropertySheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSpellPropertySheet();
  void RefreshDialog();

protected:
// Generated message map functions
	//{{AFX_MSG(CSpellPropertySheet)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // __SPELLPROPERTIES_H__
