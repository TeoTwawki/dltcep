// FileExtract.cpp : implementation file
//

#include "stdafx.h"

#include <fcntl.h>
#include <sys/stat.h>
#include "chitem.h"
#include "FileExtract.h"
#include "zlib.h"
#include "MyFileDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileExtract dialog

CFileExtract::CFileExtract(CWnd* pParent /*=NULL*/)
	: CDialog(CFileExtract::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFileExtract)
	m_filemask = _T("");
	m_override = FALSE;
	//}}AFX_DATA_INIT
  m_bifidx=0xffff;
  extract_or_search=false; //extract=true
}

void CFileExtract::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFileExtract)
	DDX_Control(pDX, IDC_BIFNAME, m_bifname_control);
	DDX_Control(pDX, IDC_FILETYPE, m_filetype_control);
	DDX_Text(pDX, IDC_FILEMASK, m_filemask);
	DDV_MaxChars(pDX, m_filemask, 8);
	DDX_Check(pDX, IDC_OVERRIDE, m_override);
	//}}AFX_DATA_MAP
}

BOOL CFileExtract::OnInitDialog() 
{
  CWnd *button;
  int i;

	CDialog::OnInitDialog();
  button=GetDlgItem(IDOK);
  if(extract_or_search)
  {
    SetWindowText("Extracting from bifs");
    button->SetWindowText("Extract");
  }
  else
  {
    SetWindowText("Searching in bifs");
    button->SetWindowText("Search");
  }
  m_filetype=0;
	m_filetype_control.SetCurSel(0);
  m_bifname_control.ResetContent();
  for(i=0;i<key_headerinfo.numbif;i++)
  {
    m_bifname_control.AddString(bifs[i].bifname);
  }
  m_bifname_control.AddString("");
  m_filetype_control.ResetContent();
  for(i=0;i<=NUM_OBJTYPE;i++)
  {
    m_filetype_control.AddString(objexts[i]);
  }
  //tooltips
  {
    m_tooltip.Create(this,TTS_NOPREFIX);
    m_tooltip.SetMaxTipWidth(200);
    m_tooltip.SetTipBkColor(RGB(240,224,160));
    
    m_tooltip.AddTool(GetDlgItem(IDCANCEL), IDS_CANCEL);
    m_tooltip.AddTool(GetDlgItem(IDC_BIFNAME), IDS_BIF);
    m_tooltip.AddTool(GetDlgItem(IDC_FILEMASK), IDS_FILEMASK);
    m_tooltip.AddTool(GetDlgItem(IDC_FILETYPE), IDS_FILEEXT);
    m_tooltip.AddTool(GetDlgItem(IDC_OVERRIDE), IDS_OVERRIDE);
    if(extract_or_search)
    {
      m_tooltip.AddTool(GetDlgItem(IDOK), IDS_EXTRACT);
    }
    else
    {
      m_tooltip.AddTool(GetDlgItem(IDOK), IDS_SEARCH);
    }
  }
	return TRUE;
}

BEGIN_MESSAGE_MAP(CFileExtract, CDialog)
	//{{AFX_MSG_MAP(CFileExtract)
	ON_CBN_CLOSEUP(IDC_BIFNAME, OnCloseupBifname)
	ON_EN_KILLFOCUS(IDC_FILEMASK, OnKillfocusFilemask)
	ON_BN_CLICKED(IDC_OVERRIDE, OnOverride)
	ON_CBN_SELCHANGE(IDC_FILETYPE, OnSelchangeFiletype)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileExtract message handlers

void CFileExtract::do_copy_file(CString key, CString ext, loc_entry fileloc)
{
  CString filename;
  int fhandle, finput;
  int decrypt;
  int oflg;

  finput=locate_file(fileloc, LF_IGNOREOVERRIDE); //ignore override
  if(finput<1)
  {
    return;
  }
  if (fileloc.bifname=="override\\"+key+ext)
  {
    return; //extraction copy already done
  }
  filename=bgfolder+"override\\"+key+ext;
  if(m_override) oflg=O_BINARY|O_RDWR|O_TRUNC|O_CREAT|O_SEQUENTIAL;
  else oflg=O_BINARY|O_RDWR|O_EXCL|O_CREAT|O_SEQUENTIAL;
  errno=0;
  fhandle=open(filename,oflg,S_IWRITE|S_IREAD);
  if(fhandle<1)
  {
    if(errno==17)
    {
      MessageBox("File already exists: "+filename,"Warning",MB_ICONWARNING|MB_OK);
    }
    else
    {
      MessageBox("Can't create file: "+filename,"Error",MB_ICONSTOP|MB_OK);
    }
    close(finput);
    return; //error or override
  }
  if(ext==".tis")
  {
    if(!write_tis_header(fhandle, fileloc))
    {
      MessageBox("Write error.","Error",MB_ICONSTOP|MB_OK);
      return;
    }
  }
  if(ext==".2da" || ext==".ids") decrypt=1;
  else decrypt=0;
  switch(copy_file(finput, fhandle, fileloc.size, decrypt))
  {
  case 0:
    m_num_extract++;
    break;
  case -3:
    MessageBox("Out of memory","Error",MB_ICONSTOP|MB_OK);
    break;
  case -2:
    MessageBox("Read error.","Error",MB_ICONSTOP|MB_OK);
    break;
  case -1:
    MessageBox("Write error.","Error",MB_ICONSTOP|MB_OK);
    break;
  case -4:
    MessageBox("Incorrect file size.","Error",MB_ICONSTOP|MB_OK);
    break;
  default:
    MessageBox("Internal error.","Error",MB_ICONSTOP|MB_OK);
    break;
  }
  close(finput);
  close(fhandle);
}

int CFileExtract::extract_files(int restype, CString extension, CStringMapLocEntry &refs)
{
  CFileExtract2 dlg;
  loc_entry fileloc;
	POSITION pos;
  CString key;
  int res;

  if((m_filetype==restype) || !m_filetype)
  {
    pos=refs.GetStartPosition();
    while(pos)
    {
      refs.GetNextAssoc(pos,key,fileloc);
      if(fileloc.bifindex==0xffff) continue;
      if((fileloc.bifindex!=m_bifidx) && (m_bifidx!=0xffff) ) continue;
      if(!m_filemask.IsEmpty() && (key.Find(m_filemask,0)==-1) ) continue;
      if(extract_or_search)
      {
        do_copy_file(key, extension, fileloc);
      }
      else
      {
        dlg.m_text.Format("File: %s found in \nbif: %s\nDo you want to extract it?",key+extension,bifs[fileloc.bifindex].bifname);
        res=dlg.DoModal();
        if(res==IDCANCEL) return -1;
        if(res==IDYES)
        {          
          do_copy_file(key, extension, fileloc);
        }
      }
    }
  }
  return 0;
}

void CFileExtract::OnOK() 
{
  CString tmpstr;
  int i;
  int res;

  UpdateData(UD_RETRIEVE);
  m_num_extract=0;
  res=0;
  for(i=1;i<=NUM_OBJTYPE;i++)
  {
    res=extract_files(objrefs[i], objexts[i], *resources[i])|res;  //1
    if(res==-1) break;
  }
  if(m_num_extract) tmpstr.Format("Extracted %d file(s)",m_num_extract);
  else tmpstr.Format("Couldn't find any files to exact.");
  MessageBox(tmpstr,"Extract",MB_OK);
}

void CFileExtract::OnSelchangeFiletype() 
{
  int idx;

  idx=m_filetype_control.GetCurSel();
  if(idx>=0)
  {
    m_filetype=objrefs[idx];
  }
  else m_filetype=0;
}

//this is a bit complicated because we let the filenames sorted
void CFileExtract::OnCloseupBifname() 
{
  int idx;
  CString bif;

	idx=m_bifname_control.GetCurSel();
  if(idx>=0)
  {
    m_bifname_control.GetLBText(idx,bif);
    for(idx=0;idx<key_headerinfo.numbif;idx++)
    {
      if(bifs[idx].bifname==bif)
      {
        m_bifidx=(unsigned short) idx;
        return;
      }
    }
  }
  m_bifidx=0xffff;
}

void CFileExtract::OnKillfocusFilemask() 
{
	UpdateData(UD_RETRIEVE);
}

void CFileExtract::OnOverride() 
{
	UpdateData(UD_RETRIEVE);
}

BOOL CFileExtract::PreTranslateMessage(MSG* pMsg) 
{
  m_tooltip.RelayEvent(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}

/////////////////////////////////////////////////////////////////////////////
// CFileExtract2 dialog

CFileExtract2::CFileExtract2(CWnd* pParent /*=NULL*/)
	: CDialog(CFileExtract2::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFileExtract2)
	m_text = _T("");
	//}}AFX_DATA_INIT
}

void CFileExtract2::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFileExtract2)
	DDX_Text(pDX, IDC_TEXT, m_text);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFileExtract2, CDialog)
	//{{AFX_MSG_MAP(CFileExtract2)
	ON_BN_CLICKED(IDYES, OnYes)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileExtract2 message handlers

void CFileExtract2::OnYes() 
{
	EndDialog(IDYES);
}

/////////////////////////////////////////////////////////////////////////////
// CFileExtract3 dialog


CFileExtract3::CFileExtract3(CWnd* pParent /*=NULL*/)
	: CDialog(CFileExtract3::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFileExtract3)
	m_override = FALSE;
	m_filename = _T("");
	m_filemask = _T("");
	//}}AFX_DATA_INIT
  cbf_or_sav=false;  //sav=0, cbf=1
  skim_sav=false;    //skim = 1
}


void CFileExtract3::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFileExtract3)
	DDX_Control(pDX, IDC_OPENFILE, m_openfile);
	DDX_Control(pDX, IDC_FILETYPE, m_filetype_control);
	DDX_Check(pDX, IDC_OVERRIDE, m_override);
	DDX_Text(pDX, IDC_FILENAME, m_filename);
	DDV_MaxChars(pDX, m_filename, 260);
	DDX_Text(pDX, IDC_FILEMASK, m_filemask);
	DDV_MaxChars(pDX, m_filemask, 8);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFileExtract3, CDialog)
	//{{AFX_MSG_MAP(CFileExtract3)
	ON_BN_CLICKED(IDC_OPENFILE, OnOpenfile)
	ON_EN_KILLFOCUS(IDC_FILEMASK, OnKillfocusFilemask)
	ON_BN_CLICKED(IDC_OVERRIDE, OnOverride)
	ON_CBN_SELCHANGE(IDC_FILETYPE, OnSelchangeFiletype)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileExtract3 message handlers

static char BASED_CODE BASED_CODE szFilter[] = "Compressed files (*.sav;*.cbf)|*.sav;*cbf|All files (*.*)|*.*||";

void CFileExtract3::OnOpenfile() 
{
  int res;
  
  res=OFN_FILEMUSTEXIST|OFN_ENABLESIZING|OFN_EXPLORER;
  if(readonly) res|=OFN_READONLY;
  itemname="new ";
  CMyFileDialog m_getfiledlg(TRUE, cbf_or_sav?"cbf":"sav", makeitemname(cbf_or_sav?".cbf":".sav",cbf_or_sav+2), res, szFilter);
  m_getfiledlg.m_ofn.nFilterIndex = 1;

  m_getfiledlg.m_ofn.lpstrTitle="Which archive to uncompress from?";
  if( m_getfiledlg.DoModal() == IDOK )
  {
    m_filename=m_getfiledlg.GetPathName();
    if(cbf_or_sav) lastopeneddata=m_filename.Left(m_filename.ReverseFind('\\'));
    else lastopenedsave=m_filename.Left(m_filename.ReverseFind('\\'));
  }
  UpdateData(UD_DISPLAY);
}

void CFileExtract3::OnKillfocusFilemask() 
{
	UpdateData(UD_RETRIEVE);
}

void CFileExtract3::OnOverride() 
{
	UpdateData(UD_RETRIEVE);
}

void CFileExtract3::OnSelchangeFiletype() 
{
  int idx;

  idx=m_filetype_control.GetCurSel();
  if(idx>=0)
  {
    m_filetype_control.GetLBText(idx, m_filetype);
  }
  else m_filetype="";
}

void CFileExtract3::OnOK() 
{
  CString tmpstr;
  char header[8];
  int maxlen;
  int finput, fhandle;
  int ret=0;

  UpdateData(UD_RETRIEVE);
  m_num_extract=0;

  if(m_filename.IsEmpty())
  {
    MessageBox("Pick a .SAV or .CBF","Extract",MB_ICONEXCLAMATION|MB_OK);
    return;
  }
  finput=open(m_filename,O_RDONLY|O_BINARY);
  if(finput<1)
  {
    MessageBox("Cannot read file.","Extract",MB_ICONEXCLAMATION|MB_OK);
    return;
  }
  fhandle=0;

  if(read(finput,header,sizeof(header) )!=sizeof(header) )
  {
    MessageBox("Cannot read file.","Extract",MB_ICONEXCLAMATION|MB_OK);
    goto endofquest;
  }
  if(memcmp(header,"BIF ",4) && memcmp(header,"SAV ",4) )
  {
    MessageBox("Pick a .SAV or .CBF","Extract",MB_ICONEXCLAMATION|MB_OK);
    goto endofquest;
  }

  if(skim_sav)
  {
    fhandle = open(m_filename+".tmp",O_BINARY|O_RDWR|O_TRUNC|O_CREAT|O_SEQUENTIAL,
      S_IREAD|S_IWRITE);
    if(fhandle<1)
    {
      goto endofquest;
    }
    if(write(fhandle,"SAV V1.0",8)!=8)
    {
      goto endofquest;
    }
  }
  maxlen=filelength(finput)-sizeof(header);
  if(m_filetype==".*") m_filetype.Empty();
  do
  {
    if(skim_sav)
    {
      ret=remove_from_sav(m_filemask, m_filetype, finput, maxlen, fhandle);
    }
    else
    {
      ret=extract_from_cbf(m_filemask, m_filetype, finput, m_override,maxlen);
    }
    switch(ret)
    {
    case 3:
      if(MessageBox("There is some inconsistency...","Error",MB_OKCANCEL)==IDOK)
      {
        ret=0;
      }
      break;
    case 2: //overwrite or not match
      ret=0;
      break;
    case 1: //end
      break;
    case 0:
      m_num_extract++;
      break;
    case -4:
      MessageBox("Write error.","Error",MB_ICONSTOP|MB_OK);
      break;
    case -3:
      MessageBox("Not enough memory.","Error",MB_ICONSTOP|MB_OK);
      break;
    case -2:
      MessageBox("Read error.","Error",MB_ICONSTOP|MB_OK);
      break;
    default:
      MessageBox("Internal error.","Error",MB_ICONSTOP|MB_OK);
      break;
    }
  }
  while(!ret);

  if(skim_sav)
  {
    if(m_num_extract) tmpstr.Format("Copied %d file(s)",m_num_extract);
    else tmpstr.Format("Couldn't find any files to spare.");
  }
  else
  {
    if(m_num_extract) tmpstr.Format("Extracted %d file(s)",m_num_extract);
    else tmpstr.Format("Couldn't find any files to exact.");
  }
  MessageBox(tmpstr,"Extract",MB_ICONINFORMATION|MB_OK);
endofquest:
  if(fhandle>0) close(fhandle);
  close(finput);
  if(skim_sav)
  {
    if (ret==1)
    {
      rename(m_filename,m_filename+".old");
      if (!file_exists(m_filename))
      {
        rename(m_filename+".tmp",m_filename);
      }
      else
      {
        MessageBox("Couldn't rename file!","Extract",MB_ICONWARNING|MB_OK);
      }
    }
  }
}

BOOL CFileExtract3::OnInitDialog() 
{
  int i;

	CDialog::OnInitDialog();
	if(skim_sav) SetWindowText("Search&remove from .SAV");
  else SetWindowText(CString("Search&extract from ")+(cbf_or_sav?".CBF":".SAV") );
  m_openfile.SetBitmap(theApp.m_bbmp);

  m_filetype_control.ResetContent();
  if(skim_sav)
  {
    m_filetype_control.AddString(".*");
    m_filetype_control.AddString(".are");
    m_filetype_control.AddString(".sto");
    GetDlgItem(IDC_OVERRIDE)->ShowWindow(false);
    m_filetype_control.SetCurSel(0);
    return TRUE;
  }
  if(cbf_or_sav)
  {
    m_filetype_control.AddString(".*");
    m_filetype_control.AddString(".bif");
    m_filetype_control.SetCurSel(0);
    return TRUE;
  }
  for(i=0;i<=NUM_OBJTYPE;i++)
  {
    m_filetype_control.AddString(objexts[i]);
  }
  m_filetype_control.SetCurSel(0);
	return TRUE;
}
