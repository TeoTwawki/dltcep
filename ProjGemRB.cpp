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
  CString tmpstr, tmpstr2;
  CString idsname;
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
  for(i=0;i<32;i++)
  {
    cb=(CButton *) GetDlgItem(IDC_FLAG1+i);
    if (cb)
    {
      flg=!!(the_projectile.header.extflags&j);
      cb->SetCheck(flg);
      j<<=1;
    }
  }
  tmpstr.Format("%d", the_projectile.header.text);
  DDX_Text(pDX, IDC_STRREF, tmpstr);
  the_projectile.header.text=strtonum(tmpstr);

  tmpstr.Format("0x%08x", the_projectile.header.rgb);
  DDX_Text(pDX, IDC_RGB, tmpstr);
  the_projectile.header.rgb=strtonum(tmpstr);
  DDX_Text(pDX, IDC_SPEED, the_projectile.header.pulsespeed);
  DDX_Text(pDX, IDC_SCREEN, the_projectile.header.shake);

  idsname=IDSName2(the_projectile.header.atype, true);
  tmpstr.Format("%d %s", the_projectile.header.atype, idsname);
  DDX_Text(pDX, IDC_IDS, tmpstr);
  the_projectile.header.atype = (short) IDSIndex(tmpstr, true);

  tmpstr2 = IDSToken(idsname, the_projectile.header.affects, false);
  tmpstr.Format("%d %s", the_projectile.header.affects, tmpstr2);
  DDX_Text(pDX, IDC_VALUE, tmpstr);
  the_projectile.header.affects = (short) IDSKey2(idsname, tmpstr);

  idsname=IDSName2(the_projectile.header.atype2, true);
  tmpstr.Format("%d %s", the_projectile.header.atype2, idsname);
  DDX_Text(pDX, IDC_IDS2, tmpstr);
  the_projectile.header.atype2 = (short) IDSIndex(tmpstr, true);

  tmpstr2 = IDSToken(idsname, the_projectile.header.affects2, false);
  tmpstr.Format("%d %s", the_projectile.header.affects2, tmpstr2);
  DDX_Text(pDX, IDC_VALUE2, tmpstr);
  the_projectile.header.affects2 = (short) IDSKey2(idsname, tmpstr);

  RetrieveResref(tmpstr, the_projectile.header.failspell);
  DDX_Text(pDX, IDC_SPELL, tmpstr);
  DDV_MaxChars(pDX, tmpstr, 8);
  StoreResref(tmpstr, the_projectile.header.failspell);

  RetrieveResref(tmpstr, the_projectile.header.succspell);
  DDX_Text(pDX, IDC_SPELL2, tmpstr);
  DDV_MaxChars(pDX, tmpstr, 8);
  StoreResref(tmpstr, the_projectile.header.succspell);
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
	ON_EN_KILLFOCUS(IDC_STRREF, OnKillfocus)
	ON_BN_CLICKED(IDC_FLAG19, OnFlag19)
	ON_EN_KILLFOCUS(IDC_STRING, OnKillfocusString)
	ON_BN_CLICKED(IDC_FLAG20, OnFlag20)
	ON_BN_CLICKED(IDC_FLAG21, OnFlag21)
	ON_BN_CLICKED(IDC_FLAG22, OnFlag22)
	ON_BN_CLICKED(IDC_FLAG23, OnFlag23)
	ON_BN_CLICKED(IDC_COLOR, OnColor)
	ON_BN_CLICKED(IDC_FLAG24, OnFlag24)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_BROWSE2, OnBrowse2)
	ON_BN_CLICKED(IDC_FLAG25, OnFlag25)
	ON_BN_CLICKED(IDC_FLAG26, OnFlag26)
	ON_BN_CLICKED(IDC_FLAG27, OnFlag27)
	ON_EN_KILLFOCUS(IDC_RGB, OnKillfocus)
	ON_EN_KILLFOCUS(IDC_SPEED, OnKillfocus)
	ON_EN_KILLFOCUS(IDC_SCREEN, OnKillfocus)
	ON_EN_KILLFOCUS(IDC_FLAGS, OnKillfocus)
	ON_EN_KILLFOCUS(IDC_SPELL, OnKillfocus)
	ON_EN_KILLFOCUS(IDC_IDS, OnKillfocus)
	ON_EN_KILLFOCUS(IDC_VALUE, OnKillfocus)
	ON_EN_KILLFOCUS(IDC_SPELL2, OnKillfocus)
	ON_EN_KILLFOCUS(IDC_IDS2, OnKillfocus)
	ON_EN_KILLFOCUS(IDC_VALUE2, OnKillfocus)
	ON_BN_CLICKED(IDC_FLAG28, OnFlag28)
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

void CProjGemRB::OnFlag23() 
{
  the_projectile.header.extflags^=PROJ_CYCLE;
	UpdateData(UD_DISPLAY);
}

void CProjGemRB::OnFlag24() 
{
  the_projectile.header.extflags^=PROJ_RGB;
	UpdateData(UD_DISPLAY);
}

void CProjGemRB::OnFlag25() 
{
  the_projectile.header.extflags^=PROJ_TOUCH;
	UpdateData(UD_DISPLAY);
}

void CProjGemRB::OnFlag26() 
{
  the_projectile.header.extflags^=PROJ_NOTIDS;
	UpdateData(UD_DISPLAY);
}

void CProjGemRB::OnFlag27() 
{
  the_projectile.header.extflags^=PROJ_NOTIDS2;
	UpdateData(UD_DISPLAY);
}

void CProjGemRB::OnFlag28() 
{
  the_projectile.header.extflags^=PROJ_BOTH;
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

void CProjGemRB::OnColor() 
{
	int color;
  CColorDialog dlg;
  COLORREF predef[16];

  for(int i=0;i<16;i++)
  {
    predef[i]=the_projectile.header.rgb;
  }
  dlg.m_cc.lpTemplateName="Pick a colour for the RGB effect";
  dlg.m_cc.lpCustColors=predef;
  if(dlg.DoModal()==IDOK)
  {
    color=dlg.GetColor();
    the_projectile.header.rgb=color;
  }
  UpdateData(UD_DISPLAY);
}

void CProjGemRB::OnBrowse() 
{
  pickerdlg.m_restype=REF_SPL;
  RetrieveResref(pickerdlg.m_picked, the_projectile.header.failspell);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked, the_projectile.header.failspell);
  }
	UpdateData(UD_DISPLAY);	
}

void CProjGemRB::OnBrowse2() 
{
  pickerdlg.m_restype=REF_SPL;
  RetrieveResref(pickerdlg.m_picked, the_projectile.header.succspell);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked, the_projectile.header.succspell);
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
/////////////////////////////////////////////////////////////////////////////
// CProjAreaGemRB dialog


CProjAreaGemRB::CProjAreaGemRB(CWnd* pParent /*=NULL*/)
	: CDialog(CProjAreaGemRB::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProjAreaGemRB)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CProjAreaGemRB::DoDataExchange(CDataExchange* pDX)
{
  CButton *cb;
  CString tmpstr;
  int i,j;
  int flg;

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProjAreaGemRB)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
  tmpstr.Format("%d", the_projectile.extension.gemrbflags);
  DDX_Text(pDX, IDC_FLAGS, tmpstr);
  the_projectile.extension.gemrbflags=strtonum(tmpstr);
  j=1;
  for(i=0;i<16;i++)
  {
    cb=(CButton *) GetDlgItem(IDC_FLAG1+i);
    if (cb)
    {
      flg=!!(the_projectile.extension.gemrbflags&j);
      cb->SetCheck(flg);
      j<<=1;
    }
  }
  RetrieveResref(tmpstr,the_projectile.extension.wavc2);
  DDX_Text(pDX, IDC_SOUND1, tmpstr);
  DDV_MaxChars(pDX, tmpstr, 8);
  StoreResref(tmpstr,the_projectile.extension.wavc2);

  RetrieveResref(tmpstr,the_projectile.extension.spread);
  DDX_Text(pDX, IDC_BAM1, tmpstr);
  DDV_MaxChars(pDX, tmpstr, 8);
  StoreResref(tmpstr,the_projectile.extension.spread);

  RetrieveResref(tmpstr,the_projectile.extension.second);
  DDX_Text(pDX, IDC_BAM2, tmpstr);
  DDV_MaxChars(pDX, tmpstr, 8);
  StoreResref(tmpstr,the_projectile.extension.second);
}


BEGIN_MESSAGE_MAP(CProjAreaGemRB, CDialog)
	//{{AFX_MSG_MAP(CProjAreaGemRB)
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
	ON_BN_CLICKED(IDC_BROWSE1, OnBrowse1)
	ON_BN_CLICKED(IDC_BROWSE2, OnBrowse2)
	ON_BN_CLICKED(IDC_BROWSE3, OnBrowse3)
	ON_BN_CLICKED(IDC_PLAY1, OnPlay1)
	ON_EN_KILLFOCUS(IDC_FLAGS, OnKillfocus)
	ON_EN_KILLFOCUS(IDC_SOUND1, OnKillfocus)
	ON_EN_KILLFOCUS(IDC_BAM1, OnKillfocus)
	ON_EN_KILLFOCUS(IDC_BAM2, OnKillfocus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProjAreaGemRB message handlers

void CProjAreaGemRB::OnFlag1() 
{
  the_projectile.extension.gemrbflags^=APF_TINT;
	UpdateData(UD_DISPLAY);
}

void CProjAreaGemRB::OnFlag2() 
{
  the_projectile.extension.gemrbflags^=APF_FILL;
	UpdateData(UD_DISPLAY);
}

void CProjAreaGemRB::OnFlag3() 
{
  the_projectile.extension.gemrbflags^=APF_SCATTER;
	UpdateData(UD_DISPLAY);
}

void CProjAreaGemRB::OnFlag4() 
{
  the_projectile.extension.gemrbflags^=APF_VVCPAL;
	UpdateData(UD_DISPLAY);
}

void CProjAreaGemRB::OnFlag5() 
{
  the_projectile.extension.gemrbflags^=APF_SPREAD;
	UpdateData(UD_DISPLAY);
}

void CProjAreaGemRB::OnFlag6() 
{
  the_projectile.extension.gemrbflags^=APF_PALETTE;
	UpdateData(UD_DISPLAY);
}

void CProjAreaGemRB::OnFlag7() 
{
  the_projectile.extension.gemrbflags^=APF_BOTH;
	UpdateData(UD_DISPLAY);
}

void CProjAreaGemRB::OnFlag8() 
{
  the_projectile.extension.gemrbflags^=APF_MORE;
	UpdateData(UD_DISPLAY);
}

void CProjAreaGemRB::OnFlag9() 
{
  the_projectile.extension.gemrbflags^=APF_FAILSPELL;
	UpdateData(UD_DISPLAY);
}

void CProjAreaGemRB::OnFlag10() 
{
  the_projectile.extension.gemrbflags^=APF_MULTIPLE;
	UpdateData(UD_DISPLAY);
}

void CProjAreaGemRB::OnKillfocus() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CProjAreaGemRB::OnBrowse1() 
{
  CString tmpstr;
  
  pickerdlg.m_restype=REF_WAV;
  RetrieveResref(pickerdlg.m_picked,the_projectile.extension.wavc2);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_projectile.extension.wavc2);
  }
	UpdateData(UD_DISPLAY);
}

void CProjAreaGemRB::OnBrowse2() 
{
  CString tmpstr;
  
  pickerdlg.m_restype=REF_BAM;
  RetrieveResref(pickerdlg.m_picked,the_projectile.extension.spread);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_projectile.extension.spread);
  }
	UpdateData(UD_DISPLAY);
}

void CProjAreaGemRB::OnBrowse3() 
{
  CString tmpstr;
  
  pickerdlg.m_restype=REF_BAM;
  RetrieveResref(pickerdlg.m_picked,the_projectile.extension.second);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_projectile.extension.second);
  }
	UpdateData(UD_DISPLAY);
}

void CProjAreaGemRB::OnPlay1() 
{
  CString tmpstr;

  RetrieveResref(tmpstr, the_projectile.extension.wavc2);
  play_acm(tmpstr,false,false);
}

