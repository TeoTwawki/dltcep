// CreatureLevels.cpp : implementation file
//

#include "stdafx.h"
#include "chitem.h"
#include "CreatureLevels.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCreatureLevels dialog


CCreatureLevels::CCreatureLevels(CWnd* pParent /*=NULL*/)
	: CDialog(CCreatureLevels::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCreatureLevels)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CCreatureLevels::DoDataExchange(CDataExchange* pDX)
{
  int i;

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCreatureLevels)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP

  DDX_Text(pDX, IDC_U1, the_creature.iwd2header.totlevel);

  for(i=0;i<11;i++)
  {
    DDX_Text(pDX, IDC_U2+i, the_creature.iwd2header.levels[i]);
  }
}


BEGIN_MESSAGE_MAP(CCreatureLevels, CDialog)
	//{{AFX_MSG_MAP(CCreatureLevels)
	ON_EN_KILLFOCUS(IDC_U1, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U2, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U3, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U4, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U5, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U6, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U7, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U8, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U9, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U10, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U11, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U12, OnDefaultKillfocus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCreatureLevels message handlers

BOOL CCreatureLevels::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	GetDlgItem(IDC_U1)->EnableWindow(0);

  UpdateData(UD_DISPLAY);
	return TRUE;
}

void CCreatureLevels::OnDefaultKillfocus() 
{
  int i, sum;

	UpdateData(UD_RETRIEVE);
  sum=0;
  for(i=0;i<11;i++)
  {
    sum+=the_creature.iwd2header.levels[i];
  }
  the_creature.iwd2header.totlevel=(unsigned char) sum;
	UpdateData(UD_DISPLAY);
}
