// TisDialog.cpp : implementation file
//

#include "stdafx.h"
#include <fcntl.h>
#include <sys/stat.h>
#include "chitem.h"
#include "ImageView.h"
#include "TisDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTisDialog dialog

CTisDialog::CTisDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CTisDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTisDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
  m_overlaynum=0;
  m_tileheaders=NULL;
  m_tileindices=NULL;
}

void CTisDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTisDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
  if(pDX->m_bSaveAndValidate==UD_DISPLAY)
  {
    DDX_Text(pDX, IDC_POSX, m_posx);
    DDX_Text(pDX, IDC_POSY, m_posy);
  }
  DDX_Text(pDX, IDC_MINX, m_minx);
  DDX_Text(pDX, IDC_MINY, m_miny);
  DDX_Text(pDX, IDC_MAXX, m_maxx);
  DDX_Text(pDX, IDC_MAXY, m_maxy);
}

void CTisDialog::setrange(CString tisname, int x, int y, int readnow)
{
  m_tisname=tisname;
  m_posx=x;
  m_posy=y;
  m_minx=0;
  m_maxx=x-1;
  m_miny=0;
  m_maxy=y-1;
  //if we called it from the bare tis editor, then the tis is already loaded
  m_tis_or_mos=readnow&5;
  if(readnow&2)
  {
    if(m_tis_or_mos)
    {
      read_tis(tisname,&the_mos,true);
      the_mos.TisToMos(x,y);
    }
    else
    {
      read_mos(tisname,&the_mos,true);
    }
  }
  if(the_mos.GetFrameCount()<x*y) //the tis framecount may be higher because of doors
  {
    MessageBox("Corrupt Tileset","Tis Extractor",MB_ICONSTOP|MB_OK);
  }
}

BEGIN_MESSAGE_MAP(CTisDialog, CDialog)
	//{{AFX_MSG_MAP(CTisDialog)
	ON_EN_KILLFOCUS(IDC_POSX, OnKillfocusPosx)
	ON_EN_KILLFOCUS(IDC_POSY, OnKillfocusPosy)
	ON_EN_KILLFOCUS(IDC_MAXX, OnKillfocusMaxx)
	ON_EN_KILLFOCUS(IDC_MAXY, OnKillfocusMaxy)
	ON_EN_KILLFOCUS(IDC_MINX, OnKillfocusMinx)
	ON_EN_KILLFOCUS(IDC_MINY, OnKillfocusMiny)
	ON_BN_CLICKED(IDC_BMP, OnBmp)
	ON_BN_CLICKED(IDC_SAVETIS, OnSavetis)
	ON_BN_CLICKED(IDC_TOP, OnTop)
	ON_BN_CLICKED(IDC_BOTTOM, OnBottom)
	ON_BN_CLICKED(IDC_PREVIEW, OnPreview)
	ON_BN_CLICKED(IDC_MINIMAP, OnMinimap)
	ON_BN_CLICKED(IDC_OPEN, OnOpen)
	ON_BN_CLICKED(IDC_CLEARALL, OnClearall)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTisDialog message handlers

void CTisDialog::RefreshDialog()
{
  CButton *cb = (CButton *) GetDlgItem(IDC_OPEN);
  if(the_mos.m_drawclosed) cb->SetWindowText("Closed doors");
  else cb->SetWindowText("Open doors");
  cb->SetCheck(!the_mos.m_drawclosed);
}

void CTisDialog::OnKillfocusPosx() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CTisDialog::OnKillfocusPosy() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CTisDialog::OnKillfocusMaxx() 
{
	UpdateData(UD_RETRIEVE);
  if(m_maxx<0) m_maxx=0;
  else if(m_maxx>=m_posx) m_maxx=m_posx-1;
  if(m_maxx<m_minx) m_maxx=m_minx;
	UpdateData(UD_DISPLAY);
}

void CTisDialog::OnKillfocusMaxy() 
{
	UpdateData(UD_RETRIEVE);
  if(m_maxy<0) m_maxy=0;
  else if(m_maxy>=m_posy) m_maxy=m_posy-1;
  if(m_maxy<m_miny) m_maxy=m_miny;
	UpdateData(UD_DISPLAY);
}

void CTisDialog::OnKillfocusMinx() 
{
	UpdateData(UD_RETRIEVE);
  if(m_minx<0) m_minx=0;
  else if(m_minx>=m_posx) m_minx=m_posx-1;
  if(m_maxx<m_minx) m_maxx=m_minx;
	UpdateData(UD_DISPLAY);
}

void CTisDialog::OnKillfocusMiny() 
{
	UpdateData(UD_RETRIEVE);
  if(m_miny<0) m_miny=0;
  else if(m_miny>=m_posy) m_miny=m_posy-1;
  if(m_maxy<m_miny) m_maxy=m_miny;
	UpdateData(UD_DISPLAY);
}

void CTisDialog::OnBmp() 
{
  SaveTis(0);
}

void CTisDialog::SaveTis(int how)
{
  CString filepath;
  CString newname;
  CString tmpstr;
  int fhandle;
  int res;

  if(readonly)
  {
    MessageBox("You opened it read only!","Warning",MB_ICONEXCLAMATION|MB_OK);
    return;
  }
  res=OFN_HIDEREADONLY|OFN_ENABLESIZING|OFN_EXPLORER;
  CFileDialog m_getfiledlg(FALSE, how?"tis":"bmp", makeitemname(how?".tis":".bmp",0), res, ImageFilter(how?2:3) );

restart:  
  if( m_getfiledlg.DoModal() == IDOK )
  {
    filepath=m_getfiledlg.GetPathName();
    filepath.MakeLower();
    newname=m_getfiledlg.GetFileTitle();
    if(newname.GetLength()>8 || newname.GetLength()<1 || newname.Find(" ",0)!=-1)
    {
      tmpstr.Format("The resource name '%s' is bad, it should be 8 characters long and without spaces.",newname);
      MessageBox(tmpstr,"Warning",MB_ICONEXCLAMATION|MB_OK);
      goto restart;
    }
    if(newname!=itemname && file_exists(filepath) )
    {
      res=MessageBox("Do you want to overwrite "+newname+"?","Warning",MB_ICONQUESTION|MB_YESNO);
      if(res==IDNO) goto restart;
    }
    fhandle=open(filepath+".tmp", O_BINARY|O_RDWR|O_CREAT|O_TRUNC,S_IREAD|S_IWRITE);
    if(fhandle<1)
    {
      MessageBox("Can't write file!","Error",MB_ICONSTOP|MB_OK);
      goto restart;
    }
    if(how) res=the_mos.WriteTisToFile(fhandle,m_minx,m_miny,m_maxx,m_maxy);
    else res=the_mos.WriteTisToBmpFile(fhandle,m_minx,m_miny,m_maxx,m_maxy);
    close(fhandle);
    switch(res)
    {
    case 0:
      unlink(filepath);
      rename(filepath+".tmp",filepath);
      MessageBox("Image exported","Tis extractor",MB_OK);
      break; //saved successfully
    case -2:
      MessageBox("Error while writing file!","Error",MB_ICONSTOP|MB_OK);
      break;
    case -3:
      MessageBox("Not enough memory.","Error",MB_ICONSTOP|MB_OK);
      break;
    default:
      MessageBox("Unhandled error!","Error",MB_ICONSTOP|MB_OK);
    }
  }
}

void CTisDialog::OnSavetis() 
{
  SaveTis(1);
}

void CTisDialog::OnTop() 
{
  CImageView dlg;
  CPoint point;
  
  dlg.InitView(IW_ENABLEBUTTON|IW_SHOWGRID|IW_MARKTILE, &the_mos);
  dlg.m_maxclipx=m_maxx+1;
  dlg.m_maxclipy=m_maxy+1;
  dlg.m_clipx=m_maxx;
  dlg.m_clipy=m_maxy;
  if(dlg.m_maxclipx-dlg.m_minclipx<dlg.m_maxextentx)
  {
    dlg.m_maxextentx=dlg.m_maxclipx-dlg.m_minclipx;
  }
  if(dlg.m_maxclipy-dlg.m_minclipy<dlg.m_maxextenty)
  {
    dlg.m_maxextenty=dlg.m_maxclipy-dlg.m_minclipy;
  }
  dlg.m_showgrid=true;

  if(dlg.DoModal()==IDOK)
  {
    point=dlg.GetPoint(GP_TILE);
    if(point.x>=0 && point.x<=the_mos.mosheader.wColumn &&
       point.y>=0 && point.y<=the_mos.mosheader.wRow)
    {
      m_minx=point.x;
      m_miny=point.y;
      if(m_maxx<m_minx) m_maxx=m_minx;
      if(m_maxy<m_miny) m_maxy=m_miny;
      UpdateData(UD_DISPLAY);
    }
  }
}

void CTisDialog::OnBottom() 
{
  CImageView dlg;
  CPoint point;

  dlg.InitView(IW_ENABLEBUTTON|IW_SHOWGRID|IW_MARKTILE, &the_mos);
  dlg.m_minclipx=m_minx;
  dlg.m_minclipy=m_miny;
  dlg.m_clipx=m_minx;
  dlg.m_clipy=m_miny;
  if(dlg.m_maxclipx-dlg.m_minclipx<dlg.m_maxextentx)
  {
    dlg.m_maxextentx=dlg.m_maxclipx-dlg.m_minclipx;
  }
    if(dlg.m_maxclipy-dlg.m_minclipy<dlg.m_maxextenty)
  {
    dlg.m_maxextenty=dlg.m_maxclipy-dlg.m_minclipy;
  }
  dlg.m_showgrid=true;

  if(dlg.DoModal()==IDOK)
  {
    point=dlg.GetPoint(GP_TILE);
    if(point.x>=0 && point.x<=the_mos.mosheader.wColumn &&
       point.y>=0 && point.y<=the_mos.mosheader.wRow)
    {
      m_maxx=point.x;
      m_maxy=point.y;
      if(m_maxx<m_minx) m_minx=m_maxx;
      if(m_maxy<m_miny) m_miny=m_maxy;
      UpdateData(UD_DISPLAY);
    }
  }
}

void CTisDialog::OnPreview() 
{
	CImageView dlg;
	
  the_mos.SetOverlay(m_overlaynum?1<<m_overlaynum:0, m_tileheaders, m_tileindices);
  dlg.InitView(IW_ENABLEBUTTON|IW_SHOWGRID, &the_mos);
  //instead of initview we set the attributes manually
  dlg.m_clipx=dlg.m_minclipx=m_minx;
  dlg.m_clipy=dlg.m_minclipy=m_miny;
  dlg.m_maxclipx=m_maxx+1;
  dlg.m_maxclipy=m_maxy+1;
  if(dlg.m_maxclipx-dlg.m_minclipx<dlg.m_maxextentx)
  {
    dlg.m_maxextentx=dlg.m_maxclipx-dlg.m_minclipx;
  }
    if(dlg.m_maxclipy-dlg.m_minclipy<dlg.m_maxextenty)
  {
    dlg.m_maxextenty=dlg.m_maxclipy-dlg.m_minclipy;
  }
 
  dlg.DoModal();
}

int tisdialogids[]={IDC_SAVETIS, IDC_MINIMAP, IDC_OPEN,0};

BOOL CTisDialog::OnInitDialog() 
{
  int i;

	CDialog::OnInitDialog();
  for(i=0;tisdialogids[i];i++)
  {
    GetDlgItem(tisdialogids[i])->ShowWindow(m_tis_or_mos==1);
  }
  //this button has no meaning in plain tis editor, we need the wed info
  if(!m_tileheaders) GetDlgItem(IDC_OPEN)->ShowWindow(false);
  switch(m_tis_or_mos)
  {
  case 5:
    SetWindowText("Select overlap");
    RefreshDialog();
    break;
  case 1:
    SetWindowText("Tileset extraction: "+m_tisname);
    RefreshDialog();
    break;
  case 0:
    SetWindowText("Mos extraction: "+m_tisname);
    break;
  }
  //tooltips
  {
    m_tooltip.Create(this,TTS_NOPREFIX);
    m_tooltip.SetMaxTipWidth(200);
    m_tooltip.SetTipBkColor(RGB(240,224,160));
    
    m_tooltip.AddTool(GetDlgItem(IDCANCEL), IDS_CANCEL);
    m_tooltip.AddTool(GetDlgItem(IDC_MINIMAP), IDS_MINIMAP);
  }
	return TRUE;
}

void CTisDialog::OnMinimap() 
{
  CreateMinimap(GetSafeHwnd());
}

void CTisDialog::OnOpen() 
{
	the_mos.m_drawclosed=!the_mos.m_drawclosed;
  RefreshDialog();
}

void CTisDialog::OnClearall() 
{
	m_minx=m_miny=0;
	m_maxx=m_posx-1;
  m_maxy=m_posy-1;
	UpdateData(UD_DISPLAY);
}

BOOL CTisDialog::PreTranslateMessage(MSG* pMsg) 
{
  m_tooltip.RelayEvent(pMsg);	
	return CDialog::PreTranslateMessage(pMsg);
}
