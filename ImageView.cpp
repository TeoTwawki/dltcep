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

#define MAX_COLOR 4

static unsigned long circle_colors[MAX_COLOR]={
  RGB(255,255,255),
  RGB(255,255,0),
  RGB(255,0,255),
  RGB(0,255,255),
};

CImageView::CImageView(CWnd* pParent /*=NULL*/) : CDialog(CImageView::IDD, pParent)
{
	//{{AFX_DATA_INIT(CImageView)
	m_showall = FALSE;
	m_showgrid = FALSE;
	m_fill = FALSE;
	//}}AFX_DATA_INIT
  m_text=true;
  m_max=15;
  m_bm=NULL;
  m_clipx=0;
  m_clipy=0;
  m_animbam=NULL;
  m_frame=0;
  m_map=NULL;
  m_palette=NULL;
  m_maxclipx=m_maxclipy=1;
  m_sourcepoint=0;
  m_mos=NULL;
  m_enablebutton=IW_NOREDRAW;
  m_value=-1;
  m_actvertex=0;
  m_vertexcount=0;
  m_gridcolor1=color1;
  m_gridcolor2=color2;
  m_gridcolor3=color3;
  m_maptype=MT_DEFAULT;
  m_maxextentx=0;
  m_maxextenty=0;
}

void CImageView::DoDataExchange(CDataExchange* pDX)
{
  CString tmpstr;

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CImageView)
	DDX_Control(pDX, IDC_FILL, m_fill_control);
	DDX_Control(pDX, IDC_OVERLAY, m_overlay_control);
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
	DDX_Check(pDX, IDC_OVERLAY, m_text);
	//}}AFX_DATA_MAP
  
  if(m_max && (m_value!=-1) )
  {
    tmpstr=GetMapTypeValue(m_maptype,m_value);
    DDX_Text(pDX, IDC_VALUE, tmpstr);
    m_value=strtonum(tmpstr);
    DDV_MinMaxInt(pDX, m_value, 0, m_max);
  }

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
  ON_BN_CLICKED(IDC_OVERLAY, OnOverlay)
	ON_WM_SIZE()
	ON_WM_MOUSEWHEEL()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_REFRESH, RefreshDialog)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImageView message handlers

void CImageView::PutPixel(CPoint point, unsigned char color)
{
  int w,pos;
  LPBYTE tmp = (LPBYTE) m_map;

  if(m_maptype==MT_EXPLORED)
  {
    w=the_area.m_width/32;
    if(!pst_compatible_var())
    {
      w++;
    }
  }
  else
  {
    w=the_area.m_width/GR_WIDTH;
  }
  pos=point.x+point.y*w;
  
  if((m_max==1) && !m_fill )
  {
    tmp[pos]=!tmp[pos];
    return;
  }
  tmp[pos]=(unsigned char) color;
}

//intentionally int, so we can return -1 when coordinates are out of map
int CImageView::GetPixel(CPoint point)
{
  int w,h;
  LPBYTE tmp = (LPBYTE) m_map;

  if(m_maptype==MT_EXPLORED)
  {
    w=the_area.m_width/32;
    h=the_area.m_height/32;
    if(!pst_compatible_var())
    {
      w++;
      h++;
    }
  }
  else
  {
    w=the_area.m_width/GR_WIDTH;
    h=(the_area.m_height+GR_HEIGHT-1)/GR_HEIGHT;
  }
  if(point.x<0 || point.y<0) return -1;
  if(point.x>=w) return -1;
  if(point.y>=h) return -1;
  return tmp[point.x+point.y*w];
}

void CImageView::InitView(int flags, Cmos *my_mos)
{
  m_mos = my_mos;  
  m_oladjust = 0;
  m_animbam = NULL;
  m_confirmed = m_mousepoint = 0;
  m_clipx = m_minclipx = 0;
  m_clipy = m_minclipy = 0;
  if(m_mos)
  {
    m_maxclipx=m_mos->mosheader.wColumn;
    m_maxclipy=m_mos->mosheader.wRow;
    if(m_maxclipx<1) m_maxclipx=1;
    if(m_maxclipy<1) m_maxclipy=1;
  }
  else
  {
    m_maxclipx=1;
    m_maxclipy=1;
  }
  m_enablebutton=flags;
  if(flags&IW_GETPOLYGON)
  {
    CenterPolygon((CPtrList *) m_map, m_value);
  }
}

void CImageView::SetOverlay(bool type)
{
  m_text=type;
}

unsigned long boolean_palette[2]={0,1};

void CImageView::SetMapType(int maptype, LPVOID map)
{
  m_maptype=maptype;
  m_map=map;
  switch(maptype)
  {
  case MT_BAM:
    m_palette=NULL;
    m_max=0;
    break;
  case MT_HEIGHT:
    m_palette=the_area.htpal;
    m_max=15;
    break;
  case MT_HEIGHT8:
    m_palette=the_area.ht8pal;
    m_max=255;
    break;
  case MT_LIGHT:
    m_palette=the_area.lmpal;
    m_max=255;
    break;
  case MT_SEARCH:
    m_palette=the_area.srpal;
    m_max=15;
    break;
  case MT_BLOCK: case MT_EXPLORED:
    m_palette=boolean_palette;
    m_max=1;
    break;
  default:
    m_palette=NULL;
    m_max=GetCountPolygon();
  }
}

#define TMPX (m_maxextentx/2)
#define TMPY (m_maxextenty/2)

void CImageView::SetupAnimationPlacement(Cbam *bam, int orgx, int orgy, int frame)
{
  CPoint point;
  int tmpw;

  m_frame=frame;
  point=bam->GetFrameSize(frame);
  point.x/=m_mos->mosheader.dwBlockSize;
  point.y/=m_mos->mosheader.dwBlockSize;
  m_animbam=bam;
  point=bam->GetFramePos(frame);
  tmpw=min(TMPX-point.x,TMPY-point.y);
  if(tmpw<0) tmpw=0;
  m_clipx=(orgx-point.x)/(signed) m_mos->mosheader.dwBlockSize;
  m_clipy=(orgy-point.y)/(signed) m_mos->mosheader.dwBlockSize;

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

  m_mousepoint.x=orgx;
  m_mousepoint.y=orgy;
  m_confirmed = m_mousepoint;
  m_oladjust=point;
}

void CImageView::CenterPolygon(CPtrList *polygons, int idx)
{
  int size;
  POSITION pos;
  area_vertex *polygon;

  pos=GetPolygonAt(polygons, idx);
  if(pos)
  {
    size=GetPolygonSize(idx);
    if(size)
    {
      polygon=(area_vertex *) polygons->GetAt(pos);
      m_clipx=polygon[0].x/m_mos->mosheader.dwBlockSize-TMPX;
      m_clipy=polygon[0].y/m_mos->mosheader.dwBlockSize-TMPY;
    }
  }
}

POSITION CImageView::GetPolygonAt(CPtrList *polygons, int idx)
{
  switch(m_maptype)
  {
  case MT_AMBIENT:
    return NULL;
  case MT_WALLPOLYLIST:
  case MT_REGION:
    return polygons->FindIndex(idx);
  case MT_DOORPOLYLIST: case MT_BLOCK:
    return polygons->FindIndex(idx+the_area.wallpolygoncount);
  case MT_CONTAINER:
    return polygons->FindIndex(idx+the_area.triggercount);
  case MT_DOOR:
    //must multiply it by 2 because of the impeded blocks
    if(idx&1)
    {
      return polygons->FindIndex((idx<<1)-1+the_area.triggercount+the_area.containercount);
    }
    else
    {
      return polygons->FindIndex((idx<<1)+the_area.triggercount+the_area.containercount);
    }
  }
  return NULL;
}

int CImageView::GetCountPolygon()
{
  switch(m_maptype)
  {
  case MT_AMBIENT:
    return the_area.ambientcount;
  case MT_WALLPOLYLIST:
    return the_area.wallpolygoncount;
  case MT_DOORPOLYLIST: case MT_BLOCK:
    return the_area.doorpolygoncount;
  case MT_REGION:
    return the_area.triggercount;
  case MT_CONTAINER:
    return the_area.containercount;
  case MT_DOOR:
    return the_area.doorcount*2;
  }
  return 0;
}

CString CImageView::GetPolygonText(int idx)
{
  int found;
  CString tmp;

  switch(m_maptype)
  {
  case MT_WALLPOLYLIST:
    tmp.Format("%d",idx+1);
    break;
  case MT_DOORPOLYLIST:case MT_BLOCK:
    for(found=0;found<the_area.weddoorcount;found++)
    {
      if(CheckIntervallum(idx,the_area.weddoorheaders[found].offsetpolygonopen, the_area.weddoorheaders[found].countpolygonopen)!=-1 )
      {      
        RetrieveResref(tmp,the_area.weddoorheaders[found].doorid);
        break;
      }
      if(CheckIntervallum(idx,the_area.weddoorheaders[found].offsetpolygonclose, the_area.weddoorheaders[found].countpolygonclose)!=-1 )
      {      
        RetrieveResref(tmp,the_area.weddoorheaders[found].doorid);
        break;
      }
    }
    break;
  case MT_REGION:
    RetrieveVariable(tmp,the_area.triggerheaders[idx].infoname);
    break;
  case MT_CONTAINER:
    RetrieveVariable(tmp,the_area.containerheaders[idx].containername);
    break;
  case MT_DOOR:
    RetrieveResref(tmp,the_area.doorheaders[idx/2].doorid);
    break;
  }
  return tmp;
}

short *CImageView::GetPolygonBox(int idx)
{
  switch(m_maptype)
  {
  case MT_WALLPOLYLIST:
    return &the_area.wallpolygonheaders[idx].minx;
  case MT_DOORPOLYLIST:case MT_BLOCK:
    return &the_area.doorpolygonheaders[idx].minx;
  case MT_REGION:
    return &the_area.triggerheaders[idx].p1x;
  case MT_CONTAINER:
    return &the_area.containerheaders[idx].p1x;
  case MT_DOOR:
    if(idx&1) return &the_area.doorheaders[idx/2].cp1x;
    return &the_area.doorheaders[idx/2].op1x;      
  }
  return NULL;
}

int CImageView::GetPolygonSize(int idx)
{
  if (idx<0) return 0;
  switch(m_maptype)
  {
  case MT_WALLPOLYLIST:
    return the_area.wallpolygonheaders[idx].countvertex;
  case MT_DOORPOLYLIST:case MT_BLOCK:
    return the_area.doorpolygonheaders[idx].countvertex;
  case MT_REGION:
    return the_area.triggerheaders[idx].vertexcount;
  case MT_CONTAINER:
    return the_area.containerheaders[idx].vertexcount;
  case MT_DOOR:
    if(idx&1) return the_area.doorheaders[idx/2].countvertexclose;
    return the_area.doorheaders[idx/2].countvertexopen;
  }
  return 0;
}

inline int CImageView::IsPointInPolygon(void *wedvertex, int idx, CPoint point)
{
  if (m_maptype==MT_AMBIENT)
  {
    return PointInCircle((area_ambient *) wedvertex+idx, point);
  }
  else
  {
    return PointInPolygon((area_vertex *) wedvertex, GetPolygonSize(idx), point);
  }
}

int CImageView::FindPolygon(CPtrList *polygons, CPoint point)
{
  POSITION pos;
  int idx, max;

  max=GetCountPolygon();
  if(m_maptype==MT_DOOR)
  {
    m_value&=~1;
    m_value+=2;
  }
  else m_value++;
  if (m_value>=max) m_value=0;

  pos=GetPolygonAt(polygons, m_value);
  for(idx=m_value;idx<max;idx++)
  {
    if(IsPointInPolygon(polygons->GetNext(pos), idx, point) )
    {
      return idx;
    }
    if((m_maptype==MT_DOOR) && (idx&1) )
    {
      polygons->GetNext(pos);
      polygons->GetNext(pos);
    }
  }
  pos=GetPolygonAt(polygons, 0);
  for(idx=0;idx<m_value;idx++)
  {
    if(IsPointInPolygon(polygons->GetNext(pos), idx, point) )
    {
      return idx;
    }
    if((m_maptype==MT_DOOR) && (idx&1) )
    {
      polygons->GetNext(pos);
      polygons->GetNext(pos);
    }
  }
  return -1;
}

int CImageView::FindAmbient(area_ambient *ambients, CPoint point)
{
  int idx, max;

  if (m_maptype!=MT_AMBIENT) return -1;
  if (m_max<0) return -1;
  max=the_area.ambientcount;
  m_value++;
  if (m_value>=max) m_value=0;

  for(idx=m_value;idx<max;idx++)
  {
    if(IsPointInPolygon(ambients, idx, point) )
    {
      return idx;
    }
  }  
  for(idx=0;idx<m_value;idx++)
  {
    if(IsPointInPolygon(ambients, idx, point) )
    {
      return idx;
    }
  }
  return -1;
}

void CImageView::DrawActors()
{
  int i;

  CDC *pDC = m_bitmap.GetDC();
  if(!pDC) return;
	CDC dcBmp;
  dcBmp.CreateCompatibleDC(pDC);
  //selecting the bitmap for drawing
  CBitmap *cbtmp=dcBmp.SelectObject(CBitmap::FromHandle(m_bitmap.GetBitmap()));
  //setting a red color
  dcBmp.SetBkMode(1);
  dcBmp.SetTextColor(RGB(255,255,255) );
  CPen pen2(PS_SOLID,2,m_gridcolor2 ); //red
  CPen *cpentmp=dcBmp.SelectObject(&pen2);

  for(i=0;i<the_area.actorcount;i++)
  {
    area_actor *aa = the_area.actorheaders+i;

    CString title = CString(aa->creresref,sizeof(aa->creresref));
    int x = aa->posx-m_clipx*m_mos->mosheader.dwBlockSize;
    int y = aa->posy-m_clipy*m_mos->mosheader.dwBlockSize;
    dcBmp.TextOut(x,y,title);
    dcBmp.Ellipse(x-2, y-2, x+2, y+2);
  }

  //close
  //restore original settings (this will prevent some leaks)
  dcBmp.SelectObject(cbtmp);
  dcBmp.SelectObject(cpentmp);
  m_bitmap.Invalidate(false);
  m_bitmap.ReleaseDC(pDC);
}

void CImageView::DrawAnims()
{
  int i, fc;
  CPoint point;
  Cbam tmpbam;

  for(i=0;i<the_area.animcount;i++)
  {
    area_anim *aa = the_area.animheaders+i;
    read_bam(CString(aa->bam), &tmpbam, true);
    point.x = aa->posx-m_clipx*m_mos->mosheader.dwBlockSize;
    point.y = aa->posy-m_clipy*m_mos->mosheader.dwBlockSize;
    fc=tmpbam.GetFrameIndex(aa->cyclenum,aa->framenum);
    point-=tmpbam.GetFramePos(fc);
    tmpbam.MakeBitmap(fc,GREY,*m_mos,BM_OVERLAY,point.x, point.y);
  }
}

void CImageView::DrawAmbients()
{
  int i;

  for(i=0;i<the_area.ambientcount;i++)
  {
    DrawCircle(the_area.ambientheaders[i].posx, the_area.ambientheaders[i].posy, the_area.ambientheaders[i].radius, i%MAX_COLOR);
  }
}

void CImageView::DrawMapText()
{
  int i;
  CString tmpstr;
	CDC dcBmp;
  CPoint point;
  map_area *ar;

  CDC *pDC = m_bitmap.GetDC();
  if(!pDC) return;
  dcBmp.CreateCompatibleDC(pDC);
  CBitmap *cbtmp=dcBmp.SelectObject(CBitmap::FromHandle(m_bitmap.GetBitmap()));

  for(i=0;i<m_animbam->GetCycleCount();i++)
  {
    ar=((map_area *) m_map)+i;
    point.x = ar->xpos-m_clipx*m_mos->mosheader.dwBlockSize;
    point.y = ar->ypos-m_clipy*m_mos->mosheader.dwBlockSize;
    ar=((map_area *) m_map)+i;
    RetrieveResref(tmpstr, ar->areaname);
    dcBmp.TextOut(point.x,point.y,tmpstr);
  }
  dcBmp.SelectObject(cbtmp);
  m_bitmap.Invalidate(false);
  m_bitmap.ReleaseDC(pDC);
}

void CImageView::DrawIcons()
{
  int i, fc;
  CPoint point;
  map_area *ar;

  for(i=0;i<m_animbam->GetCycleCount();i++)
  {
    ar=((map_area *) m_map)+i;
    fc=m_animbam->GetFrameIndex(ar->bamindex,0);
    if(fc==m_frame) continue;
    point.x = ar->xpos-m_clipx*m_mos->mosheader.dwBlockSize;
    point.y = ar->ypos-m_clipy*m_mos->mosheader.dwBlockSize;
    point-=m_animbam->GetFramePos(fc);
    m_animbam->MakeBitmap(fc,GREY,*m_mos,BM_OVERLAY,point.x, point.y);

  }
}

void CImageView::DrawPolyPolygon(CPtrList *polygons)
{
  CString tmpstr;
  POSITION pos;
  short *bbox;
  int idx, max;
  unsigned int count;
  bool fill;

  idx=0;
  pos=GetPolygonAt(polygons, 0);
  max=GetCountPolygon();
  for(idx=0;idx<max;idx++)
  {
    if(idx==m_value) fill=true;
    else fill=false;
    count=GetPolygonSize(idx);
    if(count>1)
    {      
      bbox=GetPolygonBox(idx);
      tmpstr=GetPolygonText(idx);
      //the wed bbox and the area bbox are not compatible, Bioware learnt this from M$, i guess
      if((unsigned int) bbox[BBMINX]>(m_clipx+m_maxextentx)*m_mos->mosheader.dwBlockSize) goto endofquest;
      if((unsigned int) bbox[m_maptype>=MT_REGION?BBMAXX:WBBMAXX]<m_clipx*m_mos->mosheader.dwBlockSize) goto endofquest;
      if((unsigned int) bbox[m_maptype>=MT_REGION?BBMINY:WBBMINY]>(m_clipy+m_maxextenty)*m_mos->mosheader.dwBlockSize) goto endofquest;
      if((unsigned int) bbox[BBMAXY]<m_clipy*m_mos->mosheader.dwBlockSize) goto endofquest;      
      DrawLines((POINTS *) polygons->GetAt(pos), count, tmpstr, fill,0);
    }
endofquest:
    polygons->GetNext(pos);
    if((m_maptype==MT_DOOR) && (idx&1) )
    {
      polygons->GetNext(pos);
      polygons->GetNext(pos);
    }
  }
}

void CImageView::DrawCircle(int x, int y, int radius, int colorindex)
{
  CDC *pDC = m_bitmap.GetDC();
  if(!pDC) return;
	CDC dcBmp;
  dcBmp.CreateCompatibleDC(pDC);
  CBitmap *cbtmp=dcBmp.SelectObject(CBitmap::FromHandle(m_bitmap.GetBitmap()));
  //setting a green color
  dcBmp.SetBkMode(TRANSPARENT);
  dcBmp.SetTextColor( circle_colors[colorindex] );
  CPen pen1(PS_DOT,1,m_gridcolor3 ); //blue  
  CPen *cpentmp=dcBmp.SelectObject(&pen1);
  CBrush b;
  b.CreateHatchBrush(colorindex,circle_colors[colorindex]);
  CBrush *cbrushtmp=dcBmp.SelectObject(&b);
  x-=m_clipx*m_mos->mosheader.dwBlockSize;
  y-=m_clipy*m_mos->mosheader.dwBlockSize;

  dcBmp.Ellipse(x-radius,y-radius,x+radius, y+radius);
  //close
  //restore original settings (this will prevent some leaks)
  dcBmp.SelectObject(cbtmp);
  dcBmp.SelectObject(cpentmp);
  dcBmp.SelectObject(cbrushtmp);
  m_bitmap.Invalidate(false);
  m_bitmap.ReleaseDC(pDC);
}

void CImageView::DrawLines(POINTS *polygon, unsigned int count, CString title, int fill, int actv)
{
  unsigned int i;

  CDC *pDC = m_bitmap.GetDC();
  if(!pDC) return;
	CDC dcBmp;
  dcBmp.CreateCompatibleDC(pDC);
  
  //selecting the bitmap for drawing
  CBitmap *cbtmp=dcBmp.SelectObject(CBitmap::FromHandle(m_bitmap.GetBitmap()));
  //setting a green color
  dcBmp.SetBkMode(1);
  dcBmp.SetTextColor(RGB(255,255,255) );
  CPen mypen(PS_SOLID,1,m_gridcolor1 );//green
  CPen pen1(PS_SOLID,1,m_gridcolor3 ); //blue  
  CPen pen2(PS_SOLID,2,m_gridcolor2 ); //red
  CPen *cpentmp=dcBmp.SelectObject(&mypen);

  //draw here
  if(polygon)
  {
    //we close the polygon if needed
    //this hack exploits that POINTS is the same size as a long
    int needclose=((long *)polygon)[0]!=((long *)polygon)[count-1];
    POINT *m_poly = (POINT *) malloc (sizeof(POINT)*(count+needclose) );
    if(m_poly)
    {
      for(i=0;i<count;i++)
      {
        m_poly[i].x=polygon[i].x-m_clipx*m_mos->mosheader.dwBlockSize;
        m_poly[i].y=polygon[i].y-m_clipy*m_mos->mosheader.dwBlockSize;
      }
      if(needclose)
      {
        m_poly[count]=m_poly[0];
      }
      dcBmp.TextOut(m_poly[0].x,m_poly[0].y,title);

      if(fill)
      {
        dcBmp.Polygon(m_poly,count); //this closes the polygon on its own
      }
      else
      {
        dcBmp.Polyline(m_poly,(count+needclose) );
      }
      if(count>1)
      {
        dcBmp.SelectObject(&pen1);
        dcBmp.Ellipse(m_poly[0].x-1, m_poly[0].y-1,m_poly[0].x+1, m_poly[0].y+1);
        dcBmp.MoveTo(m_poly[0]);
        dcBmp.LineTo(m_poly[1]);
      }
      dcBmp.SelectObject(&pen2);
      dcBmp.Ellipse(m_poly[actv].x-1, m_poly[actv].y-1,m_poly[actv].x+1, m_poly[actv].y+1);
      free(m_poly);
    }
  }

  //close
  //restore original settings (this will prevent some leaks)
  dcBmp.SelectObject(cbtmp);
  dcBmp.SelectObject(cpentmp);
  m_bitmap.Invalidate(false);
  m_bitmap.ReleaseDC(pDC);
}

void CImageView::DrawGrid()
{
  int i,j;
  int xs, ys;
  int sx, sy;

  if(m_enablebutton&IW_EDITMAP)
  {
    if(m_maptype==MT_EXPLORED)
    {
      xs=32;
      ys=32; //x and y sizes
      if(pst_compatible_var())
      {
        sx=0;
        sy=0;
      }
      else
      {
        sx=-16; //x and y starting points for grids
        sy=-16;
      }
    }
    else
    {
      xs=GR_WIDTH; ys=GR_HEIGHT; //x and y sizes
      sx=m_clipx*m_mos->mosheader.dwBlockSize%xs; //x and y starting points for grids
      sy=m_clipy*m_mos->mosheader.dwBlockSize%ys;
    }
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
  CBitmap *cbtmp=dcBmp.SelectObject(CBitmap::FromHandle(m_bitmap.GetBitmap()));
  //setting a green color
  dcBmp.SetBkMode(1);
  dcBmp.SetTextColor(RGB(255,255,255) );
  CPen mypen1(PS_SOLID,1,m_gridcolor1 );
  CPen mypen2(PS_SOLID,1,m_gridcolor2 );
  CPen *cpentmp=dcBmp.SelectObject(&mypen1);

  CRect cr;
  CString tmpstr;
  LPBYTE tmp = (LPBYTE) m_map;
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
    int p, px, py;

    if(m_showgrid)
    {
      dcBmp.MoveTo(0,j); dcBmp.LineTo(cr.Width(),j);
    }
    if(m_map && (m_enablebutton&IW_EDITMAP) )
    {
      px=(int) (m_clipx*m_mos->mosheader.dwBlockSize)/xs;
      py=(int) (m_clipy*m_mos->mosheader.dwBlockSize+j-sy)/ys;
      if((m_maptype==MT_EXPLORED) && !pst_compatible_var())
      {
        p=py*(the_area.m_width/xs+1)+px;
      }
      else
      {
        p=py*the_area.m_width/xs+px;
      }
      for(i=sx;i<cr.Width();i+=xs)
      {
        if(m_showall || (tmp[p]==m_value) )
        {
          if (m_text) {
            if(m_max==255) tmpstr.Format("%02x",tmp[p]);
            else tmpstr.Format("%d",tmp[p]);
            int tx=i+(xs-10)/2;
            if(tx<0) tx=0;
            int ty=j+(ys-12)/2;
            if(ty<0) ty=0;
            dcBmp.TextOut(tx,ty,tmpstr);
          } else {
            CRect r;
            COLORREF c = m_palette[tmp[p]];
            
            CBrush b((c&0xff)<<16 | (c&0xff0000)>>16 | c&0xff00 );
            r.left=i+xs/3;
            r.top=j+ys/3;
            r.right=i+2*xs/3;
            r.bottom=j+2*ys/3;
            dcBmp.FrameRect(r, &b);
          }
        }
        p++;
      }
    }
  }
  if(m_showgrid && (m_enablebutton&IW_MARKTILE) )
  {
    CRect rect;
    dcBmp.SelectObject(&mypen2);

    rect.TopLeft()=GetPoint(GP_TILE);
    rect.left-=m_clipx;
    rect.top-=m_clipy;
    rect.left*=m_mos->mosheader.dwBlockSize;
    rect.top*=m_mos->mosheader.dwBlockSize;
    rect.bottom=rect.top+m_mos->mosheader.dwBlockSize;
    rect.right=rect.left+m_mos->mosheader.dwBlockSize;
    dcBmp.DrawFocusRect(rect);
  }
 
  //restore original settings (this will prevent some leaks)
  dcBmp.SelectObject(cbtmp);
  dcBmp.SelectObject(cpentmp);
  m_bitmap.Invalidate(false);
  m_bitmap.ReleaseDC(pDC);
}

void CImageView::SetClipSize(int x, int y)
{
  m_clipx=x;
  m_clipy=y;
  if(m_clipx>m_maxclipx-m_maxextentx)
  {
    if(m_maxclipx>=m_maxextentx) m_clipx=m_maxclipx-m_maxextentx;
    else m_clipx=0;
  }
  else
  {
    if(m_clipx<0) m_clipx=0;
  }
  if(m_clipy>m_maxclipy-m_maxextenty)
  {
    if(m_maxclipy>=m_maxextenty) m_clipy=m_maxclipy-m_maxextenty;
    else m_clipy=0;
  }
  else
  {
    if(m_clipy<0) m_clipy=0;
  }
}

//this was not completely implemented
void CImageView::SetPoint(CPoint &point, int frame)
{
  m_confirmed = point;
  switch(frame)
  {
  case GP_EXPLORED:
    if (pst_compatible_var())
    {
      m_confirmed.x*=32;
      m_confirmed.y*=32;
    }
    else
    {
      //not sure
      m_confirmed.x*=32;
      m_confirmed.y*=32;
    }
    break;
  case GP_BLOCK:
    //whatever
    break;
  case GP_TILE:
    m_confirmed.x*=m_mos->mosheader.dwBlockSize;
    m_confirmed.y*=m_mos->mosheader.dwBlockSize;
    break;
  }
}

//returns a point (frame=0) or the tile (frame=1) clicked on
// frame==2 for editmap
CPoint CImageView::GetPoint(int frame)
{
  CPoint ret;

  switch(frame)
  {
  case GP_EXPLORED:
    if(pst_compatible_var())
    {
      ret.x=m_confirmed.x/32;
      ret.y=m_confirmed.y/32;
    }
    else
    {
      ret.x=(m_confirmed.x+16)/32;
      ret.y=(m_confirmed.y+16)/32;
    }
    break;
  case GP_BLOCK:
    ret.x=(m_confirmed.x-m_clipx*m_mos->mosheader.dwBlockSize%GR_WIDTH*2)/GR_WIDTH;
    ret.y=(m_confirmed.y-m_clipy*m_mos->mosheader.dwBlockSize%GR_HEIGHT*2)/GR_HEIGHT;    
    break;
  case GP_TILE:
    ret.x=m_confirmed.x/m_mos->mosheader.dwBlockSize;
    ret.y=m_confirmed.y/m_mos->mosheader.dwBlockSize;
    break;
  case GP_POINT:
    ret.x=m_confirmed.x;
    ret.y=m_confirmed.y;
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

  if(!this->m_hWnd) return;
  if(m_maxextentx>m_maxclipx) m_maxextentx=m_maxclipx;
  if(m_maxextenty>m_maxclipy) m_maxextenty=m_maxclipy;
  theScrollInfo.fMask=SIF_PAGE|SIF_POS|SIF_RANGE;
  adjustx=0;
  if (m_horizontal && m_maxextentx)
  {
    theScrollInfo.nPage=(m_maxclipx-m_minclipx+m_maxextentx-1)/m_maxextentx;
    if(m_clipx<0) m_clipx=0;
    theScrollInfo.nPos=m_clipx;
    theScrollInfo.nMin=m_minclipx;
    theScrollInfo.nMax=m_maxclipx-m_maxextentx+theScrollInfo.nPage-1;
    m_horizontal.SetScrollInfo(&theScrollInfo,true);
    if(m_maxclipx-m_minclipx>m_maxextentx)
    {
      adjustx=20;
      m_horizontal.ShowWindow(SW_SHOW);
    }
    else
    {    
      m_horizontal.ShowWindow(SW_HIDE);
    }
  }

  adjusty=0;
  if (m_vertical && m_maxextenty)
  {
    theScrollInfo.nPage=(m_maxclipy-m_minclipy+m_maxextenty-1)/m_maxextenty;
    if(m_clipy<0) m_clipy=0; //why was this removed?
    theScrollInfo.nPos=m_clipy;
    theScrollInfo.nMin=m_minclipy;
    theScrollInfo.nMax=m_maxclipy-m_maxextenty+theScrollInfo.nPage-1;
    m_vertical.SetScrollInfo(&theScrollInfo,true);
    if(m_maxclipy-m_minclipy>m_maxextenty)
    {
      m_vertical.ShowWindow(SW_SHOW);
      adjusty=20;
    }
    else
    {
      m_vertical.ShowWindow(SW_HIDE);
    }
  }
  rect.SetRectEmpty();
  if(m_mos)
  {
    rect.right=m_maxextentx*m_mos->mosheader.dwBlockSize;
    rect.bottom=m_maxextenty*m_mos->mosheader.dwBlockSize;
  }

  if (m_bitmap)
  {
    if(m_enablebutton&IW_NOREDRAW)
    {
      m_bitmap.SetBitmap(m_bm);
      if(m_bm)
      {
        m_bitmap.GetWindowRect(rect);
      }
    }
    else
    {
      maxx=m_clipx+m_maxextentx;
      maxy=m_clipy+m_maxextenty;
      if(m_maxclipx<maxx) maxx=m_maxclipx;
      if(m_maxclipy<maxy) maxy=m_maxclipy;
      m_mos->MakeBitmapWhole(GREY, m_bm, m_clipx, m_clipy, maxx, maxy, false);
      m_bitmap.SetBitmap(m_bm);
    }
  }

  if (m_button)
  {
    m_button.GetWindowRect(brect);
    m_adjust.x=10;
    m_adjust.y=10;
    m_oladjust=m_adjust;
    m_bitmap.SetWindowPos(0,m_adjust.x,m_adjust.y,0,0,SWP_NOSIZE|SWP_NOZORDER);
    if(rect.Width()<brect.Width() +20 ) rect.right=brect.Width()+20+rect.left;
    if(rect.Width()<400)
    {
      if(m_enablebutton&(IW_EDITMAP|IW_SHOWGRID|IW_PLACEIMAGE) )
      {
        rect.right=400+rect.left;
      }
    }
    //if there are buttons (not preview), then move window to top left
    UINT flags = SWP_NOACTIVATE|SWP_SHOWWINDOW|SWP_NOZORDER;
    if (!(m_enablebutton&IW_ENABLEBUTTON)) flags |= SWP_NOMOVE;
    
    SetWindowPos(0, 0,0, rect.Width()+adjusty+20, rect.Height()+adjustx+80, flags);
    m_horizontal.SetWindowPos(0,m_adjust.x,rect.Height()+m_adjust.y,rect.Width(),15,SWP_NOZORDER);
    m_vertical.SetWindowPos(0,rect.Width()+m_adjust.x,m_adjust.y,15,rect.Height(),SWP_NOZORDER);
    if(m_enablebutton&IW_PLACEIMAGE)
    {
      adjusty=(rect.Width()-brect.Width())/3;
      m_setbutton.SetWindowPos(0,adjusty,rect.Height()+adjustx+25 ,0,0, SWP_SHOWWINDOW|SWP_NOZORDER|SWP_NOSIZE);
      m_button.SetWindowPos(0,adjusty*2,rect.Height()+adjustx+25 ,0,0, SWP_SHOWWINDOW|SWP_NOZORDER|SWP_NOSIZE);
      m_spinx.SetWindowPos(0,25,rect.Height()+adjustx+25 ,0,0, SWP_SHOWWINDOW|SWP_NOZORDER|SWP_NOSIZE);
      m_spiny.SetWindowPos(0,60,rect.Height()+adjustx+25 ,0,0, SWP_SHOWWINDOW|SWP_NOZORDER|SWP_NOSIZE);
      m_spinx.SetRange32(0,m_mos->mosheader.wWidth);
      m_spiny.SetRange32(0,m_mos->mosheader.wWidth);
      m_spinx.SetPos(m_confirmed.x);
      m_spiny.SetPos(m_confirmed.y);
    }
    else
    {
      m_button.SetWindowPos(0, (rect.Width()-brect.Width())/2,rect.Height()+adjustx+25 ,0,0, SWP_SHOWWINDOW|SWP_NOZORDER|SWP_NOSIZE);
    }
    m_button.ShowWindow(!!(m_enablebutton&IW_ENABLEBUTTON));
  }
  
  if (m_overlay_control)
  {
    if(m_max>1)
    {
      m_overlay_control.ShowWindow((m_enablebutton&(IW_SHOWALL|IW_EDITMAP))==(IW_SHOWALL|IW_EDITMAP) );
    }
    else
    {
      m_overlay_control.ShowWindow(false);
    }
  }

  if (m_showall_control)
  {
    m_showall_control.ShowWindow(!!(m_enablebutton&IW_SHOWALL));
  }

  if (m_showgrid_control)
  {
    m_showgrid_control.ShowWindow(!!(m_enablebutton&IW_SHOWGRID));
    m_showgrid_control.SetWindowPos(0,(rect.Width()-brect.Width())*3/4+70,rect.Height()+adjustx+25 ,0,0, SWP_NOZORDER|SWP_NOSIZE);
  }

  if (m_fill_control)
  {
    m_fill_control.ShowWindow(!!(m_enablebutton&IW_ENABLEFILL));
    m_fill_control.SetWindowPos(0,(rect.Width()-brect.Width())*3/4,rect.Height()+adjustx+25 ,0,0, SWP_NOZORDER|SWP_NOSIZE);
  }

  if(m_enablebutton&IW_SHOWALL)
  {
    m_showall_control.SetWindowPos(0,(rect.Width()-brect.Width())*3/4+140,rect.Height()+adjustx+25 ,0,0, SWP_NOZORDER|SWP_NOSIZE);
    m_overlay_control.SetWindowPos(0,(rect.Width()-brect.Width())*3/4+210,rect.Height()+adjustx+25 ,0,0, SWP_NOZORDER|SWP_NOSIZE);
  }
  if(m_enablebutton&(IW_EDITMAP|IW_GETPOLYGON) )
  {
    if(m_value!=-1)
    {
      m_value_control.SetWindowPos(0,(rect.Width()-brect.Width())/4-20,rect.Height()+adjustx+25 ,0,0, SWP_SHOWWINDOW|SWP_NOZORDER|SWP_NOSIZE);
    }
  }
  if (m_setbutton)
  {
    m_setbutton.ShowWindow(!!(m_enablebutton&IW_PLACEIMAGE));
  }

  if(!(m_enablebutton&IW_NOREDRAW))
  {
    PostMessage(WM_COMMAND,ID_REFRESH,0);//need a redraw for selection of the bottom right side of the tis
  }
}

BOOL CImageView::OnInitDialog() 
{
  CString tmpstr;
  int i;

  if(editflg&LARGEWINDOW)
  {
    m_maxextentx=m_maxextenty=8;//maximum number of tiles fitting on screen
  }
  else
  {
    m_maxextentx=m_maxextenty=winsize;
  }

  if(!m_mos)
  {
    m_mos=&the_mos; 
  }
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
    case MT_HEIGHT8: case MT_HEIGHT: SetWindowText("Edit heightmap: "+itemname+"HT"); break;
    case MT_LIGHT:
      if(the_area.m_night) tmpstr="LN";
      else tmpstr="LM";
      SetWindowText("Edit lightmap: "+itemname+tmpstr);
      break;
    case MT_SEARCH: SetWindowText("Edit searchmap: "+itemname+"SR"); break;
    case MT_BLOCK: SetWindowText("Edit impeded blocks"); break;
    case MT_OVERLAY: SetWindowText("Edit overlaid tiles"); break;
    case MT_EXPLORED: SetWindowText("Edit explored bitmap"); break;
    default:
      UpdateData(UD_DISPLAY);
      return TRUE;
      break;
    }
    m_sourcepoint=0;
  }
  else
  {
    if(m_enablebutton&IW_OVERLAY)
    {
      SetWindowText("Edit overlay");
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
    double elevationy =fabs((double) diffx)/diffy;
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
    double elevationx = fabs((double) diffy)/diffx;
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
  m_bitmap.SetFocus();
  m_confirmed = m_mousepoint;
  if( m_map && (m_enablebutton&IW_OVERLAY))
  {
    CPoint point;
    int pos;
    
    //actually this part now could be put into the parent code
    //and we don't really need to set m_map anymore
    point=GetPoint(GP_TILE);
    pos=point.y*m_mos->mosheader.wColumn+point.x;
    ((wed_tilemap *) m_map)[pos].overlayflags^=m_mos->m_overlay;
    the_area.wedchanged=true;
    m_sourcepoint=GetPoint(GP_TILE); //again???
    if(GetParent())
    {
      GetParent()->PostMessage(WM_COMMAND,ID_REFRESH,0 );
    }
    /*else */ PostMessage(WM_COMMAND,ID_REFRESH,0 );
    return;
  }
  if(m_enablebutton&(IW_SETVERTEX|IW_MARKTILE) )
  {
    m_sourcepoint=GetPoint(GP_TILE);
    if(GetParent())
    {
      GetParent()->PostMessage(WM_COMMAND,ID_REFRESH,0 );
    }
    PostMessage(WM_COMMAND,ID_REFRESH,0 );
    return;
  }

  if( m_map && (m_enablebutton&IW_GETPOLYGON))
  {
    CPoint point;
    int pos;

    point=GetPoint(GP_POINT);
    if (m_maptype==MT_AMBIENT)
    {      
      if ((GetKeyState(VK_SHIFT)<0) && (m_value!=-1) )
      {
        the_area.ambientheaders[m_value].posx = (short) point.x;
        the_area.ambientheaders[m_value].posy = (short) point.y;
        PostMessage(WM_COMMAND,ID_REFRESH,0);
        return;
      }
      else
      {
        pos=FindAmbient((area_ambient *) m_map, point);
      }
    }
    else
    {
      pos=FindPolygon((CPtrList *) m_map, point);
    }
    if(pos!=-1)
    {
      m_value=pos;
      if (m_maptype==MT_AMBIENT)
      {
        //m_clipx = ((area_ambient *) m_map)[pos].posx/m_mos->mosheader.dwBlockSize-TMPX;
        //m_clipy = ((area_ambient *) m_map)[pos].posy/m_mos->mosheader.dwBlockSize-TMPY;
        SetClipSize(((area_ambient *) m_map)[pos].posx/m_mos->mosheader.dwBlockSize-TMPX,
          ((area_ambient *) m_map)[pos].posy/m_mos->mosheader.dwBlockSize-TMPY);
      }
      else
      {
        CenterPolygon((CPtrList *) m_map, pos);
      }
      RedrawContent(); //refreshes scrollbars
    }
    return;
  }

  if(m_map && (m_enablebutton&IW_EDITMAP) )
  {
    CPoint point;
    
    if(m_maptype==MT_EXPLORED)
    {
      point=GetPoint(GP_EXPLORED);
    }
    else
    {
      point=GetPoint(GP_BLOCK);
    }
    if(m_fill)
    {
      the_area.WriteMap("TMP",(LPBYTE) m_map,m_palette,m_max+1);
      FloodFill(point, (unsigned char) m_value);
      m_fill=FALSE;
    }
    else
    {
      PutPixel(point, (unsigned char) m_value);
    }
    switch(m_maptype)
    {
    case MT_EXPLORED:case MT_BLOCK:
      //changed area itself, no flag yet
      break;
    case MT_HEIGHT8:
      the_area.changedmap[0]=true;
      break;
    default:
      the_area.changedmap[m_maptype]=true;
    }
    PostMessage(WM_COMMAND,ID_REFRESH,0);
    return;
  }
  if(m_enablebutton&IW_PLACEIMAGE)
  {
    m_spinx.SetPos(m_confirmed.x);
    m_spiny.SetPos(m_confirmed.y);
    PostMessage(WM_COMMAND,ID_REFRESH,0);
  }

  if (m_enablebutton&IW_AUTOSET)
  {
    PostMessage(WM_COMMAND,IDOK,0 );
  }
}

void CImageView::OnMouseMove(UINT nFlags, CPoint point) 
{
	m_mousepoint=point;
  ScreenToClient(&m_mousepoint);
  if(m_mos)
  {
    m_mousepoint.x+=m_clipx*m_mos->mosheader.dwBlockSize;
    m_mousepoint.y+=m_clipy*m_mos->mosheader.dwBlockSize;
  }
  CDialog::OnMouseMove(nFlags, point);
}

void CImageView::RefreshDialog()
{
  CString tmpstr;
  CPoint point;
  int nReplaced;
  int maxx,maxy;

  if(m_vertical.IsWindowVisible())  m_clipy=m_vertical.GetScrollPos();
  else if(m_clipy<0) m_clipy=0;
  if(m_horizontal.IsWindowVisible()) m_clipx=m_horizontal.GetScrollPos();
  else if(m_clipx<0) m_clipx=0;
  if(m_enablebutton&IW_NOREDRAW)
  {
    return;
  }
  maxx=m_clipx+m_maxextentx;
  maxy=m_clipy+m_maxextenty;
  if(m_maxclipx<maxx) maxx=m_maxclipx;
  if(m_maxclipy<maxy) maxy=m_maxclipy;
  m_mos->MakeBitmapWhole(GREY, m_bm, m_clipx, m_clipy, maxx, maxy, false);
  if(m_enablebutton&IW_MARKTILE)
  {
    point=GetPoint(GP_TILE);
    if(m_map)
    {
      tmpstr.Format("Select overlaid tile... (%d,%d)",point.x,point.y);
    }
    else
    {
      tmpstr.Format("Select tile... (%d,%d)",point.x,point.y);
    }
    SetWindowText(tmpstr);
  }
  else if(m_animbam && (m_enablebutton&IW_PLACEIMAGE|IW_AUTOSET))
  {
    if (m_showall)
    {
      switch(m_maptype) {
      case MT_BAM:
        if (m_map) DrawIcons();
        break;
      case MT_DEFAULT:
        DrawAnims();
        break;
      }
    }

    if(m_enablebutton&IW_MATCH) nReplaced=BM_OVERLAY|BM_MATCH;
    else nReplaced=BM_OVERLAY;
    point=m_confirmed-m_animbam->GetFramePos(m_frame);
    point.x-=m_clipx*m_mos->mosheader.dwBlockSize;
    point.y-=m_clipy*m_mos->mosheader.dwBlockSize;
    nReplaced=m_animbam->MakeBitmap(m_frame,GREY,*m_mos,nReplaced,point.x, point.y);

    point=GetPoint(GP_POINT);
    if(m_enablebutton&IW_MATCH) tmpstr.Format("Place animation... (%d,%d) Match:%d",point.x,point.y,nReplaced);
    else tmpstr.Format("Select point... (%d,%d)",point.x,point.y);
    SetWindowText(tmpstr);
  }
  //lets delete the bitmap before reassigning  
  if(m_bm)
  {
    DeleteObject(m_bm);
    m_bm=NULL;
  }
  MakeBitmapExternal(m_mos->GetDIB(),m_mos->m_pixelwidth,m_mos->m_pixelheight,m_bm);
  m_bitmap.SetBitmap(m_bm);
  if (m_showall)
  {
    switch(m_maptype)
    {
    case MT_DEFAULT:
      DrawActors();
      break;
    case MT_AMBIENT:
      DrawAmbients();
      break;
    case MT_DOORPOLYLIST: case MT_WALLPOLYLIST:
      if(m_enablebutton&IW_SETVERTEX) DrawPolyPolygon(&the_area.wedvertexheaderlist);
      break;
    case MT_BAM:
      DrawMapText();
      break;
    default:
      if(m_enablebutton&IW_SETVERTEX) DrawPolyPolygon(&the_area.vertexheaderlist);
    }
  }
  if(m_enablebutton&IW_SETVERTEX)
  {  
    point=GetPoint(GP_POINT);
    tmpstr.Format("Select point... (%d,%d)",point.x,point.y);
    SetWindowText(tmpstr);
    if(m_vertexcount)
    {
      DrawLines((POINTS *) m_polygon, m_vertexcount,"", m_fill, m_actvertex);
    }
  }
  else if(m_enablebutton&IW_EDITMAP)
  {
    if(m_showall && (m_maptype==MT_BLOCK) )
    {      
      DrawPolyPolygon(&the_area.wedvertexheaderlist);
    }
  }
  else if(m_enablebutton&IW_GETPOLYGON)
  {
    point=GetPoint(GP_POINT);
    if (m_maptype==MT_AMBIENT)
    {
      tmpstr.Format("Select ambient  (%.32s)",the_area.ambientheaders[m_value].ambiname);
      SetWindowText(tmpstr);
      if (!m_showall)
      {      
        DrawCircle(the_area.ambientheaders[m_value].posx, the_area.ambientheaders[m_value].posy, the_area.ambientheaders[m_value].radius, m_value%MAX_COLOR);
      }
    }
    else
    {
      tmpstr.Format("Select polygon... (%d,%d)",point.x,point.y);
      SetWindowText(tmpstr);
      DrawPolyPolygon((CPtrList *) m_map);
    }
  }

  if(m_enablebutton&IW_SHOWGRID)
  {
    DrawGrid();
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

void CImageView::OnOverlay() 
{
  UpdateData(UD_RETRIEVE);
  PostMessage(WM_COMMAND,ID_REFRESH,0);
}

void CImageView::OnSize( UINT /*type*/, int cx, int cy)
{
	CRect rect;
	GetWindowRect(rect);
	//if( (rect.Width() != cx || rect.Height() != cy) && m_bitmap && m_maptype!=-1) 
  if( (rect.Width() != cx || rect.Height() != cy) && m_bitmap) 
	{
    if(m_horizontal && m_horizontal.IsWindowVisible())
    {
      cx-=20;
    }
    if(m_vertical && m_vertical.IsWindowVisible())
    {
      cy-=20;
    }

		rect.left = (cx-20)/64+1;
		rect.top = (cy-80)/64+1;
		//rect.left = (cx)/64;
		//rect.top = (cy)/64;
    if (m_maxextentx!=rect.left || m_maxextenty!=rect.top)
    {
      m_maxextentx=rect.left;
      m_maxextenty=rect.top;
		  RedrawContent();
    }
	}
}

void CImageView::OnSelchangeValue() 
{
	m_value=((CComboBox *) GetDlgItem(IDC_VALUE))->GetCurSel();
  if ((m_maptype==MT_AMBIENT) && (m_value!=-1) )
  {
    //m_clipx = ((area_ambient *) m_map)[m_value].posx/m_mos->mosheader.dwBlockSize-TMPX;
    //m_clipy = ((area_ambient *) m_map)[m_value].posy/m_mos->mosheader.dwBlockSize-TMPY;
    SetClipSize( ((area_ambient *) m_map)[m_value].posx/m_mos->mosheader.dwBlockSize-TMPX,
      ((area_ambient *) m_map)[m_value].posy/m_mos->mosheader.dwBlockSize-TMPY);
  }
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


BOOL CImageView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
  if (m_maptype==MT_AMBIENT)
  {
    if (m_value!=-1)
    {
      int tmp = the_area.ambientheaders[m_value].radius+zDelta;
      if (tmp<0) tmp = 0;
      else if (tmp>16383) tmp=16383;
      the_area.ambientheaders[m_value].radius = (short) tmp;
      PostMessage(WM_COMMAND,ID_REFRESH,0);
    }
  }
	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}

BOOL CImageView::PreTranslateMessage(MSG* pMsg) 
{
  if(!m_mos) return CDialog::PreTranslateMessage(pMsg);
	if(pMsg->hwnd==m_bitmap.m_hWnd) 
  {
    if(pMsg->message==WM_MOUSEMOVE)
    {
      m_mousepoint=pMsg->pt;
      ScreenToClient(&m_mousepoint);
      m_mousepoint-=m_oladjust;
      m_mousepoint.x+=m_clipx*m_mos->mosheader.dwBlockSize;
      m_mousepoint.y+=m_clipy*m_mos->mosheader.dwBlockSize;
    }
    else
    {
      CPoint point;
      
      if(pMsg->message==WM_LBUTTONUP)
      {
        if(m_map && (m_enablebutton&IW_EDITMAP) && (m_sourcepoint.x || m_sourcepoint.y) )
        {
          m_confirmed = m_mousepoint;
          if(m_maptype==MT_EXPLORED)
          {
            point=GetPoint(GP_EXPLORED);
          }
          else
          {
            point=GetPoint(GP_BLOCK);
          }
          DrawLine(m_sourcepoint, point, (unsigned char) m_value);
        }
      }
      else if(pMsg->message==WM_LBUTTONDOWN)
      {
        if(m_map && (m_enablebutton&IW_EDITMAP) )
        {
          m_confirmed = m_mousepoint;
          if(m_maptype==MT_EXPLORED)
          {
            point=GetPoint(GP_EXPLORED);
          }
          else
          {
            point=GetPoint(GP_BLOCK);
          }
          m_sourcepoint=point;
        }
      }
    }
  }
	return CDialog::PreTranslateMessage(pMsg);
}

void CImageView::PostNcDestroy() 
{
  if(m_bm)
  {
    DeleteObject(m_bm);
    m_bm=NULL;
  }
	CDialog::PostNcDestroy();
}
