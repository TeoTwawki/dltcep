#if !defined(AFX_AREAPROPERTIES_H__426C03AC_1D92_4AA7_8E0C_832BD0062BBF__INCLUDED_)
#define AFX_AREAPROPERTIES_H__426C03AC_1D92_4AA7_8E0C_832BD0062BBF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AreaProperties.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAreaGeneral dialog

class CAreaGeneral : public CPropertyPage
{
	DECLARE_DYNCREATE(CAreaGeneral)

// Construction
public:
	CAreaGeneral();
	~CAreaGeneral();
  void RefreshGeneral();

// Dialog Data
	//{{AFX_DATA(CAreaGeneral)
	enum { IDD = IDD_AREAGENERAL };
	CComboBox	m_areaflag_control;
	//}}AFX_DATA
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAreaGeneral)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  CToolTipCtrl m_tooltip;

  int CantMakeMinimap(CString tmpstr);
	afx_msg void DefaultKillfocus();
	// Generated message map functions
	//{{AFX_MSG(CAreaGeneral)
	afx_msg void OnOutdoor();
	afx_msg void OnDaynight();
	afx_msg void OnWeather();
	afx_msg void OnCity();
	afx_msg void OnForest();
	afx_msg void OnDungeon();
	afx_msg void OnExtendednight();
	afx_msg void OnCanrest();
	virtual BOOL OnInitDialog();
	afx_msg void OnV10();
	afx_msg void OnV91();
	afx_msg void OnSongs();
	afx_msg void OnInt();
	afx_msg void OnSong();
	afx_msg void OnRest();
	afx_msg void OnDelexp();
	afx_msg void OnUnknown();
	afx_msg void OnView();
	afx_msg void OnBrowse();
	afx_msg void OnEdit();
	afx_msg void OnBrowse2();
	afx_msg void OnNight();
	afx_msg void OnExplored();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CAreaActor dialog

class CAreaActor : public CPropertyPage
{
	DECLARE_DYNCREATE(CAreaActor)

// Construction
public:
	CAreaActor();
	~CAreaActor();
  void RefreshActor();

// Dialog Data
	//{{AFX_DATA(CAreaActor)
	enum { IDD = IDD_AREAACTOR };
	CComboBox	m_actorpicker;
	CButton	m_schedule_control;
	CString	m_timeofday;
	//}}AFX_DATA

  area_actor actorcopy;
  int m_actornum;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAreaActor)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  CToolTipCtrl m_tooltip;

	afx_msg void DefaultKillfocus();
	// Generated message map functions
	//{{AFX_MSG(CAreaActor)
	virtual BOOL OnInitDialog();
	afx_msg void OnSchedule();
	afx_msg void OnAdd();
	afx_msg void OnRemove();
	afx_msg void OnCopy();
	afx_msg void OnPaste();
	afx_msg void OnKillfocusActorpicker();
	afx_msg void OnSelchangeActorpicker();
	afx_msg void OnFields();
	afx_msg void OnBrowse();
	afx_msg void OnBrowse2();
	afx_msg void OnBrowse3();
	afx_msg void OnBrowse4();
	afx_msg void OnBrowse5();
	afx_msg void OnBrowse6();
	afx_msg void OnBrowse7();
	afx_msg void OnBrowse8();
	afx_msg void OnSame();
	afx_msg void OnClear();
	afx_msg void OnEdit();
	afx_msg void OnSetpos();
	afx_msg void OnSetdest();
	afx_msg void OnBrowse9();
	afx_msg void OnEmbed();
	afx_msg void OnButton1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CAreaTrigger dialog

class CAreaTrigger : public CPropertyPage
{
	DECLARE_DYNCREATE(CAreaTrigger)

// Construction
public:
	CAreaTrigger();
	~CAreaTrigger();
  void RefreshTrigger();

// Dialog Data
	//{{AFX_DATA(CAreaTrigger)
	enum { IDD = IDD_AREATRIGGER };
	CSpinButtonCtrl	m_spin_control;
	CComboBox	m_entrancenamepicker;
	CComboBox	m_regiontype_control;
	CComboBox	m_triggerpicker;
	//}}AFX_DATA
  area_trigger triggercopy;
  area_vertex *vertexcopy;
  int vertexsize;
  HBITMAP hbc;
 
  int m_triggernum;
  CString m_infostr;
	CString m_dlgstr;
  CStatic m_cursoricon;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAreaTrigger)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:  
  CToolTipCtrl m_tooltip;

	afx_msg void DefaultKillfocus();
	// Generated message map functions
	//{{AFX_MSG(CAreaTrigger)
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusTriggerpicker();
	afx_msg void OnSelchangeTriggerpicker();
	afx_msg void OnRecalcbox();
	afx_msg void OnParty();
	afx_msg void OnAdd();
	afx_msg void OnRemove();
	afx_msg void OnCopy();
	afx_msg void OnPaste();
	afx_msg void OnNonpc();
	afx_msg void OnTundet();
	afx_msg void OnTreset();
	afx_msg void OnTdetect();
	afx_msg void OnTunk1();
	afx_msg void OnTunk2();
	afx_msg void OnTnpc();
	afx_msg void OnTunk3();
	afx_msg void OnTdeactivated();
	afx_msg void OnTdoor();
	afx_msg void OnToverride();
	afx_msg void OnTunk4();
	afx_msg void OnTunk5();
	afx_msg void OnTunk6();
	afx_msg void OnTunk7();
	afx_msg void OnCursor();
	afx_msg void OnUnknown();
	afx_msg void OnDropdownEntrancename();
	afx_msg void OnChangeCursoridx();
	afx_msg void OnKillfocusDestarea();
	afx_msg void OnSet();
	afx_msg void OnBrowse();
	afx_msg void OnBrowse2();
	afx_msg void OnBrowse3();
	afx_msg void OnBrowse4();
	afx_msg void OnKillfocusRegiontype();
	afx_msg void OnEditpolygon();
	afx_msg void OnSelection();
	afx_msg void OnString();
	afx_msg void OnSet2();
	afx_msg void OnBrowse5();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CAreaSpawn dialog

class CAreaSpawn : public CPropertyPage
{
	DECLARE_DYNCREATE(CAreaSpawn)

// Construction
public:
	CAreaSpawn();
	~CAreaSpawn();
  void RefreshSpawn();

// Dialog Data
	//{{AFX_DATA(CAreaSpawn)
	enum { IDD = IDD_AREASPAWN };
	CComboBox	m_spawnnumpicker;
	CButton	m_schedule_control;
	CComboBox	m_spawnpicker;
	CString	m_timeofday;
	//}}AFX_DATA

  area_spawn spawncopy;
  int m_spawnnum;
  int m_crenum;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAreaSpawn)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  CToolTipCtrl m_tooltip;

	afx_msg void DefaultKillfocus();
	// Generated message map functions
	//{{AFX_MSG(CAreaSpawn)
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusSpawnpicker();
	afx_msg void OnSelchangeSpawnpicker();
	afx_msg void OnKillfocusSpawnnumpicker();
	afx_msg void OnSelchangeSpawnnumpicker();
	afx_msg void OnSchedule();
	afx_msg void OnKillfocusCreres();
	afx_msg void OnCopy();
	afx_msg void OnPaste();
	afx_msg void OnAdd();
	afx_msg void OnRemove();
	afx_msg void OnAddcre();
	afx_msg void OnDelcre();
	afx_msg void OnBrowse();
	afx_msg void OnKillfocusUnknown82();
	afx_msg void OnUnknown();
	afx_msg void OnSet();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CAreaEntrance dialog

class CAreaEntrance : public CPropertyPage
{
	DECLARE_DYNCREATE(CAreaEntrance)

// Construction
public:
	CAreaEntrance();
	~CAreaEntrance();
  void RefreshEntrance();

// Dialog Data
	//{{AFX_DATA(CAreaEntrance)
	enum { IDD = IDD_AREAENTRANCE };
	CComboBox	m_entrancepicker;
	//}}AFX_DATA

  area_entrance entrancecopy;
  int m_entrancenum;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAreaEntrance)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  CToolTipCtrl m_tooltip;

	// Generated message map functions
	//{{AFX_MSG(CAreaEntrance)
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusEntrancepicker();
	afx_msg void OnSelchangeEntrancepicker();
	afx_msg void OnKillfocusPosx();
	afx_msg void OnKillfocusPosy();
	afx_msg void OnKillfocusFace();
	afx_msg void OnAdd();
	afx_msg void OnRemove();
	afx_msg void OnCopy();
	afx_msg void OnPaste();
	afx_msg void OnKillfocusUnknown26();
	afx_msg void OnKillfocusUnknown28();
	afx_msg void OnUnknown();
	afx_msg void OnKillfocusUnknown2a();
	afx_msg void OnSet();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CAreaAmbient dialog

class CAreaAmbient : public CPropertyPage
{
	DECLARE_DYNCREATE(CAreaAmbient)

// Construction
public:
	CAreaAmbient();
	~CAreaAmbient();
  void RefreshAmbient();

// Dialog Data
	//{{AFX_DATA(CAreaAmbient)
	enum { IDD = IDD_AREAAMBIENT };
	CComboBox	m_ambinumpicker;
	CButton	m_schedule_control;
	CComboBox	m_ambientpicker;
	CString	m_timeofday;
	//}}AFX_DATA

  area_ambient ambientcopy;
  int m_ambientnum;
  int m_wavnum;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAreaAmbient)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  CToolTipCtrl m_tooltip;

	// Generated message map functions
	//{{AFX_MSG(CAreaAmbient)
	afx_msg void OnKillfocusAmbientpicker();
	afx_msg void OnSelchangeAmbientpicker();
	afx_msg void OnKillfocusPosx();
	afx_msg void OnKillfocusPosy();
	afx_msg void OnAdd();
	afx_msg void OnRemove();
	afx_msg void OnCopy();
	afx_msg void OnPaste();
	afx_msg void OnUnknown();
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusRadius();
	afx_msg void OnKillfocusHeight();
	afx_msg void OnKillfocusVolume();
	afx_msg void OnSchedule();
	afx_msg void OnKillfocusAmbinumpicker();
	afx_msg void OnSelchangeAmbinumpicker();
	afx_msg void OnKillfocusWavres();
	afx_msg void OnAddwav();
	afx_msg void OnDelwav();
	afx_msg void OnBrowse();
	afx_msg void OnPlay();
	afx_msg void OnKillfocusNum();
	afx_msg void OnKillfocusGap();
	afx_msg void OnKillfocusSoundnum();
	afx_msg void OnFlag1();
	afx_msg void OnFlag2();
	afx_msg void OnFlag3();
	afx_msg void OnFlag4();
	afx_msg void OnKillfocusU28();
	afx_msg void OnKillfocusU2c();
	afx_msg void OnKillfocusU90();
	afx_msg void OnKillfocusFlags();
	afx_msg void OnSet();
	afx_msg void OnFlag5();
	afx_msg void OnSelection();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CAreaContainer dialog

class CAreaContainer : public CPropertyPage
{
 	DECLARE_DYNCREATE(CAreaContainer)

// Construction
public:
	CAreaContainer();
	~CAreaContainer();
  void RefreshContainer();

// Dialog Data
	//{{AFX_DATA(CAreaContainer)
	enum { IDD = IDD_AREACONTAINER };
	CComboBox	m_itemnumpicker;
	CComboBox	m_containertype_control;
	CComboBox	m_containerpicker;
	//}}AFX_DATA

  area_container containercopy;
  area_vertex *vertexcopy;
  int vertexsize;
  area_item *itemcopy;
  int itemsize;

  int m_containernum;
  int m_itemnum;
  CString m_text;
  CStatic m_containericon;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAreaContainer)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  CToolTipCtrl m_tooltip;

  bool removeitems(int cnum);
	afx_msg void DefaultKillfocus();
	// Generated message map functions
	//{{AFX_MSG(CAreaContainer)
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusContainerpicker();
	afx_msg void OnSelchangeContainerpicker();
	afx_msg void OnAdd();
	afx_msg void OnRemove();
	afx_msg void OnCopy();
	afx_msg void OnPaste();
	afx_msg void OnIdentified();
	afx_msg void OnNosteal();
	afx_msg void OnStolen();
	afx_msg void OnBrowse();
	afx_msg void OnKillfocusItemnumpicker();
	afx_msg void OnSelchangeItemnumpicker();
	afx_msg void OnKillfocusFlags();
	afx_msg void OnKillfocusUse1();
	afx_msg void OnKillfocusUse2();
	afx_msg void OnKillfocusUse3();
	afx_msg void OnAdditem();
	afx_msg void OnDelitem();
	afx_msg void OnRecalcbox();
	afx_msg void OnKillfocusStrref();
	afx_msg void OnKillfocusText();
	afx_msg void OnBrowse2();
	afx_msg void OnBrowse3();
	afx_msg void OnHidden();
	afx_msg void OnNopc();
	afx_msg void OnSet();
	afx_msg void OnEditpolygon();
	afx_msg void OnNodrop();
	afx_msg void OnSelection();
	afx_msg void OnFit();
	afx_msg void OnTreset();
	afx_msg void OnKillfocusLocked();
	afx_msg void OnNocut();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CAreaVariable dialog

class CAreaVariable : public CPropertyPage
{
	DECLARE_DYNCREATE(CAreaVariable)
// Construction
public:
	CAreaVariable();   // standard constructor
	~CAreaVariable();   
	void RefreshVariable();  

// Dialog Data
	//{{AFX_DATA(CAreaVariable)
	enum { IDD = IDD_AREAVARIABLE };
	CComboBox	m_notepicker;
	CComboBox	m_variablepicker;
	//}}AFX_DATA

  area_variable variablecopy;
  area_mapnote notecopy;

  int m_variablenum;
  int m_notenum;
	CString	m_infostr;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAreaVariable)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  CToolTipCtrl m_tooltip;

	// Generated message map functions
	//{{AFX_MSG(CAreaVariable)
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusVariablepicker();
	afx_msg void OnSelchangeVariablepicker();
	afx_msg void OnKillfocusNotepicker();
	afx_msg void OnSelchangeNotepicker();
	afx_msg void OnKillfocusVariable();
	afx_msg void OnKillfocusUnknown20();
	afx_msg void OnKillfocusValue();
	afx_msg void OnKillfocusPos1x();
	afx_msg void OnKillfocusPos1y();
	afx_msg void OnKillfocusStrref();
	afx_msg void OnKillfocusColor();
	afx_msg void OnKillfocusNote();
	afx_msg void OnKillfocusUnknownc();
	afx_msg void OnAdd();
	afx_msg void OnRemove();
	afx_msg void OnCopy();
	afx_msg void OnPaste();
	afx_msg void OnUnknown();
	afx_msg void OnAdd2();
	afx_msg void OnRemove2();
	afx_msg void OnCopy2();
	afx_msg void OnPaste2();
	afx_msg void OnUnknown2();
	afx_msg void OnKillfocusUnknown24();
	afx_msg void OnTagged();
	afx_msg void OnSet();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CAreaDoor dialog

class CAreaDoor : public CPropertyPage
{
	DECLARE_DYNCREATE(CAreaDoor)
// Construction
public:
	CAreaDoor();   // standard constructor
	~CAreaDoor();  
  void RefreshDoor();   

// Dialog Data
	//{{AFX_DATA(CAreaDoor)
	enum { IDD = IDD_AREADOOR };
	CComboBox	m_regionpicker;
	CSpinButtonCtrl	m_spin_control;
	CComboBox	m_doorpicker;
	BOOL	m_openclose;
	//}}AFX_DATA

  area_door doorcopy;
  area_vertex *vertexcopy[4];
  int vertexsizes[4];
  HBITMAP hbd;

  int m_doornum;
  CString m_infostr;
  CStatic m_cursoricon;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAreaDoor)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  CToolTipCtrl m_tooltip;

  int AddWedDoor(CString doorid);
  int RemoveWedDoor(char *doorid);
	afx_msg void DefaultKillfocus();
	// Generated message map functions
	//{{AFX_MSG(CAreaDoor)
	afx_msg void OnAdd();
	afx_msg void OnRemove();
	afx_msg void OnCopy();
	afx_msg void OnPaste();
	afx_msg void OnKillfocusDoorpicker();
	afx_msg void OnSelchangeDoorpicker();
	virtual BOOL OnInitDialog();
	afx_msg void OnOpenClose();
	afx_msg void OnRecalcbox();
	afx_msg void OnKillfocusStrref();
	afx_msg void OnKillfocusInfostr();
	afx_msg void OnFlag1();
	afx_msg void OnFlag2();
	afx_msg void OnFlag3();
	afx_msg void OnFlag4();
	afx_msg void OnFlag5();
	afx_msg void OnFlag6();
	afx_msg void OnFlag7();
	afx_msg void OnFlag8();
	afx_msg void OnFlag9();
	afx_msg void OnFlag10();
	afx_msg void OnFlag11();
	afx_msg void OnFlag12();
	afx_msg void OnKillfocusArea();
	afx_msg void OnBrowse();
	afx_msg void OnBrowse2();
	afx_msg void OnPlay();
	afx_msg void OnPlaysound();
	afx_msg void OnBrowse3();
	afx_msg void OnBrowse4();
	afx_msg void OnUnknown();
	afx_msg void OnBrowse5();
	afx_msg void OnChangeCursoridx();
	afx_msg void OnSet();
	afx_msg void OnEditblock();
	afx_msg void OnEditpolygon();
	afx_msg void OnSet2();
	afx_msg void OnSet3();
	afx_msg void OnSelection();
	afx_msg void OnFlag13();
	afx_msg void OnFlag14();
	afx_msg void OnFlag15();
	afx_msg void OnFlag16();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CAreaAnim dialog

class CAreaAnim : public CPropertyPage
{
	DECLARE_DYNCREATE(CAreaAnim)
// Construction
public:
	CAreaAnim();   // standard constructor
	~CAreaAnim();  
  void RefreshAnim();   

// Dialog Data
	//{{AFX_DATA(CAreaAnim)
	enum { IDD = IDD_AREAANIM };
	CStatic	m_bamframe;
	CButton	m_schedule_control;
	CComboBox	m_animpicker;
	CString	m_timeofday;
	//}}AFX_DATA
  CString bma;
  HBITMAP hbanim, hbma;
  Cbam the_anim;
  COLORREF bgcolor;
  bool play;
  int playmax, playindex;
  area_anim animcopy;
  int m_animnum;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAreaAnim)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  CToolTipCtrl m_tooltip;

	afx_msg void DefaultKillfocus();
  void RefreshFrameSize();
	// Generated message map functions
	//{{AFX_MSG(CAreaAnim)
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusFrame();
	afx_msg void OnKillfocusCycle();
	afx_msg void OnKillfocusAnimpicker();
	afx_msg void OnSelchangeAnimpicker();
	afx_msg void OnFlag1();
	afx_msg void OnFlag2();
	afx_msg void OnFlag3();
	afx_msg void OnFlag4();
	afx_msg void OnFlag5();
	afx_msg void OnFlag6();
	afx_msg void OnFlag7();
	afx_msg void OnFlag8();
	afx_msg void OnFlag9();
	afx_msg void OnFlag10();
	afx_msg void OnFlag11();
	afx_msg void OnFlag12();
	afx_msg void OnFlag13();
	afx_msg void OnFlag14();
	afx_msg void OnFlag15();
	afx_msg void OnFlag16();
	afx_msg void OnAdd();
	afx_msg void OnRemove();
	afx_msg void OnCopy();
	afx_msg void OnPaste();
	afx_msg void OnBrowse2();
	afx_msg void OnBrowse();
	afx_msg void OnSchedule();
	afx_msg void OnPlay();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnFit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CAreaMap dialog

class CAreaMap : public CPropertyPage
{
	DECLARE_DYNCREATE(CAreaMap)
// Construction
public:
	CAreaMap();   // standard constructor
	~CAreaMap();  
  bool CanHaveNightMap();
  void RefreshMap(bool init);   

// Dialog Data
	//{{AFX_DATA(CAreaMap)
	enum { IDD = IDD_AREAMAP };
	CButton	m_special_control;
	CComboBox	m_value_control;
	CStatic	m_bitmap;
	int		m_maptype;
	//}}AFX_DATA
  int m_set;
  bool m_showall;
  CPoint m_mousepoint;
  CPoint m_oladjust;
  HBITMAP hbmap;
  LPBYTE the_map;
  COLORREF *the_palette;
  COLORREF bgcolor;
  int m_function, m_adjust;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAreaMap)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  CToolTipCtrl m_tooltip;

  void Allocatemap(bool allocate);
  void ResetCombo();
  void AddTravelRegions();

	// Generated message map functions
	//{{AFX_MSG(CAreaMap)
	afx_msg void OnHeightmap();
	afx_msg void OnLightmap();
	afx_msg void OnSearchmap();
	afx_msg void OnDefaultKillfocus();
	afx_msg void OnClear();
	afx_msg void OnSet();
	afx_msg void OnInit();
	afx_msg void OnMap();
	virtual BOOL OnInitDialog();
	afx_msg void OnEdit();
	afx_msg void OnUndo();
	afx_msg void OnPalette();
	afx_msg void OnSpecial();
	afx_msg void OnRefresh();
	afx_msg void OnNightmap();
	afx_msg void OnHeightmap2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CAreaProj dialog

class CAreaProj : public CPropertyPage
{
	DECLARE_DYNCREATE(CAreaProj)
// Construction
public:
	CAreaProj();   // standard constructor
	~CAreaProj();  
  void RefreshProj();   

// Dialog Data
	//{{AFX_DATA(CAreaProj)
	enum { IDD = IDD_AREAPROJ };
	CComboBox	m_trappicker;
	CListBox	m_effect_control;
	//}}AFX_DATA

  int m_trapnum;
 	int	effectnum;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAreaProj)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  CToolTipCtrl m_tooltip;

	// Generated message map functions
	//{{AFX_MSG(CAreaProj)
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusProjpicker();
	afx_msg void OnSelchangeProjpicker();
	afx_msg void OnRemove();
	afx_msg void OnBrowse();
	afx_msg void OnAdd();
	afx_msg void DefaultKillfocus();
	afx_msg void OnAdd2();
	afx_msg void OnRemove2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CAreaPropertySheet

class CAreaPropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CAreaPropertySheet)

// Construction
public:
	CAreaPropertySheet(CWnd* pWndParent = NULL);

// Attributes
public:
	CAreaGeneral m_PageGeneral;
	CAreaActor m_PageActor;
	CAreaTrigger m_PageTrigger;
	CAreaSpawn m_PageSpawn;
	CAreaEntrance m_PageEntrance;
	CAreaContainer m_PageContainer;
	CAreaAmbient m_PageAmbient;
	CAreaDoor m_PageDoor;
	CAreaVariable m_PageVariable;
	CAreaAnim m_PageAnim;
	CAreaMap m_PageMap;
	CAreaProj m_PageProj;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAreaPropertySheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAreaPropertySheet();
  void RefreshDialog();

protected:
// Generated message map functions
	//{{AFX_MSG(CAreaPropertySheet)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_AREAPROPERTIES_H__426C03AC_1D92_4AA7_8E0C_832BD0062BBF__INCLUDED_)
