// CFBDialog.cpp : implementation file
//

#include "stdafx.h"

#include <fcntl.h>
#include <direct.h>
#include <sys/stat.h>
#include <shlobj.h>
#include "chitem.h"
#include "CFBDialog.h"
#include "MyFileDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCFBDialog dialog


CCFBDialog::CCFBDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CCFBDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCFBDialog)
	m_etitle = _T("");
	m_event = _T("");
	//}}AFX_DATA_INIT
  m_output=bgfolder+"cfb";
}

BOOL CCFBDialog::OnInitDialog() 
{
  CString tmpstr;

	CDialog::OnInitDialog();
  tmpstr.Format("%s all %s in the override folder",(mode&1)?"Add CFB to":"Remove CFB from",(mode&2)?"items":"spells");
  SetWindowText(tmpstr);

  m_outputbutton.SetBitmap(theApp.m_bbmp);
  {
    m_tooltip.Create(this,TTS_NOPREFIX);
    m_tooltip.SetMaxTipWidth(200);
    m_tooltip.SetTipBkColor(RGB(240,224,160));
    
    tmpstr.LoadString(IDS_CFBHELP); //too long string
    m_tooltip.AddTool(GetDlgItem(IDC_FILENAME), tmpstr);
    m_tooltip.AddTool(GetDlgItem(IDC_OUTFOLDER), IDS_CFBFOLDER);
    m_tooltip.AddTool(GetDlgItem(IDC_OUTPUT), IDS_CFBFOLDER);
    m_tooltip.AddTool(GetDlgItem(IDC_OPENFILE), IDS_CFBOPEN);
    m_tooltip.AddTool(GetDlgItem(IDC_EVENT), IDS_EVENT);
  }

	return TRUE;
}

void CCFBDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCFBDialog)
	DDX_Control(pDX, IDC_OUTPUT, m_outputbutton);
	DDX_Control(pDX, IDC_EVENT, m_event_control);
	DDX_Text(pDX, IDC_EVENT_TYPE, m_etitle);
	DDX_Text(pDX, IDC_EVENT, m_event);
	DDX_Text(pDX, IDC_OUTFOLDER, m_output);
	DDV_MaxChars(pDX, m_output, 200);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCFBDialog, CDialog)
	//{{AFX_MSG_MAP(CCFBDialog)
	ON_BN_CLICKED(IDC_OPENFILE, OnOpenfile)
	ON_WM_CONTEXTMENU()
	ON_WM_HELPINFO()
	ON_EN_KILLFOCUS(IDC_OUTFOLDER, OnKillfocusOutfolder)
	ON_BN_CLICKED(IDC_OUTPUT, OnOutput)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCFBDialog message handlers

void CCFBDialog::log(CString format, ...)
{
  int x;
  va_list arglist;
  CString str;
  
  if(newitem)
  {
    newitem=FALSE;
    log(itemname);
  }
  m_etitle="Event log:";
  m_event_control.SetReadOnly(true);
  if(format)
  {
    va_start(arglist, format);
    str.FormatV(format,arglist);
    va_end(arglist);
    if(m_event.IsEmpty())
    {
      m_event=str;
      x=0;
    }
    else
    {      
      x=m_event_control.LineIndex(MAX_LINE);
      if(x>-1)
      {
        m_event=m_event.Right(m_event.GetLength()-x);
      }
      m_event+=CString("\r\n")+str;
    }
    UpdateData(UD_DISPLAY);
    m_event_control.LineScroll(MAX_LINE);
  }
}

void CCFBDialog::OnOpenfile() 
{
  POSITION pos;
  CString tmpstr;
  CString key;
  loc_entry fileloc;
  int featcount, featnum;
  feat_block *cfb;
  int flg, ret;
  int fhandle;
  int ext;

  newitem=FALSE;
  mkdir(m_output);
  log("%s",m_output+" created");

  if(mode&2) ext=determinetype(REF_ITM);
  else ext=determinetype(REF_SPL);
  if(mode&1)
  {
    flg=OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|OFN_ENABLESIZING|OFN_EXPLORER;
    CMyFileDialog m_getfiledlg(TRUE, "bin", bgfolder+"cfb.bin", flg, cfbFilter);
  
  restart:
    if( m_getfiledlg.DoModal() == IDOK )
    {
      tmpstr=m_getfiledlg.GetPathName();
      fhandle=open(tmpstr,O_RDONLY|O_BINARY);
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
    }
    else return;
    read(fhandle,cfb,featcount);
    close(fhandle);
  }

  pos=resources[ext]->GetStartPosition();
  while(pos)
  {
    resources[ext]->GetNextAssoc(pos,key,fileloc);    
    if(fileloc.bifindex!=0xffff)
    {
      continue;
    }    
    changeitemname(key);
    fhandle=locate_file(fileloc, 0);
    if(fhandle<1)
    {
      log("Cannot open file.");
      continue;
    }
    if(mode&2) ret=the_item.ReadItemFromFile(fhandle, fileloc.size);
    else ret=the_spell.ReadSpellFromFile(fhandle, fileloc.size);
    close(fhandle);
    if(ret)
    {
      log("Invalid file.");
      continue;
    }    
    //featnum is used only when mode&1, in this case it gets initialized
    if(ModifyCFB(mode, featnum, cfb))
    {
      log("No CFB... skipping file");
      continue;
    }
    if(mode&2) tmpstr=".itm";
    else tmpstr=".spl";
    fhandle=open(m_output+"/"+key+tmpstr,O_RDWR|O_BINARY|O_TRUNC|O_CREAT,S_IREAD|S_IWRITE);
    if(mode&2) the_item.WriteItemToFile(fhandle,0);
    else the_spell.WriteSpellToFile(fhandle,0);
    close(fhandle);
    log("Done.");
  }
  //cfb usually gets initialized once if mode==true and 
  //execution doesn't reach this point if it was not
  //(the warning of uninitialized local is unwarranted)
  if(mode&1) delete [] cfb;
}

void CCFBDialog::OnContextMenu(CWnd* pWnd, CPoint point) 
{
  CRect tmp;
  
  GetClientRect(tmp);
  ClientToScreen(tmp);
  
  if(tmp.PtInRect(point) )
  {
    PostMessage(WM_SYSCOMMAND,SC_CONTEXTHELP,0);
  }
  else CDialog::OnContextMenu(pWnd, point);
}

BOOL CCFBDialog::OnHelpInfo(HELPINFO* pHelpInfo) 
{
  CString helpstring;
  
  if(pHelpInfo->iContextType!=HELPINFO_WINDOW) return false;
  switch(pHelpInfo->iCtrlId)
  {
  case IDC_OUTPUT: case IDC_OUTFOLDER:
    helpstring.LoadString(IDS_CFBFOLDER);
    break;
  case IDC_FILENAME:
    helpstring.LoadString(IDS_CFBHELP);
    break;
  case IDC_OPENFILE:
    helpstring.LoadString(IDS_CFBOPEN);
    break;
  case IDC_EVENT:
    helpstring.LoadString(IDS_EVENT);
    break;
  default:
    helpstring="?";
    break;
  }
  
  MessageBox(helpstring,"Help",MB_ICONINFORMATION|MB_OK);
  return true;
}

void CCFBDialog::OnKillfocusOutfolder() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CCFBDialog::OnOutput() 
{
  folderbrowse_t fb;

  fb.initial=m_output;
  fb.title="Select output folder for CFB";
  if(BrowseForFolder(&fb, m_hWnd)==IDOK)
  {
    m_output=fb.initial;
    UpdateData(UD_DISPLAY);
  }
}

BOOL CCFBDialog::PreTranslateMessage(MSG* pMsg)
{
  m_tooltip.RelayEvent(pMsg);
  return CDialog::PreTranslateMessage(pMsg);
}
