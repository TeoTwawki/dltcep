// StoreEdit.cpp : implementation file
//

#include "stdafx.h"

#include <fcntl.h>
#include <sys/stat.h>
#include "chitem.h"
#include "chitemDlg.h"
#include "StoreEdit.h"
#include "tbg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStoreEdit dialog

CStoreEdit::CStoreEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CStoreEdit::IDD, pParent)
{
	m_pModelessPropSheet = NULL;
	//{{AFX_DATA_INIT(CStoreEdit)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CStoreEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStoreEdit)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	OnProperties();
}

void CStoreEdit::NewStore()
{
	itemname="new store";
  the_store.KillCures();
  the_store.KillDrinks();
  the_store.KillHeaders();
  the_store.KillItemTypes();
  memset(&the_store.header,0,sizeof(store_header));
  if(has_xpvar()) //both iwd and iwd2
  {
    the_store.revision=90;
  }
  else
  {
    if(pst_compatible_var() )
    {
      the_store.revision=11;
    }
    else
    {
      the_store.revision=10;
    }
  }
  the_store.header.cureoffset=sizeof(store_header);
  the_store.header.drinkoffset=sizeof(store_header);
  the_store.header.offset=sizeof(store_header);
  the_store.header.pitemoffset=sizeof(store_header);
}

BEGIN_MESSAGE_MAP(CStoreEdit, CDialog)
	//{{AFX_MSG_MAP(CStoreEdit)
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_BN_CLICKED(IDC_LOADEX, OnLoadex)
	ON_BN_CLICKED(IDC_SAVEAS, OnSaveas)
	ON_BN_CLICKED(IDC_NEW, OnNew)
	ON_BN_CLICKED(IDC_CHECK, OnCheck)
	ON_COMMAND(ID_FILE_TBG, OnFileTbg)
	ON_COMMAND(ID_CHECK, OnCheck)
	ON_COMMAND(ID_FILE_NEW, OnNew)
	ON_COMMAND(ID_FILE_LOAD, OnLoad)
	ON_COMMAND(ID_FILE_LOADEXTERNALSCRIPT, OnLoadex)
	ON_COMMAND(ID_FILE_SAVEAS, OnSaveas)
	ON_COMMAND(ID_FILE_SAVE, OnSave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CStoreEdit::OnInitDialog() 
{
  CString tmpstr, tmpstr1, tmpstr2;

	CDialog::OnInitDialog();
	
  //tooltips
  {
    m_tooltip.Create(this,TTS_NOPREFIX);
    m_tooltip.SetMaxTipWidth(200);
    m_tooltip.SetTipBkColor(RGB(240,224,160));
    
    m_tooltip.AddTool(GetDlgItem(IDCANCEL), IDS_CANCEL);
    tmpstr1.LoadString(IDS_LOAD);
    tmpstr2.LoadString(IDS_STORE);
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

/////////////////////////////////////////////////////////////////////////////
// CStoreEdit message handlers

void CStoreEdit::OnLoad() 
{
	int res;
	
  pickerdlg.m_restype=REF_STO;
  pickerdlg.m_picked=itemname;
	res=pickerdlg.DoModal();
	if(res==IDOK)
	{
		res=read_store(pickerdlg.m_picked);
    switch(res)
    {
    case -3:
      MessageBox("Out of memory","Error",MB_ICONSTOP|MB_OK);
  		itemname=pickerdlg.m_picked;
      break;
    case 0:
  		itemname=pickerdlg.m_picked;
      break;
    default:
      MessageBox("Cannot read store!","Error",MB_ICONSTOP|MB_OK);
      NewStore();
      break;
    }
    SetWindowText("Edit store: "+itemname);
    m_pModelessPropSheet->RefreshDialog();
	}
}

static char BASED_CODE szFilter[] = "Store files (*.sto)|*.sto|All files (*.*)|*.*||";

void CStoreEdit::OnLoadex() 
{
  CString filepath;
  int fhandle;
  int res;
  
  res=OFN_FILEMUSTEXIST|OFN_ENABLESIZING|OFN_EXPLORER;
  if(readonly) res|=OFN_READONLY;  
  CFileDialog m_getfiledlg(TRUE, "sto", makeitemname(".sto",0), res, szFilter);

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
    res=the_store.ReadStoreFromFile(fhandle,-1);
    close(fhandle);
    lastopenedoverride=filepath.Left(filepath.ReverseFind('\\'));
    switch(res)
    {
    case -3:
      MessageBox("Store loaded with errors.","Warning",MB_ICONEXCLAMATION|MB_OK);
      itemname=m_getfiledlg.GetFileTitle();
      itemname.MakeUpper();
      break;
    case 0:
      itemname=m_getfiledlg.GetFileTitle();
      itemname.MakeUpper();
      break;
    default:
      MessageBox("Cannot read store!","Error",MB_ICONSTOP|MB_OK);
      NewStore();
      break;
    }
    SetWindowText("Edit store: "+itemname);
    m_pModelessPropSheet->RefreshDialog();
  }
}

void CStoreEdit::OnNew() 
{
	NewStore();
  SetWindowText("Edit store: "+itemname);
  m_pModelessPropSheet->RefreshDialog();
}

void CStoreEdit::OnSave() 
{
  SaveStore(1);
}

void CStoreEdit::OnSaveas() 
{
  SaveStore(0);
}

void CStoreEdit::SaveStore(int save) 
{
  CString filepath;
  CString newname;
  CString tmpstr;
  int res;

  if(readonly)
  {
    MessageBox("You opened it read only!","Warning",MB_ICONEXCLAMATION|MB_OK);
    return;
  }
  res=OFN_HIDEREADONLY|OFN_ENABLESIZING|OFN_EXPLORER;
  CFileDialog m_getfiledlg(FALSE, "sto", makeitemname(".sto",0), res, szFilter);

  if(save)
  {
    newname=itemname;
    filepath=makeitemname(".sto",0);
    goto gotname;
  }
restart:  
  if( m_getfiledlg.DoModal() == IDOK )
  {
    filepath=m_getfiledlg.GetPathName();
    filepath.MakeLower();
    if(filepath.Right(4)!=".sto")
    {
      filepath+=".sto";
    }
    newname=m_getfiledlg.GetFileName();
    newname.MakeUpper();
    if(newname.Right(4)==".STO") newname=newname.Left(newname.GetLength()-4);
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
    
    res = write_store(newname, filepath);
    lastopenedoverride=filepath.Left(filepath.ReverseFind('\\'));
    switch(res)
    {
    case 0:
      itemname=newname;
      break; //saved successfully
    case -2:
      MessageBox("Error while writing file!","Error",MB_ICONSTOP|MB_OK);
      break;
    default:
      MessageBox("Unhandled error!","Error",MB_ICONSTOP|MB_OK);
    }
  }
  SetWindowText("Edit store: "+itemname);
  m_pModelessPropSheet->RefreshDialog();
}

void CStoreEdit::OnFileTbg() 
{
 ExportTBG(this, REF_STO);
}

void CStoreEdit::OnCheck() 
{
  int ret;

	ret=((CChitemDlg *) AfxGetMainWnd())->check_store();
  if(ret)
  {
    MessageBox(lasterrormsg,"Store editor",MB_ICONEXCLAMATION|MB_OK);
  }
}

void CStoreEdit::OnProperties()
{	
  SetWindowText("Edit store: "+itemname);
	if (m_pModelessPropSheet == NULL)
	{
		m_pModelessPropSheet = new CStorePropertySheet(this);

    //WOW DS_CONTROL
		if (!m_pModelessPropSheet->Create(this, 
    DS_CONTROL | WS_CHILD | WS_VISIBLE ))
		{
			delete m_pModelessPropSheet;
			m_pModelessPropSheet = NULL;
			return;
		}
		m_pModelessPropSheet->SetWindowPos(0,0,0,0,0,SWP_NOSIZE);		
	}
	
	if (m_pModelessPropSheet != NULL && !m_pModelessPropSheet->IsWindowVisible())
		m_pModelessPropSheet->ShowWindow(SW_SHOW);
}

void CStoreEdit::PostNcDestroy() 
{
  if (m_pModelessPropSheet)
  {
    delete m_pModelessPropSheet;
    m_pModelessPropSheet=NULL;
  }	
	CDialog::PostNcDestroy();
}

BOOL CStoreEdit::PreTranslateMessage(MSG* pMsg) 
{
  m_tooltip.RelayEvent(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}
