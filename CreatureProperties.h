#if !defined(AFX_CREATUREGENERAL_H__A0634BD1_B659_442E_BD5B_D5F6F56A742B__INCLUDED_)
#define AFX_CREATUREGENERAL_H__A0634BD1_B659_442E_BD5B_D5F6F56A742B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CreatureGeneral.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCreatureGeneral dialog

class CCreatureGeneral : public CPropertyPage
{
	DECLARE_DYNCREATE(CCreatureGeneral)
 // Construction
public:
	CCreatureGeneral();
	~CCreatureGeneral();
  void RefreshGeneral();

// Dialog Data
	//{{AFX_DATA(CCreatureGeneral)
	enum { IDD = IDD_CREATUREGENERAL };
	CString	m_longname;
	BOOL	m_longnametag;
	CString	m_shortname;
	BOOL	m_shortnametag;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCreatureGeneral)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  CToolTipCtrl m_tooltip;

  CString FindKit(unsigned int kit);
  int FillKitCombo(CString idsname, CComboBox *cb, int len);

	// Generated message map functions
	//{{AFX_MSG(CCreatureGeneral)
	afx_msg void OnKillfocusLongname();
	afx_msg void OnKillfocusLongnameref();
	afx_msg void OnLongnametag();
	afx_msg void OnKillfocusShortname();
	afx_msg void OnKillfocusShortnameref();
	afx_msg void OnShortnametag();
	afx_msg void OnKillfocusXpvalue();
	afx_msg void OnKillfocusStateflags();
	afx_msg void OnKillfocusCurrxp();
	afx_msg void OnKillfocusGold();
	afx_msg void OnKillfocusCurrhp();
	afx_msg void OnKillfocusMaxhp();
	afx_msg void OnKillfocusAnimation();
	afx_msg void OnKillfocusLevel1();
	afx_msg void OnKillfocusLevel2();
	afx_msg void OnKillfocusLevel3();
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusMorale();
	afx_msg void OnKillfocusMbreak();
	afx_msg void OnKillfocusMrecovery();
	afx_msg void OnKillfocusIdsea();
	afx_msg void OnKillfocusIdsgeneral();
	afx_msg void OnKillfocusIdsrace();
	afx_msg void OnKillfocusIdsclass();
	afx_msg void OnKillfocusIdsspecific();
	afx_msg void OnKillfocusIdsgender();
	afx_msg void OnKillfocusIdsalignment();
	afx_msg void OnKillfocusHated();
	afx_msg void OnKillfocusThac0();
	afx_msg void OnKillfocusNumattacks();
	afx_msg void OnKillfocusDualflags();
	afx_msg void OnV10();
	afx_msg void OnV22();
	afx_msg void OnV90();
	afx_msg void OnV12();
	afx_msg void OnNew1();
	afx_msg void OnNew2();
	afx_msg void OnKillfocusKit();
	afx_msg void OnLevelslot();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CCreatureIcons dialog

class CCreatureIcons : public CPropertyPage
{
	DECLARE_DYNCREATE(CCreatureIcons)
 // Construction
public:
	CCreatureIcons();
	~CCreatureIcons();
  void RefreshIcons();

// Dialog Data
	//{{AFX_DATA(CCreatureIcons)
	enum { IDD = IDD_CREATUREICONS };
	CStatic	m_largeportrait;
	CStatic	m_smallportrait;
	//}}AFX_DATA
  CString m_bms, m_bml;
  COLORREF m_bgcolor;
  bool m_play;
  int m_playmax, m_playindex;
  HBITMAP m_hb1, m_hb2;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCreatureIcons)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  CToolTipCtrl m_tooltip;

  void ColorDlg(int idx);
  void OnBrowseBCS(int idx);

	// Generated message map functions
	//{{AFX_MSG(CCreatureIcons)
	afx_msg void OnKillfocusSmallportrait();
	afx_msg void OnKillfocusLargeportrait();
	afx_msg void OnKillfocusCarmor();
	afx_msg void OnKillfocusChair();
	afx_msg void OnKillfocusCleather();
	afx_msg void OnKillfocusCmajor();
	afx_msg void OnKillfocusCmetal();
	afx_msg void OnKillfocusCminor();
	afx_msg void OnKillfocusCskin();
	afx_msg void OnKillfocusOverride();
	afx_msg void OnKillfocusClass();
	afx_msg void OnKillfocusRace();
	afx_msg void OnKillfocusGeneral();
	afx_msg void OnKillfocusDefault();
	afx_msg void OnKillfocusDialog();
	afx_msg void OnKillfocusDvar();
	afx_msg void OnBrowse();
	afx_msg void OnBrowse1();
	afx_msg void OnBrowse2();
	afx_msg void OnBrowse3();
	afx_msg void OnBrowse4();
	afx_msg void OnBrowse5();
	afx_msg void OnBrowse6();
	afx_msg void OnBrowse7();
	afx_msg void OnBrowse8();
	afx_msg void OnBrowse9();
	afx_msg void OnBrowse10();
	afx_msg void OnBrowse11();
	afx_msg void OnBrowse12();
	afx_msg void OnBrowse13();
	afx_msg void OnBrowse14();
	virtual BOOL OnInitDialog();
	afx_msg void OnPlay();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CCreatureSkills dialog

class CCreatureSkills : public CPropertyPage
{
	DECLARE_DYNCREATE(CCreatureSkills)
 // Construction
public:
	CCreatureSkills();
	~CCreatureSkills();
  void RefreshSkills();

// Dialog Data
	//{{AFX_DATA(CCreatureSkills)
	enum { IDD = IDD_CREATURESKILLS };
	CString	m_pst;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCreatureSkills)
  public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  CToolTipCtrl m_tooltip;

	// Generated message map functions
	//{{AFX_MSG(CCreatureSkills)
	afx_msg void OnKillfocusHide();
	afx_msg void OnKillfocusDetill();
	afx_msg void OnKillfocusSettrap();
	afx_msg void OnKillfocusLore();
	afx_msg void OnKillfocusLockpick();
	afx_msg void OnKillfocusStealth();
	afx_msg void OnKillfocusFindtrap();
	afx_msg void OnKillfocusPickpocket();
	afx_msg void OnKillfocusLuck();
	afx_msg void OnKillfocusTracking();
	afx_msg void OnKillfocusStr();
	afx_msg void OnKillfocusInt();
	afx_msg void OnKillfocusDex();
	afx_msg void OnKillfocusCha();
	afx_msg void OnKillfocusFatigue();
	afx_msg void OnKillfocusStrbon();
	afx_msg void OnKillfocusWis();
	afx_msg void OnKillfocusCon();
	afx_msg void OnKillfocusIntox();
	afx_msg void OnKillfocusAc();
	afx_msg void OnKillfocusEffac();
	afx_msg void OnKillfocusCrushing();
	afx_msg void OnKillfocusMissile();
	afx_msg void OnKillfocusSlashing();
	afx_msg void OnKillfocusPiercing();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CCreatureResist dialog

class CCreatureResist : public CPropertyPage
{
	DECLARE_DYNCREATE(CCreatureResist)
 // Construction
public:
	CCreatureResist();
	~CCreatureResist();
  void RefreshResist();

// Dialog Data
	//{{AFX_DATA(CCreatureResist)
	enum { IDD = IDD_CREATURERES };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCreatureResist)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  CToolTipCtrl m_tooltip;

  void SetDefaultSaves(CString table);

	// Generated message map functions
	//{{AFX_MSG(CCreatureResist)
	afx_msg void OnKillfocusDeath();
	afx_msg void OnKillfocusWands();
	afx_msg void OnKillfocusPolymorph();
	afx_msg void OnKillfocusBreath();
	afx_msg void OnKillfocusSpells();
	afx_msg void OnKillfocusFire();
	afx_msg void OnKillfocusCold();
	afx_msg void OnKillfocusElectricity();
	afx_msg void OnKillfocusAcid();
	afx_msg void OnKillfocusMagic();
	afx_msg void OnKillfocusMagfire();
	afx_msg void OnKillfocusMagcold();
	afx_msg void OnKillfocusSlashing();
	afx_msg void OnKillfocusCrushing();
	afx_msg void OnKillfocusPiercing();
	afx_msg void OnKillfocusMissile();
	afx_msg void OnDef1();
	afx_msg void OnDef2();
	afx_msg void OnDef3();
	afx_msg void OnDef4();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CCreatureStrings dialog

class CCreatureStrings :  public CPropertyPage
{
	DECLARE_DYNCREATE(CCreatureStrings)
// Construction
public:
	CCreatureStrings();
	~CCreatureStrings();
  void RefreshStrings();

// Dialog Data
	//{{AFX_DATA(CCreatureStrings)
	enum { IDD = IDD_CREATURESTRINGS };
	CEdit	m_buddycontrol;
	CSpinButtonCtrl	m_spincontrol;
	CComboBox	m_slotpicker_control;
	CString	m_text;
	BOOL	m_tagged;
	CString	m_soundref;
	int		m_ref;
	//}}AFX_DATA
  int m_stringnum;
  int m_stringcount;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCreatureStrings)
  public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  CToolTipCtrl m_tooltip;

	// Generated message map functions
	//{{AFX_MSG(CCreatureStrings)
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusSlotpicker();
	afx_msg void OnSelchangeSlotpicker();
	afx_msg void OnKillfocusRef();
	afx_msg void OnTagged();
	afx_msg void OnKillfocusSoundref();
	afx_msg void OnKillfocusText();
	afx_msg void OnPlay();
	afx_msg void OnChangeBuddy();
	afx_msg void OnCopy();
	afx_msg void OnPaste();
	afx_msg void OnExport();
	afx_msg void OnImport();
	afx_msg void OnClear();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CCreatureUnknown dialog

class CCreatureUnknown : public CPropertyPage
{
	DECLARE_DYNCREATE(CCreatureUnknown)
 // Construction
public:
	CCreatureUnknown();
	~CCreatureUnknown();
  void RefreshUnknown();

// Dialog Data
	//{{AFX_DATA(CCreatureUnknown)
	enum { IDD = IDD_CREATUREUNKNOWN };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCreatureUnknown)
  public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  CToolTipCtrl m_tooltip;

	// Generated message map functions
	//{{AFX_MSG(CCreatureUnknown)
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusLarge();
	afx_msg void OnKillfocusSmall();
	afx_msg void OnKillfocusBow();
	afx_msg void OnKillfocusSpear();
	afx_msg void OnKillfocusBlunt();
	afx_msg void OnKillfocusSpiked();
	afx_msg void OnKillfocusAxe();
	afx_msg void OnKillfocusMissile();
	afx_msg void OnKillfocusU1();
	afx_msg void OnKillfocusU2();
	afx_msg void OnKillfocusU3();
	afx_msg void OnKillfocusU4();
	afx_msg void OnKillfocusU5();
	afx_msg void OnKillfocusU6();
	afx_msg void OnKillfocusU7();
	afx_msg void OnKillfocusU8();
	afx_msg void OnKillfocusU9();
	afx_msg void OnKillfocusU10();
	afx_msg void OnKillfocusU11();
	afx_msg void OnKillfocusU12();
	afx_msg void OnKillfocusU13();
	afx_msg void OnKillfocusU20();
	afx_msg void OnKillfocusU21();
	afx_msg void OnKillfocusU22();
	afx_msg void OnKillfocusU23();
	afx_msg void OnKillfocusU24();
	afx_msg void OnKillfocusU25();
	afx_msg void OnKillfocusU26();
	afx_msg void OnKillfocusU27();
	afx_msg void OnClearall();
	afx_msg void OnKillfocusU15();
	afx_msg void OnKillfocusU14();
	afx_msg void OnKillfocusU28();
	afx_msg void OnKillfocusU16();
	afx_msg void OnKillfocusU17();
	afx_msg void OnKillfocusU18();
	afx_msg void OnKillfocusU19();
	afx_msg void OnKillfocusU29();
	afx_msg void OnKillfocusU30();
	afx_msg void OnKillfocusReputation();
	afx_msg void OnTorment();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CCreatureItem dialog

class CCreatureItem : public CPropertyPage
{
	DECLARE_DYNCREATE(CCreatureItem)
 // Construction
public:
	CCreatureItem();
	~CCreatureItem();
  void RefreshItem();

// Dialog Data
	//{{AFX_DATA(CCreatureItem)
	enum { IDD = IDD_CREATUREITEMS };
	CComboBox	m_bookpicker;
	CComboBox	m_spellpicker;
	CComboBox	m_levelpicker;
	CStatic	m_invicon;
	CComboBox	m_slotpicker;
	CString	m_itemres;
	CString	m_maxspell;
	CString	m_maxslot;
	CString	m_spellres;
	CString	m_spellres2;
	//}}AFX_DATA
  int m_idx;
  int m_spellslot;
  HBITMAP m_hb;
  
  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CCreatureItem)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
  
  // Implementation
protected:  
  CToolTipCtrl m_tooltip;

  void RefreshSpellPicker(int pos);
  CIntMapString *GetIWD2SpellList(int pos);
  CString ResolveIWD2SpellResRef(int spid, int pos);
  int ResolveIWD2SpellID(CString resref, int pos);
  int ResolveTypeAndLevel(CString key);
  CString ResolveSpellName(CString key);
  int AddBook(int level, int type);
  void AddIWD2Spell(CString spellres);
  void AddSpell(int slot, CString spellres);
  void RemoveSpell(int slot);
  int CalculateIWD2Slots();
  int SetMemoryByClass(CString daname, int type, int level);
  // Generated message map functions
  //{{AFX_MSG(CCreatureItem)
  virtual BOOL OnInitDialog();
  afx_msg void OnKillfocusSlotpicker();
  afx_msg void OnSelchangeSlotpicker();
  afx_msg void OnBrowse();
  afx_msg void OnKillfocusItemres();
  afx_msg void OnKillfocusUse1();
  afx_msg void OnKillfocusUse2();
	afx_msg void OnKillfocusUse3();
	afx_msg void OnKillfocusFlags();
	afx_msg void OnIdentified();
	afx_msg void OnNosteal();
	afx_msg void OnStolen();
	afx_msg void OnKillfocusUnknown();
	afx_msg void OnKillfocusLevelslot();
	afx_msg void OnSelchangeLevelslot();
	afx_msg void OnClear();
	afx_msg void OnKillfocusLevel();
	afx_msg void OnKillfocusMin();
	afx_msg void OnKillfocusMax();
	afx_msg void OnAdd();
	afx_msg void OnBrowse2();
	afx_msg void OnKillfocusSpellpicker();
	afx_msg void OnSelchangeSpellpicker();
	afx_msg void OnKillfocusSpelltype();
	afx_msg void OnDelslot();
	afx_msg void OnAddslot();
	afx_msg void OnKillfocusBookpicker();
	afx_msg void OnSelchangeBookpicker();
	afx_msg void OnKillfocusSpellres2();
	afx_msg void OnBrowse3();
	afx_msg void OnMemorise();
	afx_msg void OnRemove();
	afx_msg void OnKillfocusSpelltype2();
	afx_msg void OnKillfocusLevel2();
	afx_msg void OnBook();
	afx_msg void OnForget();
	afx_msg void OnBook2();
	afx_msg void OnClass();
	afx_msg void OnKillfocusSelected();
	afx_msg void OnUndroppable();
	afx_msg void OnClearall();
	afx_msg void OnMemorised();
	afx_msg void OnDefaultKillfocus();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CCreatureEffect dialog

class CCreatureEffect : public CPropertyPage
{
	DECLARE_DYNCREATE(CCreatureEffect)
 // Construction
public:
	CCreatureEffect();
	~CCreatureEffect();
  void RefreshEffect();

// Dialog Data
	//{{AFX_DATA(CCreatureEffect)
	enum { IDD = IDD_CREATUREEFF };
	CListBox	m_slotpicker_control;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCreatureEffect)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  CToolTipCtrl m_tooltip;

	// Generated message map functions
	//{{AFX_MSG(CCreatureEffect)
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	afx_msg void OnEdit();
	afx_msg void OnKillfocusSlotpicker();
	afx_msg void OnV10();
	afx_msg void OnV20();
	afx_msg void OnPaste();
	afx_msg void OnCopy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CCreaturePropertySheet

class CCreaturePropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CCreaturePropertySheet)

// Construction
public:
	CCreaturePropertySheet(CWnd* pWndParent = NULL);

// Attributes
public:
	CCreatureGeneral m_PageGeneral;
	CCreatureIcons m_PageIcons;
	CCreatureSkills m_PageSkills;
	CCreatureResist m_PageResist;
	CCreatureStrings m_PageStrings;
	CCreatureItem m_PageItem;
	CCreatureEffect m_PageEffect;
	CCreatureUnknown m_PageUnknown;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCreaturePropertySheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCreaturePropertySheet();
  void RefreshDialog();

protected:
// Generated message map functions
	//{{AFX_MSG(CCreaturePropertySheet)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CREATUREGENERAL_H__A0634BD1_B659_442E_BD5B_D5F6F56A742B__INCLUDED_)
