// WedTile.cpp : implementation file
//

#include "stdafx.h"
#include <fcntl.h>
#include <sys/stat.h>
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
	m_open = FALSE;
	m_setoverlay = FALSE;
	//}}AFX_DATA_INIT
  m_tilenum=-1;
  m_firstindex=-1;
  m_doornum=-1;
  m_overlay=-1;
  m_pdooridx=NULL;
  m_graphics = false;
}

void CWedTile::DoDataExchange(CDataExchange* pDX)
{
  CString tmpstr;
  CWnd *cw;
  int tmp, tilenum;

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWedTile)
	DDX_Control(pDX, IDC_FIRST, m_indexpicker);
	DDX_Control(pDX, IDC_BLOCKPICKER, m_tilepicker);
	DDX_Check(pDX, IDC_OPEN, m_open);
	DDX_Check(pDX, IDC_OVERLAY, m_setoverlay);
	//}}AFX_DATA_MAP
  if(m_tilenum>=0)
  {
    if(m_pdooridx)
    {
      tilenum=m_pdooridx[m_tilenum];
    }
    else
    {
      tilenum=m_tilenum;
    }
    tmp=m_ptileidx[m_firstindex+m_ptileheader[tilenum].firsttileprimary];
    DDX_Text(pDX, IDC_TILEIDX, m_ptileidx[m_firstindex+m_ptileheader[tilenum].firsttileprimary]);
    if(tmp!=m_ptileidx[m_firstindex+m_ptileheader[tilenum].firsttileprimary])
    {
      the_area.wedchanged=true;
    }
    cw = GetDlgItem(IDC_NUM);
    tmpstr.Format("/ %d",m_ptileheader[tilenum].counttileprimary);
    cw->SetWindowText(tmpstr);
    tmp=m_ptileheader[tilenum].alternate;
    DDX_Text(pDX, IDC_ALT, m_ptileheader[tilenum].alternate);
    DDV_MinMaxInt(pDX, m_ptileheader[tilenum].alternate,-1,the_mos.GetFrameCount());
    if(tmp!=m_ptileheader[tilenum].alternate)
    {
      the_area.wedchanged=true;
    }
    tmp=m_ptileheader[tilenum].flags;
    DDX_Text(pDX, IDC_FLAGS, m_ptileheader[tilenum].flags);
    DDX_Text(pDX, IDC_UNKNOWN, m_ptileheader[tilenum].unknown);

    if(tmp!=m_ptileheader[tilenum].flags)
    {
      the_area.wedchanged=true;
    }
  }
}

BEGIN_MESSAGE_MAP(CWedTile, CDialog)
	//{{AFX_MSG_MAP(CWedTile)
	ON_LBN_SELCHANGE(IDC_BLOCKPICKER, OnSelchangeBlockpicker)
	ON_BN_CLICKED(IDC_ADDALT, OnAddalt)
	ON_BN_CLICKED(IDC_FIXALTER, OnFixalter)
	ON_BN_CLICKED(IDC_GREENWATER, OnGreenwater)
	ON_BN_CLICKED(IDC_OVERLAY, OnOverlay)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_EN_KILLFOCUS(IDC_ALT, DefaultKillfocus)
	ON_CBN_KILLFOCUS(IDC_FIRST, OnKillfocusFirst)
	ON_EN_KILLFOCUS(IDC_TILEIDX, OnKillfocusTileidx)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_BN_CLICKED(IDC_LOAD2, OnLoad2)
	ON_BN_CLICKED(IDC_LOAD3, OnLoad3)
	ON_BN_CLICKED(IDC_OPEN, OnOpen)
	ON_BN_CLICKED(IDC_PREVIEW, OnPreview)
	ON_BN_CLICKED(IDC_DROP, OnDrop)
	ON_EN_KILLFOCUS(IDC_FLAGS, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_UNKNOWN, DefaultKillfocus)
	ON_LBN_DBLCLK(IDC_BLOCKPICKER, OnRemove)
	//}}AFX_MSG_MAP
ON_COMMAND(ID_REFRESH, OnTile)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWedTile message handlers

void CWedTile::RefreshTile()
{
  CString tmpstr, tiles;
  int i,j;
  int mx,my;

  if(!m_tilepicker) return;
  switch(the_mos.m_overlay)
  {
  case -1:
    tmpstr.Format("Edit door tiles (%s)",m_tisname);
    break;
  case 0:
    tmpstr.Format("Edit tiles & overlays (%s)",m_tisname);  
    break;
  default:
    j=0;
    for(i=0;i<10;i++)
    {
      if(the_mos.m_friend[i] && (the_mos.m_overlay&j) )
      {
        if(tiles.GetLength())
        {
          tiles+=", ";
        }
        tiles+=the_mos.m_friend[the_mos.m_overlay]->m_name;
      }
      j<<=1;
    }
    tmpstr.Format("Edit tiles & overlays (%s+%s)",m_tisname, tiles);
    break;
  }
  if(the_mos.MosChanged()) tmpstr+=" *";
  SetWindowText(tmpstr);  
  m_tilenum=m_tilepicker.GetCurSel();
  if(m_tilenum<0) m_tilenum=0;
  m_tilepicker.ResetContent();
  if(m_pdooridx)
  {
    mx=1;
    my=m_maxtile;
  }
  else
  {
    mx=m_tilecountx;
    my=m_tilecounty;
  }
  for(j=0;j<my;j++) for(i=0;i<mx;i++)
  {
    wed_tilemap *wtm;
    
    if(m_overlay==-1)
    {
      wtm=m_ptileheader+m_pdooridx[j];
    }
    else
    {
      wtm=m_ptileheader+(j*mx+i);
    }
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

  if(m_tilenum>=(int) m_maxtile) m_tilenum=m_maxtile;
  m_tilenum=m_tilepicker.SetCurSel(m_tilenum);
  RefreshFields();

}

void CWedTile::RefreshFields()
{
  CString tmpstr;
  DWORD i;
  DWORD tilenum;

  m_firstindex=m_indexpicker.GetCurSel();
  if(m_firstindex<0) m_firstindex=0;
  m_indexpicker.ResetContent();
  if(m_tilenum!=-1)
  {
    if(m_overlay==-1) tilenum=m_pdooridx[m_tilenum];
    else tilenum=m_tilenum;
    for(i=0;i<m_ptileheader[tilenum].counttileprimary;i++)
    {
      tmpstr.Format("%d %d",i+1, m_ptileidx[i+m_ptileheader[tilenum].firsttileprimary]);
      m_indexpicker.AddString(tmpstr);
    }
    if(m_firstindex<0 || m_firstindex>=(int) i) m_firstindex=0;
  }
  m_indexpicker.SetCurSel(m_firstindex);
  RefreshDialog();
}

void CWedTile::RefreshDialog()
{
  CButton *cb;
  int tilenum;

  cb = (CButton *) GetDlgItem(IDC_OPEN);
  if(m_open) cb->SetWindowText("Draw open");
  else cb->SetWindowText("Draw closed");
  the_mos.m_drawclosed=!m_open;

  if(m_preview.m_bm)
  {
    DeleteObject(m_preview.m_bm);
    m_preview.m_bm=0;
  }
  if(m_tilenum!=-1)
  {
    if(m_pdooridx)
    {
      tilenum=m_pdooridx[m_tilenum];
    }
    else
    {
      tilenum=m_tilenum;
    }
    
    m_preview.m_clipx=tilenum%m_tilecountx-m_preview.m_maxextentx/2;
    m_preview.m_clipy=tilenum/m_tilecountx-m_preview.m_maxextenty/2; //it is tilecountx here too!
    if(m_preview.m_clipx<0) m_preview.m_clipx=0;
    if(m_preview.m_clipy<0) m_preview.m_clipy=0;
    m_preview.m_sourcepoint.x=tilenum%m_tilecountx*the_mos.mosheader.dwBlockSize;
    m_preview.m_sourcepoint.y=tilenum/m_tilecountx*the_mos.mosheader.dwBlockSize;
  }
  if(m_graphics)
  {
    m_preview.RedrawContent();
  }
  m_preview.ShowWindow(m_graphics);
}

int buttonidx[]={IDC_SAVE, IDC_ADDALT, IDC_FIXALTER, IDC_GREENWATER, 
IDC_LOAD, IDC_LOAD2, IDC_REMOVE, IDC_PREVIEW, IDC_OPEN,
0};

BOOL CWedTile::OnInitDialog() 
{
  CRect rect;
  CWnd *cw;
  int i;

	CDialog::OnInitDialog();
  //tooltips
  {
    m_tooltip.Create(this,TTS_NOPREFIX);
    m_tooltip.SetMaxTipWidth(200);
    m_tooltip.SetTipBkColor(RGB(240,224,160));
    
    m_tooltip.AddTool(GetDlgItem(IDOK), IDS_CANCEL);
  }
  for(i=0;buttonidx[i];i++)
  {
    cw=GetDlgItem(buttonidx[i]);
    if(cw) cw->EnableWindow(!m_overlay);
  }
  cw=GetDlgItem(IDC_OVERLAY);
  if(cw) cw->EnableWindow(the_mos.m_overlay>0);
  if(m_pdooridx)
  {
    cw=GetDlgItem(IDC_PREVIEW);
    cw->EnableWindow(true);
    cw=GetDlgItem(IDC_OPEN);
    cw->EnableWindow(true);
    cw=GetDlgItem(IDC_ADDALT);
    cw->EnableWindow(true);
    cw->SetWindowText("Add tile");
    cw=GetDlgItem(IDC_REMOVE);
    cw->EnableWindow(true);
    cw->SetWindowText("Remove tile");
    cw=GetDlgItem(IDC_LOAD);
    cw->EnableWindow(true);
    cw->SetWindowText("Set door (tis)");
    cw=GetDlgItem(IDC_LOAD2);
    cw->EnableWindow(true);
    cw->SetWindowText("Set door (bmp)");
    cw=GetDlgItem(IDC_LOAD3);
    cw->ShowWindow(true);
  }

  m_preview.InitView(IW_SHOWGRID|IW_OVERLAY|IW_MARKTILE, &the_mos); //initview must be before create
  m_preview.m_showgrid=true;
  m_preview.Create(IDD_IMAGEVIEW,this);
  GetWindowRect(rect);
  m_preview.SetWindowPos(0,rect.right,rect.top,0,0,SWP_NOZORDER|SWP_HIDEWINDOW|SWP_NOSIZE);
  RefreshTile();
  UpdateData(UD_DISPLAY);
	return TRUE;
}

void CWedTile::OnSelchangeBlockpicker() 
{
  m_tilenum=m_tilepicker.GetCurSel();
  RefreshFields();
  UpdateData(UD_DISPLAY);
}

void CWedTile::OnKillfocusTileidx() 
{
  UpdateData(UD_RETRIEVE);
  RefreshTile();
  UpdateData(UD_DISPLAY);
}

void CWedTile::DefaultKillfocus() 
{
  UpdateData(UD_RETRIEVE);
  RefreshDialog(); //refreshes the graphic too
  UpdateData(UD_DISPLAY);
}

void CWedTile::OnTile() 
{
  CPoint cp;
  int tile;
  DWORD i;

  cp=m_preview.GetPoint(GP_TILE);
  tile=cp.y*m_tilecountx+cp.x;
  if(m_pdooridx)
  {
    //find the doortile
    for(i=0;i<m_maxtile;i++)
    {
      if(m_pdooridx[i]==tile) break;
    }
    if(i==m_maxtile)
    {
      tile=-1;
    }
    else
    {
      tile=i;
    }
  }
  m_tilenum=m_tilepicker.SetCurSel(tile);
  if(m_setoverlay) RefreshTile();
  else RefreshDialog();
  UpdateData(UD_DISPLAY);
}

void CWedTile::OnOverlay() 
{
//  CImageView dlg;
//  CPoint cp;

  UpdateData(UD_RETRIEVE);
  if(m_pdooridx) //add door tiles
  {
    return;
  }
  if(m_setoverlay)
  {
    m_preview.SetMapType(MT_OVERLAY, (LPBYTE) m_ptileheader);
  }
  else
  {
    m_preview.SetMapType(MT_OVERLAY, NULL);
  }
  RefreshDialog();//updates graphical view's title 
}

void CWedTile::OnDrop() 
{
  int i;

  i=m_maxtile;
  while(i--)
  {
    if(m_ptileheader[i].alternate==-1)
    {
      m_ptileheader[i].flags=0;
      the_area.wedchanged=true;
    }
  }
  RefreshTile();
  UpdateData(UD_DISPLAY);
}

void CWedTile::OnFixalter() 
{
  int i;

  i=m_maxtile;
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
  if(m_pdooridx) //add a door tile
  {
    CPoint cp;
    short *newtiles;
    POSITION pos;
    int tile;
    DWORD i;

    cp=m_preview.GetPoint(GP_TILE);
    tile=cp.y*m_tilecountx+cp.x;
    for(i=0;i<m_maxtile;i++)
    {
      if(m_pdooridx[i]==tile)
      {
        m_tilepicker.SetCurSel(i);
        return;
      }
    }
    pos=the_area.doortilelist.FindIndex(m_doornum);
    
    //remove m_tilenum
    newtiles=new short[m_maxtile+1];
    if(!newtiles)
    {
      return;
    }
    memcpy(newtiles, m_pdooridx, m_maxtile*sizeof(short) );
    newtiles[m_maxtile++]=(short) tile;
    m_pdooridx=newtiles; //no need to delete the old pointer
    the_area.doortilelist.SetAt(pos, m_pdooridx);
    the_area.weddoorheaders[m_doornum].countdoortileidx=(unsigned short) m_maxtile;
    the_area.wedchanged=true;
    RefreshTile();
    UpdateData(UD_DISPLAY);
    return;
  }
  if(m_ptileheader[m_tilenum].alternate!=-1)
  {
    MessageBox("This tile already has an alternate.","Warning",MB_ICONINFORMATION|MB_OK);
    return;
  }
	m_ptileheader[m_tilenum].alternate=(short) the_mos.AddTileCopy(m_ptileidx[m_ptileheader[m_tilenum].firsttileprimary]);
  the_area.wedchanged=true;
  RefreshTile();
  UpdateData(UD_DISPLAY);
}

void CWedTile::OnRemove() 
{
  POSITION pos;
  short *newtiles;
  int tile, tilenum;

  the_area.wedchanged=true;
  if(m_pdooridx)
  {
    pos=the_area.doortilelist.FindIndex(m_doornum);

    //remove m_tilenum
    newtiles=new short[--m_maxtile];
    if(!newtiles)
    {
      m_maxtile++;
      return;
    }
    memcpy(newtiles, m_pdooridx, m_tilenum*sizeof(short) );
    memcpy(newtiles+m_tilenum, m_pdooridx+m_tilenum+1, (m_maxtile-m_tilenum)*sizeof(short) );
    m_pdooridx=newtiles; //no need to delete the old pointer
    the_area.doortilelist.SetAt(pos, m_pdooridx);
    the_area.weddoorheaders[m_doornum].countdoortileidx=(unsigned short) m_maxtile;
    RefreshTile();
    UpdateData(UD_DISPLAY);
    return;
  }
  if(m_ptileheader[m_tilenum].alternate==-1)
  {
    MessageBox("This tile has no alternate.","Warning",MB_ICONINFORMATION|MB_OK);
    return;
  }
  tile=m_ptileheader[m_tilenum].alternate;
  the_mos.RemoveTile(tile);
  //removes the same tile from everywhere
  for(tilenum=0;tilenum<the_area.overlaytilecount;tilenum++)
  {
    if(m_ptileheader[tilenum].alternate>tile)
    {
      m_ptileheader[tilenum].alternate--;
    }
    else if(m_ptileheader[tilenum].alternate==tile)
    {
      m_ptileheader[tilenum].alternate=-1;
    }
  }  
  RefreshTile();
  UpdateData(UD_DISPLAY);
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
  i=m_maxtile;
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

void CWedTile::OnSave() 
{
  write_tis(m_tisname,"");
}

void CWedTile::OnKillfocusFirst() 
{
  m_firstindex=m_indexpicker.GetCurSel();
  RefreshTile();
	UpdateData(UD_DISPLAY);	
}

void CWedTile::OnLoad()
{
  int firsttile;

  if(m_pdooridx)
  {
    qsort(m_pdooridx, m_maxtile, sizeof(short), shortsortb);
    LoadTileSetAt(m_tilenum, 1);
    return;
  }
  firsttile=m_ptileidx[m_firstindex+m_ptileheader[m_tilenum].firsttileprimary];
  LoadTileSetAt(firsttile,0);
}

void CWedTile::OnLoad2() 
{
  int firsttile;

  if(m_pdooridx)
  {
    qsort(m_pdooridx, m_maxtile, sizeof(short), shortsortb);
    LoadTileSetAt(m_tilenum, 3);
    return;
  }
  firsttile=m_ptileheader[m_tilenum].alternate;
  LoadTileSetAt(firsttile,0);
}

void CWedTile::OnLoad3()
{
  Cmos tmpmos;
  CString filepath;
  int fhandle;
  int res;
  int bmportis;
  
  qsort(m_pdooridx, m_maxtile, sizeof(short), shortsortb);
  res=OFN_FILEMUSTEXIST|OFN_ENABLESIZING|OFN_EXPLORER;
  if(readonly) res|=OFN_READONLY;  
  bmportis=3;
  CFileDialog m_getfiledlg(TRUE, "bmp", makeitemname(".bmp",0), res, ImageFilter(0x032) );

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
    readonly=m_getfiledlg.GetReadOnlyPref();
    if(bmportis==3)
    {
      res=tmpmos.ReadBmpFromFile(fhandle,-1);
    }
    else
    {
      res=tmpmos.ReadTisFromFile(fhandle,-1,true);
    }
    close(fhandle);
    lastopenedoverride=filepath.Left(filepath.ReverseFind('\\'));
    switch(res)
    {
    case 0:
      InsertTiles(tmpmos,0,1);
      break;
    default:
      MessageBox("Cannot read bitmap!","Error",MB_ICONSTOP|MB_OK);
      break;
    }
  }
  RefreshTile();
  UpdateData(UD_DISPLAY);	
}

void CWedTile::InsertTiles(Cmos &tmpmos, int firsttile, int door)
{
  DWORD i;
  DWORD nFrameWanted;  

  for(i=0;i<tmpmos.tisheader.numtiles;i++)
  {
    if(i>=m_maxtile)
    {
      MessageBox("Couldn't load the whole tileset.","Tile editor",MB_ICONWARNING|MB_OK);
      RefreshTile();
      UpdateData(UD_DISPLAY);	
      return;
    }
    if(door)
    {
      nFrameWanted=m_ptileheader[m_pdooridx[i]].alternate;
      if(nFrameWanted==(DWORD) -1)
      {
        nFrameWanted=the_mos.AddTileCopy(m_pdooridx[i]);
        m_ptileheader[m_pdooridx[i]].alternate=(short) nFrameWanted;
      }
    }
    else
    {
      nFrameWanted=firsttile+i;
    }
    if(the_mos.tisheader.numtiles<=nFrameWanted)
    {
      MessageBox("Couldn't load the whole tileset.","Tile editor",MB_ICONWARNING|MB_OK);
      return;
    }
    the_mos.SetFrameData(nFrameWanted, tmpmos.GetFrameData(i));
  }
}

//flags = 1   door  (alternate)
//flags = 2   bmp   (tis)

void CWedTile::LoadTileSetAt(int firsttile, int flags)
{
  Cmos tmpmos;

  if(firsttile<0)
  {
    MessageBox("Select a position first.","Tile editor",MB_ICONWARNING|MB_OK);
    return;
  }
  if(flags&2) pickerdlg.m_restype=REF_BMP;
  else pickerdlg.m_restype=REF_TIS;
  if(pickerdlg.DoModal()==IDOK)
  {
    if(flags&2)
    {
      if(read_bmp(pickerdlg.m_picked, &tmpmos, false)) return;
    }
    else
    {
      if(read_tis(pickerdlg.m_picked, &tmpmos, false)) return;
    }
    InsertTiles(tmpmos,firsttile,flags&1);
  }
  RefreshTile();
  UpdateData(UD_DISPLAY);	
}

void CWedTile::OnOpen() 
{
  UpdateData(UD_RETRIEVE);
  RefreshDialog();
}

void CWedTile::OnPreview() 
{
  m_graphics=!m_graphics;
  RefreshTile();
}

BOOL CWedTile::PreTranslateMessage(MSG* pMsg) 
{
	m_tooltip.RelayEvent(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}
