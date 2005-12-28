// GameStatistics.cpp : implementation file
//

#include "stdafx.h"
#include "chitem.h"
#include "GameStatistics.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGameStatistics dialog

CGameStatistics::CGameStatistics(CWnd* pParent /*=NULL*/)
	: CDialog(CGameStatistics::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGameStatistics)
	m_text = _T("");
	//}}AFX_DATA_INIT
}

void CGameStatistics::DoDataExchange(CDataExchange* pDX)
{
  CString tmpstr;

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGameStatistics)
	DDX_Control(pDX, IDC_SPELLPICKER, m_spellpicker);
	DDX_Control(pDX, IDC_WEAPON, m_weaponpicker);
	DDX_Text(pDX, IDC_MPVNAME, m_text);
	//}}AFX_DATA_MAP
	int pos1 = m_weaponpicker.GetCurSel();
	if (pos1<0) pos1=0;
	int pos2 = m_spellpicker.GetCurSel();
	if (pos2<0) pos2=0;
  switch(the_game.revision)
  {
  case 12: //pst
    RetrieveVariable(tmpstr,m_stats->gpn.name,true);
    DDX_Text(pDX,IDC_NAME, tmpstr);
    StoreVariable(tmpstr,m_stats->gpn.name,true);
    RetrieveResref(tmpstr,m_stats->gpn.soundset);
    DDX_Text(pDX,IDC_SOUNDREF, tmpstr);
    StoreResref(tmpstr,m_stats->gpn.soundset);
    DDX_Text(pDX,IDC_TALKNUM,m_stats->gpn.talkcount);
    DDX_Text(pDX,IDC_MPVXP,m_stats->gpn.xpofmpv);
    DDX_Text(pDX,IDC_STRREF,m_stats->gpn.nameofmpv);
    DDX_Text(pDX,IDC_KILLXP,m_stats->gpn.totalkillxp);
    DDX_Text(pDX,IDC_KILLNUM,m_stats->gpn.totalkillnum);
    DDX_Text(pDX,IDC_KILLXP2,m_stats->gpn.killxp);
    DDX_Text(pDX,IDC_KILLNUM2,m_stats->gpn.killnum);
		GetDlgItem(IDC_VOICESET)->EnableWindow(false);
    break;
  case 22: //iwd2
    RetrieveVariable(tmpstr,m_stats->gwn.name,true);
    DDX_Text(pDX,IDC_NAME, tmpstr);
    StoreVariable(tmpstr,m_stats->gwn.name,true);
    RetrieveVariable(tmpstr,m_stats->gwn.voiceset);
    DDX_Text(pDX,IDC_VOICESET, tmpstr);
    StoreName(tmpstr,m_stats->gwn.voiceset);
    RetrieveResref(tmpstr,m_stats->gwn.soundset);
    DDX_Text(pDX,IDC_SOUNDREF, tmpstr);
    StoreResref(tmpstr,m_stats->gwn.soundset);
    DDX_Text(pDX,IDC_TALKNUM,m_stats->gwn.talkcount);
    DDX_Text(pDX,IDC_MPVXP,m_stats->gwn.xpofmpv);
    DDX_Text(pDX,IDC_STRREF,m_stats->gwn.nameofmpv);
    DDX_Text(pDX,IDC_KILLXP,m_stats->gwn.totalkillxp);
    DDX_Text(pDX,IDC_KILLNUM,m_stats->gwn.totalkillnum);
    DDX_Text(pDX,IDC_KILLXP2,m_stats->gwn.killxp);
    DDX_Text(pDX,IDC_KILLNUM2,m_stats->gwn.killnum);
    break;
  case 11: //iwd1
    RetrieveVariable(tmpstr,m_stats->gin.name,true);
    DDX_Text(pDX,IDC_NAME, tmpstr);
    StoreVariable(tmpstr,m_stats->gin.name,true);
    RetrieveVariable(tmpstr,m_stats->gin.voiceset);
    DDX_Text(pDX,IDC_VOICESET, tmpstr);
    StoreName(tmpstr,m_stats->gin.voiceset);
    RetrieveResref(tmpstr,m_stats->gin.soundset);
    DDX_Text(pDX,IDC_SOUNDREF, tmpstr);
    StoreResref(tmpstr,m_stats->gin.soundset);
    DDX_Text(pDX,IDC_TALKNUM,m_stats->gin.talkcount);
    DDX_Text(pDX,IDC_MPVXP,m_stats->gin.xpofmpv);
    DDX_Text(pDX,IDC_STRREF,m_stats->gin.nameofmpv);
    DDX_Text(pDX,IDC_KILLXP,m_stats->gin.totalkillxp);
    DDX_Text(pDX,IDC_KILLNUM,m_stats->gin.totalkillnum);
    DDX_Text(pDX,IDC_KILLXP2,m_stats->gin.killxp);
    DDX_Text(pDX,IDC_KILLNUM2,m_stats->gin.killnum);
    break;
  default://bg1, bg2
    RetrieveVariable(tmpstr,m_stats->gbn.name,true);
    DDX_Text(pDX,IDC_NAME, tmpstr);
    StoreVariable(tmpstr,m_stats->gbn.name,true);
    RetrieveResref(tmpstr,m_stats->gbn.soundset);
    DDX_Text(pDX,IDC_SOUNDREF, tmpstr);
    StoreResref(tmpstr,m_stats->gbn.soundset);
    DDX_Text(pDX,IDC_TALKNUM,m_stats->gbn.talkcount);
    DDX_Text(pDX,IDC_MPVXP,m_stats->gbn.xpofmpv);
    DDX_Text(pDX,IDC_STRREF,m_stats->gbn.nameofmpv);
    DDX_Text(pDX,IDC_KILLXP,m_stats->gbn.totalkillxp);
    DDX_Text(pDX,IDC_KILLNUM,m_stats->gbn.totalkillnum);
    DDX_Text(pDX,IDC_KILLXP2,m_stats->gbn.killxp);
    DDX_Text(pDX,IDC_KILLNUM2,m_stats->gbn.killnum);
		GetDlgItem(IDC_VOICESET)->EnableWindow(false);
    break;
  }
	RetrieveResref(tmpstr,m_wstr[pos1]);
	DDX_Text(pDX,IDC_ITEM, tmpstr);
	StoreResref(tmpstr,m_wstr[pos1]);
	DDX_Text(pDX,IDC_COUNT, m_wcnt[pos1]);
	RetrieveResref(tmpstr,m_sstr[pos2]);
	DDX_Text(pDX,IDC_SPELL, tmpstr);
	StoreResref(tmpstr,m_sstr[pos2]);
	DDX_Text(pDX,IDC_COUNT2, m_scnt[pos2]);
}

BEGIN_MESSAGE_MAP(CGameStatistics, CDialog)
	//{{AFX_MSG_MAP(CGameStatistics)
	ON_EN_KILLFOCUS(IDC_TALKNUM, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_STRREF, OnKillfocusStrref)
	ON_CBN_SELCHANGE(IDC_WEAPON, OnSelchangeWeapon)
	ON_CBN_SELCHANGE(IDC_SPELLPICKER, OnSelchangeSpellpicker)
	ON_EN_KILLFOCUS(IDC_ITEM, OnKillfocusPicker)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_EN_KILLFOCUS(IDC_MPVXP, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_KILLNUM, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_KILLNUM2, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_KILLXP, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_KILLXP2, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_NAME, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_SOUNDREF, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_COUNT, OnKillfocusPicker)
	ON_EN_KILLFOCUS(IDC_SPELL, OnKillfocusPicker)
	ON_EN_KILLFOCUS(IDC_COUNT2, OnKillfocusPicker)
	ON_BN_CLICKED(IDC_BROWSE2, OnBrowse2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGameStatistics message handlers

BOOL CGameStatistics::OnInitDialog() 
{
  switch(the_game.revision)
  {
  case 12:
    m_wstr=m_stats->gpn.favweap;
    m_wcnt=m_stats->gpn.favwpcnt;
    m_sstr=m_stats->gpn.favspell;
    m_scnt=m_stats->gpn.favspcnt;
    break;
  case 22:
    m_wstr=m_stats->gwn.favweap;
    m_wcnt=m_stats->gwn.favwpcnt;
    m_sstr=m_stats->gwn.favspell;
    m_scnt=m_stats->gwn.favspcnt;
    break;
  case 11:
    m_wstr=m_stats->gin.favweap;
    m_wcnt=m_stats->gin.favwpcnt;
    m_sstr=m_stats->gin.favspell;
    m_scnt=m_stats->gin.favspcnt;
    break;
  default: //bg1, bg2
    m_wstr=m_stats->gbn.favweap;
    m_wcnt=m_stats->gbn.favwpcnt;
    m_sstr=m_stats->gbn.favspell;
    m_scnt=m_stats->gbn.favspcnt;
    break;
  }
	CDialog::OnInitDialog();
	OnKillfocusStrref();	
  //tooltips
  {
    m_tooltip.Create(this,TTS_NOPREFIX);
    m_tooltip.SetMaxTipWidth(200);
    m_tooltip.SetTipBkColor(RGB(240,224,160));
    
    m_tooltip.AddTool(GetDlgItem(IDCANCEL), IDS_CANCEL);
  }
  RefreshDialog();
	return TRUE;
}

void CGameStatistics::RefreshDialog()
{
  CString tmpstr;
	int pos;
  int i;

  if(!m_weaponpicker) return;
	pos = m_weaponpicker.GetCurSel();
	if (pos<0) pos=0;
  m_weaponpicker.ResetContent();
  for(i=0;i<4;i++)
  {
    tmpstr.Format("%8.8s %d",m_wstr[i], m_wcnt[i]);
    m_weaponpicker.AddString(tmpstr);
  }
  m_weaponpicker.SetCurSel(pos);

	pos = m_spellpicker.GetCurSel();
	if (pos<0) pos=0;
  m_spellpicker.ResetContent();
  for(i=0;i<4;i++)
  {
    tmpstr.Format("%8.8s %d",m_sstr[i], m_scnt[i]);
    m_spellpicker.AddString(tmpstr);
  }
  m_spellpicker.SetCurSel(pos);
}

void CGameStatistics::OnDefaultKillfocus() 
{
  UpdateData(UD_RETRIEVE);	
  UpdateData(UD_DISPLAY);
}

void CGameStatistics::OnKillfocusStrref() 
{
  UpdateData(UD_RETRIEVE);
  switch(the_game.revision)
  {
  case 12:
    m_text=resolve_tlk_text(m_stats->gpn.nameofmpv);
    break;
  case 22:
    m_text=resolve_tlk_text(m_stats->gwn.nameofmpv);
    break;
  case 11:
    m_text=resolve_tlk_text(m_stats->gin.nameofmpv);
    break;
  default:
    m_text=resolve_tlk_text(m_stats->gbn.nameofmpv);
    break;
  }
  UpdateData(UD_DISPLAY);
}

void CGameStatistics::OnSelchangeWeapon() 
{
	UpdateData(UD_DISPLAY);
}

void CGameStatistics::OnSelchangeSpellpicker() 
{
	UpdateData(UD_DISPLAY);
}

void CGameStatistics::OnKillfocusPicker() 
{
  UpdateData(UD_RETRIEVE);
	RefreshDialog();
	UpdateData(UD_DISPLAY);
}

void CGameStatistics::OnBrowse() 
{
  CString tmpstr;
	int pos;
  
	pos = m_weaponpicker.GetCurSel();
	if (pos<0) pos=0;
  pickerdlg.m_restype=REF_ITM;

  RetrieveResref(pickerdlg.m_picked,m_wstr[pos]);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,m_wstr[pos]);
  }
	RefreshDialog();
	UpdateData(UD_DISPLAY);
}

void CGameStatistics::OnBrowse2() 
{
  CString tmpstr;
	int pos;
  
	pos = m_spellpicker.GetCurSel();
	if (pos<0) pos=0;
  pickerdlg.m_restype=REF_SPL;
  RetrieveResref(pickerdlg.m_picked,m_sstr[pos]);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,m_sstr[pos]);
  }
	RefreshDialog();
	UpdateData(UD_DISPLAY);
}

BOOL CGameStatistics::PreTranslateMessage(MSG* pMsg) 
{
  m_tooltip.RelayEvent(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}

