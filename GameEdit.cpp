// GameEdit.cpp : implementation file
//

#include "stdafx.h"

#include <fcntl.h>
#include <sys/stat.h>
#include "chitem.h"
#include "chitemDlg.h"
#include "GameEdit.h"
#include "CreatureEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGameEdit dialog

CGameEdit::CGameEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CGameEdit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGameEdit)
	m_original = _T("");
	m_revision = _T("");
	//}}AFX_DATA_INIT
}

static int npcboxids[]={IDC_NPCPICKER, IDC_AREA, IDC_BROWSE1, IDC_XPOS, IDC_YPOS,
IDC_FACE, IDC_CRERES, IDC_BROWSE2, IDC_DELNPC, IDC_JOINABLE,0};

static int varboxids[]={IDC_VARIABLEPICKER, IDC_VARNAME, IDC_VALUE,
IDC_REMOVEVAR, IDC_VARIABLES, 0};

static int dvarboxids[]={IDC_VARIABLEPICKER2, IDC_VARNAME2, IDC_VALUE2,
IDC_REMOVEVAR2, IDC_DVAR, 0, IDC_ADDVAR2, IDC_SCREEN}; //this is a hack for the last element

static journalids[]={IDC_JOURNALPICKER, IDC_STRREF, IDC_CHAPTER, 
IDC_SECTION,IDC_TIME, IDC_JOURNAL, IDC_DELJOURNAL,0};

static familiarids[]={IDC_FAMILIARPICKER, IDC_FAMILIAR,0 };

static pcboxids[]={IDC_PARTY, IDC_CREATURENUM, IDC_EDIT, 0};

void CGameEdit::DoDataExchange(CDataExchange* pDX)
{
  CString tmpstr;
  int pos;
  int i;
  int flg;

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGameEdit)
	DDX_Control(pDX, IDC_CREATURENUM, m_creaturenum_control);
	DDX_Control(pDX, IDC_FACE, m_facepicker);
	DDX_Control(pDX, IDC_FAMILIARPICKER, m_familiarpicker);
	DDX_Control(pDX, IDC_JOURNALPICKER, m_journalpicker);
	DDX_Control(pDX, IDC_VARIABLEPICKER2, m_dvarpicker);
	DDX_Control(pDX, IDC_VARIABLEPICKER, m_variablepicker);
	DDX_Control(pDX, IDC_NPCPICKER, m_npcpicker);
	DDX_Text(pDX, IDC_MODIFIED, m_original);
	DDX_Text(pDX, IDC_REVISION, m_revision);
	//}}AFX_DATA_MAP
  DDX_Text(pDX, IDC_PARTYSIZE, the_game.header.pccount);

  DDX_Text(pDX, IDC_GAMETIME, the_game.header.gametime);
  DDX_Text(pDX, IDC_GOLD, the_game.header.gold);

  RetrieveResref(tmpstr,the_game.header.mainarea);
  DDX_Text(pDX, IDC_STARTAREA, tmpstr);
  StoreResref(tmpstr,the_game.header.mainarea);

  if(the_game.header.npccount)
  {
    for(i=0;npcboxids[i];i++) GetDlgItem(npcboxids[i])->EnableWindow(true);
    pos=m_npcpicker.GetCurSel();
    if(pos>=0 && pos<the_game.header.npccount)
    {
      RetrieveResref(tmpstr, the_game.npcs[pos].curarea);
      DDX_Text(pDX, IDC_AREA, tmpstr);
      DDV_MaxChars(pDX, tmpstr, 8);
      StoreResref(tmpstr, the_game.npcs[pos].curarea);
      
      RetrieveResref(tmpstr, the_game.npcs[pos].creresref);
      DDX_Text(pDX, IDC_CRERES, tmpstr);
      DDV_MaxChars(pDX, tmpstr, 8);
      StoreResref(tmpstr, the_game.npcs[pos].creresref);
      
      DDX_Text(pDX, IDC_XPOS, the_game.npcs[pos].xpos);
      DDX_Text(pDX, IDC_YPOS, the_game.npcs[pos].ypos);
      
      tmpstr=format_direction(the_game.npcs[pos].direction);
      DDX_Text(pDX, IDC_FACE,tmpstr);
      the_game.npcs[pos].direction=strtonum(tmpstr);
      
      if(the_game.npcs[pos].creoffset || the_game.npcs[pos].creresref[0]=='*')
      {
        m_original="Modified";
      }
      else m_original="Original";
    }
    else m_original="";
  }
  else
  {
    for(i=0;npcboxids[i];i++) GetDlgItem(npcboxids[i])->EnableWindow(false);
    m_original="";
  }
  
  for(i=0;pcboxids[i];i++)
  {
    GetDlgItem(pcboxids[i])->EnableWindow(!!the_game.header.pccount);
  }

  if(the_game.header.variablecount)
  {
    for(i=0;varboxids[i];i++)
    {
      GetDlgItem(varboxids[i])->EnableWindow();
    }
    pos=GetActualPosition(m_variablepicker);
    if(pos>=0 && pos<the_game.header.variablecount)
    {
      RetrieveVariable(tmpstr, the_game.variables[pos].variablename);
      DDX_Text(pDX, IDC_VARNAME, tmpstr);
      DDV_MaxChars(pDX, tmpstr, 32);
      StoreVariable(tmpstr, the_game.variables[pos].variablename);
      DDX_Text(pDX, IDC_VALUE, the_game.variables[pos].value);
    }
  }
  else
  {
    for(i=0;varboxids[i];i++)
    {
      GetDlgItem(varboxids[i])->EnableWindow(false);
    }
  }

  if(the_game.revision==12)
  {
    flg=!!the_game.header.dvarcount;
    for(i=0;dvarboxids[i];i++)
    {
      GetDlgItem(dvarboxids[i])->EnableWindow(flg);
    }
    //this element is always on when revision=12
    GetDlgItem(dvarboxids[++i])->EnableWindow(true); 
    //this element is always false on revision=12
    GetDlgItem(dvarboxids[++i])->EnableWindow(false); 
    pos=GetActualPosition(m_dvarpicker);
    if(pos>=0 && pos<the_game.header.dvarcount)
    {
      RetrieveVariable(tmpstr, the_game.deathvariables[pos].variablename);
      DDX_Text(pDX, IDC_VARNAME2, tmpstr);
      DDV_MaxChars(pDX, tmpstr, 32);
      StoreVariable(tmpstr, the_game.deathvariables[pos].variablename);
      DDX_Text(pDX, IDC_VALUE2, the_game.deathvariables[pos].value);
    }
  }
  else
  {
    for(i=0;dvarboxids[i];i++)
    {
      GetDlgItem(dvarboxids[i])->EnableWindow(false);
    }
    GetDlgItem(dvarboxids[++i])->EnableWindow(false);
    GetDlgItem(dvarboxids[++i])->EnableWindow(true);
    DDX_Text(pDX, IDC_SCREEN, the_game.header.dvaroffset);
  }

  if(the_game.header.journalcount)
  {
    for(i=0;journalids[i];i++)
    {
      GetDlgItem(journalids[i])->EnableWindow(true);
    }
    pos=m_journalpicker.GetCurSel();
    if(pos>=0 && pos<the_game.header.journalcount)
    {
      DDX_Text(pDX, IDC_STRREF, the_game.journals[pos].strref);
      DDX_Text(pDX, IDC_CHAPTER, the_game.journals[pos].chapter);
      DDX_Text(pDX, IDC_SECTION, the_game.journals[pos].section);
      DDX_Text(pDX, IDC_TIME, the_game.journals[pos].time);

      if(pDX->m_bSaveAndValidate==UD_DISPLAY)
      {
        tmpstr=resolve_tlk_text(the_game.journals[pos].strref);
        DDX_Text(pDX, IDC_TEXT, tmpstr);
      }
    }
  }
  else
  {
    for(i=0;journalids[i];i++)
    {
      GetDlgItem(journalids[i])->EnableWindow(false);
    }
  }

  if(the_game.revision==20 || the_game.revision==11)
  {
    for(i=0;familiarids[i];i++)
    {
      GetDlgItem(familiarids[i])->EnableWindow(!!the_game.header.familiaroffset);
    }
    
    pos=m_familiarpicker.GetCurSel();
    if(pos>=0 && pos<9)
    {
      RetrieveResref(tmpstr,the_game.familiar.familiars[pos]);
      DDX_Text(pDX, IDC_FAMILIAR, tmpstr);
      DDV_MaxChars(pDX, tmpstr, 8);
      StoreResref(tmpstr,the_game.familiar.familiars[pos]);
    }
  }
  else
  {
    for(i=0;familiarids[i];i++)
    {
      GetDlgItem(familiarids[i])->EnableWindow(false);
    }
  }
}

CString ResolveName(char *customname, creature_header *creatureheader)
{
  CString ret;

  ret=CString(customname);
  if(ret.IsEmpty()) ret=resolve_tlk_text(creatureheader->longname);
  return ret;
}

void CGameEdit::RefreshDialog()
{
  CString tmpstr;
  int pos;
  int i;

  SetWindowText("Edit Game: "+itemname);
  m_revision.Format("Game revision: %2.1d",the_game.revision);

  pos=m_npcpicker.GetCurSel();
  if(pos<0) pos=0;
  m_npcpicker.ResetContent();
  for(i=0;i<the_game.header.npccount;i++)
  {
    tmpstr.Format("%d. %-.8s (%-.8s)", i+1, the_game.npcs[i].creresref, the_game.npcs[i].curarea);
    m_npcpicker.SetItemData(m_npcpicker.AddString(tmpstr),i);
  }
  m_npcpicker.SetCurSel(pos);

  pos=m_creaturenum_control.GetCurSel();
  if(pos<0) pos=0;
  m_creaturenum_control.ResetContent();
  for(i=0;i<the_game.header.pccount;i++)
  {    
    tmpstr.Format("%d. %s",i+1,ResolveName((char *) (the_game.pcextensions+i),(creature_header *) the_game.pcstructs[i] ) );
    m_creaturenum_control.SetItemData(m_creaturenum_control.AddString(tmpstr),i);
  }
  m_creaturenum_control.SetCurSel(pos);

  pos=m_variablepicker.GetCurSel();
  if(pos<0) pos=0;
  m_variablepicker.ResetContent();
  for(i=0;i<the_game.header.variablecount;i++)
  {
    tmpstr.Format("%-.32s (%d)",the_game.variables[i].variablename, the_game.variables[i].value);
    m_variablepicker.SetItemData(m_variablepicker.AddString(tmpstr),i);
  }
  m_variablepicker.SetCurSel(pos);

  if(the_game.revision==12)
  {
    pos=m_dvarpicker.GetCurSel();
    if(pos<0) pos=0;
    m_dvarpicker.ResetContent();
    for(i=0;i<the_game.header.dvarcount;i++)
    {
      tmpstr.Format("%-.32s (%d)",the_game.deathvariables[i].variablename, the_game.deathvariables[i].value);
      m_dvarpicker.SetItemData(m_dvarpicker.AddString(tmpstr),i);
    }
    m_dvarpicker.SetCurSel(pos);
  }

  pos=m_journalpicker.GetCurSel();
  if(pos<0) pos=0;
  m_journalpicker.ResetContent();
  for(i=0;i<the_game.header.journalcount;i++)
  {
    tmpstr.Format("%d. (%d)",i+1, the_game.journals[i].strref);
    m_journalpicker.SetItemData(m_journalpicker.AddString(tmpstr),i);
  }
  m_journalpicker.SetCurSel(pos);

  if(the_game.revision==20)
  {
    pos=m_familiarpicker.GetCurSel();
    if(pos<0) pos=0;
    m_familiarpicker.ResetContent();
    for(i=0;i<9;i++)
    {
      tmpstr.Format("%d. %-.8s",i+1, the_game.familiar.familiars[i]);
      m_familiarpicker.SetItemData(m_familiarpicker.AddString(tmpstr),i);
    }
    m_familiarpicker.SetCurSel(pos);
  }
  UpdateData(UD_DISPLAY);
}

BOOL CGameEdit::OnInitDialog() 
{
  CString tmpstr, tmpstr1, tmpstr2;
  int i;

	CDialog::OnInitDialog();
	RefreshDialog();
  for(i=0;i<NUM_FVALUES;i++)
  {
    m_facepicker.AddString(format_direction(i));
  }
  //tooltips
  {
    m_tooltip.Create(this,TTS_NOPREFIX);
    m_tooltip.SetMaxTipWidth(200);
    m_tooltip.SetTipBkColor(RGB(240,224,160));
    
    m_tooltip.AddTool(GetDlgItem(IDCANCEL), IDS_CANCEL);
    tmpstr1.LoadString(IDS_LOAD);
    tmpstr2.LoadString(IDS_GAME);
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

void CGameEdit::NewGame()
{
	itemname="new game";
  the_game.KillVariables();
  the_game.KillPCs();
  the_game.KillNPCs();
  the_game.KillPCStructs();
  the_game.KillNPCStructs();
  the_game.KillPCExtensions();
  the_game.KillNPCExtensions();
  the_game.KillUnknowns1();
  the_game.KillUnknowns2();
  the_game.KillJournals();
  the_game.KillDeathVariables();

  memset(&the_game.header,0,sizeof(gam_header) );
  memset(&the_game.pstheader,0,sizeof(gam_pst_header) );
  the_game.header.unknown1count=100;
  the_game.unknowns1=new gam_unknown1[the_game.header.unknown1count];
  if(!the_game.unknowns1) the_game.header.unknown1count=0;
  memset(the_game.unknowns1,0,the_game.header.unknown1count*sizeof(gam_unknown1) );
  the_game.header.unknown1offset=sizeof(gam_header);
  memset(&the_game.familiar,0,sizeof(gam_familiar) );
  memset(the_game.familiardata,0,sizeof(the_game.familiardata));
  if(iwd2_structures()) the_game.revision=22;
  else
  {
    if(pst_compatible_var())
    {
      the_game.revision=12;
    }
    else
    {
      if(bg1_compatible_area())
      {
        the_game.revision=11;
      }
      else
      {
        the_game.header.familiaroffset=1;
        the_game.familiar.offset=1; //set it so we will save it if needed
        the_game.revision=20;
      }
    }
  }
}

BEGIN_MESSAGE_MAP(CGameEdit, CDialog)
	//{{AFX_MSG_MAP(CGameEdit)
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_BN_CLICKED(IDC_LOADEX, OnLoadex)
	ON_BN_CLICKED(IDC_SAVEAS, OnSaveas)
	ON_BN_CLICKED(IDC_NEW, OnNew)
	ON_CBN_KILLFOCUS(IDC_NPCPICKER, OnKillfocusNpcpicker)
	ON_CBN_SELCHANGE(IDC_NPCPICKER, OnSelchangeNpcpicker)
	ON_EN_KILLFOCUS(IDC_AREA, OnKillfocusArea)
	ON_EN_KILLFOCUS(IDC_CRERES, OnKillfocusCreres)
	ON_EN_KILLFOCUS(IDC_XPOS, OnKillfocusXpos)
	ON_EN_KILLFOCUS(IDC_YPOS, OnKillfocusYpos)
	ON_BN_CLICKED(IDC_BROWSE1, OnBrowse1)
	ON_BN_CLICKED(IDC_BROWSE2, OnBrowse2)
	ON_BN_CLICKED(IDC_ADDNPC, OnAddnpc)
	ON_BN_CLICKED(IDC_DELNPC, OnDelnpc)
	ON_CBN_KILLFOCUS(IDC_VARIABLEPICKER, OnKillfocusVariablepicker)
	ON_CBN_SELCHANGE(IDC_VARIABLEPICKER, OnSelchangeVariablepicker)
	ON_EN_KILLFOCUS(IDC_VALUE, OnKillfocusValue)
	ON_BN_CLICKED(IDC_REMOVEVAR, OnDelvar)
	ON_BN_CLICKED(IDC_ADDVAR, OnAddvar)
	ON_CBN_KILLFOCUS(IDC_VARIABLEPICKER2, OnKillfocusVariablepicker2)
	ON_CBN_SELCHANGE(IDC_VARIABLEPICKER2, OnSelchangeVariablepicker2)
	ON_EN_KILLFOCUS(IDC_VALUE2, OnKillfocusValue2)
	ON_BN_CLICKED(IDC_REMOVEVAR2, OnDelvar2)
	ON_BN_CLICKED(IDC_ADDVAR2, OnAddvar2)
	ON_CBN_KILLFOCUS(IDC_FAMILIARPICKER, OnKillfocusFamiliarpicker)
	ON_CBN_SELCHANGE(IDC_FAMILIARPICKER, OnSelchangeFamiliarpicker)
	ON_EN_KILLFOCUS(IDC_FAMILIAR, OnKillfocusFamiliar)
	ON_CBN_KILLFOCUS(IDC_JOURNALPICKER, OnKillfocusJournalpicker)
	ON_CBN_SELCHANGE(IDC_JOURNALPICKER, OnSelchangeJournalpicker)
	ON_EN_KILLFOCUS(IDC_STRREF, OnKillfocusStrref)
	ON_BN_CLICKED(IDC_ADDJOURNAL, OnAddjournal)
	ON_BN_CLICKED(IDC_DELJOURNAL, OnDeljournal)
	ON_EN_KILLFOCUS(IDC_STARTAREA, OnKillfocusStartarea)
	ON_EN_KILLFOCUS(IDC_GAMETIME, OnKillfocusGametime)
	ON_EN_KILLFOCUS(IDC_GOLD, OnKillfocusGold)
	ON_BN_CLICKED(IDC_JOINABLE, OnJoinable)
	ON_BN_CLICKED(IDC_PARTY, OnParty)
	ON_BN_CLICKED(IDC_VARIABLES, OnVariables)
	ON_BN_CLICKED(IDC_DVAR, OnDvar)
	ON_BN_CLICKED(IDC_JOURNAL, OnJournal)
	ON_EN_KILLFOCUS(IDC_VARNAME, OnKillfocusVarname)
	ON_EN_KILLFOCUS(IDC_VARNAME2, OnKillfocusVarname2)
	ON_CBN_KILLFOCUS(IDC_FACE, OnKillfocusFace)
	ON_EN_KILLFOCUS(IDC_CHAPTER, OnKillfocusChapter)
	ON_EN_KILLFOCUS(IDC_TIME, OnKillfocusTime)
	ON_EN_KILLFOCUS(IDC_SECTION, OnKillfocusSection)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_CHECK, OnCheck)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	ON_CBN_KILLFOCUS(IDC_CREATURENUM, OnKillfocusCreaturenum)
	ON_COMMAND(ID_FILE_SAVE, OnSave)
	ON_COMMAND(ID_FILE_NEW, OnNew)
	ON_COMMAND(ID_FILE_LOAD, OnLoad)
	ON_COMMAND(ID_FILE_LOADEXTERNALSCRIPT, OnLoadex)
	ON_COMMAND(ID_FILE_SAVEAS, OnSaveas)
	ON_COMMAND(ID_CHECK, OnCheck)
	ON_EN_KILLFOCUS(IDC_SCREEN, OnKillfocusScreen)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGameEdit message handlers

void CGameEdit::OnLoad() 
{
	int res;
	
  pickerdlg.m_restype=REF_GAM;
  pickerdlg.m_picked=itemname;
	res=pickerdlg.DoModal();
	if(res==IDOK)
	{
		res=read_game(pickerdlg.m_picked);
    switch(res)
    {
    case -1:
      MessageBox("Game loaded with errors (corrected).","Warning",MB_ICONEXCLAMATION|MB_OK);
      break;
    case 1:
      MessageBox("Headers will be reordered.","Warning",MB_ICONEXCLAMATION|MB_OK);
      break;
    case 0:
      break;
    default:
      MessageBox("Cannot read game!","Error",MB_ICONEXCLAMATION|MB_OK);
      NewGame();
      break;
    }
	}
  RefreshDialog();
}

static char BASED_CODE szFilter[] = "Game files (*.gam)|*.gam|All files (*.*)|*.*||";

void CGameEdit::OnLoadex() 
{
  CString filepath;
  int fhandle;
  int res;
  
  res=OFN_FILEMUSTEXIST|OFN_ENABLESIZING|OFN_EXPLORER;
  if(readonly) res|=OFN_READONLY;
  CFileDialog m_getfiledlg(TRUE, "gam", makeitemname(".gam",0), res, szFilter);

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
    readonly=m_getfiledlg.GetReadOnlyPref();
    itemname=m_getfiledlg.GetFileTitle(); //itemname moved here because the loader relies on it
    itemname.MakeUpper();
    res=the_game.ReadGameFromFile(fhandle,-1);
    close(fhandle);
    switch(res)
    {
    case -1:
      MessageBox("Game loaded with errors (corrected).","Warning",MB_ICONEXCLAMATION|MB_OK);
      break;
    case 1:
      MessageBox("Headers will be reordered.","Warning",MB_ICONEXCLAMATION|MB_OK);
      break;
    case 0:
      break;
    default:
      MessageBox("Cannot read game!","Error",MB_ICONEXCLAMATION|MB_OK);
      NewGame();
      break;
    }
  }
  RefreshDialog();
}

void CGameEdit::OnNew() 
{
  NewGame();
  RefreshDialog();
}

void CGameEdit::OnSaveas()
{
  SaveGame(0);
}

void CGameEdit::OnSave()
{
  SaveGame(1);
}

void CGameEdit::SaveGame(int save) 
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
  CFileDialog m_getfiledlg(FALSE, "gam", makeitemname(".gam",0), res, szFilter);

  if(save)
  {
    newname=itemname;
    filepath=makeitemname(".gam",0);
    goto gotname;
  }
restart:  
  if( m_getfiledlg.DoModal() == IDOK )
  {
    filepath=m_getfiledlg.GetPathName();
    filepath.MakeLower();
    if(filepath.Right(4)!=".gam")
    {
      filepath+=".gam";
    }
    newname=m_getfiledlg.GetFileName();
    newname.MakeUpper();
    if(newname.Right(4)==".GAM") newname=newname.Left(newname.GetLength()-4);
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
      MessageBox("Can't write file!","Error",MB_ICONEXCLAMATION|MB_OK);
      goto restart;
    }
    res=the_game.WriteGameToFile(fhandle, 0);
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
      MessageBox("Not enough memory.","Error",MB_ICONSTOP|MB_OK);
      break;
    default:
      MessageBox("Unhandled error!","Error",MB_ICONSTOP|MB_OK);
    }
  }
  RefreshDialog();
}

void CGameEdit::OnCheck() 
{
  int ret;

	ret=((CChitemDlg *) AfxGetMainWnd())->check_game();
  if(ret)
  {
    MessageBox(lasterrormsg,"Game editor",MB_ICONEXCLAMATION|MB_OK);
  }
}

void CGameEdit::OnKillfocusNpcpicker() 
{
	UpdateData(UD_DISPLAY);
}

void CGameEdit::OnSelchangeNpcpicker() 
{
	UpdateData(UD_DISPLAY);
}

void CGameEdit::OnKillfocusArea() 
{
	UpdateData(UD_RETRIEVE);
  RefreshDialog();
}

void CGameEdit::OnKillfocusCreres() 
{
	UpdateData(UD_RETRIEVE);
  RefreshDialog();
}

void CGameEdit::OnKillfocusXpos() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CGameEdit::OnKillfocusYpos() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CGameEdit::OnKillfocusFace() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CGameEdit::OnBrowse1() 
{
  int pos;
//  CItemPicker picker(REF_ARE);
  
  pos=m_npcpicker.GetCurSel();
  if(pos<0) return;
  pickerdlg.m_restype=REF_ARE;
  RetrieveResref(pickerdlg.m_picked,the_game.npcs[pos].curarea);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_game.npcs[pos].curarea);
  }
  RefreshDialog();
}

void CGameEdit::OnBrowse2() 
{
  int pos;
  
  pos=m_npcpicker.GetCurSel();
  if(pos<0) return;
  pickerdlg.m_restype=REF_CRE;
  RetrieveResref(pickerdlg.m_picked,the_game.npcs[pos].creresref);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_game.npcs[pos].creresref);
  }
  RefreshDialog();
}
int CGameEdit::ReadTempCreature(char *&creature, long &esize)
{
  int fhandle;

  fhandle=open("dltcep.cre",O_RDONLY);
  if(fhandle<1) return -2;
  esize=filelength(fhandle);
  if(creature) delete creature;
  creature=new char[esize];
  if(!creature)
  {
    close(fhandle);
    return -3;
  }
  if(read(fhandle,creature,esize)!=esize)
  {
    close(fhandle);
    return -2;
  }
  close(fhandle);
  return 0;
}

void CGameEdit::OnEdit() 
{
  CString tmpname;
  CCreatureEdit dlg;
  int creaturenum;
  int ret;

	creaturenum=m_creaturenum_control.GetCurSel();
	if(creaturenum<0) return;
  //creatureedit
  tmpname=itemname;
  itemname=ResolveName((char *) (the_game.pcextensions+creaturenum),(creature_header *) the_game.pcstructs[creaturenum] );
  ret=WriteTempCreature(the_game.pcstructs[creaturenum],the_game.pcs[creaturenum].cresize);
  if(ret>=0)
  {
    dlg.DoModal();
  }
  itemname=tmpname;
}

void CGameEdit::OnBrowse() 
{
  CString tmpstr;
  
  pickerdlg.m_restype=REF_ARE;
  RetrieveResref(pickerdlg.m_picked,the_game.header.mainarea);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_game.header.mainarea);
  }
  RefreshDialog();
}

void CGameEdit::OnAddnpc() 
{
  int pos;

  pos=m_npcpicker.GetCurSel()+1;
	the_game.insert_npc_header(pos);
  m_npcpicker.AddString("");
  m_npcpicker.SetCurSel(pos);
  RefreshDialog();
}

void CGameEdit::OnDelnpc() 
{
  gam_npc *newnpcs;
  gam_npc_extension *newextensions;
  charpoi *newstructs;
  int pos;

	pos=m_npcpicker.GetCurSel();
  if(pos<0 || pos>the_game.header.npccount) return;  
  if(the_game.npcstructs[pos])
  {
    delete [] the_game.npcstructs[pos];
    the_game.npcstructs[pos]=0;
  }
  the_game.npcs[pos].creoffset=0;
  the_game.npcs[pos].cresize=0;
  the_game.npccount--;
  newnpcs=new gam_npc[the_game.npccount];
  if(!newnpcs)
  {
    the_game.npccount++;
    return;
  }
  newstructs=new charpoi[the_game.npccount];
  if(!newstructs)
  {
    the_game.npccount++;
    return;
  }
  
  memcpy(newnpcs,the_game.npcs,pos*sizeof(gam_npc) );
  memcpy(newnpcs+pos,the_game.npcs+pos+1, (the_game.npccount-pos)*sizeof(gam_npc) );
  memcpy(newstructs,the_game.npcstructs,pos*sizeof(charpoi) );
  memcpy(newstructs+pos,the_game.npcstructs+pos+1, (the_game.npccount-pos)*sizeof(charpoi) );
  newextensions=new gam_npc_extension[the_game.npccount];
  if(!newextensions)
  {
    the_game.npccount++;
    return;
  }
  memcpy(newextensions,the_game.npcextensions,pos*sizeof(gam_npc_extension) );
  memcpy(newextensions+pos,the_game.npcextensions+pos+1, (the_game.npccount-pos)*sizeof(gam_npc_extension) );
  delete [] the_game.npcextensions;
  the_game.npcextensions=newextensions;
  delete [] the_game.npcs;
  the_game.npcs=newnpcs;
  delete [] the_game.npcstructs;
  the_game.npcstructs=newstructs;
  
  the_game.header.npccount=the_game.npccount;
  the_game.npcstructcount=the_game.npccount;
  the_game.npcextensioncount=the_game.npccount;
  m_npcpicker.SetCurSel(pos);
  RefreshDialog();
}

void CGameEdit::OnKillfocusVariablepicker() 
{
	UpdateData(UD_DISPLAY);
}

void CGameEdit::OnSelchangeVariablepicker() 
{
	UpdateData(UD_DISPLAY);
}

void CGameEdit::OnKillfocusValue() 
{
	UpdateData(UD_RETRIEVE);
	RefreshDialog();
}

void CGameEdit::OnKillfocusVarname() 
{
  UpdateData(UD_RETRIEVE);
  RefreshDialog();
}

int CGameEdit::GetActualPosition(CComboBox &cb)
{
  int pos;

	pos=cb.GetCurSel();
  if(pos<0) return -1;
  return cb.GetItemData(pos);
}
void CGameEdit::OnDelvar() 
{
  gam_variable *newvars;
  int pos;

  pos=GetActualPosition(m_variablepicker);
  if(pos<0 || pos>=the_game.header.variablecount) return;  

  newvars=new gam_variable[the_game.variablecount-1];
  if(!newvars) return;
  memcpy(newvars,the_game.variables,pos*sizeof(gam_variable) );
  memcpy(newvars+pos,the_game.variables+pos+1, (the_game.variablecount-pos-1)*sizeof(gam_variable) );
  delete [] the_game.variables;
  the_game.variables=newvars;
  the_game.variablecount--;
  the_game.header.variablecount=the_game.variablecount;
  RefreshDialog();
}

void CGameEdit::OnAddvar() 
{
  gam_variable *newvars;
  int pos;

  pos=GetActualPosition(m_variablepicker)+1;
  if(pos>the_game.header.variablecount) pos=the_game.header.variablecount;
  newvars=new gam_variable[the_game.header.variablecount+1];
  memcpy(newvars,the_game.variables,pos*sizeof(gam_variable) );
  memcpy(newvars+pos+1,the_game.variables+pos, (the_game.variablecount-pos)*sizeof(gam_variable) );
  delete [] the_game.variables;
  memset(newvars+pos,0,sizeof(gam_variable));
  the_game.variables=newvars;
  the_game.variablecount++;
  the_game.header.variablecount=the_game.variablecount;
  RefreshDialog();
}

void CGameEdit::OnKillfocusVariablepicker2() 
{
	UpdateData(UD_DISPLAY);
}

void CGameEdit::OnSelchangeVariablepicker2() 
{
	UpdateData(UD_DISPLAY);
}

void CGameEdit::OnKillfocusValue2() 
{
	UpdateData(UD_RETRIEVE);
  RefreshDialog();
}

void CGameEdit::OnKillfocusVarname2() 
{
  UpdateData(UD_RETRIEVE);
  RefreshDialog();
}

void CGameEdit::OnDelvar2() 
{
  gam_variable *newvars;
  int pos;

	pos=GetActualPosition(m_dvarpicker);
  if(pos<0 || pos>the_game.header.dvarcount) return;  
  newvars=new gam_variable[the_game.deathvariablecount-1];
  if(!newvars) return;
  memcpy(newvars,the_game.deathvariables,pos*sizeof(gam_variable) );
  memcpy(newvars+pos,the_game.deathvariables+pos+1, (the_game.deathvariablecount-pos-1)*sizeof(gam_variable) );
  delete [] the_game.deathvariables;
  the_game.deathvariables=newvars;
  the_game.deathvariablecount--;
  the_game.header.dvarcount=the_game.deathvariablecount;
  RefreshDialog();
}

void CGameEdit::OnAddvar2() 
{
  gam_variable *newvars;
  int pos;

  pos=GetActualPosition(m_dvarpicker)+1;
  if(pos>the_game.header.dvarcount) pos=the_game.header.dvarcount;
  newvars=new gam_variable[the_game.header.dvarcount+1];
  memcpy(newvars,the_game.deathvariables,pos*sizeof(gam_variable) );
  memcpy(newvars+pos+1,the_game.deathvariables+pos, (the_game.deathvariablecount-pos)*sizeof(gam_variable) );
  delete [] the_game.deathvariables;
  memset(newvars+pos,0,sizeof(gam_variable));
  the_game.deathvariables=newvars;
  the_game.deathvariablecount++;
  the_game.header.dvarcount=the_game.deathvariablecount;
  RefreshDialog();
}

void CGameEdit::OnKillfocusFamiliarpicker() 
{
	UpdateData(UD_DISPLAY);
}

void CGameEdit::OnSelchangeFamiliarpicker() 
{
	UpdateData(UD_DISPLAY);
}

void CGameEdit::OnKillfocusFamiliar() 
{
	UpdateData(UD_RETRIEVE);
  RefreshDialog();
}

void CGameEdit::OnKillfocusJournalpicker() 
{
	UpdateData(UD_DISPLAY);
}

void CGameEdit::OnSelchangeJournalpicker() 
{
	UpdateData(UD_DISPLAY);
}

void CGameEdit::OnKillfocusStrref() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CGameEdit::OnKillfocusChapter() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CGameEdit::OnKillfocusSection() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CGameEdit::OnKillfocusTime() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CGameEdit::OnAddjournal() 
{
  gam_journal *newjournals;
  int pos;

	pos=GetActualPosition(m_journalpicker)+1;
  if(pos>the_game.header.journalcount) pos=the_game.header.journalcount;
  newjournals=new gam_journal[the_game.header.journalcount+1];
  if(!newjournals) return;
  memcpy(newjournals,the_game.journals,pos*sizeof(gam_journal) );
  memcpy(newjournals+pos+1,the_game.journals+pos, (the_game.journalcount-pos-1)*sizeof(gam_journal) );
  delete [] the_game.journals;
  memset(newjournals+pos,0,sizeof(gam_journal));
  newjournals[pos].unknown09=0xff; //these appear to be set to 0xff
  newjournals[pos].unknown0b=0xff;
  the_game.journals=newjournals;
  the_game.journalcount++;
  the_game.header.journalcount=the_game.journalcount;
  RefreshDialog();
  m_journalpicker.SetCurSel(pos);
}

void CGameEdit::OnDeljournal() 
{
  gam_journal *newjournals;
  int pos;

	pos=GetActualPosition(m_journalpicker);
  if(pos<0 || pos>the_game.header.journalcount) return;  
  newjournals=new gam_journal[the_game.journalcount-1];
  if(!newjournals) return;
  memcpy(newjournals,the_game.journals,pos*sizeof(gam_journal) );
  memcpy(newjournals+pos,the_game.journals+pos+1, (the_game.journalcount-pos)*sizeof(gam_journal) );
  delete [] the_game.journals;
  the_game.journals=newjournals;
  the_game.journalcount--;
  the_game.header.journalcount=the_game.journalcount;
  m_journalpicker.SetCurSel(pos);
  RefreshDialog();
}

void CGameEdit::OnKillfocusStartarea() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CGameEdit::OnKillfocusGametime() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CGameEdit::OnKillfocusGold() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CGameEdit::OnJoinable() 
{
	the_game.KillNPCs();
	the_game.header.npccount=0;
  m_npcpicker.SetCurSel(-1);
  RefreshDialog();
}

void CGameEdit::OnParty() 
{
  the_game.KillPCs();
	the_game.header.pccount=0;
  RefreshDialog();
}

void CGameEdit::OnVariables() 
{
  the_game.KillVariables();
  the_game.header.variablecount=0;
  m_variablepicker.SetCurSel(-1);
  RefreshDialog();
}

void CGameEdit::OnDvar() 
{
  the_game.KillDeathVariables();
  the_game.header.dvarcount=0;
  m_dvarpicker.SetCurSel(-1);
  RefreshDialog();
}

void CGameEdit::OnJournal() 
{
  the_game.KillJournals();
  the_game.header.journalcount=0;
  m_journalpicker.SetCurSel(-1);
  RefreshDialog();
}

BOOL CGameEdit::PreTranslateMessage(MSG* pMsg) 
{
  m_tooltip.RelayEvent(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}

void CGameEdit::OnKillfocusCreaturenum() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CGameEdit::OnKillfocusScreen() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}
