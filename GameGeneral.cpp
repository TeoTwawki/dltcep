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
  CString tmpstr;
  int tmp;
  int i,j;

	CDialog::DoDataExchange(pDX);

  DDX_Text(pDX, IDC_GAMETIME, the_game.header.gametime);
  DDX_Text(pDX, IDC_GOLD, the_game.header.gold);

  RetrieveResref(tmpstr,the_game.header.mainarea);
  DDX_Text(pDX, IDC_STARTAREA, tmpstr);
  StoreResref(tmpstr,the_game.header.mainarea);

  DDX_Text(pDX, IDC_UNKNOWN1C, the_game.header.weather1);

  DDX_Text(pDX, IDC_WEATHER, the_game.header.weather2);

  DDX_Text(pDX, IDC_UNKNOWN48, the_game.header.unknown48);

  tmp=the_game.header.weather2;
  j=1;
  for(i=0;i<8;i++) {
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
	ON_EN_KILLFOCUS(IDC_GAMETIME, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_GOLD, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_WEATHER, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_UNKNOWN1C, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_UNKNOWN48, OnDefaultKillfocus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGameGeneral message handlers

void CGameGeneral::OnDefaultKillfocus() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CGameGeneral::OnFlag1() 
{
	the_game.header.weather2^=1;
	UpdateData(UD_DISPLAY);
}

void CGameGeneral::OnFlag2() 
{
	the_game.header.weather2^=2;	
	UpdateData(UD_DISPLAY);
}

void CGameGeneral::OnFlag3() 
{
	the_game.header.weather2^=4;	
	UpdateData(UD_DISPLAY);
}

void CGameGeneral::OnFlag4() 
{
	the_game.header.weather2^=8;
	UpdateData(UD_DISPLAY);
}

void CGameGeneral::OnFlag5() 
{
	the_game.header.weather2^=0x10;	
	UpdateData(UD_DISPLAY);
}

void CGameGeneral::OnFlag6() 
{
	the_game.header.weather2^=0x20;	
	UpdateData(UD_DISPLAY);
}

void CGameGeneral::OnFlag7() 
{
	the_game.header.weather2^=0x40;	
	UpdateData(UD_DISPLAY);
}

void CGameGeneral::OnFlag8() 
{
	the_game.header.weather2^=0x80;	
	UpdateData(UD_DISPLAY);
}
