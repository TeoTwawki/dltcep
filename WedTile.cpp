// WedTile.cpp : implementation file
//

#include "stdafx.h"
#include "chitem.h"
#include "chitemDlg.h"
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
  m_tilenum=-1;
}

static int overlayids[]={IDC_ALT, IDC_ADDALT, IDC_FIXALTER, IDC_GREENWATER,
0};

void CWedTile::DoDataExchange(CDataExchange* pDX)
{
  int tmp;

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWedTile)
	DDX_Control(pDX, IDC_BLOCKPICKER, m_tilepicker);
  //}}AFX_DATA_MAP
  if(m_tilenum>=0)
  {
    tmp=m_ptileheader[m_tilenum].firsttileprimary;
    DDX_Text(pDX, IDC_FIRST, m_ptileheader[m_tilenum].firsttileprimary);
    if(tmp!=m_ptileheader[m_tilenum].firsttileprimary)
    {
      the_area.wedchanged=true;
    }
    tmp=m_ptileidx[m_ptileheader[m_tilenum].firsttileprimary];
    DDX_Text(pDX, IDC_TILEIDX, m_ptileidx[m_ptileheader[m_tilenum].firsttileprimary]);
    if(tmp!=m_ptileidx[m_ptileheader[m_tilenum].firsttileprimary])
    {
      the_area.wedchanged=true;
    }
    DDX_Text(pDX, IDC_NUM, m_ptileheader[m_tilenum].counttileprimary);
    tmp=m_ptileheader[m_tilenum].alternate;
    DDX_Text(pDX, IDC_ALT, m_ptileheader[m_tilenum].alternate);
    DDV_MinMaxInt(pDX, m_ptileheader[m_tilenum].alternate,-1,the_mos.GetFrameCount());
    if(tmp!=m_ptileheader[m_tilenum].alternate)
    {
      the_area.wedchanged=true;
    }
    tmp=m_ptileheader[m_tilenum].flags;
    DDX_Text(pDX, IDC_FLAGS, m_ptileheader[m_tilenum].flags);
    DDX_Text(pDX, IDC_UNKNOWN, m_ptileheader[m_tilenum].unknown);

    if(tmp!=m_ptileheader[m_tilenum].flags)
    {
      the_area.wedchanged=true;
    }
  }
}

BEGIN_MESSAGE_MAP(CWedTile, CDialog)
	//{{AFX_MSG_MAP(CWedTile)
	ON_LBN_SELCHANGE(IDC_BLOCKPICKER, OnSelchangeBlockpicker)
	ON_EN_KILLFOCUS(IDC_FIRST, DefaultKillfocus)
	ON_BN_CLICKED(IDC_TILE, OnTile)
	ON_BN_CLICKED(IDC_ADDALT, OnAddalt)
	ON_BN_CLICKED(IDC_FIXALTER, OnFixalter)
	ON_BN_CLICKED(IDC_GREENWATER, OnGreenwater)
	ON_BN_CLICKED(IDC_OVERLAY, OnOverlay)
	ON_EN_KILLFOCUS(IDC_NUM, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_ALT, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_FLAGS, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_UNKNOWN, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_TILEIDX, DefaultKillfocus)
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
  tmpstr.Format("Edit tiles & overlays (%s)",m_tisname);  
  SetWindowText(tmpstr);  
  m_tilenum=m_tilepicker.GetCurSel();
  if(m_tilenum<0) m_tilenum=0;
  m_tilepicker.ResetContent();
  for(j=0;j<m_tilecounty;j++) for(i=0;i<m_tilecountx;i++)
  {
    wed_tilemap *wtm=m_ptileheader+(j*m_tilecountx+i);
    if(wtm->alternate==-1)
    {
      tmpstr.Format("[%d.%d] %d - %d",i,j,wtm->firsttileprimary, wtm->flags);
    }
    else
    {
      tmpstr.Format("[%d.%d] %d (%d) - %d",i,j,wtm->firsttileprimary, wtm->alternate, wtm->flags);
    }
    m_tilepicker.AddString(tmpstr);
  }
  maxtilecount=i*j-1;

  if(m_tilenum>maxtilecount) m_tilenum=maxtilecount;
  m_tilenum=m_tilepicker.SetCurSel(m_tilenum);
}

BOOL CWedTile::OnInitDialog() 
{
  CWnd *cw;
	CDialog::OnInitDialog();
  //tooltips
  {
    m_tooltip.Create(this,TTS_NOPREFIX);
    m_tooltip.SetMaxTipWidth(200);
    m_tooltip.SetTipBkColor(RGB(240,224,160));
    
    m_tooltip.AddTool(GetDlgItem(IDOK), IDS_CANCEL);
  }
  cw=GetDlgItem(IDC_OVERLAY);
  cw->EnableWindow(!!the_mos.m_overlay);
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

void CWedTile::OnTile() 
{
	CImageView dlg;
  CPoint cp;

  dlg.InitView(IW_ENABLEBUTTON|IW_SHOWGRID|IW_MARKTILE, &the_mos);
  //setting the clipping, not required, but setting the position is nice
  dlg.m_clipx=m_tilenum%m_tilecountx;
  dlg.m_clipy=m_tilenum/m_tilecountx;
  dlg.DoModal();
  cp=dlg.GetPoint(GP_TILE);
  m_tilenum=m_tilepicker.SetCurSel(cp.y*m_tilecountx+cp.x);
  UpdateData(UD_DISPLAY);
}

void CWedTile::OnOverlay() 
{
  CImageView dlg;
  CPoint cp;

  dlg.InitView(IW_ENABLEBUTTON|IW_SHOWGRID|IW_OVERLAY, &the_mos);
  //setting the clipping, not required, but setting the position is nice
  dlg.m_clipx=m_tilenum%m_tilecountx;
  dlg.m_clipy=m_tilenum/m_tilecountx;
  dlg.SetMapType(MT_OVERLAY, (LPBYTE) m_ptileheader);
  dlg.DoModal();
  cp=dlg.GetPoint(GP_TILE);
  m_tilenum=m_tilepicker.SetCurSel(cp.y*m_tilecountx+cp.x);
  RefreshTile();
  UpdateData(UD_DISPLAY);
}

void CWedTile::OnFixalter() 
{
  int i;

  i=m_tilecounty*m_tilecountx;
  while(i--)
  {
    if(m_ptileheader[i].flags && (m_ptileheader[i].alternate==-1) )
    {
      m_ptileheader[i].alternate=m_ptileidx[m_ptileheader[i].firsttileprimary];
      the_area.wedchanged=true;
    }
  }
  RefreshTile();
  UpdateData(UD_DISPLAY);
}

void CWedTile::OnAddalt() 
{  
  if(m_ptileheader[m_tilenum].alternate)
  {
    MessageBox("This tile already has an alternate.","Warning",MB_ICONINFORMATION|MB_OK);
    return;
  }
	m_ptileheader[m_tilenum].alternate=(short) the_mos.AddTileCopy(m_ptileidx[m_ptileheader[m_tilenum].firsttileprimary]);
  the_area.wedchanged=true;
}

void CWedTile::OnGreenwater() 
{
  int i;
  int idx;
  int ret;
  bool deepen;

  if(MessageBox("Do you want to remove the dithering too?","Tile editor",MB_YESNO)==IDYES)
  {
    deepen=true;
  }
  else
  {
    deepen=false;
  }
  i=m_tilecounty*m_tilecountx;
  ((CChitemDlg *) AfxGetMainWnd())->start_progress(i, "Converting overlays...");

  while(i--)
  {
    ((CChitemDlg *) AfxGetMainWnd())->set_progress(i); 
    if(m_ptileheader[i].flags && (m_ptileheader[i].alternate==-1) )
    {
      idx = m_ptileidx[m_ptileheader[i].firsttileprimary];
      ret=the_mos.SaturateTransparency(idx, true, deepen);
      if(ret!=m_ptileheader[i].alternate)
      {
        m_ptileheader[i].alternate=(short) ret;
        m_ptileidx[m_ptileheader[i].firsttileprimary]=(short) idx;
        the_area.wedchanged=true;
      }      
    }
  }
  ((CChitemDlg *) AfxGetMainWnd())->end_progress();
  RefreshTile();
  UpdateData(UD_DISPLAY);
}

void CWedTile::OnOK() 
{
	if(the_mos.MosChanged() )
  {
    if(MessageBox("The tileset was changed, changes will be destroyed if you don't save it now.\n"
      "Do you want to save it now?","Tile editor",MB_YESNO)==IDYES)
    {
      write_tis(m_tisname);
    }
  }
	CDialog::OnOK();
}

BOOL CWedTile::PreTranslateMessage(MSG* pMsg) 
{
	m_tooltip.RelayEvent(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}
