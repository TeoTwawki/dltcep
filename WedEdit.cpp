// WedEdit.cpp : implementation file
//

#include "stdafx.h"
#include "chitem.h"
#include "WedEdit.h"
#include "WedPolygon.h"
#include "WedTile.h"
#include "TisDialog.h"
#include "MosEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWedEdit dialog

CWedEdit::CWedEdit(CWnd* pParent /*=NULL*/)	: CDialog(CWedEdit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWedEdit)
	m_maxoverlay = _T("");
	m_maxdoor = _T("");
	m_maxwallgroup = _T("");
	m_maxdoorpoly = _T("");
	m_open = FALSE;
	m_polygontype = _T("");
	m_problem = _T("");
	m_whole = FALSE;
	//}}AFX_DATA_INIT
  m_overlaynum=-1;
  m_doornum=-1;
  m_repair=false;
}

static int overlayboxids[]={IDC_TILESET, IDC_WIDTH, IDC_HEIGHT, IDC_UNKNOWN0C,
IDC_REMOVE, IDC_EDIT, IDC_EDITTILE, IDC_OVERLAY,
0};

static int doorboxids[]={IDC_DOORID, IDC_CLOSED, IDC_DOORPOLYPICKER, IDC_DOORPOLYNUM,
IDC_EDITDOORPOLY,
0};

static int wallgroupboxids[]={IDC_WALLGROUPPICKER, IDC_EDITWALLPOLY, IDC_SELECTION2,
0};

void CWedEdit::DoDataExchange(CDataExchange* pDX)
{
  CWnd *cb;
  CString tmpstr, tmpstr2;
  int tmp;
  int flg, flg2;
  int i;

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWedEdit)
	DDX_Control(pDX, IDC_DOORPOLYPICKER, m_doorpolypicker_control);
	DDX_Control(pDX, IDC_WALLGROUPPICKER, m_wallgrouppicker_control);
	DDX_Control(pDX, IDC_OVERLAYPICKER, m_overlaypicker_control);
	DDX_Control(pDX, IDC_DOORPICKER, m_doorpicker_control);
	DDX_Text(pDX, IDC_OVERLAYNUM, m_maxoverlay);
	DDX_Text(pDX, IDC_DOORNUM, m_maxdoor);
	DDX_Text(pDX, IDC_WALLGROUPNUM, m_maxwallgroup);
	DDX_Text(pDX, IDC_DOORPOLYNUM, m_maxdoorpoly);
	DDX_Check(pDX, IDC_OPEN, m_open);
	DDX_Text(pDX, IDC_POLYGONTYPE, m_polygontype);
	DDX_Text(pDX, IDC_PROBLEM, m_problem);
	DDX_Check(pDX, IDC_OVERLAY, m_whole);
	//}}AFX_DATA_MAP
  /// OVERLAY ///
  flg=the_area.overlaycount>0;
  i=0;
  while(overlayboxids[i])
  {
    if(i && flg)
    {
      flg2=the_area.overlayheaders[m_overlaynum].tis[0];
    }
    else flg2=flg;
    cb=GetDlgItem(overlayboxids[i++]);
    cb->EnableWindow(flg2);
  }
  if(flg && m_overlaynum>=0)
  {
    RetrieveResref(tmpstr, the_area.overlayheaders[m_overlaynum].tis);
    tmpstr2=tmpstr;
    DDX_Text(pDX, IDC_TILESET, tmpstr);
    DDV_MaxChars(pDX, tmpstr, 8);
    if(tmpstr!=tmpstr2) the_area.wedchanged=true;
    StoreResref(tmpstr, the_area.overlayheaders[m_overlaynum].tis);

    tmp=the_area.overlayheaders[m_overlaynum].width;
    DDX_Text(pDX, IDC_WIDTH, the_area.overlayheaders[m_overlaynum].width);
    if(tmp!=the_area.overlayheaders[m_overlaynum].width) the_area.wedchanged=true;

    tmp=the_area.overlayheaders[m_overlaynum].height;
    DDX_Text(pDX, IDC_HEIGHT, the_area.overlayheaders[m_overlaynum].height);
    if(tmp!=the_area.overlayheaders[m_overlaynum].height) the_area.wedchanged=true;

    DDX_Text(pDX, IDC_UNKNOWN0C, the_area.overlayheaders[m_overlaynum].unknownc);
  }

  /// DOOR ///
  flg=the_area.weddoorcount>0;
  i=0;
  while(doorboxids[i])
  {
    cb=GetDlgItem(doorboxids[i++]);
    cb->EnableWindow(flg);
  }
  flg=(flg && m_doornum>=0);
  if(flg)
  {
    RetrieveResref(tmpstr, the_area.weddoorheaders[m_doornum].doorid);
    tmpstr2=tmpstr;
    DDX_Text(pDX, IDC_DOORID, tmpstr);
    DDV_MaxChars(pDX, tmpstr, 8);
    if(tmpstr!=tmpstr2) the_area.wedchanged=true;
    StoreResref(tmpstr, the_area.weddoorheaders[m_doornum].doorid);

    tmp=the_area.weddoorheaders[m_doornum].closed;
    DDX_Text(pDX, IDC_CLOSED, the_area.weddoorheaders[m_doornum].closed);
    if(tmp!=the_area.weddoorheaders[m_doornum].closed) the_area.wedchanged=true;
  }
  cb=GetDlgItem(IDC_EDITDOORPOLY);
  cb->EnableWindow(flg && m_doorpolynum>=0);

  /// WALLGROUP ///
  flg=the_area.wallpolygoncount>0;
  i=0;
  while(wallgroupboxids[i])
  {
    cb=GetDlgItem(wallgroupboxids[i++]);
    cb->EnableWindow(flg);
  }
}
void CWedEdit::RefreshOverlay()
{
  CString tisname;
  loc_entry fileloc;
  int size;
  int fhandle;
  int i;

  the_area.overlaytilecount=0;
  for(i=0;i<the_area.overlaycount;i++)
  {
    size=the_area.overlayheaders[i].width*the_area.overlayheaders[i].height;
    the_area.overlaytilecount+=size;
  }
  /*
  for(i=0;i<the_area.doorcount;i++)
  {
    the_area.overlaytilecount+=the_area.weddoorheaders[i].countdoortileidx;
  }
  */
  size=the_area.overlayheaders[m_overlaynum].width*the_area.overlayheaders[m_overlaynum].height;
  if(!m_overlaynum)
  {
    for(i=0;i<the_area.doorcount;i++)
    {
      size+=the_area.weddoorheaders[i].countdoortileidx;
    }
  }
  RetrieveResref(tisname,the_area.overlayheaders[m_overlaynum].tis);
  if(!tis.Lookup(tisname,fileloc) )
  {
    m_problem="Not found";
    return;
  }
  fhandle=locate_file(fileloc, 0);
  if(fhandle<1)
  {
    m_problem="Not found";
    return;
  }
  tis.SetAt(tisname,fileloc);
  if(!m_overlaynum && size<the_mos.RetrieveTisFrameCount(fhandle,fileloc.size) )
  {
    m_problem="Doesn't match area dimensions";
  }
  else m_problem="";
  close(fhandle);
}

void CWedEdit::RefreshWed()
{
  CWnd *cw;
  CString tmpstr;
  int i;
  int count, first;

  RetrieveResref(tmpstr,the_area.header.wed);
  if(the_area.m_night) tmpstr+="N";
  if(the_area.wedchanged) tmpstr+=" *";
  SetWindowText("Edit Wed: "+tmpstr);
  //// OVERLAY ////
  if(m_overlaypicker_control)
  {
    m_overlaypicker_control.ResetContent();
    for(i=0;i<the_area.overlaycount;i++)
    {
      tmpstr.Format("%d %-.8s %d*%d",i+1,the_area.overlayheaders[i].tis,
        the_area.overlayheaders[i].width,the_area.overlayheaders[i].height);
      m_overlaypicker_control.AddString(tmpstr);
    }
    m_maxoverlay.Format("/ %d",i);
    if(m_overlaynum<0) m_overlaynum=0;
    m_overlaynum=m_overlaypicker_control.SetCurSel(m_overlaynum);

  }

  //// DOOR ////
  if(m_doorpicker_control)
  {
    m_doorpicker_control.ResetContent();
    for(i=0;i<the_area.weddoorcount;i++)
    {
      tmpstr.Format("%d %-.8s",i+1,the_area.weddoorheaders[i].doorid);
      m_doorpicker_control.AddString(tmpstr);
    }
    m_maxdoor.Format("/ %d",i);
    if(m_doornum<0) m_doornum=0;
    m_doornum=m_doorpicker_control.SetCurSel(m_doornum);

    if(m_doornum>=0)
    {
      m_doorpolypicker_control.ResetContent();
      if(m_open)
      {
        count=the_area.weddoorheaders[m_doornum].countpolygonopen;
        first=the_area.weddoorheaders[m_doornum].offsetpolygonopen;
      }
      else
      {
        count=the_area.weddoorheaders[m_doornum].countpolygonclose;
        first=the_area.weddoorheaders[m_doornum].offsetpolygonclose;
      }
      for(i=0;i<count;i++)
      {
        tmpstr.Format("%d #%d 0x%x", i+1, the_area.doorpolygonheaders[i+first].countvertex,
          the_area.doorpolygonheaders[i+first].flags);
        m_doorpolypicker_control.AddString(tmpstr);
      }
      m_maxdoorpoly.Format("/ %d",i);
      if(m_doorpolynum<0) m_doorpolynum=0;
      m_doorpolynum=m_doorpolypicker_control.SetCurSel(m_doorpolynum);
    }
    cw=GetDlgItem(IDC_OPEN);
    if(m_open)
    {
      tmpstr="open";
    }
    else
    {
      tmpstr="closed";
    }
    cw->SetWindowText(tmpstr);
    m_polygontype.Format("Choose polygon of %s door",tmpstr);
  }

  //// WALLGROUP ////
  if(m_wallgrouppicker_control)
  {
    m_wallgrouppicker_control.ResetContent();
    for(i=0;i<the_area.wallpolygoncount;i++)
    {
      tmpstr.Format("%d #%d 0x%x",i+1,the_area.wallpolygonheaders[i].countvertex,
        the_area.wallpolygonheaders[i].flags);
      m_wallgrouppicker_control.AddString(tmpstr);
    }
    m_maxwallgroup.Format("/ %d",i);
    if(m_wallgroupnum<0) m_wallgroupnum=0;
    m_wallgroupnum=m_wallgrouppicker_control.SetCurSel(m_wallgroupnum);
  }
}

int readonlyids[]={IDC_TILESET, IDC_WIDTH, IDC_HEIGHT,0};

BOOL CWedEdit::OnInitDialog() 
{
  CEdit *ce;
  int i;

	CDialog::OnInitDialog();
  //we must catch all changes individually
  //the_area.wedchanged=true;
  for(i=0;readonlyids[i];i++)
  {
    ce=(CEdit *) GetDlgItem(readonlyids[i]);
    ce->SetReadOnly(!m_repair);
  }
	RefreshWed();
  //tooltips
  {
    m_tooltip.Create(this,TTS_NOPREFIX);
    m_tooltip.SetMaxTipWidth(200);
    m_tooltip.SetTipBkColor(RGB(240,224,160));
    
    m_tooltip.AddTool(GetDlgItem(IDOK), IDS_CANCEL);
    m_tooltip.AddTool(GetDlgItem(IDC_EDIT), IDS_TISEXTRACT);
    m_tooltip.AddTool(GetDlgItem(IDC_CLEAR), IDS_CLEANUP);
    m_tooltip.AddTool(GetDlgItem(IDC_NEW), IDS_OVERLAY);
    m_tooltip.AddTool(GetDlgItem(IDC_REMOVE), IDS_DELOVER);
    m_tooltip.AddTool(GetDlgItem(IDC_EDITTILE), IDS_TILES);
    m_tooltip.AddTool(GetDlgItem(IDC_ORDER), IDS_ORDER);
  }
	UpdateData(UD_DISPLAY);
	return TRUE;
}

BEGIN_MESSAGE_MAP(CWedEdit, CDialog)
	//{{AFX_MSG_MAP(CWedEdit)
	ON_CBN_KILLFOCUS(IDC_OVERLAYPICKER, OnKillfocusOverlaypicker)
	ON_CBN_SELCHANGE(IDC_OVERLAYPICKER, OnSelchangeOverlaypicker)
	ON_EN_KILLFOCUS(IDC_TILESET, OnKillfocusTileset)
	ON_EN_KILLFOCUS(IDC_WIDTH, OnKillfocusWidth)
	ON_EN_KILLFOCUS(IDC_HEIGHT, OnKillfocusHeight)
	ON_EN_KILLFOCUS(IDC_UNKNOWN0C, OnKillfocusUnknown0c)
	ON_CBN_KILLFOCUS(IDC_DOORPICKER, OnKillfocusDoorpicker)
	ON_CBN_SELCHANGE(IDC_DOORPICKER, OnSelchangeDoorpicker)
	ON_EN_KILLFOCUS(IDC_DOORID, OnKillfocusDoorid)
	ON_EN_KILLFOCUS(IDC_CLOSED, OnKillfocusClosed)
	ON_CBN_KILLFOCUS(IDC_WALLGROUPPICKER, OnKillfocusWallgrouppicker)
	ON_CBN_SELCHANGE(IDC_WALLGROUPPICKER, OnSelchangeWallgrouppicker)
	ON_BN_CLICKED(IDC_EDITDOORPOLY, OnEditdoorpoly)
	ON_BN_CLICKED(IDC_EDITWALLPOLY, OnEditwallpoly)
	ON_CBN_KILLFOCUS(IDC_DOORPOLYPICKER, OnKillfocusDoorpolypicker)
	ON_CBN_SELCHANGE(IDC_DOORPOLYPICKER, OnSelchangeDoorpolypicker)
	ON_BN_CLICKED(IDC_OPEN, OnOpen)
	ON_BN_CLICKED(IDC_ORDER, OnOrder)
	ON_BN_CLICKED(IDC_CLEAR, OnClear)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	ON_BN_CLICKED(IDC_NEW, OnNew)
	ON_BN_CLICKED(IDC_EDITTILE, OnEdittile)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_OVERLAY, OnOverlay)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_SELECTION, OnSelection)
	ON_BN_CLICKED(IDC_SELECTION2, OnSelection2)
	ON_BN_CLICKED(IDC_ADD3, OnAdd3)
	ON_BN_CLICKED(IDC_ADD2, OnAdd2)
	ON_BN_CLICKED(IDC_REMOVE2, OnRemove2)
	ON_BN_CLICKED(IDC_REMOVE3, OnRemove3)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWedEdit message handlers

void CWedEdit::OnKillfocusOverlaypicker() 
{
  CString tmpstr;
  int x;

  m_overlaypicker_control.GetWindowText(tmpstr);
  x=strtonum(tmpstr)-1;
  if(x>=0 && x<=the_area.entrancecount)
  {
    m_overlaynum=m_overlaypicker_control.SetCurSel(x);
  }
  RefreshWed();
	UpdateData(UD_DISPLAY);	
}

void CWedEdit::OnSelchangeOverlaypicker() 
{
  int x;

  x=m_overlaypicker_control.GetCurSel();
  if(x>=0 && x<=the_area.overlaycount) m_overlaynum=x;
  RefreshWed();
	UpdateData(UD_DISPLAY);	
}

void CWedEdit::OnKillfocusTileset() 
{
	UpdateData(UD_RETRIEVE);
  RefreshOverlay();
  RefreshWed();
	UpdateData(UD_DISPLAY);	
}

void CWedEdit::OnKillfocusWidth() 
{
	UpdateData(UD_RETRIEVE);
  RefreshOverlay();
  RefreshWed();
	UpdateData(UD_DISPLAY);	
}

void CWedEdit::OnKillfocusHeight() 
{
	UpdateData(UD_RETRIEVE);	
  RefreshOverlay();
  RefreshWed();
	UpdateData(UD_DISPLAY);	
}

void CWedEdit::OnKillfocusUnknown0c() 
{
	UpdateData(UD_RETRIEVE);	
	UpdateData(UD_DISPLAY);	
}
//////////////// DOOR /////////////////////

void CWedEdit::OnKillfocusDoorpicker() 
{
  CString tmpstr;
  int x;

  m_doorpicker_control.GetWindowText(tmpstr);
  x=strtonum(tmpstr)-1;
  if(x>=0 && x<=the_area.weddoorcount)
  {
    m_doornum=m_doorpicker_control.SetCurSel(x);
  }
  RefreshWed();
	UpdateData(UD_DISPLAY);	
}

void CWedEdit::OnSelchangeDoorpicker() 
{
  int x;

  x=m_doorpicker_control.GetCurSel();
  if(x>=0 && x<=the_area.weddoorcount) m_doornum=x;
  RefreshWed();
	UpdateData(UD_DISPLAY);	
}

void CWedEdit::OnKillfocusDoorid() 
{
	UpdateData(UD_RETRIEVE);	
  RefreshWed();
	UpdateData(UD_DISPLAY);	
}

void CWedEdit::OnKillfocusClosed() 
{
	UpdateData(UD_RETRIEVE);	
  RefreshWed();
	UpdateData(UD_DISPLAY);	
}

void CWedEdit::OnOverlay() 
{
	UpdateData(UD_RETRIEVE);	
	UpdateData(UD_DISPLAY);	
}

void CWedEdit::OnOpen() 
{
  m_open=!m_open;
	RefreshWed();
	UpdateData(UD_DISPLAY);	
}

void CWedEdit::OnKillfocusDoorpolypicker() 
{
  CString tmpstr;
  int x,max;

  m_doorpolypicker_control.GetWindowText(tmpstr);
  x=strtonum(tmpstr)-1;
  if(m_open) max=the_area.weddoorheaders[m_doornum].countpolygonopen;
  else max=the_area.weddoorheaders[m_doornum].countpolygonclose;
  if(x>=0 && x<=max)
  {
    m_doorpolynum=m_doorpolypicker_control.SetCurSel(x);
  }
	UpdateData(UD_DISPLAY);	
}

void CWedEdit::OnSelchangeDoorpolypicker() 
{
  int x,max;

  x=m_doorpolypicker_control.GetCurSel();
  if(m_open) max=the_area.weddoorheaders[m_doornum].countpolygonopen;
  else max=the_area.weddoorheaders[m_doornum].countpolygonclose;
  if(x>=0 && x<=max) m_doorpolynum=x;
	UpdateData(UD_DISPLAY);	
}

void CWedEdit::OnAdd() 
{
  CString tmpstr;
	CTisDialog dlg;
  
  UpdateData(UD_RETRIEVE);
  RetrieveResref(tmpstr, the_area.overlayheaders[0].tis);
  dlg.setrange(tmpstr, the_area.overlayheaders[0].width,
    the_area.overlayheaders[0].height, 7);//additional flag means setting location
  dlg.m_overlaynum=0;
  dlg.m_tileheaders=the_area.overlaytileheaders;
  dlg.m_tileindices=the_area.overlaytileindices;
  dlg.DoModal();
  UpdateData(UD_DISPLAY);
}

void CWedEdit::OnAdd3() 
{
  area_vertex *newelement;
  wed_polygon *newpolygons;
  POSITION pos;
  int polycount, first;
  int i;

  if(m_doornum<0) return;
  newelement = new area_vertex[0];
  if(!newelement) return;
  newpolygons = new wed_polygon [the_area.doorpolygoncount+1];
  if(!newpolygons)
  {
    delete [] newelement;
    return;
  }

  if(m_open)
  {
    polycount=the_area.weddoorheaders[m_doornum].countpolygonopen++;
    first=polycount+the_area.weddoorheaders[m_doornum].offsetpolygonopen;
    if(the_area.weddoorheaders[m_doornum].offsetpolygonclose>=first)
    {
      the_area.weddoorheaders[m_doornum].offsetpolygonclose++;
    }
  }
  else
  {
    polycount=the_area.weddoorheaders[m_doornum].countpolygonclose++;
    first=polycount+the_area.weddoorheaders[m_doornum].offsetpolygonclose;
    if(the_area.weddoorheaders[m_doornum].offsetpolygonopen>=first)
    {
      the_area.weddoorheaders[m_doornum].offsetpolygonopen++;
    }
  }
  for(i=m_doornum+1;i<the_area.weddoorcount;i++)
  {
    the_area.weddoorheaders[i].offsetpolygonclose++;
    the_area.weddoorheaders[i].offsetpolygonopen++;
  }
  pos = the_area.wedvertexheaderlist.FindIndex(first);
  memcpy(newpolygons,the_area.doorpolygonheaders,first*sizeof(wed_polygon) );
  memcpy(newpolygons+first+1,the_area.doorpolygonheaders+first,(the_area.doorpolygoncount-first)*sizeof(wed_polygon) );
  memset(newpolygons+first,0,sizeof(wed_polygon) );
  newpolygons[first].flags=0x89; //standard door flags
  delete [] the_area.doorpolygonheaders;
  the_area.doorpolygonheaders=newpolygons;
  the_area.wedvertexheaderlist.InsertBefore(pos, newelement);
  the_area.doorpolygoncount++;
  RefreshWed();
  UpdateData(UD_DISPLAY);
}

void CWedEdit::OnRemove3() 
{
	wed_polygon *newpolygons;
  POSITION pos;
  int polycount, first;
	
  if(m_open)
  {
    polycount=the_area.weddoorheaders[m_doornum].countpolygonopen;
    first=polycount+the_area.weddoorheaders[m_doornum].offsetpolygonopen;
    if(the_area.weddoorheaders[m_doornum].offsetpolygonclose>=first)
    {
      the_area.weddoorheaders[m_doornum].offsetpolygonclose--;
    }
  }
  else
  {
    polycount=the_area.weddoorheaders[m_doornum].countpolygonclose;
    first=polycount+the_area.weddoorheaders[m_doornum].offsetpolygonclose;
  }
  pos=the_area.wedvertexheaderlist.FindIndex(the_area.wallpolygoncount+first);
  if(!pos) return;
  newpolygons=new wed_polygon[--the_area.doorpolygoncount];
  if(!newpolygons)
  {
    the_area.secheader.wallpolycnt++;
    return;
  }
  memcpy(newpolygons, the_area.doorpolygonheaders, m_wallgroupnum*sizeof(wed_polygon));
  memcpy(newpolygons+m_wallgroupnum, the_area.doorpolygonheaders+m_wallgroupnum+1,(the_area.secheader.wallpolycnt-m_wallgroupnum)*sizeof(wed_polygon) );
  delete [] the_area.weddoorheaders;
  the_area.doorpolygonheaders=newpolygons;
  the_area.wedvertexheaderlist.RemoveAt(pos);//this also frees the polygon
  RefreshWed();
  UpdateData(UD_DISPLAY);
}

void CWedEdit::OnEditdoorpoly() 
{
  POSITION pos;
  CWedPolygon dlg;
  int first;
	
  if(m_doorpolynum<0) return;
  if(m_open) first=the_area.weddoorheaders[m_doornum].offsetpolygonopen+m_doorpolynum;
  else first=the_area.weddoorheaders[m_doornum].offsetpolygonclose+m_doorpolynum;  
  pos=the_area.wedvertexheaderlist.FindIndex(first+the_area.wallpolygoncount);
  dlg.SetPolygon(the_area.doorpolygonheaders+first, pos);
  dlg.m_open=m_open;
  dlg.DoModal();
  if(dlg.m_changed) the_area.wedchanged=true;
  RefreshWed();
  UpdateData(UD_DISPLAY);
}

int CheckIntervallum(int value, int first, int count)
{
  if(value<first) return -1;
  if(value>=first+count) return -1;
  return value-first;
}

void CWedEdit::OnSelection() 
{
	CImageView dlg;
  int first;
  
  if(SetupSelectPoint())
  {
    return;
  }

  if(m_open) first=the_area.weddoorheaders[m_doornum].offsetpolygonopen+m_doorpolynum;
  else first=the_area.weddoorheaders[m_doornum].offsetpolygonclose+m_doorpolynum;
  dlg.SetMapType(MT_DOORPOLYLIST, (unsigned char *) &the_area.wedvertexheaderlist);
  dlg.m_max=the_area.doorpolygoncount;
  dlg.m_value=first;
  dlg.InitView(IW_SHOWGRID|IW_GETPOLYGON|IW_ENABLEBUTTON, &the_mos);
  dlg.DoModal();
  for(m_doornum=0;m_doornum<the_area.doorpolygoncount;m_doornum++)
  {
    m_doorpolynum=CheckIntervallum(dlg.m_value, the_area.weddoorheaders[m_doornum].offsetpolygonopen,the_area.weddoorheaders[m_doornum].countpolygonopen);
    if(m_doorpolynum!=-1)
    {
      m_open=1;
      break;
    }
    m_doorpolynum=CheckIntervallum(dlg.m_value, the_area.weddoorheaders[m_doornum].offsetpolygonclose,the_area.weddoorheaders[m_doornum].countpolygonclose);
    if(m_doorpolynum!=-1)
    {
      m_open=0;
      break;
    }
  }
  RefreshWed();
}

//////////////// WALLGROUP /////////////////////

void CWedEdit::OnKillfocusWallgrouppicker() 
{
  CString tmpstr;
  int x;

  m_wallgrouppicker_control.GetWindowText(tmpstr);
  x=strtonum(tmpstr)-1;
  if(x>=0 && x<=the_area.wallpolygoncount)
  {
    m_wallgroupnum=m_wallgrouppicker_control.SetCurSel(x);
  }
  RefreshWed();
	UpdateData(UD_DISPLAY);	
}

void CWedEdit::OnSelchangeWallgrouppicker() 
{
  int x;

  x=m_wallgrouppicker_control.GetCurSel();
  if(x>=0 && x<=the_area.wallpolygoncount) m_wallgroupnum=x;
  RefreshWed();
	UpdateData(UD_DISPLAY);	
}

void CWedEdit::OnOrder() 
{
  POSITION pos;
  int i;
  int ret;

  pos=the_area.wedvertexheaderlist.GetHeadPosition();
	for(i=0;i<the_area.wallpolygoncount;i++)
  {
    ret=VertexOrder((area_vertex *) the_area.wedvertexheaderlist.GetNext(pos),the_area.wallpolygonheaders[i].countvertex);
    if(ret<0) return;
    if(ret) the_area.wedchanged=true;
  }
	for(i=0;i<the_area.doorpolygoncount;i++)
  {
    ret=VertexOrder((area_vertex *) the_area.wedvertexheaderlist.GetNext(pos),the_area.doorpolygonheaders[i].countvertex);
    if(ret<0) return;
    if(ret) the_area.wedchanged=true;
  }
	UpdateData(UD_DISPLAY);
}

void CWedEdit::OnRemove2() 
{
	wed_polygon *newpolygons;
  POSITION pos;
	
  pos=the_area.wedvertexheaderlist.FindIndex(m_wallgroupnum);
  if(!pos) return;
  newpolygons=new wed_polygon[--the_area.secheader.wallpolycnt];
  if(!newpolygons)
  {
    the_area.secheader.wallpolycnt++;
    return;
  }
  memcpy(newpolygons, the_area.wallpolygonheaders, m_wallgroupnum*sizeof(wed_polygon));
  memcpy(newpolygons+m_wallgroupnum, the_area.wallpolygonheaders+m_wallgroupnum+1,(the_area.secheader.wallpolycnt-m_wallgroupnum)*sizeof(wed_polygon) );
  delete [] the_area.wallpolygonheaders;
  the_area.wallpolygonheaders=newpolygons;
  the_area.wedvertexheaderlist.RemoveAt(pos);//this also frees the polygon
  the_area.wallpolygoncount=the_area.secheader.wallpolycnt;
  RefreshWed();
  UpdateData(UD_DISPLAY);
}

void CWedEdit::OnAdd2() 
{
  area_vertex *newelement;
  wed_polygon *newpolygons;
  POSITION pos;

  pos=the_area.wedvertexheaderlist.FindIndex(m_wallgroupnum);
  if(!pos) return;
  newelement = new area_vertex[0];
  if(!newelement) return;
  newpolygons = new wed_polygon [the_area.secheader.wallpolycnt+1];
  if(!newpolygons)
  {
    delete [] newelement;
    return;
  }

  pos = the_area.wedvertexheaderlist.FindIndex(m_wallgroupnum);
  memcpy(newpolygons,the_area.wallpolygonheaders,m_wallgroupnum*sizeof(wed_polygon) );
  memcpy(newpolygons+m_wallgroupnum+1,the_area.wallpolygonheaders+m_wallgroupnum,(the_area.secheader.wallpolycnt-m_wallgroupnum)*sizeof(wed_polygon) );
  memset(newpolygons+m_wallgroupnum,0,sizeof(wed_polygon) );
  delete [] the_area.wallpolygonheaders;
  the_area.wallpolygonheaders=newpolygons;
  the_area.wedvertexheaderlist.InsertBefore(pos, newelement);
  the_area.secheader.wallpolycnt++;
  the_area.wallpolygoncount=the_area.secheader.wallpolycnt;
  the_area.RecalcBoundingBoxes();
  RefreshWed();
  UpdateData(UD_DISPLAY);
}

void CWedEdit::OnSelection2() 
{
	CImageView dlg;
  
  if(SetupSelectPoint())
  {
    return;
  }

  dlg.SetMapType(MT_WALLPOLYLIST, (unsigned char *) &the_area.wedvertexheaderlist);
  dlg.m_value=m_wallgroupnum;
  dlg.m_max=the_area.wallpolygoncount;
  dlg.InitView(IW_SHOWGRID|IW_GETPOLYGON|IW_ENABLEBUTTON, &the_mos);
  dlg.DoModal();
  m_wallgroupnum=dlg.m_value;
  RefreshWed();  
}

void CWedEdit::OnEditwallpoly() 
{
  POSITION pos;
	CWedPolygon dlg;
	
  if(m_wallgroupnum<0) return;
  pos=the_area.wedvertexheaderlist.FindIndex(m_wallgroupnum);
  dlg.SetPolygon(the_area.wallpolygonheaders+m_wallgroupnum,pos);
  dlg.DoModal();
  if(dlg.m_changed) the_area.wedchanged=true;
  RefreshWed();
  UpdateData(UD_DISPLAY);
}

void CWedEdit::OnClear() 
{
	int ret;
	
  ret=the_area.RecalcBoundingBoxes();
  ret=the_area.ConsolidateDoortiles();
  if(ret)
  {
    the_area.wedchanged=true;
    MessageBox("Changed something.","Area editor",MB_ICONINFORMATION|MB_OK);
  }
  RefreshWed();
  UpdateData(UD_DISPLAY);
}

// edit tileset (extract door tiles)
void CWedEdit::OnEdit() 
{
  Cmos overlay;
  CString tmpstr;
	CTisDialog dlg;
  int baseoverlay;
  
  UpdateData(UD_RETRIEVE);
  if(m_whole) baseoverlay=0;
  else baseoverlay=m_overlaynum;
  RetrieveResref(tmpstr, the_area.overlayheaders[baseoverlay].tis);
  dlg.setrange(tmpstr, the_area.overlayheaders[baseoverlay].width,
    the_area.overlayheaders[baseoverlay].height, 3);
  dlg.m_overlaynum=m_overlaynum;
  dlg.m_tileheaders=the_area.overlaytileheaders+the_area.getotc(baseoverlay);
  dlg.m_tileindices=the_area.overlaytileindices+the_area.getfoti(baseoverlay);
  if(baseoverlay!=m_overlaynum)
  {
    RetrieveResref(tmpstr, the_area.overlayheaders[m_overlaynum].tis);
    read_tis(tmpstr, &overlay, false);
  }
  dlg.DoModal();
  UpdateData(UD_DISPLAY);
}

//clears a tileset
void CWedEdit::OnRemove() 
{
  int i;
  int tilecount;
  wed_tilemap *newtileheaders;

  if(m_overlaynum==0)
  {
    MessageBox("Can't remove the first overlay!","Area editor",MB_ICONSTOP|MB_OK);
    return;
  }
  tilecount = the_area.getotc(1);
  //removing the overlay marker bits
  the_area.wedchanged=true;
  for(i=0;i<tilecount;i++)
  {
    the_area.overlaytileheaders[i].flags&=~(1<<m_overlaynum);
  }
  tilecount = the_area.overlayheaders[m_overlaynum].height*the_area.overlayheaders[m_overlaynum].width; //removed tilecount
  newtileheaders = new wed_tilemap[the_area.overlaytilecount-tilecount];
  if(!newtileheaders)
  {
    MessageBox("Out of memory","Area editor",MB_ICONSTOP|MB_OK);
    return;
  }
  the_area.overlaytilecount-=tilecount;
  i=the_area.getotc(m_overlaynum);
  memcpy(newtileheaders, the_area.overlaytileheaders,  i*sizeof(wed_tilemap) );
  memcpy(newtileheaders + i, the_area.overlaytileheaders+the_area.getotc(m_overlaynum+1), (the_area.overlaytilecount-i) *sizeof(wed_tilemap));
  memset(the_area.overlayheaders+m_overlaynum,0,sizeof(wed_overlay));
  delete [] the_area.overlaytileheaders;
  the_area.overlaytileheaders=newtileheaders;
  RefreshOverlay();
  RefreshWed();
	UpdateData(UD_DISPLAY);	
}

//adds new tileset
void CWedEdit::OnNew() 
{
  CString tmpstr;
	CMosEdit dlg;
  wed_tilemap *newtileheaders;
  wed_polyidx *newwgc;
  short *newindices;
  int tilecount;
  int i;
  int wgc;
	 
  if(m_overlaynum<0 || m_overlaynum>=the_area.overlaycount)
  {
    return;
  }
  tmpstr=itemname;
  dlg.NewMos(TM_TIS|TM_CONFIRM); //true = TIS

  if(dlg.DoModal()==IDCANCEL)
  {
    itemname=tmpstr;
    return;
  }
  the_area.wedchanged=true;
  tilecount=the_mos.tisheader.numtiles;
  
  StoreResref(itemname,the_area.overlayheaders[m_overlaynum].tis);
  itemname=tmpstr;
  //setting up overlaytileindices

  tilecount=the_mos.mosheader.wColumn*the_mos.mosheader.wRow;

  //wallgroupindices for first overlay
  newindices=new short[the_area.overlaytileidxcount+tilecount];
  if(!newindices)
  {
    return;
  }
  newtileheaders=new wed_tilemap[the_area.overlaytilecount+tilecount];
  if(!newtileheaders)
  {
    delete [] newindices;
    return;
  }

  if(m_overlaynum==0)
  {
    if(the_area.wallgroupindices) delete [] the_area.wallgroupindices;
    wgc=((the_mos.mosheader.wColumn+9)/10)*((the_mos.mosheader.wRow*2+14)/15);
    newwgc=new wed_polyidx[wgc];
    if(!newwgc)
    {
      delete [] newindices;
      delete [] newtileheaders;
      the_area.wallgroupindices=NULL;
      the_area.wallgroupidxcount=0;
      return;
    }
    memset(newwgc,0,sizeof(wed_polyidx)*wgc);
    the_area.wallgroupindices=newwgc;
    the_area.wallgroupidxcount=wgc;
  }

  memcpy(newtileheaders,the_area.overlaytileheaders,the_area.overlaytilecount*sizeof(wed_tilemap) );
  memset(newtileheaders+the_area.overlaytilecount,0,tilecount*sizeof(wed_tilemap) );

  memcpy(newindices,the_area.overlaytileindices,the_area.overlaytileidxcount*sizeof(short) );
  i=tilecount;
  while(i--)
  {
    newindices[the_area.overlaytileidxcount+i]=(short) i;
    newtileheaders[the_area.overlaytilecount+i].firsttileprimary=(short) i;
    newtileheaders[the_area.overlaytilecount+i].counttileprimary=1;
    newtileheaders[the_area.overlaytilecount+i].alternate=-1;
  }

  delete [] the_area.overlaytileheaders;
  the_area.overlaytileheaders=newtileheaders;

  delete [] the_area.overlaytileindices;
  the_area.overlaytileindices=newindices;

  the_area.overlaytilecount+=tilecount;
  the_area.overlaytileidxcount+=tilecount;

  the_area.overlayheaders[m_overlaynum].tileidxoffset=-1;
  the_area.overlayheaders[m_overlaynum].tilemapoffset=-1;
  the_area.overlayheaders[m_overlaynum].width=the_mos.mosheader.wColumn;
  the_area.overlayheaders[m_overlaynum].height=the_mos.mosheader.wRow;
  the_area.overlayheaders[m_overlaynum].unknownc=0;

  RefreshOverlay();
  RefreshWed();
	UpdateData(UD_DISPLAY);	
}

void CWedEdit::OnEdittile() 
{
  Cmos overlay;
  CWedTile dlg;
  CString tisname;
  int x,y;
  int baseoverlay;

  if(m_overlaynum<0) return;
  if(m_whole) baseoverlay=0;
  else baseoverlay=m_overlaynum;
  RetrieveResref(tisname, the_area.overlayheaders[baseoverlay].tis);
  /*
  read_tis(tisname,&the_mos,true);
  the_mos.TisToMos(x,y);
  the_mos.SetOverlay(m_overlaynum, the_area.overlaytileheaders+the_area.getotc(baseoverlay),
    the_area.overlaytileindices+the_area.getfoti(baseoverlay) );
  */
  if(SetupSelectPoint()) return;
  x=the_area.overlayheaders[baseoverlay].width;
  y=the_area.overlayheaders[baseoverlay].height;
  dlg.SetTile(the_area.overlaytileheaders+the_area.getotc(baseoverlay),
    the_area.overlaytileindices+the_area.getfoti(baseoverlay), x,y);
  if(the_mos.MosChanged()) the_area.wedchanged=true;
  dlg.m_tisname=tisname;

  if(baseoverlay!=m_overlaynum)
  {
    RetrieveResref(tisname, the_area.overlayheaders[m_overlaynum].tis);
    read_tis(tisname, &overlay, false);
  }
  dlg.DoModal();
  RefreshWed();
  UpdateData(UD_DISPLAY);
}

BOOL CWedEdit::PreTranslateMessage(MSG* pMsg) 
{
	m_tooltip.RelayEvent(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}
