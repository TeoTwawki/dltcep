// StrRefDlg.cpp : implementation file
//

#include "stdafx.h"
#include "chitem.h"
#include "chitemDlg.h"
#include "StrRefDlg.h"
#include "ItemPicker.h"
#include "massclear.h"
#include "options.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStrRefDlg dialog


CStrRefDlg::CStrRefDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CStrRefDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CStrRefDlg)
	m_text = _T("");
	m_sound = _T("");
	m_tag = FALSE;
	m_maxstr = _T("");
	//}}AFX_DATA_INIT
  m_strref=0;
  m_searchdlg=NULL;
  m_mode=0;
  m_refresh=false;
}

void CStrRefDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
  m_maxstr.Format("/ %d",tlk_headerinfo[0].entrynum);
	//{{AFX_DATA_MAP(CStrRefDlg)
	DDX_Text(pDX, IDC_SOUND, m_sound);
	DDV_MaxChars(pDX, m_sound, 8);
	DDX_Check(pDX, IDC_TAG, m_tag);
	DDX_Text(pDX, IDC_MAX, m_maxstr);
	DDV_MaxChars(pDX, m_maxstr, 10);
	DDX_Control(pDX, IDC_TEXT, m_text_control);
	DDX_Text(pDX, IDC_TEXT, m_text);
	DDX_Control(pDX, IDC_SPIN, m_spin_control);
	DDX_Control(pDX, IDC_STRREF, m_strref_control);
	//}}AFX_DATA_MAP
  m_spin_control.SetRange32(0,tlk_headerinfo[0].entrynum);
}

BEGIN_MESSAGE_MAP(CStrRefDlg, CDialog)
	//{{AFX_MSG_MAP(CStrRefDlg)
	ON_EN_UPDATE(IDC_STRREF, OnUpdateStrref)
	ON_EN_KILLFOCUS(IDC_TEXT, OnKillfocusText)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_EN_KILLFOCUS(IDC_SOUND, OnKillfocusSound)
	ON_BN_CLICKED(IDC_TAG, OnTag)
	ON_BN_CLICKED(IDC_SEARCH, OnSearch)
	ON_BN_CLICKED(IDC_PLAY, OnPlay)
	ON_BN_CLICKED(IDC_FIND, OnFind)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_RELOAD, OnReload)
	ON_COMMAND(ID_TRIM, OnTrim)
	ON_COMMAND(ID_CLEAR, OnClear)
	ON_EN_KILLFOCUS(IDC_STRREF, OnKillfocusStrref)
	ON_COMMAND(ID_CHECKSOUND, OnChecksound)
	ON_COMMAND(ID_CHECKTAG, OnChecktag)
	//}}AFX_MSG_MAP
  ON_REGISTERED_MESSAGE( WM_FINDREPLACE, OnFindReplace )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStrRefDlg message handlers

void CStrRefDlg::OnUpdateStrref() 
{
  CString strref;

  if(!m_refresh) return;
  GetDlgItem(IDC_STRREF)->GetWindowText(strref);
  m_strref=strtonum(strref);
  m_tag=resolve_tlk_tag(m_strref);
  m_text=resolve_tlk_text(m_strref);
  m_sound=resolve_tlk_soundref(m_strref);
  strref="Edit TLK ";
  if(global_changed[0]==true) strref+="*";
  SetWindowText(strref);
  UpdateData(UD_DISPLAY);
}

void CStrRefDlg::OnKillfocusText() 
{
  CString old;

	UpdateData(UD_RETRIEVE);
  old=resolve_tlk_text(m_strref);
  if(old==m_text) return;
  if(editflg&TLKCHANGE)
  {
    if(MessageBox("Do you want to update dialog.tlk?","TLK editor",MB_ICONQUESTION|MB_YESNO)!=IDYES)
    {
      goto end;
    }
  }

  if(tlk_headerinfo[0].entrynum==-1)
  {
    memcpy(tlk_headerinfo[0].signature,"TLK V1  ",8);
    tlk_headerinfo[0].entrynum=0;
    store_tlk_text(0,"<NO TEXT>");
    m_strref=1;
  }
	if(m_strref>0 && m_strref<=tlk_headerinfo[0].entrynum) //allow = so we can add strings
  {
    m_strref=store_tlk_text(m_strref, m_text);
  }
end:
  old.Format("%d",m_strref);
  GetDlgItem(IDC_STRREF)->SetWindowText(old);
  UpdateData(UD_DISPLAY);
}

void CStrRefDlg::OnKillfocusSound() 
{
  CString old;

	UpdateData(UD_RETRIEVE);
  old=resolve_tlk_soundref(m_strref);
  if(old==m_sound) return;
  if(editflg&TLKCHANGE)
  {
    if(MessageBox("Do you want to update dialog.tlk?","TLK editor",MB_ICONQUESTION|MB_YESNO)!=IDYES)
    {
      return;
    }
  }

	if(m_strref>0 && m_strref<tlk_headerinfo[0].entrynum)
  {
    m_strref=store_tlk_soundref(m_strref, m_sound);
  }
}

void CStrRefDlg::OnTag() 
{
  if(editflg&TLKCHANGE)
  {
    if(MessageBox("Do you want to update dialog.tlk?","TLK editor",MB_ICONQUESTION|MB_YESNO)!=IDYES)
    {
      return;
    }
  }

	if(m_strref>0 && m_strref<tlk_headerinfo[0].entrynum)
  {
    toggle_tlk_tag(m_strref);	
  }
  m_tag=resolve_tlk_tag(m_strref);
}

void CStrRefDlg::OnPlay() 
{
	play_acm(m_sound,false,false);	
}

void CStrRefDlg::OnBrowse() 
{
  pickerdlg.m_restype=REF_WAV;
  pickerdlg.m_picked=m_sound;
  if(pickerdlg.DoModal()==IDOK)
  {
    m_sound=pickerdlg.m_picked;
  	UpdateData(UD_DISPLAY);
    OnKillfocusSound();
  }
}

void CStrRefDlg::StartFindReplace(int mode)
{
  int a,b;
    
  if(!m_searchdlg)
  {
    m_mode=mode;
    m_searchdlg=new CFindReplaceDialog;

    if(!m_searchdlg)
    {
      MessageBox("Not enough memory.","Error",MB_ICONEXCLAMATION|MB_OK);
      return;
    }
    m_text_control.GetSel(a,b);
    if(mode)
    {
      m_searchdlg->Create(readonly,m_text.Mid(a,b-a),NULL, FR_DOWN | FR_MATCHCASE, this);
    }
    else
    {
      m_searchdlg->Create(readonly,m_sound,NULL, FR_DOWN | FR_HIDEMATCHCASE|FR_HIDEWHOLEWORD, this);
    }
  }
}

void CStrRefDlg::OnFind() 
{
	StartFindReplace(0);
}

void CStrRefDlg::OnSearch() 
{
  StartFindReplace(1);
}

long CStrRefDlg::OnFindReplace(WPARAM /*wParam*/, LPARAM /*lParam*/)
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
  direction=m_searchdlg->SearchDown();
  mode=!!m_searchdlg->ReplaceCurrent()+(!!m_searchdlg->ReplaceAll())*2;
  //matchcase is cheaper, therefore it is marked 0
  match=!m_searchdlg->MatchCase()+(!!m_searchdlg->MatchWholeWord())*2; 
  search=m_searchdlg->GetFindString();
  replace=m_searchdlg->GetReplaceString();
  do_search_and_replace(direction,mode,match,search,replace);
  return 0;
}

int perform_search_and_replace_sound(int idx, int mode, CString search, CString replace)
{
  CString tmpstr;
  int tmpstart;

  RetrieveResref(tmpstr,tlk_entries[0][idx].reference.soundref);
  tmpstart=tmpstr.Find(search);
  if(tmpstart!=-1)
  {
    if(mode)
    {
      tmpstr.Replace(search,replace);
      StoreResref(tmpstr,tlk_entries[0][idx].reference.soundref);
      global_changed[0]=true;
    }
    return true;
  }
  return false;
}

int perform_search_and_replace(int idx, int mode, int match, CString search, CString replace, int stringmode)
{
  int ch;
  int found;
  int len;
  int tmplen, tmpstart;
  CString tmp2;
  CString srch;
  
  if(!stringmode)
  {
    return perform_search_and_replace_sound(idx, mode,search,replace);
  }
  found=false;
  srch=search;
  len=srch.GetLength();
  switch(mode)
  {
  case SR_SEARCH:
    tmp2=tlk_entries[0][idx].text;
    if(match&1) // case insensitive trick
    {
      tmp2.MakeLower();
    }
    if(match&2) // full word trick
    {
      tmp2=" "+tlk_entries[0][idx].text+" "; //in this
    }
    if(tmp2.Find(srch) !=-1) found=true;
    break;
  case SR_REPLACE: case SR_ALL:
    tmplen=tlk_entries[0][idx].text.GetLength();
    if(match&2)
    {
      tlk_entries[0][idx].text=" "+tlk_entries[0][idx].text+" ";
    }
    if(match&1)
    {
      tmp2=tlk_entries[0][idx].text;
      tmp2.MakeLower();
      tmpstart=tmp2.Find(srch);
      if(tmpstart==-1) ch=0;
      else
      {
        tlk_entries[0][idx].text.Delete(tmpstart,len);
        tlk_entries[0][idx].text.Insert(tmpstart,replace);
        ch=true;        
      }
    }
    else
    {
      ch=tlk_entries[0][idx].text.Replace(srch,replace);
    }
    if(match&2)
    {
      tlk_entries[0][idx].text=tlk_entries[0][idx].text.Mid(1,tmplen);
    }
    if(ch)
    {
      tlk_entries[0][idx].reference.length=tlk_entries[0][idx].text.GetLength();
      if(!tlk_entries[0][idx].reference.length)
      {
        if(tlk_entries[0][idx].reference.soundref[0]) tlk_entries[0][idx].reference.flags=2; //only sound reference
        else
        {
          tlk_entries[0][idx].reference.flags=0; //deleted entry
          tlk_entries[0][idx].reference.pitch=0;
          tlk_entries[0][idx].reference.volume=0;
          memset(tlk_entries[0][idx].reference.soundref,0,sizeof(resref_t));
        }         
      }
      global_changed[0]=true;
      found=true;
    }
    break;
  }
  return found;
}

int CStrRefDlg::do_search_and_replace(int direction,int mode,int match,CString search,CString replace)
{
  int found, count;
  int oldidx, idx;
  CString operation;
  CString what;
  
  idx=m_strref;
  if(idx<0 || idx>=tlk_headerinfo[0].entrynum)
  {
    idx=0;
  }
  if(!tlk_headerinfo[0].entrynum) return -1;
  oldidx=idx;

  if(m_mode) //search for string
  {
    if(match&1) //we can do this here
    {
      search.MakeLower();
    }
    if(match&2) //this too
    {
      search=" "+search+" ";
    }
  }
  else search.MakeUpper(); //search for soundref

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
  while(direction<1? idx : idx<tlk_headerinfo[0].entrynum)
  {
    if(perform_search_and_replace(idx,mode,match,search,replace,m_mode))
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
    if(MessageBox(operation+" hit the "+what+" of the list, do you want to continue?",operation,MB_YESNO)==IDNO)
    {
      return found;
    }
    
    if(direction>0)
    {
      idx=0;
    }
    else
    {
      idx=tlk_headerinfo[0].entrynum-1;
    }
    while(idx!=oldidx)
    {
      if(perform_search_and_replace(idx,mode,match,search,replace,m_mode))
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
    m_strref=found;
    what.Format("%d",m_strref);
    m_strref_control.SetWindowText(what);
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
  return found;
}

BOOL CStrRefDlg::OnInitDialog() 
{
  CString tmpstr, tmpstr1, tmpstr2;

	CDialog::OnInitDialog();
	
  //tooltips
  {
    m_tooltip.Create(this,TTS_NOPREFIX);
    m_tooltip.SetMaxTipWidth(200);
    m_tooltip.SetTipBkColor(RGB(240,224,160));
    
    m_tooltip.AddTool(GetDlgItem(IDCANCEL), IDS_CANCEL);
  }
  m_refresh=true;
  m_strref_control.SetWindowText("0");
	return TRUE;
}

void CStrRefDlg::OnSave() 
{
	((CChitemDlg *) AfxGetMainWnd())->write_file_progress(0);
  if(global_changed[0]==true)
  {
    MessageBox(((CChitemDlg *) AfxGetMainWnd())->GetTlkFileName(0)+" wasn't saved, exit all other programs that might use it, and try again.","Warning",MB_ICONEXCLAMATION|MB_OK);
  }
	((CChitemDlg *) AfxGetMainWnd())->write_file_progress(1);
  if(global_changed[1]==true)
  {
    MessageBox(((CChitemDlg *) AfxGetMainWnd())->GetTlkFileName(1)+" wasn't saved, exit all other programs that might use it, and try again.","Warning",MB_ICONEXCLAMATION|MB_OK);
  }
  OnUpdateStrref();
}

void CStrRefDlg::OnReload() 
{
	((CChitemDlg *) AfxGetMainWnd())->rescan_dialog(false);
	OnUpdateStrref();
}

void CStrRefDlg::OnTrim() 
{
  CString tmpstr;

	OnKillfocusStrref();
  if(tlk_headerinfo[0].entrynum<1)
  {
    MessageBox("Can't truncate an empty file.","Warning",MB_OK);
    return;
  }
  tmpstr.Format("Do you really want to remove all entries above #%ld.",m_strref);
  if(MessageBox(tmpstr,"Truncate",MB_YESNO)==IDYES)
  {
    if(truncate_references(m_strref+1,0)) global_changed[0]=true;
    if(truncate_references(m_strref+1,1)) global_changed[1]=true;
  	UpdateData(UD_DISPLAY);
  }
}

void CStrRefDlg::OnClear() 
{
	massclear mscDlg;
	
  OnKillfocusStrref();
  mscDlg.SetRange(1,tlk_headerinfo[0].entrynum-1, m_strref);
  mscDlg.SetText("Clearing an interval of entries");  
  if(mscDlg.DoModal()==IDOK)
  {
    while(mscDlg.m_from<=mscDlg.m_to)
    {
      store_tlk_soundref(mscDlg.m_from,"");
      store_tlk_text(mscDlg.m_from++,"*!*");
    }
  }
}

void CStrRefDlg::OnKillfocusStrref() 
{
  CString old;

	if(m_strref>tlk_headerinfo[0].entrynum)
  {
    m_strref=0;
  }
  old.Format("%d",m_strref);
  GetDlgItem(IDC_STRREF)->SetWindowText(old);
  UpdateData(UD_DISPLAY);
}

void CStrRefDlg::OnChecksound() 
{
  loc_entry tmploc;
  CString tmpref,tmpstr;
	int i;
  int ret;
  int chg;

  chg=0;
  for(i=0;i<tlk_headerinfo[0].entrynum;i++)
  {
    tmpref=resolve_tlk_soundref(i);
    if(tmpref.IsEmpty())
    {
      tlk_entries[0][i].reference.pitch=tlk_entries[0][i].reference.volume=0;
      if(tlk_entries[0][i].text.IsEmpty())
      {
        if(tlk_entries[0][i].reference.flags)
        {
          chg=1;
          tlk_entries[0][i].reference.flags=0;
        }
      }
      continue;
    }
    else
    {
      if(tlk_entries[0][i].reference.flags&2)
      {
        chg=1;
        tlk_entries[0][i].reference.flags|=2;
      }
    }

    if(!sounds.Lookup(tmpref, tmploc) )
    {
      tmpstr.Format("%d",i);
      GetDlgItem(IDC_STRREF)->SetWindowText(tmpstr);
      UpdateData(UD_DISPLAY);
      tmpstr.Format("Missing sound reference: %s, do you want to clear it?",tmpref);
      ret=MessageBox(tmpstr,"Tlk editor",MB_ICONEXCLAMATION|MB_YESNOCANCEL);
      if(ret==IDCANCEL) return;
      if(ret==IDYES)
      {
        chg=1;
        store_tlk_soundref(i,"");
      }
    }
  }
  if(!chg) MessageBox("No problem found!","Tlk editor",MB_OK);
}

void CStrRefDlg::OnChecktag() 
{
	int i;
  int chg;

  chg=0;
  for(i=0;i<tlk_headerinfo[0].entrynum;i++)
  {
  }	
  if(!chg) MessageBox("No problem found!","Tlk editor",MB_OK);
}

BOOL CStrRefDlg::PreTranslateMessage(MSG* pMsg) 
{
	m_tooltip.RelayEvent(pMsg);
  return CDialog::PreTranslateMessage(pMsg);
}
