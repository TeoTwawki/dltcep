// SpellEdit.cpp : implementation file
//

#include "stdafx.h"

#include <fcntl.h>
#include <sys/stat.h>
#include "chitem.h"
#include "chitemDlg.h"
#include "SpellEdit.h"
#include "StrRefDlg.h"
#include "tbg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSpellEdit dialog

CSpellEdit::CSpellEdit(CWnd* pParent /*=NULL*/)
: CDialog(CSpellEdit::IDD, pParent)
{
	m_pModelessPropSheet = NULL;
	//{{AFX_DATA_INIT(CSpellEdit)
	//}}AFX_DATA_INIT
}

void CSpellEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSpellEdit)
	//}}AFX_DATA_MAP
}

void CSpellEdit::NewSpell()
{
	itemname="new spell";
  the_spell.KillExtHeaders();
  the_spell.KillFeatHeaders();
  memset(&the_spell.header,0,sizeof(spell_header));
  the_spell.header.extheadoffs=sizeof(spell_header);  
  the_spell.header.featureoffs=sizeof(spell_header);
  the_spell.header.unknown0c=the_spell.header.unknown54=9999999;
  the_spell.header.unknown38=1;
}

BEGIN_MESSAGE_MAP(CSpellEdit, CDialog)
//{{AFX_MSG_MAP(CSpellEdit)
ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_BN_CLICKED(IDC_LOADEX, OnLoadex)
	ON_BN_CLICKED(IDC_SAVEAS, OnSaveas)
	ON_BN_CLICKED(IDC_NEW, OnNew)
	ON_BN_CLICKED(IDC_CHECK, OnCheck)
	ON_COMMAND(ID_FILE_SAVE, OnSave)
	ON_COMMAND(ID_FILE_TBG, OnFileTbg)
	ON_COMMAND(ID_ADDCFB, OnAddcfb)
	ON_COMMAND(ID_REMOVECFB, OnRemovecfb)
	ON_COMMAND(ID_SAVECFB, OnSavecfb)
	ON_COMMAND(ID_FILE_NEW, OnNew)
	ON_COMMAND(ID_FILE_LOAD, OnLoad)
	ON_COMMAND(ID_FILE_LOADEXTERNALSCRIPT, OnLoadex)
	ON_COMMAND(ID_FILE_SAVEAS, OnSaveas)
	ON_COMMAND(ID_CHECK, OnCheck)
	ON_COMMAND(ID_TOOLS_LOOKUPSTRREF, OnToolsLookupstrref)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpellEdit message handlers
void CSpellEdit::OnLoad() 
{
	int res;
	
  pickerdlg.m_restype=REF_SPL;
  pickerdlg.m_picked=itemname;
	res=pickerdlg.DoModal();
	if(res==IDOK)
	{
		res=read_spell(pickerdlg.m_picked);
    switch(res)
    {
    case -3:
      MessageBox("Spell loaded with errors.","Warning",MB_ICONEXCLAMATION|MB_OK);
  		itemname=pickerdlg.m_picked;
      break;
    case 0:
  		itemname=pickerdlg.m_picked;
      break;
    default:
      MessageBox("Cannot read spell!","Error",MB_ICONSTOP|MB_OK);
      NewSpell();
      break;
    }
    SetWindowText("Edit spell: "+itemname);
    m_pModelessPropSheet->RefreshDialog();
    UpdateData(UD_DISPLAY);
	}
}

static char BASED_CODE szFilter[] = "Spell files (*.spl)|*.spl|All files (*.*)|*.*||";

void CSpellEdit::OnLoadex() 
{
  CString filepath;
  int fhandle;
  int res;
  
  res=OFN_FILEMUSTEXIST|OFN_ENABLESIZING|OFN_EXPLORER;
  if(readonly) res|=OFN_READONLY;  
  CFileDialog m_getfiledlg(TRUE, "spl", makeitemname(".spl",0), res, szFilter);

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
    res=the_spell.ReadSpellFromFile(fhandle,-1);
    close(fhandle);
    switch(res)
    {
    case -4:
      MessageBox("Spell loaded with serious errors (no extensions).","Warning",MB_ICONEXCLAMATION|MB_OK);
      itemname=m_getfiledlg.GetFileTitle();
      itemname.MakeUpper();
      break;
    case -3:
      MessageBox("Spell loaded with errors (harmless inconsistency).","Warning",MB_ICONEXCLAMATION|MB_OK);
      itemname=m_getfiledlg.GetFileTitle();
      itemname.MakeUpper();
      break;
    case 0:
      itemname=m_getfiledlg.GetFileTitle();
      itemname.MakeUpper();
      break;
    default:
      MessageBox("Cannot read spell!","Error",MB_ICONSTOP|MB_OK);
      NewSpell();
      break;
    }
    SetWindowText("Edit spell: "+itemname);
    m_pModelessPropSheet->RefreshDialog();
    UpdateData(UD_DISPLAY);
  }
}

void CSpellEdit::OnNew() 
{
  NewSpell();
  SetWindowText("Edit spell: "+itemname);
  m_pModelessPropSheet->RefreshDialog();
  UpdateData(UD_DISPLAY);
}

void CSpellEdit::OnSave() 
{
  SaveSpell(1);
}

void CSpellEdit::OnSaveas() 
{
  SaveSpell(0);
}

void CSpellEdit::SaveSpell(int save)
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

  CFileDialog m_getfiledlg(FALSE, "spl",makeitemname(".spl",0), res, szFilter);

  if(save)
  {
    newname=itemname;
    filepath=makeitemname(".spl",0);
    goto gotname;
  }    

restart:  
  if( m_getfiledlg.DoModal() == IDOK )
  {
    filepath=m_getfiledlg.GetPathName();
    filepath.MakeLower();
    if(filepath.Right(4)!=".spl")
    {
      filepath+=".spl";
    }
    newname=m_getfiledlg.GetFileName();
    newname.MakeUpper();
    if(newname.Right(4)==".SPL") newname=newname.Left(newname.GetLength()-4);
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
    /*
    fhandle=open(filepath, O_BINARY|O_RDWR|O_CREAT|O_TRUNC,S_IREAD|S_IWRITE);
    if(fhandle<1)
    {
      MessageBox("Can't write file!","Error",MB_ICONSTOP|MB_OK);
      goto restart;
    }
    res=the_spell.WriteSpellToFile(fhandle,0);
    close(fhandle);
    */
    res = write_spell(newname);
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
  SetWindowText("Edit spell: "+itemname);
  m_pModelessPropSheet->RefreshDialog();
  UpdateData(UD_DISPLAY);
}

void CSpellEdit::OnFileTbg() 
{
 ExportTBG(this, REF_SPL);
}

void CSpellEdit::OnCheck() 
{
  int ret;

	ret=((CChitemDlg *) AfxGetMainWnd())->check_spell();
  if(ret)
  {
    MessageBox(lasterrormsg,"Spell editor",MB_ICONEXCLAMATION|MB_OK);
  }
}

void CSpellEdit::OnAddcfb() 
{
  int featcount, featnum;
  feat_block *cfb;
  CString cfbfile;
  int flg;
  int fhandle;
  
  flg=OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|OFN_ENABLESIZING|OFN_EXPLORER;
  CFileDialog m_getfiledlg(TRUE, "bin", bgfolder+"cfb.bin", flg, cfbFilter);
  
restart:
  if( m_getfiledlg.DoModal() == IDOK )
  {
    cfbfile=m_getfiledlg.GetPathName();
    fhandle=open(cfbfile,O_RDONLY|O_BINARY);
    featcount=filelength(fhandle);
    if(featcount%sizeof(feat_block) || !featcount)
    {
      close(fhandle);
      MessageBox("This is not a valid cfb file. (It must contain feature blocks)","Warning",MB_ICONEXCLAMATION|MB_OK);
      goto restart;
    }
    cfb = new feat_block[featnum=featcount/sizeof(feat_block)];
    if(!cfb)
    {
      close(fhandle);
      MessageBox("Not enough memory.","Error",MB_ICONSTOP|MB_OK);
      return;
    }
    read(fhandle,cfb,featcount);
    close(fhandle);
    ModifyCFB(1,featnum,cfb);
    m_pModelessPropSheet->RefreshDialog();
    UpdateData(UD_DISPLAY);
  }
}

void CSpellEdit::OnRemovecfb() 
{
	ModifyCFB(0,0,0);	 //remove cfb
  m_pModelessPropSheet->RefreshDialog();
  UpdateData(UD_DISPLAY);
}

void CSpellEdit::OnSavecfb() 
{
  int esize, i;
  feat_block *cfb;
  CString cfbfile;
  int flg;
  int fhandle;
  
  flg=OFN_HIDEREADONLY|OFN_ENABLESIZING|OFN_EXPLORER;
  CFileDialog m_getfiledlg(FALSE, "bin", bgfolder+"cfb.bin", flg, cfbFilter);
  
  if( m_getfiledlg.DoModal() == IDOK )
  {
    cfbfile=m_getfiledlg.GetPathName();
    fhandle=open(cfbfile,O_BINARY|O_RDWR|O_CREAT|O_TRUNC,S_IREAD|S_IWRITE);
    cfb = new feat_block[the_spell.header.featblkcount];
    if(!cfb)
    {
      close(fhandle);
      MessageBox("Not enough memory","Error",MB_ICONSTOP|MB_OK);
      return;
    }
    esize=the_spell.header.featblkcount*sizeof(feat_block);
    memcpy(cfb,the_spell.featblocks,esize);
    for(i=0;i<the_spell.header.featblkcount;i++)
    {
      RetrieveResref(cfbfile,cfb[i].vvc);
      if(cfbfile==itemname) StoreResref(SELF_REFERENCE,cfb[i].vvc);
      if(cfb[i].par1.parl==the_spell.header.level)
      {
        memcpy(&cfb[i].par1.parb,"++++",4);
      }
      if(cfb[i].par1.parl==-the_spell.header.level)
      {
        memcpy(&cfb[i].par1.parb,"----",4);
      }      
    }
    if(write(fhandle,cfb,esize)!=esize)
    {
      MessageBox("Can't write file","Error",MB_ICONSTOP|MB_OK);
    }
    close(fhandle);
    delete [] cfb;
  }
}

BOOL CSpellEdit::OnInitDialog() 
{
  CString tmpstr, tmpstr1, tmpstr2;

	CDialog::OnInitDialog();
  SetWindowText("Edit spell: "+itemname);
	if (m_pModelessPropSheet == NULL)
	{
		m_pModelessPropSheet = new CSpellPropertySheet(this);

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
    tmpstr2.LoadString(IDS_SPELL);
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

void CSpellEdit::OnToolsLookupstrref() 
{
	CStrRefDlg dlg;
	
  dlg.DoModal();
  m_pModelessPropSheet->RefreshDialog();
  UpdateData(UD_DISPLAY);
}

void CSpellEdit::PostNcDestroy() 
{
  if (m_pModelessPropSheet)
  {
    delete m_pModelessPropSheet;
    m_pModelessPropSheet=NULL;
  }	
	CDialog::PostNcDestroy();
}

BOOL CSpellEdit::PreTranslateMessage(MSG* pMsg) 
{
  m_tooltip.RelayEvent(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}
