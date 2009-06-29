// ProjGemRB.cpp : implementation file
//

#include "stdafx.h"
#include "chitem.h"
#include "ProjGemRB.h"
#include "options.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProjGemRB dialog

static int textids[]={IDC_TEXT, IDC_STRREF,0};

CProjGemRB::CProjGemRB(CWnd* pParent /*=NULL*/)
	: CDialog(CProjGemRB::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProjGemRB)
	m_text = _T("");
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
	DDX_Text(pDX, IDC_STRING, m_text);
	//}}AFX_DATA_MAP
  tmpstr.Format("%d", the_projectile.header.extflags);
  DDX_Text(pDX, IDC_FLAGS, tmpstr);
  the_projectile.header.extflags=strtonum(tmpstr);
  j=1;
  for(i=0;i<24;i++)
  {
    cb=(CButton *) GetDlgItem(IDC_FLAG1+i);
    if (cb)
    {
      flg=!!(the_projectile.header.extflags&j);
      cb->SetCheck(flg);
      j<<=1;
    }
  }
  tmpstr.Format("%d",  the_projectile.header.text);
  DDX_Text(pDX, IDC_STRREF, tmpstr);
  the_projectile.header.text=strtonum(tmpstr);
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
	ON_BN_CLICKED(IDC_FLAG12, OnFlag12)
	ON_BN_CLICKED(IDC_FLAG13, OnFlag13)
	ON_BN_CLICKED(IDC_FLAG14, OnFlag14)
	ON_BN_CLICKED(IDC_FLAG15, OnFlag15)
	ON_BN_CLICKED(IDC_FLAG16, OnFlag16)
	ON_BN_CLICKED(IDC_FLAG17, OnFlag17)
	ON_BN_CLICKED(IDC_FLAG18, OnFlag18)
	ON_EN_KILLFOCUS(IDC_FLAGS, OnKillfocus)
	ON_BN_CLICKED(IDC_FLAG19, OnFlag19)
	ON_EN_KILLFOCUS(IDC_STRING, OnKillfocusString)
	ON_BN_CLICKED(IDC_FLAG20, OnFlag20)
	ON_BN_CLICKED(IDC_FLAG21, OnFlag21)
	ON_EN_KILLFOCUS(IDC_STRREF, OnKillfocus)
	ON_BN_CLICKED(IDC_FLAG22, OnFlag22)
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

void CProjGemRB::OnFlag12() 
{
  the_projectile.header.extflags^=PROJ_TILE;
	UpdateData(UD_DISPLAY);
}

void CProjGemRB::OnFlag13() 
{
  the_projectile.header.extflags^=PROJ_FALLING;
	UpdateData(UD_DISPLAY);
}

void CProjGemRB::OnFlag14() 
{
  the_projectile.header.extflags^=PROJ_INCOMING;
	UpdateData(UD_DISPLAY);
}

void CProjGemRB::OnFlag15() 
{
  the_projectile.header.extflags^=PROJ_LINE;
	UpdateData(UD_DISPLAY);
}

void CProjGemRB::OnFlag16() 
{
  the_projectile.header.extflags^=PROJ_WALL;
	UpdateData(UD_DISPLAY);
}

void CProjGemRB::OnFlag17() 
{
  the_projectile.header.extflags^=PROJ_BACKGROUND;
	UpdateData(UD_DISPLAY);
}

void CProjGemRB::OnFlag18() 
{
  the_projectile.header.extflags^=PROJ_POP;
	UpdateData(UD_DISPLAY);
}

void CProjGemRB::OnFlag19() 
{
  the_projectile.header.extflags^=PROJ_POP_OUT;
	UpdateData(UD_DISPLAY);
}

void CProjGemRB::OnFlag20() 
{
  the_projectile.header.extflags^=PROJ_FADE;
	UpdateData(UD_DISPLAY);
}

void CProjGemRB::OnFlag21() 
{
  the_projectile.header.extflags^=PROJ_TEXT;
	UpdateData(UD_DISPLAY);
}

void CProjGemRB::OnFlag22() 
{
  the_projectile.header.extflags^=PROJ_WANDERING;
	UpdateData(UD_DISPLAY);
}

void CProjGemRB::RefreshStrings()
{
  m_text=resolve_tlk_text(the_projectile.header.text);
}

void CProjGemRB::OnKillfocus() 
{
	UpdateData(UD_RETRIEVE);
  RefreshStrings();
	UpdateData(UD_DISPLAY);
}

void CProjGemRB::OnKillfocusString() 
{
  CString tmpstr;

	UpdateData(UD_RETRIEVE);
  tmpstr=resolve_tlk_text(the_projectile.header.text);
  if(tmpstr!=m_text)
  {
    if(editflg&TLKCHANGE)
    {
      if(MessageBox("Do you want to update dialog.tlk?","Area editor",MB_YESNO)!=IDYES)
      {
        return;
      }      
    }
    the_projectile.header.text=store_tlk_text(the_projectile.header.text,m_text);
  }
	UpdateData(UD_DISPLAY);
}

BOOL CProjGemRB::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  RefreshStrings();
  UpdateData(UD_DISPLAY);
	return TRUE;
}
