// StoreProperties.h : header file
//

#ifndef __STOREPROPERTIES_H__
#define __STOREPROPERTIES_H__

/////////////////////////////////////////////////////////////////////////////
// CStoreGeneral dialog

class CStoreGeneral : public CPropertyPage
{
	DECLARE_DYNCREATE(CStoreGeneral)

// Construction
public:
	CStoreGeneral();
	~CStoreGeneral();
  void RefreshGeneral();

protected:
  CToolTipCtrl m_tooltip;
// Dialog Data
	//{{AFX_DATA(CStoreGeneral)
	enum { IDD = IDD_STOREGENERAL };
	CComboBox	m_storetype_control;
	CString	m_storename;
	BOOL	m_storenametag;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CStoreGeneral)
	public:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CStoreGeneral)
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusStoretype();
	afx_msg void OnSelling();
	afx_msg void OnBuying();
	afx_msg void OnIdentify();
	afx_msg void OnStealing();
	afx_msg void OnCures();
	afx_msg void OnDonate();
	afx_msg void OnDrinks();
	afx_msg void OnUnknown1();
	afx_msg void OnUnknown2();
	afx_msg void OnFenced();
	afx_msg void OnUnknown3();
	afx_msg void OnUnknown4();
	afx_msg void OnUnknown5();
	afx_msg void OnUnknown6();
	afx_msg void OnUnknown7();
	afx_msg void OnUnknown8();
	afx_msg void OnKillfocusStorenameref();
	afx_msg void OnKillfocusStorename();
	afx_msg void OnKillfocusBuyingpercent();
	afx_msg void OnKillfocusSellingpercent();
	afx_msg void OnKillfocusDeprecation();
	afx_msg void OnKillfocusRumourres();
	afx_msg void OnKillfocusLore();
	afx_msg void OnKillfocusStealingpercent();
	afx_msg void OnKillfocusIdPrice();
	afx_msg void OnKillfocusIconres();
	afx_msg void OnKillfocusCapacity();
	afx_msg void OnV10();
	afx_msg void OnV11();
	afx_msg void OnV90();
	afx_msg void OnBrowse();
	afx_msg void OnBrowse2();
	afx_msg void OnKillfocusDialogres();
	afx_msg void OnBrowse1();
	afx_msg void OnTagged();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////
// CStoreRental dialog

class CStoreRental : public CPropertyPage
{
	DECLARE_DYNCREATE(CStoreRental)

// Construction
public:
	CStoreRental();
	~CStoreRental();
  void RefreshRental();

protected:
  CToolTipCtrl m_tooltip;
// Dialog Data
	//{{AFX_DATA(CStoreRental)
	enum { IDD = IDD_STORERENTAL };
	CString	m_ubytes;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CStoreRental)
	public:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CStoreRental)
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusPeasantprice();
	afx_msg void OnKillfocusMerchantprice();
	afx_msg void OnKillfocusNobleprice();
	afx_msg void OnKillfocusRoyalprice();
	afx_msg void OnPeasant();
	afx_msg void OnMerchant();
	afx_msg void OnNoble();
	afx_msg void OnRoyal();
	afx_msg void OnKillfocusUnknown();
	afx_msg void OnClearall();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CStoreItems dialog

class CStoreItems : public CPropertyPage
{
	DECLARE_DYNCREATE(CStoreItems)

// Construction
public:
	CStoreItems();
	~CStoreItems();
  void RefreshItems();

protected:
  CToolTipCtrl m_tooltip;
  Citem my_item;
// Dialog Data
	//{{AFX_DATA(CStoreItems)
	enum { IDD = IDD_STOREITEMS };
	CButton	m_itemtype_button;
	CComboBox	m_stored_control;
	CComboBox	m_itemtype_picker_control;
	CComboBox	m_itemtype_control;
	CString m_itemtype;
	CString	m_itemres;
	int	m_count;
	int	m_infinite;
	int	m_flags;
	short	m_timer;
	int		m_strref;
	CString	m_trigger;
	//}}AFX_DATA
  Citem tmpitem;
  int updateflags;
  short m_usages[3];

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CStoreItems)
	public:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
protected:
  CString ResolveKey(CString key);
  void UpdateItemtypepicker();
  void UpdateStoreditempicker();
  void RefreshEntry(int count);
  void DisplayEntry(int count);
	// Generated message map functions
	//{{AFX_MSG(CStoreItems)
	virtual BOOL OnInitDialog();
	afx_msg void OnAdditem();
	afx_msg void OnAdditem2();
	afx_msg void OnKillfocusAlloweditems();
	afx_msg void OnRemoveitem();
	afx_msg void OnRemoveitem2();
	afx_msg void OnRemallitem();
	afx_msg void OnKillfocusItemtype();
	afx_msg void OnKillfocusStoreditems();
	afx_msg void OnKillfocusItemres();
	afx_msg void OnKillfocusCount();
	afx_msg void OnKillfocusUnlimited();
	afx_msg void OnKillfocusUse1();
	afx_msg void OnKillfocusUse2();
	afx_msg void OnKillfocusUse3();
	afx_msg void OnRemallitem2();
	afx_msg void OnKillfocusUnknown();
	afx_msg void OnSelchangeAlloweditems();
	afx_msg void OnSelchangeStoreditems();
	afx_msg void OnIdentified();
	afx_msg void OnStolen();
	afx_msg void OnKillfocusFlags();
	afx_msg void OnNosteal();
	afx_msg void OnBrowse();
	afx_msg void OnOrder();
	afx_msg void OnKillfocusStrref();
	afx_msg void OnKillfocusScript();
	afx_msg void OnNew();
	afx_msg void OnRecharges();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////
// CStoreDrinks dialog

class CStoreDrinks : public CPropertyPage
{
	DECLARE_DYNCREATE(CStoreDrinks)

// Construction
public:
	CStoreDrinks();
	~CStoreDrinks();
  void RefreshDrinks();

protected:
  CToolTipCtrl m_tooltip;
// Dialog Data
	//{{AFX_DATA(CStoreDrinks)
	enum { IDD = IDD_STOREDRINKS };
	CButton	m_spell_button;
	CComboBox	m_spellresref_control;
	CComboBox	m_drink_control;
	CComboBox	m_spell_control;
	int		m_curedescref;
	int		m_drinknameref;
	int		m_drinkprice;
	int		m_cureprice;
	CString	m_spellresref;
	CString	m_drinkresref;
	CString	m_curedesc;
	CString	m_drinkname;
	int		m_strength;
	//}}AFX_DATA
  int updateflags;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CStoreDrinks)
	public:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
protected:
  void UpdateSpellpicker();
  void UpdateDrinkpicker();
  void RefreshEntry(int count);
  void DisplayEntry(int count);
  void RefreshSpell(int count);
  void DisplaySpell(int count);
  void RefreshDrink(int count);
  void DisplayDrink(int count);
	// Generated message map functions
	//{{AFX_MSG(CStoreDrinks)
	virtual BOOL OnInitDialog();
	afx_msg void OnAdddrink();
	afx_msg void OnAddspell();
	afx_msg void OnRemovedrink();
	afx_msg void OnRemovespell();
	afx_msg void OnRemallspell();
	afx_msg void OnRemalldrink();
	afx_msg void OnKillfocusSpells();
	afx_msg void OnSelchangeSpells();
	afx_msg void OnKillfocusSpellresref();
	afx_msg void OnKillfocusCuredescref();
	afx_msg void OnKillfocusCurename();
	afx_msg void OnKillfocusCureprice();
	afx_msg void OnKillfocusDrinks();
	afx_msg void OnSelchangeDrinks();
	afx_msg void OnKillfocusDrinkresref();
	afx_msg void OnKillfocusDrinknameref();
	afx_msg void OnKillfocusDrinkname();
	afx_msg void OnKillfocusDrinkprice();
	afx_msg void OnKillfocusDrinkstrength();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CStoreExtra dialog

class CStoreExtra : public CPropertyPage
{
	DECLARE_DYNCREATE(CStoreExtra)

// Construction
public:
	CStoreExtra();
	~CStoreExtra();
  void RefreshExtra();

protected:
  CToolTipCtrl m_tooltip;
  int page;
// Dialog Data
	//{{AFX_DATA(CStoreExtra)
	enum { IDD = IDD_STOREEXTRA };
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CStoreExtra)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CStoreExtra)
	afx_msg void OnPage();
	afx_msg void OnClearall();
	afx_msg void OnKillfocusCapacity();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CStorePropertySheet

class CStorePropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CStorePropertySheet)

// Construction
public:
	CStorePropertySheet(CWnd* pWndParent = NULL);

// Attributes
public:
	CStoreGeneral m_PageGeneral;
  CStoreRental m_PageRental;
	CStoreItems m_PageItems;
  CStoreDrinks m_PageDrinks;
	CStoreExtra m_PageExtra;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStorePropertySheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CStorePropertySheet();
  void RefreshDialog();

protected:
// Generated message map functions
	//{{AFX_MSG(CStorePropertySheet)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // __STOREPROPERTIES_H__
