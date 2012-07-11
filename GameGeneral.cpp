// GameGeneral.cpp : implementation file
//

#include "stdafx.h"
#include "chitem.h"
#include "GameGeneral.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGameGeneral dialog


CGameGeneral::CGameGeneral(CWnd* pParent /*=NULL*/)
	: CDialog(CGameGeneral::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGameGeneral)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CGameGeneral::DoDataExchange(CDataExchange* pDX)
{
  CButton *cb;
	CWnd *cw;
  CString tmpstr;
  int tmp;
  int i,j;

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGameGeneral)
	DDX_Control(pDX, IDC_SLIDER, m_slider_control);
	//}}AFX_DATA_MAP

  DDX_Text(pDX, IDC_GAMETIME, the_game.header.gametime);

  DDX_Text(pDX, IDC_GOLD, the_game.header.gold);
	if (the_game.revision==12)
	{
		DDX_Text(pDX, IDC_REPUTATION, the_game.pstheader.reputation);
		RetrieveResref(tmpstr,the_game.pstheader.mainarea);
		DDX_Text(pDX, IDC_STARTAREA, tmpstr);
		StoreResref(tmpstr,the_game.pstheader.mainarea);
		DDX_Text(pDX, IDC_AREA, tmpstr);
		StoreResref(tmpstr,the_game.pstheader.curarea);
    cw = GetDlgItem(IDC_SCREEN);
		cw->EnableWindow(false);
		DDX_Text(pDX, IDC_UNKNOWN48, the_game.pstheader.unknown48);
		cw = GetDlgItem(IDC_SLIDER);
		cw->ShowWindow(false);
		DDX_Text(pDX, IDC_U1, the_game.header.formation);
		DDV_MinMaxShort(pDX, the_game.header.formation, 0, 11);
		for(i=1;i<5;i++)
		{
			cw = GetDlgItem(IDC_U1+i);
			cw->ShowWindow(false);
		}
    cw=GetDlgItem(IDC_REALTIME);
    cw->EnableWindow(false);
	}
	else
	{
    DDX_Text(pDX, IDC_REALTIME, the_game.header.realtime);
		DDX_Text(pDX, IDC_REPUTATION, the_game.header.reputation);
		RetrieveResref(tmpstr,the_game.header.currentarea);
		DDX_Text(pDX, IDC_STARTAREA, tmpstr);
		StoreResref(tmpstr,the_game.header.currentarea);
    RetrieveResref(tmpstr,the_game.header.masterarea);
		DDX_Text(pDX, IDC_AREA, tmpstr);
		StoreResref(tmpstr,the_game.header.masterarea);
		DDX_Text(pDX, IDC_SCREEN, the_game.header.controls);
    DDX_Text(pDX, IDC_UNKNOWN48, the_game.header.currentlink);
    DDX_Text(pDX, IDC_UNKNOWN64, the_game.header.version);
		i = the_game.header.formation;
		DDX_Slider(pDX, IDC_SLIDER,i);
		the_game.header.formation = (short) i;
		for(i=0;i<5;i++)
		{
			DDX_Text(pDX, IDC_U1+i, the_game.header.formations[i]);
			DDV_MinMaxShort(pDX, the_game.header.formations[i], 0, 11);
		}
	}

  DDX_Text(pDX, IDC_UNKNOWN1C, the_game.header.areaviewed);
  DDX_Text(pDX, IDC_WEATHER, the_game.header.weather);

  tmp=the_game.header.weather;
  j=1;
  for(i=0;i<9;i++)
	{
		cb=(CButton *) GetDlgItem(IDC_FLAG1+i);
		if(cb) cb->SetCheck(!!(tmp&j) );
		j<<=1;
  }	
}


BEGIN_MESSAGE_MAP(CGameGeneral, CDialog)
	//{{AFX_MSG_MAP(CGameGeneral)
	ON_EN_KILLFOCUS(IDC_STARTAREA, OnDefaultKillfocus)
	ON_BN_CLICKED(IDC_FLAG1, OnFlag1)
	ON_BN_CLICKED(IDC_FLAG2, OnFlag2)
	ON_BN_CLICKED(IDC_FLAG3, OnFlag3)
	ON_BN_CLICKED(IDC_FLAG4, OnFlag4)
	ON_BN_CLICKED(IDC_FLAG5, OnFlag5)
	ON_BN_CLICKED(IDC_FLAG6, OnFlag6)
	ON_BN_CLICKED(IDC_FLAG7, OnFlag7)
	ON_BN_CLICKED(IDC_FLAG8, OnFlag8)
	ON_BN_CLICKED(IDC_FLAG9, OnFlag9)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_BROWSE2, OnBrowse2)
	ON_EN_KILLFOCUS(IDC_GAMETIME, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_GOLD, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_WEATHER, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_UNKNOWN1C, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_UNKNOWN48, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_SCREEN, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_REPUTATION, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U1, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U2, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U3, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U4, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U5, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_AREA, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_UNKNOWN64, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_REALTIME, OnDefaultKillfocus)
	//}}AFX_MSG_MAP
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER, OnDefaultKillfocusX)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGameGeneral message handlers

BOOL CGameGeneral::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_slider_control.SetRange(0, 4, TRUE);
  //tooltips
  {
    m_tooltip.Create(this,TTS_NOPREFIX);
    m_tooltip.SetMaxTipWidth(200);
    m_tooltip.SetTipBkColor(RGB(240,224,160));
    
    m_tooltip.AddTool(GetDlgItem(IDCANCEL), IDS_CANCEL);
	}
	
	return TRUE;
}

void CGameGeneral::OnDefaultKillfocus() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CGameGeneral::OnDefaultKillfocusX(NMHDR * /*header*/, LRESULT * /*result*/) 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CGameGeneral::OnFlag1() 
{
	the_game.header.weather^=1;
	UpdateData(UD_DISPLAY);
}

void CGameGeneral::OnFlag2() 
{
	the_game.header.weather^=2;	
	UpdateData(UD_DISPLAY);
}

void CGameGeneral::OnFlag3() 
{
	the_game.header.weather^=4;	
	UpdateData(UD_DISPLAY);
}

void CGameGeneral::OnFlag4() 
{
	the_game.header.weather^=8;
	UpdateData(UD_DISPLAY);
}

void CGameGeneral::OnFlag5() 
{
	the_game.header.weather^=0x10;	
	UpdateData(UD_DISPLAY);
}

void CGameGeneral::OnFlag6() 
{
	the_game.header.weather^=0x20;	
	UpdateData(UD_DISPLAY);
}

void CGameGeneral::OnFlag7() 
{
	the_game.header.weather^=0x40;	
	UpdateData(UD_DISPLAY);
}

void CGameGeneral::OnFlag8() 
{
	the_game.header.weather^=0x80;	
	UpdateData(UD_DISPLAY);
}

void CGameGeneral::OnFlag9() 
{
	the_game.header.weather^=0x100;	
	UpdateData(UD_DISPLAY);
}

void CGameGeneral::OnBrowse() 
{
  CString tmpstr;
  
  pickerdlg.m_restype=REF_ARE;
  RetrieveResref(pickerdlg.m_picked,the_game.header.currentarea);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_game.header.currentarea);
  }
	UpdateData(UD_DISPLAY);
}


void CGameGeneral::OnBrowse2() 
{
  CString tmpstr;
  
  pickerdlg.m_restype=REF_ARE;
  RetrieveResref(pickerdlg.m_picked,the_game.header.masterarea);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_game.header.masterarea);
  }
	UpdateData(UD_DISPLAY);
}

