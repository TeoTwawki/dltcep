// ImageView.cpp : implementation file
//

#include "stdafx.h"
#include "chitem.h"
#include "ImageView.h"

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
	//}}AFX_DATA_INIT
  m_bm=0;
  m_clipx=0;
  m_clipy=0;
  m_animbam=0;
  m_map=0;
  m_maxextentx=m_maxextenty=8;//maximum number of tiles fitting on screen
}

void CImageView::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CImageView)
	DDX_Control(pDX, IDC_SPINY, m_spiny);
	DDX_Control(pDX, IDC_SPINX, m_spinx);
	DDX_Control(pDX, IDOK, m_setbutton);
	DDX_Control(pDX, IDC_VERTICAL, m_vertical);
	DDX_Control(pDX, IDC_HORIZONTAL, m_horizontal);
	DDX_Control(pDX, IDCANCEL, m_button);
	DDX_Control(pDX, IDC_BITMAP, m_bitmap);
	//}}AFX_DATA_MAP
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
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_REFRESH, RefreshDialog)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImageView message handlers

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
  if(m_enablebutton&IW_SETVERTEX) //we will draw lines
  {
    m_lines.CreateBitmap(the_mos.mosheader.wColumn,the_mos.mosheader.wHeight,1,8,NULL);    
  }
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

//returns a point (frame=0) or the tile (frame=1) clicked on
CPoint CImageView::GetPoint(int frame)
{
  CPoint ret;

  if(frame)
  {
    ret.x=m_confirmed.x/the_mos.mosheader.dwBlockSize+m_clipx;
    ret.y=m_confirmed.y/the_mos.mosheader.dwBlockSize+m_clipy;
  }
  else
  {
    ret.x=m_confirmed.x+m_clipx*the_mos.mosheader.dwBlockSize;
    ret.y=m_confirmed.y+m_clipy*the_mos.mosheader.dwBlockSize;
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
  SetWindowPos(0, 0,0, rect.Width()+adjusty+20, rect.Height()+adjustx+80,SWP_NOACTIVATE|SWP_SHOWWINDOW|SWP_NOZORDER|SWP_NOMOVE);
  m_horizontal.SetWindowPos(0,0,0,rect.Width(),15,SWP_NOMOVE|SWP_NOZORDER);
  m_vertical.SetWindowPos(0,rect.Width()+10,adjustx+10,15,rect.Height(),SWP_NOZORDER);
  if(m_enablebutton&IW_SETVERTEX)
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
  m_button.ShowWindow(!!(m_enablebutton&IW_ENABLEBUTTON));
  m_setbutton.ShowWindow(!!(m_enablebutton&IW_SETVERTEX));
  if(m_enablebutton&IW_ENABLEBUTTON)
  {
    RefreshDialog(); //need a redraw for selection of the bottom right side of the tis
  }
}

BOOL CImageView::OnInitDialog() 
{
	CDialog::OnInitDialog();
  RedrawContent();
	return TRUE;  
}

void CImageView::OnBitmap() 
{
  m_confirmed=m_mousepoint;
  if(m_enablebutton&IW_SETVERTEX)
  {
    if(m_enablebutton&IW_PLACEIMAGE)
    {
      m_spinx.SetPos(m_confirmed.x);
      m_spiny.SetPos(m_confirmed.y);
      RefreshDialog();
    }
    return;
  }
  OnOK();	
}

void CImageView::OnMouseMove(UINT nFlags, CPoint point) 
{
	m_mousepoint=point;
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
  UpdateData(UD_DISPLAY);
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
  
}

void CImageView::PostNcDestroy() 
{
  if(m_bm)
  {
    DeleteObject(m_bm);
  }
	CDialog::PostNcDestroy();
}

void CImageView::OnDeltaposSpinx(NMHDR* pNMHDR, LRESULT* pResult) 
{
  int x;
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	x=pNMUpDown->iPos+pNMUpDown->iDelta;
  m_confirmed.x=x;
	*pResult = 0;
  RefreshDialog();
}

void CImageView::OnDeltaposSpiny(NMHDR* pNMHDR, LRESULT* pResult) 
{
  int y;
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

  pNMUpDown->iDelta=-pNMUpDown->iDelta;
	y=pNMUpDown->iPos+pNMUpDown->iDelta;
  m_confirmed.y=y;
	*pResult = 0;
  RefreshDialog();
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
  }
	return CDialog::PreTranslateMessage(pMsg);
}
