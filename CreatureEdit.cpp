// CreatureEdit.cpp : implementation file
//

#include "stdafx.h"

#include <fcntl.h>
#include <sys/stat.h>
#include "chitem.h"
#include "chitemDlg.h"
#include "CreatureEdit.h"
#include "2da.h"
#include "2daEdit.h"
#include "tbg.h"
#include "MyFileDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCreatureEdit dialog

CCreatureEdit::CCreatureEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CCreatureEdit::IDD, pParent)
{
	m_pModelessPropSheet = NULL;
	//{{AFX_DATA_INIT(CCreatureEdit)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
  the_ids.new_ids();
  m_idsname="";
}

void CCreatureEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCreatureEdit)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

static unsigned char defaultcolors[7]={30,37,57,12,23,28,0};

void CCreatureEdit::NewCreature()
{
	itemname="new creature";
  the_creature.KillBooks();
  the_creature.KillEffects();
  the_creature.KillItems();
  the_creature.KillMemos();
  the_creature.KillSelects();
  memset(&the_creature.header,0,sizeof(creature_header));
  if(iwd2_structures()) //iwd2
  {
    the_creature.revision=22;
    the_creature.slotcount = IWD2_SLOT_COUNT;
  }
  else if(has_xpvar())  //iwd
  {
    the_creature.revision=90;
    the_creature.slotcount = SLOT_COUNT;
  }
  else if(pst_compatible_var()) //pst
  {
    the_creature.revision=12;
    the_creature.slotcount = PST_SLOT_COUNT; 
  }
  else //bg1, bg2, tob
  {
    the_creature.revision=10;
    the_creature.slotcount = SLOT_COUNT;
  }

  the_creature.header.effbyte=1;
  //-1 is -1 always
  memset(the_creature.header.strrefs,-1,sizeof(the_creature.header.strrefs) );
  the_creature.header.sex=1;
  the_creature.header.globalID=-1;
  the_creature.header.localID=-1;
  memset(the_creature.itemslots,-1,sizeof(the_creature.itemslots) );
  *(int *) (the_creature.itemslots+the_creature.slotcount)=1000;
  memcpy(the_creature.header.colours,defaultcolors,7);
  the_creature.m_changed=false;
}

BEGIN_MESSAGE_MAP(CCreatureEdit, CDialog)
	//{{AFX_MSG_MAP(CCreatureEdit)
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_BN_CLICKED(IDC_LOADEX, OnLoadex)
	ON_BN_CLICKED(IDC_SAVEAS, OnSaveas)
	ON_BN_CLICKED(IDC_NEW, OnNew)
	ON_BN_CLICKED(IDC_CHECK, OnCheck)
	ON_COMMAND(ID_FILE_SAVE, OnSave)
	ON_COMMAND(ID_FILE_TBG, OnFileTbg)
	ON_COMMAND(ID_FILE_TP2, OnFileTp2)
	ON_COMMAND(ID_EXPORTSOUNDSET, OnExportsoundset)
	ON_COMMAND(ID_IMPORTSOUNDSET, OnImportsoundset)
	ON_COMMAND(ID_TOOLS_IDSBROWSER, OnToolsIdsbrowser)
	ON_COMMAND(ID_FILE_NEW, OnNew)
	ON_COMMAND(ID_FILE_LOAD, OnLoad)
	ON_COMMAND(ID_FILE_LOADEXTERNALSCRIPT, OnLoadex)
	ON_COMMAND(ID_FILE_SAVEAS, OnSaveas)
	ON_COMMAND(ID_CHECK, OnCheck)
	ON_COMMAND(ID_FILE_LOADCHAR, OnFileLoadchar)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCreatureEdit message handlers



void CCreatureEdit::OnFileLoadchar() 
{
	int res;
	
  pickerdlg.m_restype=REF_CHR;
  pickerdlg.m_picked=itemname;
	res=pickerdlg.DoModal();
	if(res==IDOK)
	{
		res=read_character(pickerdlg.m_picked, &the_creature);
    m_pModelessPropSheet->RefreshDialog();
    switch(res)
    {
    case -3:
      MessageBox("Not enough memory.","Warning",MB_ICONEXCLAMATION|MB_OK);
  		itemname=pickerdlg.m_picked;
      break;
    case 1: case 2:
      MessageBox("Character will be reordered (harmless inconsistency).","Warning",MB_ICONEXCLAMATION|MB_OK);
  		itemname=pickerdlg.m_picked;
      break;
    case 0:
  		itemname=pickerdlg.m_picked;
      break;
    default:
      MessageBox("Cannot read character!","Error",MB_ICONSTOP|MB_OK);
      NewCreature();
      break;
    }
    SetWindowText("Edit character: "+itemname);
    UpdateData(UD_DISPLAY);
	}
}

void CCreatureEdit::OnLoad() 
{
	int res;
	
  pickerdlg.m_restype=REF_CRE;
  pickerdlg.m_picked=itemname;
	res=pickerdlg.DoModal();
	if(res==IDOK)
	{
		res=read_creature(pickerdlg.m_picked, &the_creature);
    m_pModelessPropSheet->RefreshDialog();
    switch(res)
    {
    case -3:
      MessageBox("Not enough memory.","Warning",MB_ICONEXCLAMATION|MB_OK);
  		itemname=pickerdlg.m_picked;
      break;
    case 1: case 2:
      MessageBox("Creature will be reordered (harmless inconsistency).","Warning",MB_ICONEXCLAMATION|MB_OK);
  		itemname=pickerdlg.m_picked;
      break;
    case 0:
  		itemname=pickerdlg.m_picked;
      break;
    default:
      MessageBox("Cannot read creature!","Error",MB_ICONSTOP|MB_OK);
      NewCreature();
      break;
    }
    SetWindowText("Edit creature: "+itemname);
    UpdateData(UD_DISPLAY);
	}
}

static char BASED_CODE szFilter[] = "Creature files (*.cre)|*.cre|"
                                    "Character files (*.chr)|*.chr|All files (*.*)|*.*||";

void CCreatureEdit::OnLoadex() 
{
  int fhandle;
  int res;
  
  res=OFN_FILEMUSTEXIST|OFN_ENABLESIZING|OFN_EXPLORER;
  if(readonly) res|=OFN_READONLY;  
  CMyFileDialog m_getfiledlg(TRUE, "cre", makeitemname(".cre",0), res, szFilter);

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
    res=the_creature.ReadCreatureFromFile(fhandle,-1);
    close(fhandle);
    lastopenedoverride=filepath.Left(filepath.ReverseFind('\\'));
    switch(res)
    {
    case -3:
      MessageBox("Not enough memory.","Warning",MB_ICONEXCLAMATION|MB_OK);
      itemname=m_getfiledlg.GetFileTitle();
      itemname.MakeUpper();
      break;
    case 1: case 2:
      MessageBox("Creature will be reordered (harmless inconsistency).","Warning",MB_ICONEXCLAMATION|MB_OK);
      itemname=m_getfiledlg.GetFileTitle();
      itemname.MakeUpper();
      break;
    case 0:
      itemname=m_getfiledlg.GetFileTitle();
      itemname.MakeUpper();
      break;
    default:
      MessageBox("Cannot read creature!","Error",MB_ICONSTOP|MB_OK);
      NewCreature();
      break;
    }
    SetWindowText("Edit creature: "+itemname);
    m_pModelessPropSheet->RefreshDialog();
    UpdateData(UD_DISPLAY);
  }
}

void CCreatureEdit::OnFileTbg() 
{
 ExportTBG(this, REF_CRE, 0);
}

void CCreatureEdit::OnFileTp2() 
{
 ExportTBG(this, REF_CRE, 1);
}

void CCreatureEdit::OnCheck() 
{
  int ret;

	ret=((CChitemDlg *) AfxGetMainWnd())->check_creature();
  if(ret)
  {
    MessageBox(lasterrormsg,"Creature editor",MB_ICONEXCLAMATION|MB_OK);
  }
}

void CCreatureEdit::OnNew() 
{
  NewCreature();
  SetWindowText("Edit creature: "+itemname);
  m_pModelessPropSheet->RefreshDialog();
  UpdateData(UD_DISPLAY);
}

void CCreatureEdit::OnSave() 
{
  SaveCreature(1);
}

void CCreatureEdit::OnSaveas() 
{
  SaveCreature(0);
}

void CCreatureEdit::SaveCreature(int save) 
{
  CString newname;
  CString tmpstr;
  int chrorcre;
  int res;

  if(readonly)
  {
    MessageBox("You opened it read only!","Warning",MB_ICONEXCLAMATION|MB_OK);
    return;
  }
  res=OFN_HIDEREADONLY|OFN_ENABLESIZING|OFN_EXPLORER;
  CMyFileDialog m_getfiledlg(FALSE, "cre", makeitemname(".cre",0), res, szFilter);

  if(save)
  {
    newname=itemname;
    filepath=makeitemname(".cre",0);
    chrorcre=0;
    goto gotname;
  }    
restart:
  //if (filepath.GetLength()) strncpy(m_getfiledlg.m_ofn.lpstrFile,filepath, filepath.GetLength()+1);
  chrorcre=0;
  if( m_getfiledlg.DoModal() == IDOK )
  {
    filepath=m_getfiledlg.GetPathName();
    filepath.MakeLower();
    if(filepath.Right(4)!=".cre")
    {
      if(filepath.Right(4)==".chr")
      {
        chrorcre=2;
      }
      else
      {
        filepath+=".cre";
      }
    }
    newname=m_getfiledlg.GetFileName();
    newname.MakeUpper();
    if(newname.Right(4)==".CRE") newname=newname.Left(newname.GetLength()-4);
    else if(newname.Right(4)==".CHR") newname=newname.Left(newname.GetLength()-4);
gotname:
    if(newname.GetLength()>8 || newname.GetLength()<1 || newname.FindOneOf(" *")!=-1)
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
    
    if (chrorcre==2) {
      res = write_character(newname, filepath);
    } else {
      res = write_creature(newname, filepath);
    }
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
  SetWindowText("Edit creature: "+itemname);
  m_pModelessPropSheet->RefreshDialog();
  UpdateData(UD_DISPLAY);
}

void CCreatureEdit::OnExportsoundset() 
{
  ExportTBG(this, REF_CRE|TBG_ALT, 0);
}

void CCreatureEdit::OnImportsoundset() 
{
  CString tmp;
  
  tmp=itemname;
  ((CChitemDlg *) AfxGetMainWnd())->Importtbg(TBG_ALT);
  itemname=tmp;
}

BOOL CCreatureEdit::OnInitDialog() 
{
  CString tmpstr, tmpstr1, tmpstr2;

	CDialog::OnInitDialog();
  SetWindowText("Edit creature: "+itemname);

	if (m_pModelessPropSheet == NULL)
	{
		m_pModelessPropSheet = new CCreaturePropertySheet(this);

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
    tmpstr2.LoadString(IDS_CREATURE);
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

void CCreatureEdit::PostNcDestroy() 
{
  if (m_pModelessPropSheet)
  {
    delete m_pModelessPropSheet;
    m_pModelessPropSheet=NULL;
  }	
	CDialog::PostNcDestroy();
}

void CCreatureEdit::OnToolsIdsbrowser() 
{
	CIDSEdit dlg;
  CString tmpstr;

  dlg.SetReadOnly(true);
  tmpstr=itemname;
  itemname=m_idsname;
  dlg.DoModal();
  m_idsname=itemname;
  itemname=tmpstr;
}

void CCreatureEdit::OnCancel() 
{
  CString tmpstr;

	if(the_creature.m_changed && the_creature.m_savechanges)
  {
    tmpstr.Format("Changes have been made to the file (%s).\n"
      "Do you want to quit without save?\n",itemname);
    if(MessageBox(tmpstr,"Warning",MB_YESNO)==IDNO)
    {
      return;
    }
  }
	CDialog::OnCancel();
}

BOOL CCreatureEdit::PreTranslateMessage(MSG* pMsg) 
{
  m_tooltip.RelayEvent(pMsg);	
	return CDialog::PreTranslateMessage(pMsg);
}
