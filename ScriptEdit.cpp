// ScriptEdit.cpp : implementation file
//

#include "stdafx.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <direct.h>
#include "2da.h"
#include "chitemDlg.h"
#include "ScriptEdit.h"
#include "2daEdit.h"
#include "options.h"
#include "compat.h"
#include "WeiduLog.h"
#include "StrRefDlg.h"
#include "Decompiler.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define SHOW_MESSAGES 1
#define FORCED_CHECK  2
#define WHOLE_CHECK   4

#define MAX_SCRIPT_LEN 6553599
#define LINES 30

#define TA_ACTION    0
#define TA_TRIGGER   1
#define TA_IF        2
#define TA_RESPONSE  3
#define TA_THEN      4
#define TA_ENDIF     5

//static function
//displays compiler data in human readable format
CString format_parameters(compiler_data &compiler_data)
{
  CString ret, add;
  int i;

  for(i=0;i<compiler_data.parnum;i++)
  {
    switch(compiler_data.parameters[i].type)
    {
    case SPT_ZERO: add="0"; break;
    case SPT_1AREA: case SPT_2AREA:
    case SPT_AREA1: case SPT_AREA2: case SPT_AREA3: case SPT_AREA4: add="qualifier"; break;
    case SPT_VAR1: case SPT_VAR2: case SPT_VAR3: case SPT_VAR4: add="variable"; break;
    case SPT_OBJECT:case SPT_OVERRIDE: case SPT_SECONDOB: add="object"; break;
    case SPT_INTEGER:case SPT_INTEGER2:case SPT_INTEGER3:
      if(compiler_data.parameters[i].idsfile.IsEmpty()) add="integer";
      else add="ids:"+compiler_data.parameters[i].idsfile;
      break;
    case SPT_STRING1: case SPT_STRING2: add="string"; break;
    case SPT_STRREF:case SPT_STRREF2: case SPT_STRREF3: add="strref"; break;
    case SPT_ACTION: add="action"; break;
    case SPT_POINT: add="point"; break;
    case SPT_RES1:case SPT_RES2:case SPT_RES3:case SPT_RES4:
    case SPT_RESREF1: case SPT_RESREF2: add="resource"; break;
    case SPT_DEAD1: case SPT_DEAD2: add="deathvar"; break;
    case SPT_COLUMN1: case SPT_COLUMN2: add="xplist"; break;
    case SPT_TOKEN1: case SPT_TOKEN2: add="token"; break;
    case SPT_NUMLIST1: case SPT_NUMLIST2: add="spellist"; break;
    default: add="unknown";
    }
    if(i) ret+=", "+add;
    else ret+=add;
  }
  return ret;
}

//a rather lame windows-style way of outputting a richedit control
unsigned long __stdcall writestream_callback(unsigned long cookie, LPBYTE buffer, long cb, long *pcb)
{
  int res;

  res=write(cookie,buffer,cb)!=cb;
  *pcb=cb;
  return res;
}

int writestream(int fhandle, CRichEditCtrl &ctrl)
{
  EDITSTREAM es;

  es.dwCookie=fhandle;
  es.dwError=0;
  es.pfnCallback=writestream_callback;
  ctrl.StreamOut(SF_TEXT,es);
  return es.dwError;
}
/////////////////////////////////////////////////////////////////////////////
// CScriptEdit dialog

CScriptEdit::CScriptEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CScriptEdit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CScriptEdit)
	//}}AFX_DATA_INIT
  m_errors=NULL;
  m_count=0;
  m_firsterror=0;
  m_bcs=1;
  m_searchdlg=NULL;
}

void CScriptEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScriptEdit)
	DDX_Control(pDX, IDC_TEXT, m_text_control);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CScriptEdit, CDialog)
	//{{AFX_MSG_MAP(CScriptEdit)
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_BN_CLICKED(IDC_LOADEX, OnLoadex)
	ON_BN_CLICKED(IDC_SAVEAS, OnSaveas)
	ON_BN_CLICKED(IDC_NEW, OnNew)
	ON_EN_SETFOCUS(IDC_TEXT, OnSetfocusText)
	ON_BN_CLICKED(IDC_CHECK, OnCheck)
	ON_COMMAND(ID_FILE_SAVESCRIPT, OnSaveAsBAF)
	ON_COMMAND(ID_FILE_SAVECOMPILESCRIPT, OnSaveAsBCS)
	ON_COMMAND(ID_FILE_LOADSOURCE, OnFileLoadsource)
	ON_COMMAND(ID_SEARCH, OnSearch)
	ON_EN_KILLFOCUS(IDC_TEXT, OnKillfocusText)
	ON_EN_MAXTEXT(IDC_TEXT, OnMaxtextText)
	ON_COMMAND(ID_OPTIONS_AUTOCHECK, OnOptionsAutocheck)
	ON_COMMAND(ID_OPTIONS_LARGEINDENT, OnOptionsLargeindent)
	ON_COMMAND(ID_EDIT_SELECTALL, OnEditSelectall)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_COMMAND(ID_EDIT_DELETE, OnEditDelete)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_TOOLS_IDSBROWSER, OnIDS)
	ON_WM_HELPINFO()
	ON_EN_CHANGE(IDC_TEXT, OnChangeText)
	ON_EN_UPDATE(IDC_TEXT, OnUpdateText)
	ON_COMMAND(ID_TOOLS_LOOKUPSTRREF, OnToolsLookupstrref)
	ON_COMMAND(ID_OPTIONS_USEINTERNALCOMPILER, OnOptionsUseinternalcompiler)
	ON_COMMAND(ID_OPTIONS_USEINTERNALDECOMPILER, OnOptionsUseinternaldecompiler)
	ON_COMMAND(ID_FILE_NEW, OnNew)
	ON_COMMAND(ID_FILE_LOAD, OnLoad)
	ON_COMMAND(ID_FILE_SAVEAS, OnSaveas)
	ON_COMMAND(ID_FILE_LOADEXTERNALSCRIPT, OnLoadex)
	ON_COMMAND(ID_CHECK, OnCheck)
	ON_COMMAND(ID_OPTIONS_LOGGING, OnOptionsLogging)
	//}}AFX_MSG_MAP
  ON_REGISTERED_MESSAGE( WM_FINDREPLACE, OnFindReplace )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScriptEdit message handlers

BOOL CScriptEdit::OnInitDialog() 
{
  CFont Font;
  CString tmpstr,tmpstr1,tmpstr2;

  the_ids.new_ids();
  m_idsname="";
  if(editflg&INDENT) m_indent=2;
  else m_indent=4;
	CDialog::OnInitDialog();
  m_text_control.LimitText(MAX_SCRIPT_LEN);
  m_text_control.ModifyStyle(0,ES_SAVESEL);
  m_text_control.SetOptions(ECOOP_OR,ECO_AUTOWORDSELECTION|ECO_SAVESEL|ECO_WANTRETURN|ECO_NOHIDESEL);
  //enables the update event for a rich edit control
  m_text_control.SetEventMask(ENM_CHANGE|ENM_UPDATE|ENM_MOUSEEVENTS|ENM_KEYEVENTS|ENM_SCROLL|ENM_SELCHANGE);
  Font.CreateFont(15,0,0,0,0,0,0,0,0,0,0,0,0,"Courier New");
  m_text_control.SetFont(&Font);
  Font.DeleteObject();
//  UpdateData(UD_DISPLAY);
  CheckScript(WHOLE_CHECK|FORCED_CHECK);
  RefreshDialog();
  RefreshMenu();
  m_infind=0;
  //tooltips
  {
    m_tooltip.Create(this,TTS_NOPREFIX);
    m_tooltip.SetMaxTipWidth(200);
    m_tooltip.SetTipBkColor(RGB(240,224,160));
    
    m_tooltip.AddTool(GetDlgItem(IDCANCEL), IDS_CANCEL);
    tmpstr1.LoadString(IDS_LOAD);
    tmpstr2.LoadString(IDS_SCRIPT);
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

int CScriptEdit::RunWeidu(CString syscommand)
{
  CString tmpstr;
  int res;

  //flushing dialog.tlk
  if(global_changed[0]==true)
  {
    ((CChitemDlg *) AfxGetMainWnd())->write_file_progress(0); 
  }
  if(global_changed[1]==true)
  {
    ((CChitemDlg *) AfxGetMainWnd())->write_file_progress(1); 
  }
  if(weiduflg&WEI_LOGGING)
  {
    unlink(WEIDU_LOG);
  }
  res=my_system(syscommand);
  
  if(weiduflg&WEI_LOGGING)
  {
    if(file_exists(WEIDU_LOG) )
    {
      CWeiduLog dlg;
      dlg.DoModal();
    }
    else MessageBox("No logfile has been created by WeiDU.","Script editor",MB_OK);
  }
  if(res)
  {
    tmpstr.Format("[%s] returned: %d",syscommand, res);
    MessageBox(tmpstr,"Script editor",MB_OK|MB_ICONSTOP);
  }

  return res;
}

int CScriptEdit::compile(CString filepath)
{
  CString tmpstr;
  CString outpath;
  CString syscommand;
  int res;

  res=0;
  if(weidupath.IsEmpty())
  {
    MessageBox("Please set up WeiDU before use.","Script editor",MB_OK|MB_ICONSTOP);
    return -1;
  }
  if(!file_exists(weidupath) )
  {
    MessageBox("WeiDU executable not found.","Script editor",MB_OK|MB_ICONSTOP);
    return -1;
  }
  chdir(bgfolder);
  outpath.Format("override");
  mkdir(outpath);
  if(!dir_exists(outpath))
  {
    tmpstr.Format("%s cannot be created as output path.",outpath);
    MessageBox(tmpstr,"Dialog editor",MB_OK|MB_ICONSTOP);
    return -1;
  }
  syscommand=AssembleWeiduCommandLine(filepath,outpath); //import (compile)
  res=RunWeidu(syscommand);
  ((CChitemDlg *) AfxGetMainWnd())->rescan_dialog(true);
  ((CChitemDlg *) AfxGetMainWnd())->scan_override();
  return res;
}

//decompiles file and returns the decompiled filename
//filepath is the long path of the compiled file

int CScriptEdit::decompile(CString &filepath, CString tmpname)
{
  CString tmpstr;
  CString syscommand;
  int res;

  unlink(bgfolder+weidudecompiled+"\\"+tmpname+".baf");
  res=0;
  //actually, if the bit is set, it means weidu is used
  if(editflg&INTERNALDECOMP)
  {
    if(weidupath.IsEmpty())
    {
      MessageBox("Please set up WeiDU before use.","Script editor",MB_OK|MB_ICONSTOP);
      return -1;
    }
    if(!file_exists(weidupath) )
    {
      MessageBox("WeiDU executable not found.","Script editor",MB_OK|MB_ICONSTOP);
      return -1;
    }
  }
  chdir(bgfolder);
  mkdir(weidudecompiled);
  if(!dir_exists(weidudecompiled))
  {
    tmpstr.Format("%s cannot be created as output path.",weidudecompiled);
    MessageBox(tmpstr,"Dialog editor",MB_OK|MB_ICONSTOP);
    return -1;
  }
  if(global_changed[0]==true)
  {
    ((CChitemDlg *) AfxGetMainWnd())->write_file_progress(0); 
  }
  if(global_changed[1]==true)
  {
    ((CChitemDlg *) AfxGetMainWnd())->write_file_progress(1); 
  }

  if(editflg&INTERNALDECOMP)
  {
    syscommand=AssembleWeiduCommandLine(filepath,weidudecompiled); //export
    res=RunWeidu(syscommand);
  }
  else
  {
    Decompiler *sdec = new Decompiler(weiduflg&WEI_LOGGING);
    //setting up decompiler with options
    res = sdec->Decompile(filepath, bgfolder+weidudecompiled+"\\"+tmpname+".baf");
    //actually this is not needed, delete will close all anyway
    sdec->CloseAll();
    delete sdec;
    if (res) {
      MessageBox("Decompiler error.","Script editor",MB_OK|MB_ICONSTOP);
      res = 0;
    }
  }
  filepath=bgfolder+weidudecompiled+"\\"+tmpname+".baf";
  return res;
}

void CScriptEdit::OnLoad() 
{
  CString filepath;
  CString m_text;
  int fhandle;
	int res;
  char *pos;
	
  pickerdlg.m_restype=REF_BCS;
  pickerdlg.m_picked=itemname;
	res=pickerdlg.DoModal();
	if(res==IDOK)
	{
    filepath=pickerdlg.m_picked+".bcs";
    itemname=pickerdlg.m_picked;
    m_bcs=1;
		res=decompile(filepath, itemname); //this will decompile the bcs
    if(res)
    {
      MessageBox("Cannot decompile file!","Error",MB_OK);
    }
    else
    {
      fhandle=open(filepath, O_RDONLY|O_BINARY);
      if(!fhandle)
      {
        MessageBox("Cannot open file!","Error",MB_OK);
        res=-1;
      }
      if(!res) 
      {//fhandle will always get initialized if res was 0
        res=filelength(fhandle);
        if(res>=0)
        {
          pos=m_text.GetBufferSetLength(res);
          if(pos)
          {
            if(read(fhandle,pos,res)!=res) res=-1;
            else res=0;
          }
          else res=-1;
          m_text.ReleaseBuffer();
          m_text_control.SetWindowText(m_text);
        }
        close(fhandle);
      }
    }
    switch(res)
    {
    case 0:
      if((m_bcs==1) && (editflg&REMBAF) )
      {//removing decompiled baf
        unlink(filepath);
      }
      break;
    default:
      MessageBox("Cannot read script!","Error",MB_OK);
      OnNew();
      return;
    }
	}
  UpdateData(UD_DISPLAY);
  the_script.m_changed=false;
  CheckScript(WHOLE_CHECK|FORCED_CHECK);
  RefreshDialog();
}

static char BASED_CODE szFiltera[] = "Script files (*.bcs)|*.bcs|Script Source files (*.baf)|*.baf|All files (*.*)|*.*||";
static char BASED_CODE szFilterb[] = "Script Source files (*.baf)|*.baf|Script files (*.bcs)|*.bcs|All files (*.*)|*.*||";

void CScriptEdit::OnLoadex() 
{
  char BASED_CODE *szFilter;
  CString filepath;
  CString m_text;
  int fhandle;
  int res;
  char *pos;
  
  res=OFN_FILEMUSTEXIST|OFN_ENABLESIZING|OFN_EXPLORER;
  if(readonly) res|=OFN_READONLY;
  if(m_bcs)
  {
    szFilter=szFiltera;
  }
  else
  {
    szFilter=szFilterb;
  }
  CFileDialog m_getfiledlg(TRUE, m_bcs?"bcs":"baf", m_bcs ? makeitemname(".bcs",0): makeitemname(".baf",1), res, szFilter);

  if( m_getfiledlg.DoModal() == IDOK )
  {
    filepath=m_getfiledlg.GetPathName();
    if(checkfile(filepath,"SC") )
    {
      m_bcs=1;
      res=decompile(filepath, m_getfiledlg.GetFileTitle()); //decompile first
      if(res)
      {
        MessageBox("Cannot decompile file!","Error",MB_OK);
        return;
      }
      lastopenedoverride=filepath.Left(filepath.ReverseFind('\\'));
    }
    else
    { //remember the path when opening a script source 
      m_bcs=0;
      lastopenedscript=filepath.Left(filepath.ReverseFind('\\'));
    }
    fhandle=open(filepath, O_RDONLY|O_BINARY);
    if(fhandle<1)
    {
      MessageBox("Cannot open file!","Error",MB_OK);
      return;
    }
    readonly=m_getfiledlg.GetReadOnlyPref();
    res=filelength(fhandle);
    if(res>=0)
    {
      pos=m_text.GetBufferSetLength(res);
      if(pos)
      {
        if(read(fhandle,pos,res)!=res) res=-9;
        else res=0;
      }
      else res=-9;
    }
    m_text.ReleaseBuffer();
    close(fhandle);
    m_text_control.SetWindowText(m_text);
    switch(res)
    {
    case 0:
      if((m_bcs==1) && (editflg&REMBAF) )
      {//removing decompiled baf
        unlink(filepath);
      }
      itemname=m_getfiledlg.GetFileTitle();
      itemname.MakeUpper();
      break;
    case -9: //special error: cannot read decompiled script
      MessageBox("Cannot read script!","Error",MB_OK);
    default: //that error was already handled elsewhere
      OnNew();
      return;
    }
  }
  UpdateData(UD_DISPLAY);
  the_script.m_changed=false;
  CheckScript(WHOLE_CHECK|FORCED_CHECK);
  RefreshDialog();
}

void CScriptEdit::OnSaveas() 
{
  OnSaveasGeneral(-1);
}

void CScriptEdit::OnSaveAsBCS() 
{
  OnSaveasGeneral(1);
}

void CScriptEdit::OnSaveAsBAF() 
{
  OnSaveasGeneral(0);
}

void CScriptEdit::OnSaveasGeneral(int type) 
{
  char BASED_CODE *szFilter;
  CString tmpstr;
  CString newname;
  CString filepath;
  int fhandle;
  int res;

  if(readonly)
  {
    MessageBox("You opened it read only!","Warning",MB_ICONEXCLAMATION|MB_OK);
    return;
  }
  res=OFN_HIDEREADONLY|OFN_ENABLESIZING|OFN_EXPLORER;

  if(type!=-1) m_bcs=type;
  if(m_bcs)
  {
    szFilter=szFiltera;
    tmpstr=bgfolder+"override\\"+itemname;
  }
  else
  {
    szFilter=szFilterb;    
    tmpstr=makeitemname(".baf",1);
  }
  CFileDialog m_getfiledlg(FALSE, m_bcs?"bcs":"baf", tmpstr, res, szFilter);

restart:  
  if( m_getfiledlg.DoModal() == IDOK )
  {
    filepath=m_getfiledlg.GetPathName();
    filepath.MakeLower();
    if(filepath.Right(4)==".baf") m_bcs=0;
    else if(filepath.Right(4)==".bcs") m_bcs=1; //this is the default
    if(m_bcs)
    {
      if(filepath.Right(4)!=".bcs")  filepath+=".bcs";
      lastopenedoverride=filepath.Left(filepath.ReverseFind('\\'));
    }
    else
    {
      if(filepath.Right(4)!=".baf")  filepath+=".baf";
      lastopenedscript=filepath.Left(filepath.ReverseFind('\\'));
    }
    newname=m_getfiledlg.GetFileName();
    newname.MakeUpper();
    if(newname.Right(4)==".BCS") newname=newname.Left(newname.GetLength()-4);
    else if(newname.Right(4)==".BAF") newname=newname.Left(newname.GetLength()-4);
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
    
    itemname=newname;
    res=0;
    if(filepath.Right(4)==".bcs")
    {
      chdir(bgfolder);
      tmpstr=bgfolder+weidudecompiled;
      mkdir(tmpstr);
      if(!dir_exists(tmpstr) )
      {
        res=-3;
      }
      else tmpstr+="\\"+newname+".baf";
    }
    if(!res)
    {
      fhandle=creat(tmpstr,S_IWRITE);
      if(fhandle>0)
      {
        //this is the intended way of extracting info from a richedit control
        res=writestream(fhandle,m_text_control);
        close(fhandle);
      }
      else res=-2;
      if(m_bcs)
      {
        if(!res) res=compile(tmpstr);
      }
    }
    switch(res)
    {
    case -3:
      newname.Format("Cannot create output path:%s",tmpstr);
      MessageBox(tmpstr,"Script editor",MB_ICONSTOP|MB_OK);
      break;
    case -2:
      newname.Format("Cannot create  file:%s",tmpstr);
      MessageBox(tmpstr,"Script editor",MB_ICONSTOP|MB_OK);
      break;
    case 0:
      if(m_bcs) MessageBox("Script saved and compiled...","Script editor",MB_ICONINFORMATION|MB_OK);
      else MessageBox("Script saved...","Script editor",MB_ICONINFORMATION|MB_OK);
      break;
    default:
      MessageBox("Error during save.","Script editor",MB_ICONSTOP|MB_OK);
    }
  }
  UpdateData(UD_DISPLAY);
  the_script.m_changed=false;
  CheckScript(0);
  RefreshDialog();
}

void CScriptEdit::NewScript()
{
	itemname="new script";
  if(m_errors) delete [] m_errors;
  m_errors=NULL;
  m_count=0;
  m_firsterror=0;
  m_bcs=1;
  the_script.m_changed=false;
}

void CScriptEdit::OnNew() 
{
  NewScript();
  m_text_control.SetWindowText("");
  UpdateData(UD_DISPLAY);
  CheckScript(FORCED_CHECK|WHOLE_CHECK);
  RefreshDialog();
}

void format_line(CString &line, int indent)
{
  if(line.IsEmpty()) return;
  line.SetAt(0,(char) toupper(line.GetAt(0)));
  line=CString(TCHAR(' '),indent)+line;
}

int match_string(CString &line, CString token, int indent, int error)
{
  int maxi, maxj;
  int i,j;

  maxi=line.GetLength();
  if(!maxi) return 0;
  maxj=token.GetLength();
  i=0;
  j=0;
  while(j<maxj)
  {
    if(token[j]=='%')
    {
      switch(token[++j])
      {
      case 'd': //number
        if(i>=maxi) return error;
        while((i<maxi) && (line[i]>='0') && (line[i]<='9') ) i++;
        break;
      }
      j++;
      continue;
    }
    if(i>=maxi) return error;           //end of line without full parse
    if(toupper(line[i])!=token[j]) return error; //difference
    i++;
    j++;
  }
  if(i<maxi) return error;              //excess on line
  line.MakeUpper();
  line=CString(TCHAR(' '),indent)+line;
  return 0;
}

void CScriptEdit::RefreshMenu()
{
  CMenu *pMenu;

  pMenu=GetMenu();
  if(pMenu)
  {
    pMenu->CheckMenuItem(ID_OPTIONS_AUTOCHECK, MF_SET(!(editflg&NOCHECK)));
    pMenu->CheckMenuItem(ID_OPTIONS_LARGEINDENT, MF_SET(!(editflg&INDENT)));
    pMenu->CheckMenuItem(ID_OPTIONS_USEINTERNALDECOMPILER, MF_SET(!(editflg&INTERNALDECOMP)));
    pMenu->CheckMenuItem(ID_OPTIONS_USEINTERNALCOMPILER, MF_SET(!(editflg&INTERNALCOMPILER)));
    pMenu->CheckMenuItem(ID_OPTIONS_LOGGING, MF_SET(weiduflg&WEI_LOGGING));
  }
}

void CScriptEdit::RefreshDialog()
{
  CString tmpstr;
  CButton *cb;
  int topline;
  int i;

  if(m_firsterror>=0)
  {
    topline=m_text_control.GetFirstVisibleLine();
    for(i=0;i<LINES;i++)
    {
      cb=(CButton *) GetDlgItem(IDC_U1+i);
      if(topline+i>=m_count)
      {
        cb->ShowWindow(false);
      }
      else
      {
        cb->ShowWindow(true);
        tmpstr.Format("%d",topline+i+1);
        cb->SetWindowText(tmpstr);
        if(m_errors[topline+i]&0xffff) cb->SetCheck(false);
        else cb->SetCheck(true);
      }
    }
  }
  else
  {
    topline=m_text_control.GetFirstVisibleLine();
    for(i=0;i<LINES;i++)
    {
      cb=(CButton *) GetDlgItem(IDC_U1+i);
      if(topline+i>=m_count)
      {
        cb->ShowWindow(false);
      }
      else
      {
        cb->ShowWindow(true);
        tmpstr.Format("%d",topline+i+1);
        cb->SetWindowText(tmpstr);
        cb->SetCheck(true);
      }
    }
  }
}

CString CScriptEdit::GetLine(int idx)
{
  int len;
  CString line;

  len=m_text_control.LineLength(m_text_control.LineIndex(idx));
  m_text_control.GetLine(idx,line.GetBufferSetLength(len+4),len);
  line.ReleaseBuffer(len);
  return line;
}

void CScriptEdit::CheckScript(int messages)
{
  CString tmpstr;
  CString linestr;
  CString line;
  trigger trigger;
  action action;
  int i;
  int triggeroraction;
  int startline, endline;
  int num_or;
  int commentpos;

  
  if(m_bcs) {
    tmpstr.Format("Edit script: %s%s",itemname, the_script.m_changed?"*":"");
  }
  else {
    tmpstr.Format("Edit script source: %s%s",itemname, the_script.m_changed?"*":"");
  }
  SetWindowText(tmpstr);
  if(!(messages&FORCED_CHECK) )
  {
    if(editflg&NOCHECK) return;
    if(!m_text_control.GetModify()) return;
  }
  m_text_control.SetModify(false);
  m_count=m_text_control.GetLineCount();

  if(m_errors) delete [] m_errors;
  triggeroraction=TA_IF;
  if(m_count<0)
  {
    m_errors=new int[1];
    m_errors[0]=m_count;
    m_firsterror=0;
    endline=m_count=1;
  }
  else
  {
    m_firsterror=-1;
    m_errors=new int[m_count];
    if(messages&WHOLE_CHECK)
    {
      startline=0;
      endline=m_count;
    }
    else
    {
      startline=m_text_control.GetFirstVisibleLine();
      endline=min(m_count,startline+LINES);
      while(startline>0)
      {
        if(GetLine(startline)=="IF") break;
        startline--;
      }
    }
    num_or=0;
    for(i=startline;i<endline;i++)
    {
      line=GetLine(i);
      commentpos=line.Find("//");
      if(commentpos>=0)
      {
        tmpstr=line.Left(commentpos);
      }
      else tmpstr=line;
      tmpstr.TrimRight();
      tmpstr.TrimLeft();
      if(!tmpstr.IsEmpty())
      {
        switch(triggeroraction)
        {
        case TA_ACTION:
          m_errors[i]=match_string(tmpstr, "END",0,-130);
          if(!m_errors[i])
          {
            triggeroraction=TA_IF;
            break;
          }
          m_errors[i]=match_string(tmpstr, "RESPONSE #%d",m_indent,-110);
          if(!m_errors[i])
          {         
            triggeroraction=TA_ACTION;
            break;
          }
          m_errors[i]=compile_action(tmpstr, action, 0);
          format_line(tmpstr,m_indent*2);
          break;
        case TA_TRIGGER:
          m_errors[i]=match_string(tmpstr, "THEN",0,-120);
          if(!m_errors[i])
          {
            if(num_or)
            {
              m_errors[i]=CE_INCOMPLETE_OR;
            }
            triggeroraction=TA_RESPONSE;
            break;
          }
          m_errors[i]=compile_trigger(tmpstr, trigger);
          format_line(tmpstr, num_or?m_indent+2:m_indent);
          if((trigger.opcode&0x1fff)==TR_OR)
          {
            if(num_or) m_errors[i]=CE_INCOMPLETE_OR;
            num_or=trigger.bytes[0];
          }
          else
          {
            if(num_or) num_or--;
          }
          break;
        case TA_IF:
          m_errors[i]=match_string(tmpstr, "IF",0,-100);
          triggeroraction=TA_TRIGGER;
          num_or=0;
          break;
        case TA_RESPONSE:
          m_errors[i]=match_string(tmpstr, "RESPONSE #%d",m_indent,-110);
          triggeroraction=TA_ACTION;
          break;
        }
        if(m_errors[i])
        {
          if(m_firsterror==-1) m_firsterror=i;
        }
      }
      else m_errors[i]=0;
      if(commentpos>=0) line=tmpstr+line.Mid(commentpos);
      else line=tmpstr;
    }
  }
  if(messages&SHOW_MESSAGES)
  {
    if(m_firsterror>=0)
    {
      for(i=m_firsterror;i<endline;i++)
      {
        if(m_errors[i])
        {
          tmpstr.Format("Error in script line #%d: %s",i+1,get_compile_error(m_errors[i]) );
          if(MessageBox(tmpstr,"Dialog editor",MB_ICONEXCLAMATION|MB_OKCANCEL)==IDCANCEL)
          {
            m_firsterror=i; //positioning to the cancelled line
            break;
          }
        }
      }
    }
  }
}

void CScriptEdit::OnKillfocusText() 
{
  UpdateData(UD_RETRIEVE);
}

void CScriptEdit::OnSetfocusText() 
{
  if(!m_firsterror) CheckScript(0);
  RefreshDialog();	
}

void CScriptEdit::OnCheck() 
{
  int startchar, linelength;

  CheckScript(WHOLE_CHECK|FORCED_CHECK|SHOW_MESSAGES);
  if(m_firsterror>=0)
  {
    m_text_control.SetFocus();
    startchar=m_text_control.LineIndex(m_firsterror);
    if(!(editflg&SELECTION))
    {
      linelength=m_text_control.LineLength(startchar);
      m_text_control.SetSel(startchar, startchar+linelength);
    }
    else
    {
      m_text_control.SetSel(startchar, startchar);
    }
  }
  else MessageBox("The script appears correct.","Script editor",MB_OK);
}

void CScriptEdit::OnChangeText() 
{
  CWnd *cwnd;

  cwnd=GetFocus();
  //make sure that the edit window has the input focus, otherwise this could
  //cause various unexpected bugs
//  if(!cwnd || (m_text_control.GetSafeHwnd()!=cwnd->GetSafeHwnd()) ) return;
  if(!cwnd || (cwnd->GetDlgCtrlID()!=IDC_TEXT) ) return;
  UpdateData(UD_RETRIEVE);
  if(m_text_control.GetModify())
  {
    the_script.m_changed=true;
  }
  CheckScript(FORCED_CHECK);
  RefreshDialog();
}

void CScriptEdit::PostNcDestroy() 
{
	if(m_errors) delete [] m_errors;
	m_errors=NULL;
	CDialog::PostNcDestroy();
}

void CScriptEdit::OnFileLoadsource() 
{
	m_bcs=0;
	OnLoadex();
}

void CScriptEdit::OnMaxtextText() 
{
	MessageBox("Script is too long, sorry.","Warning",MB_ICONEXCLAMATION|MB_OK);	
}

void CScriptEdit::OnSearch() 
{
  CString tmpstr;

  if(!m_searchdlg)
  {
    m_searchdlg=new CFindReplaceDialog;

    if(!m_searchdlg)
    {
      MessageBox("Insufficient memory","Error",MB_ICONEXCLAMATION|MB_OK);
      return;
    }
    tmpstr=m_text_control.GetSelText();
    tmpstr.Replace("\r\n","\n");
    m_searchdlg->Create(readonly,tmpstr,NULL, FR_DOWN | FR_MATCHCASE, this);
  }
}

long CScriptEdit::OnFindReplace(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
  int direction;
  int mode; //0 - search, 1 - replace current, 2 - replace all
  int match;
  CString search;
  CString replace;

  if(!m_searchdlg) return 0;
  if(m_searchdlg->IsTerminating() )
  {
    m_searchdlg=NULL;
    return 0;
  }
  if(m_infind) return 0;

  m_infind=1;
  direction=m_searchdlg->SearchDown();
  mode=!!m_searchdlg->ReplaceCurrent()+(!!m_searchdlg->ReplaceAll())*2;
  //matchcase is cheaper, therefore it is marked 0
  match=!m_searchdlg->MatchCase()+(!!m_searchdlg->MatchWholeWord())*2; 
  search=m_searchdlg->GetFindString();
  replace=m_searchdlg->GetReplaceString();
  do_search_and_replace(direction,mode,match,search,replace);
  m_infind=0;
  OnSetfocusText();
  return 0;
}

int CScriptEdit::perform_search_and_replace(int idx, int mode, int match, CString search, CString replace)
{
  int ch;
  int found;
  int len;
  int tmplen, tmpstart;
  CString tmp2;
  CString line;

  line=GetLine(idx);
  ch=m_text_control.LineIndex(idx);
  len=m_text_control.LineLength(ch);
  found=false;
  switch(mode)
  {
  case SR_SEARCH:
    tmp2=line;
    if(match&1) // case insensitive trick
    {
      tmp2.MakeLower();
    }
    if(match&2) // full word trick
    {
      tmp2=" "+line+" "; //in this
    }
    tmplen=tmp2.Find(search);
    if(tmplen!=-1)
    {
      m_text_control.SetSel(ch+tmplen,ch+tmplen+search.GetLength());
      found=true;
    }
    break;
  case SR_REPLACE: case SR_ALL:
    tmplen=line.GetLength();
    if(match&2)
    {
      line=" "+line+" ";
    }
    if(match&1)
    {
      tmp2=line;
      tmp2.MakeLower();
      tmpstart=tmp2.Find(search);
      if(tmpstart==-1) ch=0;
      else
      {
        line.Delete(tmpstart,len);
        line.Insert(tmpstart,replace);
        found=true;        
      }
    }
    else
    {
      found=line.Replace(search,replace);
    }
    if(match&2)
    {
      line=line.Mid(1,tmplen);
    }
    if(found)
    {
      m_text_control.SetSel(ch,ch+len);
      m_text_control.ReplaceSel(line,true);
      found=true;
      UpdateData(UD_DISPLAY);
    }
    break;
  }
  return found;
}

void CScriptEdit::do_search_and_replace(int direction,int mode,int match,CString search,CString replace)
{
  CString what;
  CString operation;
  int linecount;
  int idx, oldidx;
  int count;
  int found;

  idx=m_text_control.LineFromChar(-1);
  linecount=m_text_control.GetLineCount();
  if(!linecount) return;
  if(idx<0 || idx>=linecount)
  {
    idx=0;
  }
  oldidx=idx;

  if(match&1) //we can do this here
  {
    search.MakeLower();
  }
  if(match&2) //this too
  {
    search=" "+search+" ";
  }

  found=-1;
  count=0;
  if(direction)
  {
    direction=1;
    what="bottom";
  }
  else
  {
    direction=-1;
    what="top";
  }
  switch(mode)
  {
  case SR_SEARCH: operation="Search"; break; //0
  case SR_REPLACE: operation="Search & replace"; break; //1
  case SR_ALL: operation="Replace all"; break; //2
  }  
  if(!mode)
  {
    idx+=direction;
  }
  while(direction<1? idx : idx<linecount)
  {
    if(perform_search_and_replace(idx,mode,match,search,replace))
    {
      found=idx;
      count++;
    }
    if(found==-1)
    {//we found nothing
      if(mode==1) mode=0; //couldn't find anything in the actual position, we will search from now
    }
    else
    {//we found something, if it is not replace all then we stop here
      if(mode!=2) break;
    }
    idx+=direction;
  }
  if((found==-1) || (mode==2)) //replace all should go around even if it has found something
  {
    if(MessageBox(operation+" hit the "+what+" of the list, do you want to continue ?",operation,MB_SYSTEMMODAL|MB_YESNO)==IDNO)
    {
      return;
    }
    
    if(direction>0)
    {
      idx=0;
    }
    else
    {
      idx=linecount-1;
    }
    while(idx!=oldidx)
    {
      if(perform_search_and_replace(idx,mode,match,search,replace))
      {
        found=idx;
        count++;
      }
      if(found==-1)
      {//we found nothing
        if(mode==1) mode=0; //couldn't find anything in the actual position, we will search from now
      }
      else
      {//we found something, if it is not replace all then we stop here
        if(mode!=2) break;
      }
      idx+=direction;
    }
  }
  if(found!=-1)
  {
    if(count>1)
    {
      operation.Format("Found %d entries.",count);
      MessageBox(operation);
    }
  }
  else
  {
    MessageBox("Couldn't find any entries containing: "+search);
  }
  return;
}

void CScriptEdit::OnOptionsAutocheck() 
{
	editflg^=NOCHECK;
	RefreshMenu();
}

void CScriptEdit::OnOptionsLargeindent() 
{
	editflg^=INDENT;
	RefreshMenu();
}

void CScriptEdit::OnOptionsUseinternaldecompiler() 
{
	editflg^=INTERNALDECOMP;
	RefreshMenu();
}

void CScriptEdit::OnOptionsUseinternalcompiler() 
{
	editflg^=INTERNALCOMPILER;
	RefreshMenu();
}

void CScriptEdit::OnOptionsLogging() 
{
	weiduflg^=WEI_LOGGING;
	RefreshMenu();
}

void CScriptEdit::OnEditSelectall() 
{
	m_text_control.SetSel(0,-1);	
}

void CScriptEdit::OnEditUndo() 
{
	m_text_control.Undo();	
}

void CScriptEdit::OnEditCut() 
{
  m_text_control.Cut();	
}

void CScriptEdit::OnEditCopy() 
{
	m_text_control.Copy();	
}

void CScriptEdit::OnEditPaste() 
{
	m_text_control.Paste();
}

void CScriptEdit::OnEditDelete() 
{
  m_text_control.Clear();
}

#define EDITMENU 1
void CScriptEdit::OnContextMenu(CWnd* pWnd, CPoint point) 
{
  if(pWnd->GetDlgCtrlID()==IDC_TEXT)
  {
    CMenu *menu;
    CMenu *popupmenu;
    
    menu=GetMenu();    
    popupmenu=menu->GetSubMenu(EDITMENU); //this should be better defined
    popupmenu->TrackPopupMenu(TPM_CENTERALIGN,point.x,point.y,this);   
  }
}

void CScriptEdit::OnIDS() 
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
 
BOOL CScriptEdit::OnHelpInfo(HELPINFO* /*pHelpInfo*/) 
{
  CString tmpstr;
  CString helpstr;
  compiler_data compiler_data;

  tmpstr=m_text_control.GetSelText();
  tmpstr.MakeLower();
  tmpstr.TrimLeft();
  tmpstr.TrimRight();
	if(action_data.Lookup(tmpstr,compiler_data) )
  {
    helpstr.Format("%s(%s)",tmpstr,format_parameters(compiler_data));
    MessageBox(helpstr,"Script Action",MB_ICONINFORMATION|MB_OK);
    return false;
  }
  if(trigger_data.Lookup(tmpstr,compiler_data) )
  {
    helpstr.Format("%s(%s)",tmpstr,format_parameters(compiler_data));
    MessageBox(helpstr,"Script Trigger",MB_ICONINFORMATION|MB_OK);
    return false;
  }
	return false;	
}

void CScriptEdit::OnUpdateText() 
{
  if(m_text_control.GetModify() )
  {
    the_script.m_changed=true;
    return; //this will be handled in onchange
  }
  OnChangeText(); //this event is called from scroll
}

void CScriptEdit::OnCancel() 
{
  CString tmpstr;

	if(the_script.m_changed)
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

void CScriptEdit::OnToolsLookupstrref() 
{
	CStrRefDlg dlg;
	
  dlg.DoModal();
	RefreshMenu();
}

BOOL CScriptEdit::PreTranslateMessage(MSG* pMsg) 
{
	m_tooltip.RelayEvent(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}
