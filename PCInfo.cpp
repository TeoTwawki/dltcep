// PCInfo.cpp : implementation file
//

#include "stdafx.h"
#include "chitem.h"
#include "PCInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPCInfo dialog


CPCInfo::CPCInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CPCInfo::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPCInfo)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CPCInfo::DoDataExchange(CDataExchange* pDX)
{
	CWnd *dw;
  CButton *db;
  CString tmpstr;
	int i;

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPCInfo)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	RetrieveResref(tmpstr,m_data->curarea);
	DDX_Text(pDX,IDC_AREA, tmpstr);
	StoreResref(tmpstr,m_data->curarea);
	DDX_Text(pDX,IDC_XPOS, m_data->xpos);
	DDX_Text(pDX,IDC_YPOS, m_data->ypos);
	RetrieveResref(tmpstr,m_data->creresref);
	DDX_Text(pDX,IDC_CRERES, tmpstr);
	StoreResref(tmpstr,m_data->creresref);
  tmpstr=format_direction(m_data->direction);
  DDX_Text(pDX, IDC_FACE,tmpstr);
  m_data->direction=(short) strtonum(tmpstr);
  
  DDX_Text(pDX, IDC_HAPPINESS, m_data->happiness);
  DDX_Text(pDX, IDC_ACTION, m_data->action);
  DDX_Text(pDX, IDC_SELECTED, m_data->selected);

  if (the_game.revision==12)
  {
    for(i=0;i<5;i++)
    {
      DDX_Text(pDX,IDC_U11+i,quickitems[i]);
      db = (CButton *) GetDlgItem(IDC_U16+i);
      db->SetCheck(quickslots[i]==0);
    }
  }
  else
  {
    for(i=0;i<3;i++)
    {
      DDX_Text(pDX,IDC_U11+i,quickitems[i]);
      db = (CButton *) GetDlgItem(IDC_U16+i);
      db->SetCheck(quickslots[i]==0);
    }
    for(i=3;i<5;i++)
    {
      dw = GetDlgItem(IDC_U11+i);
      dw->EnableWindow(false);
      dw = GetDlgItem(IDC_U16+i);
      dw->EnableWindow(false);
    }
  }
  if (the_game.revision==22)
  {
    for(i=0;i<9;i++)
    {
      RetrieveResref(tmpstr,quickspells[i]);
      DDX_Text(pDX,IDC_SPELL1+i, tmpstr);
      StoreResref(tmpstr,quickspells[i]);	
    }
    for(i=0;i<9;i++)
    {
      DDX_Text(pDX, IDC_U1+i, m_stat->gwn.qslots[i]);
    }
    for(i=0;i<8;i++)
    {
      DDX_Text(pDX,IDC_FLAG1+i,quickweapons[i]);
      db = (CButton *) GetDlgItem(IDC_FLAG11+i);
      db->SetCheck(quickwslots[i]==0);
    }
  }
  else
  {
    //going with the bg struct as it is the same
    for(i=0;i<3;i++)
    {
      RetrieveResref(tmpstr,quickspells[i]);
      DDX_Text(pDX,IDC_SPELL1+i, tmpstr);
      StoreResref(tmpstr,quickspells[i]);	
    }
    for(i=3;i<9;i++)
    {
      dw = GetDlgItem(IDC_SPELL1+i);
      dw->EnableWindow(false);
      dw = GetDlgItem(IDC_BROWSE3+i);
      dw->EnableWindow(false);
    }
    for(i=0;i<9;i++)
    {
      dw = GetDlgItem(IDC_U1+i);
      dw->EnableWindow(false);
    }
    for(i=0;i<4;i++)
    {
      DDX_Text(pDX,IDC_FLAG1+i,quickweapons[i]);
      db = (CButton *) GetDlgItem(IDC_FLAG11+i);
      db->SetCheck(quickwslots[i]==0);
    }
    for(i=0;i<4;i++)
    {
      dw = GetDlgItem(IDC_FLAG5+i);
      dw->EnableWindow(false);
      dw = GetDlgItem(IDC_FLAG15+i);
      dw->EnableWindow(false);
    }
  }
}


BEGIN_MESSAGE_MAP(CPCInfo, CDialog)
	//{{AFX_MSG_MAP(CPCInfo)
	ON_CBN_KILLFOCUS(IDC_FACE, OnDefaultKillfocus)
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
	ON_BN_CLICKED(IDC_FLAG11, OnFlag11)
	ON_BN_CLICKED(IDC_FLAG12, OnFlag12)
	ON_BN_CLICKED(IDC_FLAG13, OnFlag13)
	ON_BN_CLICKED(IDC_FLAG14, OnFlag14)
	ON_BN_CLICKED(IDC_FLAG15, OnFlag15)
	ON_BN_CLICKED(IDC_FLAG16, OnFlag16)
	ON_BN_CLICKED(IDC_FLAG17, OnFlag17)
	ON_BN_CLICKED(IDC_FLAG18, OnFlag18)
	ON_BN_CLICKED(IDC_U16, OnU16)
	ON_BN_CLICKED(IDC_U17, OnU17)
	ON_BN_CLICKED(IDC_U18, OnU18)
	ON_BN_CLICKED(IDC_U19, OnU19)
	ON_BN_CLICKED(IDC_U20, OnU20)
	ON_EN_KILLFOCUS(IDC_XPOS, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_YPOS, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_AREA, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_CRERES, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_HAPPINESS, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_ACTION, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_SPELL1, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_SPELL2, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_SPELL3, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_SELECTED, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U1, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U2, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U3, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U4, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U5, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U6, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U7, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U8, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U9, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_FLAG1, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_FLAG2, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_FLAG8, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_FLAG7, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_FLAG6, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_FLAG5, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_FLAG4, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_FLAG3, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_SPELL4, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_SPELL5, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_SPELL6, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_SPELL7, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_SPELL8, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_SPELL9, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U11, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U12, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U13, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U14, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U15, OnDefaultKillfocus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPCInfo message handlers

BOOL CPCInfo::OnInitDialog() 
{
	CComboBox *cb;
	int i;

  switch (the_game.revision)
  {
  case 22:
    quickspells = m_stat->gwn.quickspells;
    quickitems = m_stat->gwn.quickitems;
    quickslots = m_stat->gwn.quickslots;
    quickweapons = m_stat->gwn.weapons;
    quickwslots = m_stat->gwn.wslots;
    break;
  case 12:
    quickspells = m_stat->gpn.quickspells;
    quickitems = m_stat->gpn.quickitems;
    quickslots = m_stat->gpn.quickslots;
    quickweapons = m_stat->gpn.weapons;
    quickwslots = m_stat->gpn.wslots;
    break;
  default:
    quickspells = m_stat->gbn.quickspells;
    quickitems = m_stat->gbn.quickitems;
    quickslots = m_stat->gbn.quickslots;
    quickweapons = m_stat->gbn.weapons;
    quickwslots = m_stat->gbn.wslots;
    break;
  }

	CDialog::OnInitDialog();
	
  cb=(CComboBox *) GetDlgItem(IDC_FACE);
  for(i=0;i<NUM_FVALUES;i++)
  {
    cb->AddString(format_direction(i));
  }
  //tooltips
  {
    m_tooltip.Create(this,TTS_NOPREFIX);
    m_tooltip.SetMaxTipWidth(200);
    m_tooltip.SetTipBkColor(RGB(240,224,160));
    
    m_tooltip.AddTool(GetDlgItem(IDCANCEL), IDS_CANCEL);
  }
	
	return TRUE;
}

void CPCInfo::OnDefaultKillfocus() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CPCInfo::OnBrowse1() 
{
  CString tmpstr;
  
  pickerdlg.m_restype=REF_ARE;
  RetrieveResref(pickerdlg.m_picked,m_data->curarea);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,m_data->curarea);
  }
	UpdateData(UD_DISPLAY);
}

void CPCInfo::OnBrowse2() 
{
  CString tmpstr;
  
  pickerdlg.m_restype=REF_CRE;
  RetrieveResref(pickerdlg.m_picked,m_data->creresref);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,m_data->creresref);
  }
	UpdateData(UD_DISPLAY);
}

void CPCInfo::BrowseSpell(int x)
{
  CString tmpstr;
  
  pickerdlg.m_restype=REF_SPL;
  RetrieveResref(pickerdlg.m_picked,quickspells[x]);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,quickspells[x]);
  }
	UpdateData(UD_DISPLAY);
}

void CPCInfo::OnBrowse3() 
{
  BrowseSpell(0);
}

void CPCInfo::OnBrowse4() 
{
  BrowseSpell(1);
}

void CPCInfo::OnBrowse5() 
{
  BrowseSpell(2);
}

void CPCInfo::OnBrowse6() 
{
  BrowseSpell(3);
}

void CPCInfo::OnBrowse7() 
{
  BrowseSpell(4);
}

void CPCInfo::OnBrowse8() 
{
  BrowseSpell(5);
}

void CPCInfo::OnBrowse9() 
{
  BrowseSpell(6);
}

void CPCInfo::OnBrowse10() 
{
  BrowseSpell(7);
}

void CPCInfo::OnBrowse11() 
{
  BrowseSpell(8);
}

void CPCInfo::Wslot(int x)
{
  if (quickwslots[x])
  {
    quickwslots[x]=0;
  }
  else
  {
    quickwslots[x]=-1;
  }
}

void CPCInfo::OnFlag11() 
{
  Wslot(0);
}

void CPCInfo::OnFlag12() 
{
	Wslot(1);
}

void CPCInfo::OnFlag13() 
{
	Wslot(2);
}

void CPCInfo::OnFlag14() 
{
	Wslot(3);
}

void CPCInfo::OnFlag15() 
{
	Wslot(4);
}

void CPCInfo::OnFlag16() 
{
	Wslot(5);
}

void CPCInfo::OnFlag17() 
{
	Wslot(6);
}

void CPCInfo::OnFlag18() 
{
	Wslot(7);
}

void CPCInfo::Islot(int x)
{
  if (quickslots[x])
  {
    quickslots[x]=0;
  }
  else
  {
    quickslots[x]=-1;
  }
}

void CPCInfo::OnU16() 
{
  Islot(0);
}

void CPCInfo::OnU17() 
{
  Islot(1);
}

void CPCInfo::OnU18() 
{
  Islot(2);
}

void CPCInfo::OnU19() 
{
  Islot(3);
}

void CPCInfo::OnU20() 
{
  Islot(4);
}

BOOL CPCInfo::PreTranslateMessage(MSG* pMsg) 
{
  m_tooltip.RelayEvent(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}
