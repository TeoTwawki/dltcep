// StoreProperties.cpp : implementation file
//

#include "stdafx.h"

#include "chitem.h"
#include "resource.h"
#include "store.h"
#include "2da.h"
#include "StoreProperties.h"
#include "compat.h"
#include "options.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CStoreGeneral, CPropertyPage)
IMPLEMENT_DYNCREATE(CStoreRental, CPropertyPage)
IMPLEMENT_DYNCREATE(CStoreItems, CPropertyPage)
IMPLEMENT_DYNCREATE(CStoreExtra, CPropertyPage)

/////////////////////////////////////////////////////////////////////////////
// CStoreGeneral property page

CStoreGeneral::CStoreGeneral() : CPropertyPage(CStoreGeneral::IDD)
{
	//{{AFX_DATA_INIT(CStoreGeneral)
	m_storename = _T("");
	//}}AFX_DATA_INIT
}

CStoreGeneral::~CStoreGeneral()
{
}

static int flagboxids[16]={
IDC_BUYING, IDC_SELLING,IDC_IDENTIFY,IDC_STEALING,IDC_CURES, IDC_DONATE, IDC_DRINKS,
IDC_UNKNOWN1, IDC_UNKNOWN2, IDC_FENCED, IDC_UNKNOWN3, IDC_UNKNOWN4, IDC_UNKNOWN5,
IDC_UNKNOWN6, IDC_UNKNOWN7, IDC_UNKNOWN8};

void CStoreGeneral::DoDataExchange(CDataExchange* pDX)
{
  CString tmpstr;
  int i,j;
  CButton *box;

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStoreGeneral)
	DDX_Control(pDX, IDC_STORETYPE, m_storetype_control);
	DDX_Text(pDX, IDC_STORENAME, m_storename);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_STORENAMEREF, the_store.header.strref);
	DDX_Text(pDX, IDC_SELLINGPERCENT, the_store.header.selling);
	DDX_Text(pDX, IDC_BUYINGPERCENT, the_store.header.buying);
	DDX_Text(pDX, IDC_STEALPERCENT, the_store.header.steal);
	DDX_Text(pDX, IDC_CAPACITY, the_store.header.capacity);
	DDX_Text(pDX, IDC_PRICECHANGE, the_store.header.depreciation);
	DDX_Text(pDX, IDC_LORE, the_store.header.lore);
	DDX_Text(pDX, IDC_IDPRICE, the_store.header.idprice);
  if(pDX->m_bSaveAndValidate==UD_RETRIEVE)
  {
  	DDX_Text(pDX, IDC_STORETYPE, tmpstr);
    the_store.header.type=strtonum(tmpstr);

	  DDX_Text(pDX, IDC_DIALOGRES, tmpstr);
    StoreResref(tmpstr, the_store.header.dlgresref1);
  	DDX_Text(pDX, IDC_RUMOURRES, tmpstr);
    StoreResref(tmpstr, the_store.header.dlgresref2);
	  DDX_Text(pDX, IDC_ICONRES, tmpstr);
    StoreResref(tmpstr, the_store.header.dlgresref3);
  }
  else
  {
    tmpstr=format_storetype(the_store.header.type);
  	DDX_Text(pDX, IDC_STORETYPE, tmpstr);

    RetrieveResref(tmpstr, the_store.header.dlgresref1);
  	DDX_Text(pDX, IDC_DIALOGRES, tmpstr);
  	DDV_MaxChars(pDX, tmpstr, 8);
    RetrieveResref(tmpstr, the_store.header.dlgresref2);
  	DDX_Text(pDX, IDC_RUMOURRES, tmpstr);
  	DDV_MaxChars(pDX, tmpstr, 8);
    RetrieveResref(tmpstr, the_store.header.dlgresref3);
	  DDX_Text(pDX, IDC_ICONRES, tmpstr);
  	DDV_MaxChars(pDX, tmpstr, 8);

    j=1;
    for(i=0;i<16;i++)
    {
      box =(CButton *)GetDlgItem(flagboxids[i]);
      if(box)
      {
        box->SetCheck(!!(the_store.header.flags&j));
      }
      j<<=1;
    }
  }
}

BOOL CStoreGeneral::OnInitDialog() 
{
  int x;
  CString tmpstr;

	CPropertyPage::OnInitDialog();
  RefreshGeneral();
  m_storetype_control.ResetContent();
  for(x=0;x<NUM_STORETYPE+1;x++)
  {
    m_storetype_control.AddString(format_storetype(x));
  }
  UpdateData(UD_DISPLAY);
	return TRUE;
}

//move itemdata to local variables
static int radioids[3]={IDC_V10,IDC_V11,IDC_V90};

void CStoreGeneral::RefreshGeneral()
{
  int i;
  int id;
  CButton *cb;

  switch(the_store.revision)
  {
  case 10:
    id=0;
    break;
  case 11:
    id=1;
    break;
  case 90:
    id=2;
    break;
  }
  for(i=0;i<3;i++)
  {
    cb=(CButton *) GetDlgItem(radioids[i]);
    if(cb)
    {
      if(i==id) cb->SetCheck(true);
      else cb->SetCheck(false);
    }
  }
  m_storename=resolve_tlk_text(the_store.header.strref);
}

BEGIN_MESSAGE_MAP(CStoreGeneral, CPropertyPage)
	//{{AFX_MSG_MAP(CStoreGeneral)
	ON_CBN_KILLFOCUS(IDC_STORETYPE, OnKillfocusStoretype)
	ON_BN_CLICKED(IDC_SELLING, OnSelling)
	ON_BN_CLICKED(IDC_BUYING, OnBuying)
	ON_BN_CLICKED(IDC_IDENTIFY, OnIdentify)
	ON_BN_CLICKED(IDC_STEALING, OnStealing)
	ON_BN_CLICKED(IDC_CURES, OnCures)
	ON_BN_CLICKED(IDC_DONATE, OnDonate)
	ON_BN_CLICKED(IDC_DRINKS, OnDrinks)
	ON_BN_CLICKED(IDC_UNKNOWN1, OnUnknown1)
	ON_BN_CLICKED(IDC_UNKNOWN2, OnUnknown2)
	ON_BN_CLICKED(IDC_FENCED, OnFenced)
	ON_BN_CLICKED(IDC_UNKNOWN3, OnUnknown3)
	ON_BN_CLICKED(IDC_UNKNOWN4, OnUnknown4)
	ON_BN_CLICKED(IDC_UNKNOWN5, OnUnknown5)
	ON_BN_CLICKED(IDC_UNKNOWN6, OnUnknown6)
	ON_BN_CLICKED(IDC_UNKNOWN7, OnUnknown7)
	ON_BN_CLICKED(IDC_UNKNOWN8, OnUnknown8)
	ON_EN_KILLFOCUS(IDC_STORENAMEREF, OnKillfocusStorenameref)
	ON_EN_KILLFOCUS(IDC_STORENAME, OnKillfocusStorename)
	ON_EN_KILLFOCUS(IDC_BUYINGPERCENT, OnKillfocusBuyingpercent)
	ON_EN_KILLFOCUS(IDC_SELLINGPERCENT, OnKillfocusSellingpercent)
	ON_EN_KILLFOCUS(IDC_PRICECHANGE, OnKillfocusDeprecation)
	ON_EN_KILLFOCUS(IDC_RUMOURRES, OnKillfocusRumourres)
	ON_EN_KILLFOCUS(IDC_LORE, OnKillfocusLore)
	ON_EN_KILLFOCUS(IDC_STEALPERCENT, OnKillfocusStealingpercent)
	ON_EN_KILLFOCUS(IDC_IDPRICE, OnKillfocusIdPrice)
	ON_EN_KILLFOCUS(IDC_ICONRES, OnKillfocusIconres)
	ON_EN_KILLFOCUS(IDC_CAPACITY, OnKillfocusCapacity)
	ON_BN_CLICKED(IDC_V10, OnV10)
	ON_BN_CLICKED(IDC_V11, OnV11)
	ON_BN_CLICKED(IDC_V90, OnV90)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_BROWSE2, OnBrowse2)
	ON_EN_KILLFOCUS(IDC_DIALOGRES, OnKillfocusDialogres)
	ON_BN_CLICKED(IDC_BROWSE1, OnBrowse1)
	ON_BN_CLICKED(IDC_TAGGED, OnTagged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CStoreGeneral::OnKillfocusStoretype() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);	
}

void CStoreGeneral::OnSelling() 
{
	the_store.header.flags^=ST_SELL;
}

void CStoreGeneral::OnBuying() 
{
	the_store.header.flags^=ST_BUY;
}

void CStoreGeneral::OnIdentify() 
{
	the_store.header.flags^=ST_ID;
}

void CStoreGeneral::OnStealing() 
{
	the_store.header.flags^=ST_STEAL;
}

void CStoreGeneral::OnCures() 
{
	the_store.header.flags^=ST_CURE;
}

void CStoreGeneral::OnDonate() 
{
	the_store.header.flags^=ST_DONAT;
}

void CStoreGeneral::OnDrinks() 
{
	the_store.header.flags^=ST_DRINK;
}

void CStoreGeneral::OnUnknown1() 
{
	the_store.header.flags^=ST_UNKN1;
}

void CStoreGeneral::OnUnknown2() 
{
	the_store.header.flags^=ST_UNKN2;
}

void CStoreGeneral::OnFenced() 
{
	the_store.header.flags^=ST_FENCE;
}

void CStoreGeneral::OnUnknown3() 
{
	the_store.header.flags^=ST_UNKN3;
}

void CStoreGeneral::OnUnknown4() 
{
	the_store.header.flags^=ST_UNKN4;
}

void CStoreGeneral::OnUnknown5() 
{
	the_store.header.flags^=ST_UNKN5;
}

void CStoreGeneral::OnUnknown6() 
{
	the_store.header.flags^=ST_UNKN6;
}

void CStoreGeneral::OnUnknown7() 
{
	the_store.header.flags^=ST_UNKN7;
}

void CStoreGeneral::OnUnknown8() 
{
	the_store.header.flags^=ST_UNKN8;
}

void CStoreGeneral::OnKillfocusStorenameref() 
{
  UpdateData(UD_RETRIEVE);
  RefreshGeneral();
  UpdateData(UD_DISPLAY);  
}

void CStoreGeneral::OnTagged() 
{
  if(editflg&TLKCHANGE)
  {
    if(MessageBox("Do you want to update dialog.tlk?","Store editor",MB_YESNO)!=IDYES)
    {
      return;
    }      
  }
	toggle_tlk_tag(the_store.header.strref);
  UpdateData(UD_DISPLAY);
}

void CStoreGeneral::OnKillfocusStorename() 
{
  CString tmpstr;
  
  UpdateData(UD_RETRIEVE);
  tmpstr=resolve_tlk_text(the_store.header.strref);
  if(tmpstr!=m_storename)
  {
    if(editflg&TLKCHANGE)
    {
      if(MessageBox("Do you want to update dialog.tlk?","Store editor",MB_YESNO)!=IDYES)
      {
        return;
      }      
    }
    //this handles the reference changes and adds a new string if needed
    the_store.header.strref=store_tlk_text(the_store.header.strref,m_storename);
    RefreshGeneral();
    //    WriteTlk();
  }
  UpdateData(UD_DISPLAY);
}

void CStoreGeneral::OnKillfocusBuyingpercent() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CStoreGeneral::OnKillfocusDeprecation() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CStoreGeneral::OnKillfocusDialogres() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CStoreGeneral::OnKillfocusLore() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CStoreGeneral::OnKillfocusSellingpercent() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CStoreGeneral::OnKillfocusStealingpercent() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CStoreGeneral::OnKillfocusCapacity() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CStoreGeneral::OnKillfocusIdPrice() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CStoreGeneral::OnKillfocusRumourres() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CStoreGeneral::OnKillfocusIconres() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CStoreGeneral::OnBrowse() 
{
  pickerdlg.m_restype=REF_DLG;
  RetrieveResref(pickerdlg.m_picked,the_store.header.dlgresref3);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_store.header.dlgresref3);
  }
	UpdateData(UD_DISPLAY);	
}

void CStoreGeneral::OnBrowse1() 
{
  pickerdlg.m_restype=REF_DLG;
  RetrieveResref(pickerdlg.m_picked,the_store.header.dlgresref2);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_store.header.dlgresref2);
  }
	UpdateData(UD_DISPLAY);	
}

void CStoreGeneral::OnBrowse2() 
{
  pickerdlg.m_restype=REF_DLG;
  RetrieveResref(pickerdlg.m_picked,the_store.header.dlgresref1);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_store.header.dlgresref1);
  }
	UpdateData(UD_DISPLAY);	
}

void CStoreGeneral::OnV10() 
{
	the_store.revision=10;
	
}

void CStoreGeneral::OnV11() 
{
	the_store.revision=11;
	
}

void CStoreGeneral::OnV90() 
{
	the_store.revision=90;
	
}

/////////////////////////////////////////////////////////////////////////////
// CStoreRental property page

CStoreRental::CStoreRental() : CPropertyPage(CStoreRental::IDD)
{
	//{{AFX_DATA_INIT(CStoreRental)
	//}}AFX_DATA_INIT
  m_ubytes=CString(" ",36);
}

CStoreRental::~CStoreRental()
{
}

int rentalboxids[4]={
IDC_PEASANT, IDC_MERCHANT, IDC_NOBLE, IDC_ROYAL,
};
void CStoreRental::DoDataExchange(CDataExchange* pDX)
{
  CButton *box;
  int i,j;

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStoreRental)
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_PEASANTPRICE, the_store.header.prices[0]);
	DDX_Text(pDX, IDC_MERCHANTPRICE, the_store.header.prices[1]);
	DDX_Text(pDX, IDC_NOBLEPRICE, the_store.header.prices[2]);
	DDX_Text(pDX, IDC_ROYALPRICE, the_store.header.prices[3]);
	DDX_Text(pDX, IDC_UNKNOWN, the_store.header.rent);
  for(i=0;i<36;i++)
  {
  	DDX_Text(pDX, IDC_U1+i, the_store.header.unknown[i]);
    if(isprint(the_store.header.unknown[i]) )
    {
      m_ubytes.SetAt(i,the_store.header.unknown[i]);
    }
    else
    {
      m_ubytes.SetAt(i,' ');
    }
  }
  if(pDX->m_bSaveAndValidate==UD_RETRIEVE)
  {
  }
  else
  {
  	DDX_Text(pDX, IDC_UBYTES, m_ubytes);
	  DDV_MaxChars(pDX, m_ubytes, 36);
    j=1;
    for(i=0;i<16;i++)
    {
      box =(CButton *)GetDlgItem(rentalboxids[i]);
      if(box)
      {
        box->SetCheck(!!(the_store.header.rent&j));
      }
      j<<=1;
    }
  }
}

BOOL CStoreRental::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	RefreshRental();
//
//
  UpdateData(UD_DISPLAY);
	return TRUE; 
}
//move itemdata to local variables
void CStoreRental::RefreshRental()
{
}

BEGIN_MESSAGE_MAP(CStoreRental, CPropertyPage)
	//{{AFX_MSG_MAP(CStoreRental)
	ON_EN_KILLFOCUS(IDC_PEASANTPRICE, OnKillfocusPeasantprice)
	ON_EN_KILLFOCUS(IDC_MERCHANTPRICE, OnKillfocusMerchantprice)
	ON_EN_KILLFOCUS(IDC_NOBLEPRICE, OnKillfocusNobleprice)
	ON_EN_KILLFOCUS(IDC_ROYALPRICE, OnKillfocusRoyalprice)
	ON_BN_CLICKED(IDC_PEASANT, OnPeasant)
	ON_BN_CLICKED(IDC_MERCHANT, OnMerchant)
	ON_BN_CLICKED(IDC_NOBLE, OnNoble)
	ON_BN_CLICKED(IDC_ROYAL, OnRoyal)
	ON_EN_KILLFOCUS(IDC_UNKNOWN, OnKillfocusUnknown)
	ON_BN_CLICKED(IDC_CLEARALL, OnClearall)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CStoreRental::OnKillfocusPeasantprice() 
{
	UpdateData(UD_RETRIEVE);
  if(the_store.header.prices[0])
  {
    the_store.header.rent|=1;
  }
  else
  {
    the_store.header.rent&=~1;
  }
	UpdateData(UD_DISPLAY);
}

void CStoreRental::OnKillfocusMerchantprice() 
{
	UpdateData(UD_RETRIEVE);
  if(the_store.header.prices[1])
  {
    the_store.header.rent|=2;
  }
  else
  {
    the_store.header.rent&=~2;
  }
	UpdateData(UD_DISPLAY);
}

void CStoreRental::OnKillfocusNobleprice() 
{
	UpdateData(UD_RETRIEVE);
  if(the_store.header.prices[2])
  {
    the_store.header.rent|=4;
  }
  else
  {
    the_store.header.rent&=~4;
  }
	UpdateData(UD_DISPLAY);
}

void CStoreRental::OnKillfocusRoyalprice() 
{
	UpdateData(UD_RETRIEVE);
  if(the_store.header.prices[3])
  {
    the_store.header.rent|=8;
  }
  else
  {
    the_store.header.rent&=~8;
  }
	UpdateData(UD_DISPLAY);
}

void CStoreRental::OnPeasant() 
{
	the_store.header.rent^=1;
  if(!(the_store.header.rent&1))
  {
    the_store.header.prices[0]=0;
  }
	UpdateData(UD_DISPLAY);
}

void CStoreRental::OnMerchant() 
{
	the_store.header.rent^=2;	
  if(!(the_store.header.rent&2))
  {
    the_store.header.prices[1]=0;
  }
	UpdateData(UD_DISPLAY);
}

void CStoreRental::OnNoble() 
{
	the_store.header.rent^=4;	
  if(!(the_store.header.rent&4))
  {
    the_store.header.prices[2]=0;
  }
	UpdateData(UD_DISPLAY);
}

void CStoreRental::OnRoyal() 
{
	the_store.header.rent^=8;	
  if(!(the_store.header.rent&8))
  {
    the_store.header.prices[3]=0;
  }
	UpdateData(UD_DISPLAY);
}

void CStoreRental::OnKillfocusUnknown() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

BOOL CStoreRental::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
  int idx;

	// TODO: Add your specialized code here and/or call the base class
  if((nCode==512) && (nID>=IDC_U1 && nID<=IDC_U36))
  {
    idx=nID-IDC_U1;
  	UpdateData(UD_RETRIEVE);
	  UpdateData(UD_DISPLAY);
  }
	
	return CPropertyPage::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CStoreRental::OnClearall() 
{
  memset(the_store.header.unknown,0,sizeof(the_store.header.unknown));	
  UpdateData(UD_DISPLAY);
}

/////////////////////////////////////////////////////////////////////////////
// CStoreItems property page

CStoreItems::CStoreItems() : CPropertyPage(CStoreItems::IDD)
{
	//{{AFX_DATA_INIT(CStoreItems)
	m_itemtype = _T("");
	m_itemres = _T("");
	m_count = 0;
	m_infinite = 0;
	m_flags = 0;
	m_unknown = 0;
	m_curedescref = 0;
	m_drinknameref = 0;
	m_drinkprice = 0;
	m_cureprice = 0;
	m_spellresref = _T("");
	m_drinkresref = _T("");
	m_curedesc = _T("");
	m_drinkname = _T("");
	m_strength = 0;
	//}}AFX_DATA_INIT
  updateflags=-1;
  memset(m_usages,0,sizeof(m_usages));
}

CStoreItems::~CStoreItems()
{
}

int attrboxids[8]={IDC_IDENTIFIED,IDC_NOSTEAL,IDC_STOLEN,0,0,0,0,0};

void CStoreItems::DoDataExchange(CDataExchange* pDX)
{
  int i,j;
  CButton *cb;

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStoreItems)
	DDX_Control(pDX, IDC_REMALLSPELL, m_spell_button);
	DDX_Control(pDX, IDC_SPELLRESREF, m_spellresref_control);
	DDX_Control(pDX, IDC_DRINKS, m_drink_control);
	DDX_Control(pDX, IDC_SPELLS, m_spell_control);
	DDX_Control(pDX, IDC_REMALLITEM, m_itemtype_button);
	DDX_Control(pDX, IDC_STOREDITEMS, m_stored_control);
	DDX_Control(pDX, IDC_ITEMTYPE, m_itemtype_picker_control);
	DDX_Control(pDX, IDC_ALLOWEDITEMS, m_itemtype_control);
	//}}AFX_DATA_MAP
  if(pDX->m_bSaveAndValidate==UD_DISPLAY)
  {
    UpdateItemtypepicker();
    UpdateStoreditempicker();
    UpdateSpellpicker();
    UpdateDrinkpicker();
    j=1;
    for(i=0;i<8;i++)
    {
      if(attrboxids[i])
      {
        cb=(CButton *) GetDlgItem(attrboxids[i]);
        cb->SetCheck(!!(m_flags&j));
      }
      j<<=1;
    }
  }
	DDX_Text(pDX, IDC_ITEMTYPE, m_itemtype);
  DDX_Text(pDX, IDC_ITEMRES, m_itemres);
  DDV_MaxChars(pDX, m_itemres, 8);
	DDX_Text(pDX, IDC_COUNT, m_count);
	DDX_Text(pDX, IDC_UNLIMITED, m_infinite);
	DDX_Text(pDX, IDC_FLAGS, m_flags);
	DDX_Text(pDX, IDC_UNKNOWN, m_unknown);
  for(i=0;i<3;i++)
  {
    DDX_Text(pDX, IDC_USE1+i,m_usages[i]);
    DDV_MinMaxInt(pDX, m_usages[i],0,65535);
  }
	DDX_Text(pDX, IDC_CUREDESCREF, m_curedescref);
	DDX_Text(pDX, IDC_DRINKNAMEREF, m_drinknameref);
	DDX_Text(pDX, IDC_DRINKPRICE, m_drinkprice);
	DDX_Text(pDX, IDC_CUREPRICE, m_cureprice);
	DDX_Text(pDX, IDC_SPELLRESREF, m_spellresref);
	DDX_Text(pDX, IDC_DRINKRESREF, m_drinkresref);
	DDX_Text(pDX, IDC_CURENAME, m_curedesc);
	DDX_Text(pDX, IDC_DRINKNAME, m_drinkname);
	DDX_Text(pDX, IDC_DRINKSTRENGTH, m_strength);
}

void CStoreItems::RefreshSpeldesc()
{
  POSITION pos;
  CString key;
  tooltip_data value;

  m_spellresref_control.ResetContent();
  pos=store_spell_desc.GetStartPosition();
  while(pos)
  {
    store_spell_desc.GetNextAssoc(pos, key, value);
    m_spellresref_control.AddString(key);
  }
}

BOOL CStoreItems::OnInitDialog() 
{
  int x;

	CPropertyPage::OnInitDialog();
	RefreshItems();
  m_itemtype_control.SetDroppedWidth(150);
  m_stored_control.SetDroppedWidth(150);
  for(x=0;x<NUM_ITEMTYPE;x++)
  {
    m_itemtype_picker_control.AddString(format_itemtype(x) );
  }
  RefreshSpeldesc();
  updateflags=-1;
  UpdateData(UD_DISPLAY);
	return TRUE;
}

int ordering(const void *a,const void *b)
{
  if(*(long*) a>*(long *) b) return 1;
  if(*(long *) a==*(long *) b) return 0;
  return -1;
}

void CStoreItems::RefreshItems()
{
  int x, y;
  int old;
  long *newitemtypes;
  
  if(the_store.itemtypenum>1)
  {
    qsort(the_store.itemtypes, the_store.itemtypenum, sizeof(long), ordering);
    old=the_store.itemtypes[0];
    y=1;
    for(x=1;x<the_store.itemtypenum;x++)
    {
      if(the_store.itemtypes[x]!=old)
      {
        the_store.itemtypes[y++]=old=the_store.itemtypes[x];
      }
    }
    if(y!=the_store.itemtypenum)
    {
      newitemtypes = new long[y];
      if(newitemtypes)
      {
        memcpy(newitemtypes,the_store.itemtypes, y*sizeof(long) );
        delete [] the_store.itemtypes;
        the_store.itemtypes=newitemtypes;
        the_store.itemtypenum=y;
        the_store.header.pitemcount=y;
        MessageBox("Consolidated itemtypes (corrected minor problem)","Warning",MB_OK);
      }
      else MessageBox("Out of memory","Error",MB_OK);
    }
  }
  updateflags=-1;
}

void CStoreItems::UpdateItemtypepicker()
{
  CString tmpstr;
  int x;

  if(updateflags&1)
  {
    m_itemtype_control.ResetContent();
    for(x=0;x<the_store.itemtypenum;x++)
    {
      m_itemtype=format_itemtype(the_store.itemtypes[x]);
      tmpstr.Format("%2d (%s)",x,m_itemtype);
      m_itemtype_control.AddString(tmpstr);
    }
    if(x)
    {
      m_itemtype_control.SetCurSel(0);
      m_itemtype_button.SetWindowText("Remove All");
    }
    else
    {
      m_itemtype_button.SetWindowText("Add All");
    }
  }
  updateflags&=~1;
}

static int storedboxids[]={
  IDC_COUNT, IDC_UNLIMITED, IDC_USE1, IDC_USE2, IDC_USE3, IDC_FLAGS,
  IDC_IDENTIFIED, IDC_NOSTEAL, IDC_STOLEN, IDC_UNKNOWN,
0};

void CStoreItems::UpdateStoreditempicker()
{
  CString tmpstr;
  int x;
  CWnd *cb;

  if(updateflags&2)
  {
    m_stored_control.ResetContent();
    for(x=0;x<the_store.entrynum;x++)
    {
      RetrieveResref(m_itemres, the_store.entries[x].itemname);
      tmpstr.Format("%d (%s)",x,m_itemres);
      m_stored_control.AddString(tmpstr);
    }
    if(x)
    {
      m_stored_control.SetCurSel(0);
      DisplayEntry(0);
      for(x=0;storedboxids[x];x++)
      {
        cb=(CButton *) GetDlgItem(storedboxids[x]);
        cb->EnableWindow(true);
      }
    }
    else
    {
      for(x=0;storedboxids[x];x++)
      {
        cb=GetDlgItem(storedboxids[x]);
        cb->EnableWindow(false);
      }
    }
  }
  updateflags&=~2;
}

static int spellboxids[]={IDC_CUREDESCREF, IDC_CURENAME, IDC_CUREPRICE,
0};

void CStoreItems::UpdateSpellpicker()
{
  CString tmpstr, spellname;
  int x, pos;
  tooltip_data refs;
  CWnd *cb;

  if(updateflags&4)
  {
    pos=m_spell_control.GetCurSel();
    if(pos<0) pos=0;
    m_spell_control.ResetContent();
    for(x=0;x<the_store.curenum;x++)
    {
      RetrieveResref(m_spellresref, the_store.cures[x].curename);
      if(store_spell_desc.Lookup(m_spellresref, refs))
      {
        spellname=resolve_tlk_text(refs.data[1]);
      }
      else spellname="<not in speldesc.2da>";
      tmpstr.Format("%d (%s) %s",x,m_spellresref,spellname);
      m_spell_control.AddString(tmpstr);
    }
    m_spell_control.SetCurSel(pos);
    if(x)
    {
      DisplaySpell(pos);
      for(x=0;spellboxids[x];x++)
      {
        cb=(CButton *) GetDlgItem(spellboxids[x]);
        cb->EnableWindow(true);
      }
      m_spell_button.SetWindowText("Remove All");
    }
    else
    {
      for(x=0;spellboxids[x];x++)
      {
        cb=GetDlgItem(spellboxids[x]);
        cb->EnableWindow(false);
      }
      m_spell_button.SetWindowText("Add All");
    }
  }
  updateflags&=~4;
}

int drinkboxids[]={IDC_DRINKRESREF, IDC_DRINKNAMEREF,
   IDC_DRINKNAME, IDC_DRINKPRICE, IDC_DRINKSTRENGTH,
0};

void CStoreItems::UpdateDrinkpicker()
{
  CString tmpstr;
  int x;
  CWnd *cb;

  if(updateflags&8)
  {
    m_drink_control.ResetContent();
    for(x=0;x<the_store.drinknum;x++)
    {
      m_drinkname=resolve_tlk_text(the_store.drinks[x].drinkname);
      tmpstr.Format("%d %s",x, m_drinkname);
      m_drink_control.AddString(tmpstr);
    }
    if(x)
    {
      m_drink_control.SetCurSel(0);
      DisplayDrink(0);
      for(x=0;drinkboxids[x];x++)
      {
        cb=(CButton *) GetDlgItem(drinkboxids[x]);
        cb->EnableWindow(true);
      }
    }
    else
    {
      for(x=0;drinkboxids[x];x++)
      {
        cb=GetDlgItem(drinkboxids[x]);
        cb->EnableWindow(false);
      }
    }
  }
  updateflags&=~8;
}

BEGIN_MESSAGE_MAP(CStoreItems, CPropertyPage)
	//{{AFX_MSG_MAP(CStoreItems)
	ON_BN_CLICKED(IDC_ADDDRINK, OnAdddrink)
	ON_BN_CLICKED(IDC_ADDITEM, OnAdditem)
	ON_BN_CLICKED(IDC_ADDITEM2, OnAdditem2)
	ON_BN_CLICKED(IDC_ADDSPELL, OnAddspell)
	ON_CBN_KILLFOCUS(IDC_ALLOWEDITEMS, OnKillfocusAlloweditems)
	ON_BN_CLICKED(IDC_REMOVEITEM, OnRemoveitem)
	ON_BN_CLICKED(IDC_REMOVEDRINK, OnRemovedrink)
	ON_BN_CLICKED(IDC_REMOVESPELL, OnRemovespell)
	ON_BN_CLICKED(IDC_REMOVEITEM2, OnRemoveitem2)
	ON_BN_CLICKED(IDC_REMALLITEM, OnRemallitem)
	ON_CBN_KILLFOCUS(IDC_ITEMTYPE, OnKillfocusItemtype)
	ON_CBN_KILLFOCUS(IDC_STOREDITEMS, OnKillfocusStoreditems)
	ON_EN_KILLFOCUS(IDC_ITEMRES, OnKillfocusItemres)
	ON_EN_KILLFOCUS(IDC_COUNT, OnKillfocusCount)
	ON_EN_KILLFOCUS(IDC_UNLIMITED, OnKillfocusUnlimited)
	ON_EN_KILLFOCUS(IDC_USE1, OnKillfocusUse1)
	ON_EN_KILLFOCUS(IDC_USE2, OnKillfocusUse2)
	ON_EN_KILLFOCUS(IDC_USE3, OnKillfocusUse3)
	ON_BN_CLICKED(IDC_REMALLITEM2, OnRemallitem2)
	ON_BN_CLICKED(IDC_REMALLSPELL, OnRemallspell)
	ON_BN_CLICKED(IDC_REMALLDRINK, OnRemalldrink)
	ON_EN_KILLFOCUS(IDC_UNKNOWN, OnKillfocusUnknown)
	ON_CBN_SELCHANGE(IDC_ALLOWEDITEMS, OnSelchangeAlloweditems)
	ON_CBN_SELCHANGE(IDC_STOREDITEMS, OnSelchangeStoreditems)
	ON_BN_CLICKED(IDC_IDENTIFIED, OnIdentified)
	ON_BN_CLICKED(IDC_STOLEN, OnStolen)
	ON_EN_KILLFOCUS(IDC_FLAGS, OnKillfocusFlags)
	ON_CBN_KILLFOCUS(IDC_SPELLS, OnKillfocusSpells)
	ON_CBN_SELCHANGE(IDC_SPELLS, OnSelchangeSpells)
	ON_CBN_KILLFOCUS(IDC_SPELLRESREF, OnKillfocusSpellresref)
	ON_EN_KILLFOCUS(IDC_CUREDESCREF, OnKillfocusCuredescref)
	ON_EN_KILLFOCUS(IDC_CURENAME, OnKillfocusCurename)
	ON_EN_KILLFOCUS(IDC_CUREPRICE, OnKillfocusCureprice)
	ON_CBN_KILLFOCUS(IDC_DRINKS, OnKillfocusDrinks)
	ON_CBN_SELCHANGE(IDC_DRINKS, OnSelchangeDrinks)
	ON_EN_KILLFOCUS(IDC_DRINKRESREF, OnKillfocusDrinkresref)
	ON_EN_KILLFOCUS(IDC_DRINKNAMEREF, OnKillfocusDrinknameref)
	ON_EN_KILLFOCUS(IDC_DRINKNAME, OnKillfocusDrinkname)
	ON_EN_KILLFOCUS(IDC_DRINKPRICE, OnKillfocusDrinkprice)
	ON_EN_KILLFOCUS(IDC_DRINKSTRENGTH, OnKillfocusDrinkstrength)
	ON_BN_CLICKED(IDC_NOSTEAL, OnNosteal)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_ORDER, OnOrder)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// allowed items section //
void CStoreItems::OnKillfocusAlloweditems() 
{
  int old;

	UpdateData(UD_RETRIEVE);
  old=m_itemtype_control.GetCurSel();
  if(old>=0)
  {
    m_itemtype=format_itemtype(the_store.itemtypes[old]);
  }
  else m_itemtype.Empty();
  UpdateData(UD_DISPLAY);
}

void CStoreItems::OnSelchangeAlloweditems() 
{
  int old;

  old=m_itemtype_control.GetCurSel();
  if(old>=0)
  {
    m_itemtype=format_itemtype(the_store.itemtypes[old]);
  }
  else m_itemtype.Empty();
  UpdateData(UD_DISPLAY);
}

void CStoreItems::OnKillfocusItemtype() 
{
	UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CStoreItems::OnAdditem() 
{
  int x;
  long old;
  long *newitemtypes;

  old=strtonum(m_itemtype);
	for(x=0;(x<the_store.itemtypenum) && (the_store.itemtypes[x]<old);x++);
	if((x<the_store.itemtypenum) && (the_store.itemtypes[x]==old))
  {
    MessageBox("This item is already accepted.","Warning",MB_OK);
    return;
  }
  the_store.itemtypenum++;
  newitemtypes=new long[the_store.itemtypenum];
  if(!newitemtypes)
  {
    MessageBox("Out of memory","Warning",MB_OK);
    return;
  }
  memcpy(newitemtypes,the_store.itemtypes,x*sizeof(long) );
  newitemtypes[x]=old;
  memcpy(newitemtypes+x+1, the_store.itemtypes+x, (the_store.header.pitemcount-x)*sizeof(long) );
  delete [] the_store.itemtypes;
  the_store.itemtypes=newitemtypes;
  the_store.header.pitemcount=the_store.itemtypenum;
  RefreshItems();
  updateflags|=1;
  UpdateData(UD_DISPLAY);
}

void CStoreItems::OnRemoveitem() 
{
  int x;
  long old;
  long *newitemtypes;

  old=strtonum(m_itemtype);
	for(x=0;(x<the_store.itemtypenum) && (the_store.itemtypes[x]!=old);x++);
	if(x==the_store.itemtypenum)
  {
    MessageBox("This item is already removed.","Warning",MB_OK);
    return;
  }
  the_store.itemtypenum--;
  if(the_store.itemtypenum)
  {
    newitemtypes=new long[the_store.itemtypenum];
    if(!newitemtypes)
    {
      MessageBox("Out of memory","Warning",MB_OK);
      return;
    }
    memcpy(newitemtypes,the_store.itemtypes,x*sizeof(long) );
    memcpy(newitemtypes+x, the_store.itemtypes+x+1, (the_store.header.pitemcount-x-1)*sizeof(long) );
  }
  else newitemtypes=NULL;
  delete [] the_store.itemtypes;
  the_store.itemtypes=newitemtypes;
  the_store.header.pitemcount=the_store.itemtypenum;
  RefreshItems();
  updateflags|=1;
  UpdateData(UD_DISPLAY);
}

void CStoreItems::OnRemallitem() 
{
  int itemnum;
  int i;

  if(the_store.itemtypes)
  {
    delete [] the_store.itemtypes;
    the_store.itemtypes=NULL;
    the_store.header.pitemcount=the_store.itemtypenum=0;
  }
  else
  {//it was empty so we can safely create it
    the_store.itemtypes=new long[NUM_ITEMTYPE];
    itemnum=0;
    for(i=0;i<NUM_ITEMTYPE;i++)
    {
      if(!(itvs2h[i]&4) || (unlimited_itemtypes()) ) //maximum itemtype depends on setup
      {
        the_store.itemtypes[i]=i;
        itemnum++;
      }
    }
    the_store.header.pitemcount=the_store.itemtypenum=itemnum;
  }
  updateflags|=1;
  UpdateData(UD_DISPLAY);
}

// stored item section //
void CStoreItems::DisplayEntry(int count)
{
  RetrieveResref(m_itemres,the_store.entries[count].itemname);
  m_count=the_store.entries[count].count;
  m_flags=the_store.entries[count].flags;
  m_infinite=the_store.entries[count].infinite;
  m_unknown=the_store.entries[count].unknown08;
  memcpy(m_usages,the_store.entries[count].usages,sizeof(m_usages));
}

void CStoreItems::RefreshEntry(int count)
{
  memset(the_store.entries+count,0,sizeof(store_item_entry11));
  StoreResref(m_itemres,the_store.entries[count].itemname);
  the_store.entries[count].count=m_count;
  the_store.entries[count].flags=m_flags;
  the_store.entries[count].infinite=m_infinite;
  the_store.entries[count].unknown08=m_unknown;
  memcpy(the_store.entries[count].usages,m_usages,sizeof(m_usages));
}

void CStoreItems::OnKillfocusStoreditems() 
{
  int count;
  CString m_stored;

	UpdateData(UD_RETRIEVE);
  count=m_stored_control.GetCurSel();
  if(count<0)
  {
    m_stored_control.GetWindowText(m_stored);
    count=strtonum(m_stored);
    count=m_stored_control.SetCurSel(count);
  }
  if(count>=0)
  {
    DisplayEntry(count);
  }
  else m_itemres.Empty();
  UpdateData(UD_DISPLAY);
}

void CStoreItems::OnSelchangeStoreditems() 
{
  int count;

  count=m_stored_control.GetCurSel();
  if(count>=0)
  {
    DisplayEntry(count);
  }
  else m_itemres.Empty();
  UpdateData(UD_DISPLAY);
}

void CStoreItems::OnKillfocusItemres() 
{
  loc_entry dummy;
  CString tmpres;

  UpdateData(UD_RETRIEVE);
  if(m_itemres.IsEmpty()) return; 
  if(!items.Lookup(m_itemres, dummy) )
  {
    MessageBox("Non-existent item!","Warning",MB_OK);
  }
	UpdateData(UD_DISPLAY);
}

void CStoreItems::OnKillfocusCount() 
{
  int count;

	UpdateData(UD_RETRIEVE);
  count=m_stored_control.GetCurSel();
  if(count>=0)
  {
    if(!m_count) m_infinite=1; //change to unlimited
    else m_infinite=0;
    RefreshEntry(count);
  }
  UpdateData(UD_DISPLAY);
}

void CStoreItems::OnKillfocusUnlimited() 
{
  int count;

	UpdateData(UD_RETRIEVE);
  count=m_stored_control.GetCurSel();
  if(count>=0)
  {
    if(m_infinite) m_count=0; //change to unlimited
    else if(!m_count) m_count=1;
    RefreshEntry(count);
  }
  UpdateData(UD_DISPLAY);
}

void CStoreItems::OnKillfocusUse1() 
{
  int count;

	UpdateData(UD_RETRIEVE);
  count=m_stored_control.GetCurSel();
  if(count>=0)
  {
    RefreshEntry(count);
  }
  UpdateData(UD_DISPLAY);
}

void CStoreItems::OnKillfocusUse2() 
{
  int count;

	UpdateData(UD_RETRIEVE);
  count=m_stored_control.GetCurSel();
  if(count>=0)
  {
    RefreshEntry(count);
  }
  UpdateData(UD_DISPLAY);
}

void CStoreItems::OnKillfocusUse3() 
{
  int count;

	UpdateData(UD_RETRIEVE);
  count=m_stored_control.GetCurSel();
  if(count>=0)
  {
    RefreshEntry(count);
  }
  UpdateData(UD_DISPLAY);
}

void CStoreItems::OnKillfocusFlags() 
{
  int count;

	UpdateData(UD_RETRIEVE);
  count=m_stored_control.GetCurSel();
  if(count>=0)
  {
    RefreshEntry(count);
  }
  UpdateData(UD_DISPLAY);
}

void CStoreItems::OnIdentified() 
{
  int count;

	UpdateData(UD_RETRIEVE);
  count=m_stored_control.GetCurSel();
  if(count>=0)
  {
    m_flags^=STI_IDENTIFIED;
    RefreshEntry(count);
  }
  UpdateData(UD_DISPLAY);
}

void CStoreItems::OnNosteal() 
{
  int count;

	UpdateData(UD_RETRIEVE);
  count=m_stored_control.GetCurSel();
  if(count>=0)
  {
    m_flags^=STI_NOSTEAL;
    RefreshEntry(count);
  }
  UpdateData(UD_DISPLAY);
}

void CStoreItems::OnStolen() 
{
  int count;

	UpdateData(UD_RETRIEVE);
  count=m_stored_control.GetCurSel();
  if(count>=0)
  {
    m_flags^=STI_STOLEN;
    RefreshEntry(count);
  }
  UpdateData(UD_DISPLAY);
}

void CStoreItems::OnKillfocusUnknown() 
{
  int count;

	UpdateData(UD_RETRIEVE);
  count=m_stored_control.GetCurSel();
  if(count>=0)
  {
    RefreshEntry(count);
  }
  UpdateData(UD_DISPLAY);
}

void CStoreItems::OnAdditem2() 
{
  int old;
  CString tmpres;
  store_item_entry11 *newentries;

	UpdateData(UD_RETRIEVE);
  if(m_itemres.IsEmpty())
  {
    MessageBox("Empty item resource.","Warning",MB_OK);
  	UpdateData(UD_DISPLAY);
    return;
  }
  for(old=0;old<the_store.entrynum;old++)
  {
    RetrieveResref(tmpres,the_store.entries[old].itemname);
    if(tmpres==m_itemres)
    {
      m_stored_control.SetCurSel(old);
      MessageBox("Item is already on the list!","Warning",MB_OK);
    	UpdateData(UD_DISPLAY);
      return;
    }
  }
  old=m_stored_control.GetCurSel()+1;
  the_store.entrynum++;
  newentries = new store_item_entry11[the_store.entrynum];
  if(the_store.header.itemcount)
  {
    memcpy(newentries,the_store.entries,old*sizeof(store_item_entry11));
    memcpy(newentries+old+1,the_store.entries+old,(the_store.header.itemcount-old)*sizeof(store_item_entry11));
    delete [] the_store.entries;
  }
  else old=0;
  the_store.entries=newentries;
  m_count=1;//default values
  m_flags=1;                      
  RefreshEntry(old); //this copies all data into an itementry
  the_store.header.itemcount=the_store.entrynum;
  updateflags|=2;
  UpdateData(UD_DISPLAY);
}

void CStoreItems::OnRemoveitem2() 
{
  int old;
  CString tmpres;
  store_item_entry11 *newentries;

	UpdateData(UD_RETRIEVE);
  if(m_itemres.IsEmpty())
  {
    MessageBox("Empty item resource.","Warning",MB_OK);
  	UpdateData(UD_DISPLAY);
    return;
  }
  for(old=0;old<the_store.entrynum;old++)
  {
    RetrieveResref(tmpres,the_store.entries[old].itemname);
    if(tmpres==m_itemres)
    {
      //remove it now
      the_store.entrynum--;
      if(the_store.entrynum)
      {
        newentries = new store_item_entry11[the_store.entrynum];
        memcpy(newentries,the_store.entries,old*sizeof(store_item_entry11) );
        memcpy(newentries+old,the_store.entries+old+1,(the_store.header.itemcount-old-1) * sizeof(store_item_entry11) );
      }
      else newentries=NULL;

      delete [] the_store.entries;
      the_store.entries=newentries;
      the_store.header.itemcount=the_store.entrynum;

      updateflags|=2;
    	UpdateData(UD_DISPLAY);
      return;
    }
  }
  MessageBox("Item is not on the list!","Warning",MB_OK);
  UpdateData(UD_DISPLAY);
}

void CStoreItems::OnRemallitem2() 
{
  if(the_store.entries)
  {
    delete [] the_store.entries;
    the_store.entries=NULL;
  }
  the_store.header.itemcount=the_store.entrynum=0;
  updateflags|=2;
  UpdateData(UD_DISPLAY);
}

void CStoreItems::OnBrowse() 
{
  pickerdlg.m_restype=REF_ITM;
  pickerdlg.m_picked=m_itemres;
  if(pickerdlg.DoModal()==IDOK)
  {
    m_itemres=pickerdlg.m_picked;
    UpdateData(UD_DISPLAY);
  }
}

int order_store_items(const void *a, const void *b)
{
  int ita, itb;
  CString tmpstr;
  int fh;
  loc_entry tmploc;

  ita=-1;
  itb=-1;
  RetrieveResref(tmpstr,((store_item_entry11 *) a)->itemname);
  if(items.Lookup(tmpstr, tmploc))
  {
    fh=locate_file(tmploc, 0);
    ita=the_item.RetrieveItemType(fh);
  }
  RetrieveResref(tmpstr,((store_item_entry11 *) b)->itemname);
  if(items.Lookup(tmpstr, tmploc))
  {
    fh=locate_file(tmploc, 0);
    itb=the_item.RetrieveItemType(fh);
  }
  if(ita>itb) return 1;
  if(itb>ita) return -1;
  return strnicmp(((store_item_entry11 *) a)->itemname,((store_item_entry11 *) b)->itemname,8);
}

void CStoreItems::OnOrder() 
{
	qsort(the_store.entries,the_store.entrynum,sizeof(store_item_entry11), order_store_items);
  updateflags|=2;
	UpdateData(UD_DISPLAY);
}


//spells (cures) section//
void CStoreItems::DisplaySpell(int count)
{
  tooltip_data refs;

  RetrieveResref(m_spellresref,the_store.cures[count].curename);
  m_cureprice=the_store.cures[count].price;
  memset(&refs,-1,sizeof(refs));
  store_spell_desc.Lookup(m_spellresref, refs);
  m_curedescref=refs.data[0];
  m_curedesc=resolve_tlk_text(m_curedescref);
}

void CStoreItems::RefreshSpell(int count)
{
  StoreResref(m_spellresref,the_store.cures[count].curename);
  the_store.cures[count].price=m_cureprice;
  updateflags|=4;
}

void CStoreItems::OnKillfocusSpells() 
{
  int count;
  CString m_spell;

	UpdateData(UD_RETRIEVE);
  count=m_spell_control.GetCurSel();
  if(count<0)
  {
    m_spell_control.GetWindowText(m_spell);
    count=strtonum(m_spell);
    count=m_spell_control.SetCurSel(count);
  }
  if(count>=0)
  {
    DisplaySpell(count);
  }
  else m_spellresref.Empty();
  UpdateData(UD_DISPLAY);
}

void CStoreItems::OnSelchangeSpells() 
{
  int count;

  count=m_spell_control.GetCurSel();
  if(count>=0)
  {
    DisplaySpell(count);
  }
  else m_spellresref.Empty();
  UpdateData(UD_DISPLAY);
}

void CStoreItems::OnKillfocusSpellresref() 
{
  int count;
  loc_entry dummy;
  CString tmpres;

  UpdateData(UD_RETRIEVE);
  if(m_spellresref.IsEmpty()) return; 
  count=m_spell_control.GetCurSel();
  if(count<0) return;
  if(!spells.Lookup(m_spellresref, dummy) )
  {
    MessageBox("Non-existent spell!","Warning",MB_OK);
  }
  RefreshSpell(count);
	UpdateData(UD_DISPLAY);
}

CString speldesc_columns[]={"DESCRIPTION"};

void CStoreItems::OnKillfocusCuredescref() 
{
  Cspell my_spell;
  loc_entry fileloc;
  tooltip_data refs;
  int flg;
  int fh;

  UpdateData(UD_RETRIEVE);
  memset(&refs,-1,sizeof(refs));
  flg=store_spell_desc.Lookup(m_spellresref, refs);
  if((m_curedescref>0) && (!flg || (refs.data[0]!=m_curedescref) ) )
  {
    if(editflg&DACHANGE)
    {
      if(MessageBox("Do you want to update speldesc.2da?","Store editor",MB_YESNO)!=IDYES)
      {
        m_curedescref=refs.data[0];
        goto endofquest;
      }
    }
    m_curedesc=resolve_tlk_text(m_curedescref);
    refs.data[0]=m_curedescref;
    //lets change speldesc.2da
    if(refs.data[0]==-1)
    {
      store_spell_desc.RemoveKey(m_spellresref);
    }
    else
    {
      spells.Lookup(m_spellresref, fileloc);
      fh=locate_file(fileloc, 0);
      refs.data[1]=my_spell.RetrieveNameRef(fh);
      store_spell_desc.SetAt(m_spellresref, refs);
    }
    Write2daArrayToFile("SPELDESC", store_spell_desc, speldesc_columns, 1);
    updateflags|=4;
    RefreshSpeldesc();
  }
endofquest:
	UpdateData(UD_DISPLAY);
}

void CStoreItems::OnKillfocusCurename() 
{
  int count;
  CString tmpstr;
  tooltip_data refs;
  int flg;

  flg=store_spell_desc.Lookup(m_spellresref, refs);
  UpdateData(UD_RETRIEVE);
  count=m_spell_control.GetCurSel();
  if(count<0)
  {
    goto endofquest;
  }
  if(m_curedescref>=0)
  {
    tmpstr=resolve_tlk_text(m_curedescref);
    if(tmpstr!=m_curedesc)
    {
      if(editflg&TLKCHANGE)
      {
        if(MessageBox("Do you want to update dialog.tlk?","Store editor",MB_YESNO)!=IDYES)
        {
          m_curedesc=tmpstr;
          goto endofquest;
        }
      }
      //stores reference and saves dialog.tlk
      //it may change the reference to the first unused
      m_curedescref=store_tlk_text(m_curedescref, m_curedesc);
      //WriteTlk();
      //this is a new entry
      if(flg && (refs.data[0]!=m_curedescref) )
      {
        refs.data[0]=m_curedescref;
        store_spell_desc.SetAt(m_spellresref, refs); //
        Write2daArrayToFile("SPELDESC", store_spell_desc, speldesc_columns, 1);
      }
      updateflags|=4;
      RefreshSpeldesc();
    }
  }
endofquest:
	UpdateData(UD_DISPLAY);
}

void CStoreItems::OnKillfocusCureprice() 
{
  int count;

  UpdateData(UD_RETRIEVE);
  count=m_spell_control.GetCurSel();
  if(count>=0)
  {
    RefreshSpell(count);
  }
	UpdateData(UD_DISPLAY);
}

void CStoreItems::OnAddspell() 
{
  int old;
  CString tmpres;
  store_cure *newspells;

	UpdateData(UD_RETRIEVE);
  if(m_spellresref.IsEmpty())
  {
    MessageBox("Empty spell resource.","Warning",MB_OK);
  	UpdateData(UD_DISPLAY);
    return;
  }
  for(old=0;old<the_store.curenum;old++)
  {
    RetrieveResref(tmpres,the_store.cures[old].curename);
    if(tmpres==m_spellresref)
    {
      m_stored_control.SetCurSel(old);
      MessageBox("Item is already on the list!","Warning",MB_OK);
    	UpdateData(UD_DISPLAY);
      return;
    }
  }
  the_store.curenum++;
  newspells=new store_cure[the_store.curenum];
  if(!newspells)
  {
    MessageBox("Out of memory","Warning",MB_OK);
    return;
  }
  memcpy(newspells,the_store.cures,the_store.header.curecount*sizeof(store_cure) );
  delete [] the_store.cures;
  the_store.cures=newspells;
  RefreshSpell(the_store.header.curecount);
  the_store.header.curecount=the_store.curenum;
  updateflags|=4;
  UpdateData(UD_DISPLAY);
}

void CStoreItems::OnRemovespell() 
{
  int x;
  store_cure *newspells;

	UpdateData(UD_RETRIEVE);
  if(m_spellresref.IsEmpty())
  {
    MessageBox("Empty item resource.","Warning",MB_OK);
  	UpdateData(UD_DISPLAY);
    return;
  }
  for(x=0;x<the_store.curenum && memcmp(m_spellresref,the_store.cures[x].curename,8); x++);
	if(x==the_store.curenum)
  {
    MessageBox("This spell is already removed.","Warning",MB_OK);
    return;
  }
  the_store.curenum--;
  if(the_store.curenum)
  {
    newspells=new store_cure[the_store.curenum];
    if(!newspells)
    {
      MessageBox("Out of memory","Warning",MB_OK);
      return;
    }
    memcpy(newspells,the_store.cures,x*sizeof(store_cure) );
    memcpy(newspells+x, the_store.cures+x+1, (the_store.header.curecount-x-1)*sizeof(store_cure) );
  }
  else newspells=NULL;
  delete [] the_store.cures;
  the_store.cures=newspells;
  the_store.header.curecount=the_store.curenum;
  updateflags|=4;
  UpdateData(UD_DISPLAY);
}

void CStoreItems::OnRemallspell() 
{
  int i;

  if(the_store.cures)
  {
    delete [] the_store.cures;
    the_store.cures=NULL;
    the_store.header.curecount=the_store.curenum=0;

  }
  else
  {
    the_store.header.curecount=m_spellresref_control.GetCount();
    the_store.cures=new store_cure[the_store.header.curecount];
    if(!the_store.cures)
    {
      MessageBox("Out of memory","Error",MB_OK);
      return;
    }
    for(i=0;i<the_store.header.curecount;i++)
    {
      m_spellresref_control.GetLBText(i,the_store.cures[i].curename);
      the_store.cures[i].price=0; //we have no idea about the price
    }
    the_store.curenum=the_store.header.curecount;
  }
  updateflags|=4;
  UpdateData(UD_DISPLAY);
}

void CStoreItems::DisplayDrink(int count)
{
  RetrieveResref(m_drinkresref,the_store.drinks[count].drinkscript);
  m_drinknameref=the_store.drinks[count].drinkname;
  m_drinkname=resolve_tlk_text(m_drinknameref);
  m_drinkprice=the_store.drinks[count].price;
  m_strength=the_store.drinks[count].strength;
}

void CStoreItems::RefreshDrink(int count)
{
  StoreResref(m_drinkresref,the_store.drinks[count].drinkscript);
  the_store.drinks[count].drinkname=m_drinknameref;
  the_store.drinks[count].price=m_drinkprice;
  the_store.drinks[count].strength=m_strength;
}

void CStoreItems::OnKillfocusDrinks() 
{
  int count;
  CString m_drink;

	UpdateData(UD_RETRIEVE);
  count=m_drink_control.GetCurSel();
  if(count<0)
  {
    m_drink_control.GetWindowText(m_drink);
    count=strtonum(m_drink);
    count=m_drink_control.SetCurSel(count);
  }
  if(count>=0)
  {
    DisplayDrink(count);
  }
  else m_drinkresref.Empty();
  UpdateData(UD_DISPLAY);
}

void CStoreItems::OnSelchangeDrinks() 
{
  int count;

  count=m_drink_control.GetCurSel();
  if(count>=0)
  {
    DisplayDrink(count);
  }
  else m_drinkresref.Empty();
  UpdateData(UD_DISPLAY);
}

void CStoreItems::OnKillfocusDrinkresref() 
{
  int count;

  UpdateData(UD_RETRIEVE);
  count=m_drink_control.GetCurSel();
  if(count>=0)
  {
    RefreshDrink(count);
  }
  UpdateData(UD_DISPLAY);
}

void CStoreItems::OnKillfocusDrinknameref() 
{
  int count;
  int old;

  UpdateData(UD_RETRIEVE);
  count=m_drink_control.GetCurSel();
  for(old=0;old<the_store.drinknum;old++)
  {
    if((m_drinknameref==the_store.drinks[old].drinkname) && (old!=count))
    {
      MessageBox("This drink is already on the list!","Warning",MB_OK);
      UpdateData(UD_DISPLAY);
      return;
    }
  }
  if(count>=0)
  {
    updateflags|=8;
    RefreshDrink(count);
  }
  UpdateData(UD_DISPLAY);
}

void CStoreItems::OnKillfocusDrinkname() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CStoreItems::OnKillfocusDrinkprice() 
{
  int count;

  UpdateData(UD_RETRIEVE);
  count=m_drink_control.GetCurSel();
  if(count>=0)
  {
    RefreshDrink(count);
  }
  UpdateData(UD_DISPLAY);
}

void CStoreItems::OnKillfocusDrinkstrength() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CStoreItems::OnAdddrink() 
{
  store_drink *newdrinks;

	UpdateData(UD_RETRIEVE);
  m_drinknameref=-1;
  the_store.drinknum++;
  newdrinks=new store_drink[the_store.drinknum];
  if(!newdrinks)
  {
    MessageBox("Out of memory","Warning",MB_OK);
    return;
  }
  memcpy(newdrinks,the_store.drinks,the_store.header.drinkcount*sizeof(store_drink) );

  delete [] the_store.drinks;
  the_store.drinks=newdrinks;
  RefreshDrink(the_store.header.drinkcount);
  the_store.header.drinkcount=the_store.drinknum;
  updateflags|=8;
  UpdateData(UD_DISPLAY);
}

void CStoreItems::OnRemovedrink() 
{
  int x;
  store_drink *newdrinks;

	UpdateData(UD_RETRIEVE);
	for(x=0;(x<the_store.drinknum) && (the_store.drinks[x].drinkname!=m_drinknameref);x++);
	if(x==the_store.drinknum)
  {
    MessageBox("This drink is already removed.","Warning",MB_OK);
    return;
  }
  the_store.drinknum--;
  if(the_store.drinknum)
  {
    newdrinks=new store_drink[the_store.drinknum];
    if(!newdrinks)
    {
      MessageBox("Out of memory","Warning",MB_OK);
      return;
    }
    memcpy(newdrinks,the_store.drinks,x*sizeof(store_drink) );
    memcpy(newdrinks+x, the_store.drinks+x+1, (the_store.header.drinkcount-x-1)*sizeof(store_drink) );
  }
  else newdrinks=NULL;
  delete [] the_store.drinks;
  the_store.drinks=newdrinks;
  the_store.header.drinkcount=the_store.drinknum;
  updateflags|=8;
  UpdateData(UD_DISPLAY);
}

void CStoreItems::OnRemalldrink() 
{
  if(the_store.drinks)
  {
    delete [] the_store.drinks;
    the_store.drinks=NULL;
  }
  the_store.header.drinkcount=the_store.drinknum=0;
  updateflags|=8;
  UpdateData(UD_DISPLAY);
}

/////////////////////////////////////////////////////////////////////////////
// CStoreExtra dialog

CStoreExtra::CStoreExtra() : CPropertyPage(CStoreExtra::IDD)
{
	//{{AFX_DATA_INIT(CStoreExtra)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
  page=0;
}

CStoreExtra::~CStoreExtra()
{
}

void CStoreExtra::DoDataExchange(CDataExchange* pDX)
{
  int i;

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStoreExtra)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
  for(i=0;i<40;i++) //sizeof(iwd2store_header)/2
  {
    DDX_Text(pDX, IDC_U1+i, the_store.iwd2header.unknown[page+i]);
    DDV_MinMaxInt(pDX,the_store.iwd2header.unknown[page+i],0,255);
  }
  DDX_Text(pDX, IDC_CAPACITY, the_store.iwd2header.capacity);
}

void CStoreExtra::RefreshExtra()
{
  page=0;
}

BOOL CStoreExtra::OnInitDialog() 
{
  CButton *cb;

	CDialog::OnInitDialog();
	RefreshExtra();

  cb=(CButton *) GetDlgItem(IDC_PAGE);
  if(cb) cb->SetCheck(false);

	return TRUE;
}

BEGIN_MESSAGE_MAP(CStoreExtra, CDialog)
	//{{AFX_MSG_MAP(CStoreExtra)
	ON_BN_CLICKED(IDC_PAGE, OnPage)
	ON_BN_CLICKED(IDC_CLEARALL, OnClearall)
	ON_EN_KILLFOCUS(IDC_CAPACITY, OnKillfocusCapacity)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStoreExtra message handlers

void CStoreExtra::OnPage() 
{
  UpdateData(UD_RETRIEVE);
	if(!page) page=40; //sizeof(iwd2store_header)/2
  else page=0;
  UpdateData(UD_DISPLAY);
}

void CStoreExtra::OnClearall() 
{
  memset(the_store.iwd2header.unknown+page,0,40);//sizeof(iwd2store_header)/2
  UpdateData(UD_DISPLAY);
}

void CStoreExtra::OnKillfocusCapacity() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

BOOL CStoreExtra::PreTranslateMessage(MSG* pMsg) 
{
	return CDialog::PreTranslateMessage(pMsg);
}

/////////////////////////////////////////////////////////////////////////////
// CStorePropertySheet

IMPLEMENT_DYNAMIC(CStorePropertySheet, CPropertySheet)

//ids_aboutbox is a fake ID, the sheet has no caption
CStorePropertySheet::CStorePropertySheet(CWnd* pWndParent)
: CPropertySheet(IDS_ABOUTBOX, pWndParent)
{
  AddPage(&m_PageGeneral);
  AddPage(&m_PageRental);
  AddPage(&m_PageItems);
  if(iwd2_structures())
  {
    AddPage(&m_PageExtra);
  }
}

CStorePropertySheet::~CStorePropertySheet()
{
}

void CStorePropertySheet::RefreshDialog()
{
  CPropertyPage *page;

  m_PageGeneral.RefreshGeneral();
  m_PageRental.RefreshRental();
  m_PageItems.RefreshItems();
  if(iwd2_structures())
  {
    m_PageExtra.RefreshExtra();
  }
  page=GetActivePage();
  page->UpdateData(UD_DISPLAY);
}

BEGIN_MESSAGE_MAP(CStorePropertySheet, CPropertySheet)
//{{AFX_MSG_MAP(CStorePropertySheet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
