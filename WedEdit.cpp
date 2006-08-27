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
  m_doorpolynum=-1;
  m_repair=false;
  m_polygonsize=0;
  m_polygoncopy=NULL;
}

static int overlayboxids[]={IDC_TILESET, IDC_WIDTH, IDC_HEIGHT, IDC_UNKNOWN0C,
IDC_REMOVE, IDC_EDIT, IDC_EDITTILE, IDC_OVERLAY,
0};

static int doorboxids[]={IDC_DOORID, IDC_CLOSED, IDC_DOORPOLYPICKER, IDC_DOORPOLYNUM,
IDC_EDITDOORPOLY, IDC_SELECTION, IDC_REMOVE3, IDC_COPY, IDC_PASTE, IDC_DROP, 
IDC_EDITTILE2, IDC_ADD3,
0};

static int wallgroupboxids[]={IDC_WALLGROUPPICKER, IDC_EDITWALLPOLY, IDC_SELECTION2,
IDC_REMOVE2, IDC_COPY2, IDC_PASTE2, IDC_TRANSPARENT, IDC_CLEAR,
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
  cb=GetDlgItem(IDC_SELECTION);
  cb->EnableWindow(flg && m_doorpolynum>=0);
  cb=GetDlgItem(IDC_REMOVE3);
  cb->EnableWindow(flg && m_doorpolynum>=0);
  cb=GetDlgItem(IDC_COPY);
  cb->EnableWindow(flg && m_doorpolynum>=0);
  cb=GetDlgItem(IDC_PASTE);
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
  Cmos tmpmos;
  int otc;
  int fhandle;
  int i;

  the_area.overlaytilecount=0;
  for(i=0;i<the_area.overlaycount;i++)
  {
    otc=the_area.overlayheaders[i].width*the_area.overlayheaders[i].height;
    the_area.overlaytilecount+=otc;
  }

  otc=the_area.overlayheaders[m_overlaynum].width*the_area.overlayheaders[m_overlaynum].height;
  if(!m_overlaynum)
  {
    for(i=0;i<the_area.weddoorcount;i++)
    {
      otc+=the_area.weddoorheaders[i].countdoortileidx;
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
  if(!m_overlaynum && otc<tmpmos.RetrieveTisFrameCount(fhandle,fileloc.size) )
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
    if(m_doornum>=m_doorpicker_control.GetCount()) m_doornum=m_doorpicker_control.GetCount()-1;
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
      if(m_doorpolynum>=m_doorpolypicker_control.GetCount()) m_doorpolynum=m_doorpolypicker_control.GetCount()-1;
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
    if(m_wallgroupnum>=m_wallgrouppicker_control.GetCount()) m_wallgroupnum=m_wallgrouppicker_control.GetCount()-1;
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
	ON_BN_CLICKED(IDC_SELECTION, OnSelection)
	ON_BN_CLICKED(IDC_SELECTION2, OnSelection2)
	ON_BN_CLICKED(IDC_ADD3, OnAdd3)
	ON_BN_CLICKED(IDC_ADD2, OnAdd2)
	ON_BN_CLICKED(IDC_REMOVE2, OnRemove2)
	ON_BN_CLICKED(IDC_REMOVE3, OnRemove3)
	ON_BN_CLICKED(IDC_COPY, OnCopy)
	ON_BN_CLICKED(IDC_PASTE, OnPaste)
	ON_BN_CLICKED(IDC_COPY2, OnCopy2)
	ON_BN_CLICKED(IDC_PASTE2, OnPaste2)
	ON_BN_CLICKED(IDC_TRANSPARENT, OnTransparent)
	ON_BN_CLICKED(IDC_CLEANUP, OnCleanup)
	ON_BN_CLICKED(IDC_EDITTILE2, OnEdittile2)
	ON_BN_CLICKED(IDC_DROP, OnDrop)
	ON_COMMAND(IDC_ADD, OnAdd)
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

void CWedEdit::OnEdittile2() 
{
//  Cmos overlay;
  CWedTile dlg;
  CString tisname;
  int x,y;

  if(m_doornum<0) return;
  x=the_area.overlayheaders[0].width;
  y=the_area.overlayheaders[0].height;
  dlg.SetTile(0,  x,y);
  dlg.SetDoor(m_doornum);

  RetrieveResref(tisname, the_area.weddoorheaders[m_doornum].doorid);
  dlg.m_tisname=tisname;

  if(SetupSelectPoint(0)) return;

  dlg.DoModal();
  if(the_mos.MosChanged()) the_area.wedchanged=true;
  RefreshWed();
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
  for(i=0;i<the_area.weddoorcount;i++)
  {
    if(i!=m_doornum)
    {
      if(the_area.weddoorheaders[i].offsetpolygonclose>=first)
      {
        the_area.weddoorheaders[i].offsetpolygonclose++;
      }
      if(the_area.weddoorheaders[i].offsetpolygonopen>=first)
      {
        the_area.weddoorheaders[i].offsetpolygonopen++;
      }
    }
  }
  pos = the_area.wedvertexheaderlist.FindIndex(the_area.wallpolygoncount+first-1);
  memcpy(newpolygons,the_area.doorpolygonheaders,first*sizeof(wed_polygon) );
  memcpy(newpolygons+first+1,the_area.doorpolygonheaders+first,(the_area.doorpolygoncount-first)*sizeof(wed_polygon) );
  memset(newpolygons+first,0,sizeof(wed_polygon) );
  newpolygons[first].flags=0x89; //standard door flags
  newpolygons[first].unkflags=255;
  delete [] the_area.doorpolygonheaders;
  the_area.doorpolygonheaders=newpolygons;
  the_area.wedvertexheaderlist.InsertAfter(pos, newelement);
  the_area.doorpolygoncount++;
  the_area.wedchanged=true;
  RefreshWed();
  UpdateData(UD_DISPLAY);
}

void CWedEdit::OnRemove3() 
{
  int first;
	
  if(m_doornum<0) return;
  if(m_doorpolynum<0) return;
  if(m_open)
  {
    first=m_doorpolynum+the_area.weddoorheaders[m_doornum].offsetpolygonopen;
  }
  else
  {
    first=m_doorpolynum+the_area.weddoorheaders[m_doornum].offsetpolygonclose;
  }
  if(the_area.RemoveDoorPolygon(first))
  {
    MessageBox("Can't remove polygon","Area editor",MB_ICONSTOP|MB_OK);
    return;
  }
  if(m_open)
  {
    the_area.weddoorheaders[m_doornum].countpolygonopen--;
    if(m_doorpolynum>=the_area.weddoorheaders[m_doornum].countpolygonopen) m_doorpolynum--;
  }
  else
  {
    the_area.weddoorheaders[m_doornum].countpolygonclose--;
    if(m_doorpolynum>=the_area.weddoorheaders[m_doornum].countpolygonclose) m_doorpolynum--;
  }
  the_area.wedchanged=true;
  RefreshWed();
  UpdateData(UD_DISPLAY);
}

void CWedEdit::OnAdd() 
{
  CString newid;

  newid.Format("Door %02d",the_area.weddoorcount+1);
  the_area.AddWedDoor(newid);
  RefreshWed();
  UpdateData(UD_DISPLAY);
}

void CWedEdit::OnDrop() 
{
  the_area.RemoveWedDoor(the_area.weddoorheaders[m_doornum].doorid);
  RefreshWed();
  UpdateData(UD_DISPLAY);
}

void CWedEdit::OnEditdoorpoly() 
{
//  POSITION pos;
  CWedPolygon dlg;
  int first;
	
  if(m_doorpolynum<0) return;
  if(m_open) first=the_area.weddoorheaders[m_doornum].offsetpolygonopen+m_doorpolynum;
  else first=the_area.weddoorheaders[m_doornum].offsetpolygonclose+m_doorpolynum;  
//  pos=the_area.wedvertexheaderlist.FindIndex(first+the_area.wallpolygoncount);
//  dlg.SetPolygon(the_area.doorpolygonheaders+first, pos);
  dlg.SetPolygon(MT_DOORPOLYLIST, first);
  dlg.m_open=m_open;
  dlg.DoModal();
  if(dlg.m_changed) the_area.wedchanged=true;
  RefreshWed();
  UpdateData(UD_DISPLAY);
}

void CWedEdit::OnSelection() 
{
	CImageView dlg;
  int first;
  
  if(SetupSelectPoint(0))
  {
    return;
  }

  if(m_open) first=the_area.weddoorheaders[m_doornum].offsetpolygonopen+m_doorpolynum;
  else first=the_area.weddoorheaders[m_doornum].offsetpolygonclose+m_doorpolynum;
  dlg.SetMapType(MT_DOORPOLYLIST, (LPBYTE) &the_area.wedvertexheaderlist);
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
  UpdateData(UD_DISPLAY);
}

void CWedEdit::OnCopy() 
{
  POSITION pos;
  int first;

  if(m_open)
  {
    first=m_doorpolynum+the_area.weddoorheaders[m_doornum].offsetpolygonopen;
  }
  else
  {
    first=m_doorpolynum+the_area.weddoorheaders[m_doornum].offsetpolygonclose;
  }
  pos=the_area.wedvertexheaderlist.FindIndex(the_area.wallpolygoncount+first);
  if(!pos) return;
	if(m_polygoncopy)
  {
    delete [] m_polygoncopy;
  }
  m_polygonsize=the_area.doorpolygonheaders[first].countvertex;
  m_polygoncopy=new area_vertex[m_polygonsize];
  if(!m_polygoncopy)
  {
    MessageBox("Out of memory","Area editor",MB_ICONSTOP|MB_OK);
    return;
  }
  memcpy(m_polygoncopy, the_area.wedvertexheaderlist.GetAt(pos), m_polygonsize * sizeof(area_vertex) );
}

void CWedEdit::OnPaste() 
{
  area_vertex *newpolygon;
  POSITION pos;
  int first;

  if(!m_polygoncopy)
  {
    return;
  }
  if(m_open)
  {
    first=m_doorpolynum+the_area.weddoorheaders[m_doornum].offsetpolygonopen;
  }
  else
  {
    first=m_doorpolynum+the_area.weddoorheaders[m_doornum].offsetpolygonclose;
  }
  pos=the_area.wedvertexheaderlist.FindIndex(the_area.wallpolygoncount+first);
  if(!pos) return;
  newpolygon = new area_vertex[m_polygonsize];
  if(!newpolygon)
  {
    MessageBox("Out of memory","Area editor",MB_ICONSTOP|MB_OK);
    return;
  }
  memcpy(newpolygon, m_polygoncopy, m_polygonsize * sizeof(area_vertex) );
  the_area.wedvertexheaderlist.SetAt(pos, newpolygon);
  the_area.doorpolygonheaders[first].countvertex=m_polygonsize;
  the_area.RecalcBoundingBoxes(); //simplicity
  RefreshWed();
  UpdateData(UD_DISPLAY);
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
  the_area.wedchanged=true;
  RefreshWed();
  UpdateData(UD_DISPLAY);
}

void CWedEdit::OnAdd2() 
{
  area_vertex *newelement;
  wed_polygon *newpolygons;
  POSITION pos;

  pos=the_area.wedvertexheaderlist.FindIndex(m_wallgroupnum);
  if(m_wallgroupnum<0) m_wallgroupnum=0;
  newelement = new area_vertex[0];
  if(!newelement) return;
  newpolygons = new wed_polygon [the_area.secheader.wallpolycnt+1];
  if(!newpolygons)
  {
    delete [] newelement;
    return;
  }

  memcpy(newpolygons,the_area.wallpolygonheaders,m_wallgroupnum*sizeof(wed_polygon) );
  memcpy(newpolygons+m_wallgroupnum+1,the_area.wallpolygonheaders+m_wallgroupnum,(the_area.secheader.wallpolycnt-m_wallgroupnum)*sizeof(wed_polygon) );
  memset(newpolygons+m_wallgroupnum,0,sizeof(wed_polygon) );
  newpolygons[m_wallgroupnum].unkflags=255;
  delete [] the_area.wallpolygonheaders;
  the_area.wallpolygonheaders=newpolygons;
  if(pos)
  {
    the_area.wedvertexheaderlist.InsertBefore(pos, newelement);
  }
  else
  {
    the_area.wedvertexheaderlist.AddHead(newelement);
  }
  the_area.secheader.wallpolycnt++;
  the_area.wallpolygoncount=the_area.secheader.wallpolycnt;
  the_area.RecalcBoundingBoxes();
  the_area.wedchanged=true;
  RefreshWed();
  UpdateData(UD_DISPLAY);
}

void CWedEdit::OnSelection2() 
{
	CImageView dlg;
  
  if(SetupSelectPoint(0))
  {
    return;
  }

  dlg.SetMapType(MT_WALLPOLYLIST, (LPBYTE) &the_area.wedvertexheaderlist);
  dlg.m_value=m_wallgroupnum;
  dlg.m_max=the_area.wallpolygoncount;
  dlg.InitView(IW_SHOWGRID|IW_GETPOLYGON|IW_ENABLEBUTTON, &the_mos);
  dlg.DoModal();
  m_wallgroupnum=dlg.m_value;
  RefreshWed();  
  UpdateData(UD_DISPLAY);
}

void CWedEdit::OnEditwallpoly() 
{
//  POSITION pos;
	CWedPolygon dlg;
	
  if(m_wallgroupnum<0) return;
  //pos=the_area.wedvertexheaderlist.FindIndex(m_wallgroupnum);
  dlg.SetPolygon(MT_WALLPOLYLIST, m_wallgroupnum);
  dlg.DoModal();
  if(dlg.m_changed) the_area.wedchanged=true;
  RefreshWed();
  UpdateData(UD_DISPLAY);
}

void CWedEdit::OnCopy2() 
{
  POSITION pos;

  pos=the_area.wedvertexheaderlist.FindIndex(m_wallgroupnum);
  if(!pos) return;
	if(m_polygoncopy)
  {
    delete [] m_polygoncopy;
  }
  m_polygonsize=the_area.wallpolygonheaders[m_wallgroupnum].countvertex;
  m_polygoncopy=new area_vertex[m_polygonsize];
  if(!m_polygoncopy)
  {
    MessageBox("Out of memory","Area editor",MB_ICONSTOP|MB_OK);
    return;
  }
  memcpy(m_polygoncopy, the_area.wedvertexheaderlist.GetAt(pos), m_polygonsize * sizeof(area_vertex) );
}

void CWedEdit::OnPaste2() 
{
  area_vertex *newpolygon;
  POSITION pos;

  if(!m_polygoncopy)
  {
    return;
  }
  pos=the_area.wedvertexheaderlist.FindIndex(m_wallgroupnum);
  if(!pos) return;
  newpolygon = new area_vertex[m_polygonsize];
  if(!newpolygon)
  {
    MessageBox("Out of memory","Area editor",MB_ICONSTOP|MB_OK);
    return;
  }
  memcpy(newpolygon, m_polygoncopy, m_polygonsize * sizeof(area_vertex) );
  the_area.wedvertexheaderlist.SetAt(pos, newpolygon);
  the_area.wallpolygonheaders[m_wallgroupnum].countvertex=m_polygonsize;
  the_area.RecalcBoundingBoxes(); //simplicity
  the_area.wedchanged=true;
  RefreshWed();
  UpdateData(UD_DISPLAY);
}

//returns true if the whole tile became transparent
int CWedEdit::HandleOverlay(int x, int y, area_vertex *polygon, int size, int tile)
{
  CPoint tmp;
  unsigned char *framebuffer;
  int px,py;
  int transparent;

  framebuffer=the_mos.GetFrameBuffer(tile);
  if(!framebuffer) return 0;
  tmp=the_mos.GetFrameSize(tile);
  if(tmp.x!=64 || tmp.y!=64) return 0;

  transparent=1;
  tmp.y=y;
  for(py=0;py<64;py++)
  {
    tmp.x=x;
    for(px=0;px<64;px++)
    {
      if(PointInPolygon(polygon, size, tmp ) )
      {
        *framebuffer=0;
      }
      if(*framebuffer) transparent=0;
      framebuffer++;
      tmp.x++;
    }
    tmp.y++;
  }
  return transparent;
}

//returns true if the whole tile became opaque (ie. same as the original)
//non-transparency check doesn't work for doors!
int CWedEdit::RemoveOverlay(int x, int y, area_vertex *polygon, int size, int tile, int original)
{
  CPoint tmp;
  unsigned char *framebuffer;
  unsigned char *originalbuffer;
  int px,py;
  int opaque;

  framebuffer=the_mos.GetFrameBuffer(tile);
  if(!framebuffer) return 0;
  originalbuffer=the_mos.GetFrameBuffer(original);
  if(!originalbuffer) return 0;
  tmp=the_mos.GetFrameSize(tile);
  if(tmp.x!=64 || tmp.y!=64) return 0;

  opaque=1;
  tmp.y=y;
  for(py=0;py<64;py++)
  {
    tmp.x=x;
    for(px=0;px<64;px++)
    {
      if(PointInPolygon(polygon, size, tmp ) )
      {
        if(*originalbuffer) *framebuffer=*originalbuffer;
        else *framebuffer=255;
      }
      else
      {
        if(*framebuffer!=*originalbuffer) opaque=0;
      }
      framebuffer++;
      originalbuffer++;
      tmp.x++;
    }
    tmp.y++;
  }
  return opaque;
}

void CWedEdit::OnTransparent() 
{
  int x,y;
  POSITION pos;
  area_vertex *polygon;
  int size, tile, tilenum;

  if(m_overlaynum<1 || !the_area.overlayheaders[m_overlaynum].tis[0])
  {
    MessageBox("First select an overlay!","Area editor",MB_ICONINFORMATION|MB_OK);
    return;
  }

  if(m_wallgroupnum<0) return;
  pos=the_area.wedvertexheaderlist.FindIndex(m_wallgroupnum);
  if(!pos) return;
  polygon = (area_vertex *) the_area.wedvertexheaderlist.GetAt(pos);
  size = the_area.wallpolygonheaders[m_wallgroupnum].countvertex;
  if(size<3 || !polygon) return;
  if(SetupSelectPoint(0)) return;
  for(y=0;y<the_area.overlayheaders[0].height;y++)
  {
    for(x=0;x<the_area.overlayheaders[0].width;x++)
    {
      if(!PolygonInBox( polygon,size,CRect(x*64, y*64, (x+1)*64, (y+1)*64) ) )
      {
        continue;
      }
      tilenum=y*the_area.overlayheaders[0].width+x;
      tile=the_area.overlaytileheaders[tilenum].alternate;
      if(tile==-1) //this tile wasn't yet overlaid, create the alternate tile now
      {
        tile=the_area.overlaytileheaders[tilenum].alternate=(short) the_mos.AddTileCopy(the_area.overlaytileindices[the_area.overlaytileheaders[tilenum].firsttileprimary]);
      }
      the_area.overlaytileheaders[tilenum].flags|=1<<m_overlaynum;
      //prepare the tile
      if(HandleOverlay(x*64,y*64, polygon, size, tile))
      {
        //fully overlaid tiles don't need to be stored
        the_mos.RemoveTile(tile);
        //          the_area.overlaytileheaders[tilenum].alternate=-1;
        for(tilenum=0;tilenum<the_area.overlaytilecount;tilenum++)
        {
          if(the_area.overlaytileheaders[tilenum].alternate>tile)
          {
            the_area.overlaytileheaders[tilenum].alternate--;
          }
          else if(the_area.overlaytileheaders[tilenum].alternate==tile)
          {
            the_area.overlaytileheaders[tilenum].alternate=-1;
          }
        }
      }
    }
  }
  MessageBox("The polygon was removed after the creation of the overlay.","Area editor",MB_ICONINFORMATION|MB_OK);
  //remove the polygon after use
  OnRemove2();
}

void CWedEdit::OnClear() 
{
  int x,y;
  POSITION pos;
  area_vertex *polygon;
  int size, tile, original, tilenum;

  if(m_wallgroupnum<0) return;
  pos=the_area.wedvertexheaderlist.FindIndex(m_wallgroupnum);
  if(!pos) return;
  polygon = (area_vertex *) the_area.wedvertexheaderlist.GetAt(pos);
  size = the_area.wallpolygonheaders[m_wallgroupnum].countvertex;
  if(size<3 || !polygon) return;
  if(SetupSelectPoint(0)) return;
  for(y=0;y<the_area.overlayheaders[0].height;y++)
  {
    for(x=0;x<the_area.overlayheaders[0].width;x++)
    {
      if(!PolygonInBox( polygon,size,CRect(x*64, y*64, (x+1)*64, (y+1)*64) ) )
      {
        continue;
      }
      tilenum=y*the_area.overlayheaders[0].width+x;
      tile=the_area.overlaytileheaders[tilenum].alternate;
      original=the_area.overlaytileindices[the_area.overlaytileheaders[tilenum].firsttileprimary];
      if(tile==-1) 
      {
        //the tile isn't overlaid at all?
        if(!the_area.overlaytileheaders[tilenum].flags) continue; 
        //this tile is (was) fully overlaid, create overlay now (fully transparent)
        tile=the_area.overlaytileheaders[tilenum].alternate=(short) the_mos.AddTileCopy(original,NULL,2);
      }
      //prepare the tile
      if(RemoveOverlay(x*64, y*64, polygon, size, tile, original))
      {
        the_area.overlaytileheaders[tilenum].flags=0;
        //fully opaque tiles don't need to be stored twice
        the_mos.RemoveTile(tile);
        for(tilenum=0;tilenum<the_area.overlaytilecount;tilenum++)
        {
          if(the_area.overlaytileheaders[tilenum].alternate>tile)
          {
            the_area.overlaytileheaders[tilenum].alternate--;
          }
          else if(the_area.overlaytileheaders[tilenum].alternate==tile)
          {
            the_area.overlaytileheaders[tilenum].alternate=-1;
          }
        }
      }
    }
  }
  MessageBox("The polygon was removed after the creation of the overlay.","Area editor",MB_ICONINFORMATION|MB_OK);
  //remove the polygon after use
  OnRemove2();
}

void CWedEdit::OnCleanup() 
{
	int ret;
	
  ret=the_area.RecalcBoundingBoxes();
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
  CString tmpstr;
	CTisDialog dlg;
  int baseoverlay;
  
  UpdateData(UD_RETRIEVE);
  if(m_whole) baseoverlay=0;
  else baseoverlay=m_overlaynum;
  RetrieveResref(tmpstr, the_area.overlayheaders[baseoverlay].tis);
  dlg.setrange(tmpstr, the_area.overlayheaders[baseoverlay].width,
    the_area.overlayheaders[baseoverlay].height, 3);
//  dlg.m_overlaynum=m_overlaynum;
  dlg.m_tileheaders=the_area.overlaytileheaders+the_area.getotc(baseoverlay);
  dlg.m_tileindices=the_area.overlaytileindices+the_area.getfoti(baseoverlay);
  if(SetupSelectPoint(baseoverlay)) return;
  if(!m_whole)
  {
    for(baseoverlay=0;baseoverlay<10;baseoverlay++)
    {
      if(baseoverlay!=m_overlaynum)
      {
        the_mos.SetOverlay(baseoverlay,NULL);
      }
    }
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
  
  if(!m_overlaynum && the_area.overlaytileidxcount)
  {
    if(the_area.overlaytileidxcount!=the_area.overlayheaders[0].height*the_area.overlayheaders[0].width)
    {
      MessageBox("Can't replace the first layer while there are doors or overlays!");
      itemname=tmpstr;
      return;
    }
    the_area.overlaytileidxcount=0;
    the_area.overlaytilecount=0;
  }
  the_area.wedchanged=true;
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

  memcpy(newindices,the_area.overlaytileindices,the_area.overlaytileidxcount*sizeof(short) );
  for(i=0;i<tilecount;i++)
  {
    newindices[the_area.overlaytileidxcount+i]=(short) i;
  }
  the_area.overlaytileidxcount+=tilecount;
  delete [] the_area.overlaytileindices;
  the_area.overlaytileindices=newindices;

  if(m_overlaynum)
  {
    tilecount=1;
  }

  newtileheaders=new wed_tilemap[the_area.overlaytilecount+tilecount];
  if(!newtileheaders)
  {
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

  i=tilecount;
  while(i--)
  {
    if(m_overlaynum)
    {
      newtileheaders[the_area.overlaytilecount+i].counttileprimary=the_mos.mosheader.wRow;
    }
    else
    {
      newtileheaders[the_area.overlaytilecount+i].firsttileprimary=(short) i;
      newtileheaders[the_area.overlaytilecount+i].counttileprimary=1;
// i don't know why was this here, it would set the overlay flags to some weird value
//      newtileheaders[the_area.overlaytilecount+i].flags=(short) (the_mos.mosheader.wRow<<8);
    }
    newtileheaders[the_area.overlaytilecount+i].alternate=-1;
  }

  delete [] the_area.overlaytileheaders;
  the_area.overlaytileheaders=newtileheaders;

  the_area.overlaytilecount+=tilecount;

  the_area.overlayheaders[m_overlaynum].tileidxoffset=-1;
  the_area.overlayheaders[m_overlaynum].tilemapoffset=-1;
  if(m_overlaynum)
  {
    the_area.overlayheaders[m_overlaynum].width=1;
    the_area.overlayheaders[m_overlaynum].height=1;
  }
  else
  {
    the_area.overlayheaders[m_overlaynum].width=the_mos.mosheader.wColumn;
    the_area.overlayheaders[m_overlaynum].height=the_mos.mosheader.wRow;
    the_area.m_width=the_mos.mosheader.wWidth;
    the_area.m_height=the_mos.mosheader.wHeight;
  }
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

  x=the_area.overlayheaders[baseoverlay].width;
  y=the_area.overlayheaders[baseoverlay].height;
  dlg.SetTile(baseoverlay,  x,y);

  RetrieveResref(tisname, the_area.overlayheaders[baseoverlay].tis);
  dlg.m_tisname=tisname;

  if(baseoverlay!=m_overlaynum)
  {
    RetrieveResref(tisname, the_area.overlayheaders[m_overlaynum].tis);
    read_tis(tisname, &overlay, false);
    baseoverlay=m_overlaynum;
  }
  else
  {
    baseoverlay=0;
  }
//  if(SetupSelectPoint(baseoverlay, &overlay)) return;
  if(SetupSelectPoint(0)) return;
  if(!m_whole)
  {
    for(baseoverlay=0;baseoverlay<10;baseoverlay++)
    {
      if(baseoverlay!=m_overlaynum)
      {
        the_mos.SetOverlay(baseoverlay,NULL);
      }
    }
  }
  dlg.DoModal();
  if(the_mos.MosChanged()) the_area.wedchanged=true;
  RefreshWed();
  UpdateData(UD_DISPLAY);
}

BOOL CWedEdit::PreTranslateMessage(MSG* pMsg) 
{
	m_tooltip.RelayEvent(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}

void CWedEdit::PostNcDestroy() 
{
  if(m_polygoncopy)
  {
    delete [] m_polygoncopy;	
    m_polygoncopy=NULL;
  }
	CDialog::PostNcDestroy();
}

