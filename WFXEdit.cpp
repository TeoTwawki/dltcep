// WFXEdit.cpp : implementation file
//

#include "stdafx.h"
#include <fcntl.h>
#include <sys/stat.h>
#include "chitem.h"
#include "WFXEdit.h"
#include "MyFileDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// WFXEdit dialog


WFXEdit::WFXEdit(CWnd* pParent /*=NULL*/)
	: CDialog(WFXEdit::IDD, pParent)
{
	//{{AFX_DATA_INIT(WFXEdit)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

static int boxids[]={IDC_FLAG1, IDC_FLAG2, IDC_FLAG3, IDC_FLAG4, IDC_FLAG5};

void WFXEdit::DoDataExchange(CDataExchange* pDX)
{
  wfx_header tmp;
  CString tmpstr;
  int i, j;
  long value;
  CButton *cb;

  memcpy(&tmp,&the_wfx.header,sizeof(wfx_header));
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(WFXEdit)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
  DDX_Text(pDX, IDC_RADIUS, the_wfx.header.srcurve);
  DDX_Text(pDX, IDC_FREQUENCY, the_wfx.header.frvar);
  DDX_Text(pDX, IDC_VOLUME, the_wfx.header.volvar);
  DDX_Text(pDX, IDC_FLAGS, the_wfx.header.flags);

  value = the_wfx.header.flags;
  j=1;
  for(i=0;i<5;i++)
  {
    if(boxids[i])
    {
      cb=(CButton *) GetDlgItem(boxids[i]);
      cb->SetCheck(!!(value&j));
    }
    j<<=1;
  }
  if(memcmp(&tmp,&the_wfx.header,sizeof(wfx_header)))
  {
    the_wfx.m_changed=true;
  }
  RefreshDialog();
}

void WFXEdit::NewWFX()
{
	itemname="new wfx";
  memset(&the_wfx.header,0,sizeof(wfx_header));
  the_wfx.m_changed=false;
}


BEGIN_MESSAGE_MAP(WFXEdit, CDialog)
	//{{AFX_MSG_MAP(WFXEdit)
	ON_BN_CLICKED(IDC_FLAG1, OnFlag1)
	ON_BN_CLICKED(IDC_FLAG2, OnFlag2)
	ON_BN_CLICKED(IDC_FLAG3, OnFlag3)
	ON_BN_CLICKED(IDC_FLAG4, OnFlag4)
	ON_BN_CLICKED(IDC_FLAG5, OnFlag5)
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_BN_CLICKED(IDC_LOADEX, OnLoadex)
	ON_BN_CLICKED(IDC_SAVEAS, OnSaveas)
	ON_BN_CLICKED(IDC_NEW, OnNew)
	ON_BN_CLICKED(IDC_CHECK, OnCheck)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_EN_KILLFOCUS(IDC_FLAGS, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_VOLUME, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_RADIUS, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_FREQUENCY, DefaultKillfocus)
	ON_COMMAND(ID_FILE_NEW, OnNew)
	ON_COMMAND(ID_CHECK, OnCheck)
	ON_COMMAND(ID_FILE_SAVE, OnSave)
	ON_COMMAND(ID_FILE_SAVEAS, OnSaveas)
	ON_COMMAND(ID_FILE_LOAD, OnLoad)
	ON_COMMAND(ID_FILE_LOADEXTERNAL, OnLoadex)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// WFXEdit message handlers

void WFXEdit::OnLoad() 
{
	int res;
	
  pickerdlg.m_restype=REF_WFX;
  pickerdlg.m_picked=itemname;
	res=pickerdlg.DoModal();
	if(res==IDOK)
	{
		res=read_wfx(pickerdlg.m_picked);
    switch(res)
    {
    case 0:
  		itemname=pickerdlg.m_picked;
      break;
    default:
      MessageBox("Cannot read wave effect!","Error",MB_ICONSTOP|MB_OK);
      NewWFX();
      break;
    }
	}
  RefreshDialog();
  UpdateData(UD_DISPLAY);
}

void WFXEdit::RefreshDialog()
{
  CEdit *chkctrl;

  SetWindowText("Edit wave effect: "+itemname);
  chkctrl = (CEdit *) GetDlgItem(IDC_VOLUME);  
  chkctrl->EnableWindow(!!(the_wfx.header.flags&WFX_VOLUME));
  chkctrl = (CEdit *) GetDlgItem(IDC_FREQUENCY);  
  chkctrl->EnableWindow(!!(the_wfx.header.flags&WFX_FREQUENCY));
  chkctrl = (CEdit *) GetDlgItem(IDC_RADIUS);  
  chkctrl->EnableWindow(!!(the_wfx.header.flags&WFX_SRCURVE));
}

BOOL WFXEdit::OnInitDialog() 
{
	CDialog::OnInitDialog();
	RefreshDialog();	
  //tooltips
  {
    m_tooltip.Create(this,TTS_NOPREFIX);
    m_tooltip.SetMaxTipWidth(200);
    m_tooltip.SetTipBkColor(RGB(240,224,160));
    
    m_tooltip.AddTool(GetDlgItem(IDCANCEL), IDS_CANCEL);
  }	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void WFXEdit::DefaultKillfocus() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void WFXEdit::OnFlag1() 
{
	the_wfx.header.flags^=WFX_CUTSCENE;
	UpdateData(UD_DISPLAY);
}

void WFXEdit::OnFlag2() 
{
	the_wfx.header.flags^=WFX_SRCURVE;
	UpdateData(UD_DISPLAY);
}

void WFXEdit::OnFlag3() 
{
	the_wfx.header.flags^=WFX_FREQUENCY;
	UpdateData(UD_DISPLAY);
}

void WFXEdit::OnFlag4() 
{
	the_wfx.header.flags^=WFX_VOLUME;
	UpdateData(UD_DISPLAY);
}

void WFXEdit::OnFlag5() 
{
	the_wfx.header.flags^=WFX_NO_EAX;
	UpdateData(UD_DISPLAY);
}

static char BASED_CODE szFilter[] = "Wave effect files (*.wfx)|*.wfx|All files (*.*)|*.*||";

void WFXEdit::OnLoadex() 
{
  int fhandle;
  int res;
  
  res=OFN_FILEMUSTEXIST|OFN_ENABLESIZING|OFN_EXPLORER;
  if(readonly) res|=OFN_READONLY;  
  CMyFileDialog m_getfiledlg(TRUE, "wfx", makeitemname(".wfx",0), res, szFilter);

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
    res=the_wfx.ReadWFXFromFile(fhandle,-1);
    close(fhandle);
    lastopenedoverride=filepath.Left(filepath.ReverseFind('\\'));
    switch(res)
    {
    case 0:
      itemname=m_getfiledlg.GetFileTitle();
      itemname.MakeUpper();
      break;
    default:
      MessageBox("Cannot read wave effect!","Error",MB_ICONSTOP|MB_OK);
      NewWFX();
      break;
    }
  }
  RefreshDialog();
  UpdateData(UD_DISPLAY);
}

void WFXEdit::OnSaveas() 
{
  SaveWFX(0);
}

void WFXEdit::OnSave() 
{
  SaveWFX(1);
}


void WFXEdit::SaveWFX(int save) 
{
  CString newname;
  CString tmpstr;
  int res;

  if(readonly)
  {
    MessageBox("You opened it read only!","Warning",MB_ICONEXCLAMATION|MB_OK);
    return;
  }
  res=OFN_HIDEREADONLY|OFN_ENABLESIZING|OFN_EXPLORER;
  CMyFileDialog m_getfiledlg(FALSE, "wfx", makeitemname(".wfx",0), res, szFilter);

  if(save)
  {
    newname=itemname;
    filepath=makeitemname(".wfx",0);
    goto gotname;
  }    
  
restart:  
  //if (filepath.GetLength()) strncpy(m_getfiledlg.m_ofn.lpstrFile,filepath, filepath.GetLength()+1);
  if( m_getfiledlg.DoModal() == IDOK )
  {
    filepath=m_getfiledlg.GetPathName();
    filepath.MakeLower();
    if(filepath.Right(4)!=".wfx")
    {
      filepath+=".wfx";
    }
    newname=m_getfiledlg.GetFileName();
    newname.MakeUpper();
    if(newname.Right(4)==".WFX") newname=newname.Left(newname.GetLength()-4);
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
    
    res = write_wfx(newname, filepath);
    lastopenedoverride=filepath.Left(filepath.ReverseFind('\\'));
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
  UpdateData(UD_DISPLAY);
}

void WFXEdit::OnNew() 
{
	NewWFX();
  RefreshDialog();
  UpdateData(UD_DISPLAY);
}

void WFXEdit::OnCheck() 
{
	// TODO: Add your control notification handler code here
	
}

void WFXEdit::OnCancel() 
{
  CString tmpstr;

  if(the_wfx.m_changed)
  {
    tmpstr.Format("Changes have been made to the wave effect.\n"
      "Do you want to quit without save?\n");
    if(MessageBox(tmpstr,"Warning",MB_YESNO)==IDNO)
    {
      return;
    }
  }
	CDialog::OnCancel();
}

BOOL WFXEdit::PreTranslateMessage(MSG* pMsg) 
{
  m_tooltip.RelayEvent(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}
