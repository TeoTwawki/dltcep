// BamEdit.cpp : implementation file
//

#include "stdafx.h"

#include <fcntl.h>
#include <sys/stat.h>
#include "chitem.h"
#include "chitemDlg.h"
#include "BamEdit.h"
#include "PaletteEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//bam play flags
#define PLAYBAM     1
#define CONTINUOUS  2

static char BASED_CODE szFilter[] = "Animation files (*.bam)|*.bam|All files (*.*)|*.*||";
static char BASED_CODE szFilter2[] = "Bitmap files (*.bmp)|*.bmp|Paperdoll files (*.plt)|*.plt|All files (*.*)|*.*||";

CString GetNumberTag(CString fullname, int &startnumber, int &digits)
{
  int len;

  len=fullname.GetLength();
  for(digits=0;digits<5 && digits<len;digits++)
  {
    if(!isdigit(fullname[len-1-digits]) ) break;
  }
  if(!digits) return fullname;
  startnumber=strtonum(fullname.Right(digits) );
  return fullname.Left(len-digits);
}

/////////////////////////////////////////////////////////////////////////////
// CBamEdit dialog

CBamEdit::CBamEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CBamEdit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBamEdit)
	m_framenum2 = 0;
	//}}AFX_DATA_INIT
  topleft=CPoint(40,40);
  bgcolor=RGB(32,32,32);
  background=0;
  memset(hbf,0,sizeof(hbf));
  hbanim=0;
  m_function=0;
  m_adjust=0;
  m_cycledata=0;
}

int controlids[]={IDC_BAMFRAME1, IDC_BAMFRAME2, IDC_BAMFRAME3, IDC_BAMFRAME4};
int bamboxids[]={IDC_XPOS, IDC_YPOS, IDC_RLE, IDC_DROPFRAME, IDC_POS,
0};
//set these only if we have a cycle (empty lookup is allowed)
int cycleids[]={IDC_DROPCYC,IDC_ADDFRAME,IDC_FRAME2,
0};
//set these only if we have a cycle with a lookup table
int cycboxids[]={IDC_CYCFRAME, IDC_PLAY,IDC_SET,IDC_DELFRAME,
0};

void CBamEdit::DoDataExchange(CDataExchange* pDX)
{
  CString tmpstr;
  WORD framenum, invframe;
  int tmp;
  int i;
  CPoint newtopleft, origin;
  CPoint framesize, framepos;
  BOOL rle;
  CStatic *bf_control;
  CButton *cb_control;

	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CBamEdit)
	DDX_Control(pDX, IDC_SAVEORDER, m_saveorder_control);
	DDX_Control(pDX, IDC_BAMFRAME4, m_bamframe4_control);
	DDX_Control(pDX, IDC_BAMFRAME3, m_bamframe3_control);
	DDX_Control(pDX, IDC_BAMFRAME2, m_bamframe2_control);
	DDX_Control(pDX, IDC_BAMFRAME1, m_bamframe1_control);
	DDX_Control(pDX, IDC_CYCFRAME, m_cycleframe_control);
	DDX_Control(pDX, IDC_CYCLE, m_cyclenum_control);
	DDX_Control(pDX, IDC_FRAME, m_framenum_control);
	DDX_Text(pDX, IDC_FRAME2, m_framenum2);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_COMPRESSED, the_bam.m_bCompressed); //kept it for simplicity
  DDX_Text(pDX, IDC_TRANSPARENT, the_bam.m_header.chTransparentIndex);

  framenum=(unsigned short) m_framenum_control.GetCurSel();
  if(pDX->m_bSaveAndValidate==UD_DISPLAY)
  {
    if(m_play&PLAYBAM) tmpstr="Stop Cycle";
    else tmpstr="Play Cycle";
    cb_control=(CButton *) GetDlgItem(IDC_PLAY);
    cb_control->SetWindowText(tmpstr);
    cb_control=(CButton *) GetDlgItem(IDC_BACKGROUND);
    cb_control->SetCheck(background&1);
    cb_control=(CButton *) GetDlgItem(IDC_PAPERDOLL);
    cb_control->SetCheck(background&2);
    tmp=the_bam.GetFrameDataSize(framenum);
    DDX_Text(pDX, IDC_DSIZE, tmp);          //frame data size
    tmp=the_bam.GetFrameCount();
    for(i=0;bamboxids[i];i++)
    {
      GetDlgItem(bamboxids[i])->EnableWindow(tmp>0);
    }
    tmp=m_cycleframe_control.GetCount();
    for(i=0;cycboxids[i];i++)
    {
      GetDlgItem(cycboxids[i])->EnableWindow(tmp>0);
    }
    tmp=the_bam.GetCycleCount();
    for(i=0;cycleids[i];i++)
    {
      GetDlgItem(cycleids[i])->EnableWindow(tmp>0);
    }

    rle=the_bam.GetFrameRLE(framenum);
    framepos=the_bam.GetFramePos(framenum);
    origin=the_bam.GetFramePos(0);
    newtopleft=topleft-the_bam.GetFramePos(framenum)+origin;
    framesize=the_bam.GetFrameSize(framenum);
    m_bamframe1_control.MoveWindow(CRect(newtopleft,framesize));
    the_bam.MakeBitmap(framenum,bgcolor,hbf[0],BM_RESIZE,1,1);
    m_bamframe1_control.SetBitmap(hbf[0]);
    DDX_Text(pDX, IDC_XPOS, framepos.x);
    DDX_Text(pDX, IDC_YPOS, framepos.y);
    DDX_Text(pDX, IDC_XSIZE, framesize.x);
    DDX_Text(pDX, IDC_YSIZE, framesize.y);
    DDX_Check(pDX, IDC_RLE, rle);
    switch(background)
    {
    case 0:
      for(invframe=1;invframe<4;invframe++)
      {
        bf_control=(CStatic *) GetDlgItem(controlids[invframe]);
        bf_control->SetBitmap(0);
      }
      break;
    case 1:
      for(invframe=1;invframe<4;invframe++)
      {
        bf_control=(CStatic *) GetDlgItem(controlids[invframe]);
        
        if(framenum) bf_control->SetBitmap(0);
        else
        {
          newtopleft=topleft-the_bam.GetFramePos(invframe)+origin;
          framesize=the_bam.GetFrameSize(invframe);
          bf_control->MoveWindow(CRect(newtopleft,framesize));
          the_bam.MakeBitmap(invframe,bgcolor,hbf[invframe],BM_RESIZE,1,1);
          bf_control->SetBitmap(hbf[invframe]);
        }
      }
      break;
    case 2:
      bf_control=(CStatic *) GetDlgItem(controlids[1]);
      newtopleft=topleft+CPoint(origin.x,the_bam.GetFrameSize(framenum).y)-the_bam.GetFramePos(framenum^1);
      framesize=the_bam.GetFrameSize(framenum^1);
      bf_control->MoveWindow(CRect(newtopleft,framesize));
      the_bam.MakeBitmap(framenum^1,bgcolor,hbf[1],BM_RESIZE,1,1);
      bf_control->SetBitmap(hbf[1]);
      for(invframe=2;invframe<4;invframe++)
      {
        bf_control=(CStatic *) GetDlgItem(controlids[invframe]);
        bf_control->SetBitmap(0);
      }
    }

  }
  else //retrieve
  {
    DDX_Text(pDX, IDC_XPOS, framepos.x);
    DDX_Text(pDX, IDC_YPOS, framepos.y);
    DDX_Check(pDX, IDC_RLE, rle);
//    DDX_Text(pDX, IDC_XSIZE, framesize.x); //don't store frame size, it needs review
//    DDX_Text(pDX, IDC_YSIZE, framesize.y); //
    //store them
    if(the_bam.GetFrameCount()>framenum)
    {
      the_bam.SetFramePos(framenum, framepos.x, framepos.y);
      if(!the_bam.SetFrameRLE(framenum, rle))
      {
        if(the_bam.GetFrameRLE(framenum)==!rle)
        {
          MessageBox("Sorry the operation failed.","Error",MB_ICONSTOP|MB_OK);
        }
        else
        {
          MessageBox("The program encountered some inconsistency.","Warning",MB_ICONEXCLAMATION|MB_OK);
        }
      }
    }
  }
}

void CBamEdit::NewBam()
{
	itemname="new animation";
  the_bam.new_bam();
}

void CBamEdit::RefreshMenu()
{
  CMenu *pMenu;

  pMenu=GetMenu();
  if(pMenu)
  {
    pMenu->CheckMenuItem(ID_CONTINUOUSPLAY, MF_SET(m_play&CONTINUOUS));
    pMenu->EnableMenuItem(1,MFP_SET(!!the_bam.GetFrameCount()));
  }
}

CString parts[]={"Palette","Frame/cycle indices","Frame lookup table","Raw data"};

void CBamEdit::RefreshDialog()
{
  int nFrames;
  CString tmpstr;
  int so; //save order in bam
  int i;
  int tmp;

  SetWindowText("Edit animation: "+itemname);

  nFrames=the_bam.GetFrameCount();
  tmp=m_framenum_control.GetCurSel();
  if(tmp<0) tmp=0;
  m_framenum_control.ResetContent();
  for(i=0;i<nFrames;i++)
  {
    tmpstr.Format("%d",i);
    m_framenum_control.AddString(tmpstr);
  }
  if(m_framenum_control.SetCurSel(tmp)<0)
  {
    m_framenum_control.SetCurSel(tmp-1);
  }

  nFrames=the_bam.GetCycleCount();
  tmp=m_cyclenum_control.GetCurSel();
  if(tmp<0) tmp=0;
  m_cyclenum_control.ResetContent();
  for(i=0;i<nFrames;i++)
  {
    tmpstr.Format("%d",i);
    m_cyclenum_control.AddString(tmpstr);
  }
  if(m_cyclenum_control.SetCurSel(tmp)<0)
  {
    m_cyclenum_control.SetCurSel(tmp-1);
  }
  
  m_framenum2=0;

  m_saveorder_control.ResetContent();
  so=the_bam.GetSaveOrder();
  for(i=0;i<4;i++)
  {
    tmpstr.Format("%d. %s",i+1, parts[so&3]);
    m_saveorder_control.AddString(tmpstr);
    so>>=2;
  }
  RefreshFramePicker();
  RefreshMenu();
  UpdateData(UD_DISPLAY);
}

void CBamEdit::RefreshFramePicker()
{
  int nCycle, nFrameIndex;
  CPoint CycleData;
  CString tmpstr;
  int i;
  int pos;

  if(m_play&CONTINUOUS)
  {
    playindex=0;
  }
  else
  {
    m_play&=~PLAYBAM;
  }
  playcycle=nCycle=m_cyclenum_control.GetCurSel();
  CycleData=the_bam.GetCycleData(nCycle);
  playmax=CycleData.y;
  m_cycleframe_control.ResetContent();
  pos=m_cycleframe_control.GetCurSel();
  if(pos<0) pos=0;
  for(i=0;i<CycleData.y;i++)
  {
    nFrameIndex=the_bam.GetFrameIndex(nCycle,i);
    if(nFrameIndex<0)
    {
      tmpstr.Format("%d ???",i);
    }
    tmpstr.Format("%d %d",i,nFrameIndex);
    m_cycleframe_control.AddString(tmpstr);
  }
  m_cycleframe_control.SetCurSel(pos);
}

BEGIN_MESSAGE_MAP(CBamEdit, CDialog)
	//{{AFX_MSG_MAP(CBamEdit)
	ON_BN_CLICKED(IDC_COMPRESSED, OnCompressed)
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_BN_CLICKED(IDC_LOADEX, OnLoadex)
	ON_BN_CLICKED(IDC_SAVEAS, OnSaveas)
	ON_CBN_KILLFOCUS(IDC_FRAME, OnKillfocusFrame)
	ON_CBN_SELCHANGE(IDC_FRAME, OnSelchangeFrame)
	ON_CBN_KILLFOCUS(IDC_CYCLE, OnKillfocusCycle)
	ON_CBN_SELCHANGE(IDC_CYCLE, OnSelchangeCycle)
	ON_CBN_SELCHANGE(IDC_CYCFRAME, OnSelchangeCycframe)
	ON_BN_CLICKED(IDC_PLAY, OnPlay)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BACKGROUND, OnBackground)
	ON_COMMAND(ID_SAVEPALETTE, OnSavepalette)
	ON_COMMAND(ID_LOADPALETTE, OnLoadpalette)
	ON_COMMAND(ID_EDITPALETTE, OnEditpalette)
	ON_COMMAND(ID_COLORSCALE, OnColorscale)
	ON_COMMAND(ID_FORCEPALETTE, OnForcepalette)
	ON_COMMAND(ID_ORDERPALETTE, OnOrderpalette)
	ON_COMMAND(ID_TRIMPALETTE, OnTrimpalette)
	ON_BN_CLICKED(IDC_PAPERDOLL, OnPaperdoll)
	ON_CBN_SELCHANGE(IDC_SAVEORDER, OnSelchangeSaveorder)
	ON_BN_CLICKED(IDC_DROPFRAME, OnDropframe)
	ON_COMMAND(ID_COLORSCALE2, OnColorscale2)
	ON_COMMAND(ID_REVERSECYCLE, OnReversecycle)
	ON_BN_CLICKED(IDC_NEW, OnNew)
	ON_COMMAND(ID_FILE_SAVE, OnSave)
	ON_BN_CLICKED(IDC_CHECK, OnCheck)
	ON_BN_CLICKED(IDC_DROPCYC, OnDropcyc)
	ON_BN_CLICKED(IDC_ADDFRAME, OnAddframe)
	ON_BN_CLICKED(IDC_DELFRAME, OnDelframe)
	ON_BN_CLICKED(IDC_NEWFRAME, OnNewframe)
	ON_BN_CLICKED(IDC_NEWCYCLE, OnNewcycle)
	ON_BN_CLICKED(IDC_SET, OnSet)
	ON_COMMAND(ID_CONTINUOUSPLAY, OnContinuousplay)
	ON_BN_CLICKED(IDC_CLEANUP, OnCleanup)
	ON_COMMAND(ID_CYCLE_INSERTCYCLE, OnInsertcycle)
	ON_BN_CLICKED(IDC_CENTER, OnCenter)
	ON_BN_CLICKED(IDC_POS, OnCenterPos)
	ON_BN_CLICKED(IDC_IMPORT, OnImport)
	ON_COMMAND(ID_FILE_LOADEXTERNALBMP, OnLoadexBmp)
	ON_COMMAND(ID_TOOLS_COMPRESSALLFRAMES, OnCompressFrames)
	ON_COMMAND(ID_TOOLS_DECOMPRESSALLFRAMES, OnDecompress)
	ON_COMMAND(ID_TOOLS_ALIGNALL, OnAlign)
	ON_COMMAND(ID_CYCLE_COPYCYCLE, OnCopycycle)
	ON_EN_KILLFOCUS(IDC_XSIZE, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_YSIZE, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_XPOS, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_YPOS, DefaultKillfocus)
	ON_BN_CLICKED(IDC_RLE, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_FRAME2, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_TRANSPARENT, DefaultKillfocus)
	ON_COMMAND(ID_FILE_LOADBAM, OnLoad)
	ON_COMMAND(ID_FILE_LOADEXTERNALBAM, OnLoadex)
	ON_COMMAND(ID_FILE_SAVEBAMAS, OnSaveas)
	ON_COMMAND(ID_ANIMATECYCLE, OnPlay)
	ON_COMMAND(ID_FILE_NEW, OnNew)
	ON_COMMAND(ID_FILE_SAVEBAM, OnSave)
	ON_COMMAND(ID_CLEANUP, OnCleanup)
	ON_COMMAND(ID_CYCLE_SETCYCLE, OnSet)
	ON_COMMAND(ID_CYCLE_REMOVECYCLE, OnDropcyc)
	ON_COMMAND(ID_EDIT_DROPFRAME, OnDropframe)
	ON_COMMAND(ID_CYCLE_ADDCYCLE, OnNewcycle)
	ON_COMMAND(ID_EDIT_ADDFRAME, OnAddframe)
	ON_COMMAND(ID_EDIT_IMPORTFRAME, OnNewframe)
	ON_COMMAND(ID_CYCLE_DROPFRAME, OnDelframe)
	ON_COMMAND(ID_FRAME_CENTERFRAME, OnCenterPos)
	ON_COMMAND(ID_TOOLS_CENTERFRAMES, OnCenter)
	ON_COMMAND(ID_TOOLS_IMPORTFRAMES, OnImport)
	ON_COMMAND(ID_CYCLE_PASTECYCLE, OnCyclePastecycle)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBamEdit message handlers

BOOL CBamEdit::OnInitDialog() 
{
  CString tmpstr1, tmpstr2, tmpstr;
  CWnd *bc;

	CDialog::OnInitDialog();
	RefreshDialog();
  if(no_compress())
  {
    bc=GetDlgItem(IDC_COMPRESSED);
    bc->EnableWindow(false);
  }
  //tooltips
  {
    m_tooltip.Create(this,TTS_NOPREFIX);
    m_tooltip.SetMaxTipWidth(200);
    m_tooltip.SetTipBkColor(RGB(240,224,160));
    
    m_tooltip.AddTool(GetDlgItem(IDCANCEL), IDS_CANCEL);
    tmpstr1.LoadString(IDS_LOAD);
    tmpstr2.LoadString(IDS_ANIMATION);
    tmpstr.Format(tmpstr1, tmpstr2);
    m_tooltip.AddTool(GetDlgItem(IDC_LOAD), tmpstr);
    tmpstr1.LoadString(IDS_LOADEX);
    tmpstr.Format(tmpstr1, tmpstr2);
    m_tooltip.AddTool(GetDlgItem(IDC_LOADEX), tmpstr);
    tmpstr1.LoadString(IDS_SAVE);
    tmpstr.Format(tmpstr1, tmpstr2);
    m_tooltip.AddTool(GetDlgItem(IDC_SAVEAS), tmpstr);
    tmpstr1.LoadString(IDS_NEW);
    tmpstr.Format(tmpstr1, tmpstr2);
    m_tooltip.AddTool(GetDlgItem(IDC_NEW), tmpstr);
    tmpstr1.LoadString(IDS_CHECK);
    tmpstr.Format(tmpstr1, tmpstr2);
    m_tooltip.AddTool(GetDlgItem(IDC_CHECK), tmpstr);
    m_tooltip.AddTool(GetDlgItem(IDC_BACKGROUND), IDS_INVBAM);
    m_tooltip.AddTool(GetDlgItem(IDC_PAPERDOLL), IDS_PAPERDOLL);
    m_tooltip.AddTool(GetDlgItem(IDC_COMPRESSED), IDS_COMPRESSED);
    m_tooltip.AddTool(GetDlgItem(IDC_RLE), IDS_RLE);
    m_tooltip.AddTool(GetDlgItem(IDC_SAVEORDER), IDS_SAVEORDER);
    m_tooltip.AddTool(GetDlgItem(IDC_XPOS), IDS_XPOS);
    m_tooltip.AddTool(GetDlgItem(IDC_YPOS), IDS_YPOS);
    m_tooltip.AddTool(GetDlgItem(IDC_XSIZE), IDS_XSIZE);
    m_tooltip.AddTool(GetDlgItem(IDC_YSIZE), IDS_YSIZE);
    m_tooltip.AddTool(GetDlgItem(IDC_FRAME), IDS_FRAME);
    m_tooltip.AddTool(GetDlgItem(IDC_CYCLE), IDS_CYCLE);
    m_tooltip.AddTool(GetDlgItem(IDC_CYCFRAME), IDS_FINC);
    m_tooltip.AddTool(GetDlgItem(IDC_DSIZE), IDS_DSIZE);
    m_tooltip.AddTool(GetDlgItem(IDC_PLAY), IDS_PLAY);
    m_tooltip.AddTool(GetDlgItem(IDC_DROPFRAME), IDS_DROPFRAME);
    m_tooltip.AddTool(GetDlgItem(IDC_NEWFRAME), IDS_NEWFRAME);
    m_tooltip.AddTool(GetDlgItem(IDC_IMPORT), IDS_MULTIFRAME);
    m_tooltip.AddTool(GetDlgItem(IDC_DROPCYC), IDS_DROPCYCLE);
    m_tooltip.AddTool(GetDlgItem(IDC_NEWCYCLE), IDS_NEWCYCLE);
    m_tooltip.AddTool(GetDlgItem(IDC_CENTER), IDS_CENTER);
    m_tooltip.AddTool(GetDlgItem(IDC_POS), IDS_CENTERFRAME);
    m_tooltip.AddTool(GetDlgItem(IDC_CLEANUP), IDS_CLEANUP);
    m_tooltip.AddTool(GetDlgItem(IDC_TRANSPARENT), IDS_TRANSPARENT);
    m_tooltip.AddTool(GetDlgItem(IDC_FRAME2), IDS_FRAME2);
    m_tooltip.AddTool(GetDlgItem(IDC_ADDFRAME), IDS_ADDFRAME);
    m_tooltip.AddTool(GetDlgItem(IDC_SET), IDS_SETFRAME);
    m_tooltip.AddTool(GetDlgItem(IDC_DELFRAME), IDS_DELFRAME);
  }

	return TRUE;
}

void CBamEdit::OnLoad() 
{
	int res;
	
  pickerdlg.m_restype=REF_BAM;
  pickerdlg.m_picked=itemname;
	res=pickerdlg.DoModal();  
	if(res==IDOK)
	{
    m_play&=~PLAYBAM;
		res=read_bam(pickerdlg.m_picked);
    switch(res)
    {
    case -1:
      MessageBox("Animation loaded with errors.","Warning",MB_ICONEXCLAMATION|MB_OK);
  		itemname=pickerdlg.m_picked;
      break;
    case 0:
  		itemname=pickerdlg.m_picked;
      break;
    default:
      MessageBox("Cannot read animation!","Error",MB_ICONSTOP|MB_OK);
      NewBam();
      break;
    }
    RefreshDialog();
	}
}

void CBamEdit::OnLoadex() 
{
  CString filepath;
  int fhandle;
  int res;
  
  res=OFN_FILEMUSTEXIST|OFN_ENABLESIZING|OFN_EXPLORER;
  if(readonly) res|=OFN_READONLY;  
  CFileDialog m_getfiledlg(TRUE, "bam", makeitemname(".bam",0), res, szFilter);

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
    m_play&=~PLAYBAM;
    res=the_bam.ReadBamFromFile(fhandle,-1,0);
    close(fhandle);
    switch(res)
    {
    case -1:
      MessageBox("Animation loaded with errors.","Warning",MB_ICONEXCLAMATION|MB_OK);
      itemname=m_getfiledlg.GetFileTitle();
      itemname.MakeUpper();
      break;
    case 0:
      itemname=m_getfiledlg.GetFileTitle();
      itemname.MakeUpper();
      break;
    default:
      MessageBox("Cannot read animation!","Error",MB_ICONSTOP|MB_OK);
      NewBam();
      break;
    }
  }
  RefreshDialog();
}

void CBamEdit::Savebam(Cbam &my_bam, int save) 
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
  CFileDialog m_getfiledlg(FALSE, "bam", makeitemname(".bam",0), res, szFilter);
  if(save)
  {
    newname=itemname;
    filepath=makeitemname(".bam",0);
    goto gotname;
  }    
restart:  
  if( m_getfiledlg.DoModal() == IDOK )
  {
    filepath=m_getfiledlg.GetPathName();
    filepath.MakeLower();
    if(filepath.Right(4)!=".bam")
    {
      filepath+=".bam";
    }
    newname=m_getfiledlg.GetFileName();
    newname.MakeUpper();
    if(newname.Right(4)==".BAM") newname=newname.Left(newname.GetLength()-4);
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
    
    fhandle=open(filepath, O_BINARY|O_RDWR|O_CREAT|O_TRUNC,S_IREAD|S_IWRITE);
    if(fhandle<1)
    {
      MessageBox("Can't write file!","Error",MB_ICONSTOP|MB_OK);
      return;
    }
    res=my_bam.WriteBamToFile(fhandle);
    close(fhandle);
    switch(res)
    {
    case 0:
      itemname=newname;
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
}

void CBamEdit::OnSave() 
{
  Savebam(the_bam,1);
}

void CBamEdit::OnSaveas() 
{
  Savebam(the_bam,0);
}

void CBamEdit::OnNew() 
{
	NewBam();
  RefreshDialog();
}

void CBamEdit::OnCompressed() 
{
	the_bam.m_bCompressed^=1;
	UpdateData(UD_DISPLAY);
}

void CBamEdit::DefaultKillfocus() 
{
	UpdateData(UD_RETRIEVE);	
	UpdateData(UD_DISPLAY);
}

void CBamEdit::OnCenterPos() 
{
  CPoint point;
  int i;

  i=m_framenum_control.GetCurSel();
  point=the_bam.GetFrameSize(i);
  the_bam.SetFramePos(i,point.x/2,point.y/2);
  UpdateData(UD_DISPLAY);
}

void CBamEdit::OnKillfocusFrame() 
{
  int nIndex;

	UpdateData(UD_RETRIEVE);
  m_framenum2=m_framenum_control.GetCurSel();
  if(m_framenum2<0) m_framenum2=0;
  m_framenum_control.SetCurSel(m_framenum2);
  m_cyclenum_control.SetCurSel(the_bam.FindFrame(m_framenum2, nIndex) );
  RefreshFramePicker();
  m_cycleframe_control.SetCurSel(nIndex);
	RefreshDialog();
}

void CBamEdit::OnSelchangeFrame() 
{
  int nIndex;

  m_framenum2=m_framenum_control.GetCurSel();
  if(m_framenum2<0) m_framenum2=0; //don't allow this field to go below 0
  m_framenum_control.SetCurSel(m_framenum2);
  m_cyclenum_control.SetCurSel(the_bam.FindFrame(m_framenum2, nIndex) );
  RefreshFramePicker();
  m_cycleframe_control.SetCurSel(nIndex);
	RefreshDialog();
}

void CBamEdit::OnKillfocusCycle() 
{
  int nCycle, nFrameIndex;

	UpdateData(UD_RETRIEVE);
  nCycle=m_cyclenum_control.GetCurSel();
  if(nCycle<0) nCycle=m_cyclenum_control.SetCurSel(0);
  nFrameIndex=the_bam.GetFrameIndex(nCycle,0);
  m_framenum_control.SetCurSel(nFrameIndex);
  RefreshFramePicker();
  m_cycleframe_control.SetCurSel(0);
  if(nFrameIndex>=0)
  {
    m_framenum2=nFrameIndex;
  }
  UpdateData(UD_DISPLAY);
}

void CBamEdit::OnSelchangeCycle() 
{
  int nCycle, nFrameIndex;

  nCycle=m_cyclenum_control.GetCurSel();
  if(nCycle<0) nCycle=m_cyclenum_control.SetCurSel(0);
  nFrameIndex=the_bam.GetFrameIndex(nCycle,0);
  m_framenum_control.SetCurSel(nFrameIndex);
  RefreshFramePicker();
  m_cycleframe_control.SetCurSel(0);
  if(nFrameIndex>=0)
  {
    m_framenum2=nFrameIndex;
  }
	UpdateData(UD_DISPLAY);
}

void CBamEdit::OnSelchangeCycframe() 
{
  int nCycle, nIndex;

  nIndex=m_cycleframe_control.GetCurSel();
  nCycle=m_cyclenum_control.GetCurSel();
  m_framenum2=the_bam.GetFrameIndex(nCycle,nIndex);
//  m_cycleframe.Format("%d %d",nIndex, m_framenum2);
  m_framenum_control.SetCurSel(m_framenum2);
	UpdateData(UD_DISPLAY);
}

void CBamEdit::OnTimer(UINT nIDEvent)
{
  int nFrameIndex;
  CPoint newtopleft;
  CPoint origin;
  CButton *cb;

  if(!(m_play&PLAYBAM) || playindex>playmax)
  {
    KillTimer(nIDEvent);
    cb=(CButton *) GetDlgItem(IDC_PLAY);
    if(cb) cb->SetWindowText("Play Cycle");
    return;
  }
  if(playindex==playmax)
  {
    playindex=0;
    if(!(m_play&CONTINUOUS))
    {
      m_play&=~PLAYBAM;
      nFrameIndex=the_bam.GetFrameIndex(playcycle, 0);
      if(m_cycleframe_control) m_cycleframe_control.SetCurSel(0);
      m_framenum2=nFrameIndex;
    }
  }
  nFrameIndex=the_bam.GetFrameIndex(playcycle, playindex);
  origin=the_bam.GetFramePos(0);
  newtopleft=topleft-the_bam.GetFramePos(nFrameIndex)+origin;
  the_bam.MakeBitmap(nFrameIndex,bgcolor,hbanim,BM_RESIZE,1,1);
  m_bamframe1_control.MoveWindow(CRect(newtopleft,the_bam.GetFrameSize(nFrameIndex)));
  m_bamframe1_control.SetBitmap(hbanim);
  playindex++;
	CDialog::OnTimer(nIDEvent);
}

void CBamEdit::OnPlay() 
{
  int nIndex, nCycle;

  if(m_play&PLAYBAM)
  {
    m_play&=~PLAYBAM;
    nIndex=playindex;
    nCycle=playcycle;
    m_framenum2=the_bam.GetFrameIndex(nCycle,nIndex);
    m_cycleframe_control.SetCurSel(nIndex);
    m_framenum_control.SetCurSel(m_framenum2);
  }
  else
  {
    playcycle=m_cyclenum_control.GetCurSel();
    playmax=the_bam.GetCycleData(playcycle).y;
    playindex=0;
    m_play|=PLAYBAM;
    SetTimer(0,100,NULL);
  }
  UpdateData(UD_DISPLAY);
}

void CBamEdit::OnSavepalette() 
{
  Cbam palette; //save palette of a bam
	
	palette.new_bam();
  palette.SetCompress(0);
  memcpy(palette.m_palette,the_bam.m_palette,the_bam.m_palettesize); //sizeof(palette)
  palette.m_palettesize=the_bam.m_palettesize;
  //fhandle
  Savebam(palette,0);
}

void CBamEdit::Loadpalette(bool force_or_import) 
{
  Cbam palette;
  int fhandle;
  CString filepath;
  int res;
  
  res=OFN_FILEMUSTEXIST|OFN_ENABLESIZING|OFN_EXPLORER;
  if(readonly) res|=OFN_READONLY;  
  CFileDialog m_getfiledlg(TRUE, NULL, makeitemname(".bam",0), res, szFilter);

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
    res=palette.ReadBamFromFile(fhandle,-1,0);
    close(fhandle);
    if(force_or_import) the_bam.ForcePalette(palette.m_palette);
    else
    {
      memcpy(the_bam.m_palette,palette.m_palette,palette.m_palettesize ); //sizeof(palette)
    }
    switch(res)
    {
    case -1:
      MessageBox("Palette imported with errors.","Warning",MB_ICONEXCLAMATION|MB_OK);
      break;
    case 0:
      break;
    default:
      MessageBox("Cannot read palette!","Error",MB_ICONSTOP|MB_OK);
      break;
    }
  }
  RefreshDialog();
}

void CBamEdit::OnLoadpalette() 
{
  Loadpalette(0); //simple transfer
}

void CBamEdit::OnForcepalette() 
{
  Loadpalette(1); //force palette
}

void CBamEdit::OnEditpalette() 
{
	CPaletteEdit dlg;

  if(!the_bam.HasPalette())
  {
    the_bam.InitPalette();
  }
  dlg.SetPalette(the_bam.m_palette, m_function, m_adjust);
  if(dlg.DoModal()==IDOK)
  {
    m_function=dlg.m_function;
    m_adjust=dlg.GetAdjustment();
  }
}

void CBamEdit::OnBackground() 
{
	background^=1;
  if(background&1)
  {
    bgcolor=RGB(240,224,160);
    topleft=CPoint(60,140);
    background=1;
  }
  else
  {
    bgcolor=RGB(32,32,32);  
    topleft=CPoint(40,40);
  }
  UpdateData(UD_DISPLAY);
}

void CBamEdit::OnPaperdoll() 
{
	background^=2;
  if(background&2)
  {
    background=2;
  }
  bgcolor=RGB(32,32,32);  
  topleft=CPoint(40,40);
  UpdateData(UD_DISPLAY);
}

COLORREF predefcolors[]={RGB(128,128,128), RGB(32,32,32), RGB(240,240,240),
RGB(240,240,112),RGB(128,0,0),RGB(0,128,0),RGB(0,0,128),RGB(128,128,0),
RGB(128,128,64),RGB(192,128,64),RGB(192,192,64),RGB(192,160,64),
RGB(255,255,255),RGB(255,255,255),RGB(255,255,255),RGB(255,255,255)
};

void CBamEdit::OnColorscale() 
{
	CColorDialog dlg;
  COLORREF mycolor;

  dlg.m_cc.lpTemplateName="Pick a shade colour";
  dlg.m_cc.lpCustColors=predefcolors;
  if(dlg.DoModal()==IDOK)
  {
    mycolor=dlg.GetColor();
    the_bam.ConvertToGrey(mycolor);
    UpdateData(UD_DISPLAY);
  }
}

void CBamEdit::OnColorscale2() 
{
	CColorDialog dlg;
  COLORREF mycolor;

  dlg.m_cc.lpTemplateName="Pick a shade colour (keep gray)";
  dlg.m_cc.lpCustColors=predefcolors;
  if(dlg.DoModal()==IDOK)
  {
    mycolor=dlg.GetColor();
    the_bam.ConvertToGrey(mycolor,true); //keep gray
    UpdateData(UD_DISPLAY);
  }
}

void CBamEdit::OnOrderpalette() 
{
  UpdateData(UD_RETRIEVE);
  the_bam.OrderPalette();
  UpdateData(UD_DISPLAY);
}

void CBamEdit::OnTrimpalette() 
{
  the_bam.DropUnusedPalette();
}

void CBamEdit::OnSelchangeSaveorder() 
{
	int sel;
  int tmp1[4];
  int tmp2;
  int i;

  sel=m_saveorder_control.GetCurSel();
  if(sel>0 && sel<4)
  {
    tmp2=the_bam.GetSaveOrder();
    for(i=0;i<4;i++)
    {
      tmp1[i]=tmp2&3;
      tmp2>>=2;
    }
    tmp2=tmp1[sel];
    for(i=sel;i>0;i--) tmp1[i]=tmp1[i-1];
    tmp1[0]=tmp2;
    tmp2=0;
    for(i=3;i>=0;i--)
    {
      tmp2<<=2;
      tmp2|=tmp1[i];
    }
    the_bam.SetSaveOrder(tmp2);
    RefreshDialog();
    m_saveorder_control.SetCurSel(0);
  }
}

void CBamEdit::OnDelframe() 
{
  int nIndex, nCycle;

  nIndex=m_cycleframe_control.GetCurSel();
  nCycle=m_cyclenum_control.GetCurSel();
  if(the_bam.RemoveFrameFromCycle(nCycle,nIndex,1))
  {
    MessageBox("Failed...","Bam editor",MB_ICONWARNING|MB_OK);
  }
  RefreshDialog();
}

void CBamEdit::OnAddframe() 
{
  CString tmpstr;
  int nIndex, nCycle;

  nIndex=the_bam.GetFrameCount();  
  if(m_framenum2==nIndex)
  {
    tmpstr.Format("Do you want to create frame #%d?",nIndex);
    if(MessageBox(tmpstr,"Bam editor",MB_YESNO)==IDYES)
    {
      the_bam.AddFrame(m_framenum2,0);
    }
    else goto badframe;
  }
  if(m_framenum2>nIndex || m_framenum2<0)
  {
badframe:
    tmpstr.Format("Please enter a frame number between %d and %d",0,nIndex);
    MessageBox(tmpstr,"Warning",MB_ICONEXCLAMATION|MB_OK);
    return;
  }
  nIndex=m_cycleframe_control.GetCurSel()+1;
  nCycle=m_cyclenum_control.GetCurSel();
  if(the_bam.AddFrameToCycle(nCycle,nIndex, m_framenum2,1))
  {
    MessageBox("Failed...","Bam editor",MB_ICONWARNING|MB_OK);
  }
  RefreshDialog(); //refreshing ALL
}

void CBamEdit::OnSet() 
{
  CString tmpstr;
  int nIndex, nCycle;

  nIndex=the_bam.GetFrameCount();  
  if(m_framenum2==nIndex)
  {
    tmpstr.Format("Do you want to create frame #%d?",nIndex);
    if(MessageBox(tmpstr,"Bam editor",MB_YESNO)==IDYES)
    {
      the_bam.AddFrame(m_framenum2,0);
    }
    else goto badframe;
  }
  if(m_framenum2>nIndex || m_framenum2<0)
  {
badframe:
    tmpstr.Format("Please enter a frame number between %d and %d",0,nIndex);
    MessageBox(tmpstr,"Warning",MB_ICONEXCLAMATION|MB_OK);
    return;
  }
  nIndex=m_cycleframe_control.GetCurSel();
  nCycle=m_cyclenum_control.GetCurSel();
	if(the_bam.SetFrameIndex(nCycle,nIndex, m_framenum2))
  {
    MessageBox("Failed...","Bam editor",MB_ICONWARNING|MB_OK);
  }
  RefreshDialog();
}

void CBamEdit::OnDropframe() 
{
  int nFrameIndex;

  nFrameIndex=m_framenum_control.GetCurSel();
  if(the_bam.DropFrame(nFrameIndex))
  {
    MessageBox("Failed...","Bam editor",MB_ICONWARNING|MB_OK);
  }
  RefreshDialog();
}

void CBamEdit::OnImport() //this imports many frames
{
  Cbam tmpbam;
	CString tmpstr,internalname, tmpfilename, format;
  int nFrameIndex, nStartFrame;
  int startframe, digits;
  int fhandle;
  int res;
	
  //simply reading the first bmp as a bam
  res=PickBmpFile(the_bam);
  if(res) goto endofquest;
  internalname=GetNumberTag(itemname, startframe, digits);
  itemname=internalname;
  if(itemname.IsEmpty()) itemname="new bam";
  nFrameIndex=the_bam.GetFrameCount();
  nStartFrame=nFrameIndex-1;
  do
  {
    format.Format("%s%%s%%0%dd.bmp",m_folder,digits);
    tmpfilename.Format(format,internalname,++startframe);
    if(!file_exists(tmpfilename) ) break;
    fhandle=open(tmpfilename,O_RDONLY|O_BINARY);
    if(!fhandle)
    {
      res=-2;
      break;
    }
    //import
    res=tmpbam.ReadBmpFromFile(fhandle,-1);
    close(fhandle);
    if(res) break;
    res=the_bam.AddFrame(nFrameIndex,tmpbam.GetFrameDataSize(0));
    if(res) break;
    the_bam.ImportFrameData(nFrameIndex, tmpbam);
  }
  while(++nFrameIndex<65000);
endofquest:
  switch(res)
  {
  case -2:
  case -1:
    MessageBox("Animation loaded with errors.","Warning",MB_ICONEXCLAMATION|MB_OK);
    break;
  case 0:
    the_bam.InsertCycle(digits=the_bam.GetCycleCount());
    //adding into the new cycle
    the_bam.AddFrameToCycle(digits,0,nStartFrame,nFrameIndex-nStartFrame);
    break;
  default:
    MessageBox("Out of memory","Error",MB_ICONSTOP|MB_OK);
    break;
  }
  RefreshDialog();
}

void CBamEdit::OnLoadexBmp() 
{
	PickBmpFile(the_bam);
  RefreshDialog();
}

int CBamEdit::PickBmpFile(Cbam &tmpbam)
{
  CString filepath;
  int fhandle;
  int res;

  res=OFN_FILEMUSTEXIST|OFN_ENABLESIZING|OFN_EXPLORER;
  if(readonly) res|=OFN_READONLY;  
  CFileDialog m_getfiledlg(TRUE, "bmp", makeitemname(".bmp",0), res, szFilter2);
  
restart:  
  if( m_getfiledlg.DoModal() == IDOK )
  {
    filepath=m_getfiledlg.GetPathName();
    m_folder=filepath.Left(filepath.GetLength()-m_getfiledlg.GetFileName().GetLength());
    fhandle=open(filepath, O_RDONLY|O_BINARY);
    if(!fhandle)
    {
      MessageBox("Cannot open file!","Error",MB_ICONSTOP|MB_OK);
      goto restart;
    }
    readonly=m_getfiledlg.GetReadOnlyPref();
    res=tmpbam.ReadBmpFromFile(fhandle,-1);
    close(fhandle);
    switch(res)
    {
    case -1:
      MessageBox("Animation loaded with errors.","Warning",MB_ICONEXCLAMATION|MB_OK);
      itemname=m_getfiledlg.GetFileTitle();
      itemname.MakeUpper();
      break;
    case 0:
      itemname=m_getfiledlg.GetFileTitle();
      itemname.MakeUpper();
      break;
    default:
      MessageBox("Cannot read bitmap!","Error",MB_ICONSTOP|MB_OK);
      break;
    }
  }
  return res;
}

void CBamEdit::OnNewframe() //this imports one frame
{
  Cbam tmpbam;
  CString tmpstr;
  CPoint CycleData;
  int nFrameIndex;
  int nCycle;

  nFrameIndex=the_bam.GetFrameCount(); 
  tmpstr.Format("Do you want to insert frame #%d?",nFrameIndex);
  if(MessageBox(tmpstr,"Bam editor",MB_YESNO)==IDYES)
  {
    if(the_bam.AddFrame(nFrameIndex,0))
    {
      MessageBox("Failed...","Bam editor",MB_ICONWARNING|MB_OK);
    }
  }
  else goto endofquest;

  m_play&=~PLAYBAM;
  //importing
  
  if(!PickBmpFile(tmpbam))
  {
    //let's hope this works better
    tmpbam.DropUnusedPalette();            //try to free up at least one entry
    tmpbam.OrderPalette();
    tmpbam.m_header.chTransparentIndex=255;  //this is the least likely used entry
    tmpbam.OrderPalette();
    if(the_bam.GetFrameCount()!=1)
    {
      tmpbam.ForcePalette(the_bam.m_palette); //forcing the major palette
    }
    else
    { //taking the first palette without change
      memcpy(the_bam.m_palette,tmpbam.m_palette,sizeof(the_bam.m_palette) ); 
    }
    the_bam.SetFrameData(nFrameIndex, tmpbam.GetFrameData(0),tmpbam.GetFrameSize(0));
    nCycle=m_cyclenum_control.GetCurSel();
    if(nCycle<0)
    {
      nCycle=0;
      the_bam.InsertCycle(nCycle);
    }
    CycleData=the_bam.GetCycleData(nCycle);
    the_bam.AddFrameToCycle(nCycle,CycleData.y,nFrameIndex,1);
  }

endofquest:
  RefreshDialog();
}

void CBamEdit::OnDropcyc() 
{
  int nCycle;

	nCycle=m_cyclenum_control.GetCurSel();
  if(the_bam.DropCycle(nCycle))
  {
    MessageBox("Failed...","Bam editor",MB_ICONWARNING|MB_OK);
  }
  RefreshDialog();
}

void CBamEdit::OnInsertcycle() 
{
  int nCycle;

	nCycle=m_cyclenum_control.GetCurSel();
  if(nCycle<0) nCycle=0;
	if(the_bam.InsertCycle(nCycle))
  {
    MessageBox("Failed...","Bam editor",MB_ICONWARNING|MB_OK);
  }
  m_cyclenum_control.SetCurSel(nCycle);
  RefreshDialog();
}

void CBamEdit::OnCopycycle() 
{
  CPoint CycleData;
  int nCycle;

	nCycle=m_cyclenum_control.GetCurSel();
  CycleData=the_bam.GetCycleData(nCycle);
  if(m_cycledata) delete [] m_cycledata;
  m_cycledata=NULL;
  m_cyclesize=0;
	if(CycleData.y<1) return;
  m_cycledata=new short[CycleData.y]; //x=firstframe, y=count
  if(!m_cycledata) return;
  m_cyclesize=CycleData.y;
  memcpy(m_cycledata,the_bam.m_pFrameLookup+CycleData.x,sizeof(short)*CycleData.y);
}

void CBamEdit::OnCyclePastecycle() 
{
  CPoint CycleData;
  int nCycle;

	nCycle=m_cyclenum_control.GetCurSel();
  CycleData=the_bam.GetCycleData(nCycle);
  if(CycleData.y<0) return; // allow 0
	the_bam.ReplaceCycle(nCycle,m_cycledata,m_cyclesize);
  RefreshDialog();
}

void CBamEdit::OnNewcycle() 
{
  int nCycle;

	nCycle=m_cyclenum_control.GetCurSel()+1;
	if(the_bam.InsertCycle(nCycle))
  {
    MessageBox("Failed...","Bam editor",MB_ICONWARNING|MB_OK);
  }
  m_cyclenum_control.AddString(""); //dummy
  m_cyclenum_control.SetCurSel(nCycle);
  RefreshDialog();
}

void CBamEdit::OnCenter() 
{
  int i;
  CPoint point;

	i=the_bam.GetFrameCount();
  while(i--)
  {
    point=the_bam.GetFrameSize(i);
    the_bam.SetFramePos(i,point.x/2,point.y/2);
  }
  UpdateData(UD_DISPLAY);
}

void CBamEdit::OnCleanup() 
{
  CString tmpstr;
  int n;

	n=the_bam.DropUnusedFrame(1);
  if(n)
  {
    tmpstr.Format("Dropped %d unused frames.",n);
    MessageBox(tmpstr,"Bam editor",MB_ICONINFORMATION|MB_OK);
    RefreshDialog();
  }
}

void CBamEdit::OnReversecycle() 
{
  int nCycle;
  CPoint CycleData;
  int i;

  nCycle=m_cyclenum_control.GetCurSel();
  CycleData=the_bam.GetCycleData(nCycle);
  CycleData.y--;
  for(i=CycleData.y/2;i>=0;i--)
  {
    the_bam.SwapFrames(CycleData.x+i,CycleData.x+CycleData.y-i);
  }
  RefreshDialog();
}

void CBamEdit::OnAlign() 
{
  int nFrameWanted;
  CPoint point;

  point=the_bam.GetFramePos(0);
	for(nFrameWanted=1;nFrameWanted<the_bam.GetFrameCount();nFrameWanted++)
  {
	  the_bam.SetFramePos(nFrameWanted,point.x,point.y);
  }
  UpdateData(UD_DISPLAY);
}

void CBamEdit::OnDecompress() 
{
  int nFrameWanted;

	for(nFrameWanted=0;nFrameWanted<the_bam.GetFrameCount();nFrameWanted++)
  {
	  the_bam.SetFrameRLE(nFrameWanted,0);
  }
  UpdateData(UD_DISPLAY);
}

void CBamEdit::OnCompressFrames() 
{
  int nFrameWanted;

	for(nFrameWanted=0;nFrameWanted<the_bam.GetFrameCount();nFrameWanted++)
  {
	  the_bam.SetFrameRLE(nFrameWanted,1);
  }
  UpdateData(UD_DISPLAY);
}

void CBamEdit::OnContinuousplay() 
{
	m_play^=CONTINUOUS;
	RefreshMenu();
}

void CBamEdit::OnCheck() 
{
  int ret;

	ret=((CChitemDlg *) AfxGetMainWnd())->check_bam();
  if(ret)
  {
    MessageBox(lasterrormsg,"BAM editor",MB_ICONEXCLAMATION|MB_OK);
  }
}

void CBamEdit::PostNcDestroy() 
{
  int i;

  for(i=0;i<4;i++)
  {
    if(hbf[i]) DeleteObject(hbf[i]);
  }
  if(hbanim) DeleteObject(hbanim);
  if(m_cycledata) delete [] m_cycledata;
  m_cycledata=0;
	CDialog::PostNcDestroy();
}

BOOL CBamEdit::PreTranslateMessage(MSG* pMsg)
{
  m_tooltip.RelayEvent(pMsg);
  return CDialog::PreTranslateMessage(pMsg);
}
