// MosEdit.cpp : implementation file
//

#include "stdafx.h"

#include <fcntl.h>
#include <sys/stat.h>
#include "chitem.h"
#include "chitemDlg.h"
#include "MosEdit.h"
#include "PaletteEdit.h"
#include "ImageView.h"
#include "TisDialog.h"
#include "options.h"
#include "MyFileDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//this may be static, local code
/////////////////////////////////////////////////////////////////////////////
// CMosEdit dialog

CMosEdit::CMosEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CMosEdit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMosEdit)
	m_red = 0;
	m_green = 255;
	m_blue = 0;
	m_limit = 33;
	m_xsize = 0;
	m_ysize = 0;
	//}}AFX_DATA_INIT
  m_function=0;
  m_adjust=0;
}

static int tisboxids[]={IDC_WIDTH,IDC_HEIGHT, IDC_PWIDTH, IDC_PHEIGHT, 
IDC_MINIMAP,//IDC_EXTRACT, 
0};
static int mosboxids[]={IDC_VIEW, IDC_PALETTE, IDC_FRAME, IDC_DROP, IDC_TRANSPARENT,
  IDC_LIMIT, IDC_RED, IDC_GREEN, IDC_BLUE, IDC_EXTRACT,
0};
static int mosboxids2[]={IDC_COMPRESSED, 
0};

void CMosEdit::setrange(int x, int y)
{
  m_xsize=x;
  m_ysize=y;
}

void CMosEdit::DoDataExchange(CDataExchange* pDX)
{
  int i;
  int flg;
  int maxx, maxy;

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMosEdit)
	DDX_Control(pDX, IDC_FRAME, m_framenum_control);
	DDX_Text(pDX, IDC_RED, m_red);
	DDX_Text(pDX, IDC_GREEN, m_green);
	DDX_Text(pDX, IDC_BLUE, m_blue);
	DDX_Text(pDX, IDC_LIMIT, m_limit);
	DDV_MinMaxInt(pDX, m_limit, 0, 100);
	DDX_Text(pDX, IDC_XSIZE, m_xsize);
	DDX_Text(pDX, IDC_YSIZE, m_ysize);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_COMPRESSED, the_mos.m_bCompressed);
  i=0;
  flg=m_framenum_control.GetCount();
  for(i=0;mosboxids[i];i++)
  {
    GetDlgItem(mosboxids[i])->EnableWindow(flg);
  }
  for(i=0;mosboxids2[i];i++)
  {
    GetDlgItem(mosboxids2[i])->EnableWindow(!(tis_or_mos&TM_TIS) && flg);
  }
  i=0;
  for(i=0;tisboxids[i];i++)
  {
    GetDlgItem(tisboxids[i])->EnableWindow((tis_or_mos&TM_TIS) && flg);
  }
  
  GetDlgItem(IDOK)->EnableWindow((tis_or_mos&TM_CONFIRM) && flg);

  DDX_Text(pDX, IDC_WIDTH, (short &) the_mos.mosheader.wColumn);
  DDX_Text(pDX, IDC_HEIGHT, (short &) the_mos.mosheader.wRow);

  maxx=the_mos.GetImageWidth(0,0);
  DDX_Text(pDX, IDC_PWIDTH, maxx);

  maxy=the_mos.GetImageHeight(0,0);
  DDX_Text(pDX, IDC_PHEIGHT, maxy);
}

BEGIN_MESSAGE_MAP(CMosEdit, CDialog)
	//{{AFX_MSG_MAP(CMosEdit)
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_BN_CLICKED(IDC_LOADEX, OnLoadex)
	ON_BN_CLICKED(IDC_SAVEAS, OnSaveas)
	ON_BN_CLICKED(IDC_NEW, OnNew)
	ON_CBN_KILLFOCUS(IDC_FRAME, OnKillfocusFrame)
	ON_CBN_SELCHANGE(IDC_FRAME, OnSelchangeFrame)
	ON_BN_CLICKED(IDC_COMPRESSED, OnCompressed)
	ON_BN_CLICKED(IDC_TRANSPARENT, OnTransparent)
	ON_BN_CLICKED(IDC_PALETTE, OnPalette)
	ON_BN_CLICKED(IDC_VIEW, OnView)
	ON_BN_CLICKED(IDC_DROP, OnDrop)
	ON_EN_KILLFOCUS(IDC_WIDTH, OnKillfocusWidth)
	ON_EN_KILLFOCUS(IDC_HEIGHT, OnKillfocusHeight)
	ON_BN_CLICKED(IDC_EXTRACT, OnExtract)
	ON_COMMAND(ID_FILE_SAVE, OnSave)
	ON_BN_CLICKED(IDC_MINIMAP, OnMinimap)
	ON_COMMAND(ID_FILE_LOADBMP, OnLoadBmp)
	ON_COMMAND(ID_TOOLS_GUESSDIMENSIONS, OnToolsGuessdimensions)
	ON_EN_KILLFOCUS(IDC_LIMIT, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_RED, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_GREEN, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_BLUE, DefaultKillfocus)
	ON_COMMAND(ID_TOOLS_EXTRACTTILES, OnExtract)
	ON_COMMAND(ID_PALETTE_TRIM, OnDrop)
	ON_COMMAND(ID_PALETTE_EDIT, OnPalette)
	ON_COMMAND(ID_FILE_NEW, OnNew)
	ON_COMMAND(ID_FILE_LOAD, OnLoad)
	ON_COMMAND(ID_FILE_LOADEXTERNALSCRIPT, OnLoadex)
	ON_COMMAND(ID_FILE_SAVEAS, OnSaveas)
	ON_COMMAND(ID_TOOLS_CONVERTNIGHT, OnToolsConvertnight)
	//}}AFX_MSG_MAP
ON_COMMAND(ID_REFRESH, OnTile)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMosEdit message handlers

BOOL CMosEdit::OnInitDialog() 
{
  CRect rect;
  CWnd *cb;
  CString tmpstr, tmpstr1, tmpstr2;

	CDialog::OnInitDialog();
  m_preview.InitView(IW_SHOWGRID|IW_MARKTILE|IW_ENABLEBUTTON, &the_mos);
  m_preview.m_showgrid=true;
  m_preview.Create(IDD_IMAGEVIEW,this);
  GetWindowRect(rect);
  m_preview.SetWindowPos(0,rect.right,rect.top,0,0,SWP_NOZORDER|SWP_HIDEWINDOW|SWP_NOSIZE);

	RefreshDialog();
  //tooltips
  {
    m_tooltip.Create(this,TTS_NOPREFIX);
    m_tooltip.SetMaxTipWidth(200);
    m_tooltip.SetTipBkColor(RGB(240,224,160));
    
    m_tooltip.AddTool(GetDlgItem(IDCANCEL), IDS_CANCEL);
    tmpstr1.LoadString(IDS_LOAD);
    if(tis_or_mos&TM_TIS) tmpstr2.LoadString(IDS_TILESET);
    else tmpstr2.LoadString(IDS_MOS);
    tmpstr.Format(tmpstr1, tmpstr2);
    cb=GetDlgItem(IDC_LOAD);
    m_tooltip.AddTool(cb, tmpstr);
    cb->SetWindowText((tis_or_mos&TM_TIS)?"Load Tis":"Load Mos");
    tmpstr1.LoadString(IDS_LOADEX);
    tmpstr.Format(tmpstr1, tmpstr2);
    cb=GetDlgItem(IDC_LOADEX);
    m_tooltip.AddTool(cb, tmpstr);
    cb->SetWindowText((tis_or_mos&TM_TIS)?"Load external Tis":"Load external Mos");
    tmpstr1.LoadString(IDS_SAVE);
    tmpstr.Format(tmpstr1, tmpstr2);
    cb=GetDlgItem(IDC_SAVEAS);
    m_tooltip.AddTool(cb, tmpstr);
    cb->SetWindowText((tis_or_mos&TM_TIS)?"Save Tis As":"Save Mos As");
    tmpstr1.LoadString(IDS_NEW);
    tmpstr.Format(tmpstr1, tmpstr2);
    cb=GetDlgItem(IDC_NEW);
    m_tooltip.AddTool(cb, tmpstr);
    cb->SetWindowText((tis_or_mos&TM_TIS)?"New Tis":"New Mos");
    m_tooltip.AddTool(GetDlgItem(IDC_MINIMAP), IDS_MINIMAP);
    m_tooltip.AddTool(GetDlgItem(IDC_EXTRACT), IDS_TISEXTRACT);
    m_tooltip.AddTool(GetDlgItem(IDOK), IDS_OVERLAY);
  }	
	return TRUE;  
}

void CMosEdit::OnLoadBmp()
{
	int res;
	
  pickerdlg.m_restype=REF_BMP;
  pickerdlg.m_picked=itemname;
	res=pickerdlg.DoModal();
	if(res==IDOK)
	{
    res=read_bmp(pickerdlg.m_picked, &the_mos);
    switch(res)
    {
    case -1:
      MessageBox("Image loaded with errors.","Warning",MB_ICONEXCLAMATION|MB_OK);
  		itemname=pickerdlg.m_picked;
      break;
    case 0:
  		itemname=pickerdlg.m_picked;
      break;
    default:
      MessageBox("Cannot read image!","Error",MB_ICONSTOP|MB_OK);
      NewMos(tis_or_mos);
      break;
    }
  }
  m_preview.ShowWindow(false);
  RefreshDialog();
}

void CMosEdit::OnLoad() 
{
	int res;
	
  pickerdlg.m_restype=(tis_or_mos&TM_TIS)?REF_TIS:REF_MOS;
  pickerdlg.m_picked=itemname;
	res=pickerdlg.DoModal();
	if(res==IDOK)
	{
 		itemname=pickerdlg.m_picked;
		if(tis_or_mos&TM_TIS) res=read_tis(pickerdlg.m_picked,false);
    else res=read_mos(pickerdlg.m_picked,false);
    switch(res)
    {
    case -1:
      MessageBox("Image loaded with errors.","Warning",MB_ICONEXCLAMATION|MB_OK);
      break;
    case 0:
      break;
    default:
      MessageBox("Cannot read image!","Error",MB_ICONSTOP|MB_OK);
      NewMos(tis_or_mos);
      break;
    }
	}
  m_preview.ShowWindow(false);
  RefreshDialog();
}

void CMosEdit::OnLoadex() 
{
  CString tmpstr;
  int fhandle;
  int res;
  long tm1,tm2;
  
  res=OFN_FILEMUSTEXIST|OFN_ENABLESIZING|OFN_EXPLORER;
  if(readonly) res|=OFN_READONLY;  
  CMyFileDialog m_getfiledlg(TRUE, (tis_or_mos&TM_TIS)?"tis":"mos", makeitemname(tis_or_mos?".tis":".mos",0), res, ImageFilter(tis_or_mos?0x0312:0x0321) );

restart:  
  //if (filepath.GetLength()) strncpy(m_getfiledlg.m_ofn.lpstrFile,filepath, filepath.GetLength()+1);
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
    if(!filepath.Right(4).CompareNoCase(".bmp"))
    {
      time(&tm1);
      res=the_mos.ReadBmpFromFile(fhandle,-1); //read bmp 
      if(!res)
      {
        time(&tm2);
        tmpstr.Format("Average quality loss: %d per pixel\nElapsed time: %d",the_mos.m_nQualityLoss/100,tm2-tm1);
        MessageBox(tmpstr,"Image Editor",MB_ICONINFORMATION|MB_OK);
      }
    }
    else
    {
      if(tis_or_mos&TM_TIS) res=the_mos.ReadTisFromFile(fhandle, NULL, true, false);
      else res=the_mos.ReadMosFromFile(fhandle,-1);
    }
    close(fhandle);
    lastopenedoverride=filepath.Left(filepath.ReverseFind('\\'));
    switch(res)
    {
    case 1:
      MessageBox("Image loaded, IETME bug fixed.","Warning",MB_ICONWARNING|MB_OK);
      itemname=m_getfiledlg.GetFileTitle();
      itemname.MakeUpper();
      break;
    case 0:
      itemname=m_getfiledlg.GetFileTitle();
      itemname.MakeUpper();
      break;
    case -3:
      MessageBox("Out of memory","Error",MB_ICONSTOP|MB_OK);
      NewMos(tis_or_mos);
      break;
    case -1:
      MessageBox("Image is incompatible.","Error",MB_ICONSTOP|MB_OK);
      NewMos(tis_or_mos);
      break;
    default:
      MessageBox("Cannot read image!","Error",MB_ICONSTOP|MB_OK);
      NewMos(tis_or_mos);
      break;
    }
  }
  m_preview.ShowWindow(false);
  RefreshDialog();
}

int CMosEdit::Savemos(Cmos &my_mos, int save) 
{
  CString newname;
  CString tmpstr;
  int bmpsave;
  int fhandle;
  int res;

  if(readonly)
  {
    MessageBox("You opened it read only!","Warning",MB_ICONEXCLAMATION|MB_OK);
    return -1;
  }
  res=OFN_HIDEREADONLY|OFN_ENABLESIZING|OFN_EXPLORER;
  CMyFileDialog m_getfiledlg(FALSE, (tis_or_mos&TM_TIS)?"tis":"mos", makeitemname("",0), res, ImageFilter(tis_or_mos?0x03142:0x03421));

  if(save)
  {
    newname=itemname;
    filepath=makeitemname((tis_or_mos&TM_TIS)?".tis":".mos",0);
    bmpsave=false;
    goto gotname;
  }    
restart:  
  //if (filepath.GetLength()) strncpy(m_getfiledlg.m_ofn.lpstrFile,filepath, filepath.GetLength()+1);
  if( m_getfiledlg.DoModal() == IDOK )
  {
    filepath=m_getfiledlg.GetPathName();
    filepath.MakeLower();
    if(filepath.Right(4)!=".bmp")
    {
      if((tis_or_mos&TM_TIS)) tmpstr=".tis";
      else tmpstr=".mos";
      if(filepath.Right(4)!=".tis" && filepath.Right(4)!=".mos")
      {
        filepath+=tmpstr;
      }
      bmpsave=false;
    }
    else
    {
      tmpstr=".bmp";
      bmpsave=true;
    }
    newname=m_getfiledlg.GetFileTitle();
gotname:
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
    if(bmpsave)
    {
      res=my_mos.WriteTisToBmpFile(fhandle);
    }
    else
    {
      if((tis_or_mos&TM_TIS))
      {
        res=my_mos.WriteTisToFile(fhandle);
      }
      else res=my_mos.WriteMosToFile(fhandle);
    }
    close(fhandle);
    lastopenedoverride=filepath.Left(filepath.ReverseFind('\\'));
    switch(res)
    {
    case 1:
      MessageBox("The tileset was padded to a multiplication of 64","Warning",MB_ICONWARNING|MB_OK);
      res=0;
      //fall thru
    case 0:
      itemname=newname;
      unlink(filepath);
      rename(filepath+".tmp",filepath);
      break; //saved successfully
    case -2:
      MessageBox("Error while writing file!","Error",MB_ICONSTOP|MB_OK);
      break;
    case -3:
      MessageBox("Internal Error!","Error",MB_ICONSTOP|MB_OK);
      break;
    default:
      MessageBox("Unhandled error!","Error",MB_ICONSTOP|MB_OK);
    }
  }
  RefreshDialog();
  return res;
}

void CMosEdit::OnSave() 
{
  Savemos(the_mos,1);
}

void CMosEdit::OnSaveas() 
{
  Savemos(the_mos,0);
}

void CMosEdit::NewMos(int torm) 
{
  tis_or_mos=torm;
  if(torm) itemname="new tileset";
	else itemname="new image";
  the_mos.new_mos();
}

void CMosEdit::RefreshDialog()
{
  CPoint sizes;
  int nFrames;
  CString tmpstr;
  int pos;
  int i;

  if(tis_or_mos&TM_TIS) SetWindowText("Edit Tis: "+itemname);
  else SetWindowText("Edit Mos: "+itemname);
  nFrames=the_mos.GetFrameCount();
  pos=m_framenum_control.GetCurSel();
  if(pos<0) pos=0;
  m_framenum_control.ResetContent();
  for(i=0;i<nFrames;i++)
  {
    tmpstr.Format("%d",i);
    m_framenum_control.AddString(tmpstr);
  }
  if(pos>=i) pos=0;
  pos=m_framenum_control.SetCurSel(pos);
	sizes=the_mos.GetFrameSize(pos);
  m_xsize=sizes.x;
  m_ysize=sizes.y;
  if(the_mos.mosheader.wColumn && the_mos.mosheader.wRow)
  {
    if(editflg&LARGEWINDOW)
    {
      m_preview.m_maxextentx=m_preview.m_maxextenty=8;//maximum number of tiles fitting on screen
    }
    else
    {
      m_preview.m_maxextentx=m_preview.m_maxextenty=10;
    }
    m_preview.m_maxclipx=the_mos.mosheader.wColumn;
    m_preview.m_maxclipy=the_mos.mosheader.wRow;
    m_preview.RedrawContent();
  }
  else
  {
    m_preview.ShowWindow(false);
  }
  UpdateData(UD_DISPLAY);
}

void CMosEdit::OnNew() 
{
  NewMos(tis_or_mos);
  RefreshDialog();
}

void CMosEdit::OnKillfocusFrame() 
{
  CPoint sizes;

	UpdateData(UD_RETRIEVE);
	sizes=the_mos.GetFrameSize(m_framenum_control.GetCurSel());
  m_xsize=sizes.x;
  m_ysize=sizes.y;
	UpdateData(UD_DISPLAY);
}

void CMosEdit::OnSelchangeFrame() 
{
  CPoint sizes;

	sizes=the_mos.GetFrameSize(m_framenum_control.GetCurSel());
  m_xsize=sizes.x;
  m_ysize=sizes.y;
	UpdateData(UD_DISPLAY);
}

void CMosEdit::OnCompressed() 
{
	the_mos.m_bCompressed^=1;
	UpdateData(UD_DISPLAY);
}

void CMosEdit::DefaultKillfocus() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CMosEdit::OnTransparent() 
{
  CString tmpstr;
  DWORD nFrame;
  COLORREF redgreenblue;
  int nHits, nTotal, nCount;

  redgreenblue=m_red|(m_green<<8)|(m_blue<<16);
  nTotal=nCount=0;
  for(nFrame=0;nFrame<the_mos.tisheader.numtiles;nFrame++)
  {
    nHits=the_mos.MakeTransparent(nFrame, redgreenblue, m_limit);
    if(nHits<0) return;
    if(nHits)
    {
      nTotal+=nHits;
      nCount++;
    }
  }
  if(nTotal)
  {
    tmpstr.Format("Modified %d pixels in %d frames.",nTotal,nCount);
    MessageBox(tmpstr,"Image Editor",MB_ICONINFORMATION|MB_OK);
  }
}

void CMosEdit::OnPalette() 
{
  unsigned long *pos;
	CPaletteEdit dlg;

  pos=the_mos.GetFramePalette(m_framenum_control.GetCurSel());
  if(!pos)
  {
    MessageBox("There is no palette.","Image editor",MB_ICONEXCLAMATION|MB_OK);
    return;
  }
  dlg.SetPalette(pos, m_function, m_adjust); //really set palette
  if(dlg.DoModal()==IDOK)
  {
    m_function=dlg.m_function;
    m_adjust=dlg.GetAdjustment();
  }
}

void CMosEdit::OnView() 
{
  m_preview.m_showgrid=true;
  RefreshDialog();
  m_preview.ShowWindow(true);
}
void CMosEdit::OnTile()
{
  CPoint point;
  
  point=m_preview.GetPoint(GP_TILE);
  if(point.x>=0 && point.x<=the_mos.mosheader.wColumn &&
    point.y>=0 && point.y<=the_mos.mosheader.wRow)
  {
    m_framenum_control.SetCurSel(point.y*the_mos.mosheader.wColumn+point.x);
    RefreshDialog();
  }
}

void CMosEdit::OnDrop() 
{
	the_mos.DropUnusedPalette();
	RefreshDialog();
}

BOOL CMosEdit::PreTranslateMessage(MSG* pMsg) 
{
  m_tooltip.RelayEvent(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}

void CMosEdit::OnKillfocusWidth() 
{
  UpdateData(UD_RETRIEVE);
  if(the_mos.mosheader.wColumn*the_mos.mosheader.wRow>the_mos.GetFrameCount())
  {
    the_mos.mosheader.wRow=(WORD) (the_mos.GetFrameCount()/the_mos.mosheader.wColumn);
  }
  the_mos.TisToMos(the_mos.mosheader.wColumn,the_mos.mosheader.wRow);
  RefreshDialog();
  UpdateData(UD_DISPLAY);
}

void CMosEdit::OnKillfocusHeight() 
{
	UpdateData(UD_RETRIEVE);
  if(the_mos.mosheader.wColumn*the_mos.mosheader.wRow>the_mos.GetFrameCount())
  {
    the_mos.mosheader.wColumn=(WORD) (the_mos.GetFrameCount()/the_mos.mosheader.wRow);
  }
  the_mos.TisToMos(the_mos.mosheader.wColumn,the_mos.mosheader.wRow);
  RefreshDialog();
	UpdateData(UD_DISPLAY);
}

void CMosEdit::OnExtract() 
{
	CTisDialog dlg;

  m_preview.ShowWindow(false);
  dlg.setrange(itemname,the_mos.mosheader.wColumn,the_mos.mosheader.wRow,tis_or_mos);
  dlg.DoModal();
}

void CMosEdit::OnToolsConvertnight() 
{
	if (itemname.GetLength()==6)
  {
    itemname+="N";
    the_mos.m_changed=true;
    the_mos.DropUnusedPalette();
    the_mos.ApplyPaletteRGB(200,100,100,256);
    RefreshDialog();
  }
  else
  {
    MessageBox("The name should be exactly 6 characters long.","Tileset editor",MB_OK);
  }
}


void CMosEdit::OnOK() 
{
  loc_entry dummy;

  if(tis.Lookup(itemname,dummy) )
  {
    if(MessageBox("Tis already exists, you don't have to create it.\nDo you still want to create it?","Tileset editor",MB_YESNO)!=IDYES)
    {
      CDialog::OnOK();
      return; //never reach this point
    }    
  }
  //here we save the .tis and optionally create the .mos for the area
	if(the_mos.mosheader.wColumn*the_mos.mosheader.wRow!=(int) the_mos.tisheader.numtiles)
  {
    if(MessageBox("This TIS has incorrect or unset dimensions, or extra door tiles.\nDo you want to still use it?","Tileset editor",MB_YESNO)!=IDYES)
    {
      return;
    }
  }	
  if(!Savemos(the_mos,0))
  {
    UpdateIEResource(itemname,REF_TIS,"override\\"+itemname+".tis",-1);
	  CDialog::OnOK();
  }
}

void CMosEdit::OnMinimap() 
{
  CreateMinimap(GetSafeHwnd());
}

unsigned int CMosEdit::TileDiff(unsigned int upper, unsigned int lower)
{
  unsigned int i;
  unsigned int diff=0;
  BYTE *a, *b;
  unsigned long *pa, *pb;

  pa=the_mos.GetFramePalette(upper);
  pb=the_mos.GetFramePalette(lower);
  a=the_mos.GetFrameBuffer(upper)+63*64; //lower line of upper tile
  b=the_mos.GetFrameBuffer(lower);       //upper line of lower tile
  for(i=0;i<64;i++)
  {
    diff+=ChiSquare((BYTE *) &pa[a[i]], (BYTE *) &pb[b[i]]);
  }
  return diff;
}

unsigned int CMosEdit::RowDiff(unsigned int increment)
{
  unsigned int i,j,k;
  unsigned __int64 diff=0;

  j=0;
  for(i=0;i+increment*2<the_mos.tisheader.numtiles;i+=increment)
  {
    for(k=0;k<increment;k++)
    {
      diff+=TileDiff(i+k,i+increment+k);
      j++;
      if(j>100) goto done; //we got enough samples
    }
  }
  if(!j) return 65535;
done:
  return (unsigned int) (diff/j);
}

void CMosEdit::OnToolsGuessdimensions() 
{
	if(!tis_or_mos)
  {
    return;
  }
	if(the_mos.mosheader.wColumn!=1)
  {
    return;
  }
  unsigned long i;
  unsigned long best=0;
  unsigned int diff;
  unsigned int mindiff=655350;

  for(i=1;(i<65535) && (i<the_mos.tisheader.numtiles/2);i++)
  {    
    diff=RowDiff(i);
    if(mindiff>diff)
    {
      mindiff=diff;
      best=i;
    }
  }
  if(best)
  {
    the_mos.mosheader.wColumn=(unsigned short) best;
    UpdateData(UD_DISPLAY);
    OnKillfocusWidth();
  }
}
