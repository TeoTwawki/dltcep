// ItemEdit.cpp : implementation file
//

#include "stdafx.h"

#include <fcntl.h>
#include <sys/stat.h>
#include "chitem.h"
#include "chitemDlg.h"
#include "ItemEdit.h"
#include "StrRefDlg.h"
#include "tbg.h"
#include "MyFileDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CItemEdit dialog


CItemEdit::CItemEdit(CWnd* pParent /*=NULL*/)
: CDialog(CItemEdit::IDD, pParent)
{
	m_pModelessPropSheet = NULL;
	//{{AFX_DATA_INIT(CItemEdit)
	//}}AFX_DATA_INIT
}

void CItemEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CItemEdit)
	//}}AFX_DATA_MAP
}

void CItemEdit::NewItem()
{
	itemname="new item";
  the_item.KillExtHeaders();
  the_item.KillFeatHeaders();
  memset(&the_item.header,0,sizeof(item_header));
  memset(&the_item.iwd2header,0,sizeof(iwd2_header));
  if(iwd2_structures() ) //iwd2
  {
    the_item.revision=20;
  }
  else if(has_xpvar()) //iwd
  {
    the_item.revision=10;
  }
  else if(pst_compatible_var()) //pst
  {
    the_item.revision=11;
  }
  else //bg1, bg2
  {
    the_item.revision=1;
  }
  the_item.header.iddesc=-1;
  the_item.header.idref=-1;
  the_item.header.uniddesc=-1;
  the_item.header.unidref=-1;
  the_item.header.extheadoffs=sizeof(item_header);  
  the_item.header.featureoffs=sizeof(item_header);
	the_item.header.itmattr=ATTR_COPYABLE|ATTR_MOVABLE|ATTR_DISPLAY; //these bits are usually set
  the_item.m_changed=false;
}

BEGIN_MESSAGE_MAP(CItemEdit, CDialog)
//{{AFX_MSG_MAP(CItemEdit)
ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_BN_CLICKED(IDC_LOADEX, OnLoadex)
	ON_BN_CLICKED(IDC_SAVEAS, OnSaveas)
	ON_BN_CLICKED(IDC_NEW, OnNew)
	ON_BN_CLICKED(IDC_CHECK, OnCheck)
	ON_COMMAND(ID_FILE_SAVE, OnSave)
	ON_COMMAND(ID_FILE_TBG, OnFileTbg)
	ON_COMMAND(ID_FILE_TP2, OnFileTp2)
	ON_COMMAND(ID_TOOLS_LOOKUPSTRREF, OnToolsLookupstrref)
	ON_COMMAND(ID_FILE_NEW, OnNew)
	ON_COMMAND(ID_FILE_LOAD, OnLoad)
	ON_COMMAND(ID_FILE_LOADEXTERNALSCRIPT, OnLoadex)
	ON_COMMAND(ID_FILE_SAVEAS, OnSaveas)
	ON_COMMAND(ID_CHECK, OnCheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CItemEdit message handlers
void CItemEdit::OnLoad() 
{
	int res;
	
  pickerdlg.m_restype=REF_ITM;
  pickerdlg.m_picked=itemname;
	res=pickerdlg.DoModal();
	if(res==IDOK)
	{
		res=read_item(pickerdlg.m_picked);
    switch(res)
    {
    case -3:
      MessageBox("Item loaded with errors.","Warning",MB_ICONEXCLAMATION|MB_OK);
  		itemname=pickerdlg.m_picked;
      break;
    case 1:
      MessageBox("Item size problems fixed.","Warning",MB_ICONEXCLAMATION|MB_OK);
  		itemname=pickerdlg.m_picked;
      break;
    case 2:
      MessageBox("Item header order fixed.","Warning",MB_ICONEXCLAMATION|MB_OK);
  		itemname=pickerdlg.m_picked;
      break;
    case 0:
  		itemname=pickerdlg.m_picked;
      break;
    default:
      MessageBox("Cannot read item!","Error",MB_ICONSTOP|MB_OK);
      NewItem();
      break;
    }
    SetWindowText("Edit item: "+itemname);
    m_pModelessPropSheet->RefreshDialog();
    UpdateData(UD_DISPLAY);
	}
}

static char BASED_CODE szFilter[] = "Item files (*.itm)|*.itm|All files (*.*)|*.*||";

void CItemEdit::OnLoadex() 
{
  int fhandle;
  int res;
  
  res=OFN_FILEMUSTEXIST|OFN_ENABLESIZING|OFN_EXPLORER;
  if(readonly) res|=OFN_READONLY;  
  CMyFileDialog m_getfiledlg(TRUE, "itm", makeitemname(".itm",0), res, szFilter);

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
    res=the_item.ReadItemFromFile(fhandle,-1);
    close(fhandle);
    lastopenedoverride=filepath.Left(filepath.ReverseFind('\\'));
    switch(res)
    {
    case 2:
      MessageBox("Item loaded with serious errors (no extensions).","Warning",MB_ICONEXCLAMATION|MB_OK);
      itemname=m_getfiledlg.GetFileTitle();
      itemname.MakeUpper();
      break;
    case 1:
      MessageBox("Item loaded with errors (harmless inconsistency).","Warning",MB_ICONEXCLAMATION|MB_OK);
      itemname=m_getfiledlg.GetFileTitle();
      itemname.MakeUpper();
      break;
    case 0:
      itemname=m_getfiledlg.GetFileTitle();
      itemname.MakeUpper();
      break;
    default:
      MessageBox("Cannot read item!","Error",MB_ICONSTOP|MB_OK);
      NewItem();
      break;
    }
    SetWindowText("Edit item: "+itemname);
    m_pModelessPropSheet->RefreshDialog();
    UpdateData(UD_DISPLAY);
  }
}

void CItemEdit::OnNew() 
{
  NewItem();
  SetWindowText("Edit item: "+itemname);
  m_pModelessPropSheet->RefreshDialog();
  UpdateData(UD_DISPLAY);
}

void CItemEdit::OnSave() 
{
  SaveItem(1);
}

void CItemEdit::OnSaveas() 
{
  SaveItem(0);
}

void CItemEdit::SaveItem(int save) 
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
  CMyFileDialog m_getfiledlg(FALSE, "itm", makeitemname(".itm",0), res, szFilter);

  if(save)
  {
    newname=itemname;
    filepath=makeitemname(".itm",0);
    goto gotname;
  }    

restart:  
  //if (filepath.GetLength()) strncpy(m_getfiledlg.m_ofn.lpstrFile,filepath, filepath.GetLength()+1);
  if( m_getfiledlg.DoModal() == IDOK )
  {
    filepath=m_getfiledlg.GetPathName();
    filepath.MakeLower();
    if(filepath.Right(4)!=".itm")
    {
      filepath+=".itm";
    }
    newname=m_getfiledlg.GetFileName();
    newname.MakeUpper();
    if(newname.Right(4)==".ITM") newname=newname.Left(newname.GetLength()-4);
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
    
    res = write_item(newname, filepath);
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
      MessageBox("Internal Error (feature block counter incorrect)!","Error",MB_ICONSTOP|MB_OK);
      break;
    default:
      MessageBox("Unhandled error!","Error",MB_ICONSTOP|MB_OK);
    }
  }
  SetWindowText("Edit item: "+itemname);
  m_pModelessPropSheet->RefreshDialog();
  UpdateData(UD_DISPLAY);
}

void CItemEdit::OnFileTbg() 
{
 ExportTBG(this, REF_ITM, 0);
}

void CItemEdit::OnFileTp2() 
{
 ExportTBG(this, REF_ITM, 1);
}

void CItemEdit::OnCheck() 
{
  int ret;

	ret=((CChitemDlg *) AfxGetMainWnd())->check_item();
  if(ret)
  {
    MessageBox(lasterrormsg,"Item editor",MB_ICONEXCLAMATION|MB_OK);
  }
}

BOOL CItemEdit::OnInitDialog() 
{
  CString tmpstr, tmpstr1, tmpstr2;

	CDialog::OnInitDialog();
  SetWindowText("Edit item: "+itemname);
	if (m_pModelessPropSheet == NULL)
	{
		m_pModelessPropSheet = new CItemPropertySheet(this);

    //WOW DS_CONTROL
		if (!m_pModelessPropSheet->Create(this, 
    DS_CONTROL | WS_CHILD | WS_VISIBLE ))
		{
			delete m_pModelessPropSheet;
			m_pModelessPropSheet = NULL;
			return TRUE;
		}
		m_pModelessPropSheet->SetWindowPos(0,0,0,0,0,SWP_NOSIZE);		
	}
	
	if (m_pModelessPropSheet != NULL && !m_pModelessPropSheet->IsWindowVisible())
		m_pModelessPropSheet->ShowWindow(SW_SHOW);
  //tooltips
  {
    m_tooltip.Create(this,TTS_NOPREFIX);
    m_tooltip.SetMaxTipWidth(200);
    m_tooltip.SetTipBkColor(RGB(240,224,160));
    
    m_tooltip.AddTool(GetDlgItem(IDCANCEL), IDS_CANCEL);
    tmpstr1.LoadString(IDS_LOAD);
    tmpstr2.LoadString(IDS_ITEM);
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
  }	
	return TRUE;
}

void CItemEdit::OnToolsLookupstrref() 
{
	CStrRefDlg dlg;
	
  dlg.DoModal();
  m_pModelessPropSheet->RefreshDialog();
  UpdateData(UD_DISPLAY);
}

void CItemEdit::PostNcDestroy() 
{
  if (m_pModelessPropSheet)
  {
    delete m_pModelessPropSheet;
    m_pModelessPropSheet=NULL;
  }	
	CDialog::PostNcDestroy();
}

void CItemEdit::OnCancel() 
{
  CString tmpstr;

  if(the_item.m_changed)
  {
    tmpstr.Format("Changes have been made to the item.\n"
      "Do you want to quit without save?\n");
    if(MessageBox(tmpstr,"Warning",MB_YESNO)==IDNO)
    {
      return;
    }
  }
	CDialog::OnCancel();
}

BOOL CItemEdit::PreTranslateMessage(MSG* pMsg)
{
  m_tooltip.RelayEvent(pMsg);
  return CDialog::PreTranslateMessage(pMsg);
}

