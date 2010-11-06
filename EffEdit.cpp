// EffEdit.cpp : implementation file
//

#include "stdafx.h"

#include <fcntl.h>
#include <sys/stat.h>
#include "chitem.h"
#include "chitemDlg.h"
#include "EffEdit.h"
#include "2da.h"
#include "2daEdit.h"
#include "tbg.h"
#include "options.h"
#include "MyFileDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEffEdit dialog

CEffEdit::CEffEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CEffEdit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEffEdit)
	m_durationlabel = _T("");
	m_param1 = _T("");
	m_param2 = _T("");
	m_param1b1 = 0;
	m_param1b2 = 0;
	m_param1b3 = 0;
	m_param1b4 = 0;
	m_param2b1 = 0;
	m_param2b2 = 0;
	m_param2b3 = 0;
	m_param2b4 = 0;
	m_text = _T("");
	m_text2 = _T("");
	//}}AFX_DATA_INIT
  m_par_type=0;
  m_hexadecimal=0;
  the_ids.new_ids();
  m_idsname="";
}

void CEffEdit::DoDataExchange(CDataExchange* pDX)
{
  CString tmpstr;

	CDialog::DoDataExchange(pDX);
  if(editflg&SORTEFF)
  {
    DDX_Control(pDX, IDC_EFFOPCODE2, m_effopcode_control);
  }
  else
  {
    DDX_Control(pDX, IDC_EFFOPCODE, m_effopcode_control);
  }
  m_effopcode_control.ShowWindow(true);
	//{{AFX_DATA_MAP(CEffEdit)
	DDX_Control(pDX, IDC_TEXT2, m_text2_control);
	DDX_Control(pDX, IDC_SECTYPE, m_sectype_control);
	DDX_Control(pDX, IDC_SCHOOL, m_school_control);
	DDX_Control(pDX, IDC_RESIST, m_resist_control);
	DDX_Control(pDX, IDC_TEXT, m_text_control);
	DDX_Control(pDX, IDC_EFFTARGET, m_efftarget_control);
	DDX_Control(pDX, IDC_TIMING, m_timing_control);
	DDX_Control(pDX, IDC_SAVETYPE, m_stype_control);
	DDX_Text(pDX, IDC_DURATIONLABEL, m_durationlabel);
	DDX_Text(pDX, IDC_PAR1, m_param1);
	DDX_Text(pDX, IDC_PAR2, m_param2);
	DDX_Text(pDX, IDC_PAR1B1, m_param1b1);
	DDX_Text(pDX, IDC_PAR1B2, m_param1b2);
	DDX_Text(pDX, IDC_PAR1B3, m_param1b3);
	DDX_Text(pDX, IDC_PAR1B4, m_param1b4);
	DDX_Text(pDX, IDC_PAR2B1, m_param2b1);
	DDX_Text(pDX, IDC_PAR2B2, m_param2b2);
	DDX_Text(pDX, IDC_PAR2B3, m_param2b3);
	DDX_Text(pDX, IDC_PAR2B4, m_param2b4);
	DDX_Text(pDX, IDC_TEXT, m_text);
	DDX_Text(pDX, IDC_TEXT2, m_text2);
	//}}AFX_DATA_MAP

  if(pDX->m_bSaveAndValidate==UD_DISPLAY)
  {
    m_opcode=get_opcode_text(the_effect.header.feature);//display only
  }
  if(editflg&SORTEFF)
  {
    DDX_Text(pDX, IDC_EFFOPCODE2,m_opcode);
  }
  else
  {
    DDX_Text(pDX, IDC_EFFOPCODE,m_opcode);
  }
  
  tmpstr=get_timing_type(the_effect.header.timing);
  DDX_Text(pDX, IDC_TIMING, tmpstr);
  the_effect.header.timing=(unsigned short) strtonum(tmpstr);
 
  DDX_Text(pDX, IDC_UNKNOWN, (short &) the_effect.header.unknown2); //part of timing

  tmpstr=get_resist_type(the_effect.header.resist);
  DDX_Text(pDX, IDC_RESIST, tmpstr);
  the_effect.header.resist=atoi(tmpstr);

  tmpstr=get_efftarget_type(the_effect.header.target);
  if(tmpstr.IsEmpty()) tmpstr.Format("0x%x Unknown",the_effect.header.target);
  DDX_Text(pDX, IDC_EFFTARGET, tmpstr);
  the_effect.header.target=strtonum(tmpstr);
  
  DDX_Text(pDX, IDC_DURATION, the_effect.header.duration);
  
  DDX_Text(pDX, IDC_ROLL2, the_effect.header.count);
  DDX_Text(pDX, IDC_DIE2, the_effect.header.sides);
  DDX_Text(pDX, IDC_POWER, the_effect.header.power);
  
  DDX_Text(pDX, IDC_PROB1,(short &) the_effect.header.prob1);
  DDV_MinMaxInt(pDX, (short &) the_effect.header.prob1, 0 , 100);
  DDX_Text(pDX, IDC_PROB2,(short &) the_effect.header.prob2);
  if (the_effect.header.prob1==100) {
    //gemrb extension, chance by stat
    DDV_MinMaxInt(pDX, (short &) the_effect.header.prob2, 0 , 255);
  } else {
    DDV_MinMaxInt(pDX, (short &) the_effect.header.prob2, 0 , 100);
  }
  
  if (the_effect.header.prob1<100)
  {
    if(the_effect.header.prob2<the_effect.header.prob1)
    {
      the_effect.header.prob2=the_effect.header.prob1;
    }
  }
  UpdateTooltip();

  tmpstr=get_save_type(the_effect.header.stype);
  DDX_Text(pDX, IDC_SAVETYPE, tmpstr);
  the_effect.header.stype=atoi(tmpstr);
  
  DDX_Text(pDX, IDC_SAVEBONUS, the_effect.header.sbonus);
  DDV_MinMaxLong(pDX, the_effect.header.sbonus, -100, 100);
  
  tmpstr=get_par1_label(the_effect.header.feature);
  DDX_Text(pDX, IDC_PARLABEL1, tmpstr);
  
  tmpstr=get_par2_label(the_effect.header.feature);
  DDX_Text(pDX, IDC_PARLABEL2, tmpstr);
  
  tmpstr=get_duration_label(the_effect.header.timing);
  DDX_Text(pDX, IDC_DURATIONLABEL, tmpstr);
  
  tmpstr=get_opcode_desc(the_effect.header.feature, the_effect.header.par2.parl);
  DDX_Text(pDX, IDC_DESCRIPTION, tmpstr);
  
  RetrieveResref(tmpstr, the_effect.header.resource); 
  DDX_Text(pDX, IDC_RESOURCE, tmpstr);
  DDV_MaxChars(pDX, tmpstr, 8);
  StoreResref(tmpstr, the_effect.header.resource); 
  
  RetrieveResref(tmpstr, the_effect.header.vvc);
  DDX_Text(pDX, IDC_VVC, tmpstr);
  DDV_MaxChars(pDX, tmpstr, 8);
  StoreResref(tmpstr, the_effect.header.vvc);

  RetrieveResref(tmpstr, the_effect.header.resource3);
  DDX_Text(pDX, IDC_RESOURCE3, tmpstr);
  DDV_MaxChars(pDX, tmpstr, 8);
  StoreResref(tmpstr, the_effect.header.resource3);
  
  RetrieveVariable(tmpstr, the_effect.header.variable);
  DDX_Text(pDX, IDC_VARNAME, tmpstr);
  DDV_MaxChars(pDX, tmpstr, 32);
  StoreVariable(tmpstr, the_effect.header.variable);

  DDX_Text(pDX, IDC_UNKNOWN2C, the_effect.header.unknown2c);
  /// end of v1.0 effect
  tmpstr=format_schooltype(the_effect.header.school);
  DDX_Text(pDX,IDC_SCHOOL, tmpstr);
  the_effect.header.school=strtonum(tmpstr);

  tmpstr=format_sectype(the_effect.header.sectype);
  DDX_Text(pDX,IDC_SECTYPE, tmpstr);
  the_effect.header.sectype=strtonum(tmpstr);

  RetrieveResref(tmpstr, the_effect.header.source);
  DDX_Text(pDX, IDC_SOURCE, tmpstr);
  DDV_MaxChars(pDX, tmpstr, 8);
  StoreResref(tmpstr, the_effect.header.source);

  DDX_Text(pDX, IDC_PAR3, the_effect.header.par3);
  DDX_Text(pDX, IDC_PAR4, the_effect.header.par4);
  DDX_Text(pDX, IDC_U3, the_effect.header.projectile);
  DDX_Text(pDX, IDC_U4, the_effect.header.casterlevel);
  DDX_Text(pDX, IDC_U5, the_effect.header.slot);

  DDX_Text(pDX, IDC_POS1X, the_effect.header.sx);
  DDX_Text(pDX, IDC_POS1Y, the_effect.header.sy);
  DDX_Text(pDX, IDC_POS2X, the_effect.header.dx);
  DDX_Text(pDX, IDC_POS2Y, the_effect.header.dy);

  DDX_Text(pDX, IDC_MIN, the_effect.header.minlevel);
  DDX_Text(pDX, IDC_MAX, the_effect.header.maxlevel);
}

void CEffEdit::SetDefaultDuration(int arg)
{
  m_defduration=arg;
}

void CEffEdit::SetLimitedEffect(int arg)
{
  m_limitedeffect=arg;
}

void CEffEdit::NewEff()
{
	itemname="new effect";
  memset(&the_effect.header,0,sizeof(eff_header));
  the_effect.header.prob2=100;
}

void CEffEdit::RefreshDialog()
{
  int *strref_opcodes;
  CButton *cb;
  CString tmp;
  CString longformat;
  int strref;

  if(m_hexadecimal) {
    longformat="0x%08x";
  } else {
    longformat="%ld";
  }

  SetWindowText("Edit effect: "+itemname);
  cb=(CButton *) GetDlgItem(IDC_PLAY);
  cb->EnableWindow(feature_resource(the_effect.header.feature)==REF_WAV);
  switch(m_par_type)
  {
  case 0:
    m_param2.Format(longformat,the_effect.header.par2.parl);
    cb=(CButton *) GetDlgItem(IDC_PAR_UNDEFINED);
    break;
  case 1: //colour
    m_param2.Format(longformat,the_effect.header.par2.parl);
    cb=(CButton *) GetDlgItem(IDC_PAR_COLOR);
    break;
  case 2: //ids
    tmp=IDSName2(the_effect.header.par2.parl, true);
    if((unsigned long) the_effect.header.par2.parl <9)
    {
      m_param2.Format("%d-%s",the_effect.header.par2.parl, IDSType(the_effect.header.par2.parl, true) );
    }
    else
    {
      m_param2.Format("%d-Unknown",the_effect.header.par2.parl);
    }
    cb=(CButton *) GetDlgItem(IDC_PAR_IDS);
    break;
  case 3:
    m_param2.Format(longformat+"-%s",the_effect.header.par2.parl,DamageType(the_effect.header.par2.parl)+DamageStyle(the_effect.header.par2.parl));
    cb=(CButton *) GetDlgItem(IDC_PAR_DAMAGE);
    break;
  case 4:
    m_param2.Format(longformat,the_effect.header.par2.parl);
    cb=(CButton *) GetDlgItem(IDC_PAR_SPECIAL);
    break;
  }
  cb->SetCheck(true);
  switch(m_par_type)
  {
  case 1:
    m_param1=colortitle(the_effect.header.par1.parl);
    break;
  case 0:case 3:
    m_param1.Format(longformat,the_effect.header.par1.parl);
    break;
  case 2: //ids
    tmp=IDSToken(tmp, the_effect.header.par1.parl, false);
    if(tmp.IsEmpty()) tmp="unknown";
    m_param1.Format(longformat+"-%s",the_effect.header.par1.parl, tmp);
    break;
  case 4:
    m_param1.Format("0x%04x %s",the_effect.header.par1.parl,IDSToken("ANIMATE",the_effect.header.par1.parl, false) );
    break;
  }
  m_param1b1=the_effect.header.par1.parb[0];
  m_param1b2=the_effect.header.par1.parb[1];
  m_param1b3=the_effect.header.par1.parb[2];
  m_param1b4=the_effect.header.par1.parb[3];

  m_param2b1=the_effect.header.par2.parb[0];
  m_param2b2=the_effect.header.par2.parb[1];
  m_param2b3=the_effect.header.par2.parb[2];
  m_param2b4=the_effect.header.par2.parb[3];

  strref_opcodes=get_strref_opcodes();
  if(member_array(the_effect.header.feature,strref_opcodes)!=-1)
  {
    m_text_control.EnableWindow(true);
    m_text=resolve_tlk_text(the_effect.header.par1.parl);
  }
  else
  {
    m_text_control.EnableWindow(false);
    m_text="N/A";
  }
  if(m_efftext.Lookup(the_effect.header.feature,strref) )
  {
    m_text2_control.EnableWindow(true);
    m_text2=resolve_tlk_text(strref);
  }
  else
  {
    m_text2_control.EnableWindow(false);
    m_text2="N/A";
  }
  UpdateData(UD_DISPLAY);
}

static const int limiteffboxids[]={IDC_VVC, IDC_BROWSE2, IDC_VARNAME,IDC_SCHOOL,
 IDC_PAR3, IDC_PAR4, IDC_U3, IDC_U4, IDC_U5, IDC_UNKNOWN, IDC_SECTYPE, 
 IDC_SOURCE, IDC_RESOURCE3, IDC_BROWSE3, IDC_BROWSE4, IDC_POS1X, IDC_POS1Y,
 IDC_POS2X, IDC_POS2Y, IDC_MIN, IDC_MAX,
0};

void CEffEdit::UpdateTooltip()
{
  CString tmpstr, tmpstr1;
  
  if (the_effect.header.prob1==100) {
    tmpstr1.LoadString(IDS_PROB2B);
    tmpstr.Format("%s%s", tmpstr1, IDSToken("STATS", the_effect.header.prob2, true));
    m_tooltip.AddTool(GetDlgItem(IDC_PROB2), tmpstr);
  } else {
    m_tooltip.AddTool(GetDlgItem(IDC_PROB2), IDS_PROB2);
  }
}

BOOL CEffEdit::OnInitDialog() 
{
  POSITION pos;
  loc_entry tmploc;
  CString tmpstr, tmpstr1, tmpstr2;
  CWnd *cw;
  int i,j;

	CDialog::OnInitDialog();
  darefs.Lookup("EFFTEXT",tmploc);
  Read2daStrref(tmploc,m_efftext,1);
  for(i=0;i<NUM_FEATS;i++)
  {
    j=m_effopcode_control.AddString(get_opcode_text(i));
    m_effopcode_control.SetItemData(j,i);
  }

  for(i=0;i<NUM_RTYPE;i++)
  {
    m_resist_control.AddString(resist_types[i]);
  }

  if(iwd2_structures())
  {
    for(i=0;i<NUM_STYPE2;i++)
    {
      m_stype_control.AddString(save_types2[i]);
    }
  }
  else
  {
    for(i=0;i<NUM_STYPE;i++)
    {
      m_stype_control.AddString(save_types[i]);
    }
  }

  for(i=0;i<NUM_TMTYPE-1;i++)
  {
    m_timing_control.AddString(get_timing_type(i));
  }
  m_timing_control.AddString(get_timing_type(0x1000));

  for(i=0;i<NUM_ETTYPE;i++)
  {
    m_efftarget_control.AddString(get_efftarget_type(i));
  }

  pos=school_names.GetHeadPosition();
  i=0;
  while(pos)
  {
    tmpstr2=school_names.GetNext(pos);
    if(tmpstr2.IsEmpty()) tmpstr2="Unknown";
    tmpstr.Format("0x%0x-%s",i, tmpstr2);
    m_school_control.AddString(tmpstr);
    i++;
  }

  pos=sectype_names.GetHeadPosition();
  i=0;
  while(pos)
  {
    tmpstr2=sectype_names.GetNext(pos);
    if(tmpstr2.IsEmpty()) tmpstr2="Unknown";
    tmpstr.Format("0x%0x-%s",i, tmpstr2);
    m_sectype_control.AddString(tmpstr);
    i++;
  }

  RefreshDialog();
  //tooltips
  {
    m_tooltip.Create(this,TTS_NOPREFIX);
    m_tooltip.SetMaxTipWidth(200);
    m_tooltip.SetTipBkColor(RGB(240,224,160));
    
    m_tooltip.AddTool(GetDlgItem(IDCANCEL), IDS_CANCEL);
    tmpstr1.LoadString(IDS_LOAD);
    tmpstr2.LoadString(IDS_EFFECT);
    tmpstr.Format(tmpstr1, tmpstr2);
    m_tooltip.AddTool(GetDlgItem(IDC_LOAD), tmpstr);
    tmpstr1.LoadString(IDS_LOADEX);
    tmpstr.Format(tmpstr1, tmpstr2);
    m_tooltip.AddTool(GetDlgItem(IDC_LOADEX), tmpstr);
    tmpstr1.LoadString(IDS_SAVE);
    tmpstr.Format(tmpstr1, tmpstr2);
    m_tooltip.AddTool(GetDlgItem(IDC_SAVEAS), tmpstr);
    tmpstr1.LoadString(IDS_NEW);
    tmpstr.Format(tmpstr1, tmpstr2);
    m_tooltip.AddTool(GetDlgItem(IDC_NEW), tmpstr);
    tmpstr1.LoadString(IDS_CHECK);
    tmpstr.Format(tmpstr1, tmpstr2);
    m_tooltip.AddTool(GetDlgItem(IDC_CHECK), tmpstr);

    m_tooltip.AddTool(GetDlgItem(IDC_EFFOPCODE), IDS_FEATURE);
    m_tooltip.AddTool(GetDlgItem(IDC_DESCRIPTION), IDS_DESC);
    m_tooltip.AddTool(GetDlgItem(IDC_EFFTARGET), IDS_TARGET);
    m_tooltip.AddTool(GetDlgItem(IDC_TIMING), IDS_TIMING);
    m_tooltip.AddTool(GetDlgItem(IDC_DURATION), IDS_DURATION);
    m_tooltip.AddTool(GetDlgItem(IDC_TEXT), IDS_TEXT);
    m_tooltip.AddTool(GetDlgItem(IDC_TEXT2), IDS_TEXT2);
    m_tooltip.AddTool(GetDlgItem(IDC_PAR_UNDEFINED), IDS_UNDEFINED);
    m_tooltip.AddTool(GetDlgItem(IDC_PAR_DAMAGE), IDS_PDAMAGE);
    m_tooltip.AddTool(GetDlgItem(IDC_PAR_COLOR), IDS_PCOLOR);
    m_tooltip.AddTool(GetDlgItem(IDC_PAR_IDS), IDS_PIDS);

    tmpstr1.LoadString(IDS_PROB1);
    m_tooltip.AddTool(GetDlgItem(IDC_PROB1), tmpstr1);
    UpdateTooltip();

    m_tooltip.AddTool(GetDlgItem(IDC_UNKNOWN), IDS_UNKNOWN);
    m_tooltip.AddTool(GetDlgItem(IDC_PAR1), IDS_PARAM1);
    m_tooltip.AddTool(GetDlgItem(IDC_PAR1B1), IDS_PARAM1);
    m_tooltip.AddTool(GetDlgItem(IDC_PAR1B2), IDS_PARAM1);
    m_tooltip.AddTool(GetDlgItem(IDC_PAR1B3), IDS_PARAM1);
    m_tooltip.AddTool(GetDlgItem(IDC_PAR1B4), IDS_PARAM1);
    tmpstr.LoadString(IDS_PARAM2);
    m_tooltip.AddTool(GetDlgItem(IDC_PAR2), tmpstr);
    m_tooltip.AddTool(GetDlgItem(IDC_PAR2B1), tmpstr);
    m_tooltip.AddTool(GetDlgItem(IDC_PAR2B2), tmpstr);
    m_tooltip.AddTool(GetDlgItem(IDC_PAR2B3), tmpstr);
    m_tooltip.AddTool(GetDlgItem(IDC_PAR2B4), tmpstr);
    m_tooltip.AddTool(GetDlgItem(IDC_DIE2), IDS_DIE);
    m_tooltip.AddTool(GetDlgItem(IDC_ROLL2), IDS_ROLL);
    m_tooltip.AddTool(GetDlgItem(IDC_POWER), IDS_POWER);
    m_tooltip.AddTool(GetDlgItem(IDC_POWER), IDS_POWER);
    m_tooltip.AddTool(GetDlgItem(IDC_SAVETYPE), IDS_STHROW);
    m_tooltip.AddTool(GetDlgItem(IDC_SAVEBONUS), IDS_SBONUS);
    m_tooltip.AddTool(GetDlgItem(IDC_RESIST), IDS_RESIST);
    m_tooltip.AddTool(GetDlgItem(IDC_RESOURCE), IDS_RESOURCE);
    m_tooltip.AddTool(GetDlgItem(IDC_VVC), IDS_RESOURCE2);
    m_tooltip.AddTool(GetDlgItem(IDC_RESOURCE3), IDS_RESOURCE3);
    m_tooltip.AddTool(GetDlgItem(IDC_SECTYPE), IDS_SECTYPE);
    m_tooltip.AddTool(GetDlgItem(IDC_SCHOOL), IDS_SCHOOL);
    m_tooltip.AddTool(GetDlgItem(IDC_SOURCE), IDS_SOURCE);
    m_tooltip.AddTool(GetDlgItem(IDC_VARNAME), IDS_VARNAME);
    m_tooltip.AddTool(GetDlgItem(IDC_PAR3), IDS_PAR3);
    m_tooltip.AddTool(GetDlgItem(IDC_PAR4), IDS_PAR3);
    m_tooltip.AddTool(GetDlgItem(IDC_UNKNOWN2C), IDS_2C);
    m_tooltip.AddTool(GetDlgItem(IDC_U3), IDS_UNKNOWN);
    m_tooltip.AddTool(GetDlgItem(IDC_U4), IDS_UNKNOWN);
    m_tooltip.AddTool(GetDlgItem(IDC_U5), IDS_UNKNOWN);
  }
  if(m_limitedeffect)
  {
    GetDlgItem(IDC_TYPE)->SetWindowText("Dice/level limit");
    i=0;
    while(limiteffboxids[i])
    {
      cw=GetDlgItem(limiteffboxids[i++]);
      cw->EnableWindow(false);
    }
  }
	return TRUE;
}

BEGIN_MESSAGE_MAP(CEffEdit, CDialog)
	//{{AFX_MSG_MAP(CEffEdit)
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_BN_CLICKED(IDC_LOADEX, OnLoadex)
	ON_BN_CLICKED(IDC_SAVEAS, OnSaveas)
	ON_CBN_KILLFOCUS(IDC_EFFOPCODE, OnKillfocusEffopcode)
	ON_EN_KILLFOCUS(IDC_PAR1, OnKillfocusPar1)
	ON_EN_KILLFOCUS(IDC_PAR2, OnKillfocusPar2)
	ON_CBN_SELCHANGE(IDC_EFFOPCODE, OnSelchangeEffopcode)
	ON_BN_CLICKED(IDC_PAR_UNDEFINED, OnParUndefined)
	ON_BN_CLICKED(IDC_PAR_COLOR, OnParColor)
	ON_BN_CLICKED(IDC_PAR_IDS, OnParIds)
	ON_BN_CLICKED(IDC_PAR_DAMAGE, OnParDamage)
	ON_BN_CLICKED(IDC_EXPLODE1, OnExplode1)
	ON_BN_CLICKED(IDC_EXPLODE2, OnExplode2)
	ON_EN_KILLFOCUS(IDC_PAR1B1, OnKillfocusPar1b1)
	ON_EN_KILLFOCUS(IDC_PAR1B2, OnKillfocusPar1b2)
	ON_EN_KILLFOCUS(IDC_PAR1B3, OnKillfocusPar1b3)
	ON_EN_KILLFOCUS(IDC_PAR1B4, OnKillfocusPar1b4)
	ON_EN_KILLFOCUS(IDC_PAR2B1, OnKillfocusPar2b1)
	ON_EN_KILLFOCUS(IDC_PAR2B2, OnKillfocusPar2b2)
	ON_EN_KILLFOCUS(IDC_PAR2B3, OnKillfocusPar2b3)
	ON_EN_KILLFOCUS(IDC_PAR2B4, OnKillfocusPar2b4)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_BROWSE2, OnBrowse2)
	ON_BN_CLICKED(IDC_NEW, OnNew)
	ON_BN_CLICKED(IDC_CHECK, OnCheck)
	ON_COMMAND(ID_FILE_SAVE, OnSave)
	ON_COMMAND(ID_FILE_TBG, OnFileTbg)
	ON_COMMAND(ID_FILE_TP2, OnFileTp2)
	ON_EN_KILLFOCUS(IDC_TEXT, OnKillfocusText)
	ON_BN_CLICKED(IDC_PLAY, OnPlay)
	ON_BN_CLICKED(IDC_BROWSE3, OnBrowse3)
	ON_BN_CLICKED(IDC_BROWSE4, OnBrowse4)
	ON_COMMAND(ID_TOOLS_DURATION, OnToolsDuration)
	ON_EN_KILLFOCUS(IDC_TEXT2, OnKillfocusText2)
	ON_COMMAND(ID_TOOLS_IDSBROWSER, OnToolsIdsbrowser)
	ON_EN_KILLFOCUS(IDC_PAR4, OnDefaultKillfocus)
	ON_BN_CLICKED(IDC_PAR_SPECIAL, OnParSpecial)
	ON_BN_CLICKED(IDC_HEXADECIMAL, OnHexadecimal)
	ON_CBN_KILLFOCUS(IDC_TIMING, DefaultKillfocus)
	ON_CBN_KILLFOCUS(IDC_EFFTARGET, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_DURATION, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_PROB1, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_PROB2, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_ROLL2, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_DIE2, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_POWER, DefaultKillfocus)
	ON_CBN_KILLFOCUS(IDC_SAVETYPE, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_SAVEBONUS, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_RESOURCE, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_VVC, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_VARNAME, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_UNKNOWN3, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_UNKNOWN2C, DefaultKillfocus)
	ON_CBN_KILLFOCUS(IDC_RESIST, DefaultKillfocus)
	ON_CBN_KILLFOCUS(IDC_SCHOOL, DefaultKillfocus)
	ON_CBN_KILLFOCUS(IDC_SECTYPE, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_SOURCE, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U3, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U4, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U5, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_UNKNOWN, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_PAR3, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_RESOURCE3, DefaultKillfocus)
	ON_COMMAND(ID_CHECK, OnCheck)
	ON_COMMAND(ID_FILE_NEW, OnNew)
	ON_COMMAND(ID_FILE_LOAD, OnLoad)
	ON_COMMAND(ID_FILE_LOADEXTERNALSCRIPT, OnLoadex)
	ON_COMMAND(ID_FILE_SAVEAS, OnSaveas)
	ON_CBN_KILLFOCUS(IDC_EFFOPCODE2, OnKillfocusEffopcode)
	ON_CBN_SELCHANGE(IDC_EFFOPCODE2, OnSelchangeEffopcode)
	ON_EN_KILLFOCUS(IDC_POS1X, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_POS1Y, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_POS2X, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_POS2Y, DefaultKillfocus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEffEdit message handlers

void CEffEdit::OnLoad() 
{
	int res;
	
  pickerdlg.m_restype=REF_EFF;
  pickerdlg.m_picked=itemname;
	res=pickerdlg.DoModal();
	if(res==IDOK)
	{
		res=read_effect(pickerdlg.m_picked);
    switch(res)
    {
    case -3:
      MessageBox("Effect loaded with errors.","Warning",MB_ICONEXCLAMATION|MB_OK);
  		itemname=pickerdlg.m_picked;
      break;
    case 0:
  		itemname=pickerdlg.m_picked;
      break;
    default:
      MessageBox("Cannot read effect!","Error",MB_OK);
      NewEff();
      break;
    }
	}
  RefreshDialog();
}

static char BASED_CODE szFilter[] = "Effect files (*.eff)|*.eff|All files (*.*)|*.*||";

void CEffEdit::OnLoadex() 
{
  CString filepath;
  int fhandle;
  int res;
  
  res=OFN_FILEMUSTEXIST|OFN_ENABLESIZING|OFN_EXPLORER;
  if(readonly) res|=OFN_READONLY;  
  CMyFileDialog m_getfiledlg(TRUE, "eff", makeitemname(".eff",0), res, szFilter);

restart:  
  if( m_getfiledlg.DoModal() == IDOK )
  {
    filepath=m_getfiledlg.GetPathName();
    fhandle=open(filepath, O_RDONLY|O_BINARY);
    if(!fhandle)
    {
      MessageBox("Cannot open file!","Error",MB_ICONEXCLAMATION|MB_OK);
      goto restart;
    }
    readonly=m_getfiledlg.GetReadOnlyPref();
    res=the_effect.ReadEffectFromFile(fhandle,-1);
    close(fhandle);
    switch(res)
    {
    case -3:
      MessageBox("Effect loaded with errors.","Warning",MB_ICONEXCLAMATION|MB_OK);
      itemname=m_getfiledlg.GetFileTitle();
      itemname.MakeUpper();
      break;
    case 0:
      itemname=m_getfiledlg.GetFileTitle();
      itemname.MakeUpper();
      break;
    default:
      MessageBox("Cannot read effect!","Error",MB_ICONEXCLAMATION|MB_OK);
      NewEff();
      break;
    }
  }
  RefreshDialog();
}

void CEffEdit::OnSave() 
{
  SaveEff(1);
}
void CEffEdit::OnSaveas() 
{
  SaveEff(0);
}

void CEffEdit::SaveEff(int save) 
{
  CString filepath;
  CString newname;
  CString tmpstr;
  int res;

  if(readonly)
  {
    MessageBox("You opened it read only!","Warning",MB_ICONEXCLAMATION|MB_OK);
    return;
  }
  res=OFN_HIDEREADONLY|OFN_ENABLESIZING|OFN_EXPLORER;
  CMyFileDialog m_getfiledlg(FALSE, "eff", makeitemname(".eff",0), res, szFilter);

  if(save)
  {
    newname=itemname;
    filepath=makeitemname(".eff",0);
    goto gotname;
  }
restart:  
  if( m_getfiledlg.DoModal() == IDOK )
  {
    filepath=m_getfiledlg.GetPathName();
    filepath.MakeLower();
    if(filepath.Right(4)!=".eff")
    {
      filepath+=".eff";
    }
    newname=m_getfiledlg.GetFileName();
    newname.MakeUpper();
    if(newname.Right(4)==".EFF") newname=newname.Left(newname.GetLength()-4);
gotname:
    if(newname.GetLength()>8 || newname.GetLength()<1 || newname.Find(" ",0)!=-1)
    {
      tmpstr.Format("The resource name '%s' is bad, it should be 8 characters long and without spaces.",newname);
      MessageBox(tmpstr,"Warning",MB_ICONEXCLAMATION|MB_OK);
      goto restart;
    }
    if(newname!=itemname && file_exists(filepath) )
    {
      res=MessageBox("Do you want to overwrite "+newname+"?","Warning",MB_ICONQUESTION|MB_YESNO);
      if(res==IDNO) goto restart;
    }
    
    res = write_effect(newname, filepath);
    lastopenedoverride=filepath.Left(filepath.ReverseFind('\\'));
    switch(res)
    {
    case 0:
      itemname=newname;
      break; //saved successfully
    case -2:
      MessageBox("Error while writing file!","Error",MB_OK);
      break;
    case -3:
      MessageBox("Internal Error!","Error",MB_OK);
      break;
    default:
      MessageBox("Unhandled error!","Error",MB_OK);
    }
  }
  RefreshDialog();
}

void CEffEdit::OnFileTbg() 
{
  ExportTBG(this, REF_EFF,0);
}

void CEffEdit::OnFileTp2() 
{
  ExportTBG(this, REF_EFF,1);
}

void CEffEdit::OnCheck() 
{
  int ret;

	ret=((CChitemDlg *) AfxGetMainWnd())->check_effect();
  if(ret)
  {
    MessageBox(lasterrormsg,"Effect editor",MB_ICONEXCLAMATION|MB_OK);
  }
}

void CEffEdit::OnNew() 
{
  NewEff();
	RefreshDialog();	
}

void CEffEdit::OnKillfocusEffopcode() 
{
  int opcode;

  UpdateData(UD_RETRIEVE);
  opcode=m_effopcode_control.GetCurSel();
  opcode=m_effopcode_control.GetItemData(opcode);
  if(m_opcode==get_opcode_text(opcode)) return;
  opcode=m_effopcode_control.FindStringExact(0,m_opcode);
  if(opcode<0)
  {
    opcode=strtonum(m_opcode);
  }
  else opcode=m_effopcode_control.GetItemData(opcode);
  if(opcode>=0)
  {
    the_effect.header.feature=(short) opcode;
  }
  RefreshDialog();
  UpdateData(UD_DISPLAY);
}

void CEffEdit::OnSelchangeEffopcode() 
{
  int opcode;

  opcode=m_effopcode_control.GetCurSel();
  opcode=m_effopcode_control.GetItemData(opcode);
  if(opcode>=0)
  {
    the_effect.header.feature=(short) opcode;
  }
  RefreshDialog();
  UpdateData(UD_DISPLAY);	
}

void CEffEdit::DefaultKillfocus() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CEffEdit::OnKillfocusPar1() 
{
  UpdateData(UD_RETRIEVE);
  switch(m_par_type) {
  case 4:
    the_effect.header.par1.parl=IDSKey("ANIMATE", m_param1);
    if(the_effect.header.par1.parl!=0xffffffff)
    {
      break;
    }
    //fall through
  default:
    the_effect.header.par1.parl=strtonum(m_param1);
  }
  RefreshDialog();
}

void CEffEdit::OnKillfocusText() 
{
  CString tmpstr;
  
  UpdateData(UD_RETRIEVE);
  tmpstr=resolve_tlk_text(the_effect.header.par1.parl);
  if(tmpstr!=m_text)
  {
    if(editflg&TLKCHANGE)
    {
      if(MessageBox("Do you want to update dialog.tlk?","Effect editor",MB_YESNO)!=IDYES)
      {
        UpdateData(UD_DISPLAY);
        return;
      }
    }
    the_effect.header.par1.parl=store_tlk_text(the_effect.header.par1.parl,m_text);
  }
  RefreshDialog();
}

void CEffEdit::OnKillfocusText2() 
{
  CString tmpstr;
  int strref;

  UpdateData(UD_RETRIEVE);
  if(!m_efftext.Lookup(the_effect.header.feature, strref) ) return;

  tmpstr=resolve_tlk_text(strref);
  if(tmpstr!=m_text2)
  {
    if(editflg&TLKCHANGE)
    {
      if(MessageBox("Do you want to update dialog.tlk?","Effect editor",MB_YESNO)!=IDYES)
      {
        UpdateData(UD_DISPLAY);
        return;
      }
    }
    store_tlk_text(strref,m_text2);
  }
  RefreshDialog();
}

void CEffEdit::OnKillfocusPar2() 
{
  CString tmp;
  int x,y,z;

  UpdateData(UD_RETRIEVE);
  tmp=m_param2;
  tmp.MakeLower();
  switch(m_par_type)
  {
  case 0:
    break;
  case 2: //ids
    for(x=0;x<NUM_IDS;x++)
    {
      if(idstype[x]==tmp)
      {
        the_effect.header.par2.parl=x+2;
        goto endofquest;
      }
    }
    break;
  case 3:
    y=0x10000;
    for(x=0;x<32;x++)
    {
      for(z=0;z<4;z++)
      {
        if(DamageType(y)+DamageStyle(z)==tmp)
        {
          the_effect.header.par2.parl=y|z;
          goto endofquest;
        }
      }
      y<<=1;
    }
    break;
  }
  the_effect.header.par2.parl=strtonum(m_param2);
endofquest:
  RefreshDialog();
}

void CEffEdit::Explode(int *boxids, int parid, int buttonid) 
{
  CButton *cb;
  int flg;
  int i;

  cb=(CButton *) GetDlgItem(buttonid);
  flg=cb->GetCheck();
  GetDlgItem(parid)->ShowWindow(!flg);  
  for(i=0;i<4;i++)
  {
    GetDlgItem(boxids[i])->ShowWindow(flg);
  }
  if(flg) cb->SetWindowText("Implode");
  else cb->SetWindowText("Explode");  
  UpdateData(UD_DISPLAY);
}

static int par1boxids[4]={IDC_PAR1B1, IDC_PAR1B2,IDC_PAR1B3, IDC_PAR1B4};

void CEffEdit::OnExplode1() 
{
  CButton *cb;

  if(m_par_type==1)
  {
    colordlg.m_picked=the_effect.header.par1.parl;
    if(colordlg.DoModal()==IDOK)
    {
      the_effect.header.par1.parl=colordlg.m_picked;
    }
    cb=(CButton *) GetDlgItem(IDC_EXPLODE1);
    cb->SetCheck(false);
    RefreshDialog();
    return;
  }
  Explode(par1boxids,IDC_PAR1,IDC_EXPLODE1);
}

static int par2boxids[4]={IDC_PAR2B1, IDC_PAR2B2,IDC_PAR2B3, IDC_PAR2B4};

void CEffEdit::OnExplode2() 
{
  Explode(par2boxids,IDC_PAR2,IDC_EXPLODE2);
}

void CEffEdit::OnParUndefined() 
{
  Explode(par1boxids,IDC_PAR1,IDC_EXPLODE1);
	m_par_type=0;	
  RefreshDialog();
}

void CEffEdit::OnParColor() 
{
  CButton *cb;
  int i;

  cb=(CButton *) GetDlgItem(IDC_EXPLODE1);
  cb->SetCheck(false);
  GetDlgItem(IDC_PAR1)->ShowWindow(true);  
  for(i=0;i<4;i++)
  {
    GetDlgItem(par1boxids[i])->ShowWindow(false);
  }
  Explode(par1boxids,IDC_PAR1,IDC_EXPLODE1);
  cb->SetWindowText("Pick color");
	m_par_type=1;
  RefreshDialog();
}

void CEffEdit::OnParIds() 
{
  Explode(par1boxids,IDC_PAR1,IDC_EXPLODE1);
	m_par_type=2;
  RefreshDialog();
}

void CEffEdit::OnParDamage() 
{
  Explode(par1boxids,IDC_PAR1,IDC_EXPLODE1);
  m_par_type=3;
  RefreshDialog();
}

void CEffEdit::OnHexadecimal() 
{
  m_hexadecimal = !m_hexadecimal;
	RefreshDialog();
}

void CEffEdit::OnParSpecial() 
{
  Explode(par1boxids,IDC_PAR1,IDC_EXPLODE1);
  m_par_type=4;
  RefreshDialog();
}

void CEffEdit::OnKillfocusPar1b1() 
{
  UpdateData(UD_RETRIEVE);
  the_effect.header.par1.parb[0]=m_param1b1;
  RefreshDialog();
}

void CEffEdit::OnKillfocusPar1b2() 
{
  UpdateData(UD_RETRIEVE);
  the_effect.header.par1.parb[1]=m_param1b2;
  RefreshDialog();
}

void CEffEdit::OnKillfocusPar1b3() 
{
  UpdateData(UD_RETRIEVE);
  the_effect.header.par1.parb[2]=m_param1b3;
  RefreshDialog();
}

void CEffEdit::OnKillfocusPar1b4() 
{
  UpdateData(UD_RETRIEVE);
  the_effect.header.par1.parb[3]=m_param1b4;
  RefreshDialog();
}

void CEffEdit::OnKillfocusPar2b1() 
{
  UpdateData(UD_RETRIEVE);
  the_effect.header.par2.parb[0]=m_param2b1;
  RefreshDialog();
}

void CEffEdit::OnKillfocusPar2b2() 
{
  UpdateData(UD_RETRIEVE);
  the_effect.header.par2.parb[1]=m_param2b2;
  RefreshDialog();
}

void CEffEdit::OnKillfocusPar2b3() 
{
  UpdateData(UD_RETRIEVE);
  the_effect.header.par2.parb[2]=m_param2b3;
  RefreshDialog();
}

void CEffEdit::OnKillfocusPar2b4() 
{
  UpdateData(UD_RETRIEVE);
  the_effect.header.par2.parb[3]=m_param2b4;
  RefreshDialog();
}

void CEffEdit::OnDefaultKillfocus() 
{
	// TODO: Add your control notification handler code here
	
}

void CEffEdit::OnBrowse() 
{
  pickerdlg.m_restype=feature_resource(the_effect.header.feature);
  RetrieveResref(pickerdlg.m_picked,the_effect.header.resource);
  if(pickerdlg.DoModal()==IDOK)
  {    
    StoreResref(pickerdlg.m_picked,the_effect.header.resource);
  }
  UpdateData(UD_DISPLAY);	
}

void CEffEdit::OnPlay() 
{
  CString tmpstr;

  RetrieveResref(tmpstr,the_effect.header.resource);
	play_acm(tmpstr,false,false);	
}

void CEffEdit::OnBrowse2() 
{
  pickerdlg.m_restype=REF_VVC;
  RetrieveResref(pickerdlg.m_picked,the_effect.header.vvc);
  if(pickerdlg.DoModal()==IDOK)
  {    
    StoreResref(pickerdlg.m_picked,the_effect.header.vvc);
  }
  UpdateData(UD_DISPLAY);	
}

void CEffEdit::OnBrowse3() 
{
  pickerdlg.m_restype=REF_SPL;
  RetrieveResref(pickerdlg.m_picked,the_effect.header.source);
  if(pickerdlg.DoModal()==IDOK)
  {    
    StoreResref(pickerdlg.m_picked,the_effect.header.source);
  }
  UpdateData(UD_DISPLAY);	
}

void CEffEdit::OnBrowse4() 
{
  pickerdlg.m_restype=REF_SPL;
  RetrieveResref(pickerdlg.m_picked,the_effect.header.resource3);
  if(pickerdlg.DoModal()==IDOK)
  {    
    StoreResref(pickerdlg.m_picked,the_effect.header.resource3);
  }
  UpdateData(UD_DISPLAY);
}

void CEffEdit::OnToolsDuration() 
{
	the_effect.header.duration=m_defduration;
	UpdateData(UD_DISPLAY);
}

void CEffEdit::OnToolsIdsbrowser() 
{
	CIDSEdit dlg;
  CString tmpstr;

  dlg.SetReadOnly(true);
  tmpstr=itemname;
  itemname=m_idsname;
  dlg.DoModal();
  m_idsname=itemname;
  itemname=tmpstr;
}

BOOL CEffEdit::PreTranslateMessage(MSG* pMsg) 
{
  m_tooltip.RelayEvent(pMsg);
  return CDialog::PreTranslateMessage(pMsg);
}
