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
	m_maxwallpoly = _T("");
	m_open = FALSE;
	m_polygontype = _T("");
	m_problem = _T("");
	//}}AFX_DATA_INIT
  m_overlaynum=-1;
  m_doornum=-1;
  m_night=FALSE;
}

static int overlayboxids[]={IDC_TILESET, IDC_WIDTH, IDC_HEIGHT, IDC_UNKNOWN0C,
IDC_BROWSE, IDC_EDIT, IDC_EDITTILE,
0};

static int doorboxids[]={IDC_DOORID, IDC_CLOSED, IDC_DOORPOLYPICKER, IDC_DOORPOLYNUM,
0};

static int wallgroupboxids[]={IDC_WALLPOLYPICKER, IDC_WALLPOLYNUM,
0};

void CWedEdit::DoDataExchange(CDataExchange* pDX)
{
  CWnd *cb;
  CString tmpstr;
  int flg;
  int i;

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWedEdit)
	DDX_Control(pDX, IDC_WALLPOLYPICKER, m_wallpolypicker_control);
	DDX_Control(pDX, IDC_DOORPOLYPICKER, m_doorpolypicker_control);
	DDX_Control(pDX, IDC_WALLGROUPPICKER, m_wallgrouppicker_control);
	DDX_Control(pDX, IDC_OVERLAYPICKER, m_overlaypicker_control);
	DDX_Control(pDX, IDC_DOORPICKER, m_doorpicker_control);
	DDX_Text(pDX, IDC_OVERLAYNUM, m_maxoverlay);
	DDX_Text(pDX, IDC_DOORNUM, m_maxdoor);
	DDX_Text(pDX, IDC_WALLGROUPNUM, m_maxwallgroup);
	DDX_Text(pDX, IDC_DOORPOLYNUM, m_maxdoorpoly);
	DDX_Text(pDX, IDC_WALLPOLYNUM, m_maxwallpoly);
	DDX_Check(pDX, IDC_OPEN, m_open);
	DDX_Text(pDX, IDC_POLYGONTYPE, m_polygontype);
	DDX_Text(pDX, IDC_PROBLEM, m_problem);
	//}}AFX_DATA_MAP
  /// OVERLAY ///
  flg=the_area.overlaycount>0;
  i=0;
  while(overlayboxids[i])
  {
    cb=GetDlgItem(overlayboxids[i++]);
    cb->EnableWindow(flg);
  }
  if(flg && m_overlaynum>=0)
  {
    RetrieveResref(tmpstr, the_area.overlayheaders[m_overlaynum].tis);
    DDX_Text(pDX, IDC_TILESET, tmpstr);
    DDV_MaxChars(pDX, tmpstr, 8);
    StoreResref(tmpstr, the_area.overlayheaders[m_overlaynum].tis);

    DDX_Text(pDX, IDC_WIDTH, the_area.overlayheaders[m_overlaynum].width);
    DDX_Text(pDX, IDC_HEIGHT, the_area.overlayheaders[m_overlaynum].height);
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
    DDX_Text(pDX, IDC_DOORID, tmpstr);
    DDV_MaxChars(pDX, tmpstr, 8);
    StoreResref(tmpstr, the_area.weddoorheaders[m_doornum].doorid);

    DDX_Text(pDX, IDC_CLOSED, the_area.weddoorheaders[m_doornum].closed);
  }
  cb=GetDlgItem(IDC_EDITDOORPOLY);
  cb->EnableWindow(flg && m_doorpolynum>=0);

  /// WALLGROUP ///
  flg=the_area.wallgroupcount>0;
  i=0;
  while(wallgroupboxids[i])
  {
    cb=GetDlgItem(wallgroupboxids[i++]);
    cb->EnableWindow(flg);
  }
  flg=(flg && m_wallgroupnum>=0);
  cb=GetDlgItem(IDC_EDITWALLPOLY);
  cb->EnableWindow(flg && m_wallpolynum>=0);
}
void CWedEdit::RefreshOverlay()
{
  CString tisname;
  loc_entry fileloc;
  int size;
  int fhandle;
  int i;

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
  CString tmpstr, tmp;
  int i;
  int count, first, pos;

  RetrieveResref(tmpstr,the_area.header.wed);
  if(m_night) tmpstr+="N";
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
    for(i=0;i<the_area.wallgroupidxcount;i++)
    {
      tmpstr.Format("%d #%d",i+1,the_area.wallgroupindices[i].count);
      m_wallgrouppicker_control.AddString(tmpstr);
    }
    m_maxwallgroup.Format("/ %d",i);
    if(m_wallgroupnum<0) m_wallgroupnum=0;
    m_wallgroupnum=m_wallgrouppicker_control.SetCurSel(m_wallgroupnum);

    if(m_wallgroupnum>=0)
    {
      m_wallpolypicker_control.ResetContent();
      first=the_area.wallgroupindices[m_wallgroupnum].index;
      for(i=0;i<the_area.wallgroupindices[m_wallgroupnum].count;i++)
      {
        if(the_area.polygonindices[first+i]>=the_area.wallgroupcount)
        {
          pos=the_area.polygonindices[first+i]-the_area.wallgroupcount;
          tmp.Format("Door %d #%d 0x%x",pos,the_area.doorpolygonheaders[pos].countvertex,
            the_area.doorpolygonheaders[pos].flags);
        }
        else
        {
          pos=the_area.polygonindices[first+i];
          tmp.Format("%d #%d 0x%x",pos,the_area.wallgroupheaders[pos].countvertex,
            the_area.wallgroupheaders[pos].flags);
        }
        tmpstr.Format("%d %s",i+1,tmp);
        m_wallpolypicker_control.AddString(tmpstr);
      }
      m_maxwallpoly.Format("/ %d",i);
      if(m_wallpolynum<0) m_wallpolynum=0;
      m_wallpolynum=m_wallpolypicker_control.SetCurSel(m_wallpolynum);
    }
  }
}

BOOL CWedEdit::OnInitDialog() 
{
	CDialog::OnInitDialog();
  the_area.wedchanged=true;
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
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_CBN_KILLFOCUS(IDC_WALLGROUPPICKER, OnKillfocusWallgrouppicker)
	ON_CBN_SELCHANGE(IDC_WALLGROUPPICKER, OnSelchangeWallgrouppicker)
	ON_BN_CLICKED(IDC_EDITDOORPOLY, OnEditdoorpoly)
	ON_BN_CLICKED(IDC_EDITWALLPOLY, OnEditwallpoly)
	ON_CBN_KILLFOCUS(IDC_DOORPOLYPICKER, OnKillfocusDoorpolypicker)
	ON_CBN_SELCHANGE(IDC_DOORPOLYPICKER, OnSelchangeDoorpolypicker)
	ON_CBN_KILLFOCUS(IDC_WALLPOLYPICKER, OnKillfocusWallpolypicker)
	ON_CBN_SELCHANGE(IDC_WALLPOLYPICKER, OnSelchangeWallpolypicker)
	ON_BN_CLICKED(IDC_OPEN, OnOpen)
	ON_BN_CLICKED(IDC_ORDER, OnOrder)
	ON_BN_CLICKED(IDC_CLEAR, OnClear)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	ON_BN_CLICKED(IDC_NEW, OnNew)
	ON_BN_CLICKED(IDC_EDITTILE, OnEdittile)
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

void CWedEdit::OnBrowse() 
{
  if(m_overlaynum<0) return;
  pickerdlg.m_restype=REF_TIS;
  RetrieveResref(pickerdlg.m_picked,the_area.overlayheaders[m_overlaynum].tis);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_area.overlayheaders[m_overlaynum].tis);
  }
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

void CWedEdit::OnEditdoorpoly() 
{
	CWedPolygon dlg;
  int first;
	
  if(m_doorpolynum<0) return;
  if(m_open) first=the_area.weddoorheaders[m_doornum].offsetpolygonopen+m_doorpolynum;
  else first=the_area.weddoorheaders[m_doornum].offsetpolygonclose+m_doorpolynum;
  dlg.SetPolygon(the_area.doorpolygonheaders+first,the_area.doorvertices+the_area.doorpolygonheaders[first].firstvertex-the_area.wallgroupvertexcount);
  dlg.DoModal();
}

//////////////// WALLGROUP /////////////////////

void CWedEdit::OnKillfocusWallgrouppicker() 
{
  CString tmpstr;
  int x;

  m_wallgrouppicker_control.GetWindowText(tmpstr);
  x=strtonum(tmpstr)-1;
  if(x>=0 && x<=the_area.wallgroupcount)
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
  if(x>=0 && x<=the_area.wallgroupcount) m_wallgroupnum=x;
  RefreshWed();
	UpdateData(UD_DISPLAY);	
}

void CWedEdit::OnKillfocusWallpolypicker() 
{
	m_wallpolynum=m_wallpolypicker_control.GetCurSel();
	UpdateData(UD_DISPLAY);
}

void CWedEdit::OnSelchangeWallpolypicker() 
{
	m_wallpolynum=m_wallpolypicker_control.GetCurSel();
	UpdateData(UD_DISPLAY);
}

void CWedEdit::OnOrder() 
{
  int i;

	for(i=0;i<the_area.wallgroupcount;i++)
  {
    the_area.VertexOrder(the_area.wallgroupvertices+the_area.wallgroupheaders[i].firstvertex,the_area.wallgroupheaders[i].countvertex);
  }
	UpdateData(UD_DISPLAY);
}

void CWedEdit::OnEditwallpoly() 
{
	CWedPolygon dlg;
  int first;
	
  if(m_wallpolynum<0) return;
  first=the_area.polygonindices[the_area.wallgroupindices[m_wallgroupnum].index+m_wallpolynum];
  if(first>=the_area.wallgroupcount)
  {
    first-=the_area.wallgroupcount;
    dlg.SetPolygon(the_area.doorpolygonheaders+first,the_area.doorvertices+the_area.doorpolygonheaders[first].firstvertex-the_area.wallgroupvertexcount);
  }
  else
  {
    dlg.SetPolygon(the_area.wallgroupheaders+first,the_area.wallgroupvertices+the_area.wallgroupheaders[first].firstvertex);
  }
  dlg.DoModal();
  RefreshWed();
  UpdateData(UD_DISPLAY);
}

void CWedEdit::OnClear() 
{
	int ret;
	
  ret=the_area.ConsolidateDoortiles();
  //consolidate polygons
  ret=the_area.ConsolidateVertices()|ret;
  if(ret)
  {
    MessageBox("Changed something.","Wed editor",MB_ICONINFORMATION|MB_OK);
  }
  RefreshWed();
  UpdateData(UD_DISPLAY);
}

// edit tileset (extract door tiles)
void CWedEdit::OnEdit() 
{
  CString tmpstr;
	CTisDialog dlg;
  
  the_mos.new_mos(); //clearing up previous crap
  RetrieveResref(tmpstr, the_area.overlayheaders[m_overlaynum].tis);
  if(m_night) tmpstr+="N";
  dlg.setrange(tmpstr, the_area.overlayheaders[m_overlaynum].width,
    the_area.overlayheaders[m_overlaynum].height, 3);
  dlg.DoModal();
}

//adds new tileset
void CWedEdit::OnNew() 
{
  CString tmpstr;
	CMosEdit dlg;
  wed_overlay *newoverlay;
  wed_tilemap *newtileheaders;
  wed_polyidx *newwgc;
  short *newindices;
  int tilecount;
  int i;
  int wgc;
	 
  if(the_area.wedheader.overlaycnt)
  {
    MessageBox("Sorry, DLTCEP cannot add more overlays at the moment.","Wed editor",MB_OK);
    return;
  }
  tmpstr=itemname;
  dlg.NewMos(TM_TIS|TM_CONFIRM); //true = TIS
  dlg.DoModal();
  
  m_overlaynum=the_area.wedheader.overlaycnt;
  newoverlay=new wed_overlay[m_overlaynum+1];
  if(!newoverlay)
  {
    itemname=tmpstr;
    return;
  }
  memcpy(newoverlay,the_area.overlayheaders,m_overlaynum*sizeof(wed_overlay) );
  tilecount=the_mos.tisheader.numtiles;
  
  StoreResref(itemname,newoverlay[m_overlaynum].tis);
  itemname=tmpstr;
  newoverlay[m_overlaynum].tileidxoffset=-1;
  newoverlay[m_overlaynum].tilemapoffset=-1;
  newoverlay[m_overlaynum].width=the_mos.mosheader.wColumn;
  newoverlay[m_overlaynum].height=the_mos.mosheader.wRow;
  newoverlay[m_overlaynum].unknownc=0;
  //setting up overlaytileindices

  tilecount=the_mos.mosheader.wColumn*the_mos.mosheader.wRow;

  //wallgroupindices for first overlay
  newindices=new short[the_area.overlaytileidxcount+tilecount];
  if(!newindices)
  {
    m_overlaynum--;
    return;
  }
  newtileheaders=new wed_tilemap[the_area.overlaytilecount+tilecount];
  if(!newtileheaders)
  {
    delete [] newindices;
    m_overlaynum--;
    return;
  }

  if(m_overlaynum==0)
  {
    if(the_area.wallgroupindices) delete [] the_area.wallgroupindices;
    wgc=((newoverlay[0].width+9)/10)*((newoverlay[0].height*2+14)/15);
    newwgc=new wed_polyidx[wgc];
    if(!newwgc)
    {
      delete [] newindices;
      delete [] newtileheaders;
      m_overlaynum--;
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
  delete [] the_area.overlayheaders;
  the_area.overlayheaders=newoverlay;

  delete [] the_area.overlaytileheaders;
  the_area.overlaytileheaders=newtileheaders;

  delete [] the_area.overlaytileindices;
  the_area.overlaytileindices=newindices;

  the_area.overlaytilecount+=tilecount;
  the_area.overlaytileidxcount+=tilecount;

  the_area.overlaycount=the_area.wedheader.overlaycnt=m_overlaynum+1;
  UpdateData(UD_DISPLAY);
}

void CWedEdit::OnEdittile() 
{
  CWedTile dlg;
  CString tisname;
  int i;
  int firsttile;
  int x,y;

  if(m_overlaynum<0) return;
  firsttile=0;
  for(i=0;i<m_overlaynum;i++)
  {
    firsttile+=the_area.overlayheaders[i].height*the_area.overlayheaders[i].width;
  }
  x=the_area.overlayheaders[i].width;
  y=the_area.overlayheaders[i].height;
  dlg.SetTile(the_area.overlaytileheaders+firsttile,
    the_area.overlaytileindices+the_area.getfoti(m_overlaynum), x,y);
  RetrieveResref(tisname, the_area.overlayheaders[i].tis);
  read_tis(tisname,false);
  the_mos.TisToMos(x,y);
  dlg.DoModal();
  RefreshWed();
  UpdateData(UD_DISPLAY);
}

BOOL CWedEdit::PreTranslateMessage(MSG* pMsg) 
{
	m_tooltip.RelayEvent(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}
