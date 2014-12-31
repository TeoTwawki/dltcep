// ItemProperties.h : header file
//

#ifndef __ITEMPROPERTIES_H__
#define __ITEMPROPERTIES_H__

/////////////////////////////////////////////////////////////////////////////
// CItemGeneral dialog

class CItemGeneral : public CPropertyPage
{
	DECLARE_DYNCREATE(CItemGeneral)

// Construction
public:
	CItemGeneral();
	~CItemGeneral();
  void RefreshGeneral();

protected:
  CToolTipCtrl m_tooltip;

// Dialog Data
	//{{AFX_DATA(CItemGeneral)
	enum { IDD = IDD_ITEMGENERAL };
	CStatic	m_itmcolor;
	CComboBox	m_colorpicker_control;
	CComboBox	m_proficiencypicker_control;
	CComboBox	m_animationpicker_control;
	CComboBox	m_itemtypepicker_control;
	//}}AFX_DATA

  HBITMAP hbb;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CItemGeneral)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
  protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CItemGeneral)
	virtual BOOL OnInitDialog();
	afx_msg void OnDefaultKillfocus();
	afx_msg void OnV1();
	afx_msg void OnV11();
	afx_msg void OnV20();
	afx_msg void OnItmcolor();
	afx_msg void OnKillfocusColor();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////
// CItemIcons dialog

class CItemIcons : public CPropertyPage
{
	DECLARE_DYNCREATE(CItemIcons)

// Construction
public:
	CItemIcons();
	~CItemIcons();  
  void RefreshIcons();

// Dialog Data
	//{{AFX_DATA(CItemIcons)
	enum { IDD = IDD_ITEMICONS };
	CStatic	m_grnicon;
	CStatic	m_invicon2;
	CStatic	m_invicon1;
	int   	m_convref;
	CString	m_conv;
	//}}AFX_DATA
  CStatic m_descicon[4];
  CString bmi, bmg, bmd;
  CPoint topleft;
  HBITMAP hbmi1, hbmi2;
  HBITMAP hbmg;
  HBITMAP hbmd[4];
  //CStringMapInt itemflags;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CItemIcons)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  CToolTipCtrl m_tooltip;

  inline void Togglebit(int value)
  {
    the_item.header.itmattr^=value;  
    the_item.m_changed=true;
  }
  inline void Togglebit2(int value)
  {
    the_item.header.splattr^=value;  
    the_item.m_changed=true;
  }
	// Generated message map functions
	//{{AFX_MSG(CItemIcons)
	virtual BOOL OnInitDialog();
	afx_msg void OnBow();
	afx_msg void OnCold();
	afx_msg void OnConversible();
	afx_msg void OnCopyable();
	afx_msg void OnCritical();
	afx_msg void OnCursed();
	afx_msg void OnDisplayable();
	afx_msg void OnFlag1();
	afx_msg void OnFlag2();
	afx_msg void OnFlag3();
	afx_msg void OnFlag4();
	afx_msg void OnMagical();
	afx_msg void OnMovable();
	afx_msg void OnSilver();
	afx_msg void OnStolen();
	afx_msg void OnTwohand();
	afx_msg void OnKillfocusConvref();
	afx_msg void OnKillfocusUsedup();
	afx_msg void OnKillfocusIteminvicon();
	afx_msg void OnKillfocusItemgrndicon();
	afx_msg void OnKillfocusItemdescicon();
	afx_msg void OnKillfocusFlags();
	afx_msg void OnIcenter();
	afx_msg void OnGcenter();
	afx_msg void OnDcenter();
	afx_msg void OnItemexcl();
	afx_msg void OnPlaysound();
	afx_msg void OnInvicon();
	afx_msg void OnGrndicon();
	afx_msg void OnDescicon();
	afx_msg void OnKillfocusDialog();
	afx_msg void OnBrowse1();
	afx_msg void OnBrowse2();
	afx_msg void OnBrowse3();
	afx_msg void OnBrowse4();
	afx_msg void OnBrowse();
	afx_msg void OnFlag5();
	afx_msg void OnFlag6();
	afx_msg void OnFlag7();
	afx_msg void OnFlag8();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};


/////////////////////////////////////////////////////////////////////////////
// CItemDescription dialog

class CItemDescription : public CPropertyPage
{
	DECLARE_DYNCREATE(CItemDescription)

// Construction
public:
	CItemDescription();
	~CItemDescription();
  void RefreshDescription();

// Dialog Data
	//{{AFX_DATA(CItemDescription)
	enum { IDD = IDD_ITEMDESC };
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
	//{{AFX_VIRTUAL(CItemDescription)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  CToolTipCtrl m_tooltip;

	// Generated message map functions
	//{{AFX_MSG(CItemDescription)
	afx_msg void OnKillfocusLongdescref();
	afx_msg void OnKillfocusLongref();
	afx_msg void OnKillfocusShortdescref();
	afx_msg void OnKillfocusShortref();
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusShortdesc();
	afx_msg void OnKillfocusLongdesc();
	afx_msg void OnKillfocusShortname();
	afx_msg void OnKillfocusLongname();
	afx_msg void OnShortnametag();
	afx_msg void OnShortdesctag();
	afx_msg void OnLongnametag();
	afx_msg void OnLongdesctag();
	afx_msg void OnNew1();
	afx_msg void OnNew2();
	afx_msg void OnNew3();
	afx_msg void OnNew4();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};


/////////////////////////////////////////////////////////////////////////////
// CItemUsability dialog

class CItemUsability : public CPropertyPage
{
	DECLARE_DYNCREATE(CItemUsability)

// Construction
public:
	CItemUsability();
	~CItemUsability();
  void RefreshUsability();

// Dialog Data
	//{{AFX_DATA(CItemUsability)
	enum { IDD = IDD_ITEMUSE };
	//}}AFX_DATA
  unsigned char bytes[8];

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CItemUsability)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  CToolTipCtrl m_tooltip;

  inline void Togglebit(int byte, int value)
  {
    bytes[byte]^=value;
    the_item.header.usability[0]=bytes[0]; 
    the_item.header.usability[1]=bytes[1];
    the_item.header.usability[2]=bytes[2];
    the_item.header.usability[3]=bytes[3];
    the_item.header.kits1=bytes[4];
    the_item.header.kits2=bytes[5];
    the_item.header.kits3=bytes[6];
    the_item.header.kits4=bytes[7];
    the_item.m_changed=true;
  }

	// Generated message map functions
	//{{AFX_MSG(CItemUsability)
	afx_msg void OnAbjurer();
	afx_msg void OnAssassin();
	afx_msg void OnAvenger();
	afx_msg void OnBarbarian();
	afx_msg void OnBard();
	afx_msg void OnBaseclass();
	afx_msg void OnBeast();
	afx_msg void OnBerserker();
	afx_msg void OnBlade();
	afx_msg void OnBounty();
	afx_msg void OnCavalier();
	afx_msg void OnChaotic();
	afx_msg void OnCleric();
	afx_msg void OnClmage();
	afx_msg void OnClranger();
	afx_msg void OnClthief();
	afx_msg void OnConjurer();
	afx_msg void OnDiviner();
	afx_msg void OnDruid();
	afx_msg void OnDwarf();
	afx_msg void OnEgneutral();
	afx_msg void OnElf();
	afx_msg void OnEnchanter();
	afx_msg void OnEvil();
	afx_msg void OnFerlain();
	afx_msg void OnFicleric();
	afx_msg void OnFidruid();
	afx_msg void OnFighter();
	afx_msg void OnFimacleric();
	afx_msg void OnFimage();
	afx_msg void OnFimathief();
	afx_msg void OnFithief();
	afx_msg void OnGnome();
	afx_msg void OnGood();
	afx_msg void OnHalfElf();
	afx_msg void OnHalfling();
	afx_msg void OnHalforc();
	afx_msg void OnHelm();
	afx_msg void OnHuman();
	afx_msg void OnIllusionist();
	afx_msg void OnInquisitor();
	afx_msg void OnInvoker();
	afx_msg void OnJester();
	afx_msg void OnKensai();
	afx_msg void OnLathander();
	afx_msg void OnLawful();
	afx_msg void OnLcneutral();
	afx_msg void OnMage();
	afx_msg void OnMathief();
	afx_msg void OnMonk();
	afx_msg void OnNecromancer();
	afx_msg void OnPaladin();
	afx_msg void OnRanger();
	afx_msg void OnShape();
	afx_msg void OnSkald();
	afx_msg void OnStalker();
	afx_msg void OnSwash();
	afx_msg void OnTalos();
	afx_msg void OnThief();
	afx_msg void OnTotemic();
	afx_msg void OnTransmuter();
	afx_msg void OnUndeadhunter();
	afx_msg void OnWildmage();
	afx_msg void OnWizardslayer();
	virtual BOOL OnInitDialog();
	afx_msg void OnAllset();
	afx_msg void OnAllunset();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};


/////////////////////////////////////////////////////////////////////////////
// CItemEquip dialog

class CItemEquip : public CPropertyPage
{
	DECLARE_DYNCREATE(CItemEquip)

// Construction
public:
	CItemEquip();
	~CItemEquip();
  void UpdateEquippicker();
//  void DoDataExchangeEquip(CDataExchange* pDX);
  void EnableWindow_Equip(bool value);
  void RefreshEquip();

// Dialog Data
	//{{AFX_DATA(CItemEquip)
	enum { IDD = IDD_ITEMEQUIP };
	CListBox	m_equipnum_control;
	//}}AFX_DATA
	int	equipnum;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CItemEquip)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  CToolTipCtrl m_tooltip;

	// Generated message map functions
	//{{AFX_MSG(CItemEquip)
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusEquipnum();
	afx_msg void OnEquipadd();
	afx_msg void OnEquipcopy();
	afx_msg void OnEquippaste();
	afx_msg void OnEquipremove();
	afx_msg void OnEdit();
	afx_msg void OnOrder();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CItemExtended dialog

class CItemExtended : public CPropertyPage
{
	DECLARE_DYNCREATE(CItemExtended)

// Construction
public:
	CItemExtended();
	~CItemExtended();
  void UpdateExtheadpicker();
  void DoDataExchangeEffects(CDataExchange* pDX);
  void DoDataExchangeExtended(CDataExchange* pDX);
  void EnableWindow_ExtEffect(bool value);
  void EnableWindow_Extended(bool value, bool second);
  void RefreshExtended();
// Dialog Data
	//{{AFX_DATA(CItemExtended)
	enum { IDD = IDD_ITEMEFFECT };
	CComboBox	m_sectype_control;
	CComboBox	m_school_control;
	CComboBox	m_melee_control;
  CStatic m_useicon_control;
	CComboBox	m_projid_control;
	CComboBox	m_resist_control;
	CComboBox	m_timing_control;
	CComboBox	m_efftarget_control;
	CComboBox	m_stype_control;
	CComboBox	m_exteffopcode_control;
	CComboBox	m_chargetype_control;
	CComboBox	m_exteffnum_control;
	CComboBox	m_damagetype_control;
	CComboBox	m_extheadnum_control;
	CComboBox	m_projframe_control;
	CComboBox	m_target_control;
	CComboBox	m_loc_control;
	CComboBox	m_exttype_control;
	CString	m_headnum;
	CString	m_effnum;
	//}}AFX_DATA
  // real counters
	int		m_extheadnum;
	int		m_exteffnum;
	tooltip_data m_tooltipref;
	CString	m_tooltipstring[3];
	CString	m_opcode;
  int extheadnum;
  int exteffnum;
  CString bmu;
  HBITMAP hbmu;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CItemExtended)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
  protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  CToolTipCtrl m_tooltip;

	// Generated message map functions
	//{{AFX_MSG(CItemExtended)
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusExteffnum();
	afx_msg void OnKillfocusExtheadnum();
	afx_msg void OnExtpaste();
	afx_msg void OnExteffadd();
	afx_msg void OnExteffcopy();
	afx_msg void OnExteffpaste();
	afx_msg void OnExteffremove();
	afx_msg void OnExtcopy();
	afx_msg void OnExtadd();
	afx_msg void OnExtremove();
	afx_msg void OnKillfocusUnknown1();
	afx_msg void OnSelchangeExtheadnum();
	afx_msg void OnKillfocusThac0();
	afx_msg void OnKillfocusRoll();
	afx_msg void OnKillfocusDamagetype();
	afx_msg void OnKillfocusDie();
	afx_msg void OnKillfocusAdd();
	afx_msg void OnKillfocusTooltiptext();
	afx_msg void OnKillfocusTooltipref();
	afx_msg void OnDefaultKillfocus();
	afx_msg void OnKillfocusAnim1();
	afx_msg void OnKillfocusAnim2();
	afx_msg void OnKillfocusAnim3();
	afx_msg void OnBow();
	afx_msg void OnXbow();
	afx_msg void OnMisc();
	afx_msg void OnKillfocusExtuseicon();
	afx_msg void OnUseicon();
	afx_msg void OnEdit();
	afx_msg void OnId();
	afx_msg void OnNoid();
	afx_msg void OnSelchangeMelee();
	afx_msg void OnFlag1();
	afx_msg void OnFlag2();
	afx_msg void OnFlag11();
	afx_msg void OnFlag12();
	afx_msg void OnRecharges();
	afx_msg void OnStrbonus();
	afx_msg void OnBreakable();
	afx_msg void OnHostile();
	afx_msg void OnKillfocusSectype();
	afx_msg void OnKillfocusSchool();
	afx_msg void OnStrthac0();
	afx_msg void OnDamage();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////
// CItemGeneral dialog

class CItemExtra : public CPropertyPage
{
	DECLARE_DYNCREATE(CItemExtra)

// Construction
public:
	CItemExtra();
	~CItemExtra();
  void RefreshExtra();

protected:
// Dialog Data
	//{{AFX_DATA(CItemExtra)
	enum { IDD = IDD_ITEMEXTRA };
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CItemExtra)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
  protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  CToolTipCtrl m_tooltip;

	// Generated message map functions
	//{{AFX_MSG(CItemExtra)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CItemPropertySheet

class CItemPropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CItemPropertySheet)

// Construction
public:
	CItemPropertySheet(CWnd* pWndParent = NULL);

// Attributes
public:
	CItemGeneral m_PageGeneral;
	CItemIcons m_PageIcons;
	CItemDescription m_PageDescription;
	CItemUsability m_PageUsability;
	CItemEquip m_PageEquip;
	CItemExtended m_PageExtended;
	CItemExtra m_PageExtra;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CItemPropertySheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CItemPropertySheet();
  void RefreshDialog();

protected:
// Generated message map functions
	//{{AFX_MSG(CItemPropertySheet)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // __ITEMPROPERTIES_H__
