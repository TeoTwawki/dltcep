// Polygon.cpp : implementation file
//

#include "stdafx.h"

#include <fcntl.h>
#include <sys/stat.h>
#include "chitem.h"
#include "Polygon.h"
#include "options.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPolygon dialog

CPolygon::CPolygon(CWnd* pParent /*=NULL*/)
	: CDialog(CPolygon::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPolygon)
	m_open = FALSE;
	m_insertpoint = FALSE;
	m_movepolygon = FALSE;
	//}}AFX_DATA_INIT
  m_vertexnum=-1;
  m_changed=0;
	m_graphics = false;
  m_polynum=0;
}

void CPolygon::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPolygon)
	DDX_Control(pDX, IDC_VERTEXPICKER, m_vertexpicker);
	DDX_Check(pDX, IDC_OPEN, m_open);
	DDX_Check(pDX, IDC_INSERT, m_insertpoint);
	DDX_Check(pDX, IDC_MOVE, m_movepolygon);
	//}}AFX_DATA_MAP
  DDX_Text(pDX, IDC_MINX, bbox[BBMINX]);
  DDX_Text(pDX, IDC_MINY, bbox[BBMINY]);
  DDX_Text(pDX, IDC_MAXX, bbox[BBMAXX]);
  DDX_Text(pDX, IDC_MAXY, bbox[BBMAXY]);
  if(m_vertexnum>=0)
  {
    DDX_Text(pDX, IDC_POSX, (short &) m_polygon[m_vertexnum].x);
    DDX_Text(pDX, IDC_POSY, (short &) m_polygon[m_vertexnum].y);
  }
}

void CPolygon::SetPolygon(int type, int index, short *cv, short *bb)
{
  m_preview.SetMapType(type, (LPBYTE) &the_area.vertexheaderlist); 
  m_polynum=index;
  pos=the_area.vertexheaderlist.FindIndex(index);
  countvertex=cv;
  bbox=bb;
  m_polygon=(area_vertex *) the_area.vertexheaderlist.GetAt(pos);
}

static int vertexboxids[]={IDC_POSX, IDC_POSY,IDC_REVERSE, IDC_ORDER,
0};

void CPolygon::RefreshPolygon(bool repos)
{
  CString tmpstr;
  CButton *cb;
  int i;  

  if(!m_vertexpicker) return;
  m_vertexpicker.ResetContent();
  for(i=0;i<*countvertex;i++)
  {
    tmpstr.Format("%d  [%d.%d]",i+1, m_polygon[i].x,m_polygon[i].y);
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

  cb = (CButton *) GetDlgItem(IDC_OPEN);
  if(m_open) cb->SetWindowText("Draw open");
  else cb->SetWindowText("Draw closed");
  the_mos.m_drawclosed=!m_open;
  
  if(m_preview.m_bm)
  {
    DeleteObject(m_preview.m_bm);
    m_preview.m_bm=0;
  }
  if(repos && m_vertexnum>=0)
  {
    m_preview.m_clipx=m_polygon[m_vertexnum].x/the_mos.mosheader.dwBlockSize-m_preview.m_maxextentx/2;
    m_preview.m_clipy=m_polygon[m_vertexnum].y/the_mos.mosheader.dwBlockSize-m_preview.m_maxextenty/2;
  }
  if(m_graphics)
  {
    m_preview.m_polygon=m_polygon;
    m_preview.m_vertexcount=*countvertex;
    m_preview.m_actvertex=m_vertexnum;
    m_preview.RedrawContent();
  }
  m_preview.ShowWindow(m_graphics);
}

void CPolygon::FixPolygon(CPoint &point)
{  
  int i;

  if(CanMove(point, m_polygon, *countvertex) )
  {
    MessageBox("The polygon would reach over the map boundaries","Area editor",MB_ICONWARNING|MB_OK);
  }
  for(i=0;i<(*countvertex);i++)
  {
    m_polygon[i].x=(unsigned short) (m_polygon[i].x+point.x);
    m_polygon[i].y=(unsigned short) (m_polygon[i].y+point.y);
  }
  RecalcBox((int) (*countvertex),m_polynum,(POINTS &) (bbox[BBMINX]),(POINTS &) (bbox[BBMAXX]) );
}

void CPolygon::RefreshVertex()
{
  area_vertex *newvertices;
  CPoint point;

  if(!m_preview.m_hWnd) return;
  m_changed=1;
  point=m_preview.GetPoint(GP_POINT);
  if(m_insertpoint)
  {
    m_vertexnum++;
    newvertices=new area_vertex[(*countvertex)+1];
    if(!newvertices)
    {
      MessageBox("Out of memory", "Area editor",MB_ICONSTOP|MB_OK);
      return;
    }
    memcpy(newvertices, m_polygon, sizeof(area_vertex)*m_vertexnum);
    memcpy(newvertices+m_vertexnum+1, m_polygon+m_vertexnum, sizeof(area_vertex)*((*countvertex)-m_vertexnum) );
    the_area.vertexheaderlist.SetAt(pos,newvertices); //this will also free up the old wedvertex
    (*countvertex)++;
    m_polygon=newvertices;
  }
  else
  {
    if(m_vertexnum<0) return;
    if(m_movepolygon)
    {
      point.x-=m_polygon[m_vertexnum].x;
      point.y-=m_polygon[m_vertexnum].y;
      FixPolygon(point);
      RefreshPolygon(!(editflg&CENTER));
      UpdateData(UD_DISPLAY);
      return;
    }
  }
  m_polygon[m_vertexnum].x=(short) point.x;
  m_polygon[m_vertexnum].y=(short) point.y;
	RecalcBox((int) (*countvertex),m_polynum,(POINTS &) (bbox[BBMINX]),(POINTS &) (bbox[BBMAXX]) );
  RefreshPolygon(!(editflg&CENTER));
  UpdateData(UD_DISPLAY);
}

BEGIN_MESSAGE_MAP(CPolygon, CDialog)
	//{{AFX_MSG_MAP(CPolygon)
	ON_LBN_SELCHANGE(IDC_VERTEXPICKER, OnSelchangeVertexpicker)
	ON_LBN_DBLCLK(IDC_VERTEXPICKER, OnDblclkVertexpicker)
	ON_LBN_KILLFOCUS(IDC_VERTEXPICKER, OnKillfocusVertexpicker)
	ON_BN_CLICKED(IDC_PREVIEW, OnPreview)
	ON_BN_CLICKED(IDC_RECALCBOX, OnRecalcbox)
	ON_BN_CLICKED(IDC_INSERT, OnInsert)
	ON_BN_CLICKED(IDC_ORDER, OnOrder)
	ON_BN_CLICKED(IDC_REVERSE, OnReverse)
	ON_BN_CLICKED(IDC_SHIFT, OnShift)
	ON_BN_CLICKED(IDC_MOVE, OnMove)
	ON_BN_CLICKED(IDC_OPEN, OnOpen)
	ON_EN_KILLFOCUS(IDC_POSX, OnKillfocusPosx)
	ON_EN_KILLFOCUS(IDC_POSY, OnKillfocusPosy)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	//}}AFX_MSG_MAP
ON_COMMAND(ID_REFRESH, RefreshVertex)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPolygon message handlers

BOOL CPolygon::OnInitDialog() 
{
  CRect rect;

	CDialog::OnInitDialog();
  if( SetupSelectPoint(0) )
  {
    OnCancel();
    return FALSE;
  }
  //tooltips
  {
    m_tooltip.Create(this,TTS_NOPREFIX);
    m_tooltip.SetMaxTipWidth(200);
    m_tooltip.SetTipBkColor(RGB(240,224,160));
    
    m_tooltip.AddTool(GetDlgItem(IDOK), IDS_CANCEL);
  }

  m_preview.m_value=-1;
  m_preview.InitView(IW_SHOWALL|IW_SHOWGRID|IW_SETVERTEX|IW_ENABLEFILL, &the_mos); //initview must be before create
  m_preview.Create(IDD_IMAGEVIEW,this);
  GetWindowRect(rect);
  m_preview.SetWindowPos(0,rect.right,rect.top,0,0,SWP_NOZORDER|SWP_HIDEWINDOW|SWP_NOSIZE);

  RefreshPolygon(true);
  UpdateData(UD_DISPLAY);
  return TRUE;
}

void CPolygon::OnSelchangeVertexpicker() 
{
	m_vertexnum=m_vertexpicker.GetCurSel();
  if(m_vertexnum>=0)
  {
    RefreshPolygon(true);
  }
	UpdateData(UD_DISPLAY);
}

void CPolygon::OnDblclkVertexpicker() 
{
  area_vertex *newvertices;
  
  m_changed=1;
  m_vertexnum=m_vertexpicker.GetCurSel();
  if(m_vertexnum<0) return;
  newvertices=new area_vertex[--(*countvertex)];
  if(!newvertices)
  {
    MessageBox("Out of memory", "Area editor",MB_ICONSTOP|MB_OK);
    (*countvertex)++;
    return;
  }
  memcpy(newvertices, m_polygon, sizeof(area_vertex)*m_vertexnum);
  memcpy(newvertices+m_vertexnum, m_polygon+m_vertexnum+1, sizeof(area_vertex)*((*countvertex)-m_vertexnum) );
  the_area.vertexheaderlist.SetAt(pos,newvertices); //this will also free up the old wedvertex
  m_polygon=newvertices;
  RefreshPolygon(true);
}

void CPolygon::OnKillfocusVertexpicker() 
{
	m_vertexnum=m_vertexpicker.GetCurSel();
	UpdateData(UD_DISPLAY);
}

void CPolygon::OnPreview() 
{
  m_graphics=!m_graphics;
  RefreshPolygon(true);
}

void CPolygon::OnRecalcbox() 
{
	RecalcBox((int) (*countvertex),m_polynum,(POINTS &) (bbox[BBMINX]),(POINTS &) (bbox[BBMAXX]) );
  RefreshPolygon(true);
  UpdateData(UD_DISPLAY);
}

void CPolygon::OnInsert() 
{
  UpdateData(UD_RETRIEVE);
  if(m_insertpoint)
  {
    m_movepolygon=FALSE;
  }
  UpdateData(UD_DISPLAY);
}

void CPolygon::OnMove() 
{
  UpdateData(UD_RETRIEVE);
  if(m_movepolygon)
  {
    m_insertpoint=FALSE;
  }
  UpdateData(UD_DISPLAY);
}

void CPolygon::OnOrder() 
{
  if(VertexOrder(m_polygon, *countvertex))
  {
    m_changed=1;
  }
  RefreshPolygon(true);
  UpdateData(UD_DISPLAY);
}

void CPolygon::OnReverse() 
{
  int i;
  area_vertex tmp;

  m_changed=1;
	for(i=0;i<(*countvertex)/2;i++)
  {
    tmp=m_polygon[i];
    m_polygon[i]=m_polygon[(*countvertex)-i-1];
    m_polygon[(*countvertex)-i-1]=tmp;
  }
  RefreshPolygon(true);
  UpdateData(UD_DISPLAY);
}

void CPolygon::OnShift() 
{
  area_vertex *tmp;
  int i;

  m_changed=1;
  tmp=new area_vertex[*countvertex];
  if(!tmp)
  {
    return;
  }
  for(i=0;i<*countvertex;i++)
  {
    tmp[i]=m_polygon[(i+1)%(*countvertex)];
  }
  memcpy(m_polygon,tmp,(*countvertex)*sizeof(area_vertex) );
  delete [] tmp;
  RefreshPolygon(true);
  UpdateData(UD_DISPLAY);
}

void CPolygon::OnOpen() 
{
  UpdateData(UD_RETRIEVE);
  RefreshPolygon(true); //redraws preview
}

void CPolygon::OnKillfocusPosx() 
{
  UpdateData(UD_RETRIEVE);
  RefreshPolygon(true);
	UpdateData(UD_DISPLAY);
}

void CPolygon::OnKillfocusPosy() 
{
  UpdateData(UD_RETRIEVE);
  RefreshPolygon(true);
	UpdateData(UD_DISPLAY);
}

static char BASED_CODE szFilter[] = "Polygon files (*.ply)|*.ply|All files (*.*)|*.*||";

void CPolygon::OnSave() 
{
  CString filepath, newname;
  int res;
  int fhandle;

  res=OFN_HIDEREADONLY|OFN_ENABLESIZING|OFN_EXPLORER;
  CFileDialog m_getfiledlg(FALSE, "ply", makeitemname(".ply",0), res, szFilter);

  if( m_getfiledlg.DoModal() == IDOK )
  {
    filepath=m_getfiledlg.GetPathName();
    filepath.MakeLower();
    if(filepath.Right(4)!=".ply")
    {
      filepath+=".ply";
    }
    fhandle=open(filepath, O_BINARY|O_RDWR|O_CREAT|O_TRUNC,S_IREAD|S_IWRITE);
    res = WritePolygon(fhandle, &m_polygon, countvertex);
    close(fhandle);
  }
}

void CPolygon::OnLoad() 
{
  area_vertex *newvertices;
  CString filepath;
  int fhandle;
  int res;
  
  res=OFN_FILEMUSTEXIST|OFN_ENABLESIZING|OFN_EXPLORER;
  if(readonly) res|=OFN_READONLY;  
  CFileDialog m_getfiledlg(TRUE, "ply", makeitemname(".ply",0), res, szFilter);

restart:  
  if( m_getfiledlg.DoModal() == IDOK )
  {
    filepath=m_getfiledlg.GetPathName();
    fhandle=open(filepath, O_RDONLY|O_BINARY);
    if(!fhandle)
    {
      MessageBox("Cannot open file!","Error",MB_ICONSTOP|MB_OK);
      goto restart;
    }
    the_area.m_night=false;
    readonly=m_getfiledlg.GetReadOnlyPref();
    newvertices=NULL;
    res=ReadPolygon(fhandle, &newvertices, countvertex);
    if(newvertices)
    {
      if(res)
      {
        delete [] newvertices;
      }
      else
      {
        the_area.vertexheaderlist.SetAt(pos,newvertices); //this will also free up the old wedvertex
        m_polygon=newvertices;
      }
    }
    close(fhandle);    
  }
  if(countvertex)
  {
    CPoint point;
    point.x=0;
    point.y=0;
    FixPolygon(point);
  }
  RefreshPolygon(true);
	UpdateData(UD_DISPLAY);
}

BOOL CPolygon::PreTranslateMessage(MSG* pMsg) 
{
	m_tooltip.RelayEvent(pMsg);	
	return CDialog::PreTranslateMessage(pMsg);
}
