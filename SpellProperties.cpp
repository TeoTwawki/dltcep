// SpellProperties.cpp : implementation file
//

#include "stdafx.h"

#include "chitem.h"
#include "resource.h"
#include "2da.h"
#include "SpellProperties.h"
#include "EffEdit.h"
#include "options.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CSpellGeneral, CPropertyPage)
IMPLEMENT_DYNCREATE(CSpellDescription, CPropertyPage)
IMPLEMENT_DYNCREATE(CSpellEquip, CPropertyPage)
IMPLEMENT_DYNCREATE(CSpellExtended, CPropertyPage)
IMPLEMENT_DYNCREATE(CSpellExtra, CPropertyPage)
IMPLEMENT_DYNCREATE(CSpellTool, CPropertyPage)

//these variables are accessible from CSpellEquip and CSpellExtended
static feat_block  featcopy;
static spl_ext_header  headcopy;
static feat_block *featblks=NULL; //for headcopy
static int featblkcnt=0;          //for headcopy

#define OPC_DAMAGE  12

//gets the starting feature block counter for this extended header
static int GetFBC(int extheader_num)
{
  int fbc;
  int i;

  //equipping feature block count
  fbc=the_spell.header.featblkcount;
  //plus extended header feature block counts
  for(i=0;i<extheader_num;i++)
  {
    fbc+=the_spell.extheaders[i].fbcount;
  }
  return fbc;
}

static int DeleteFeatures(int start, int count, int newcount) 
{
  int max, current;
  int i;
  feat_block *new_featblocks;

  if(count==newcount) return 0;
  max=GetFBC(the_spell.header.extheadcount);
  if(the_spell.featblkcount!=max)
  {
    //internal error
    return -2;
  }
  current=max-count+newcount;
  if(current)
  {
    new_featblocks = new feat_block[current];
    if(!new_featblocks)
    {
      return -1; //memory trouble!!!
    }
  }

  for(i=0;i<start;i++)
  {
    new_featblocks[i]=the_spell.featblocks[i];
  }
  for(i=start;i<current-newcount;i++)
  {
    new_featblocks[i+newcount]=the_spell.featblocks[i+count];
  }
  delete [] the_spell.featblocks;
  if(current)
  {
    //new_featblocks gets initialized when current==true 
    //(the warning of uninitialized local is unwarranted)
    the_spell.featblocks=new_featblocks;
  }
  else the_spell.featblocks=NULL;
  //this is the cummulative featblockcount
  the_spell.featblkcount-=count;
  the_spell.featblkcount+=newcount;
  //current is the new featblockcount (was max before cutting)
  if(the_spell.featblkcount!=current)
  {
    //internal error
    return -2;
  }
  return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CSpellGeneral property page

CSpellGeneral::CSpellGeneral() : CPropertyPage(CSpellGeneral::IDD)
{
	//{{AFX_DATA_INIT(CSpellGeneral)
	//}}AFX_DATA_INIT
  hbmb = 0;
}

CSpellGeneral::~CSpellGeneral()
{
  if(hbmb) DeleteObject(hbmb);
}

static int iconboxids[8]={IDC_ATTR1,IDC_ATTR2,IDC_ATTR3,IDC_ATTR4,IDC_ATTR5,
IDC_ATTR6,IDC_ATTR7,IDC_ATTR8
};

void CSpellGeneral::DoDataExchange(CDataExchange* pDX)
{
  CString tmpstr;
  int id, bit;
  CButton *checkbox;
  CComboBox *cb;
  CPoint newtopleft;
  int value;

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSpellGeneral)
	DDX_Control(pDX, IDC_INVICON1, m_bookicon_control);
	DDX_Control(pDX, IDC_SECTYPE, m_sectype_control);
	DDX_Control(pDX, IDC_SCHOOL, m_school_control);
	DDX_Control(pDX, IDC_SPELLTYPEPICKER, m_spelltypepicker_control);
	//}}AFX_DATA_MAP
  if(pDX->m_bSaveAndValidate==UD_RETRIEVE)
  {
    DDX_Text(pDX,IDC_SPELLTYPEPICKER, tmpstr);
    the_spell.header.spelltype=(short) strtonum(tmpstr);
    DDX_Text(pDX,IDC_PRIESTTYPE, tmpstr);
    the_spell.header.priesttype=(unsigned short) strtonum(tmpstr);
    DDX_Text(pDX,IDC_SCHOOL, tmpstr);
    the_spell.header.school3=(short) strtonum(tmpstr);
    DDX_Text(pDX,IDC_SECTYPE, tmpstr);
    the_spell.header.sectype=(unsigned char) strtonum(tmpstr);

    DDX_Text(pDX, IDC_EXCLUDE, tmpstr);
    value=IDSKey(kitfile,tmpstr);
    if(value==-1)
    {
      the_spell.header.school1=(short) strtonum(tmpstr);
    }
    else
    {
      the_spell.header.school1=(short) value;
    }

    DDX_Text(pDX,IDC_SELECTION, tmpstr);
    the_spell.header.castingglow=(unsigned short) strtonum(tmpstr);

    DDX_Text(pDX,IDC_LEVEL, the_spell.header.level);
    DDX_Text(pDX, IDC_BYTE1,tmpstr);
    the_spell.header.itmattr=(unsigned char) strtonum(tmpstr);
    DDX_Text(pDX, IDC_BYTE34,tmpstr);
    the_spell.header.unkattr2=(unsigned short) strtonum(tmpstr);

    DDX_Text(pDX,IDC_WAV,tmpstr);
    StoreResref(tmpstr,the_spell.header.wavc);

    DDX_Text(pDX, IDC_BOOKICON, tmpstr);
    StoreResref(tmpstr, the_spell.header.icon);
  }
  else  //display
  {
    tmpstr=format_spelltype(the_spell.header.spelltype);
    DDX_Text(pDX,IDC_SPELLTYPEPICKER, tmpstr);
    tmpstr=format_priesttype(the_spell.header.priesttype);
    DDX_Text(pDX,IDC_PRIESTTYPE, tmpstr);
    tmpstr=format_schooltype(the_spell.header.school3);
    DDX_Text(pDX,IDC_SCHOOL, tmpstr);
    tmpstr=format_sectype(the_spell.header.sectype);
    DDX_Text(pDX,IDC_SECTYPE, tmpstr);
   
    tmpstr.Format("0x%x %s",the_spell.header.school1,IDSToken(kitfile,the_spell.header.school1) );
    DDX_Text(pDX, IDC_EXCLUDE, tmpstr);

    cb=(CComboBox *) GetDlgItem(IDC_SELECTION);
    tmpstr.Format("%02d ",the_spell.header.castingglow);
    if(cb->SelectString(0,tmpstr)<0)
    {
      tmpstr.Format("%02d Unknown",the_spell.header.castingglow);
      DDX_Text(pDX, IDC_SELECTION, tmpstr);
    }

    DDX_Text(pDX,IDC_LEVEL, the_spell.header.level);
    DDX_Text(pDX, IDC_BYTE1,the_spell.header.itmattr);
    DDX_Text(pDX, IDC_BYTE34,(short &) the_spell.header.unkattr2);

    RetrieveResref(tmpstr,the_spell.header.wavc);
    DDX_Text(pDX,IDC_WAV,tmpstr);
  	DDV_MaxChars(pDX, tmpstr, 8);

    RetrieveResref(tmpstr, the_spell.header.icon);
    DDX_Text(pDX, IDC_BOOKICON, tmpstr);
  	DDV_MaxChars(pDX, tmpstr, 8);
    if(bmb!=tmpstr)
    {
      bmb=tmpstr;
      if(read_bam(tmpstr))
      {
        the_bam.new_bam();
      }
      the_bam.MakeBitmap(0,RGB(240,224,160),hbmb,BM_RESIZE,32,32);
      m_bookicon_control.SetBitmap(hbmb);
    }

    for(id=0;id<8;id++)
    {
      bit=1<<id;
    
      checkbox=(class CButton *) GetDlgItem(iconboxids[id]);
      checkbox->SetCheck(!!(the_spell.header.splattr&bit));
    }
  }
  value=!!(the_spell.header.splattr&64) && (the_spell.revision==20);
  if(value)
  {
    DDX_Text(pDX, IDC_U1, the_spell.iwd2header.unknown[0]);
    DDX_Text(pDX, IDC_U2, the_spell.iwd2header.unknown[1]);
  }
  for(id=0;id<2;id++) GetDlgItem(IDC_U1+id)->EnableWindow(value);
//unknowns
  DDX_Text(pDX, IDC_UNKNOWN0C, the_spell.header.unknown0c);
  DDX_Text(pDX, IDC_UNKNOWN23, the_spell.header.unknown24);
  DDX_Text(pDX, IDC_UNKNOWN28, the_spell.header.unknown28);
  DDX_Text(pDX, IDC_UNKNOWN2C, the_spell.header.unknown2c);
  DDX_Text(pDX, IDC_UNKNOWN30, the_spell.header.unknown30);
  DDX_Text(pDX, IDC_UNKNOWN38, the_spell.header.unknown38);
  DDX_Text(pDX, IDC_UNKNOWN42, the_spell.header.unknown42);
  DDX_Text(pDX, IDC_UNKNOWN44, the_spell.header.unknown44);
  DDX_Text(pDX, IDC_UNKNOWN48, the_spell.header.unknown48);
  DDX_Text(pDX, IDC_UNKNOWN4C, the_spell.header.unknown4c);
  DDX_Text(pDX, IDC_UNKNOWN54, the_spell.header.unknown54);
  DDX_Text(pDX, IDC_UNKNOWN58, the_spell.header.unknown58);
  DDX_Text(pDX, IDC_UNKNOWN5C, the_spell.header.unknown5c);
  DDX_Text(pDX, IDC_UNKNOWN60, the_spell.header.unknown60);
}

BOOL CSpellGeneral::OnInitDialog() 
{
  CComboBox *cb;
  CString tmpstr, tmpstr2;
  POSITION pos;
  loc_entry dummy;
  int num;
  int x;

	CPropertyPage::OnInitDialog();	
  RefreshGeneral();

  //we load this file in case we have it (i guess only iwd2 has it)
  if(idrefs.Lookup("SCEFFECT",dummy))
  {
    cb=(CComboBox *) GetDlgItem(IDC_SELECTION);
    FillCombo("SCEFFECT",cb,-2);
  }
  m_spelltypepicker_control.ResetContent();
  for(x=0;x<NUM_SPELLTYPE+1;x++)
  {
    m_spelltypepicker_control.AddString(format_spelltype(x));
  }
  pos=school_names.GetHeadPosition();
  num=0;
  while(pos)
  {
    tmpstr2=school_names.GetNext(pos);
    if(tmpstr2.IsEmpty()) tmpstr2="Unknown";
    tmpstr.Format("0x%0x-%s",num, tmpstr2);
    m_school_control.AddString(tmpstr);
    num++;
  }

  pos=sectype_names.GetHeadPosition();
  num=0;
  while(pos)
  {
    tmpstr2=sectype_names.GetNext(pos);
    if(tmpstr2.IsEmpty()) tmpstr2="Unknown";
    tmpstr.Format("0x%0x-%s",num, tmpstr2);
    m_sectype_control.AddString(tmpstr);
    num++;
  }

  cb=(CComboBox *) GetDlgItem(IDC_EXCLUDE);
  FillCombo(kitfile,cb,4);
 
  UpdateData(UD_DISPLAY);
	return TRUE;
}

static int radioids[4]={IDC_V1,IDC_V20};
//move spelldata to local variables
void CSpellGeneral::RefreshGeneral()
{
  int i;
  int id;
  CButton *cb;

  switch(the_spell.revision)
  {
  case 1:
    id=0;
    break;
  case 20:
    id=1;
    break;
  }
  for(i=0;i<2;i++)
  {
    cb=(CButton *) GetDlgItem(radioids[i]);
    if(i==id) cb->SetCheck(true);
    else cb->SetCheck(false);
  }
}

BEGIN_MESSAGE_MAP(CSpellGeneral, CPropertyPage)
	//{{AFX_MSG_MAP(CSpellGeneral)
	ON_EN_KILLFOCUS(IDC_WAV, OnKillfocusWav)
	ON_CBN_KILLFOCUS(IDC_SPELLTYPEPICKER, OnKillfocusSpelltype)
	ON_EN_KILLFOCUS(IDC_LEVEL, OnKillfocusLevel)
	ON_CBN_KILLFOCUS(IDC_SCHOOL, OnKillfocusSchool)
	ON_CBN_KILLFOCUS(IDC_SECTYPE, OnKillfocusSectype)
	ON_CBN_KILLFOCUS(IDC_SELECTION, OnKillfocusSelection)
	ON_EN_KILLFOCUS(IDC_UNKNOWN0C, OnKillfocusUnknown0c)
	ON_EN_KILLFOCUS(IDC_UNKNOWN23, OnKillfocusUnknown23)
	ON_EN_KILLFOCUS(IDC_UNKNOWN28, OnKillfocusUnknown28)
	ON_EN_KILLFOCUS(IDC_UNKNOWN2C, OnKillfocusUnknown2c)
	ON_EN_KILLFOCUS(IDC_UNKNOWN30, OnKillfocusUnknown30)
	ON_EN_KILLFOCUS(IDC_UNKNOWN38, OnKillfocusUnknown38)
	ON_EN_KILLFOCUS(IDC_UNKNOWN44, OnKillfocusUnknown44)
	ON_EN_KILLFOCUS(IDC_UNKNOWN48, OnKillfocusUnknown48)
	ON_EN_KILLFOCUS(IDC_UNKNOWN4C, OnKillfocusUnknown4c)
	ON_EN_KILLFOCUS(IDC_UNKNOWN54, OnKillfocusUnknown54)
	ON_EN_KILLFOCUS(IDC_UNKNOWN58, OnKillfocusUnknown58)
	ON_EN_KILLFOCUS(IDC_UNKNOWN5C, OnKillfocusUnknown5c)
	ON_EN_KILLFOCUS(IDC_UNKNOWN60, OnKillfocusUnknown60)
	ON_BN_CLICKED(IDC_ATTR1, OnAttr1)
	ON_BN_CLICKED(IDC_ATTR2, OnAttr2)
	ON_BN_CLICKED(IDC_ATTR3, OnAttr3)
	ON_BN_CLICKED(IDC_ATTR4, OnAttr4)
	ON_BN_CLICKED(IDC_ATTR5, OnAttr5)
	ON_BN_CLICKED(IDC_ATTR6, OnAttr6)
	ON_BN_CLICKED(IDC_ATTR7, OnAttr7)
	ON_BN_CLICKED(IDC_ATTR8, OnAttr8)
	ON_CBN_KILLFOCUS(IDC_EXCLUDE, OnKillfocusExclude)
	ON_EN_KILLFOCUS(IDC_BOOKICON, OnKillfocusBookicon)
	ON_EN_KILLFOCUS(IDC_BYTE1, OnKillfocusByte1)
	ON_EN_KILLFOCUS(IDC_BYTE34, OnKillfocusByte34)
	ON_BN_CLICKED(IDC_V1, OnV1)
	ON_BN_CLICKED(IDC_V20, OnV20)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_INVICON1, OnInvicon1)
	ON_BN_CLICKED(IDC_PLAYSOUND, OnPlaysound)
	ON_CBN_KILLFOCUS(IDC_PRIESTTYPE, OnKillfocusPriesttype)
	ON_EN_KILLFOCUS(IDC_U1, OnKillfocusU1)
	ON_EN_KILLFOCUS(IDC_U2, OnKillfocusU2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CSpellGeneral::OnKillfocusSpelltype() 
{
	UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CSpellGeneral::OnKillfocusPriesttype() 
{
	UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CSpellGeneral::OnKillfocusWav() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CSpellGeneral::OnPlaysound() 
{
  CString resref;

  RetrieveResref(resref, the_spell.header.wavc);
  play_acm(resref,false,false);	
}

void CSpellGeneral::OnBrowse() 
{
  pickerdlg.m_restype=REF_WAV;
  RetrieveResref(pickerdlg.m_picked,the_spell.header.wavc);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_spell.header.wavc);
  }
	UpdateData(UD_DISPLAY);	
}

void CSpellGeneral::OnKillfocusLevel() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CSpellGeneral::OnKillfocusSchool() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CSpellGeneral::OnKillfocusSectype() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CSpellGeneral::OnKillfocusExclude() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CSpellGeneral::OnKillfocusSelection() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CSpellGeneral::OnKillfocusBookicon() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}


void CSpellGeneral::OnKillfocusU1() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CSpellGeneral::OnKillfocusU2() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CSpellGeneral::OnKillfocusByte1() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CSpellGeneral::OnKillfocusByte34() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CSpellGeneral::OnKillfocusUnknown0c() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CSpellGeneral::OnKillfocusUnknown23() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CSpellGeneral::OnKillfocusUnknown28() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CSpellGeneral::OnKillfocusUnknown2c() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CSpellGeneral::OnKillfocusUnknown30() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CSpellGeneral::OnKillfocusUnknown38() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CSpellGeneral::OnKillfocusUnknown44() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CSpellGeneral::OnKillfocusUnknown48() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CSpellGeneral::OnKillfocusUnknown4c() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CSpellGeneral::OnKillfocusUnknown54() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CSpellGeneral::OnKillfocusUnknown58() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CSpellGeneral::OnKillfocusUnknown5c() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CSpellGeneral::OnKillfocusUnknown60() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CSpellGeneral::OnInvicon1() 
{
  pickerdlg.m_restype=REF_BAM;
  RetrieveResref(pickerdlg.m_picked,the_spell.header.icon);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_spell.header.icon);
  }
	UpdateData(UD_DISPLAY);	
}

void CSpellGeneral::OnAttr1() 
{
  the_spell.header.splattr^=1;
  UpdateData(UD_DISPLAY);
}

void CSpellGeneral::OnAttr2() 
{
  the_spell.header.splattr^=2;
  UpdateData(UD_DISPLAY);
}

void CSpellGeneral::OnAttr3() 
{
  the_spell.header.splattr^=4;
  UpdateData(UD_DISPLAY);
}

void CSpellGeneral::OnAttr4() 
{
  the_spell.header.splattr^=8;
  UpdateData(UD_DISPLAY);
}

void CSpellGeneral::OnAttr5() 
{
  the_spell.header.splattr^=16;
  UpdateData(UD_DISPLAY);
}

void CSpellGeneral::OnAttr6() 
{
  the_spell.header.splattr^=32;
  UpdateData(UD_DISPLAY);
}

void CSpellGeneral::OnAttr7() 
{
  the_spell.header.splattr^=64;
  UpdateData(UD_DISPLAY);
}

void CSpellGeneral::OnAttr8() 
{
  the_spell.header.splattr^=128;
  UpdateData(UD_DISPLAY);
}


void CSpellGeneral::OnV1() 
{
  the_spell.revision=1;
  UpdateData(UD_DISPLAY);
}

void CSpellGeneral::OnV20() 
{
  the_spell.revision=20;
  UpdateData(UD_DISPLAY);
}

/////////////////////////////////////////////////////////////////////////////
// CSpellDescription property page

CSpellDescription::CSpellDescription() : CPropertyPage(CSpellDescription::IDD)
{
	//{{AFX_DATA_INIT(CSpellDescription)
	m_longdesc = _T("");
	m_longname = _T("");
	m_longdesctag = FALSE;
	m_longnametag = FALSE;
	//}}AFX_DATA_INIT
}

CSpellDescription::~CSpellDescription()
{
}

void CSpellDescription::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_LONGREF, the_spell.header.spellname);
	DDV_MinMaxLong(pDX, the_spell.header.spellname, -1, 1000000);

	DDX_Text(pDX, IDC_LONGDESCREF, the_spell.header.desc);
	DDV_MinMaxLong(pDX, the_spell.header.desc, -1, 1000000);
	//{{AFX_DATA_MAP(CSpellDescription)
	DDX_Text(pDX, IDC_LONGDESC, m_longdesc);
	DDX_Text(pDX, IDC_LONGNAME, m_longname);
	DDX_Check(pDX, IDC_LONGDESCTAG, m_longdesctag);
	DDX_Check(pDX, IDC_LONGNAMETAG, m_longnametag);
	//}}AFX_DATA_MAP
}

BOOL CSpellDescription::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
  RefreshDescription();
  UpdateData(UD_DISPLAY);
	return TRUE;
}

BEGIN_MESSAGE_MAP(CSpellDescription, CPropertyPage)
	//{{AFX_MSG_MAP(CSpellDescription)
	ON_EN_KILLFOCUS(IDC_LONGDESCREF, OnKillfocusLongdescref)
	ON_EN_KILLFOCUS(IDC_LONGREF, OnKillfocusLongref)
	ON_EN_KILLFOCUS(IDC_LONGNAME, OnKillfocusLongname)
	ON_EN_KILLFOCUS(IDC_LONGDESC, OnKillfocusLongdesc)
	ON_BN_CLICKED(IDC_LONGNAMETAG, OnLongnametag)
	ON_BN_CLICKED(IDC_LONGDESCTAG, OnLongdesctag)
	ON_BN_CLICKED(IDC_NEW1, OnNew1)
	ON_BN_CLICKED(IDC_NEW2, OnNew2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//move global data to local
void CSpellDescription::RefreshDescription()
{
  m_longname=resolve_tlk_text(the_spell.header.spellname);
  m_longdesc=resolve_tlk_text(the_spell.header.desc);
  m_longnametag=resolve_tlk_tag(the_spell.header.spellname);
  m_longdesctag=resolve_tlk_tag(the_spell.header.desc);
}

void CSpellDescription::OnKillfocusLongdescref() 
{
	UpdateData(UD_RETRIEVE);
 	RefreshDescription();
	UpdateData(UD_DISPLAY);
}

void CSpellDescription::OnKillfocusLongref() 
{
	UpdateData(UD_RETRIEVE);
 	RefreshDescription();
	UpdateData(UD_DISPLAY);
}

void CSpellDescription::OnKillfocusLongname() 
{
	CString old;

  UpdateData(UD_RETRIEVE);
  old=resolve_tlk_text(the_spell.header.spellname);
  if(old!=m_longname)
  {
    if(editflg&TLKCHANGE)
    {
      if(MessageBox("Do you want to update dialog.tlk?","Spell editor",MB_YESNO)!=IDYES)
      {
        return;
      }
    }
    the_spell.header.spellname=store_tlk_text(the_spell.header.spellname, m_longname);
  }
 	RefreshDescription();
  UpdateData(UD_DISPLAY);
}

void CSpellDescription::OnKillfocusLongdesc() 
{
	CString old;

  UpdateData(UD_RETRIEVE);
  old=resolve_tlk_text(the_spell.header.desc);
  if(old!=m_longdesc)
  {
    if(editflg&TLKCHANGE)
    {
      if(MessageBox("Do you want to update dialog.tlk?","Spell editor",MB_YESNO)!=IDYES)
      {
        return;
      }
    }
    the_spell.header.desc=store_tlk_text(the_spell.header.desc, m_longdesc);
  }
 	RefreshDescription();
  UpdateData(UD_DISPLAY);
}

void CSpellDescription::OnLongnametag() 
{
  if(editflg&TLKCHANGE)
  {
    if(MessageBox("Do you want to update dialog.tlk?","Spell editor",MB_YESNO)!=IDYES)
    {
      return;
    }
  }
	toggle_tlk_tag(the_spell.header.spellname);
 	RefreshDescription();
  UpdateData(UD_DISPLAY);
}

void CSpellDescription::OnLongdesctag() 
{
  if(editflg&TLKCHANGE)
  {
    if(MessageBox("Do you want to update dialog.tlk?","Spell editor",MB_YESNO)!=IDYES)
    {
      return;
    }
  }
	toggle_tlk_tag(the_spell.header.desc);
 	RefreshDescription();
  UpdateData(UD_DISPLAY);
}

void CSpellDescription::OnNew1() 
{
  the_spell.header.spellname=-1;
 	RefreshDescription();
  UpdateData(UD_DISPLAY);
}

void CSpellDescription::OnNew2() 
{
  the_spell.header.desc=-1;
 	RefreshDescription();
  UpdateData(UD_DISPLAY);
}

/////////////////////////////////////////////////////////////////////////////
// CSpellEquip property page

CSpellEquip::CSpellEquip() : CPropertyPage(CSpellEquip::IDD)
{
	//{{AFX_DATA_INIT(CSpellEquip)
	//}}AFX_DATA_INIT
  equipnum = 0;
}

CSpellEquip::~CSpellEquip()
{
}

static int equeffids[]={
//fields
  IDC_EQUIPNUM, 
//buttons
  IDC_EQUIPREMOVE, IDC_EQUIPCOPY, IDC_EQUIPPASTE, IDC_EDIT,  
0};

void CSpellEquip::EnableWindow_Equip(bool value)
{
  CWnd *equeffwnd;
  int id;

  for(id=0;equeffids[id];id++)
  {
    equeffwnd=(class CWnd *) GetDlgItem(equeffids[id]);
    if(equeffwnd) equeffwnd->EnableWindow(value);
  }
}

void CSpellEquip::RefreshEquip()
{
  CString tmpstr;
  int i;

  if(m_equipnum_control)
  {
    m_equipnum_control.ResetContent();
    for(i=0;i<the_spell.header.featblkcount;i++)
    {
      tmpstr.Format("%d %s (0x%x 0x%x) %.8s",i+1,
      get_opcode_text(the_spell.featblocks[i].feature),
      the_spell.featblocks[i].par1.parl,
      the_spell.featblocks[i].par2.parl,
      the_spell.featblocks[i].vvc);
      m_equipnum_control.AddString(tmpstr);
    }
    if(equipnum<0 || equipnum>=i) equipnum=0;
    equipnum=m_equipnum_control.SetCurSel(equipnum);
  }
}

void CSpellEquip::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSpellEquip)
	DDX_Control(pDX, IDC_EQUIPNUM, m_equipnum_control);
	//}}AFX_DATA_MAP
  //this is only the equipping feature count
  if(the_spell.header.featblkcount)
  {
    EnableWindow_Equip(true);
  }
  else
  {
    EnableWindow_Equip(false);
  }
}

BOOL CSpellEquip::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
  RefreshEquip();
  UpdateData(UD_DISPLAY);
	return TRUE;
}

BEGIN_MESSAGE_MAP(CSpellEquip, CPropertyPage)
	//{{AFX_MSG_MAP(CSpellEquip)
	ON_LBN_KILLFOCUS(IDC_EQUIPNUM, OnKillfocusEquipnum)
	ON_LBN_DBLCLK(IDC_EQUIPNUM, OnEdit)
	ON_BN_CLICKED(IDC_EQUIPREMOVE, OnEquipremove)
	ON_BN_CLICKED(IDC_EQUIPADD, OnEquipadd)
	ON_BN_CLICKED(IDC_EQUIPCOPY, OnEquipcopy)
	ON_BN_CLICKED(IDC_EQUIPPASTE, OnEquippaste)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CSpellEquip::OnKillfocusEquipnum() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CSpellEquip::OnEquipremove() 
{
  if(!the_spell.header.featblkcount) return;
  equipnum=m_equipnum_control.GetCurSel();
  if(equipnum<0) return;
  if(!DeleteFeatures(equipnum,1,0))
  {
    the_spell.header.featblkcount--;  
  }
  RefreshEquip();
  UpdateData(UD_DISPLAY);
}

void CSpellEquip::OnEquipadd() 
{
  feat_block *new_featblocks;
  int fbc;
  int allcount;
  int i;

  if(the_spell.header.featblkcount>999)
  {
    MessageBox("Cannot add more features.","Warning",MB_ICONEXCLAMATION|MB_OK);
    return;
  }
  //find index for extended effect start
  //exteffnum: actual feature number in actual feature block
  equipnum=m_equipnum_control.GetCurSel();
  if(the_spell.header.featblkcount)
  {
    fbc=equipnum+1;
  }
  else fbc=0;
  allcount=GetFBC(the_spell.extheadcount); //this is the total number of features
  new_featblocks=new feat_block[allcount+1];
  if(!new_featblocks)
  {
    MessageBox("Not enough memory.","Error",MB_ICONSTOP|MB_OK);
    return;
  }

  for(i=0;i<fbc;i++)
  {
    new_featblocks[i]=the_spell.featblocks[i];
  }
  for(;i<allcount;i++)
  {
    new_featblocks[i+1]=the_spell.featblocks[i];
  }
  memset(&new_featblocks[fbc],0,sizeof(feat_block) );
  new_featblocks[fbc].target=1; //target is self
  new_featblocks[fbc].timing=2; //while equipped by default
  new_featblocks[fbc].prob2=100; //upper probability 100

  //switching to the new block
  if(the_spell.featblocks)
  {
    delete [] the_spell.featblocks;
  }
  equipnum=fbc;
  the_spell.featblocks=new_featblocks;
  //this is the cummulative feature block count
  the_spell.header.featblkcount++;
  the_spell.featblkcount++;
  RefreshEquip();
  UpdateData(UD_DISPLAY);
}

void CSpellEquip::OnEquipcopy() 
{
  equipnum=m_equipnum_control.GetCurSel();
  if(equipnum<the_spell.featblkcount && equipnum>=0)
  {
    memcpy(&featcopy,&the_spell.featblocks[equipnum],sizeof(feat_block) );
  }
}

void CSpellEquip::OnEquippaste() 
{
  equipnum=m_equipnum_control.GetCurSel();
  if(equipnum<the_spell.featblkcount && equipnum>=0)
  {
    memcpy(&the_spell.featblocks[equipnum],&featcopy,sizeof(feat_block) );
  }
  RefreshEquip();
  UpdateData(UD_DISPLAY);
}

void CSpellEquip::OnEdit() 
{
  CString tmpname;
  CEffEdit dlg;

  equipnum=m_equipnum_control.GetCurSel();
  if(equipnum<the_spell.featblkcount && equipnum>=0)
  {
    ConvertToV20Eff((creature_effect *) the_effect.header.signature,the_spell.featblocks+equipnum );
    dlg.SetLimitedEffect(1);
    tmpname=itemname;
    itemname.Format("%d of %s", equipnum+1, tmpname);
    dlg.SetDefaultDuration(0);
    dlg.DoModal();
    itemname=tmpname;
    ConvertToV10Eff((creature_effect *) the_effect.header.signature,the_spell.featblocks+equipnum );
  }
  RefreshEquip();
  UpdateData(UD_DISPLAY);
}
/////////////////////////////////////////////////////////////////////////////
// CSpellExtended property page

CSpellExtended::CSpellExtended() : CPropertyPage(CSpellExtended::IDD)
{
	//{{AFX_DATA_INIT(CSpellExtended)
	//}}AFX_DATA_INIT
	extheadnum = 0;
  exteffnum = 0;
  hbmu = 0;
}

CSpellExtended::~CSpellExtended()
{
  if(hbmu) DeleteObject(hbmu);
}

void CSpellExtended::DoDataExchangeEffects(CDataExchange* pDX)
{
  int fbc;

  fbc=GetFBC(extheadnum)+exteffnum; //this is the cummulative fbc in effect
  if(fbc>=the_spell.featblkcount)
  {
    MessageBox("Internal error (fbc is inconsistent)","Error",MB_OK);
    return;
  }
  if(pDX->m_bSaveAndValidate==UD_DISPLAY)
  {
    EnableWindow_ExtEffect(true);
    m_exteffnum_control.SetCurSel(exteffnum);
  }
}

void CSpellExtended::DoDataExchangeExtended(CDataExchange* pDX)
{
  CString tmpstr;

  if(pDX->m_bSaveAndValidate==UD_DISPLAY)
  {
    //m_extheadnum: displayed actual extended header number
    //extheadnum: actual extended header number
    m_extheadnum=extheadnum+1;
    EnableWindow_Extended(true);
    
    DDX_Text(pDX, IDC_EXTHEADNUM, m_extheadnum);    
    DDV_MinMaxLong(pDX, m_extheadnum, 1,the_spell.header.extheadcount);

    RetrieveResref(tmpstr, the_spell.extheaders[extheadnum].useicon);
  	DDX_Text(pDX, IDC_EXTUSEICON, tmpstr);
  	DDV_MaxChars(pDX, tmpstr, 8);
    if(bmu!=tmpstr)
    {
      bmu=tmpstr;
      if(read_bam(tmpstr))
      {
        the_bam.new_bam();
      }
      the_bam.MakeBitmap(0,RGB(32,32,32),hbmu,BM_RESIZE,32,32);
      m_useicon_control.SetBitmap(hbmu);
    }

    tmpstr=get_spell_form(the_spell.extheaders[extheadnum].spellform);
    DDX_Text(pDX, IDC_EXTTYPE, tmpstr);
    
    tmpstr=get_location_type(the_spell.extheaders[extheadnum].location);
    DDX_Text(pDX, IDC_LOC, tmpstr);
    
    DDX_Text(pDX, IDC_TARGETNUM, the_spell.extheaders[extheadnum].target_num);
    DDV_MinMaxInt(pDX, the_spell.extheaders[extheadnum].target_num, 0, 99);
    
    tmpstr=get_target_type(the_spell.extheaders[extheadnum].target_type);
    DDX_Text(pDX, IDC_TARGET, tmpstr);

  	DDX_Text(pDX, IDC_RANGE,(short &) the_spell.extheaders[extheadnum].range);
  	DDX_Text(pDX, IDC_LEVEL,(short &) the_spell.extheaders[extheadnum].level);
  	DDX_Text(pDX, IDC_SPEED,(short &) the_spell.extheaders[extheadnum].speed);
    DDX_Text(pDX, IDC_UNKNOWN01, the_spell.extheaders[extheadnum].unknown01);
//    DDX_Text(pDX, IDC_UNKNOWN03, the_spell.extheaders[extheadnum].unknown03);
    DDX_Text(pDX, IDC_UNKNOWN16, the_spell.extheaders[extheadnum].unknown16);
    DDX_Text(pDX, IDC_UNKNOWN1A, the_spell.extheaders[extheadnum].unknown1a);
    DDX_Text(pDX, IDC_UNKNOWN22, the_spell.extheaders[extheadnum].unknown22);
    DDX_Text(pDX, IDC_UNKNOWN24, the_spell.extheaders[extheadnum].unknown24);

    tmpstr=get_projectile_id(the_spell.extheaders[extheadnum].proref,0);
   	DDX_Text(pDX, IDC_PROJID,tmpstr);

    if(the_spell.extheaders[extheadnum].fbcount)
    {
      DoDataExchangeEffects(pDX);
    }
  }
  else //retrieve
  {
    DDX_Text(pDX, IDC_EXTHEADNUM, tmpstr);
    m_extheadnum=atoi(tmpstr);  //should stay decimal

  	DDX_Text(pDX, IDC_EXTUSEICON, tmpstr);
    StoreResref(tmpstr,the_spell.extheaders[extheadnum].useicon);
    
    DDX_Text(pDX, IDC_EXTTYPE, tmpstr);
    the_spell.extheaders[extheadnum].spellform=(unsigned char) strtonum(tmpstr);
    
    DDX_Text(pDX, IDC_LOC, tmpstr);
    the_spell.extheaders[extheadnum].location=(unsigned short) strtonum(tmpstr);
    
    DDX_Text(pDX, IDC_TARGETNUM, the_spell.extheaders[extheadnum].target_num);
    
    DDX_Text(pDX, IDC_TARGET, tmpstr);
    the_spell.extheaders[extheadnum].target_type=(unsigned char) strtonum(tmpstr);
    
  	DDX_Text(pDX, IDC_RANGE,(short &) the_spell.extheaders[extheadnum].range);
  	DDX_Text(pDX, IDC_LEVEL,(short &) the_spell.extheaders[extheadnum].level);
  	DDX_Text(pDX, IDC_SPEED,(short &) the_spell.extheaders[extheadnum].speed);
    DDX_Text(pDX, IDC_UNKNOWN01, the_spell.extheaders[extheadnum].unknown01);
//    DDX_Text(pDX, IDC_UNKNOWN03, the_spell.extheaders[extheadnum].unknown03);
    DDX_Text(pDX, IDC_UNKNOWN16, the_spell.extheaders[extheadnum].unknown16);
    DDX_Text(pDX, IDC_UNKNOWN1A, the_spell.extheaders[extheadnum].unknown1a);
    DDX_Text(pDX, IDC_UNKNOWN22, the_spell.extheaders[extheadnum].unknown22);
    DDX_Text(pDX, IDC_UNKNOWN24, the_spell.extheaders[extheadnum].unknown24);

   	DDX_Text(pDX, IDC_PROJID,tmpstr);
    the_spell.extheaders[extheadnum].proref=(unsigned short) strtonum(tmpstr);

    if(the_spell.extheaders[extheadnum].fbcount)
    {
      DoDataExchangeEffects(pDX);
    }

    extheadnum=m_extheadnum-1;
  }
}

void CSpellExtended::DoDataExchange(CDataExchange* pDX)
{
  CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSpellExtended)
	DDX_Control(pDX, IDC_EXTEFFNUM, m_exteffnum_control);
  DDX_Control(pDX, IDC_USEICON, m_useicon_control);
	DDX_Control(pDX, IDC_PROJID, m_projid_control);
	DDX_Control(pDX, IDC_EXTHEADNUM, m_extheadnum_control);
	DDX_Control(pDX, IDC_TARGET, m_target_control);
	DDX_Control(pDX, IDC_LOC, m_loc_control);
	DDX_Control(pDX, IDC_EXTTYPE, m_exttype_control);
	DDX_Text(pDX, IDC_HEADNUM, m_headnum);
	//}}AFX_DATA_MAP
  if(the_spell.header.extheadcount && (extheadnum!=-1) )
  {
    DoDataExchangeExtended(pDX);
    if(the_spell.extheaders[extheadnum].fbcount)
    {
      DoDataExchangeEffects(pDX);
    }
    else
    {
      EnableWindow_ExtEffect(false);
    }
  }
  else
  {
    EnableWindow_Extended(false);
    EnableWindow_ExtEffect(false);
  }
}

//cannot put this into refreshextended
//m_extheadnum_control doesn't exist all the time
void CSpellExtended::RefreshExtended()
{
  CString tmp;
  int fbc;
  int i;

  m_headnum.Format("%d",the_spell.header.extheadcount);
  if(IsWindow(m_extheadnum_control))
  {
    m_extheadnum_control.ResetContent();
    for(i=0;i<the_spell.extheadcount;i++)
    {
      tmp.Format("%d L:%d (%d effects)",i+1,the_spell.extheaders[i].level,the_spell.extheaders[i].fbcount);
      m_extheadnum_control.AddString(tmp);
    }
    if(extheadnum<0 || extheadnum>=i) extheadnum=0;
    extheadnum=m_extheadnum_control.SetCurSel(extheadnum);
  }
  if(IsWindow(m_exteffnum_control) )
  {
    if(extheadnum<0) return;
    if(the_spell.header.extheadcount)
    {
      fbc=GetFBC(extheadnum); //this is the starting fbc in cummulative feature block
      m_exteffnum_control.ResetContent();
      for(i=0;i<the_spell.extheaders[extheadnum].fbcount;i++)
      {
        tmp.Format("%d %s (0x%x 0x%x) %.8s",i+1,
          get_opcode_text(the_spell.featblocks[fbc+i].feature),
          the_spell.featblocks[fbc+i].par1.parl,
          the_spell.featblocks[fbc+i].par2.parl,
          the_spell.featblocks[fbc+i].vvc);
        m_exteffnum_control.AddString(tmp);
      }
      if(exteffnum<0 || exteffnum>=i) exteffnum=0;
      exteffnum=m_exteffnum_control.SetCurSel(exteffnum);
    }
  }
}

BOOL CSpellExtended::OnInitDialog() 
{
  POSITION pos, pos2;
  int i;
  CString tmp, tmp2;

	CPropertyPage::OnInitDialog();

  m_extheadnum_control.SetDroppedWidth(100);

  for(i=0;i<NUM_SFTYPE;i++)
  {
    m_exttype_control.AddString(get_spell_form(i));
  }

  for(i=0;i<NUM_LFTYPE;i++)
  {
    m_loc_control.AddString(get_location_type(i));
  }

  for(i=0;i<NUM_TTYPE;i++)
  {
    m_target_control.AddString(get_target_type(i));
  }

  pos=pro_references.GetHeadPosition();
  pos2=pro_titles.GetHeadPosition();
  i=0;
  while(pos)
  {
    if(pos2) tmp2=pro_titles.GetNext(pos2);
    else tmp2.Format("???");
    tmp.Format("%d-%s (%s)",i,tmp2, pro_references.GetNext(pos) );
    m_projid_control.AddString(tmp);
    i++;
  }
  RefreshExtended();
  UpdateData(UD_DISPLAY);
	return TRUE;
}

static int exteffids[]={
//fields
  IDC_EXTEFFNUM, IDC_EXTEFFREMOVE, IDC_EXTEFFCOPY, 
  IDC_EXTEFFPASTE, IDC_BROWSE, IDC_EDIT,
0};

void CSpellExtended::EnableWindow_ExtEffect(bool value)
{
  CWnd *exteffwnd;
  int id;

  for(id=0;exteffids[id];id++)
  {
    exteffwnd=(class CWnd *) GetDlgItem(exteffids[id]);
    if(exteffwnd) exteffwnd->EnableWindow(value);
  }
}

static int extids[]={
  //attribute controls
  IDC_EXTUSEICON, IDC_USEICON, IDC_LEVEL,
  IDC_EXTHEADNUM, IDC_EXTTYPE, IDC_LOC, IDC_TARGETNUM, IDC_TARGET,
  IDC_RANGE, IDC_SPEED, IDC_PROJID, IDC_UNKNOWN01, //IDC_UNKNOWN03,
  IDC_UNKNOWN16, IDC_UNKNOWN1A, IDC_UNKNOWN22, IDC_UNKNOWN24,
  //buttons
  //last button really belongs here!
  IDC_ORDER, IDC_EXTREMOVE, IDC_EXTCOPY, IDC_EXTPASTE, IDC_EXTEFFADD,
0};

void CSpellExtended::EnableWindow_Extended(bool value)
{
  CWnd *extwnd;
  int id;

  for(id=0;extids[id];id++)
  {
    extwnd=(class CWnd *) GetDlgItem(extids[id]);
    extwnd->EnableWindow(value);
  }
}

BEGIN_MESSAGE_MAP(CSpellExtended, CPropertyPage)
	//{{AFX_MSG_MAP(CSpellExtended)
	ON_CBN_KILLFOCUS(IDC_EXTHEADNUM, OnKillfocusExtheadnum)
	ON_BN_CLICKED(IDC_EXTPASTE, OnExtpaste)
	ON_BN_CLICKED(IDC_EXTEFFADD, OnExteffadd)
	ON_BN_CLICKED(IDC_EXTEFFCOPY, OnExteffcopy)
	ON_BN_CLICKED(IDC_EXTEFFPASTE, OnExteffpaste)
	ON_BN_CLICKED(IDC_EXTEFFREMOVE, OnExteffremove)
	ON_BN_CLICKED(IDC_EXTCOPY, OnExtcopy)
	ON_BN_CLICKED(IDC_EXTADD, OnExtadd)
	ON_BN_CLICKED(IDC_EXTREMOVE, OnExtremove)
	ON_CBN_KILLFOCUS(IDC_EXTTYPE, OnKillfocusExttype)
	ON_CBN_KILLFOCUS(IDC_LOC, OnKillfocusLoc)
	ON_CBN_KILLFOCUS(IDC_TARGET, OnKillfocusTarget)
	ON_EN_KILLFOCUS(IDC_RANGE, OnKillfocusRange)
	ON_CBN_KILLFOCUS(IDC_PROJFRAME, OnKillfocusProjframe)
	ON_EN_KILLFOCUS(IDC_SPEED, OnKillfocusSpeed)
	ON_CBN_SELCHANGE(IDC_EXTHEADNUM, OnSelchangeExtheadnum)
	ON_EN_KILLFOCUS(IDC_ROLL, OnKillfocusRoll)
	ON_CBN_KILLFOCUS(IDC_DAMAGETYPE, OnKillfocusDamagetype)
	ON_EN_KILLFOCUS(IDC_DIE, OnKillfocusDie)
	ON_EN_KILLFOCUS(IDC_ADD, OnKillfocusAdd)
	ON_EN_KILLFOCUS(IDC_TARGETNUM, OnKillfocusTargetnum)
	ON_EN_KILLFOCUS(IDC_EXTUSEICON, OnKillfocusExtuseicon)
	ON_CBN_KILLFOCUS(IDC_PROJID, OnKillfocusProjid)
	ON_EN_KILLFOCUS(IDC_UNKNOWN16, OnKillfocusUnknown16)
	ON_EN_KILLFOCUS(IDC_UNKNOWN1A, OnKillfocusUnknown1a)
	ON_EN_KILLFOCUS(IDC_UNKNOWN22, OnKillfocusUnknown22)
	ON_EN_KILLFOCUS(IDC_UNKNOWN24, OnKillfocusUnknown24)
	ON_BN_CLICKED(IDC_ORDER, OnOrder)
	ON_EN_KILLFOCUS(IDC_LEVEL, OnKillfocusLevel)
	ON_BN_CLICKED(IDC_USEICON, OnUseicon)
	ON_EN_KILLFOCUS(IDC_UNKNOWN01, OnKillfocusUnknown01)
	ON_LBN_KILLFOCUS(IDC_EXTEFFNUM, OnKillfocusExteffnum)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	ON_LBN_DBLCLK(IDC_EXTEFFNUM, OnEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CSpellExtended::OnKillfocusExtheadnum() 
{
  UpdateData(UD_RETRIEVE);
  exteffnum=0;  
  RefreshExtended();
  UpdateData(UD_DISPLAY);
}

void CSpellExtended::OnSelchangeExtheadnum() 
{
  extheadnum=m_extheadnum_control.GetCurSel();
  exteffnum=0;
  RefreshExtended();
  UpdateData(UD_DISPLAY);
}

void CSpellExtended::OnKillfocusExteffnum() 
{
  UpdateData(UD_RETRIEVE);
  exteffnum=m_exteffnum_control.GetCurSel();
  UpdateData(UD_DISPLAY);
}

void CSpellExtended::OnKillfocusLevel() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CSpellExtended::OnKillfocusExtuseicon() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);	
}

void CSpellExtended::OnUseicon() 
{
  pickerdlg.m_restype=REF_BAM;
  RetrieveResref(pickerdlg.m_picked,the_spell.extheaders[extheadnum].useicon);
  if(pickerdlg.DoModal()==IDOK)
  {    
    StoreResref(pickerdlg.m_picked,the_spell.extheaders[extheadnum].useicon);
  }
  UpdateData(UD_DISPLAY);	
}

void CSpellExtended::OnKillfocusExttype() 
{
  UpdateData(UD_RETRIEVE);  
  UpdateData(UD_DISPLAY);
}

void CSpellExtended::OnKillfocusLoc() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CSpellExtended::OnKillfocusTargetnum() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CSpellExtended::OnKillfocusTarget() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CSpellExtended::OnKillfocusProjframe() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CSpellExtended::OnKillfocusRange() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CSpellExtended::OnKillfocusSpeed() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CSpellExtended::OnKillfocusRoll() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CSpellExtended::OnKillfocusDie() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CSpellExtended::OnKillfocusAdd() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CSpellExtended::OnKillfocusDamagetype() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CSpellExtended::OnKillfocusProjid() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CSpellExtended::OnKillfocusUnknown01() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}
/*
void CSpellExtended::OnKillfocusUnknown03() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}
*/
void CSpellExtended::OnKillfocusUnknown16() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CSpellExtended::OnKillfocusUnknown1a() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CSpellExtended::OnKillfocusUnknown22() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CSpellExtended::OnKillfocusUnknown24() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

static int CompareExtension(const void *a, const void *b)
{
  spl_ext_header *tmp_extheadera=(spl_ext_header *) a;
  spl_ext_header *tmp_extheaderb=(spl_ext_header *) b;

  return (tmp_extheadera->level-tmp_extheaderb->level);
}

void CSpellExtended::OnOrder() 
{
  qsort(the_spell.extheaders, the_spell.extheadcount, sizeof(spl_ext_header), CompareExtension);
  RefreshExtended();
  UpdateData(UD_DISPLAY);
}

///// feature blocks

void CSpellExtended::OnExtremove() 
{
  int deletepos;
  int i;
  int res;
  spl_ext_header *new_extheaders;

  extheadnum=m_extheadnum_control.GetCurSel();
  if(extheadnum<0) return;
  deletepos=extheadnum;
  res=DeleteFeatures(GetFBC(deletepos),the_spell.extheaders[deletepos].fbcount, 0);
  if(res)      
  {
    if(res==-2)
    {
      MessageBox("Internal error","Error",MB_ICONSTOP|MB_OK);
    }
    else
    {
      MessageBox("Not enough memory.","Error",MB_ICONSTOP|MB_OK);
    }
    return;
  }
  the_spell.header.extheadcount--;
  if(the_spell.header.extheadcount)
  {
    new_extheaders=new spl_ext_header[the_spell.header.extheadcount];
    if(!new_extheaders)
    {
      MessageBox("Not enough memory.","Error",MB_ICONSTOP|MB_OK);
      return;
    }
    for(i=0;i<deletepos;i++)
    {
      new_extheaders[i]=the_spell.extheaders[i];
    }
    //deletes all features of this extended header (#i)
    //it starts at GetFBC
    for(;i<the_spell.header.extheadcount;i++)
    {
      new_extheaders[i]=the_spell.extheaders[i+1];
    }
  }
  else new_extheaders=0;
  //switching to the new block, delete should work here, because we
  //must have had at least one block
  delete [] the_spell.extheaders;
  extheadnum=deletepos-1;
  exteffnum=0;
  the_spell.extheadcount=the_spell.header.extheadcount;
  the_spell.extheaders=new_extheaders;
  RefreshExtended();
  UpdateData(UD_DISPLAY);
}

//todo: handle effectblocks
void CSpellExtended::OnExtadd() 
{
  int insertpos;
  int i;
  spl_ext_header *new_extheaders;

  //don't allow more than 39 extended headers for spells
  if(the_spell.header.extheadcount>39)
  {
    MessageBox("Cannot add more extended headers.","Warning",MB_ICONEXCLAMATION|MB_OK);
    return;
  }
  if(the_spell.header.extheadcount)
  {
    extheadnum=m_extheadnum_control.GetCurSel();
	  insertpos=extheadnum+1;
  }
  else insertpos=0;
  the_spell.header.extheadcount++;
  new_extheaders=new spl_ext_header[the_spell.header.extheadcount];
  if(!new_extheaders)
  {
    the_spell.header.extheadcount--;
    MessageBox("Not enough memory.","Error",MB_ICONSTOP|MB_OK);
    return;
  }
  for(i=0;i<insertpos;i++)
  {
    new_extheaders[i]=the_spell.extheaders[i];
  }
  for(;i<the_spell.header.extheadcount-1;i++)
  {
    new_extheaders[i+1]=the_spell.extheaders[i];
  }
  memset(&new_extheaders[insertpos],0,sizeof(spl_ext_header) );
  new_extheaders[insertpos].target_type=1;   //creature
  //switching to the new block
  if(the_spell.extheaders)
  {
    delete [] the_spell.extheaders;
  }
  extheadnum=insertpos;
  exteffnum=0;
  the_spell.extheadcount=the_spell.header.extheadcount;
  the_spell.extheaders=new_extheaders;
  RefreshExtended();
  UpdateData(UD_DISPLAY);
}

void CSpellExtended::OnExtcopy() 
{
  int fbc;

  extheadnum=m_extheadnum_control.GetCurSel();
  if(extheadnum<0) return;
  memcpy(&headcopy, &the_spell.extheaders[extheadnum],sizeof(spl_ext_header) );
  if(featblkcnt!=the_spell.extheaders[extheadnum].fbcount)
  {
    if(featblkcnt) delete [] featblks;
    featblkcnt=the_spell.extheaders[extheadnum].fbcount;
    if(featblkcnt)
    {
      featblks=new feat_block[featblkcnt];
      if(!featblks)
      {
        featblkcnt=0;
        MessageBox("Not enough memory.","Error",MB_ICONSTOP|MB_OK);
        return;
      }
    }
  }
  fbc=GetFBC(extheadnum);
  memcpy(featblks, &the_spell.featblocks[fbc],featblkcnt*sizeof(feat_block) );
}

void CSpellExtended::OnExtpaste() 
{
  int fbc;  

  extheadnum=m_extheadnum_control.GetCurSel();
  if(extheadnum<0) return;
  fbc=GetFBC(extheadnum);
  if(!DeleteFeatures(fbc, the_spell.extheaders[extheadnum].fbcount, featblkcnt))
  {
    memcpy(&the_spell.extheaders[extheadnum], &headcopy, sizeof(spl_ext_header) );    
    memcpy(&the_spell.featblocks[fbc], featblks, featblkcnt*sizeof(feat_block) );
  }
  RefreshExtended();
  UpdateData(UD_DISPLAY);
}

void CSpellExtended::OnExteffremove() 
{
  if(!the_spell.header.extheadcount) return;
  if(!the_spell.extheaders[extheadnum].fbcount) return;
  exteffnum=m_exteffnum_control.GetCurSel();
  if(exteffnum<0) return;
  if(!DeleteFeatures(GetFBC(extheadnum)+exteffnum,1,0))
  {
    the_spell.extheaders[extheadnum].fbcount--;  
  }
  RefreshExtended();
  UpdateData(UD_DISPLAY);
}

void CSpellExtended::OnExteffadd() 
{
  feat_block *new_featblocks;
  int fbc;
  int allcount;
  int i;

  //this is the actual maximum size of the feature block in the
  //actual extended header
  if(the_spell.extheaders[extheadnum].fbcount>999)
  {
    MessageBox("Cannot add more features.","Warning",MB_ICONEXCLAMATION|MB_OK);
    return;
  }
  //find index for extended effect start
  //exteffnum: actual feature number in actual feature block
  if(the_spell.extheaders[extheadnum].fbcount)
  {
    exteffnum=m_exteffnum_control.GetCurSel();
    fbc=GetFBC(extheadnum)+exteffnum+1;///////
  }
  else fbc=GetFBC(extheadnum);
  allcount=GetFBC(the_spell.extheadcount);
  new_featblocks=new feat_block[allcount+1];
  if(!new_featblocks)
  {
    MessageBox("Not enough memory.","Error",MB_ICONSTOP|MB_OK);
    return;
  }

  for(i=0;i<fbc;i++)
  {
    new_featblocks[i]=the_spell.featblocks[i];
  }
  for(;i<allcount;i++)
  {
    new_featblocks[i+1]=the_spell.featblocks[i];
  }
  memset(&new_featblocks[fbc],0,sizeof(feat_block) );
  new_featblocks[fbc].target=2; //target is pre-target
  new_featblocks[fbc].timing=1; //permanent by default
  new_featblocks[fbc].prob2=100; //upper probability 100

  //switching to the new block
  if(the_spell.featblocks)
  {
    delete [] the_spell.featblocks;
  }
  exteffnum=fbc-GetFBC(extheadnum);
  the_spell.extheaders[extheadnum].fbcount++;
  the_spell.featblocks=new_featblocks;
  //this is the cummulative feature block count
  the_spell.featblkcount++;
  RefreshExtended();
  UpdateData(UD_DISPLAY);
}

void CSpellExtended::OnExteffcopy() 
{
  int fbc;

  exteffnum=m_exteffnum_control.GetCurSel();
  if(exteffnum<0 || exteffnum>=the_spell.extheaders[extheadnum].fbcount) return;
  fbc=GetFBC(extheadnum)+exteffnum;
  memcpy(&featcopy,&the_spell.featblocks[fbc],sizeof(feat_block) );
}

void CSpellExtended::OnExteffpaste() 
{
  int fbc;

  exteffnum=m_exteffnum_control.GetCurSel();
  if(exteffnum<0 || exteffnum>=the_spell.extheaders[extheadnum].fbcount) return;
  fbc=GetFBC(extheadnum)+exteffnum;
  memcpy(&the_spell.featblocks[fbc],&featcopy, sizeof(feat_block) );
  RefreshExtended();
  UpdateData(UD_DISPLAY);
}

void CSpellExtended::OnEdit() 
{
  int fbc;
  CString tmpname;
  CEffEdit dlg;

  if(extheadnum<0) return;
  exteffnum=m_exteffnum_control.GetCurSel();
  if(exteffnum<0 || exteffnum>=the_spell.extheaders[extheadnum].fbcount) return;
  fbc=GetFBC(extheadnum)+exteffnum;
  ConvertToV20Eff((creature_effect *) the_effect.header.signature,the_spell.featblocks+fbc );
  dlg.SetLimitedEffect(1);
  tmpname=itemname;
  itemname.Format("%d,%d of %s", extheadnum+1, exteffnum+1, tmpname);
  dlg.SetDefaultDuration(the_spell.featblocks[GetFBC(extheadnum)].duration);
  dlg.DoModal();
  itemname=tmpname;
  ConvertToV10Eff((creature_effect *) the_effect.header.signature,the_spell.featblocks+fbc );
  RefreshExtended();
  UpdateData(UD_DISPLAY);
}

/////////////////////////////////////////////////////////////////////////////
// CSpellExtra property page
CSpellExtra::CSpellExtra() : CPropertyPage(CSpellExtra::IDD)
{
	//{{AFX_DATA_INIT(CSpellExtra)
	//}}AFX_DATA_INIT
}

CSpellExtra::~CSpellExtra()
{
}

void CSpellExtra::DoDataExchange(CDataExchange* pDX)
{
  int i;

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSpellExtra)
	//}}AFX_DATA_MAP

  //U1 and U2 are not unknown anymore
  for(i=2;i<16;i++)
  {
    DDX_Text(pDX, IDC_U1+i,the_spell.iwd2header.unknown[i]);
  	DDV_MinMaxByte(pDX, the_spell.iwd2header.unknown[i], 0, 255);
  }
}

void CSpellExtra::RefreshExtra()
{
}

BOOL CSpellExtra::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();	
	RefreshExtra();
	return TRUE;
}

BEGIN_MESSAGE_MAP(CSpellExtra, CPropertyPage)
	//{{AFX_MSG_MAP(CSpellExtra)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpellExtra message handlers



/////////////////////////////////////////////////////////////////////////////
// CSpellTool property page

CSpellTool::CSpellTool() : CPropertyPage(CSpellTool::IDD)
{
	//{{AFX_DATA_INIT(CSpellTool)
	m_rangeinc = 0;
	m_range = 0;
	m_duration = 0;
	m_durationinc = 0;
	m_levelinc = 0;
	m_level = 0;
	m_which = 0;
	m_damage = 0;
	m_damageinc = 0;
	m_half = FALSE;
	//}}AFX_DATA_INIT
}

CSpellTool::~CSpellTool()
{
}

void CSpellTool::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSpellTool)
	DDX_Text(pDX, IDC_RANGEINC, m_rangeinc);
	DDV_MinMaxInt(pDX, m_rangeinc, 0, 1000);
	DDX_Text(pDX, IDC_RANGE, m_range);
	DDV_MinMaxInt(pDX, m_range, 0, 1000);
	DDX_Text(pDX, IDC_DURATION, m_duration);
	DDV_MinMaxInt(pDX, m_duration, 0, 1000);
	DDX_Text(pDX, IDC_DURATIONINC, m_durationinc);
	DDV_MinMaxInt(pDX, m_durationinc, 0, 1000);
	DDX_Text(pDX, IDC_LEVELINC, m_levelinc);
	DDV_MinMaxInt(pDX, m_levelinc, 0, 32);
	DDX_Text(pDX, IDC_LEVEL, m_level);
	DDV_MinMaxInt(pDX, m_level, 0, 32);
	DDX_Radio(pDX, IDC_FLAG1, m_which);
	DDX_Text(pDX, IDC_DAMAGE, m_damage);
	DDV_MinMaxInt(pDX, m_damage, 0, 10);
	DDX_Text(pDX, IDC_DAMAGEINC, m_damageinc);
	DDV_MinMaxInt(pDX, m_damageinc, 0, 10);
	DDX_Check(pDX, IDC_HALF, m_half);
	//}}AFX_DATA_MAP
}

void CSpellTool::RefreshTool()
{
}

BOOL CSpellTool::OnInitDialog() 
{
	CDialog::OnInitDialog();
	RefreshTool();
	return TRUE;
}

BEGIN_MESSAGE_MAP(CSpellTool, CDialog)
	//{{AFX_MSG_MAP(CSpellTool)
	ON_EN_KILLFOCUS(IDC_RANGE, OnKillfocusRange)
	ON_EN_KILLFOCUS(IDC_RANGEINC, OnKillfocusRangeinc)
	ON_EN_KILLFOCUS(IDC_DURATION, OnKillfocusDuration)
	ON_EN_KILLFOCUS(IDC_DURATIONINC, OnKillfocusDurationinc)
	ON_BN_CLICKED(IDC_DORANGES, OnDoranges)
	ON_BN_CLICKED(IDC_DODURATIONS, OnDodurations)
	ON_EN_KILLFOCUS(IDC_LEVEL, OnKillfocusLevel)
	ON_EN_KILLFOCUS(IDC_LEVELINC, OnKillfocusLevelinc)
	ON_BN_CLICKED(IDC_DOLEVELS, OnDolevels)
	ON_EN_KILLFOCUS(IDC_DAMAGE, OnKillfocusDamage)
	ON_EN_KILLFOCUS(IDC_DAMAGEINC, OnKillfocusDamageinc)
	ON_BN_CLICKED(IDC_DODAMAGES, OnDodamages)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpellTool message handlers

void CSpellTool::OnKillfocusRange() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CSpellTool::OnKillfocusRangeinc() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CSpellTool::OnKillfocusDuration() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CSpellTool::OnKillfocusDurationinc() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CSpellTool::OnKillfocusLevel() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CSpellTool::OnKillfocusLevelinc() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CSpellTool::OnKillfocusDamage() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CSpellTool::OnKillfocusDamageinc() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CSpellTool::OnDolevels() 
{
  int i;

  UpdateData(UD_RETRIEVE);
	for(i=0;i<the_spell.extheadcount;i++)
  {
    if(i) the_spell.extheaders[i].level=(unsigned short) (m_level*i+m_levelinc);
    else the_spell.extheaders[i].level=1;
  }	
  UpdateData(UD_DISPLAY);
}

void CSpellTool::OnDoranges() 
{
  int i;
  int level;

  UpdateData(UD_RETRIEVE);
	for(i=0;i<the_spell.extheadcount;i++)
  {
    if(m_which)
    {
      if(i) level=the_spell.extheaders[i].level;
      else level=the_spell.header.level*2-1;
      if(level<1) level=1;
    }
    else
    {
      level=i; //spell's own level
    }
    the_spell.extheaders[i].range=(unsigned short) (m_range*level+m_rangeinc);
  }	
}

void CSpellTool::OnDodurations() 
{
  int i,j,k;
  int level;

  UpdateData(UD_RETRIEVE);
	for(i=0;i<the_spell.extheadcount;i++)
  {
    if(m_which)
    {
      if(i) level=the_spell.extheaders[i].level;
      else level=the_spell.header.level*2-1;
      if(level<1) level=1;
    }
    else
    {
      level=i; //spell's own level
    }
    for(j=0;j<the_spell.extheaders[i].fbcount;j++)
    {
      k=the_spell.extheaders[i].fboffs+j;
      if(k>=0 && k<the_spell.featblkcount)
      {
        if(!the_spell.featblocks[k].timing)
        {
          the_spell.featblocks[k].duration=m_duration*level+m_durationinc;
        }
      }
    }
  }
  UpdateData(UD_DISPLAY);
}

void CSpellTool::OnDodamages() 
{
  int i,j,k,l;
  int level;

  UpdateData(UD_RETRIEVE);
	for(i=0;i<the_spell.extheadcount;i++)
  {
    if(m_which)
    {
      if(i) level=the_spell.extheaders[i].level;
      else level=the_spell.header.level*2-1;
      if(level<1) level=1;
    }
    else
    {
      level=i; //spell's own level
    }
    for(j=0;j<the_spell.extheaders[i].fbcount;j++)
    {
      k=the_spell.extheaders[i].fboffs+j;
      if(k>=0 && k<the_spell.featblkcount)
      {
        //modify only the damage effect
        if(the_spell.featblocks[k].feature==OPC_DAMAGE)
        {
          l=m_damage*level+m_damageinc;
          //handle half damage 
          if(m_half)
          {
            if((!the_spell.featblocks[k].stype) && (l&1) ) l=l/2+1;
            else l/=2;
          }
          the_spell.featblocks[k].count=l;
        }
      }
    }
  }	
  UpdateData(UD_DISPLAY);
}

/////////////////////////////////////////////////////////////////////////////
// CSpellPropertySheet

IMPLEMENT_DYNAMIC(CSpellPropertySheet, CPropertySheet)

//ids_aboutbox is a fake ID, the sheet has no caption
CSpellPropertySheet::CSpellPropertySheet(CWnd* pWndParent)
: CPropertySheet(IDS_ABOUTBOX, pWndParent)
{
  AddPage(&m_PageGeneral);
  AddPage(&m_PageDescription);
  AddPage(&m_PageEquip);
  AddPage(&m_PageExtended);
  if(iwd2_structures())
  {
    AddPage(&m_PageExtra);
  }
  AddPage(&m_PageTool);
  memset(&featcopy,0,sizeof(featcopy) );
  memset(&headcopy,0,sizeof(headcopy) );
  if(featblks)
  {
    delete [] featblks;
    featblks=NULL;
  }
  featblkcnt=0;
}

CSpellPropertySheet::~CSpellPropertySheet()
{
  if(featblks)
  {
    delete [] featblks;
    featblks=NULL;
  }
}

void CSpellPropertySheet::RefreshDialog()
{
  CPropertyPage *page;

  m_PageGeneral.RefreshGeneral();
  m_PageDescription.RefreshDescription();
  m_PageEquip.RefreshEquip();
  m_PageExtended.RefreshExtended();
  if(iwd2_structures())
  {
    m_PageExtra.RefreshExtra();
  }
  m_PageTool.RefreshTool();
  page=GetActivePage();
  page->UpdateData(UD_DISPLAY);
}

BEGIN_MESSAGE_MAP(CSpellPropertySheet, CPropertySheet)
//{{AFX_MSG_MAP(CSpellPropertySheet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
