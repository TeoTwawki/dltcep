// CreatureGeneral.cpp : implementation file
//

#include "stdafx.h"

#include "chitem.h"
#include "chitemDlg.h"
#include "EffEdit.h"
#include "CreatureProperties.h"
#include "TormentCre.h"
#include "2da.h"
#include "tbg.h"
#include "options.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CCreatureGeneral, CPropertyPage)
IMPLEMENT_DYNCREATE(CCreatureIcons, CPropertyPage)
IMPLEMENT_DYNCREATE(CCreatureSkills, CPropertyPage)
IMPLEMENT_DYNCREATE(CCreatureResist, CPropertyPage)
IMPLEMENT_DYNCREATE(CCreatureStrings, CPropertyPage)
IMPLEMENT_DYNCREATE(CCreatureItem, CPropertyPage)
IMPLEMENT_DYNCREATE(CCreatureEffect, CPropertyPage)
IMPLEMENT_DYNCREATE(CCreatureUnknown, CPropertyPage)

/////////////////////////////////////////////////////////////////////////////
// CCreatureGeneral dialog

CCreatureGeneral::CCreatureGeneral(): CPropertyPage(CCreatureGeneral::IDD)
{
	//{{AFX_DATA_INIT(CCreatureGeneral)
	m_longname = _T("");
	m_longnametag = FALSE;
	m_shortname = _T("");
	m_shortnametag = FALSE;
	//}}AFX_DATA_INIT
}

CCreatureGeneral::~CCreatureGeneral()
{
}

CString CCreatureGeneral::FindKit(unsigned int kit)
{
  CString tmpstr, kitname;
  kitname = IDSToken("KIT", kit>>16);
  if(kitname.GetLength()) tmpstr.Format("0x%08x - %s",kit, kitname);
  else tmpstr.Format("0x%08x",kit);
  return tmpstr;
}

void CCreatureGeneral::DoDataExchange(CDataExchange* pDX)
{
  CString tmpstr;
  creature_header tmpheader;
  int value;

  memcpy(&tmpheader,&the_creature.header,sizeof(creature_header) );
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCreatureGeneral)
	DDX_Text(pDX, IDC_LONGNAME, m_longname);
	DDX_Check(pDX, IDC_LONGNAMETAG, m_longnametag);
	DDX_Text(pDX, IDC_SHORTNAME, m_shortname);
	DDX_Check(pDX, IDC_SHORTNAMETAG, m_shortnametag);
	//}}AFX_DATA_MAP
  DDX_Text(pDX, IDC_LONGNAMEREF, the_creature.header.longname);
  DDX_Text(pDX, IDC_SHORTNAMEREF, the_creature.header.shortname);

  tmpstr.Format("0x%08x",the_creature.header.flags);
  DDX_Text(pDX, IDC_DUALFLAGS, tmpstr);
  the_creature.header.flags=strtonum(tmpstr);
  DDX_Text(pDX, IDC_LEVEL1, the_creature.header.levels[0]);
  DDX_Text(pDX, IDC_LEVEL2, the_creature.header.levels[1]);
  DDX_Text(pDX, IDC_LEVEL3, the_creature.header.levels[2]);
  DDX_Text(pDX, IDC_XPVALUE, the_creature.header.expval);
  DDX_Text(pDX, IDC_CURRXP, the_creature.header.expcur);
  DDX_Text(pDX, IDC_CURRHP, the_creature.header.curhp);
  DDX_Text(pDX, IDC_MAXHP, the_creature.header.maxhp);
  DDX_Text(pDX, IDC_GOLD, the_creature.header.gold);

  tmpstr.Format("0x%08x",the_creature.header.state);
  DDX_Text(pDX, IDC_STATEFLAGS, tmpstr);
  the_creature.header.state=strtonum(tmpstr);
  tmpstr.Format("0x%04x %s",the_creature.header.animid,IDSToken("ANIMATE",the_creature.header.animid) );
  DDX_Text(pDX, IDC_ANIMATION, tmpstr);
  the_creature.header.animid=IDSKey("ANIMATE", tmpstr);
  if(the_creature.header.animid==0xffffffff)
  {
    the_creature.header.animid=strtonum(tmpstr);
  }

  value=the_creature.header.thac0;
  DDX_Text(pDX, IDC_THAC0, value);
  DDV_MinMaxInt(pDX, value, -20,30);
  the_creature.header.thac0=(char) value;

  tmpstr=get_attacknum(the_creature.header.attacks);
  DDX_Text(pDX, IDC_NUMATTACKS, tmpstr);
  the_creature.header.attacks=(BYTE) find_attacknum(tmpstr);
  DDV_MinMaxInt(pDX,the_creature.header.attacks,0,10); //10

  DDX_Text(pDX, IDC_MORALE, the_creature.header.morale);
  DDX_Text(pDX, IDC_MBREAK, the_creature.header.moralebreak);
  DDX_Text(pDX, IDC_MRECOVERY, the_creature.header.moralerecover);

  tmpstr.Format("0x%x %s",the_creature.header.idsea,IDSToken("EA",the_creature.header.idsea) );
  DDX_Text(pDX, IDC_IDSEA, tmpstr);
  value=IDSKey("EA",tmpstr);
  if(value==-1)
  {
    the_creature.header.idsea=(BYTE) strtonum(tmpstr);
  }
  else
  {
    the_creature.header.idsea=(BYTE) value;
  }

  tmpstr.Format("0x%x %s",the_creature.header.idsgeneral,IDSToken("GENERAL",the_creature.header.idsgeneral) );
  DDX_Text(pDX, IDC_IDSGENERAL, tmpstr);
  value=IDSKey("GENERAL", tmpstr);
  if(value==-1)
  {
    the_creature.header.idsgeneral=(BYTE) strtonum(tmpstr);
  }
  else
  {
    the_creature.header.idsgeneral=(BYTE) value;
  }

  tmpstr.Format("0x%x %s",the_creature.header.idsrace,IDSToken("RACE",the_creature.header.idsrace) );
  DDX_Text(pDX, IDC_IDSRACE, tmpstr);
  value=IDSKey("RACE", tmpstr);
  if(value==-1)
  {
    the_creature.header.idsrace=(BYTE) strtonum(tmpstr);
  }
  else
  {
    the_creature.header.idsrace=(BYTE) value;
  }

  tmpstr.Format("0x%x %s",the_creature.header.idsclass,IDSToken("CLASS",the_creature.header.idsclass) );
  DDX_Text(pDX, IDC_IDSCLASS, tmpstr);
  value=IDSKey("CLASS", tmpstr);
  if(value==-1)
  {
    the_creature.header.idsclass=(BYTE) strtonum(tmpstr);
  }
  else
  {
    the_creature.header.idsclass=(BYTE) value;
  }

  tmpstr.Format("0x%x %s",the_creature.header.idsspecific,IDSToken("SPECIFIC",the_creature.header.idsspecific) );
  DDX_Text(pDX, IDC_IDSSPECIFIC, tmpstr);
  value=IDSKey("SPECIFIC", tmpstr);
  if(value==-1)
  {
    the_creature.header.idsspecific=(BYTE) strtonum(tmpstr);
  }
  else
  {
    the_creature.header.idsspecific=(BYTE) value;
  }

  tmpstr.Format("0x%x %s",the_creature.header.idsgender,IDSToken("GENDER",the_creature.header.idsgender) );
  DDX_Text(pDX, IDC_IDSGENDER, tmpstr);
  value=IDSKey("GENDER", tmpstr);
  if(value==-1)
  {
    the_creature.header.idsgender=(BYTE) strtonum(tmpstr);
  }
  else
  {
    the_creature.header.idsgender=(BYTE) value;
  }

  tmpstr.Format("0x%x %s",the_creature.header.idsalign,IDSToken(pst_compatible_var()?"ALIGN":IDSName(ALIGN,false),the_creature.header.idsalign) );
  DDX_Text(pDX, IDC_IDSALIGNMENT, tmpstr);
  value=IDSKey(IDSName(ALIGN,false), tmpstr);
  if(value==-1)
  {
    the_creature.header.idsalign=(BYTE) strtonum(tmpstr);
  }
  else
  {
    the_creature.header.idsalign=(BYTE) value;
  }

  tmpstr.Format("0x%x %s",the_creature.header.enemy,IDSToken("RACE",the_creature.header.enemy) );
  DDX_Text(pDX, IDC_HATED, tmpstr);
  value=IDSKey("RACE", tmpstr);
  if(value==-1)
  {
    the_creature.header.enemy=(BYTE) strtonum(tmpstr);
  }
  else
  {
    the_creature.header.enemy=(BYTE) value;
  }

  //tmpstr.Format("0x%08x",the_creature.header.kit);
  tmpstr=FindKit(the_creature.header.kit);
  DDX_Text(pDX,IDC_KIT,tmpstr);
  the_creature.header.kit=strtonum(tmpstr);
  if(memcmp(&tmpheader,&the_creature.header,sizeof(creature_header) ))
  {
    the_creature.m_changed=true;
  }
}

static int radioids[4]={IDC_V10,IDC_V12,IDC_V22,IDC_V90};

//move itemdata to local variables
void CCreatureGeneral::RefreshGeneral()
{
  int i;
  int id;
  CButton *cb;

  m_longname=resolve_tlk_text(the_creature.header.longname);
  m_shortname=resolve_tlk_text(the_creature.header.shortname);
  m_longnametag=resolve_tlk_tag(the_creature.header.longname);
  m_shortnametag=resolve_tlk_tag(the_creature.header.shortname);
  switch(the_creature.revision)
  {
  case 10:
    id=0;
    break;
  case 12:
    id=1;
    break;
  case 22:
    id=2;
    break;
  case 90:
    id=3;
    break;
  }
  for(i=0;i<4;i++)
  {
    cb=(CButton *) GetDlgItem(radioids[i]);
    if(i==id) cb->SetCheck(true);
    else cb->SetCheck(false);
  }
}

BOOL CCreatureGeneral::OnInitDialog() 
{
  CComboBox *cb;

	CPropertyPage::OnInitDialog();
  RefreshGeneral();
  
	cb=(CComboBox *) GetDlgItem(IDC_STATEFLAGS);
  cb->SetDroppedWidth(200);
  FillCombo("STATE",cb,8);

	cb=(CComboBox *) GetDlgItem(IDC_ANIMATION);
  cb->SetDroppedWidth(200);
  FillCombo("ANIMATE",cb,4);

	cb=(CComboBox *) GetDlgItem(IDC_IDSEA);
  FillCombo("EA",cb,2);

	cb=(CComboBox *) GetDlgItem(IDC_IDSGENERAL);
  FillCombo("GENERAL",cb,2);

	cb=(CComboBox *) GetDlgItem(IDC_IDSRACE);
  FillCombo("RACE",cb,2);

	cb=(CComboBox *) GetDlgItem(IDC_IDSCLASS);
  FillCombo("CLASS",cb,2);

	cb=(CComboBox *) GetDlgItem(IDC_IDSSPECIFIC);
  FillCombo("SPECIFIC",cb,2);

	cb=(CComboBox *) GetDlgItem(IDC_IDSGENDER);
  FillCombo("GENDER",cb,2);

	cb=(CComboBox *) GetDlgItem(IDC_IDSALIGNMENT);
  if(pst_compatible_var()) FillCombo("ALIGN",cb,2);
  else FillCombo(IDSName(ALIGN,false),cb,2);

	cb=(CComboBox *) GetDlgItem(IDC_HATED);
  FillCombo("RACE",cb,2);

  UpdateData(UD_DISPLAY);
	return TRUE;
}

BEGIN_MESSAGE_MAP(CCreatureGeneral, CPropertyPage)
	//{{AFX_MSG_MAP(CCreatureGeneral)
	ON_EN_KILLFOCUS(IDC_LONGNAME, OnKillfocusLongname)
	ON_EN_KILLFOCUS(IDC_LONGNAMEREF, OnKillfocusLongnameref)
	ON_BN_CLICKED(IDC_LONGNAMETAG, OnLongnametag)
	ON_EN_KILLFOCUS(IDC_SHORTNAME, OnKillfocusShortname)
	ON_EN_KILLFOCUS(IDC_SHORTNAMEREF, OnKillfocusShortnameref)
	ON_BN_CLICKED(IDC_SHORTNAMETAG, OnShortnametag)
	ON_EN_KILLFOCUS(IDC_XPVALUE, OnKillfocusXpvalue)
	ON_CBN_KILLFOCUS(IDC_STATEFLAGS, OnKillfocusStateflags)
	ON_EN_KILLFOCUS(IDC_CURRXP, OnKillfocusCurrxp)
	ON_EN_KILLFOCUS(IDC_GOLD, OnKillfocusGold)
	ON_EN_KILLFOCUS(IDC_CURRHP, OnKillfocusCurrhp)
	ON_EN_KILLFOCUS(IDC_MAXHP, OnKillfocusMaxhp)
	ON_CBN_KILLFOCUS(IDC_ANIMATION, OnKillfocusAnimation)
	ON_EN_KILLFOCUS(IDC_LEVEL1, OnKillfocusLevel1)
	ON_EN_KILLFOCUS(IDC_LEVEL2, OnKillfocusLevel2)
	ON_EN_KILLFOCUS(IDC_LEVEL3, OnKillfocusLevel3)
	ON_EN_KILLFOCUS(IDC_MORALE, OnKillfocusMorale)
	ON_EN_KILLFOCUS(IDC_MBREAK, OnKillfocusMbreak)
	ON_EN_KILLFOCUS(IDC_MRECOVERY, OnKillfocusMrecovery)
	ON_CBN_KILLFOCUS(IDC_IDSEA, OnKillfocusIdsea)
	ON_CBN_KILLFOCUS(IDC_IDSGENERAL, OnKillfocusIdsgeneral)
	ON_CBN_KILLFOCUS(IDC_IDSRACE, OnKillfocusIdsrace)
	ON_CBN_KILLFOCUS(IDC_IDSCLASS, OnKillfocusIdsclass)
	ON_CBN_KILLFOCUS(IDC_IDSSPECIFIC, OnKillfocusIdsspecific)
	ON_CBN_KILLFOCUS(IDC_IDSGENDER, OnKillfocusIdsgender)
	ON_CBN_KILLFOCUS(IDC_IDSALIGNMENT, OnKillfocusIdsalignment)
	ON_CBN_KILLFOCUS(IDC_HATED, OnKillfocusHated)
	ON_EN_KILLFOCUS(IDC_THAC0, OnKillfocusThac0)
	ON_CBN_KILLFOCUS(IDC_NUMATTACKS, OnKillfocusNumattacks)
	ON_CBN_KILLFOCUS(IDC_DUALFLAGS, OnKillfocusDualflags)
	ON_EN_KILLFOCUS(IDC_KIT, OnKillfocusKit)
	ON_BN_CLICKED(IDC_V10, OnV10)
	ON_BN_CLICKED(IDC_V22, OnV22)
	ON_BN_CLICKED(IDC_V90, OnV90)
	ON_BN_CLICKED(IDC_V12, OnV12)
	ON_BN_CLICKED(IDC_NEW1, OnNew1)
	ON_BN_CLICKED(IDC_NEW2, OnNew2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CCreatureGeneral message handlers
/////////////////////////////////////////////////////////////////////////////
void CCreatureGeneral::OnKillfocusDualflags() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureGeneral::OnKillfocusLongname() 
{
  CString tmpstr;

  UpdateData(UD_RETRIEVE);
  tmpstr=resolve_tlk_text(the_creature.header.longname);
  if(tmpstr!=m_longname)
  {
    if(editflg&TLKCHANGE)
    {
      if(MessageBox("Do you want to update dialog.tlk?","Creature editor",MB_YESNO)!=IDYES)
      {
        return;
      }      
    }
    //this handles the reference changes and adds a new string if needed
    the_creature.header.longname=store_tlk_text(the_creature.header.longname,m_longname);
    RefreshGeneral();
    UpdateData(UD_DISPLAY);
  }
}

void CCreatureGeneral::OnKillfocusLongnameref() 
{
  UpdateData(UD_RETRIEVE);
 	RefreshGeneral();
  UpdateData(UD_DISPLAY);
}

void CCreatureGeneral::OnLongnametag() 
{
  if((editflg&TLKCHANGE) )
  {
    if(MessageBox("Do you want to update dialog.tlk?","Creature editor",MB_YESNO)!=IDYES)
    {
      return;
    }
  }
  toggle_tlk_tag(the_creature.header.longname);
  UpdateData(UD_DISPLAY);
}

void CCreatureGeneral::OnKillfocusShortname() 
{
  CString tmpstr;

  UpdateData(UD_RETRIEVE);
  tmpstr=resolve_tlk_text(the_creature.header.shortname);
  if(tmpstr!=m_shortname)
  {
    if(editflg&TLKCHANGE)
    {
      if(MessageBox("Do you want to update dialog.tlk?","Creature editor",MB_YESNO)!=IDYES)
      {
        return;
      }      
    }
    //this handles the reference changes and adds a new string if needed
    the_creature.header.shortname=store_tlk_text(the_creature.header.shortname,m_shortname);
    RefreshGeneral();
    UpdateData(UD_DISPLAY);
  }
}

void CCreatureGeneral::OnKillfocusShortnameref() 
{
  UpdateData(UD_RETRIEVE);
 	RefreshGeneral();
  UpdateData(UD_DISPLAY);
}

void CCreatureGeneral::OnShortnametag() 
{
  if((editflg&TLKCHANGE) )
  {
    if(MessageBox("Do you want to update dialog.tlk?","Creature editor",MB_YESNO)!=IDYES)
    {
      return;
    }
  }
  toggle_tlk_tag(the_creature.header.shortname);
  UpdateData(UD_DISPLAY);
}

void CCreatureGeneral::OnNew1() 
{
  the_creature.header.longname=-1;
  RefreshGeneral();
  UpdateData(UD_DISPLAY);
}

void CCreatureGeneral::OnNew2() 
{
  the_creature.header.shortname=-1;
  RefreshGeneral();
  UpdateData(UD_DISPLAY);
}

void CCreatureGeneral::OnKillfocusStateflags() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureGeneral::OnKillfocusXpvalue() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureGeneral::OnKillfocusCurrxp() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureGeneral::OnKillfocusLevel1() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureGeneral::OnKillfocusLevel2() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureGeneral::OnKillfocusLevel3() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);	
}

void CCreatureGeneral::OnKillfocusGold() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureGeneral::OnKillfocusCurrhp() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureGeneral::OnKillfocusMaxhp() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureGeneral::OnKillfocusAnimation() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureGeneral::OnKillfocusThac0() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureGeneral::OnKillfocusNumattacks() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureGeneral::OnKillfocusMorale() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureGeneral::OnKillfocusMbreak() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureGeneral::OnKillfocusMrecovery() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureGeneral::OnKillfocusIdsea() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureGeneral::OnKillfocusIdsgeneral() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureGeneral::OnKillfocusIdsrace() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureGeneral::OnKillfocusIdsclass() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureGeneral::OnKillfocusIdsspecific() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureGeneral::OnKillfocusIdsgender() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureGeneral::OnKillfocusIdsalignment() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureGeneral::OnKillfocusHated() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureGeneral::OnKillfocusKit() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureGeneral::OnV10() 
{
	the_creature.revision=10;
}

void CCreatureGeneral::OnV22() 
{
	the_creature.revision=22;
}

void CCreatureGeneral::OnV90() 
{
	the_creature.revision=90;
}

void CCreatureGeneral::OnV12() 
{
	the_creature.revision=12;
}

/////////////////////////////////////////////////////////////////////////////
// CCreatureIcons dialog

CCreatureIcons::CCreatureIcons(): CPropertyPage(CCreatureIcons::IDD)
{
	//{{AFX_DATA_INIT(CCreatureIcons)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
  m_play=false;
  m_bgcolor=RGB(32,32,32);
  m_hb1=NULL;
  m_hb2=NULL;
}

CCreatureIcons::~CCreatureIcons()
{
  if(m_hb1) DeleteObject(m_hb1);
  if(m_hb2) DeleteObject(m_hb2);
}

void CCreatureIcons::DoDataExchange(CDataExchange* pDX)
{
  CWnd *cw;
  CString tmpstr;
  int fc;
  int ret;
  int pst;
  int i;
  creature_header tmpheader;

  memcpy(&tmpheader,&the_creature.header,sizeof(creature_header) );
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCreatureIcons)
	DDX_Control(pDX, IDC_LARGEPORTRAITICON, m_largeportrait);
	DDX_Control(pDX, IDC_SMALLPORTRAITICON, m_smallportrait);
	//}}AFX_DATA_MAP
  RetrieveResref(tmpstr, the_creature.header.iconm);
  DDX_Text(pDX, IDC_SMALLPORTRAIT, tmpstr);
  DDV_MaxChars(pDX, tmpstr, 8);
  StoreResref(tmpstr, the_creature.header.iconm);
  if(m_bms!=tmpstr)
  {
    m_bms=tmpstr;
    
    if(!read_bmp(tmpstr,m_hb1))
    {
      m_smallportrait.SetBitmap(m_hb1);
    }
    else
    {
      m_smallportrait.SetBitmap(0);
    }
  }

  RetrieveResref(tmpstr, the_creature.header.iconl);
  DDX_Text(pDX, IDC_LARGEPORTRAIT, tmpstr);
  DDV_MaxChars(pDX, tmpstr, 8);
  StoreResref(tmpstr, the_creature.header.iconl);
  cw=GetDlgItem(IDC_PLAY);
  pst=the_creature.revision==12;
  cw->EnableWindow(pst);
  if(m_bml!=tmpstr)
  {
    m_bml=tmpstr;
    if(pst)
    {
      ret=read_bam(tmpstr);
      fc=the_bam.GetFrameIndex(1,0);
      the_bam.MakeBitmap(fc,RGB(32,32,32),m_hb2,BM_RESIZE,32,32);
    }
    else ret=read_bmp(tmpstr,m_hb2);
    if(!ret)
    {
      m_largeportrait.SetBitmap(m_hb2);
    }
    else
    {
      m_largeportrait.SetBitmap(0);
    }
  }

  for(i=0;i<7;i++)
  {
  tmpstr=colortitle(the_creature.header.colours[i]);
  DDX_Text(pDX, IDC_CMETAL+i, tmpstr);
  the_creature.header.colours[i]=(BYTE) strtonum(tmpstr);
  }
  /*
  tmpstr=colortitle(the_creature.header.colours[1]);
  DDX_Text(pDX, IDC_CMINOR, tmpstr);
  the_creature.header.colours[1]=(BYTE) strtonum(tmpstr);
 
  tmpstr=colortitle(the_creature.header.colours[2]);
  DDX_Text(pDX, IDC_CMAJOR, tmpstr);
  the_creature.header.colours[2]=(BYTE) strtonum(tmpstr);

  tmpstr=colortitle(the_creature.header.colours[3]);
  DDX_Text(pDX, IDC_CSKIN, tmpstr);
  the_creature.header.colours[3]=(BYTE) strtonum(tmpstr);

  tmpstr=colortitle(the_creature.header.colours[4]);
  DDX_Text(pDX, IDC_CLEATHER, tmpstr);
  the_creature.header.colours[4]=(BYTE) strtonum(tmpstr);

  tmpstr=colortitle(the_creature.header.colours[5]);
  DDX_Text(pDX, IDC_CARMOR, tmpstr);
  the_creature.header.colours[5]=(BYTE) strtonum(tmpstr);

  tmpstr=colortitle(the_creature.header.colours[6]);
  DDX_Text(pDX, IDC_CHAIR, tmpstr);
  the_creature.header.colours[6]=(BYTE) strtonum(tmpstr);
*/
  RetrieveResref(tmpstr,the_creature.header.scripts[0]);
  DDX_Text(pDX, IDC_OVERRIDE, tmpstr);
  DDV_MaxChars(pDX, tmpstr, 8);
  StoreResref(tmpstr,the_creature.header.scripts[0]);

  RetrieveResref(tmpstr,the_creature.header.scripts[1]);
  DDX_Text(pDX, IDC_CLASS, tmpstr);
  DDV_MaxChars(pDX, tmpstr, 8);
  StoreResref(tmpstr,the_creature.header.scripts[1]);

  RetrieveResref(tmpstr,the_creature.header.scripts[2]);
  DDX_Text(pDX, IDC_RACE, tmpstr);
  DDV_MaxChars(pDX, tmpstr, 8);
  StoreResref(tmpstr,the_creature.header.scripts[2]);

  RetrieveResref(tmpstr,the_creature.header.scripts[3]);
  DDX_Text(pDX, IDC_GENERAL, tmpstr);
  DDV_MaxChars(pDX, tmpstr, 8);
  StoreResref(tmpstr,the_creature.header.scripts[3]);

  RetrieveResref(tmpstr,the_creature.header.scripts[4]);
  DDX_Text(pDX, IDC_DEFAULT, tmpstr);
  DDV_MaxChars(pDX, tmpstr, 8);
  StoreResref(tmpstr,the_creature.header.scripts[4]);

  RetrieveResref(tmpstr,the_creature.header.dialogresref);
  DDX_Text(pDX, IDC_DIALOG, tmpstr);
  DDV_MaxChars(pDX, tmpstr, 8);
  StoreResref(tmpstr,the_creature.header.dialogresref);

  RetrieveVariable(tmpstr,the_creature.header.dvar);
  DDX_Text(pDX, IDC_DVAR, tmpstr);
  DDV_MaxChars(pDX, tmpstr, 32);
  StoreVariable(tmpstr,the_creature.header.dvar);
  if(memcmp(&tmpheader,&the_creature.header,sizeof(creature_header) ))
  {
    the_creature.m_changed=true;
  }
}

//move itemdata to local variables
void CCreatureIcons::RefreshIcons()
{
  
}

BOOL CCreatureIcons::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();	
	return TRUE;
}

BEGIN_MESSAGE_MAP(CCreatureIcons, CPropertyPage)
	//{{AFX_MSG_MAP(CCreatureIcons)
	ON_EN_KILLFOCUS(IDC_SMALLPORTRAIT, OnKillfocusSmallportrait)
	ON_EN_KILLFOCUS(IDC_LARGEPORTRAIT, OnKillfocusLargeportrait)
	ON_EN_KILLFOCUS(IDC_CARMOR, OnKillfocusCarmor)
	ON_EN_KILLFOCUS(IDC_CHAIR, OnKillfocusChair)
	ON_EN_KILLFOCUS(IDC_CLEATHER, OnKillfocusCleather)
	ON_EN_KILLFOCUS(IDC_CMAJOR, OnKillfocusCmajor)
	ON_EN_KILLFOCUS(IDC_CMETAL, OnKillfocusCmetal)
	ON_EN_KILLFOCUS(IDC_CMINOR, OnKillfocusCminor)
	ON_EN_KILLFOCUS(IDC_CSKIN, OnKillfocusCskin)
	ON_EN_KILLFOCUS(IDC_OVERRIDE, OnKillfocusOverride)
	ON_EN_KILLFOCUS(IDC_CLASS, OnKillfocusClass)
	ON_EN_KILLFOCUS(IDC_RACE, OnKillfocusRace)
	ON_EN_KILLFOCUS(IDC_GENERAL, OnKillfocusGeneral)
	ON_EN_KILLFOCUS(IDC_DEFAULT, OnKillfocusDefault)
	ON_EN_KILLFOCUS(IDC_DIALOG, OnKillfocusDialog)
	ON_EN_KILLFOCUS(IDC_DVAR, OnKillfocusDvar)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_BROWSE1, OnBrowse1)
	ON_BN_CLICKED(IDC_BROWSE2, OnBrowse2)
	ON_BN_CLICKED(IDC_BROWSE3, OnBrowse3)
	ON_BN_CLICKED(IDC_BROWSE4, OnBrowse4)
	ON_BN_CLICKED(IDC_BROWSE5, OnBrowse5)
	ON_BN_CLICKED(IDC_BROWSE6, OnBrowse6)
	ON_BN_CLICKED(IDC_BROWSE7, OnBrowse7)
	ON_BN_CLICKED(IDC_BROWSE8, OnBrowse8)
	ON_BN_CLICKED(IDC_BROWSE9, OnBrowse9)
	ON_BN_CLICKED(IDC_BROWSE10, OnBrowse10)
	ON_BN_CLICKED(IDC_BROWSE11, OnBrowse11)
	ON_BN_CLICKED(IDC_BROWSE12, OnBrowse12)
	ON_BN_CLICKED(IDC_BROWSE13, OnBrowse13)
	ON_BN_CLICKED(IDC_BROWSE14, OnBrowse14)
	ON_BN_CLICKED(IDC_PLAY, OnPlay)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCreatureIcons message handlers
/////////////////////////////////////////////////////////////////////////////

void CCreatureIcons::OnKillfocusSmallportrait() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureIcons::OnKillfocusLargeportrait() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureIcons::OnKillfocusCarmor() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureIcons::OnKillfocusChair() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureIcons::OnKillfocusCleather() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureIcons::OnKillfocusCmajor() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureIcons::OnKillfocusCmetal() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureIcons::OnKillfocusCminor() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureIcons::OnKillfocusCskin() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureIcons::OnKillfocusOverride() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureIcons::OnKillfocusClass() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureIcons::OnKillfocusRace() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureIcons::OnKillfocusGeneral() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureIcons::OnKillfocusDefault() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureIcons::OnKillfocusDialog() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureIcons::OnKillfocusDvar() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureIcons::OnBrowse() 
{
  pickerdlg.m_restype=REF_DLG;
  RetrieveResref(pickerdlg.m_picked,the_creature.header.dialogresref);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_creature.header.dialogresref);
    UpdateData(UD_DISPLAY);
  }
}

void CCreatureIcons::OnBrowseBCS(int idx) 
{
  pickerdlg.m_restype=REF_BCS;
  RetrieveResref(pickerdlg.m_picked,the_creature.header.scripts[idx]);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_creature.header.scripts[idx]);
    UpdateData(UD_DISPLAY);
  }
}

void CCreatureIcons::OnBrowse1() 
{
  OnBrowseBCS(0);
}

void CCreatureIcons::OnBrowse2() 
{
  OnBrowseBCS(1);
}

void CCreatureIcons::OnBrowse3() 
{
  OnBrowseBCS(2);
}

void CCreatureIcons::OnBrowse4() 
{
  OnBrowseBCS(3);
}

void CCreatureIcons::OnBrowse5() 
{
  OnBrowseBCS(4);
}

void CCreatureIcons::ColorDlg(int idx)
{
  colordlg.m_picked=the_creature.header.colours[idx];
  if(colordlg.DoModal()==IDOK)
  {
    the_creature.header.colours[idx]=(unsigned char) colordlg.m_picked;
    UpdateData(UD_DISPLAY);
  }
}

void CCreatureIcons::OnBrowse6() 
{
  ColorDlg(0);
}

void CCreatureIcons::OnBrowse7() 
{
  ColorDlg(1);
}

void CCreatureIcons::OnBrowse8() 
{
  ColorDlg(2);
}

void CCreatureIcons::OnBrowse9() 
{
  ColorDlg(3);
}

void CCreatureIcons::OnBrowse10() 
{
  ColorDlg(4);
}

void CCreatureIcons::OnBrowse11() 
{
  ColorDlg(5);
}

void CCreatureIcons::OnBrowse12() 
{
  ColorDlg(6);
}

void CCreatureIcons::OnBrowse13() 
{
  pickerdlg.m_restype=REF_BMP;
  RetrieveResref(pickerdlg.m_picked,the_creature.header.iconm);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_creature.header.iconm);
    UpdateData(UD_DISPLAY);
  }
}

void CCreatureIcons::OnBrowse14() 
{
  if(the_creature.revision==12) pickerdlg.m_restype=REF_BAM; //bam for pst
  else pickerdlg.m_restype=REF_BMP;
  RetrieveResref(pickerdlg.m_picked,the_creature.header.iconl);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_creature.header.iconl);
    UpdateData(UD_DISPLAY);
  }
}

void CCreatureIcons::OnTimer(UINT nIDEvent) 
{
  int nFrameIndex;

  if(!m_play || m_playindex>m_playmax)
  {
    KillTimer(nIDEvent);
    return;
  }
  if(m_playindex==m_playmax)
  {
    m_playindex=0;
    m_play=false;
  }
  nFrameIndex=the_bam.GetFrameIndex(0, m_playindex);
  the_bam.MakeBitmap(nFrameIndex,m_bgcolor,m_hb2,BM_RESIZE,1,1);
  m_largeportrait.SetBitmap(m_hb2);
  m_playindex++;
	CPropertyPage::OnTimer(nIDEvent);
}

void CCreatureIcons::OnPlay() 
{
  m_playmax=the_bam.GetCycleData(0).y;
  m_playindex=0;
  m_play=true;
  SetTimer(0,100,NULL);
}

/////////////////////////////////////////////////////////////////////////////
// CCreatureSkills dialog

CCreatureSkills::CCreatureSkills(): CPropertyPage(CCreatureSkills::IDD)
{
	//{{AFX_DATA_INIT(CCreatureSkills)
	m_pst = _T("");
	//}}AFX_DATA_INIT
}

CCreatureSkills::~CCreatureSkills()
{
}

void CCreatureSkills::DoDataExchange(CDataExchange* pDX)
{
  creature_header tmpheader;

  memcpy(&tmpheader,&the_creature.header,sizeof(creature_header) );
	CPropertyPage::DoDataExchange(pDX);
  if(the_creature.revision==12) m_pst="Unused prof.";
  else m_pst="Detect illusions";
	//{{AFX_DATA_MAP(CCreatureSkills)
	DDX_Text(pDX, IDC_PST, m_pst);
	//}}AFX_DATA_MAP
  DDX_Text(pDX, IDC_HIDE, the_creature.header.hideskill);
  DDX_Text(pDX, IDC_DETILL, the_creature.header.detillusion);
  DDX_Text(pDX, IDC_SETTRAP, the_creature.header.settraps);
  DDX_Text(pDX, IDC_LORE, the_creature.header.lore);
  DDX_Text(pDX, IDC_LOCKPICK, the_creature.header.lockpick);
  DDX_Text(pDX, IDC_STEALTH, the_creature.header.stealth);
  DDX_Text(pDX, IDC_FINDTRAP, the_creature.header.findtrap);
  DDX_Text(pDX, IDC_PICKPOCKET, the_creature.header.pickpock);
  DDX_Text(pDX, IDC_LUCK, the_creature.header.luck);
  DDX_Text(pDX, IDC_TRACKING, the_creature.header.tracking);

  DDX_Text(pDX, IDC_AC, the_creature.header.natac);
  DDX_Text(pDX, IDC_EFFAC, the_creature.header.effac);
  DDX_Text(pDX, IDC_CRUSHING, the_creature.header.crushac);
  DDX_Text(pDX, IDC_MISSILE, the_creature.header.missac);
  DDX_Text(pDX, IDC_SLASHING, the_creature.header.slashac);
  DDX_Text(pDX, IDC_PIERCING, the_creature.header.piercac);

  DDX_Text(pDX, IDC_STR, the_creature.header.strstat);
  DDX_Text(pDX, IDC_INT, the_creature.header.intstat);
  DDX_Text(pDX, IDC_WIS, the_creature.header.wisstat);
  DDX_Text(pDX, IDC_DEX, the_creature.header.dexstat);
  DDX_Text(pDX, IDC_CON, the_creature.header.constat);
  DDX_Text(pDX, IDC_CHA, the_creature.header.chastat);
  DDX_Text(pDX, IDC_STRBON, the_creature.header.strbon);
  DDX_Text(pDX, IDC_FATIGUE, the_creature.header.fatigue);
  DDX_Text(pDX, IDC_INTOX, the_creature.header.intox);
  if(memcmp(&tmpheader,&the_creature.header,sizeof(creature_header) ))
  {
    the_creature.m_changed=true;
  }
}

//move itemdata to local variables
void CCreatureSkills::RefreshSkills()
{
  
}

BEGIN_MESSAGE_MAP(CCreatureSkills, CPropertyPage)
	//{{AFX_MSG_MAP(CCreatureSkills)
	ON_EN_KILLFOCUS(IDC_HIDE, OnKillfocusHide)
	ON_EN_KILLFOCUS(IDC_DETILL, OnKillfocusDetill)
	ON_EN_KILLFOCUS(IDC_SETTRAP, OnKillfocusSettrap)
	ON_EN_KILLFOCUS(IDC_LORE, OnKillfocusLore)
	ON_EN_KILLFOCUS(IDC_LOCKPICK, OnKillfocusLockpick)
	ON_EN_KILLFOCUS(IDC_STEALTH, OnKillfocusStealth)
	ON_EN_KILLFOCUS(IDC_FINDTRAP, OnKillfocusFindtrap)
	ON_EN_KILLFOCUS(IDC_PICKPOCKET, OnKillfocusPickpocket)
	ON_EN_KILLFOCUS(IDC_LUCK, OnKillfocusLuck)
	ON_EN_KILLFOCUS(IDC_TRACKING, OnKillfocusTracking)
	ON_EN_KILLFOCUS(IDC_STR, OnKillfocusStr)
	ON_EN_KILLFOCUS(IDC_INT, OnKillfocusInt)
	ON_EN_KILLFOCUS(IDC_DEX, OnKillfocusDex)
	ON_EN_KILLFOCUS(IDC_CHA, OnKillfocusCha)
	ON_EN_KILLFOCUS(IDC_FATIGUE, OnKillfocusFatigue)
	ON_EN_KILLFOCUS(IDC_STRBON, OnKillfocusStrbon)
	ON_EN_KILLFOCUS(IDC_WIS, OnKillfocusWis)
	ON_EN_KILLFOCUS(IDC_CON, OnKillfocusCon)
	ON_EN_KILLFOCUS(IDC_INTOX, OnKillfocusIntox)
	ON_EN_KILLFOCUS(IDC_AC, OnKillfocusAc)
	ON_EN_KILLFOCUS(IDC_EFFAC, OnKillfocusEffac)
	ON_EN_KILLFOCUS(IDC_CRUSHING, OnKillfocusCrushing)
	ON_EN_KILLFOCUS(IDC_MISSILE, OnKillfocusMissile)
	ON_EN_KILLFOCUS(IDC_SLASHING, OnKillfocusSlashing)
	ON_EN_KILLFOCUS(IDC_PIERCING, OnKillfocusPiercing)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCreatureSkills message handlers
/////////////////////////////////////////////////////////////////////////////

// skills
void CCreatureSkills::OnKillfocusHide() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureSkills::OnKillfocusDetill() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureSkills::OnKillfocusSettrap() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureSkills::OnKillfocusLore() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureSkills::OnKillfocusLockpick() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureSkills::OnKillfocusStealth() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureSkills::OnKillfocusFindtrap() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureSkills::OnKillfocusPickpocket() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureSkills::OnKillfocusLuck() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureSkills::OnKillfocusTracking() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

// stats
void CCreatureSkills::OnKillfocusStr() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureSkills::OnKillfocusInt() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureSkills::OnKillfocusDex() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureSkills::OnKillfocusCha() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureSkills::OnKillfocusFatigue() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureSkills::OnKillfocusStrbon() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureSkills::OnKillfocusWis() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureSkills::OnKillfocusCon() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureSkills::OnKillfocusIntox() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}


void CCreatureSkills::OnKillfocusAc() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureSkills::OnKillfocusEffac() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureSkills::OnKillfocusCrushing() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureSkills::OnKillfocusMissile() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureSkills::OnKillfocusSlashing() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureSkills::OnKillfocusPiercing() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

/////////////////////////////////////////////////////////////////////////////
// CCreatureResist dialog

CCreatureResist::CCreatureResist(): CPropertyPage(CCreatureResist::IDD)
{
	//{{AFX_DATA_INIT(CCreatureResist)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CCreatureResist::~CCreatureResist()
{
}

void CCreatureResist::DoDataExchange(CDataExchange* pDX)
{
  int tmp;
  creature_header tmpheader;

  memcpy(&tmpheader,&the_creature.header,sizeof(creature_header) );
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCreatureResist)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP

  DDX_Text(pDX,IDC_DEATH, the_creature.header.sdeath);
  DDX_Text(pDX,IDC_WANDS, the_creature.header.swands);
  DDX_Text(pDX,IDC_POLYMORPH, the_creature.header.spoly);
  DDX_Text(pDX,IDC_BREATH, the_creature.header.sbreath);
  DDX_Text(pDX,IDC_SPELLS, the_creature.header.sspell);

  tmp=the_creature.header.resfire;
  DDX_Text(pDX,IDC_FIRE, tmp);
  DDV_MinMaxInt(pDX, tmp, -127,127);
  the_creature.header.resfire=(char) tmp;
  tmp=the_creature.header.rescold;
  DDX_Text(pDX,IDC_COLD, tmp);
  DDV_MinMaxInt(pDX, tmp, -127,127);
  the_creature.header.rescold=(char) tmp;
  tmp=the_creature.header.reselec;
  DDX_Text(pDX,IDC_ELECTRICITY, tmp);
  DDV_MinMaxInt(pDX, tmp, -127,127);
  the_creature.header.reselec=(char) tmp;
  tmp=the_creature.header.resacid;
  DDX_Text(pDX,IDC_ACID, tmp);
  DDV_MinMaxInt(pDX, tmp, -127,127);
  the_creature.header.resacid=(char) tmp;
  tmp=the_creature.header.resmagic;
  DDX_Text(pDX,IDC_MAGIC, tmp);
  DDV_MinMaxInt(pDX, tmp, -127,127);
  the_creature.header.resmagic=(char) tmp;
  tmp=the_creature.header.resmfire;
  DDX_Text(pDX,IDC_MAGFIRE, tmp);
  DDV_MinMaxInt(pDX, tmp, -127,127);
  the_creature.header.resmfire=(char) tmp;
  tmp=the_creature.header.resmcold;
  DDX_Text(pDX,IDC_MAGCOLD, tmp);
  DDV_MinMaxInt(pDX, tmp, -127,127);
  the_creature.header.resmcold=(char) tmp;
  tmp=the_creature.header.resslash;
  DDX_Text(pDX,IDC_SLASHING, tmp);
  DDV_MinMaxInt(pDX, tmp, -127,127);
  the_creature.header.resslash=(char) tmp;
  tmp=the_creature.header.rescrush;
  DDX_Text(pDX,IDC_CRUSHING, tmp);
  DDV_MinMaxInt(pDX, tmp, -127,127);
  the_creature.header.rescrush=(char) tmp;
  tmp=the_creature.header.respierc;
  DDX_Text(pDX,IDC_PIERCING, tmp);
  DDV_MinMaxInt(pDX, tmp, -127,127);
  the_creature.header.respierc=(char) tmp;
  tmp=the_creature.header.resmiss;
  DDX_Text(pDX,IDC_MISSILE, tmp);
  DDV_MinMaxInt(pDX, tmp, -127,127);
  the_creature.header.resmiss=(char) tmp;
  if(memcmp(&tmpheader,&the_creature.header,sizeof(creature_header) ))
  {
    the_creature.m_changed=true;
  }
}

//move itemdata to local variables
void CCreatureResist::RefreshResist()
{
  
}

static int willboxids[5]={IDC_STH1,IDC_STH2,IDC_STH3,IDC_STH4,IDC_STH5};
static const char *willboxtxt[5]={"Fortitude","Reflex","Will","",""};

BOOL CCreatureResist::OnInitDialog() 
{
  CWnd *cw;
  int i;

	CPropertyPage::OnInitDialog();
  if(the_creature.revision==22)
  {
    for(i=0;i<5;i++)
    {
      cw = GetDlgItem(willboxids[i]);
      if(cw)
      {
        cw->SetWindowText(willboxtxt[i]);
      }
    }
  }
	return TRUE;
}

BEGIN_MESSAGE_MAP(CCreatureResist, CPropertyPage)
	//{{AFX_MSG_MAP(CCreatureResist)
	ON_EN_KILLFOCUS(IDC_DEATH, OnKillfocusDeath)
	ON_EN_KILLFOCUS(IDC_WANDS, OnKillfocusWands)
	ON_EN_KILLFOCUS(IDC_POLYMORPH, OnKillfocusPolymorph)
	ON_EN_KILLFOCUS(IDC_BREATH, OnKillfocusBreath)
	ON_EN_KILLFOCUS(IDC_SPELLS, OnKillfocusSpells)
	ON_EN_KILLFOCUS(IDC_FIRE, OnKillfocusFire)
	ON_EN_KILLFOCUS(IDC_COLD, OnKillfocusCold)
	ON_EN_KILLFOCUS(IDC_ELECTRICITY, OnKillfocusElectricity)
	ON_EN_KILLFOCUS(IDC_ACID, OnKillfocusAcid)
	ON_EN_KILLFOCUS(IDC_MAGIC, OnKillfocusMagic)
	ON_EN_KILLFOCUS(IDC_MAGFIRE, OnKillfocusMagfire)
	ON_EN_KILLFOCUS(IDC_MAGCOLD, OnKillfocusMagcold)
	ON_EN_KILLFOCUS(IDC_SLASHING, OnKillfocusSlashing)
	ON_EN_KILLFOCUS(IDC_CRUSHING, OnKillfocusCrushing)
	ON_EN_KILLFOCUS(IDC_PIERCING, OnKillfocusPiercing)
	ON_EN_KILLFOCUS(IDC_MISSILE, OnKillfocusMissile)
	ON_BN_CLICKED(IDC_DEF1, OnDef1)
	ON_BN_CLICKED(IDC_DEF2, OnDef2)
	ON_BN_CLICKED(IDC_DEF3, OnDef3)
	ON_BN_CLICKED(IDC_DEF4, OnDef4)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCreatureResist message handlers
/////////////////////////////////////////////////////////////////////////////

void CCreatureResist::OnKillfocusDeath() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureResist::OnKillfocusWands() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureResist::OnKillfocusPolymorph() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureResist::OnKillfocusBreath() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureResist::OnKillfocusSpells() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureResist::OnKillfocusFire() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureResist::OnKillfocusCold() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureResist::OnKillfocusElectricity() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureResist::OnKillfocusAcid() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureResist::OnKillfocusMagic() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureResist::OnKillfocusMagfire() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureResist::OnKillfocusMagcold() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureResist::OnKillfocusSlashing() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureResist::OnKillfocusCrushing() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureResist::OnKillfocusPiercing() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureResist::OnKillfocusMissile() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureResist::SetDefaultSaves(CString table)
{
  int level;
  int line;

  level=the_creature.header.levels[0];
  if(level<1 || level >40) return;
  for(line=0;line<5;line++)
  {
    ((BYTE *) &the_creature.header.sdeath)[line]=(BYTE) Read2daField(table,line,level);
  }
  UpdateData(UD_DISPLAY);
}

void CCreatureResist::OnDef1() 
{
  SetDefaultSaves("SAVEWAR");
}

void CCreatureResist::OnDef2() 
{
  SetDefaultSaves("SAVEPRS");
}

void CCreatureResist::OnDef3() 
{
  SetDefaultSaves("SAVEROG");
}

void CCreatureResist::OnDef4() 
{
  SetDefaultSaves("SAVEWIZ");
}

/////////////////////////////////////////////////////////////////////////////
// CCreatureStrings dialog

CCreatureStrings::CCreatureStrings():CPropertyPage(CCreatureStrings::IDD)
{
	//{{AFX_DATA_INIT(CCreatureStrings)
	m_text = _T("");
	m_tagged = FALSE;
	m_soundref = _T("");
	m_ref = 0;
	//}}AFX_DATA_INIT
  m_stringnum=0;
}

CCreatureStrings::~CCreatureStrings()
{
}

void CCreatureStrings::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCreatureStrings)
	DDX_Control(pDX, IDC_BUDDY, m_buddycontrol);
	DDX_Control(pDX, IDC_SLOTSPIN, m_spincontrol);
	DDX_Control(pDX, IDC_SLOTPICKER, m_slotpicker_control);
	DDX_Text(pDX, IDC_TEXT, m_text);
	DDX_Check(pDX, IDC_TAGGED, m_tagged);
	DDX_Text(pDX, IDC_SOUNDREF, m_soundref);
	DDV_MaxChars(pDX, m_soundref, 8);
	DDX_Text(pDX, IDC_REF, m_ref);
	DDV_MinMaxInt(pDX, m_ref, -1, 10000000);
	//}}AFX_DATA_MAP
}

void CCreatureStrings::RefreshStrings()
{
  CString tmpstr, tmp;
  int i;

  m_ref=the_creature.header.strrefs[m_stringnum];
  m_text=resolve_tlk_text(m_ref);
  m_tagged=resolve_tlk_tag(m_ref);
  m_soundref=resolve_tlk_soundref(m_ref);

  if(!(editflg&RESOLVE) && IsWindow(m_slotpicker_control.m_hWnd) )
  {
    m_slotpicker_control.ResetContent();
    m_slotpicker_control.InitStorage(100,20);
    for(i=0;i<SND_SLOT_COUNT;i++)
    {
     
      tmp=resolve_tlk_text(the_creature.header.strrefs[i]);
      if(tmp.GetLength()>82) tmp=tmp.Left(80)+"...";
      tmpstr.Format("%3d. %-20s  %s",i,snd_slots[i], tmp );
      m_slotpicker_control.AddString(tmpstr);
    }
    m_slotpicker_control.SetCurSel(m_stringnum);
  }
}

BOOL CCreatureStrings::OnInitDialog() 
{
  CString tmpstr;
  int i;

	CPropertyPage::OnInitDialog();
  RefreshStrings();	

  m_slotpicker_control.ResetContent();
  m_slotpicker_control.InitStorage(100,20);
  for(i=0;i<SND_SLOT_COUNT;i++)
  {
    tmpstr.Format("%3d. %s",i,snd_slots[i]);
    m_slotpicker_control.AddString(tmpstr);
  }
  m_slotpicker_control.SetCurSel(m_stringnum);
  m_spincontrol.SetBuddy(&m_buddycontrol);

  m_spincontrol.SetRange(1,SND_SLOT_COUNT);
  m_spincontrol.SetPos(SND_SLOT_COUNT);
	return TRUE; 
}

BEGIN_MESSAGE_MAP(CCreatureStrings, CPropertyPage)
	//{{AFX_MSG_MAP(CCreatureStrings)
	ON_CBN_KILLFOCUS(IDC_SLOTPICKER, OnKillfocusSlotpicker)
	ON_CBN_SELCHANGE(IDC_SLOTPICKER, OnSelchangeSlotpicker)
	ON_EN_KILLFOCUS(IDC_REF, OnKillfocusRef)
	ON_BN_CLICKED(IDC_TAGGED, OnTagged)
	ON_EN_KILLFOCUS(IDC_SOUNDREF, OnKillfocusSoundref)
	ON_EN_KILLFOCUS(IDC_TEXT, OnKillfocusText)
	ON_BN_CLICKED(IDC_PLAY, OnPlay)
	ON_EN_CHANGE(IDC_BUDDY, OnChangeBuddy)
	ON_BN_CLICKED(IDC_COPY, OnCopy)
	ON_BN_CLICKED(IDC_PASTE, OnPaste)
	ON_BN_CLICKED(IDC_EXPORT, OnExport)
	ON_BN_CLICKED(IDC_IMPORT, OnImport)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCreatureStrings message handlers

void CCreatureStrings::OnKillfocusSlotpicker() 
{
  CString tmpstr;

  UpdateData(UD_RETRIEVE);
  m_slotpicker_control.GetWindowText(tmpstr);
  for(m_stringnum=0;m_stringnum<100;m_stringnum++)
  {
    if(!tmpstr.CompareNoCase(snd_slots[m_stringnum])) break;
  }
  if(m_stringnum==100)
  {
    m_stringnum=strtonum(tmpstr);
  }
  m_stringnum=m_slotpicker_control.SetCurSel(m_stringnum);
  if(m_stringnum>=0)
  {
    tmpstr.Format("%d",SND_SLOT_COUNT-m_stringnum);
    m_buddycontrol.SetWindowText(tmpstr);
  }
  RefreshStrings();
  UpdateData(UD_DISPLAY);
}

void CCreatureStrings::OnSelchangeSlotpicker() 
{
  CString tmpstr;

  m_stringnum=m_slotpicker_control.GetCurSel();
  tmpstr.Format("%d",SND_SLOT_COUNT-m_stringnum);
  m_buddycontrol.SetWindowText(tmpstr);
	RefreshStrings();
  UpdateData(UD_DISPLAY);
}

void CCreatureStrings::OnKillfocusRef() 
{
  UpdateData(UD_RETRIEVE);
  if(the_creature.header.strrefs[m_stringnum]!=m_ref)
  {
    the_creature.header.strrefs[m_stringnum]=m_ref;
    the_creature.m_changed=true;
  }
	RefreshStrings();
  UpdateData(UD_DISPLAY);
}

void CCreatureStrings::OnTagged() 
{
  if((editflg&TLKCHANGE) )
  {
    if(MessageBox("Do you want to update dialog.tlk?","Creature editor",MB_YESNO)!=IDYES)
    {
      return;
    }
  }
  toggle_tlk_tag(m_ref);
  RefreshStrings();
  UpdateData(UD_DISPLAY);
}

void CCreatureStrings::OnKillfocusSoundref() 
{
  CString tmpstr;

  UpdateData(UD_RETRIEVE);
  tmpstr=resolve_tlk_soundref(m_ref);
  if(tmpstr!=m_soundref)
  {
    if(editflg&TLKCHANGE)
    {
      if(MessageBox("Do you want to update dialog.tlk?","Creature editor",MB_YESNO)!=IDYES)
      {
        UpdateData(UD_DISPLAY);
        return;
      }
    }
    m_ref=store_tlk_soundref(m_ref, m_soundref);
  }
  RefreshStrings();
  UpdateData(UD_DISPLAY);
}

void CCreatureStrings::OnKillfocusText() 
{
  CString tmpstr;
  
  UpdateData(UD_RETRIEVE);
  tmpstr=resolve_tlk_text(m_ref);
  if(tmpstr!=m_text)
  {
    if(editflg&TLKCHANGE)
    {
      if(MessageBox("Do you want to update dialog.tlk?","Creature editor",MB_YESNO)!=IDYES)
      {
        UpdateData(UD_DISPLAY);
        return;
      }
    }
    the_creature.header.strrefs[m_stringnum]=store_tlk_text(m_ref,m_text);
  }
  RefreshStrings();
  UpdateData(UD_DISPLAY);
}

void CCreatureStrings::OnPlay() 
{
  play_acm(m_soundref,false,false);
}

void CCreatureStrings::OnChangeBuddy() 
{
  CString tmpstr;

  if(m_buddycontrol)
  {
    m_buddycontrol.GetWindowText(tmpstr);
    m_stringnum=SND_SLOT_COUNT-atoi(tmpstr);
    RefreshStrings();
    UpdateData(UD_DISPLAY);
  }
}

void CCreatureStrings::OnCopy() 
{
	memcpy(creature_strrefs,the_creature.header.strrefs,SND_SLOT_COUNT*sizeof(long) );	
}

void CCreatureStrings::OnPaste() 
{
	memcpy(the_creature.header.strrefs,creature_strrefs,SND_SLOT_COUNT*sizeof(long) );	
  RefreshStrings();
  UpdateData(UD_DISPLAY);
}

void CCreatureStrings::OnExport() 
{
  ExportTBG(this, REF_CRE|TBG_ALT);
}

void CCreatureStrings::OnImport() 
{
  CString tmp;

  tmp=itemname;
	((CChitemDlg *) AfxGetMainWnd())->Importtbg(TBG_ALT);	
  itemname=tmp;
  RefreshStrings();
  UpdateData(UD_DISPLAY);
}

/////////////////////////////////////////////////////////////////////////////
// CCreatureUnknown dialog

CCreatureUnknown::CCreatureUnknown(): CPropertyPage(CCreatureUnknown::IDD)
{
	//{{AFX_DATA_INIT(CCreatureUnknown)
	//}}AFX_DATA_INIT
}

CCreatureUnknown::~CCreatureUnknown()
{
}

void CCreatureUnknown::DoDataExchange(CDataExchange* pDX)
{
  CButton *cb;
  int i;
  creature_header tmpheader;

  memcpy(&tmpheader,&the_creature.header,sizeof(creature_header) );
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCreatureUnknown)
	//}}AFX_DATA_MAP
  DDX_Text(pDX,IDC_LARGE, the_creature.header.unused[0]);
  DDV_MinMaxInt(pDX,the_creature.header.unused[0],0,255);
  DDX_Text(pDX,IDC_SMALL, the_creature.header.unused[1]);
  DDV_MinMaxInt(pDX,the_creature.header.unused[1],0,255);
  DDX_Text(pDX,IDC_BOW, the_creature.header.unused[2]);
  DDV_MinMaxInt(pDX,the_creature.header.unused[2],0,255);
  DDX_Text(pDX,IDC_SPEAR, the_creature.header.unused[3]);
  DDV_MinMaxInt(pDX,the_creature.header.unused[3],0,255);
  DDX_Text(pDX,IDC_BLUNT, the_creature.header.unused[4]);
  DDV_MinMaxInt(pDX,the_creature.header.unused[4],0,255);
  DDX_Text(pDX,IDC_SPIKED, the_creature.header.unused[5]);
  DDV_MinMaxInt(pDX,the_creature.header.unused[5],0,255);
  DDX_Text(pDX,IDC_AXE, the_creature.header.unused[6]);
  DDV_MinMaxInt(pDX,the_creature.header.unused[6],0,255);
  DDX_Text(pDX,IDC_MISSILE, the_creature.header.unused[7]);
  DDV_MinMaxInt(pDX,the_creature.header.unused[7],0,255);
  for(i=0;i<13;i++)
  {
    DDX_Text(pDX,IDC_U1+i, the_creature.header.unused[8+i]);
    DDV_MinMaxInt(pDX,the_creature.header.unused[8+i],0,255);
  }
  DDX_Text(pDX,IDC_U14, the_creature.header.unknown237);
  DDV_MinMaxInt(pDX,the_creature.header.unknown237,0,255);
  DDX_Text(pDX,IDC_U15, the_creature.header.unknown243);
  DDV_MinMaxInt(pDX,the_creature.header.unknown243,0,255);
  for(i=0;i<5;i++)
  {
    DDX_Text(pDX,IDC_U16+i, the_creature.header.idsinternal[i]);
    DDV_MinMaxInt(pDX,the_creature.header.idsinternal[i],0,255);
  }
  for(i=0;i<8;i++)
  {
    DDX_Text(pDX,IDC_U21+i, the_creature.header.unused2[i]);
  }
  DDX_Text(pDX,IDC_U29, the_creature.header.unknown27c);
  DDV_MinMaxInt(pDX,the_creature.header.unknown27c,-32768,32767);
  DDX_Text(pDX,IDC_U30, the_creature.header.unknown27e);
  DDV_MinMaxInt(pDX,the_creature.header.unknown27e,-32768,32767);
  DDX_Text(pDX, IDC_REPUTATION, the_creature.header.reputation);

  cb=(CButton *) GetDlgItem(IDC_TORMENT);
  if(cb) cb->EnableWindow(the_creature.revision==12);

  if(memcmp(&tmpheader,&the_creature.header,sizeof(creature_header) ))
  {
    the_creature.m_changed=true;
  }
}

//move itemdata to local variables
void CCreatureUnknown::RefreshUnknown()
{
}

BOOL CCreatureUnknown::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
  RefreshUnknown();
  
	return TRUE;
}

BEGIN_MESSAGE_MAP(CCreatureUnknown, CPropertyPage)
	//{{AFX_MSG_MAP(CCreatureUnknown)
	ON_EN_KILLFOCUS(IDC_LARGE, OnKillfocusLarge)
	ON_EN_KILLFOCUS(IDC_SMALL, OnKillfocusSmall)
	ON_EN_KILLFOCUS(IDC_BOW, OnKillfocusBow)
	ON_EN_KILLFOCUS(IDC_SPEAR, OnKillfocusSpear)
	ON_EN_KILLFOCUS(IDC_BLUNT, OnKillfocusBlunt)
	ON_EN_KILLFOCUS(IDC_SPIKED, OnKillfocusSpiked)
	ON_EN_KILLFOCUS(IDC_AXE, OnKillfocusAxe)
	ON_EN_KILLFOCUS(IDC_MISSILE, OnKillfocusMissile)
	ON_EN_KILLFOCUS(IDC_U1, OnKillfocusU1)
	ON_EN_KILLFOCUS(IDC_U2, OnKillfocusU2)
	ON_EN_KILLFOCUS(IDC_U3, OnKillfocusU3)
	ON_EN_KILLFOCUS(IDC_U4, OnKillfocusU4)
	ON_EN_KILLFOCUS(IDC_U5, OnKillfocusU5)
	ON_EN_KILLFOCUS(IDC_U6, OnKillfocusU6)
	ON_EN_KILLFOCUS(IDC_U7, OnKillfocusU7)
	ON_EN_KILLFOCUS(IDC_U8, OnKillfocusU8)
	ON_EN_KILLFOCUS(IDC_U9, OnKillfocusU9)
	ON_EN_KILLFOCUS(IDC_U10, OnKillfocusU10)
	ON_EN_KILLFOCUS(IDC_U11, OnKillfocusU11)
	ON_EN_KILLFOCUS(IDC_U12, OnKillfocusU12)
	ON_EN_KILLFOCUS(IDC_U13, OnKillfocusU13)
	ON_EN_KILLFOCUS(IDC_U20, OnKillfocusU20)
	ON_EN_KILLFOCUS(IDC_U21, OnKillfocusU21)
	ON_EN_KILLFOCUS(IDC_U22, OnKillfocusU22)
	ON_EN_KILLFOCUS(IDC_U23, OnKillfocusU23)
	ON_EN_KILLFOCUS(IDC_U24, OnKillfocusU24)
	ON_EN_KILLFOCUS(IDC_U25, OnKillfocusU25)
	ON_EN_KILLFOCUS(IDC_U26, OnKillfocusU26)
	ON_EN_KILLFOCUS(IDC_U27, OnKillfocusU27)
	ON_BN_CLICKED(IDC_CLEARALL, OnClearall)
	ON_EN_KILLFOCUS(IDC_U15, OnKillfocusU15)
	ON_EN_KILLFOCUS(IDC_U14, OnKillfocusU14)
	ON_EN_KILLFOCUS(IDC_U28, OnKillfocusU28)
	ON_EN_KILLFOCUS(IDC_U16, OnKillfocusU16)
	ON_EN_KILLFOCUS(IDC_U17, OnKillfocusU17)
	ON_EN_KILLFOCUS(IDC_U18, OnKillfocusU18)
	ON_EN_KILLFOCUS(IDC_U19, OnKillfocusU19)
	ON_EN_KILLFOCUS(IDC_U29, OnKillfocusU29)
	ON_EN_KILLFOCUS(IDC_U30, OnKillfocusU30)
	ON_EN_KILLFOCUS(IDC_REPUTATION, OnKillfocusReputation)
	ON_BN_CLICKED(IDC_TORMENT, OnTorment)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCreatureUnknown message handlers
/////////////////////////////////////////////////////////////////////////////

void CCreatureUnknown::OnKillfocusLarge() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureUnknown::OnKillfocusSmall() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureUnknown::OnKillfocusBow() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureUnknown::OnKillfocusSpear() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureUnknown::OnKillfocusBlunt() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureUnknown::OnKillfocusSpiked() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureUnknown::OnKillfocusAxe() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureUnknown::OnKillfocusMissile() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureUnknown::OnKillfocusU1() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureUnknown::OnKillfocusU2() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureUnknown::OnKillfocusU3() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureUnknown::OnKillfocusU4() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureUnknown::OnKillfocusU5() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureUnknown::OnKillfocusU6() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureUnknown::OnKillfocusU7() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureUnknown::OnKillfocusU8() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureUnknown::OnKillfocusU9() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureUnknown::OnKillfocusU10() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureUnknown::OnKillfocusU11() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureUnknown::OnKillfocusU12() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureUnknown::OnKillfocusU13() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}


void CCreatureUnknown::OnKillfocusU14() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureUnknown::OnKillfocusU15() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureUnknown::OnKillfocusU16() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureUnknown::OnKillfocusU17() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureUnknown::OnKillfocusU18() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureUnknown::OnKillfocusU19() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureUnknown::OnKillfocusU20() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureUnknown::OnKillfocusU21() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureUnknown::OnKillfocusU22() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureUnknown::OnKillfocusU23() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureUnknown::OnKillfocusU24() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureUnknown::OnKillfocusU25() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureUnknown::OnKillfocusU26() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureUnknown::OnKillfocusU27() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureUnknown::OnKillfocusU28() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureUnknown::OnKillfocusU29() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureUnknown::OnKillfocusU30() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureUnknown::OnKillfocusReputation() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureUnknown::OnTorment() 
{
	CTormentCre dlg;
	
  dlg.DoModal();
}

void CCreatureUnknown::OnClearall() 
{
  memset(the_creature.header.unused,0,sizeof(the_creature.header.unused) );	
  memset(the_creature.header.unused2,0,sizeof(the_creature.header.unused2) );	
  the_creature.header.unknown237=1;
  the_creature.header.unknown243=0;
  memset(the_creature.header.idsinternal,0,sizeof(the_creature.header.idsinternal) );	
  the_creature.header.unknown27c=-1;
  the_creature.header.unknown27e=-1;
  the_creature.header.reputation=0;
  UpdateData(UD_DISPLAY);
}

/////////////////////////////////////////////////////////////////////////////
// CCreatureItem dialog

CCreatureItem::CCreatureItem()	: CPropertyPage(CCreatureItem::IDD)
{
	//{{AFX_DATA_INIT(CCreatureItem)
	m_itemres = _T("");
	m_maxspell = _T("");
	m_maxslot = _T("");
	m_spellres = _T("");
	m_spellres2 = _T("");
	//}}AFX_DATA_INIT
  m_idx=-1;
  m_hb=NULL;
}

CCreatureItem::~CCreatureItem()
{
  if(m_hb) DeleteObject(m_hb);
}

static int itemboxids[]={IDC_USE1,IDC_USE2,IDC_USE3,IDC_FLAGS,
IDC_IDENTIFIED,IDC_NOSTEAL,IDC_STOLEN, IDC_UNDROPPABLE, IDC_UNKNOWN,
0};

static int flagboxids[]={IDC_IDENTIFIED, IDC_NOSTEAL, IDC_STOLEN, IDC_UNDROPPABLE,
0};

static int spellboxids[]={IDC_SPELLPICKER, IDC_MEMORISED, IDC_FORGET,
0};

static int levelboxids[]={
  IDC_LEVELSLOT, IDC_MIN, IDC_MAX, IDC_LEVEL, IDC_SPELLTYPE, IDC_DELSLOT,
  IDC_MEMORISE, IDC_SPELLRES, IDC_BROWSE2,
0};

static int bookboxids[]={IDC_LEVEL2, IDC_SPELLTYPE2, IDC_REMOVE, IDC_BOOK2, IDC_BOOKPICKER,
0};

int CCreatureItem::ResolveTypeAndLevel(CString key)
{
  Cspell tmpspell;
  loc_entry fileloc;
  int fh;
  int pos;

  pos=-1;
  if(spells.Lookup(m_spellres,fileloc) )
  {
    fh=locate_file(fileloc,0);
    pos=tmpspell.RetrieveTypeAndLevel(fh);
  }
  return pos;
}

CString CCreatureItem::ResolveSpellName(CString key)
{
  Cspell tmpspell;
  loc_entry fileloc;
  int fh;
  long strref;

  if(editflg&RESOLVE) return "";
  if(spells.Lookup(key,fileloc) )
  {
    fh=locate_file(fileloc,0);
    strref=tmpspell.RetrieveNameRef(fh);
  }
  else strref=-1;
  return resolve_tlk_text(strref);
}

void CCreatureItem::DoDataExchange(CDataExchange* pDX)
{
  int fc;
  CString tmpstr;
  CButton *cb;
  int pos, pos2;
  int flg, flg2;
  int i,j;

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCreatureItem)
	DDX_Control(pDX, IDC_BOOKPICKER, m_bookpicker);
	DDX_Control(pDX, IDC_SPELLPICKER, m_spellpicker);
	DDX_Control(pDX, IDC_LEVELSLOT, m_levelpicker);
	DDX_Control(pDX, IDC_INVICON, m_invicon);
	DDX_Control(pDX, IDC_SLOTPICKER, m_slotpicker);
	DDX_Text(pDX, IDC_ITEMRES, m_itemres);
	DDV_MaxChars(pDX, m_itemres, 8);
	DDX_Text(pDX, IDC_MAXSPELL, m_maxspell);
	DDX_Text(pDX, IDC_MAXSLOT, m_maxslot);
	DDX_Text(pDX, IDC_SPELLRES, m_spellres);
	DDV_MaxChars(pDX, m_spellres, 8);
	DDX_Text(pDX, IDC_SPELLRES2, m_spellres2);
	DDV_MaxChars(pDX, m_spellres2, 8);
	//}}AFX_DATA_MAP

  //items section
  tmpstr=get_slottype(*(int *) (the_creature.itemslots+the_creature.slotcount));
  DDX_Text(pDX, IDC_SELECTED, tmpstr);
  *(int *) (the_creature.itemslots+the_creature.slotcount)=strtonum(tmpstr); //maybe short ???
  pos=m_slotpicker.GetCurSel();
  flg=(pos>=0) && (pos<the_creature.slotcount);
  if(flg)
  {
    m_idx=the_creature.itemslots[pos];
  }
  else m_idx=-1;
  GetDlgItem(IDC_ITEMRES)->EnableWindow(flg);
  GetDlgItem(IDC_BROWSE)->EnableWindow(flg);
  for(i=0;itemboxids[i];i++)
  {
    GetDlgItem(itemboxids[i])->EnableWindow(m_idx>=0);
  }
  if(m_idx>=0)
  {
    for(i=0;i<3;i++)
    {
      DDX_Text(pDX, IDC_USE1+i, the_creature.items[m_idx].usages[i]);
    }
    DDX_Text(pDX, IDC_FLAGS, the_creature.items[m_idx].flags);
    DDX_Text(pDX, IDC_UNKNOWN, the_creature.items[m_idx].unknown);
    j=1;
    for(i=0;i<8;i++)
    {
      flg=!!(the_creature.items[m_idx].flags&j);
      cb=(CButton *) GetDlgItem(flagboxids[i]);
      if(!cb) break;
      cb->SetCheck(flg);
      j<<=1;
    }
    if(read_item(m_itemres) )
    {
      m_invicon.SetBitmap(0);
    }
    else
    {
      RetrieveResref(tmpstr, the_item.header.invicon);
      if(read_bam(tmpstr)) m_invicon.SetBitmap(0);
      else
      {
        fc=the_bam.GetFrameIndex(1,0);
        the_bam.MakeBitmap(fc,RGB(32,32,32),m_hb,BM_RESIZE,32,32);
        m_invicon.SetBitmap(m_hb);
      }
    }
  }
  else m_invicon.SetBitmap(0);

  //spells section
  pos=m_levelpicker.GetCurSel();
  flg=(pos>=0) && (pos<the_creature.selectcount);
  if(flg)
  {
    DDX_Text(pDX,IDC_MIN, the_creature.selects[pos].num1);
    DDX_Text(pDX,IDC_MAX, the_creature.selects[pos].num2);
    j=the_creature.selects[pos].spelltype;
   	DDX_CBIndex(pDX, IDC_SPELLTYPE, j);
    the_creature.selects[pos].spelltype=(short) j;
    j=the_creature.selects[pos].level+1;
    DDX_Text(pDX, IDC_LEVEL,j);
    //range checks
    /*
    if(iwd2_structures())
    {
      DDV_MinMaxInt(pDX,j,1,9);
    }
    else
    {
      switch(the_creature.selects[pos].spelltype)
      {
      case 0:
        DDV_MinMaxInt(pDX,j,1,7);
        break;
      case 1:
        DDV_MinMaxInt(pDX,j,1,9);
        break;
      default:
        DDV_MinMaxInt(pDX,j,1,1);
      }
    }
    */
    DDV_MinMaxInt(pDX,j,1,9); //simplified range check

    the_creature.selects[pos].level=(short) (j-1);
    pos2=m_spellpicker.GetCurSel();
    flg2=(pos2>=0) && (pos2<the_creature.selects[pos].count);
    if(flg2)
    {
/*
      m_spellslot=pos2+the_creature.selects[pos].index;
      RetrieveResref(tmpstr,the_creature.memos[m_spellslot].resref);
      DDX_Text(pDX, IDC_SPELLRES,tmpstr);
      DDV_MaxChars(pDX, tmpstr, 8);
      StoreResref(tmpstr,the_creature.memos[m_spellslot].resref);
*/

      tmpstr=ResolveSpellName(m_spellres);
      DDX_Text(pDX, IDC_SPELLNAME, tmpstr);

      cb=(CButton *) GetDlgItem(IDC_MEMORISED);
      cb->SetCheck(!!the_creature.memos[m_spellslot].flags);
    }    
  }
  else flg2=0;
  for(i=0;levelboxids[i];i++)
  {
    GetDlgItem(levelboxids[i])->EnableWindow(flg);
  }
  for(i=0;spellboxids[i];i++)
  {
    GetDlgItem(spellboxids[i])->EnableWindow(flg2);
  }
  //books
  pos=m_bookpicker.GetCurSel();
  flg=(pos>=0) && (pos<the_creature.bookcount);
  if(flg)
  {
    /*
    RetrieveResref(tmpstr,the_creature.books[pos].resref);
    DDX_Text(pDX, IDC_SPELLRES2, tmpstr );
    DDV_MaxChars(pDX, tmpstr, 8);
    StoreResref(tmpstr,the_creature.books[pos].resref);
    */

    j= the_creature.books[pos].spelltype;
   	DDX_CBIndex(pDX, IDC_SPELLTYPE2,j);
    the_creature.books[pos].spelltype=(short) j;
    j=the_creature.books[pos].level+1;
    DDX_Text(pDX, IDC_LEVEL2, j);
    //range checks
    if(iwd2_structures())
    {
      DDV_MinMaxInt(pDX,j,1,9);
    }
    else
    {
      switch(the_creature.books[pos].spelltype)
      {
      case 0:
        DDV_MinMaxInt(pDX,j,1,7);
        break;
      case 1:
        DDV_MinMaxInt(pDX,j,1,9);
        break;
      default:
        DDV_MinMaxInt(pDX,j,1,1);
      }
    }
    the_creature.books[pos].level=(short) (j-1);
  }
  for(i=0;bookboxids[i];i++)
  {
    GetDlgItem(bookboxids[i])->EnableWindow(flg);
  }
}

void CCreatureItem::RefreshItem()
{
  int pos;
  CString tmpstr, tmp, tmptext;
  int idx;
  int i;

  if(m_bookpicker)
  {
    pos=m_bookpicker.GetCurSel();
    if(pos<0) pos=0;
    m_bookpicker.ResetContent();
    for(i=0;i<the_creature.bookcount;i++)
    {
      RetrieveResref(tmp,the_creature.books[i].resref);
      tmptext=ResolveSpellName(tmp);
      tmpstr.Format("%d %s %s",i+1,tmp, tmptext);
      m_bookpicker.AddString(tmpstr);
    }
    if(pos>=i) pos=i-1;
    pos=m_bookpicker.SetCurSel(pos);
    if(pos>=0)
    {
      RetrieveResref(m_spellres2,the_creature.books[pos].resref);
    }
    else m_spellres2="";
  }
  if(m_slotpicker)
  {
    pos=m_slotpicker.GetCurSel();
    if(pos<0) pos=0;
    m_slotpicker.ResetContent();
    for(i=0;i<the_creature.slotcount;i++)
    {
      idx=the_creature.itemslots[i];
      if(idx==-1) tmp="<empty>";
      else
      {
        if(the_creature.itemcount>idx && idx>=0)
        {
          RetrieveResref(tmp,the_creature.items[idx].itemname);
        }
        else
        {
          tmp="<invalid>";
        }
      }
      tmpstr.Format("%s %s",slot_names[i],tmp);
      m_slotpicker.AddString(tmpstr);
    }
    if(pos>=i) pos=i-1;
    pos=m_slotpicker.SetCurSel(pos);
    if(pos>=0)
    {
      m_idx=the_creature.itemslots[pos];
      if(m_idx==-1) m_itemres="";
      else
      {
        RetrieveResref(m_itemres, the_creature.items[m_idx].itemname);
      }
    }
  }

  if(m_levelpicker)
  {
    pos=m_levelpicker.GetCurSel();
    if(pos<0) pos=0;
    m_levelpicker.ResetContent();
    m_maxslot.Format("/ %d", the_creature.selectcount);
    for(i=0;i<the_creature.selectcount;i++)
    {
      idx=the_creature.selects[i].level;
      tmpstr.Format("%d %s level %d (%d)",i+1, format_spellslot(the_creature.selects[i].spelltype),idx+1, the_creature.selects[i].count );
      m_levelpicker.AddString(tmpstr);
    }
    if(pos>=i) pos=i-1;
    pos=m_levelpicker.SetCurSel(pos);
    if(pos>=0) //got a valid level
    {
      RefreshSpellPicker(pos);
    }
  }
}

void CCreatureItem::RefreshSpellPicker(int pos)
{
  CString tmpstr, tmp, tmptext;
  int pos2;
  int idx;
  int i;

  if(pos<0) return;
  pos2=m_spellpicker.GetCurSel();
  if(pos2<0) pos2=0;
  m_spellpicker.ResetContent();
  idx=the_creature.selects[pos].index;
  m_maxspell.Format("/ %d", the_creature.selects[pos].count);
  for(i=0;i<the_creature.selects[pos].count;i++)
  {
    RetrieveResref(tmp,the_creature.memos[idx+i].resref);
    tmptext=ResolveSpellName(tmp);
    tmpstr.Format("%d %s %s",i+1, tmp, tmptext);
    m_spellpicker.AddString(tmpstr);
  }
  if(pos2>=i) pos2=i-1;
  pos2=m_spellpicker.SetCurSel(pos2);
  if(pos2<0) m_spellslot=-1;
  else m_spellslot=pos2+the_creature.selects[pos].index;
  if(m_spellslot>=0)
  {
    RetrieveResref(m_spellres,the_creature.memos[m_spellslot].resref);
  }
  else m_spellres="";
}
/* reminder !!!
    switch(revision)
    {
    case 12: slotcount = PST_SLOT_COUNT; break;
    case 22: slotcount = IWD2_SLOT_COUNT; break;
    default: slotcount = SLOT_COUNT; break;
    }
*/
BOOL CCreatureItem::OnInitDialog() 
{
  CComboBox *cb;
  int i;

	CPropertyPage::OnInitDialog();
  cb=(CComboBox *) GetDlgItem(IDC_SELECTED);
  cb->ResetContent();
  for(i=0;i<NUM_SLOTTYPE;i++)
  {
    cb->AddString(slottypes[i]);
  }
	RefreshItem();

	UpdateData(UD_DISPLAY);
	return TRUE;
}

BEGIN_MESSAGE_MAP(CCreatureItem, CPropertyPage)
	//{{AFX_MSG_MAP(CCreatureItem)
	ON_CBN_KILLFOCUS(IDC_SLOTPICKER, OnKillfocusSlotpicker)
	ON_CBN_SELCHANGE(IDC_SLOTPICKER, OnSelchangeSlotpicker)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_EN_KILLFOCUS(IDC_ITEMRES, OnKillfocusItemres)
	ON_EN_KILLFOCUS(IDC_USE1, OnKillfocusUse1)
	ON_EN_KILLFOCUS(IDC_USE2, OnKillfocusUse2)
	ON_EN_KILLFOCUS(IDC_USE3, OnKillfocusUse3)
	ON_EN_KILLFOCUS(IDC_FLAGS, OnKillfocusFlags)
	ON_BN_CLICKED(IDC_IDENTIFIED, OnIdentified)
	ON_BN_CLICKED(IDC_NOSTEAL, OnNosteal)
	ON_BN_CLICKED(IDC_STOLEN, OnStolen)
	ON_EN_KILLFOCUS(IDC_UNKNOWN, OnKillfocusUnknown)
	ON_CBN_KILLFOCUS(IDC_LEVELSLOT, OnKillfocusLevelslot)
	ON_CBN_SELCHANGE(IDC_LEVELSLOT, OnSelchangeLevelslot)
	ON_BN_CLICKED(IDC_CLEAR, OnClear)
	ON_EN_KILLFOCUS(IDC_LEVEL, OnKillfocusLevel)
	ON_EN_KILLFOCUS(IDC_MIN, OnKillfocusMin)
	ON_EN_KILLFOCUS(IDC_MAX, OnKillfocusMax)
	ON_EN_KILLFOCUS(IDC_SPELLRES, OnKillfocusSpellres)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_BROWSE2, OnBrowse2)
	ON_CBN_KILLFOCUS(IDC_SPELLPICKER, OnKillfocusSpellpicker)
	ON_CBN_SELCHANGE(IDC_SPELLPICKER, OnSelchangeSpellpicker)
	ON_CBN_KILLFOCUS(IDC_SPELLTYPE, OnKillfocusSpelltype)
	ON_BN_CLICKED(IDC_DELSLOT, OnDelslot)
	ON_BN_CLICKED(IDC_ADDSLOT, OnAddslot)
	ON_CBN_KILLFOCUS(IDC_BOOKPICKER, OnKillfocusBookpicker)
	ON_CBN_SELCHANGE(IDC_BOOKPICKER, OnSelchangeBookpicker)
	ON_EN_KILLFOCUS(IDC_SPELLRES2, OnKillfocusSpellres2)
	ON_BN_CLICKED(IDC_BROWSE3, OnBrowse3)
	ON_BN_CLICKED(IDC_MEMORISE, OnMemorise)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_CBN_KILLFOCUS(IDC_SPELLTYPE2, OnKillfocusSpelltype2)
	ON_EN_KILLFOCUS(IDC_LEVEL2, OnKillfocusLevel2)
	ON_BN_CLICKED(IDC_BOOK, OnBook)
	ON_BN_CLICKED(IDC_FORGET, OnForget)
	ON_BN_CLICKED(IDC_BOOK2, OnBook2)
	ON_BN_CLICKED(IDC_CLASS, OnClass)
	ON_CBN_KILLFOCUS(IDC_SELECTED, OnKillfocusSelected)
	ON_BN_CLICKED(IDC_UNDROPPABLE, OnUndroppable)
	ON_BN_CLICKED(IDC_CLEARALL, OnClearall)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCreatureItem message handlers

void CCreatureItem::OnKillfocusSlotpicker() 
{	
  UpdateData(UD_RETRIEVE);
  if(m_idx==-1) m_itemres="";
  else
  {
    RetrieveResref(m_itemres, the_creature.items[m_idx].itemname);
  }
	UpdateData(UD_DISPLAY);
}

void CCreatureItem::OnSelchangeSlotpicker() 
{
  int pos;

  pos=m_slotpicker.GetCurSel();
  if(pos>=0)
  {
    m_idx=the_creature.itemslots[pos];
    if(m_idx==-1) m_itemres="";
    else
    {
      RetrieveResref(m_itemres, the_creature.items[m_idx].itemname);
    }
  }
	UpdateData(UD_DISPLAY);
}

void CCreatureItem::OnBrowse() 
{
  pickerdlg.m_restype=REF_ITM;
  pickerdlg.m_picked=m_itemres;
  if(pickerdlg.DoModal()==IDOK)
  {
    m_itemres=pickerdlg.m_picked;
    UpdateData(UD_DISPLAY);
    OnKillfocusItemres();
  }
}

void CCreatureItem::OnKillfocusSelected() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CCreatureItem::OnKillfocusItemres() 
{
  int i;
  int pos;
  creature_item *newitems;

  pos=m_slotpicker.GetCurSel();
  if(pos<0 ) return;
	UpdateData(UD_RETRIEVE);
  if(m_itemres.IsEmpty())
  {
    if(m_idx<0) return;
    newitems=new creature_item[the_creature.itemcount-1];
    if(!newitems) return;
    the_creature.itemcount--;
    the_creature.header.itemcnt--;
    memcpy(newitems, the_creature.items,m_idx*sizeof(creature_item) );
    memcpy(newitems+m_idx, the_creature.items+m_idx+1,(the_creature.itemcount-m_idx)*sizeof(creature_item) );
    if(the_creature.items) delete [] the_creature.items;
    the_creature.items=newitems;
    for(i=0;i<the_creature.slotcount;i++)
    {
      if(the_creature.itemslots[i]==m_idx) the_creature.itemslots[i]=-1;
      else if(the_creature.itemslots[i]>m_idx) the_creature.itemslots[i]--;
    }
    the_creature.itemslots[pos]=-1; 
  }
  else
  {
    if(m_idx==-1)
    {
      newitems=new creature_item[the_creature.itemcount+1];
      if(!newitems) return;
      m_idx=the_creature.itemcount;
      memcpy(newitems, the_creature.items,m_idx*sizeof(creature_item) );
      memset(newitems+m_idx,0,sizeof(creature_item) );
      delete the_creature.items;
      the_creature.items=newitems;
      the_creature.itemcount++;
      the_creature.header.itemcnt++;
    }
    StoreResref(m_itemres, the_creature.items[m_idx].itemname);
    the_creature.itemslots[pos]=(short) m_idx;
  }
  RefreshItem();
	UpdateData(UD_DISPLAY);
}

void CCreatureItem::OnKillfocusUse1() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CCreatureItem::OnKillfocusUse2() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CCreatureItem::OnKillfocusUse3() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CCreatureItem::OnKillfocusFlags() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CCreatureItem::OnIdentified() 
{
  if(m_idx<0) return;
  the_creature.items[m_idx].flags^=STI_IDENTIFIED;
	UpdateData(UD_DISPLAY);
}

void CCreatureItem::OnNosteal() 
{
  if(m_idx<0) return;
  the_creature.items[m_idx].flags^=STI_NOSTEAL;
	UpdateData(UD_DISPLAY);
}

void CCreatureItem::OnStolen() 
{
  if(m_idx<0) return;
  the_creature.items[m_idx].flags^=STI_STOLEN;
	UpdateData(UD_DISPLAY);
}


void CCreatureItem::OnUndroppable() 
{
  if(m_idx<0) return;
  the_creature.items[m_idx].flags^=STI_NODROP;
	UpdateData(UD_DISPLAY);
}

void CCreatureItem::OnKillfocusUnknown() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CCreatureItem::OnKillfocusLevelslot() 
{
	UpdateData(UD_RETRIEVE);
	RefreshSpellPicker(m_levelpicker.GetCurSel());
	UpdateData(UD_DISPLAY);
}

void CCreatureItem::OnSelchangeLevelslot() 
{
	RefreshSpellPicker(m_levelpicker.GetCurSel());
	UpdateData(UD_DISPLAY);
}

void CCreatureItem::OnClearall() 
{
	the_creature.KillItems();
	memset(the_creature.itemslots,-1,sizeof(the_creature.itemslots) );
  *(int *) (the_creature.itemslots+the_creature.slotcount)=1000;
  RefreshItem();
  UpdateData(UD_DISPLAY);
}

#define BG2_SELECTCOUNT  (7+9+1)
void CCreatureItem::OnClear() 
{
  int i;

  the_creature.KillBooks();
  the_creature.KillMemos();
  the_creature.KillSelects();
  the_creature.header.bookcnt=0;
  the_creature.header.memcnt=0;
  the_creature.header.selectcnt=0;
  the_creature.selects=new creature_select[BG2_SELECTCOUNT];
  if(!the_creature.selects) return;
  the_creature.selectcount=the_creature.header.selectcnt=BG2_SELECTCOUNT;
  memset(the_creature.selects,0,BG2_SELECTCOUNT*sizeof(creature_select) );
  for(i=0;i<7;i++)
  {
    the_creature.selects[i].level=(short) i;
  }
  for(i=0;i<9;i++)
  {
    the_creature.selects[i+7].level=(short) i;
    the_creature.selects[i+7].spelltype=1;
  }
  the_creature.selects[7+9].spelltype=2;
  RefreshItem();
	UpdateData(UD_DISPLAY);
}

void CCreatureItem::OnKillfocusLevel() 
{
	UpdateData(UD_RETRIEVE);
  RefreshItem();
	UpdateData(UD_DISPLAY);
}

void CCreatureItem::OnKillfocusSpelltype() 
{
	UpdateData(UD_RETRIEVE);
  RefreshItem();
	UpdateData(UD_DISPLAY);
}

void CCreatureItem::OnKillfocusMin() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CCreatureItem::OnKillfocusMax() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CCreatureItem::OnDelslot() 
{
  creature_select *newselects;
  int pos;

  pos=m_levelpicker.GetCurSel();
  if(pos<0) return;
  m_spellslot=the_creature.selects[pos].index;
  while(the_creature.selects[pos].count)
  {
    RemoveSpell(pos);
  }
  the_creature.selectcount--;
  newselects=new creature_select[the_creature.selectcount];
  if(!newselects)
  {
    the_creature.selectcount++;
    return;
  }
  memcpy(newselects,the_creature.selects,pos*sizeof(creature_select) );
  memcpy(newselects+pos, the_creature.selects+pos+1,(the_creature.selectcount-pos) *sizeof(creature_select) );
  if(the_creature.selects) delete [] the_creature.selects;
  the_creature.selects=newselects;
  the_creature.header.selectcnt=the_creature.selectcount;
  the_creature.m_changed=true;
  RefreshItem();
	UpdateData(UD_DISPLAY);
}

void CCreatureItem::OnAddslot() 
{
  creature_select *newselects;
  int pos;

  pos=m_levelpicker.GetCurSel()+1;
  the_creature.selectcount++;
  newselects=new creature_select[the_creature.selectcount];
  if(!newselects)
  {
    the_creature.selectcount--;
    return;
  }
  memcpy(newselects,the_creature.selects,pos*sizeof(creature_select) );
  memset(newselects+pos,0,sizeof(creature_select) );
  if(pos<7) newselects[pos].level=(short) pos;
  else if(pos<16)
  {
   newselects[pos].level=(short) (pos-7);
   newselects[pos].spelltype=1;
  }
  else newselects[pos].spelltype=2;
  memcpy(newselects+pos+1,the_creature.selects+pos,(the_creature.header.selectcnt-pos)*sizeof(creature_select) );
  if(the_creature.selects) delete [] the_creature.selects;
  the_creature.selects=newselects;
  the_creature.header.selectcnt=the_creature.selectcount;
  the_creature.m_changed=true;
  m_levelpicker.AddString("");
  m_levelpicker.SetCurSel(pos);
  RefreshItem();
	UpdateData(UD_DISPLAY);
}

int CCreatureItem::SetMemoryByClass(CString table, int type, int level)
{
  int column;
  int num, max;
  int ret;
  int i;

  ret=0;
  if(type) max=9;
  else max=7;
  for(column=0;column<max;column++)
  {
    num=Read2daField(table,level,column+1);
    for(i=0;i<the_creature.selectcount;i++)
    {
      if((the_creature.selects[i].level==column) && (the_creature.selects[i].spelltype==type))
      {
        if(the_creature.selects[i].count <= num)
        {
          the_creature.selects[i].num1=the_creature.selects[i].num2=(short) num;
        }
        else ret=1;
      }
    }
  }
  return ret;
}

void CCreatureItem::OnClass() 
{
  int flg;

	switch(the_creature.header.idsclass)
  {
  case MAGE_THIEF:
  case CLASS_MAGE: flg=SetMemoryByClass("MXSPLWIZ",1,the_creature.header.levels[0]); break;
  case CLERIC_THIEF:
  case CLASS_CLERIC: flg=SetMemoryByClass("MXSPLPRS",0,the_creature.header.levels[0]); break;
  case CLASS_BARD: flg=SetMemoryByClass("MXSPLBRD",1,the_creature.header.levels[0]); break;
  case CLASS_PALADIN: flg=SetMemoryByClass("MXSPLPAL",0,the_creature.header.levels[0]); break;
  case CLASS_DRUID: flg=SetMemoryByClass("MXSPLDRU",0,the_creature.header.levels[0]); break;
  case CLASS_RANGER: flg=SetMemoryByClass("MXSPLRAN",0,the_creature.header.levels[0]); break;
  case CLASS_SORCEROR: flg=SetMemoryByClass("SPLSRCKN",1,the_creature.header.levels[0]); break;
  case FIGHTER_MAGE: flg=SetMemoryByClass("MXSPLWIZ",1,the_creature.header.levels[1]); break;
  case FIGHTER_CLERIC: flg=SetMemoryByClass("MXSPLPRS",0,the_creature.header.levels[1]); break;
  case CLERIC_MAGE:
    flg=SetMemoryByClass("MXSPLPRS",0,the_creature.header.levels[0]);
    flg|=SetMemoryByClass("MXSPLWIZ",1,the_creature.header.levels[1]);
    break;
  case CLERIC_RANGER:
    flg=SetMemoryByClass("MXSPLPRS",0,the_creature.header.levels[0]);
    flg|=SetMemoryByClass("MXSPLRAN",0,the_creature.header.levels[1]);
    break;
  case FIGHTER_DRUID: flg=SetMemoryByClass("MXSPLDRU",0,the_creature.header.levels[1]); break;
  case FIGHTER_MAGE_CLERIC:
    flg=SetMemoryByClass("MXSPLWIZ",1,the_creature.header.levels[1]);
    flg|=SetMemoryByClass("MXSPLPRS",0,the_creature.header.levels[2]);
    break;
  default:
    MessageBox("Not spellcaster player class.","Creature editor",MB_OK);
    return;
  }
  if(flg)
  {
    MessageBox("Failed to set spell limits.","Creature editor",MB_OK);
  }
  UpdateData(UD_DISPLAY);
}

void CCreatureItem::OnKillfocusSpellres() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CCreatureItem::RemoveSpell(int slot)
{
  creature_memory *newmemos;
  int i;

  if(!the_creature.memocount) return;
  the_creature.memocount--;
  newmemos=new creature_memory[the_creature.memocount];
  if(!newmemos)
  {
    the_creature.memocount++;
    return;
  }
  memcpy(newmemos,the_creature.memos,m_spellslot*sizeof(creature_memory) );
  memcpy(newmemos+m_spellslot,the_creature.memos+m_spellslot+1,(the_creature.memocount-m_spellslot)*sizeof(creature_memory) );
  if(the_creature.memos) delete [] the_creature.memos;
  the_creature.memos=newmemos;
  the_creature.header.memcnt=the_creature.memocount;

  the_creature.selects[slot].count--;
  for(i=0;i<the_creature.selectcount;i++)
  {
    if((i!=slot) && (the_creature.selects[i].index>m_spellslot) )
    {
      the_creature.selects[i].index--;
    }
  }
  the_creature.m_changed=true;
}

void CCreatureItem::AddSpell(int slot, CString spellres)
{
  creature_memory *newmemos;
  int i;

  if(slot<0 || slot>=the_creature.selectcount) return;
  the_creature.memocount++;
  newmemos=new creature_memory[the_creature.memocount];
  if(!newmemos)
  {
    the_creature.memocount--;
    return;
  }
  m_spellslot=the_creature.selects[slot].index+the_creature.selects[slot].count;
  memcpy(newmemos,the_creature.memos,m_spellslot*sizeof(creature_memory) );
  memcpy(newmemos+m_spellslot+1,the_creature.memos+m_spellslot,(the_creature.header.memcnt-m_spellslot)*sizeof(creature_memory) );
  StoreResref(m_spellres, newmemos[m_spellslot].resref);
  newmemos[m_spellslot].flags=1;
  if(the_creature.memos) delete [] the_creature.memos;
  the_creature.memos=newmemos;
  the_creature.header.memcnt=the_creature.memocount;

  the_creature.selects[slot].count++;
  if(the_creature.selects[slot].num1<the_creature.selects[slot].count)
  {
    the_creature.selects[slot].num1=(short) the_creature.selects[slot].count;
  }
  if(the_creature.selects[slot].num2<the_creature.selects[slot].count)
  {
    the_creature.selects[slot].num2=(short) the_creature.selects[slot].count;
  }
  for(i=0;i<the_creature.selectcount;i++)
  {
    if((i!=slot) && (the_creature.selects[i].index>=m_spellslot) )
    {
      the_creature.selects[i].index++;
    }
  }
  the_creature.m_changed=true;
}

static CString typestr[]={"Priest","Wizard","Innate"};

void CCreatureItem::OnMemorise() 
{
  CString tmpstr;
  int xpos, pos;
  int level, type;

  if(m_spellres.IsEmpty()) return;
  xpos=ResolveTypeAndLevel(m_spellres);
  if(xpos<7) { level=xpos; type=0; }
  else if(xpos<16) { level=xpos-7; type=1; }
  else { level=0; type=2; }
  for(pos=0;pos<the_creature.selectcount;pos++)
  {
    if(level==the_creature.selects[pos].level && type==the_creature.selects[pos].spelltype)
    {
      pos=m_levelpicker.SetCurSel(pos);
      if(pos<0) return;
      AddSpell(pos,m_spellres);
      m_spellpicker.AddString("");
      m_spellpicker.SetCurSel(m_spellpicker.GetCount()-1);
      RefreshItem();
      UpdateData(UD_DISPLAY);
      return;
    }
  }

  tmpstr.Format("No slot available for that spell, it requires %s level %d.",typestr[type], level+1);
  MessageBox(tmpstr,"Creature editor",MB_OK);
}

void CCreatureItem::OnForget() 
{
  int pos;

	m_spellres.Empty();
  for(pos=0;pos<the_creature.selectcount;pos++)
  {
    if( (the_creature.selects[pos].index<=m_spellslot) && 
    the_creature.selects[pos].index+the_creature.selects[pos].count>m_spellslot)
    {
	    RemoveSpell(pos);
    }
  }
  RefreshItem();
	UpdateData(UD_DISPLAY);
}

void CCreatureItem::OnBrowse2() 
{
  pickerdlg.m_restype=REF_SPL;
  pickerdlg.m_picked=m_spellres;
  if(pickerdlg.DoModal()==IDOK)
  {
    m_spellres=pickerdlg.m_picked; 
  }
	UpdateData(UD_DISPLAY);
}

void CCreatureItem::OnKillfocusSpellpicker() 
{
	UpdateData(UD_RETRIEVE);
  RefreshSpellPicker(m_levelpicker.GetCurSel());
 	UpdateData(UD_DISPLAY);
}

void CCreatureItem::OnSelchangeSpellpicker() 
{
  RefreshSpellPicker(m_levelpicker.GetCurSel());
	UpdateData(UD_DISPLAY);
}

// creature book
void CCreatureItem::OnKillfocusBookpicker() 
{
	UpdateData(UD_RETRIEVE);
  RefreshItem();
 	UpdateData(UD_DISPLAY);
}

void CCreatureItem::OnSelchangeBookpicker() 
{
  RefreshItem();
 	UpdateData(UD_DISPLAY);
}

void CCreatureItem::OnKillfocusSpellres2() 
{
	UpdateData(UD_RETRIEVE);
 	UpdateData(UD_DISPLAY);
}

void CCreatureItem::OnBrowse3() 
{
  pickerdlg.m_restype=REF_SPL;
  pickerdlg.m_picked=m_spellres2;
  if(pickerdlg.DoModal()==IDOK)
  {
    m_spellres2=pickerdlg.m_picked;
  }
	UpdateData(UD_DISPLAY);
}

int CCreatureItem::AddBook(int level, int type)
{
  creature_book *newbook;
  CString tmpstr;
  int i;

  for(i=0;i<the_creature.bookcount;i++)
  {
    RetrieveResref(tmpstr,the_creature.books[i].resref);
    if(tmpstr==m_spellres2) return -1;
  }
  the_creature.bookcount++;
  newbook=new creature_book[the_creature.bookcount];
  if(!newbook)
  {
    the_creature.bookcount--;
    return -3;
  }
  memcpy(newbook,the_creature.books,the_creature.header.bookcnt*sizeof(creature_book) );
  StoreResref(m_spellres2,newbook[the_creature.header.bookcnt].resref);
  newbook[the_creature.header.bookcnt].level=(short) level;
  newbook[the_creature.header.bookcnt].spelltype=(short) type;
  if(the_creature.books) delete [] the_creature.books;
  the_creature.books=newbook;
  the_creature.header.bookcnt=the_creature.bookcount;
  the_creature.m_changed=true;
  return 0;
}

void CCreatureItem::OnAdd() 
{
  int level, type;
  int pos;

  if(m_spellres2.IsEmpty()) return;
  pos=ResolveTypeAndLevel(m_spellres2);
  if(pos>=0 && pos<the_creature.selectcount)
  {
    level=the_creature.selects[pos].level;
    type=the_creature.selects[pos].spelltype;
  }
  else
  {
    level=type=0;
  }
  
	if(AddBook(level,type))
  {
    MessageBox("This spell was already in the book.","Creature editor",MB_OK);
  }
  RefreshItem();
	UpdateData(UD_DISPLAY);
}

void CCreatureItem::OnRemove() 
{
	creature_book *newbook;
  CString tmpstr;
  int i;

  for(i=0;i<the_creature.bookcount;i++)
  {
    RetrieveResref(tmpstr,the_creature.books[i].resref);
    if(tmpstr==m_spellres2) break;
  }
	if(i==the_creature.bookcount)
  {
    MessageBox("No such spell in the book.","Creature editor",MB_OK);
    return;
  }
  the_creature.bookcount--;
  newbook=new creature_book[the_creature.bookcount];
  if(!newbook)
  {
    the_creature.bookcount++;
    return;
  }
  memcpy(newbook,the_creature.books,i*sizeof(creature_book) );
  memcpy(newbook+i,the_creature.books+i+1,(the_creature.bookcount-i)*sizeof(creature_book) );
  if(the_creature.books) delete [] the_creature.books;
  the_creature.books=newbook;
  the_creature.header.bookcnt=the_creature.bookcount;
  the_creature.m_changed=true;
	UpdateData(UD_DISPLAY);
}

void CCreatureItem::OnKillfocusSpelltype2() 
{
	UpdateData(UD_RETRIEVE);
 	UpdateData(UD_DISPLAY);
}

void CCreatureItem::OnKillfocusLevel2() 
{
	UpdateData(UD_RETRIEVE);
 	UpdateData(UD_DISPLAY);
}

void CCreatureItem::OnBook() 
{
  int i,j,k;
  int level, type;

  for(i=0;i<the_creature.selectcount;i++)
  {
    level=the_creature.selects[i].level;
    type=the_creature.selects[i].spelltype;
    for(j=0;j<the_creature.selects[i].count;j++)
    {
      k=j+the_creature.selects[i].index;
      RetrieveResref(m_spellres2,the_creature.memos[k].resref);
      AddBook(level,type);
    }
  }
  RefreshItem();
  MessageBox("Added all memorised spells to the book.","Creature editor",MB_OK);
  UpdateData(UD_DISPLAY);
}

void CCreatureItem::OnBook2() 
{
	the_creature.KillBooks();
  the_creature.header.bookcnt=0;
  the_creature.books=new creature_book[0];
  RefreshItem();
  UpdateData(UD_DISPLAY);
}

/////////////////////////////////////////////////////////////////////////////
// CCreatureEffect dialog


CCreatureEffect::CCreatureEffect()	: CPropertyPage(CCreatureEffect::IDD)
{
	//{{AFX_DATA_INIT(CCreatureEffect)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CCreatureEffect::~CCreatureEffect()
{
}

static int radioids2[4]={IDC_V10,IDC_V20};

void CCreatureEffect::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCreatureEffect)
	DDX_Control(pDX, IDC_SLOTPICKER, m_slotpicker_control);
	//}}AFX_DATA_MAP
}

void CCreatureEffect::RefreshEffect()
{
  CString tmpstr;
  int i;
  int id;
  int tmp;
  CButton *cb;

  if(IsWindow(m_slotpicker_control) )
  {
    id=(the_creature.header.effbyte!=0);
    tmp=m_slotpicker_control.GetCurSel();
    m_slotpicker_control.ResetContent();
    for(i=0;i<the_creature.effectcount;i++)
    {
      if(id)
      {
        tmpstr.Format("%d %s %.32s (0x%x 0x%x) %.8s", i+1,
          get_opcode_text(the_creature.effects[i].feature),
          the_creature.effects[i].variable,
          the_creature.effects[i].par1.parl,
          the_creature.effects[i].par2.parl,
          the_creature.effects[i].resource);
      }
      else
      {
        tmpstr.Format("%d %s (0x%x 0x%x) %.8s",i+1,
          get_opcode_text(the_creature.oldeffects[i].feature),
          the_creature.oldeffects[i].par1.parl,
          the_creature.oldeffects[i].par2.parl,
          the_creature.oldeffects[i].vvc);
      }
      m_slotpicker_control.AddString(tmpstr);
    }
    if(tmp<0) tmp=0;
    m_slotpicker_control.SetCurSel(tmp);
    for(i=0;i<2;i++)
    {
      cb=(CButton *) GetDlgItem(radioids2[i]);
      if(cb)
      {
        if(i==id) cb->SetCheck(true);
        else cb->SetCheck(false);
      }
    }
  }
}

BOOL CCreatureEffect::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();	
	RefreshEffect();
	return TRUE;
}

BEGIN_MESSAGE_MAP(CCreatureEffect, CPropertyPage)
	//{{AFX_MSG_MAP(CCreatureEffect)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	ON_LBN_KILLFOCUS(IDC_SLOTPICKER, OnKillfocusSlotpicker)
	ON_BN_CLICKED(IDC_V10, OnV10)
	ON_BN_CLICKED(IDC_V20, OnV20)
	ON_BN_CLICKED(IDC_PASTE, OnPaste)
	ON_LBN_DBLCLK(IDC_SLOTPICKER, OnEdit)
	ON_BN_CLICKED(IDC_COPY, OnCopy)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CCreatureEffect::OnKillfocusSlotpicker() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);	
}

void CCreatureEffect::OnAdd() 
{
	creature_effect *neweffects;
  feat_block *neweffects2;
  int effectcount;

  effectcount=m_slotpicker_control.GetCurSel()+1;
  the_creature.effectcount++;
  if(the_creature.header.effbyte)
  {
    neweffects=new creature_effect[the_creature.effectcount];
    if(!neweffects)
    {
      the_creature.effectcount--;
      MessageBox("Out of memory","Error",MB_ICONSTOP|MB_OK);
      return;
    }
    memcpy(neweffects, the_creature.effects, effectcount*sizeof(creature_effect) );
    memcpy(neweffects+effectcount+1, the_creature.effects+effectcount,(the_creature.header.effectcnt-effectcount)*sizeof(creature_effect) );
    memset(neweffects+effectcount,0,sizeof(creature_effect) );
    neweffects[effectcount].prob2=100;
    neweffects[effectcount].timing=1;
    delete [] the_creature.effects;
    the_creature.effects=neweffects;
  }
  else
  {
    neweffects2=new feat_block[the_creature.effectcount];
    if(!neweffects2)
    {
      the_creature.effectcount--;
      MessageBox("Out of memory","Error",MB_ICONSTOP|MB_OK);
      return;
    }
    memcpy(neweffects2, the_creature.oldeffects, effectcount*sizeof(feat_block) );
    memcpy(neweffects2+effectcount+1, the_creature.oldeffects+effectcount,(the_creature.header.effectcnt-effectcount)*sizeof(feat_block) );
    memset(neweffects2+effectcount,0,sizeof(feat_block) );
    neweffects2[effectcount].prob2=100;
    neweffects2[effectcount].timing=1;
    delete [] the_creature.oldeffects;
    the_creature.oldeffects=neweffects2;
  }
  the_creature.header.effectcnt=the_creature.effectcount;
  the_creature.m_changed=true;
  RefreshEffect();
}

void CCreatureEffect::OnDelete() 
{
	creature_effect *neweffects;
  feat_block *neweffects2;
  int effectcount;

  effectcount=m_slotpicker_control.GetCurSel();
  if(effectcount<0) return;
  the_creature.effectcount--;
  if(the_creature.header.effbyte)
  {
    neweffects=new creature_effect[the_creature.effectcount];
    if(!neweffects)
    {
      the_creature.effectcount++;
      MessageBox("Out of memory","Error",MB_ICONSTOP|MB_OK);
      return;
    }
    memcpy(neweffects, the_creature.effects, effectcount*sizeof(creature_effect) );
    memcpy(neweffects+effectcount, the_creature.effects+effectcount+1,(the_creature.effectcount-effectcount)*sizeof(creature_effect) );
    delete [] the_creature.effects;
    the_creature.effects=neweffects;
  }
  else
  {
    neweffects2=new feat_block[the_creature.effectcount];
    if(!neweffects2)
    {
      the_creature.effectcount++;
      MessageBox("Out of memory","Error",MB_ICONSTOP|MB_OK);
      return;
    }
    memcpy(neweffects2, the_creature.oldeffects, effectcount*sizeof(feat_block) );
    memcpy(neweffects2+effectcount, the_creature.oldeffects+effectcount+1,(the_creature.effectcount-effectcount)*sizeof(feat_block) );
    delete [] the_creature.oldeffects;
    the_creature.oldeffects=neweffects2;
  }
  the_creature.header.effectcnt=the_creature.effectcount;
  the_creature.m_changed=true;
  RefreshEffect();
}

void CCreatureEffect::OnEdit() 
{
  CEffEdit dlg;
  CString tmpname;
  int effectcount;

  effectcount=m_slotpicker_control.GetCurSel();
  if(effectcount<0) return;
  tmpname=itemname;
  itemname.Format("%d of %s", effectcount+1, tmpname);
  if(the_creature.header.effbyte)
  {
    dlg.SetLimitedEffect(0);
  	memcpy(the_effect.header.signature,the_creature.effects[effectcount].signature,sizeof(creature_effect) );
  }
  else
  {
    dlg.SetLimitedEffect(1);
    ConvertToV20Eff((creature_effect *) the_effect.header.signature, the_creature.oldeffects+effectcount);
  }
  dlg.SetDefaultDuration(0);
  dlg.DoModal();
  if(the_creature.header.effbyte)
  {
  	memcpy(the_creature.effects[effectcount].signature,the_effect.header.signature,sizeof(creature_effect) );   
  }
  else
  {
    ConvertToV10Eff((creature_effect *) the_effect.header.signature, the_creature.oldeffects+effectcount);
  }
  itemname=tmpname;
	RefreshEffect();
}

void CCreatureEffect::OnCopy() 
{
  int effectcount;

  effectcount=m_slotpicker_control.GetCurSel();
  if(effectcount<0) return;
  if(the_creature.header.effbyte)
  {
  	memcpy(the_effect.header.signature,the_creature.effects[effectcount].signature,sizeof(creature_effect) );   
  }
  else
  {
    ConvertToV20Eff((creature_effect *) the_effect.header.signature, the_creature.oldeffects+effectcount);
  }
	RefreshEffect();
}

void CCreatureEffect::OnPaste() 
{
  int effectcount;

  effectcount=m_slotpicker_control.GetCurSel();
  if(effectcount<0) return;
  if(the_creature.header.effbyte)
  {
  	memcpy(the_creature.effects[effectcount].signature,the_effect.header.signature,sizeof(creature_effect) );   
  }
  else
  {
    ConvertToV10Eff((creature_effect *) the_effect.header.signature, the_creature.oldeffects+effectcount);
  }
	RefreshEffect();
}

void CCreatureEffect::OnV10() 
{
  feat_block *neweffects;
  int i;

  if(the_creature.header.effbyte)
  {
    neweffects=new feat_block[the_creature.effectcount];
    if(!neweffects)
    {
      MessageBox("Not enough memory.","Error",MB_ICONSTOP|MB_OK);
      return;
    }
    for(i=0;i<the_creature.effectcount;i++)
    {
      ConvertToV10Eff(the_creature.effects+i, neweffects+i);
    }
    delete [] the_creature.effects;
    the_creature.effects=NULL;
    the_creature.oldeffects=neweffects;
    the_creature.header.effbyte=0;	
    RefreshEffect();
    UpdateData(UD_DISPLAY);
  }
}

void CCreatureEffect::OnV20() 
{
  creature_effect *neweffects;
  int i;

  if(!the_creature.header.effbyte)
  {
    neweffects=new creature_effect[the_creature.effectcount];
    if(!neweffects)
    {
      MessageBox("Not enough memory.","Error",MB_ICONSTOP|MB_OK);
      return;
    }
    for(i=0;i<the_creature.effectcount;i++)
    {
      ConvertToV20Eff(neweffects+i,the_creature.oldeffects+i);
    }
    delete [] the_creature.oldeffects;
    the_creature.effects=NULL;
    the_creature.effects=neweffects;
    the_creature.header.effbyte=1;	
    RefreshEffect();
    UpdateData(UD_DISPLAY);
  }
}

//////////////////////////////////////////////////////////
// CCreaturePropertySheet
//////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CCreaturePropertySheet, CPropertySheet)

//ids_aboutbox is a fake ID, the sheet has no caption
CCreaturePropertySheet::CCreaturePropertySheet(CWnd* pWndParent)
: CPropertySheet(IDS_ABOUTBOX, pWndParent)
{
  AddPage(&m_PageGeneral);
  AddPage(&m_PageSkills);
  AddPage(&m_PageResist);
  AddPage(&m_PageIcons);
  AddPage(&m_PageStrings);
  AddPage(&m_PageItem);
  AddPage(&m_PageEffect);
  AddPage(&m_PageUnknown);
}

CCreaturePropertySheet::~CCreaturePropertySheet()
{
}

void CCreaturePropertySheet::RefreshDialog()
{
  CPropertyPage *page;

  m_PageGeneral.RefreshGeneral();
  m_PageIcons.RefreshIcons();
  m_PageSkills.RefreshSkills();
  m_PageResist.RefreshResist();
  m_PageStrings.RefreshStrings();
  m_PageItem.RefreshItem();
  m_PageEffect.RefreshEffect();
  m_PageUnknown.RefreshUnknown();
  page=GetActivePage();
  page->UpdateData(UD_DISPLAY);
}

BEGIN_MESSAGE_MAP(CCreaturePropertySheet, CPropertySheet)
//{{AFX_MSG_MAP(CCreaturePropertySheet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

