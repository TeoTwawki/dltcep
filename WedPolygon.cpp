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
	m_open = FALSE;
	m_insertpoint = FALSE;
	m_movepolygon = FALSE;
	//}}AFX_DATA_INIT
  m_vertexnum=-1;
  m_changed=0;
	m_graphics = false;
}

static int vertexboxids[]={IDC_POSX, IDC_POSY,IDC_REVERSE, IDC_ORDER,
0};

void CWedPolygon::SetPolygon(int type, int number)
{
  m_preview.SetMapType(type, (LPBYTE) &the_area.wedvertexheaderlist);
  m_polynum=number;
  if(type==MT_DOORPOLYLIST)
  {      
    wedpolygon=the_area.doorpolygonheaders+number;
    pos=the_area.wedvertexheaderlist.FindIndex(m_polynum+the_area.wallpolygoncount);
  }
  else
  {
    wedpolygon=the_area.wallpolygonheaders+number;
    pos=the_area.wedvertexheaderlist.FindIndex(m_polynum);
  }
  wedvertex=(area_vertex *) the_area.vertexheaderlist.GetAt(pos);
}

void CWedPolygon::RefreshPolygon()
{
  CString tmpstr;
  CButton *cb;
  int i,j;  

  if(!m_vertexpicker) return;
  m_vertexpicker.ResetContent();
  for(i=0;i<wedpolygon->countvertex;i++)
  {
    tmpstr.Format("%d  [%d.%d]",i+1, wedvertex[i].x,wedvertex[i].y);
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
  cb = (CButton *) GetDlgItem(IDC_OPEN);
  if(m_open) cb->SetWindowText("Draw open");
  else cb->SetWindowText("Draw closed");
  the_mos.m_drawclosed=!m_open;
  
  if(m_preview.m_bm)
  {
    DeleteObject(m_preview.m_bm);
    m_preview.m_bm=0;
  }
  m_preview.m_clipx=wedvertex[m_vertexnum].x/the_mos.mosheader.dwBlockSize-m_preview.m_maxextentx/2;
  m_preview.m_clipy=wedvertex[m_vertexnum].y/the_mos.mosheader.dwBlockSize-m_preview.m_maxextenty/2;
  if(m_graphics)
  {
    m_preview.m_polygon=wedvertex;
    m_preview.m_vertexcount=wedpolygon->countvertex;
    m_preview.m_actvertex=m_vertexnum;
    m_preview.RedrawContent();
  }
  m_preview.ShowWindow(m_graphics);
}

void CWedPolygon::RefreshVertex()
{
  area_vertex *newvertices;
  CPoint point;
  int i;

  if(!m_preview) return;
  m_changed=1;
  point=m_preview.GetPoint(GP_POINT);
  if(m_insertpoint)
  {
    m_vertexnum++;
    newvertices=new area_vertex[wedpolygon->countvertex+1];
    if(!newvertices)
    {
      MessageBox("Out of memory", "Area editor",MB_ICONSTOP|MB_OK);
      return;
    }
    memcpy(newvertices, wedvertex, sizeof(area_vertex)*m_vertexnum);
    memcpy(newvertices+m_vertexnum+1, wedvertex+m_vertexnum, sizeof(area_vertex)*(wedpolygon->countvertex-m_vertexnum) );
    the_area.vertexheaderlist.SetAt(pos,newvertices); //this will also free up the old wedvertex
    wedpolygon->countvertex++;
    wedvertex=newvertices;
  }
  else
  {
    if(m_vertexnum<0) return;
    if(m_movepolygon)
    {
      point.x-=wedvertex[m_vertexnum].x;
      point.y-=wedvertex[m_vertexnum].y;
      if(CanMove(point, wedvertex, wedpolygon->countvertex) )
      {
        MessageBox("The polygon would reach over the map boundaries","Area editor",MB_ICONWARNING|MB_OK);
      }
      for(i=0;i<wedpolygon->countvertex;i++)
      {
        wedvertex[i].x=(unsigned short) (wedvertex[i].x+point.x);
        wedvertex[i].y=(unsigned short) (wedvertex[i].y+point.y);
      }
      UpdateData(UD_DISPLAY);
      OnRecalcbox();
      return;
    }
  }
  wedvertex[m_vertexnum].x=(short) point.x;
  wedvertex[m_vertexnum].y=(short) point.y;
  UpdateData(UD_DISPLAY);
  OnRecalcbox();
}

void CWedPolygon::DoDataExchange(CDataExchange* pDX)
{
  int tmp;

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWedPolygon)
	DDX_Control(pDX, IDC_VERTEXPICKER, m_vertexpicker);
	DDX_Check(pDX, IDC_OPEN, m_open);
	DDX_Check(pDX, IDC_INSERT, m_insertpoint);
	DDX_Check(pDX, IDC_MOVE, m_movepolygon);
	//}}AFX_DATA_MAP

  tmp=wedpolygon->flags;
  DDX_Text(pDX, IDC_FLAGS, wedpolygon->flags);
  if(tmp!=wedpolygon->flags) m_changed=1;
  tmp=wedpolygon->unkflags;
  DDX_Text(pDX, IDC_UNKNOWN, wedpolygon->unkflags);
  if(tmp!=wedpolygon->unkflags) m_changed=1;
  DDX_Text(pDX, IDC_MINX, wedpolygon->minx);
  DDX_Text(pDX, IDC_MINY, wedpolygon->miny);
  DDX_Text(pDX, IDC_MAXX, wedpolygon->maxx);
  DDX_Text(pDX, IDC_MAXY, wedpolygon->maxy);
  if(m_vertexnum>=0)
  {
    DDX_Text(pDX, IDC_POSX, (short &) wedvertex[m_vertexnum].x);
    DDX_Text(pDX, IDC_POSY, (short &) wedvertex[m_vertexnum].y);
  }
}

BOOL CWedPolygon::OnInitDialog() 
{
  CRect rect;

	CDialog::OnInitDialog();
  if( SetupSelectPoint() )
  {
    return FALSE;
  }

  m_preview.m_value=-1;
  m_preview.InitView(IW_SHOWALL|IW_SHOWGRID|IW_SETVERTEX|IW_ENABLEFILL, &the_mos); //initview must be before create
  m_preview.Create(IDD_IMAGEVIEW,this);
  GetWindowRect(rect);
  m_preview.SetWindowPos(0,rect.right,rect.top,0,0,SWP_NOZORDER|SWP_HIDEWINDOW|SWP_NOSIZE);
  RefreshPolygon();
  //tooltips
  {
    m_tooltip.Create(this,TTS_NOPREFIX);
    m_tooltip.SetMaxTipWidth(200);
    m_tooltip.SetTipBkColor(RGB(240,224,160));
    
    m_tooltip.AddTool(GetDlgItem(IDOK), IDS_CANCEL);
  }
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
	ON_BN_CLICKED(IDC_PREVIEW, OnPreview)
	ON_BN_CLICKED(IDC_OPEN, OnOpen)
	ON_LBN_DBLCLK(IDC_VERTEXPICKER, OnDblclkVertexpicker)
	ON_BN_CLICKED(IDC_INSERT, OnInsert)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SHIFT, OnDeltaposShift)
	ON_BN_CLICKED(IDC_MOVE, OnMove)
	//}}AFX_MSG_MAP
ON_COMMAND(ID_REFRESH, RefreshVertex)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWedPolygon message handlers

void CWedPolygon::OnFlag1() 
{
	wedpolygon->flags^=1;	
  m_changed=1;
  UpdateData(UD_DISPLAY);
}

void CWedPolygon::OnFlag2() 
{
	wedpolygon->flags^=2;	
  m_changed=1;
  UpdateData(UD_DISPLAY);
}

void CWedPolygon::OnFlag3() 
{
	wedpolygon->flags^=4;	
  m_changed=1;
  UpdateData(UD_DISPLAY);
}

void CWedPolygon::OnFlag4() 
{
	wedpolygon->flags^=8;	
  m_changed=1;
  UpdateData(UD_DISPLAY);
}

void CWedPolygon::OnFlag5() 
{
	wedpolygon->flags^=16;	
  m_changed=1;
  UpdateData(UD_DISPLAY);
}

void CWedPolygon::OnFlag6() 
{
	wedpolygon->flags^=32;	
  m_changed=1;
  UpdateData(UD_DISPLAY);
}

void CWedPolygon::OnFlag7() 
{
	wedpolygon->flags^=64;	
  m_changed=1;
  UpdateData(UD_DISPLAY);
}

void CWedPolygon::OnFlag8() 
{
	wedpolygon->flags^=128;	
  m_changed=1;
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
  if(m_vertexnum>=0)
  {
    RefreshPolygon();
  }
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
	the_area.RecalcBox(0,wedpolygon,wedvertex);	
  RefreshPolygon();
  UpdateData(UD_DISPLAY);
}

void CWedPolygon::OnReverse() 
{
  int i;
  area_vertex tmp;

  m_changed=1;
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
  if(VertexOrder(wedvertex, wedpolygon->countvertex))
  {
    m_changed=1;
  }
  RefreshPolygon();
  UpdateData(UD_DISPLAY);
}

void CWedPolygon::Shift(int where) 
{
  area_vertex *tmp;
  int i;

  if(where<0) where=wedpolygon->countvertex-1;
  if(where<=0) return;
  m_changed=1;
  tmp=new area_vertex[wedpolygon->countvertex];
  if(!tmp)
  {
    return;
  }
  for(i=0;i<wedpolygon->countvertex;i++)
  {
    tmp[i]=wedvertex[(i+where)%wedpolygon->countvertex];
  }
  memcpy(wedvertex,tmp,wedpolygon->countvertex*sizeof(area_vertex) );
  delete [] tmp;
  RefreshPolygon();
  UpdateData(UD_DISPLAY);
}

void CWedPolygon::OnPreview() 
{
  m_graphics=!m_graphics;
  RefreshPolygon();
}

void CWedPolygon::OnOpen() 
{
  UpdateData(UD_RETRIEVE);
  RefreshPolygon(); //redraws preview
}

void CWedPolygon::OnDblclkVertexpicker() 
{
  area_vertex *newvertices;
  
  m_changed=1;
  m_vertexnum=m_vertexpicker.GetCurSel();
  if(m_vertexnum<0) return;
  newvertices=new area_vertex[--wedpolygon->countvertex];
  if(!newvertices)
  {
    MessageBox("Out of memory", "Area editor",MB_ICONSTOP|MB_OK);
    wedpolygon->countvertex++;
    return;
  }
  memcpy(newvertices, wedvertex, sizeof(area_vertex)*m_vertexnum);
  memcpy(newvertices+m_vertexnum, wedvertex+m_vertexnum+1, sizeof(area_vertex)*(wedpolygon->countvertex-m_vertexnum) );
  the_area.vertexheaderlist.SetAt(pos,newvertices); //this will also free up the old wedvertex
  wedvertex=newvertices;
  RefreshPolygon();
}

void CWedPolygon::OnInsert() 
{
  UpdateData(UD_RETRIEVE);
  if(m_insertpoint)
  {
    m_movepolygon=FALSE;
  }
  UpdateData(UD_DISPLAY);
}

void CWedPolygon::OnMove() 
{
  UpdateData(UD_RETRIEVE);
  if(m_movepolygon)
  {
    m_insertpoint=FALSE;
  }
  UpdateData(UD_DISPLAY);
}

void CWedPolygon::OnDeltaposShift(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

  if(pNMUpDown->iDelta>0) Shift(-1);
  else Shift(1);
	*pResult = 0;
}

BOOL CWedPolygon::PreTranslateMessage(MSG* pMsg) 
{
	m_tooltip.RelayEvent(pMsg);	
	return CDialog::PreTranslateMessage(pMsg);
}
