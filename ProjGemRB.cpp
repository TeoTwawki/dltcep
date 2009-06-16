// ProjGemRB.cpp : implementation file
//

#include "stdafx.h"
#include "chitem.h"
#include "ProjGemRB.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProjGemRB dialog


CProjGemRB::CProjGemRB(CWnd* pParent /*=NULL*/)
	: CDialog(CProjGemRB::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProjGemRB)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CProjGemRB::DoDataExchange(CDataExchange* pDX)
{
  CButton *cb;
  CString tmpstr;
  int i,j;
  int flg;

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProjGemRB)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
  tmpstr.Format("%d", the_projectile.header.extflags);
  DDX_Text(pDX, IDC_FLAGS, tmpstr);
  the_projectile.header.extflags=strtonum(tmpstr);
  j=1;
  for(i=0;i<16;i++)
  {
    cb=(CButton *) GetDlgItem(IDC_FLAG1+i);
    if (cb)
    {
      flg=!!(the_projectile.header.extflags&j);
      cb->SetCheck(flg);
      j<<=1;
    }
  }
}


BEGIN_MESSAGE_MAP(CProjGemRB, CDialog)
	//{{AFX_MSG_MAP(CProjGemRB)
	ON_BN_CLICKED(IDC_FLAG1, OnFlag1)
	ON_BN_CLICKED(IDC_FLAG2, OnFlag2)
	ON_BN_CLICKED(IDC_FLAG3, OnFlag3)
	ON_BN_CLICKED(IDC_FLAG4, OnFlag4)
	ON_BN_CLICKED(IDC_FLAG5, OnFlag5)
	ON_BN_CLICKED(IDC_FLAG6, OnFlag6)
	ON_BN_CLICKED(IDC_FLAG7, OnFlag7)
	ON_BN_CLICKED(IDC_FLAG8, OnFlag8)
	ON_BN_CLICKED(IDC_FLAG9, OnFlag9)
	ON_BN_CLICKED(IDC_FLAG10, OnFlag10)
	ON_BN_CLICKED(IDC_FLAG11, OnFlag11)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProjGemRB message handlers


void CProjGemRB::OnFlag1() 
{
  the_projectile.header.extflags^=PROJ_BOUNCE;
	UpdateData(UD_DISPLAY);
}

void CProjGemRB::OnFlag2() 
{
  the_projectile.header.extflags^=PROJ_CONTINUE;
	UpdateData(UD_DISPLAY);
}

void CProjGemRB::OnFlag3() 
{
  the_projectile.header.extflags^=PROJ_FREEZE;
	UpdateData(UD_DISPLAY);
}

void CProjGemRB::OnFlag4() 
{
  the_projectile.header.extflags^=PROJ_NO_TRAVEL;
	UpdateData(UD_DISPLAY);
}

void CProjGemRB::OnFlag5() 
{
  the_projectile.header.extflags^=PROJ_TRAIL_FACE;
	UpdateData(UD_DISPLAY);
}

void CProjGemRB::OnFlag6() 
{
  the_projectile.header.extflags^=PROJ_CURVE;
	UpdateData(UD_DISPLAY);
}

void CProjGemRB::OnFlag7() 
{
  the_projectile.header.extflags^=PROJ_RANDOM;
	UpdateData(UD_DISPLAY);
}

void CProjGemRB::OnFlag8() 
{
  the_projectile.header.extflags^=PROJ_PILLAR;
	UpdateData(UD_DISPLAY);
}

void CProjGemRB::OnFlag9() 
{
  the_projectile.header.extflags^=PROJ_TRANSLUCENT;
	UpdateData(UD_DISPLAY);
}

void CProjGemRB::OnFlag10() 
{
  the_projectile.header.extflags^=PROJ_TINTED;
	UpdateData(UD_DISPLAY);
}

void CProjGemRB::OnFlag11() 
{
  the_projectile.header.extflags^=PROJ_ITERATION;
	UpdateData(UD_DISPLAY);
}
