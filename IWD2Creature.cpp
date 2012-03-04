// IWD2Creature.cpp : implementation file
//

#include "stdafx.h"
#include "chitem.h"
#include "IWD2Creature.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// IWD2Creature dialog

//these are the features with a number
#define NUM_FEAT_CNT 26
static int numbered_features[NUM_FEAT_CNT]={
39,//bow
53,//crossbow
55,//sling
38,//axe
54,//mace
40,//flail
44,//polearm
42,//hammer
56,//staff
41,//great sword
43,//large sword
57,//small sword
69,//toughness
4,//armour casting
8,//cleave
3,//armour
60,//enchantment
61,//evocation
62,//necromancy
63,//transmutation
64,//spell penetration
20,//extra rage
21,//extra shape
22,//extra smiting
23,//extra turning
18//bastardsword
};

int is_numbered_feature(int feature)
{
  for(int i=0;i<NUM_FEAT_CNT;i++)
  {
    if (feature==numbered_features[i]) return i;
  }
  return -1;
}

IWD2Creature::IWD2Creature(CWnd* pParent /*=NULL*/)
	: CDialog(IWD2Creature::IDD, pParent)
{
	//{{AFX_DATA_INIT(IWD2Creature)
	m_skill = 0;
	m_count = _T("");
	m_count2 = _T("");
	//}}AFX_DATA_INIT
  m_curskill=-1;
  m_curfeat=-1;
}


void IWD2Creature::DoDataExchange(CDataExchange* pDX)
{
  CString value2;
  CComboBox *cob;
  CEdit *edit;
  CString tmpstr;
  int value;
  int i,j;

	CDialog::DoDataExchange(pDX);
 	//{{AFX_DATA_MAP(IWD2Creature)
	DDX_Control(pDX, IDC_FEATURE, m_feature);
	DDX_Control(pDX, IDC_SKILLPICKER, m_skillpicker);
	DDX_Text(pDX, IDC_SKILL, m_skill);
	DDV_MinMaxInt(pDX, m_skill, 0, 40);
	DDX_Text(pDX, IDC_COUNT, m_count);
	DDX_Text(pDX, IDC_COUNT2, m_count2);
	//}}AFX_DATA_MAP

  DDX_Text(pDX, IDC_UNKNOWN, the_creature.iwd2header.unknown264);
  DDX_Text(pDX, IDC_TRANSPARENT, the_creature.iwd2header.translucency);
  DDX_Text(pDX, IDC_TRANSPARENT2, the_creature.iwd2header.fadespeed);
  DDX_Text(pDX, IDC_VISIBLE, the_creature.iwd2header.visible);
  DDX_Text(pDX, IDC_PICK, the_creature.iwd2header.skillpoints);

  j=1;
  for(i=0;i<4;i++)
  {
    value = !!(the_creature.iwd2header.specflags&j);
	  DDX_Check(pDX, IDC_FLAG5+i, value);
    if (value) {
      the_creature.iwd2header.specflags|=j;
    } else {
      the_creature.iwd2header.specflags&=~j;
    }
    j<<=1;
  }

  for(i=0;i<4;i++)
  {
    value = !!(the_creature.iwd2header.scriptflags[i]);
	  DDX_Check(pDX, IDC_FLAG1+i, value);
    the_creature.iwd2header.scriptflags[i] = (char) value;
  }

  int feat = m_feature.GetCurSel();
  int nf = is_numbered_feature(feat);

  edit = (CEdit *) GetDlgItem(IDC_VALUE2);
  if (nf<0)
  {
    value2="-";
    edit->EnableWindow(false);
  }
  else
  {
    value2.Format("%d", the_creature.iwd2header.feats[nf]);
    edit->EnableWindow(true);
  }
  DDX_Text(pDX, IDC_VALUE2, value2);
  if (nf>=0)
  {
    the_creature.iwd2header.feats[nf]=(unsigned char) atoi(value2);
  }
  
  cob = (CComboBox *) GetDlgItem(IDC_DAMAGE);
  if(cob)
  {
    if (the_creature.iwd2header.dr>=0 && the_creature.iwd2header.dr<=5)
    {
      cob->SetCurSel(the_creature.iwd2header.dr);
    }
    else
    {
      tmpstr.Format("%d/%d", the_creature.iwd2header.dr*5,the_creature.iwd2header.dr);
      cob->SetWindowText(tmpstr);
    }
  }
  RetrieveVariable(tmpstr, the_creature.iwd2header.scriptname, false);
	DDX_Text(pDX, IDC_SCRIPTNAME, tmpstr);
	DDV_MaxChars(pDX, tmpstr, 32);
  StoreVariable(tmpstr, the_creature.iwd2header.scriptname, false);

  RetrieveVariable(tmpstr, the_creature.iwd2header.scriptname2, false);
	DDX_Text(pDX, IDC_SCRIPTNAME2, tmpstr);
	DDV_MaxChars(pDX, tmpstr, 32);
  StoreVariable(tmpstr, the_creature.iwd2header.scriptname2, false);

  for(i=0;i<5;i++)
  {
    DDX_Text(pDX, IDC_U1+i,the_creature.iwd2header.internals[i]);
  }

  for(i=0;i<8;i++)
  {
    tmpstr.Format("0x%x %s",the_creature.iwd2header.enemy[i],IDSToken("RACE",the_creature.iwd2header.enemy[i], true) );
    DDX_Text(pDX, IDC_U10+i, tmpstr);
    value=IDSKey2("RACE", tmpstr);
    the_creature.iwd2header.enemy[i]=(BYTE) value;
  }

  value = (the_creature.header.idsrace<<16)+the_creature.iwd2header.subrace;
  tmpstr.Format("0x%x %s",the_creature.iwd2header.subrace,IDSToken("SUBRACE", value, true) );
  DDX_Text(pDX, IDC_SUBRACE, tmpstr);
  value=IDSKey2("SUBRACE", tmpstr);
  the_creature.iwd2header.subrace=(BYTE) value;

  DDX_Text(pDX, IDC_XPLIST, the_creature.iwd2header.cr);

  i=m_skillpicker.GetCurSel();
  if (i>=0)
  {
    DDX_Text(pDX, IDC_SKILL, the_creature.iwd2header.skills[i]);
  }
}


BEGIN_MESSAGE_MAP(IWD2Creature, CDialog)
	//{{AFX_MSG_MAP(IWD2Creature)
	ON_EN_KILLFOCUS(IDC_SCRIPTNAME, DefaultKillfocus)
	ON_CBN_KILLFOCUS(IDC_SKILLPICKER, OnKillfocusSkillpicker)
	ON_CBN_SELCHANGE(IDC_SKILLPICKER, OnSelchangeSkillpicker)
	ON_EN_KILLFOCUS(IDC_SKILL, OnKillfocusSkill)
	ON_CBN_KILLFOCUS(IDC_FEATURE, OnKillfocusFeature)
	ON_CBN_SELCHANGE(IDC_FEATURE, OnSelchangeFeature)
	ON_BN_CLICKED(IDC_VALUE, OnValue)
	ON_CBN_KILLFOCUS(IDC_DAMAGE, OnKillfocusDamage)
	ON_CBN_SELCHANGE(IDC_DAMAGE, OnSelchangeDamage)
	ON_EN_KILLFOCUS(IDC_VALUE2, OnKillfocusValue2)
	ON_BN_CLICKED(IDC_FLAG1, OnFlag1)
	ON_BN_CLICKED(IDC_FLAG2, OnFlag2)
	ON_BN_CLICKED(IDC_FLAG3, OnFlag3)
	ON_BN_CLICKED(IDC_FLAG4, OnFlag4)
	ON_BN_CLICKED(IDC_FLAG5, OnFlag5)
	ON_BN_CLICKED(IDC_FLAG6, OnFlag6)
	ON_BN_CLICKED(IDC_FLAG7, OnFlag7)
	ON_BN_CLICKED(IDC_FLAG8, OnFlag8)
	ON_EN_KILLFOCUS(IDC_SCRIPTNAME2, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U5, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U4, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U3, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U2, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U1, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_XPLIST, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U10, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U11, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U15, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U14, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U13, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U12, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_SUBRACE, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_UNKNOWN, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_TRANSPARENT, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_VISIBLE, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_PICK, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U17, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U16, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_TRANSPARENT2, DefaultKillfocus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// IWD2Creature message handlers

void IWD2Creature::DefaultKillfocus() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void IWD2Creature::ResetSkills()
{
  m_skillpicker.ResetContent();
  int cnt = 0;
  for(int i=0;i<IWD2_SKILL_COUNT;i++)
  {
    m_skillpicker.AddString(format_skill(i, the_creature.iwd2header.skills[i]));
    cnt+=the_creature.iwd2header.skills[i];
  }
  m_count2.Format("%d points", cnt);
  m_skillpicker.SetCurSel(m_curskill);
}

void IWD2Creature::ResetFeats()
{
  m_feature.ResetContent();
  int cnt = 0;
  for(int i=0;i<192;i++)
  {
    int bit = 1<<(i%32);
    int which = i/32;
    bool feat = !!(the_creature.iwd2header.feat[which]&bit);
    if (feat)
    {
      cnt++;
    }
    m_feature.AddString(format_feat(i, feat) );
    int value2 = is_numbered_feature(i);
    if (value2>=0)
    {
      if (!the_creature.iwd2header.feats[value2])
      {
        the_creature.iwd2header.feats[value2]=1;
      }
      if (feat)
      {
        cnt+=the_creature.iwd2header.feats[value2]-1;
      }
      else
      {
        the_creature.iwd2header.feats[value2]=0;
      }
    }
  }
  m_count.Format("%d points", cnt);
  m_feature.SetCurSel(m_curfeat);
}

BOOL IWD2Creature::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_curskill=0;
  m_curfeat=0;
  ResetSkills();
  ResetFeats();
	OnSelchangeFeature();
  //tooltips
  {
    m_tooltip.Create(this,TTS_NOPREFIX);
    m_tooltip.SetMaxTipWidth(200);
    m_tooltip.SetTipBkColor(RGB(240,224,160));
    
    m_tooltip.AddTool(GetDlgItem(IDCANCEL), IDS_CANCEL);
  }
  UpdateData(UD_DISPLAY);
	return TRUE;
}

void IWD2Creature::OnKillfocusSkillpicker() 
{
  UpdateData(UD_RETRIEVE);
  if(m_curskill==-1) m_skill=0;
  else
  {
    m_skill = the_creature.iwd2header.skills[m_curskill];
  }
	UpdateData(UD_DISPLAY);
}

void IWD2Creature::OnSelchangeSkillpicker() 
{
	UpdateData(UD_DISPLAY);
}

void IWD2Creature::OnKillfocusSkill() 
{
  UpdateData(UD_RETRIEVE);
  m_curskill=m_skillpicker.GetCurSel();
  ResetSkills();

  UpdateData(UD_DISPLAY);
}

void IWD2Creature::OnKillfocusFeature() 
{
  CButton *cb = (CButton *) GetDlgItem(IDC_VALUE);
  int m_curfeat = m_feature.GetCurSel();

  UpdateData(UD_RETRIEVE);

  if(m_curfeat==-1) {
    cb->SetCheck(false);
    cb->EnableWindow(false);
  }
  else
  {
    int bit = 1<<(m_curfeat%32);
    int which = m_curfeat/32;
    cb->SetCheck(!!(the_creature.iwd2header.feat[which]&bit));
  }
	UpdateData(UD_DISPLAY);
}

void IWD2Creature::OnSelchangeFeature() 
{
  CButton *cb = (CButton *) GetDlgItem(IDC_VALUE);
  m_curfeat=m_feature.GetCurSel();

  int bit = 1<<(m_curfeat%32);
  int which = m_curfeat/32;
  int value = the_creature.iwd2header.feat[which]&bit;
  if (value)
  {
    cb->SetCheck(true);
  }
  else
  {
    cb->SetCheck(false);
  }
	UpdateData(UD_DISPLAY);
}

void IWD2Creature::OnValue() 
{
  UpdateData(UD_RETRIEVE);
  CButton *cb = (CButton *) GetDlgItem(IDC_VALUE);

  int bit = 1<<(m_curfeat%32);
  int which = m_curfeat/32;
  int value =cb->GetCheck();
  if (value)
  {
    the_creature.iwd2header.feat[which]|=bit;
  }
  else
  {
    the_creature.iwd2header.feat[which]&=~bit;
  }
  ResetFeats();
  UpdateData(UD_DISPLAY);
}

void IWD2Creature::OnKillfocusDamage() 
{
  CString tmpstr;
  CComboBox *cob= (CComboBox *) GetDlgItem(IDC_DAMAGE);
  
  cob->GetWindowText(tmpstr);
	the_creature.iwd2header.dr=atoi(tmpstr)/5;
  UpdateData(UD_DISPLAY);
}

void IWD2Creature::OnSelchangeDamage() 
{
  CComboBox *cob= (CComboBox *) GetDlgItem(IDC_DAMAGE);
	the_creature.iwd2header.dr=cob->GetCurSel();
  UpdateData(UD_DISPLAY);
}

void IWD2Creature::OnKillfocusValue2() 
{
  CString tmpstr;

  CEdit *edit = (CEdit *) GetDlgItem(IDC_VALUE2);
  CButton *cb = (CButton *) GetDlgItem(IDC_VALUE);
  UpdateData(UD_RETRIEVE);
  edit->GetWindowText(tmpstr);
  int value2 = atoi(tmpstr);
  if (value2)
  {
    cb->SetCheck(true);
  }
  else
  {
    cb->SetCheck(false);
  }
	OnValue();
}

void IWD2Creature::OnFlag1() 
{
	the_creature.iwd2header.scriptflags[0]=!the_creature.iwd2header.scriptflags[0];
}

void IWD2Creature::OnFlag2() 
{
	the_creature.iwd2header.scriptflags[1]=!the_creature.iwd2header.scriptflags[1];
}

void IWD2Creature::OnFlag3() 
{
	the_creature.iwd2header.scriptflags[2]=!the_creature.iwd2header.scriptflags[2];
}

void IWD2Creature::OnFlag4() 
{
	the_creature.iwd2header.scriptflags[3]=!the_creature.iwd2header.scriptflags[3];
}

void IWD2Creature::OnFlag5() 
{
	the_creature.iwd2header.specflags^=1;
}

void IWD2Creature::OnFlag6() 
{
	the_creature.iwd2header.specflags^=2;
}

void IWD2Creature::OnFlag7() 
{
	the_creature.iwd2header.specflags^=4;
}

void IWD2Creature::OnFlag8() 
{
	the_creature.iwd2header.specflags^=8;
}

BOOL IWD2Creature::PreTranslateMessage(MSG* pMsg) 
{
	m_tooltip.RelayEvent(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}
