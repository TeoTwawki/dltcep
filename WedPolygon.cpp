// WedPolygon.cpp : implementation file
//

#include "stdafx.h"
#include "chitem.h"
#include "WedPolygon.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWedPolygon dialog


CWedPolygon::CWedPolygon(CWnd* pParent /*=NULL*/)
	: CDialog(CWedPolygon::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWedPolygon)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
  m_vertexnum=-1;
}

static int vertexboxids[]={IDC_POSX, IDC_POSY,IDC_REVERSE, IDC_ORDER,
0};

void CWedPolygon::RefreshPolygon()
{
  CString tmpstr;
  CButton *cb;
  int i,j;  

  if(!m_vertexpicker) return;
  m_vertexpicker.ResetContent();
  for(i=0;i<wedpolygon->countvertex;i++)
  {
    tmpstr.Format("%d  [%d.%d]",i+1, wedvertex[i].point.x,wedvertex[i].point.y);
    m_vertexpicker.AddString(tmpstr);
  }

  if(m_vertexnum<0) m_vertexnum=0;
  m_vertexnum=m_vertexpicker.SetCurSel(m_vertexnum);
  i=0;
  while(vertexboxids[i])
  {
    GetDlgItem(vertexboxids[i])->EnableWindow(m_vertexnum>=0);
    i++;
  }

  j=1;
  for(i=0;i<8;i++)
  {
    cb=(CButton *) GetDlgItem(IDC_FLAG1+i);
    cb->SetCheck(!!(wedpolygon->flags&j));
    j<<=1;
  }
}

void CWedPolygon::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWedPolygon)
	DDX_Control(pDX, IDC_VERTEXPICKER, m_vertexpicker);
	//}}AFX_DATA_MAP
  DDX_Text(pDX, IDC_FLAGS, wedpolygon->flags);
  DDX_Text(pDX, IDC_UNKNOWN, wedpolygon->unkflags);
  DDX_Text(pDX, IDC_MINX, wedpolygon->minx);
  DDX_Text(pDX, IDC_MINY, wedpolygon->miny);
  DDX_Text(pDX, IDC_MAXX, wedpolygon->maxx);
  DDX_Text(pDX, IDC_MAXY, wedpolygon->maxy);
  if(m_vertexnum>=0)
  {
    DDX_Text(pDX, IDC_POSX, wedvertex[m_vertexnum].point.x);
    DDX_Text(pDX, IDC_POSY, wedvertex[m_vertexnum].point.y);
  }
}

BOOL CWedPolygon::OnInitDialog() 
{
	CDialog::OnInitDialog();
  RefreshPolygon();
  UpdateData(UD_DISPLAY);
	return TRUE;
}

BEGIN_MESSAGE_MAP(CWedPolygon, CDialog)
	//{{AFX_MSG_MAP(CWedPolygon)
	ON_BN_CLICKED(IDC_FLAG1, OnFlag1)
	ON_BN_CLICKED(IDC_FLAG2, OnFlag2)
	ON_BN_CLICKED(IDC_FLAG3, OnFlag3)
	ON_BN_CLICKED(IDC_FLAG4, OnFlag4)
	ON_BN_CLICKED(IDC_FLAG5, OnFlag5)
	ON_BN_CLICKED(IDC_FLAG6, OnFlag6)
	ON_BN_CLICKED(IDC_FLAG7, OnFlag7)
	ON_BN_CLICKED(IDC_FLAG8, OnFlag8)
	ON_EN_KILLFOCUS(IDC_FLAGS, OnKillfocusFlags)
	ON_EN_KILLFOCUS(IDC_UNKNOWN, OnKillfocusUnknown)
	ON_EN_KILLFOCUS(IDC_MINX, OnKillfocusMinx)
	ON_EN_KILLFOCUS(IDC_MINY, OnKillfocusMiny)
	ON_EN_KILLFOCUS(IDC_MAXX, OnKillfocusMaxx)
	ON_EN_KILLFOCUS(IDC_MAXY, OnKillfocusMaxy)
	ON_LBN_KILLFOCUS(IDC_VERTEXPICKER, OnKillfocusVertexpicker)
	ON_LBN_SELCHANGE(IDC_VERTEXPICKER, OnSelchangeVertexpicker)
	ON_EN_KILLFOCUS(IDC_POSX, OnKillfocusPosx)
	ON_EN_KILLFOCUS(IDC_POSY, OnKillfocusPosy)
	ON_BN_CLICKED(IDC_RECALCBOX, OnRecalcbox)
	ON_BN_CLICKED(IDC_REVERSE, OnReverse)
	ON_BN_CLICKED(IDC_ORDER, OnOrder)
	ON_BN_CLICKED(IDC_SHIFT, OnShift)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWedPolygon message handlers

void CWedPolygon::OnFlag1() 
{
	wedpolygon->flags^=1;	
}

void CWedPolygon::OnFlag2() 
{
	wedpolygon->flags^=2;	
  UpdateData(UD_DISPLAY);
}

void CWedPolygon::OnFlag3() 
{
	wedpolygon->flags^=4;	
  UpdateData(UD_DISPLAY);
}

void CWedPolygon::OnFlag4() 
{
	wedpolygon->flags^=8;	
  UpdateData(UD_DISPLAY);
}

void CWedPolygon::OnFlag5() 
{
	wedpolygon->flags^=16;	
  UpdateData(UD_DISPLAY);
}

void CWedPolygon::OnFlag6() 
{
	wedpolygon->flags^=32;	
  UpdateData(UD_DISPLAY);
}

void CWedPolygon::OnFlag7() 
{
	wedpolygon->flags^=64;	
  UpdateData(UD_DISPLAY);
}

void CWedPolygon::OnFlag8() 
{
	wedpolygon->flags^=128;	
  UpdateData(UD_DISPLAY);
}

void CWedPolygon::OnKillfocusFlags() 
{
  UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CWedPolygon::OnKillfocusUnknown() 
{
  UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CWedPolygon::OnKillfocusMinx() 
{
  UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CWedPolygon::OnKillfocusMiny() 
{
  UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CWedPolygon::OnKillfocusMaxx() 
{
  UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CWedPolygon::OnKillfocusMaxy() 
{
  UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CWedPolygon::OnKillfocusVertexpicker() 
{
	m_vertexnum=m_vertexpicker.GetCurSel();
	UpdateData(UD_DISPLAY);
}

void CWedPolygon::OnSelchangeVertexpicker() 
{
	m_vertexnum=m_vertexpicker.GetCurSel();
	UpdateData(UD_DISPLAY);
}

void CWedPolygon::OnKillfocusPosx() 
{
  UpdateData(UD_RETRIEVE);
  RefreshPolygon();
	UpdateData(UD_DISPLAY);
}

void CWedPolygon::OnKillfocusPosy() 
{
  UpdateData(UD_RETRIEVE);
  RefreshPolygon();
	UpdateData(UD_DISPLAY);
}

void CWedPolygon::OnRecalcbox() 
{
	the_area.RecalcBox(0,wedpolygon,wedvertex,-1);	
  RefreshPolygon();
  UpdateData(UD_DISPLAY);
}

void CWedPolygon::OnReverse() 
{
  int i;
  area_vertex tmp;

	for(i=0;i<wedpolygon->countvertex/2;i++)
  {
    tmp=wedvertex[i];
    wedvertex[i]=wedvertex[wedpolygon->countvertex-i-1];
    wedvertex[wedpolygon->countvertex-i-1]=tmp;
  }
  RefreshPolygon();
  UpdateData(UD_DISPLAY);
}

void CWedPolygon::OnOrder() 
{
  the_area.VertexOrder(wedvertex, wedpolygon->countvertex);
  RefreshPolygon();
  UpdateData(UD_DISPLAY);
}

void CWedPolygon::OnShift() 
{
  area_vertex *tmp;
  int i;

  tmp=new area_vertex[wedpolygon->countvertex];
  if(!tmp)
  {
    return;
  }
  for(i=0;i<wedpolygon->countvertex;i++)
  {
    tmp[i]=wedvertex[(i+1)%wedpolygon->countvertex];
  }
  memcpy(wedvertex,tmp,wedpolygon->countvertex*sizeof(area_vertex) );
  delete [] tmp;
  RefreshPolygon();
  UpdateData(UD_DISPLAY);
}
