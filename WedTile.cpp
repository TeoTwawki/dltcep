// WedTile.cpp : implementation file
//

#include "stdafx.h"
#include "chitem.h"
#include "WedTile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWedTile dialog

CWedTile::CWedTile(CWnd* pParent /*=NULL*/) : CDialog(CWedTile::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWedTile)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

static int tileboxids[]={IDC_FLAG1, IDC_FLAG2,IDC_FLAG3, IDC_FLAG4,
IDC_FLAG5, IDC_FLAG6, IDC_FLAG7, IDC_FLAG8,
0};

void CWedTile::DoDataExchange(CDataExchange* pDX)
{
  CButton *cb;
  int i,j;

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWedTile)
	DDX_Control(pDX, IDC_BLOCKPICKER, m_tilepicker);
  //}}AFX_DATA_MAP
  if(m_tilenum>=0)
  {
    DDX_Text(pDX, IDC_FIRST, m_ptileheader[m_tilenum].firsttileprimary);
    DDX_Text(pDX, IDC_NUM, m_ptileheader[m_tilenum].counttileprimary);
    DDX_Text(pDX, IDC_ALT, m_ptileheader[m_tilenum].alternate);
    DDX_Text(pDX, IDC_FLAGS, m_ptileheader[m_tilenum].flags);
    DDX_Text(pDX, IDC_UNKNOWN, m_ptileheader[m_tilenum].unknown);

    i=0;
    j=1;
    while(tileboxids[i])
    {
      cb=(CButton *) GetDlgItem(tileboxids[i]);
      cb->SetCheck(m_ptileheader[m_tilenum].flags&j);
      i++;
      j<<=1;
    }
  }
}

BEGIN_MESSAGE_MAP(CWedTile, CDialog)
	//{{AFX_MSG_MAP(CWedTile)
	ON_LBN_SELCHANGE(IDC_BLOCKPICKER, OnSelchangeBlockpicker)
	ON_EN_KILLFOCUS(IDC_FIRST, DefaultKillfocus)
	ON_BN_CLICKED(IDC_FLAG1, OnFlag1)
	ON_BN_CLICKED(IDC_FLAG2, OnFlag2)
	ON_BN_CLICKED(IDC_FLAG3, OnFlag3)
	ON_BN_CLICKED(IDC_FLAG4, OnFlag4)
	ON_BN_CLICKED(IDC_FLAG5, OnFlag5)
	ON_BN_CLICKED(IDC_FLAG6, OnFlag6)
	ON_BN_CLICKED(IDC_FLAG7, OnFlag7)
	ON_BN_CLICKED(IDC_FLAG8, OnFlag8)
	ON_BN_CLICKED(IDC_TILE, OnTile)
	ON_BN_CLICKED(IDC_ADDALT, OnAddalt)
	ON_EN_KILLFOCUS(IDC_NUM, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_ALT, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_FLAGS, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_UNKNOWN, DefaultKillfocus)
	ON_BN_CLICKED(IDC_REMALL, OnRemall)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWedTile message handlers

void CWedTile::RefreshTile()
{
  CString tmpstr;
  int maxtilecount;
  int i,j;

  if(!m_tilepicker) return;
  m_tilenum=m_tilepicker.GetCurSel();
  if(m_tilenum<0) m_tilenum=0;
  m_tilepicker.ResetContent();
  for(j=0;j<m_tilecounty;j++) for(i=0;i<m_tilecountx;i++)
  {
    tmpstr.Format("[%d.%d]",i,j);
    m_tilepicker.AddString(tmpstr);
  }
  maxtilecount=i*j-1;

  if(m_tilenum>=maxtilecount) m_tilenum=maxtilecount-1;
  m_tilenum=m_tilepicker.SetCurSel(m_tilenum);
  i=0;
  while(tileboxids[i])
  {
    GetDlgItem(tileboxids[i])->EnableWindow(m_tilenum>=0);
    i++;
  }
}

BOOL CWedTile::OnInitDialog() 
{
	CDialog::OnInitDialog();
  RefreshTile();
  UpdateData(UD_DISPLAY);
	return TRUE;
}

void CWedTile::OnSelchangeBlockpicker() 
{
  m_tilenum=m_tilepicker.GetCurSel();
  UpdateData(UD_DISPLAY);
}

void CWedTile::DefaultKillfocus() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CWedTile::OnFlag1() 
{
	m_ptileheader[m_tilenum].flags^=1;
  UpdateData(UD_DISPLAY);
}

void CWedTile::OnFlag2() 
{
	m_ptileheader[m_tilenum].flags^=2;
  UpdateData(UD_DISPLAY);
}

void CWedTile::OnFlag3() 
{
	m_ptileheader[m_tilenum].flags^=4;
  UpdateData(UD_DISPLAY);
}

void CWedTile::OnFlag4() 
{
	m_ptileheader[m_tilenum].flags^=8;
  UpdateData(UD_DISPLAY);
}

void CWedTile::OnFlag5() 
{
	m_ptileheader[m_tilenum].flags^=16;
  UpdateData(UD_DISPLAY);
}

void CWedTile::OnFlag6() 
{
	m_ptileheader[m_tilenum].flags^=32;
  UpdateData(UD_DISPLAY);
}

void CWedTile::OnFlag7() 
{
	m_ptileheader[m_tilenum].flags^=64;
  UpdateData(UD_DISPLAY);
}

void CWedTile::OnFlag8() 
{
	m_ptileheader[m_tilenum].flags^=128;
  UpdateData(UD_DISPLAY);
}

void CWedTile::OnTile() 
{
	CImageView dlg;
  CPoint cp;

  dlg.m_clipx=dlg.m_minclipx=0;
  dlg.m_clipy=dlg.m_minclipy=0;
  dlg.m_maxclipx=m_tilecountx;
  dlg.m_maxclipy=m_tilecounty;
  dlg.m_oladjust=0;
  dlg.m_enablebutton=IW_ENABLEBUTTON;
  dlg.DoModal();
  cp=dlg.GetPoint(true);
  m_tilenum=m_tilepicker.SetCurSel(cp.y*m_tilecountx+cp.x);
  UpdateData(UD_DISPLAY);
}

void CWedTile::OnAddalt() 
{
	//makes a copy of tiles as alternate
	
}

void CWedTile::OnRemall() 
{
	// TODO: Add your control notification handler code here
	
}
