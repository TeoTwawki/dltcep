// ImageView.cpp : implementation file
//

#include "stdafx.h"
#include <math.h>
#include "chitem.h"
#include "ImageView.h"
#include "options.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CImageView dialog

CImageView::CImageView(CWnd* pParent /*=NULL*/) : CDialog(CImageView::IDD, pParent)
{
	//{{AFX_DATA_INIT(CImageView)
	m_value = 0;
	m_showall = FALSE;
	m_showgrid = FALSE;
	m_fill = FALSE;
	//}}AFX_DATA_INIT
  m_max=15;
  m_bm=0;
  m_clipx=0;
  m_clipy=0;
  m_animbam=NULL;
  m_map=NULL;
  m_palette=NULL;
  m_maxextentx=m_maxextenty=8;//maximum number of tiles fitting on screen
}

void CImageView::DoDataExchange(CDataExchange* pDX)
{
  CString tmpstr;

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CImageView)
	DDX_Control(pDX, IDC_FILL, m_fill_control);
	DDX_Control(pDX, IDC_SHOWGRID, m_showgrid_control);
	DDX_Control(pDX, IDC_VALUE, m_value_control);
	DDX_Control(pDX, IDC_SHOWALL, m_showall_control);
	DDX_Control(pDX, IDC_SPINY, m_spiny);
	DDX_Control(pDX, IDC_SPINX, m_spinx);
	DDX_Control(pDX, IDOK, m_setbutton);
	DDX_Control(pDX, IDC_VERTICAL, m_vertical);
	DDX_Control(pDX, IDC_HORIZONTAL, m_horizontal);
	DDX_Control(pDX, IDCANCEL, m_button);
	DDX_Control(pDX, IDC_BITMAP, m_bitmap);
	DDX_Check(pDX, IDC_SHOWALL, m_showall);
	DDX_Check(pDX, IDC_SHOWGRID, m_showgrid);
	DDX_Check(pDX, IDC_FILL, m_fill);
	//}}AFX_DATA_MAP
  
  tmpstr=GetMapTypeValue(m_maptype,m_value);
	DDX_Text(pDX, IDC_VALUE, tmpstr);
  m_value=strtonum(tmpstr);

	DDV_MinMaxInt(pDX, m_value, 0, m_max);
	DDX_Scroll(pDX, IDC_HORIZONTAL, m_clipx);
	DDX_Scroll(pDX, IDC_VERTICAL, m_clipy);
}

BEGIN_MESSAGE_MAP(CImageView, CDialog)
	//{{AFX_MSG_MAP(CImageView)
	ON_BN_CLICKED(IDC_BITMAP, OnBitmap)
	ON_WM_MOUSEMOVE()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPINX, OnDeltaposSpinx)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPINY, OnDeltaposSpiny)
	ON_CBN_KILLFOCUS(IDC_VALUE, OnKillfocusValue)
	ON_BN_CLICKED(IDC_SHOWALL, OnShowall)
	ON_CBN_SELCHANGE(IDC_VALUE, OnSelchangeValue)
	ON_BN_CLICKED(IDC_SHOWGRID, OnShowgrid)
	ON_BN_CLICKED(IDC_FILL, OnFill)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_REFRESH, RefreshDialog)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImageView message handlers

void CImageView::PutPixel(CPoint point, unsigned char color)
{
  m_map[point.x+point.y*(the_area.width/GR_WIDTH)]=(unsigned char) color;
}

//intentionally int, so we can return -1 when coordinates are out of map
int CImageView::GetPixel(CPoint point)
{
  if(point.x<0 || point.y<0) return -1;
  if(point.x>=the_area.width/GR_WIDTH) return -1;
  if(point.y>=the_area.height/GR_HEIGHT) return -1;
  return m_map[point.x+point.y*(the_area.width/GR_WIDTH)];
}

void CImageView::InitView(int flags)
{
  m_oladjust=0;
  m_animbam=NULL;
  m_confirmed=m_mousepoint=0;
  m_clipx=m_minclipx=0;
  m_clipy=m_minclipy=0;
  m_maxclipx=the_mos.mosheader.wColumn;
  if(m_maxclipx<0) m_maxclipx=0;
  m_maxclipy=the_mos.mosheader.wRow;
  if(m_maxclipy<0) m_maxclipy=0;
  m_enablebutton=flags;
}

void CImageView::SetMapType(int maptype, LPBYTE map)
{
  m_maptype=maptype;
  m_map=map;
  switch(maptype)
  {
  case 0:
    m_palette=the_area.htpal;
    break;
  case 1:
    m_palette=the_area.lmpal;
    break;
  case 2:
    m_palette=the_area.srpal;
    break;
  }
  if(maptype==1) m_max=255;
  else m_max=15;
}

#define TMPX (m_maxextentx/2)
#define TMPY (m_maxextenty/2)

void CImageView::SetupAnimationPlacement(Cbam *bam, int orgx, int orgy)
{
  CPoint point;
  int tmpw;

  point=bam->GetFrameSize(0);
  point.x/=the_mos.mosheader.dwBlockSize;
  point.y/=the_mos.mosheader.dwBlockSize;
  tmpw=min(TMPX-point.x,TMPY-point.y);
  if(tmpw<0) tmpw=0;
  m_animbam=bam;
  point=bam->GetFramePos(0);
  m_clipx=(orgx-point.x)/(signed) the_mos.mosheader.dwBlockSize;
  m_clipy=(orgy-point.y)/(signed) the_mos.mosheader.dwBlockSize;
  if(m_clipx>m_maxclipx-m_maxextentx)
  {
    if(m_maxclipx>=m_maxextentx) m_clipx=m_maxclipx-m_maxextentx;
    else m_clipx=0;
  }
  else
  {
    if(m_clipx>tmpw) m_clipx-=tmpw;
    else m_clipx=0;
  }
  if(m_clipy>m_maxclipy-m_maxextenty)
  {
    if(m_maxclipy>=m_maxextenty) m_clipy=m_maxclipy-m_maxextenty;
    else m_clipy=0;
  }
  else
  {
    if(m_clipy>tmpw) m_clipy-=tmpw;
    else m_clipy=0;
  }
  m_mousepoint.x=orgx-m_clipx*the_mos.mosheader.dwBlockSize;
  m_mousepoint.y=orgy-m_clipy*the_mos.mosheader.dwBlockSize;
  m_confirmed=m_mousepoint;
  m_oladjust=point;
}

void CImageView::DrawGrid()
{
  int i,j;
  int xs, ys;
  int sx, sy;

  if(m_enablebutton&IW_EDITMAP)
  {
    xs=GR_WIDTH; ys=GR_HEIGHT; //x and y sizes
    sx=m_clipx*the_mos.mosheader.dwBlockSize%xs; //x and y starting points for grids
    sy=m_clipy*the_mos.mosheader.dwBlockSize%ys;
  }
  else
  {
    xs=64; ys=64;
    sx=0; sy=0;
  }
  CDC *pDC = m_bitmap.GetDC();
	CDC dcBmp;
  dcBmp.CreateCompatibleDC(pDC);
  
  //selecting the bitmap for drawing
  CBitmap *tmp=dcBmp.SelectObject(CBitmap::FromHandle(m_bitmap.GetBitmap()));
  //setting a green color
  dcBmp.SetBkMode(1);
  dcBmp.SetTextColor(RGB(255,255,255) );
  CPen mypen(PS_SOLID,1,RGB(0,255,0) );
  dcBmp.SelectObject(&mypen);

  CRect cr;
  CString tmpstr;
  m_bitmap.GetClientRect(cr);
  
  if(m_showgrid)
  {
    for(i=sx;i<cr.Width();i+=xs)
    {
      dcBmp.MoveTo(i,0); dcBmp.LineTo(i,cr.Height());
    }
  }
  for(j=sy;j<cr.Height();j+=ys)
  {
    if(m_showgrid)
    {
      dcBmp.MoveTo(0,j); dcBmp.LineTo(cr.Width(),j);
    }
    if(m_map)
    {
      int p=(m_clipy*the_mos.mosheader.dwBlockSize+j)/GR_HEIGHT*the_area.width/GR_WIDTH+m_clipx*the_mos.mosheader.dwBlockSize/GR_WIDTH;
      for(i=sx;i<cr.Width();i+=xs)
      {
        if(m_showall || (m_map[p]==m_value) )
        {
          tmpstr.Format("%d",m_map[p]);
          dcBmp.TextOut(i+2,j,tmpstr);
        }
        p++;
      }
    }
  }
 
  dcBmp.SelectObject(tmp);
  m_bitmap.ReleaseDC(pDC);
}

//returns a point (frame=0) or the tile (frame=1) clicked on
// frame==2 for editmap
CPoint CImageView::GetPoint(int frame)
{
  CPoint ret;

  switch(frame)
  {
  case 2:
    ret.x=m_confirmed.x+m_clipx*the_mos.mosheader.dwBlockSize;
    ret.y=m_confirmed.y+m_clipy*the_mos.mosheader.dwBlockSize;
    ret.x/=GR_WIDTH;
    ret.y/=GR_HEIGHT;
    break;
  case 1:
    ret.x=m_confirmed.x/the_mos.mosheader.dwBlockSize+m_clipx;
    ret.y=m_confirmed.y/the_mos.mosheader.dwBlockSize+m_clipy;
    break;
  case 0:
    ret.x=m_confirmed.x+m_clipx*the_mos.mosheader.dwBlockSize;
    ret.y=m_confirmed.y+m_clipy*the_mos.mosheader.dwBlockSize;
    break;
  }
  return ret;
}

void CImageView::RedrawContent()
{
  CRect rect;
  CRect brect;
  int adjustx, adjusty;
  int maxx, maxy;
  SCROLLINFO theScrollInfo;
  CPoint m_adjust;

  theScrollInfo.fMask=SIF_PAGE|SIF_POS|SIF_RANGE;
  if(m_maxclipx-m_minclipx>m_maxextentx)
  {
    theScrollInfo.nPage=(m_maxclipx-m_minclipx+m_maxextentx-1)/m_maxextentx;
    theScrollInfo.nPos=m_clipx;
    theScrollInfo.nMin=m_minclipx;
    theScrollInfo.nMax=m_maxclipx-m_maxextentx+theScrollInfo.nPage-1;
    m_horizontal.SetScrollInfo(&theScrollInfo,true);
    adjustx=20;
  }
  else
  {
    m_horizontal.ShowWindow(false);
    adjustx=0;
  }
  if(m_maxclipy-m_minclipy>m_maxextenty)
  {
    theScrollInfo.nPage=(m_maxclipy-m_minclipy+m_maxextenty-1)/m_maxextenty;
    theScrollInfo.nPos=m_clipy;
    theScrollInfo.nMin=m_minclipy;
    theScrollInfo.nMax=m_maxclipy-m_maxextenty+theScrollInfo.nPage-1;
    m_vertical.SetScrollInfo(&theScrollInfo,true);
    adjusty=20;
  }
  else
  {
    m_vertical.ShowWindow(false);
    adjusty=0;
  }
  if((m_enablebutton&(IW_ENABLEBUTTON|IW_NOREDRAW) )== IW_ENABLEBUTTON)
  {
    maxx=m_minclipx+m_maxextentx;
    maxy=m_minclipy+m_maxextenty;
    if(m_maxclipx<maxx) maxx=m_maxclipx;
    if(m_maxclipy<maxy) maxy=m_maxclipy;
    the_mos.MakeBitmapWhole(GREY, m_bm, m_minclipx, m_minclipy, maxx, maxy);
  }
  m_bitmap.SetBitmap(m_bm);
  if(m_bm)
  {
    m_bitmap.GetWindowRect(rect);
  }
  else
  {
    rect=0;
  }
  m_button.GetWindowRect(brect);
  m_adjust.x=10;
  m_adjust.y=adjustx+10;
  m_oladjust+=m_adjust;
  m_bitmap.SetWindowPos(0,m_adjust.x,m_adjust.y,0,0,SWP_NOSIZE|SWP_NOZORDER);
  if(rect.Width()<brect.Width() +20 ) rect.right=brect.Width()+20+rect.left;
  if(rect.Width()<400 && (m_enablebutton&IW_EDITMAP) )
  {
    rect.right=400+rect.left;
  }
  SetWindowPos(0, 0,0, rect.Width()+adjusty+20, rect.Height()+adjustx+80,SWP_NOACTIVATE|SWP_SHOWWINDOW|SWP_NOZORDER|SWP_NOMOVE);
  m_horizontal.SetWindowPos(0,0,0,rect.Width(),15,SWP_NOMOVE|SWP_NOZORDER);
  m_vertical.SetWindowPos(0,rect.Width()+10,adjustx+10,15,rect.Height(),SWP_NOZORDER);
  if(m_enablebutton&IW_PLACEIMAGE)
  {
    adjusty=(rect.Width()-brect.Width())/3;
    m_setbutton.SetWindowPos(0,adjusty,rect.Height()+adjustx+25 ,0,0, SWP_SHOWWINDOW|SWP_NOZORDER|SWP_NOSIZE);
    m_button.SetWindowPos(0,adjusty*2,rect.Height()+adjustx+25 ,0,0, SWP_SHOWWINDOW|SWP_NOZORDER|SWP_NOSIZE);
    m_spinx.SetWindowPos(0,adjusty*2+brect.Width()+25,rect.Height()+adjustx+25 ,0,0, SWP_SHOWWINDOW|SWP_NOZORDER|SWP_NOSIZE);
    m_spiny.SetWindowPos(0,adjusty*2+brect.Width()+60,rect.Height()+adjustx+25 ,0,0, SWP_SHOWWINDOW|SWP_NOZORDER|SWP_NOSIZE);
    m_spinx.SetRange32(0,rect.Width());
    m_spiny.SetRange32(0,rect.Height());
    m_spinx.SetPos(m_confirmed.x);
    m_spiny.SetPos(m_confirmed.y);
  }
  else
  {
    m_button.SetWindowPos(0, (rect.Width()-brect.Width())/2,rect.Height()+adjustx+25 ,0,0, SWP_SHOWWINDOW|SWP_NOZORDER|SWP_NOSIZE);
  }
  m_value_control.ShowWindow(!!(m_enablebutton&IW_EDITMAP));
  m_showall_control.ShowWindow(!!(m_enablebutton&IW_EDITMAP));
  m_showgrid_control.ShowWindow(!!(m_enablebutton&IW_EDITMAP));
  m_fill_control.ShowWindow(!!(m_enablebutton&IW_EDITMAP));
  if(m_enablebutton&IW_EDITMAP)
  {
    m_value_control.SetWindowPos(0,(rect.Width()-brect.Width())/4-20,rect.Height()+adjustx+25 ,0,0, SWP_SHOWWINDOW|SWP_NOZORDER|SWP_NOSIZE);
    m_showall_control.SetWindowPos(0,(rect.Width()-brect.Width())*3/4,rect.Height()+adjustx+25 ,0,0, SWP_SHOWWINDOW|SWP_NOZORDER|SWP_NOSIZE);
    m_showgrid_control.SetWindowPos(0,(rect.Width()-brect.Width())*3/4+70,rect.Height()+adjustx+25 ,0,0, SWP_SHOWWINDOW|SWP_NOZORDER|SWP_NOSIZE);
    m_fill_control.SetWindowPos(0,(rect.Width()-brect.Width())*3/4+140,rect.Height()+adjustx+25 ,0,0, SWP_SHOWWINDOW|SWP_NOZORDER|SWP_NOSIZE);
  }
  m_button.ShowWindow(!!(m_enablebutton&IW_ENABLEBUTTON));
  m_setbutton.ShowWindow(!!(m_enablebutton&IW_PLACEIMAGE));
  if(m_enablebutton&IW_ENABLEBUTTON)
  {
    RefreshDialog(); //need a redraw for selection of the bottom right side of the tis
  }
}

BOOL CImageView::OnInitDialog() 
{
  int i;

	CDialog::OnInitDialog();
  RedrawContent();
  for(i=0;i<=m_max;i++)
  {
    m_value_control.AddString(GetMapTypeValue(m_maptype, i));
  }
  if(m_map)
  {
    switch(m_maptype)
    {
    case 0: SetWindowText("Edit heightmap"); break;
    case 1: SetWindowText("Edit lightmap"); break;
    default: SetWindowText("Edit searchmap"); break;
    }
  }
	return TRUE;  
}

static CPoint DIR_UP(0,-1);
static CPoint DIR_DOWN(0,1);
static CPoint DIR_LEFT(-1,0);
static CPoint DIR_RIGHT(1,0);

void CImageView::FloodFill(CPoint point, unsigned char color)
{
  int color_to_fill;
  CPointList pointlist;
  CPoint nextpoint;

  color_to_fill=GetPixel(point);
  if(color_to_fill<0) return;
  if(color_to_fill==color) return; //nothing to do!
  do
  {
    PutPixel(point, color);
    nextpoint=point+DIR_UP;
    if(GetPixel(nextpoint)==color_to_fill)
    {
      pointlist.AddHead(nextpoint);
    }
    nextpoint=point+DIR_DOWN;
    if(GetPixel(nextpoint)==color_to_fill)
    {
      pointlist.AddHead(nextpoint);
    }
    nextpoint=point+DIR_LEFT;
    if(GetPixel(nextpoint)==color_to_fill)
    {
      pointlist.AddHead(nextpoint);
    }
    nextpoint=point+DIR_RIGHT;
    if(GetPixel(nextpoint)==color_to_fill)
    {
      pointlist.AddHead(nextpoint);
    }
    if(!pointlist.GetCount()) break;
    point=pointlist.RemoveHead();
  }
  while(1);
}

void CImageView::DrawLine(CPoint source, CPoint destination, unsigned char color)
{
  int startx, starty;
  //no need to put a dot on start
  int diffx=source.x-destination.x;
  int diffy=source.y-destination.y;
  if(abs(diffx)>=abs(diffy))
  {
    //vertical
    double elevationy =fabs(diffx)/diffy;
    if(source.x>destination.x)
    {
      for(startx=source.x;startx>destination.x;startx--)
      {
        PutPixel(CPoint(startx,source.y-(int) ((source.x-startx)/elevationy)),color );
      }
    }
    else
    {
      for(startx=source.x;startx<destination.x;startx++)
      {
        PutPixel(CPoint(startx,source.y+(int) ((source.x-startx)/elevationy)),color );
      }
    }    
  }
  else
  {
    double elevationx = fabs(diffy)/diffx;
    if(source.y>destination.y)
    {
      for(starty=source.y;starty>destination.y;starty--)
      {
        PutPixel(CPoint(source.x-(int) ((source.y-starty)/elevationx),starty),color );
      }
    }
    else
    {
      for(starty=source.y;starty<destination.y;starty++)
      {
        PutPixel(CPoint(source.x+(int) ((source.y-starty)/elevationx),starty),color );
      }
    }
  }
}

void CImageView::OnBitmap() 
{
  m_confirmed=m_mousepoint;
  if(m_enablebutton&(IW_SETVERTEX|IW_PLACEIMAGE|IW_EDITMAP))
  {
    if(m_enablebutton&IW_SETVERTEX)
    {
    }
    if(m_enablebutton&IW_PLACEIMAGE)
    {
      m_spinx.SetPos(m_confirmed.x);
      m_spiny.SetPos(m_confirmed.y);
    }
    if(m_map && (m_enablebutton&IW_EDITMAP) )
    {
      CPoint point;

      point=GetPoint(2); //getting map point
      if(m_fill)
      {
        the_area.WriteMap("TMP",m_map,m_palette,m_max+1);
        FloodFill(point, (unsigned char) m_value);
        m_fill=FALSE;
      }
      else
      {
        PutPixel(point, (unsigned char) m_value);
      }
      the_area.changedmap[m_maptype]=true;
    }
    RefreshDialog();
    return;
  }
  OnOK();	
}

void CImageView::OnMouseMove(UINT nFlags, CPoint point) 
{
	m_mousepoint=point;
  ScreenToClient(&m_mousepoint);
  CDialog::OnMouseMove(nFlags, point);
}

void CImageView::RefreshDialog()
{
  HBITMAP tmpbm;
  CString tmpstr;
  CPoint point;
  int nReplaced;
  int maxx,maxy;

  if(m_vertical.IsWindowVisible())  m_clipy=m_vertical.GetScrollPos();
  if(m_horizontal.IsWindowVisible()) m_clipx=m_horizontal.GetScrollPos();
  if(m_enablebutton&IW_NOREDRAW) return;
  maxx=m_clipx+m_maxextentx;
  maxy=m_clipy+m_maxextenty;
  if(m_maxclipx<maxx) maxx=m_maxclipx;
  if(m_maxclipy<maxy) maxy=m_maxclipy;
  the_mos.MakeBitmapWhole(GREY, m_bm, m_clipx, m_clipy, maxx, maxy);
  if(m_enablebutton&IW_PLACEIMAGE)
  {
    if(m_enablebutton&IW_MATCH) nReplaced=BM_OVERLAY|BM_MATCH;
    else nReplaced=BM_OVERLAY;
    point=m_confirmed-m_animbam->GetFramePos(0);

    nReplaced=m_animbam->MakeBitmap(0,GREY,m_bm,nReplaced,point.x, point.y);
    tmpbm=m_bitmap.GetBitmap();
    if(tmpbm) ::DeleteObject(tmpbm);

    point=GetPoint(0);
    if(m_enablebutton&IW_MATCH) tmpstr.Format("Place animation... (%d,%d) Match:%d",point.x,point.y,nReplaced);
    else tmpstr.Format("Select point... (%d,%d)",point.x,point.y);
    SetWindowText(tmpstr);
  }
  m_bitmap.SetBitmap(m_bm);
  if(m_enablebutton&IW_SHOWGRID)
  {
    DrawGrid();
    //actually this invalidate call is intended for NOT W98 machines
    if(editflg&W98) Invalidate(true);
  }
  
  UpdateData(UD_DISPLAY); //remove this only when checked all calls to RefreshDialog
}

void CImageView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	int theOldPos;
	SCROLLINFO theScrollInfo;

  CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
  pScrollBar->GetScrollInfo(&theScrollInfo, SIF_ALL);
    
  theOldPos = theScrollInfo.nPos;

  switch(nSBCode)
  {
  case SB_TOP:
    theScrollInfo.nPos = theScrollInfo.nMin;
    break;
    
  case SB_BOTTOM:
    theScrollInfo.nPos = theScrollInfo.nMax;
    break;
    
  case SB_ENDSCROLL:
    break;
    
  case SB_LINEUP:    // Scroll left.
    if (theScrollInfo.nPos > theScrollInfo.nMin)
      theScrollInfo.nPos--;
    break;
    
  case SB_LINEDOWN:
    if (theScrollInfo.nPos < theScrollInfo.nMax)
      theScrollInfo.nPos++;
    break;
    
  case SB_PAGELEFT:
    if (theScrollInfo.nPos > theScrollInfo.nMin)
      theScrollInfo.nPos = max(theScrollInfo.nMin, (int) (theScrollInfo.nPos - theScrollInfo.nPage));
    break;
    
  case SB_PAGERIGHT:
    if (theScrollInfo.nPos < theScrollInfo.nMax)
      theScrollInfo.nPos = min(theScrollInfo.nMax, (int) (theScrollInfo.nPos + theScrollInfo.nPage));
    break;
    
  case SB_THUMBPOSITION:
    theScrollInfo.nPos = nPos;
    break;
    
  case SB_THUMBTRACK:
    theScrollInfo.nPos = nPos;
    break;
	}

	if (theOldPos != theScrollInfo.nPos)
	{
		pScrollBar->SetScrollPos(theScrollInfo.nPos, TRUE);
    PostMessage(WM_COMMAND,ID_REFRESH,0);
  }
}

void CImageView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	int	theOldPos;
	SCROLLINFO  theScrollInfo;

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
  pScrollBar->GetScrollInfo(&theScrollInfo, SIF_ALL);
  
  theOldPos = theScrollInfo.nPos;

  switch(nSBCode)
  {
  case SB_TOP:
    theScrollInfo.nPos = theScrollInfo.nMin;
    break;
    
  case SB_BOTTOM:
    theScrollInfo.nPos = theScrollInfo.nMax;
    break;
    
  case SB_ENDSCROLL:
    break;
    
  case SB_LINEUP:    // Scroll left.
    if (theScrollInfo.nPos > theScrollInfo.nMin)
      theScrollInfo.nPos--;
    break;
    
  case SB_LINEDOWN:
    if (theScrollInfo.nPos < theScrollInfo.nMax)
      theScrollInfo.nPos++;
    break;
    
  case SB_PAGELEFT:
    if (theScrollInfo.nPos > theScrollInfo.nMin)
      theScrollInfo.nPos = max(theScrollInfo.nMin, (int) (theScrollInfo.nPos - theScrollInfo.nPage));
    break;
    
  case SB_PAGERIGHT:
    if (theScrollInfo.nPos < theScrollInfo.nMax)
      theScrollInfo.nPos = min(theScrollInfo.nMax, (int) (theScrollInfo.nPos + theScrollInfo.nPage));
    break;
    
  case SB_THUMBPOSITION:
    theScrollInfo.nPos = nPos;
    break;
    
  case SB_THUMBTRACK:
    theScrollInfo.nPos = nPos;
    break;
  }

	if (theOldPos != theScrollInfo.nPos)
	{
		pScrollBar->SetScrollPos(theScrollInfo.nPos, TRUE);
    PostMessage(WM_COMMAND,ID_REFRESH,0);
  }
}

//draws the vertices
void CImageView::DrawLines()
{
  CDC *cb=m_bitmap.GetDC();
//  cb->Polygon(points,count);
  m_bitmap.ReleaseDC(cb);
}

void CImageView::OnKillfocusValue() 
{
	UpdateData(UD_RETRIEVE);
  PostMessage(WM_COMMAND,ID_REFRESH,0);
}

void CImageView::OnShowall() 
{
  UpdateData(UD_RETRIEVE);
  PostMessage(WM_COMMAND,ID_REFRESH,0);
}

void CImageView::OnShowgrid() 
{
  UpdateData(UD_RETRIEVE);
  PostMessage(WM_COMMAND,ID_REFRESH,0);
}

void CImageView::OnFill() 
{
  UpdateData(UD_RETRIEVE);
  PostMessage(WM_COMMAND,ID_REFRESH,0);
}

void CImageView::OnSelchangeValue() 
{
	m_value=((CComboBox *) GetDlgItem(IDC_VALUE))->GetCurSel();
  PostMessage(WM_COMMAND,ID_REFRESH,0);
}

void CImageView::OnDeltaposSpinx(NMHDR* pNMHDR, LRESULT* pResult) 
{
  int x;
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	x=pNMUpDown->iPos+pNMUpDown->iDelta;
  m_confirmed.x=x;
	*pResult = 0;
  PostMessage(WM_COMMAND,ID_REFRESH,0);
}

void CImageView::OnDeltaposSpiny(NMHDR* pNMHDR, LRESULT* pResult) 
{
  int y;
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

  pNMUpDown->iDelta=-pNMUpDown->iDelta;
	y=pNMUpDown->iPos+pNMUpDown->iDelta;
  m_confirmed.y=y;
	*pResult = 0;
  PostMessage(WM_COMMAND,ID_REFRESH,0);
}

BOOL CImageView::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->hwnd==m_bitmap.m_hWnd) 
  {
    if(pMsg->message==WM_MOUSEMOVE)
    {
      m_mousepoint=pMsg->pt;
      ScreenToClient(&m_mousepoint);
      m_mousepoint-=m_oladjust;
    }
    else if(pMsg->message==WM_LBUTTONUP)
    {
      m_confirmed=m_mousepoint;
      DrawLine(m_sourcepoint, GetPoint(2), (unsigned char) m_value);
    }
    else if(pMsg->message==WM_LBUTTONDOWN)
    {
      m_confirmed=m_mousepoint;
      m_sourcepoint=GetPoint(2);
    }
    else if(pMsg->message==WM_RBUTTONDOWN)
    {

    }
  }
	return CDialog::PreTranslateMessage(pMsg);
}

void CImageView::PostNcDestroy() 
{
  if(m_bm)
  {
    DeleteObject(m_bm);
  }
	CDialog::PostNcDestroy();
}
