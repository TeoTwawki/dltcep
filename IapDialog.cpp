// IapDialog.cpp : implementation file
//

#include "stdafx.h"

#include <fcntl.h>
#include <sys/stat.h>
#include "chitem.h"
#include "IapDialog.h"
#include "options.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIapDialog dialog

CIapDialog::CIapDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CIapDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CIapDialog)
	m_newother = _T("");
	m_newtbg = _T("");
	//}}AFX_DATA_INIT
}

void CIapDialog::DoDataExchange(CDataExchange* pDX)
{
  int tmp;

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIapDialog)
	DDX_Control(pDX, IDC_LAUNCH, m_launch_control);
	DDX_Control(pDX, IDC_OPENTBG, m_opentbg);
	DDX_Control(pDX, IDC_OPENOTHER, m_openother);
	DDX_Control(pDX, IDC_TBG, m_tbgpicker);
	DDX_Control(pDX, IDC_OTHER, m_otherpicker);
	DDX_CBString(pDX, IDC_OTHER, m_newother);
	DDX_CBString(pDX, IDC_TBG, m_newtbg);
	//}}AFX_DATA_MAP
  tmp=m_tbgpicker.GetCount();
  DDX_Text(pDX, IDC_TBGMAX, tmp);
  tmp=m_otherpicker.GetCount();
  DDX_Text(pDX, IDC_OTHERMAX, tmp);
  tmp=WeHaveWeiDU(false);
  DDX_Check(pDX, IDC_WEIDU, tmp);
}

BEGIN_MESSAGE_MAP(CIapDialog, CDialog)
	//{{AFX_MSG_MAP(CIapDialog)
	ON_BN_CLICKED(IDC_NEW, OnNew)
	ON_BN_CLICKED(IDC_SAVEAS, OnSaveas)
	ON_CBN_KILLFOCUS(IDC_OTHER, OnKillfocusOther)
	ON_CBN_KILLFOCUS(IDC_TBG, OnKillfocusTbg)
	ON_BN_CLICKED(IDC_OPENOTHER, OnOpenother)
	ON_BN_CLICKED(IDC_OPENTBG, OnOpentbg)
	ON_CBN_DBLCLK(IDC_OTHER, OnDblclkOther)
	ON_CBN_DBLCLK(IDC_TBG, OnDblclkTbg)
	ON_WM_CONTEXTMENU()
	ON_CBN_SELCHANGE(IDC_OTHER, OnSelchangeOther)
	ON_BN_CLICKED(IDC_LAUNCH, OnLaunch)
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_BN_CLICKED(IDC_WEIDU, OnWeidu)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIapDialog message handlers
void CIapDialog::UpdateControls()
{
  int cnt1, cnt2;
  int i, pos;

  m_tbgpicker.ResetContent();  
  m_otherpicker.ResetContent();
  cnt1=the_iap.iapheader.tbgcount;
  cnt2=the_iap.iapheader.othercount;
  for(i=0;i<cnt1;i++)
  {    
    pos=m_tbgpicker.AddString(the_iap.m_tbgnames[i]);
    m_tbgpicker.SetItemData(pos,the_iap.iapfileheaders[i].launchflag);
  }
  for(i=0;i<cnt2;i++)
  {
    pos=m_otherpicker.AddString(the_iap.m_othernames[i]);
    //other files are placed after tbg files in the IAP
    m_otherpicker.SetItemData(pos,the_iap.iapfileheaders[i+the_iap.iapheader.tbgcount].launchflag);
  }
}

void CIapDialog::RefreshDialog()
{
  SetWindowText("Create IAP: "+itemname);
  UpdateData(UD_DISPLAY);
}

void CIapDialog::Refresh()
{
  int cnt1, cnt2;
  int i;

  cnt1=m_tbgpicker.GetCount();
  cnt2=m_otherpicker.GetCount();
  if(the_iap.AllocateHeaders(cnt1,cnt2))
  {
    MessageBox("Cannot update IAP.","IAP creator",MB_ICONSTOP|MB_OK);
    OnCancel();
    return;
  }
  for(i=0;i<cnt1;i++)
  {
    m_tbgpicker.GetLBText(i,the_iap.m_tbgnames[i]);
    the_iap.iapfileheaders[i].launchflag=m_tbgpicker.GetItemData(i);
  }
  for(i=0;i<cnt2;i++)
  {
    m_otherpicker.GetLBText(i,the_iap.m_othernames[i]);
    //other files are placed after tbg files in the IAP
    the_iap.iapfileheaders[i+the_iap.iapheader.tbgcount].launchflag=m_otherpicker.GetItemData(i);
  }
  RefreshDialog();
}

BOOL CIapDialog::OnInitDialog() 
{
  CString tmpstr, tmpstr1, tmpstr2;

	CDialog::OnInitDialog();
  Refresh();
  m_openother.SetBitmap(theApp.m_bbmp);
  m_opentbg.SetBitmap(theApp.m_bbmp);
  //tooltips
  {
    m_tooltip.Create(this,TTS_NOPREFIX);
    m_tooltip.SetMaxTipWidth(200);
    m_tooltip.SetTipBkColor(RGB(240,224,160));
    
    m_tooltip.AddTool(GetDlgItem(IDCANCEL), IDS_CANCEL);
    tmpstr2.LoadString(IDS_IAP);
    tmpstr1.LoadString(IDS_SAVE);
    tmpstr.Format(tmpstr1, tmpstr2);
    m_tooltip.AddTool(GetDlgItem(IDC_SAVEAS), tmpstr);
    tmpstr1.LoadString(IDS_NEW);
    tmpstr.Format(tmpstr1, tmpstr2);
    m_tooltip.AddTool(GetDlgItem(IDC_NEW), tmpstr);
  }	
	return TRUE;
}

void CIapDialog::NewIAP()
{
  itemname="new iap";
	the_iap.new_iap();
}

void CIapDialog::OnNew() 
{
  NewIAP();
  m_otherpicker.ResetContent();
  m_tbgpicker.ResetContent();
	Refresh();
}

static char BASED_CODE szFilter[] = "IAP files (*.iap)|*.iap|All files (*.*)|*.*||";

void CIapDialog::OnLoad() 
{
  CStringList dummy;
  CString filepath;
  int fhandle;
  int res;
  
  res=OFN_FILEMUSTEXIST|OFN_ENABLESIZING;
  if(readonly) res|=OFN_READONLY;
  CFileDialog m_getfiledlg(TRUE, "iap", makeitemname(".iap",0), res, szFilter);

restart:  
  if( m_getfiledlg.DoModal() == IDOK )
  {
    filepath=m_getfiledlg.GetPathName();
    fhandle=open(filepath, O_RDONLY|O_BINARY);
    if(!fhandle)
    {
      MessageBox("Cannot open file!","Error",MB_OK);
      goto restart;
    }
    theApp.AddToRecentFileList(filepath);
    readonly=m_getfiledlg.GetReadOnlyPref();
    itemname=m_getfiledlg.GetFileTitle(); //itemname moved here because the loader relies on it
    itemname.MakeUpper();
    res=the_iap.read_iap(fhandle,-1,1,dummy); //only open
    close(fhandle);
    switch(res)
    {
    case 0:
      UpdateControls();
      break;
    default:
      MessageBox("Cannot read IAP!","Error",MB_ICONEXCLAMATION|MB_OK);
      OnNew();
      break;
    }
  }
  Refresh();
}

void CIapDialog::OnSaveas() 
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
  if(!m_otherpicker.GetCount() && !m_tbgpicker.GetCount())
  {
    MessageBox("Add some files first!","IAP creator",MB_OK);
    return;
  }
  res=OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_ENABLESIZING;
  CFileDialog m_getfiledlg(FALSE, "iap", makeitemname(".iap",0), res, szFilter);

restart:  
  if( m_getfiledlg.DoModal() == IDOK )
  {
    filepath=m_getfiledlg.GetPathName();
    filepath.MakeLower();
    if(filepath.Right(4)!=".iap")
    {
      filepath+=".iap";
    }
    newname=m_getfiledlg.GetFileName();
    newname.MakeUpper();
    if(newname.Right(4)==".IAP") newname=newname.Left(newname.GetLength()-4);
    if(newname.GetLength()<1)
    {
      tmpstr.Format("The resource name '%s' is bad, it should be 8 characters long and without spaces.",newname);
      MessageBox(tmpstr,"Warning",MB_OK);
      goto restart;
    }
    fhandle=open(filepath+".tmp", O_BINARY|O_RDWR|O_CREAT|O_TRUNC,S_IREAD|S_IWRITE);
    if(fhandle<1)
    {
      MessageBox("Can't write file!","Error",MB_OK);
      goto restart;
    }
    Refresh();
    if(editflg&USEIAP)
    {
      res=the_iap.WriteZip(fhandle);
    }
    else
    {
      res=the_iap.WriteIap(fhandle);
    }
    close(fhandle);
    switch(res)
    {
    case 0:
      itemname=newname;
      unlink(filepath);
      rename(filepath+".tmp",filepath);
      break; //saved successfully
    case -2:
      MessageBox("Error while writing file!","Error",MB_OK);
      break;
    case -3:
      MessageBox("Internal Error (feature block counter incorrect)!","Error",MB_OK);
      break;
    default:
      MessageBox("Unhandled error!","Error",MB_OK);
    }
  }
  RefreshDialog();
}

void CIapDialog::AddOther()
{
  CString tmpstr;
  int pos;

  if(m_newother.IsEmpty()) return;
  if(m_otherpicker.FindStringExact(0,m_newother)!=-1) return; //already on list
  if(bgfolder.CompareNoCase(m_newother.Left(bgfolder.GetLength())) )
  {
    tmpstr.Format("%s does not reside in the game folder.",m_newother);
    MessageBox(tmpstr,"IAP creator",MB_OK);
    return;
  }
  if(!file_exists(m_newother))
  {
    tmpstr.Format("%s does not exist.",m_newother);
    MessageBox(tmpstr,"IAP creator",MB_OK);
    return;
  }
  pos=m_otherpicker.AddString(m_newother);
  m_otherpicker.SetItemData(pos,!m_newother.Right(10).CompareNoCase("readme.txt"));
  m_newother.Empty();
}

void CIapDialog::OnKillfocusOther() 
{
	UpdateData(UD_RETRIEVE);
  AddOther();
}

void CIapDialog::AddTbg()
{
  CString tmpstr;
  int pos;

  if(m_newtbg.IsEmpty()) return;
  if(m_tbgpicker.FindStringExact(0,m_newtbg)!=-1) return; //already on list
  if(!file_exists(m_newtbg))
  {
    tmpstr.Format("%s does not exist.",m_newtbg);
    MessageBox(tmpstr,"IAP creator",MB_OK);
    return;
  }
  if(!checkfile(m_newtbg,"TBG4") )
  {
    tmpstr.Format("%s is not a TBG4 file.",m_newtbg);
    MessageBox(tmpstr,"IAP creator",MB_OK);
    return;
  }
  pos=m_tbgpicker.AddString(m_newtbg);
  m_tbgpicker.SetItemData(pos,0);
  m_newtbg.Empty();
}

void CIapDialog::OnKillfocusTbg() 
{
	UpdateData(UD_RETRIEVE);
  AddTbg();
}

static char BASED_CODE szFilter1[] =
 "Game files (*.*)|*.*|"
 "Animations (*.bam)|*.bam|"
 "Compiled script files (*.bcs)|*.bcs|"
 "Bitmaps (*.bmp)|*.bmp|"
 "Images (*.mos)|*.mos|"
 "Paperdolls (*.plt)|*.plt|"
 "Projectiles (*.pro)|*.pro|"
 "Tilesets (*.tis)|*.tis|"
 "Text files (*.txt)|*.txt|"
 "Videocells (*.vvc)|*.vvc|"
 "Sound files (*.wav)|*.wav|"
 "Wed files (*.wed)|*.wed|"
 "Effect files (*.wfx)|*.wfx||";

void CIapDialog::OnOpenother() 
{
  POSITION pos;
  int flg;
  
  flg=OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|OFN_ALLOWMULTISELECT|OFN_ENABLESIZING;

  CFileDialog m_getfiledlg(TRUE, NULL, bgfolder+"override\\*.*", flg, szFilter1);
  HackForLargeList(m_getfiledlg);
  m_getfiledlg.m_ofn.lpstrTitle="Select game files!";

  if( m_getfiledlg.DoModal() == IDOK )
  {
    pos=m_getfiledlg.GetStartPosition();
    while(pos)
    {
      m_newother=m_getfiledlg.GetNextPathName(pos);
      AddOther();
    }
  }
  RefreshDialog();
}

void CIapDialog::OnOpentbg() 
{
  POSITION pos;
  int flg;
  
  flg=OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|OFN_ALLOWMULTISELECT|OFN_ENABLESIZING|OFN_EXPLORER;
  CFileDialog m_getfiledlg(TRUE, "tbg", bgfolder+"*.tbg", flg, szFilterTbg);
  HackForLargeList(m_getfiledlg);
  m_getfiledlg.m_ofn.lpstrTitle="Select TBG files!";

  if( m_getfiledlg.DoModal() == IDOK )
  {
    pos=m_getfiledlg.GetStartPosition();
    while(pos)
    {
      m_newtbg=m_getfiledlg.GetNextPathName(pos);
      AddTbg();
    }
  }
  RefreshDialog();
}

void CIapDialog::OnDblclkOther() 
{
  int pos;

  pos=m_otherpicker.GetCurSel();
	m_otherpicker.DeleteString(pos);
}

void CIapDialog::OnDblclkTbg() 
{
  int pos;

  pos=m_tbgpicker.GetCurSel();
	m_tbgpicker.DeleteString(pos);
}

int CIapDialog::WeHaveWeiDU(bool toggle)
{
  int i, flg, ret;

  ret=0;
  i=0;
  do
  {
    flg=m_otherpicker.GetItemData(i);  
    if(flg==CB_ERR) break;
    if(flg&4)
    {
      if(toggle)
      {
        ret=true;
        m_otherpicker.DeleteString(i);
        continue;
      }
      return true;
    }
    if((flg==3) && !toggle)
    {
      return true;
    }
    i++;
  }
  while(1);
  if(!toggle) return 0;
  if(ret) return 0;
  if(weidupath.IsEmpty())
  {
    MessageBox("Use the setup first!","Warning",MB_ICONEXCLAMATION|MB_OK);
    return 0;
  }
  i=m_otherpicker.AddString(weidupath);
  m_otherpicker.SetItemData(i,5);
  return 1;
}

#define  WIT_UNUSED -1
#define  WIT_TEXT   0  //textfile (launched by notepad)
#define  WIT_EXE    1  //weidu mostly ;)
#define  WIT_SOURCE 2  //weidu source files (.d, .baf)

int WhatIsThis(CString file)
{
  if(!file.Right(4).CompareNoCase(".txt")) return WIT_TEXT;
//  if(!file.Right(4).CompareNoCase(".exe")) return WIT_EXE;
  if(!file.Right(2).CompareNoCase(".d")) return WIT_SOURCE;
  if(!file.Right(4).CompareNoCase(".baf")) return WIT_SOURCE;
  if(!file.Right(4).CompareNoCase(".tp2")) return WIT_SOURCE;
  return WIT_UNUSED;
}

void CIapDialog::OnContextMenu(CWnd* pWnd, CPoint point) 
{
  CString tmpstr;
  int pick, flg;

  if(pWnd->GetDlgCtrlID()==IDC_OTHER)
  {
    //there is no itemfrompoint function for comboboxes
    pWnd->ScreenToClient(&point);
    pick=point.y/((CComboBox *) pWnd)->GetItemHeight(0)+((CComboBox *) pWnd)->GetTopIndex()-2;
    if(pick<0 || pick>=((CComboBox *) pWnd)->GetCount()) return;
    flg=!((CComboBox *) pWnd)->GetItemData(pick);
    if(flg)
    {
      ((CComboBox *) pWnd)->GetLBText(pick,tmpstr);
      switch(WhatIsThis(tmpstr) )
      {
      case WIT_EXE:
        if(WeHaveWeiDU(false))
        {
          MessageBox("You can mark only one executable as installer.","Warning",MB_ICONWARNING|MB_OK);
          flg=0;
        }
        else
        {
          flg=4;   //this exe must be first unpacked
        }
        break;
      case WIT_SOURCE:
        flg=2;
        break;
      case WIT_TEXT:
        break;
      default:
        MessageBox("Only textfiles could be displayed on unpack.","Warning",MB_ICONWARNING|MB_OK);
      }
    }    
    ((CComboBox *) pWnd)->SetItemData(pick,flg);
    ((CComboBox *) pWnd)->SetCurSel(pick);
    OnSelchangeOther();
  }
}

void CIapDialog::OnSelchangeOther() 
{
  CString tmpstr;
  int pick, flg;

	pick=m_otherpicker.GetCurSel();
	flg=(pick>=0) && (pick<m_otherpicker.GetCount());
  m_launch_control.EnableWindow(flg);
  
  if(flg)
  {
    m_otherpicker.GetLBText(pick,tmpstr);
    switch(WhatIsThis(tmpstr) )
    {
    case WIT_EXE:
      tmpstr="Use as external compiler";
      break;
    case WIT_SOURCE:
      tmpstr="Call external compiler";
      break;
    default:
      tmpstr="Display textfile";
      break;
    }
  }
  else tmpstr="Display textfile";
  m_launch_control.SetWindowText(tmpstr);
  m_launch_control.SetCheck(m_otherpicker.GetItemData(pick)&1);
}

void CIapDialog::OnLaunch() 
{
  CString tmpstr;
	int pick, flg;
	
  pick=m_otherpicker.GetCurSel();
	flg=(pick>=0) && (pick<m_otherpicker.GetCount());
  m_launch_control.EnableWindow(flg);
  if(!flg) return;
  flg=m_otherpicker.GetItemData(pick)^1;
  if(flg&1)
  {
    m_otherpicker.GetLBText(pick,tmpstr);
    switch(WhatIsThis(tmpstr) )
    {
    case WIT_EXE:
      if(WeHaveWeiDU(false))
      {
        MessageBox("You can mark only one executable as installer.","Warning",MB_ICONWARNING|MB_OK);
        flg=0;
      }
      else
      {
        flg|=4;   //this exe must be first unpacked
      }
      break;
    case WIT_SOURCE:
      if(!WeHaveWeiDU(false) )
      {
        MessageBox("Make sure the other side has the external installer (WeiDU) as well.","Warning",MB_ICONWARNING|MB_OK);
      }
      flg|=2;
      break;
    case WIT_TEXT:
      break;
    default:
      MessageBox("Only textfiles could be displayed on unpack.","Warning",MB_ICONWARNING|MB_OK);
    }
  }
  m_otherpicker.SetItemData(pick,flg);
}

void CIapDialog::OnWeidu() 
{
  WeHaveWeiDU(true); //this will add/remove it 
  UpdateData(UD_DISPLAY);
}

BOOL CIapDialog::PreTranslateMessage(MSG* pMsg) 
{
  m_tooltip.RelayEvent(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}
