// SRCEdit.cpp : implementation file
//

#include "stdafx.h"
#include <fcntl.h>
#include <sys/stat.h>

#include "chitem.h"
#include "options.h"
#include "SRCEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSRCEdit dialog

CSRCEdit::CSRCEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CSRCEdit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSRCEdit)
	m_text = _T("");
	m_cntstr = _T("");
	m_soundref = _T("");
	m_ref = 0;
	m_tagged = FALSE;
	//}}AFX_DATA_INIT
  the_src.m_slots=NULL;
  the_src.m_cnt=0;
}

void CSRCEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSRCEdit)
	DDX_Control(pDX, IDC_SLOTSPIN, m_spincontrol);
	DDX_Control(pDX, IDC_BUDDY, m_buddycontrol);
	DDX_Control(pDX, IDC_SLOTPICKER, m_slotpicker);
	DDX_Text(pDX, IDC_TEXT, m_text);
	DDX_Text(pDX, IDC_CNT, m_cntstr);
	DDX_Text(pDX, IDC_SOUNDREF, m_soundref);
	DDV_MaxChars(pDX, m_soundref, 8);
	DDX_Text(pDX, IDC_REF, m_ref);
	DDX_Check(pDX, IDC_TAGGED, m_tagged);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSRCEdit, CDialog)
	//{{AFX_MSG_MAP(CSRCEdit)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_PLAY, OnPlay)
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_BN_CLICKED(IDC_LOADEX, OnLoadex)
	ON_BN_CLICKED(IDC_SAVEAS, OnSaveas)
	ON_BN_CLICKED(IDC_NEW, OnNew)
	ON_BN_CLICKED(IDC_CHECK, OnCheck)
	ON_EN_KILLFOCUS(IDC_REF, OnKillfocusRef)
	ON_EN_KILLFOCUS(IDC_SOUNDREF, OnKillfocusSoundref)
	ON_COMMAND(ID_FILE_SAVE, OnSave)
	ON_EN_KILLFOCUS(IDC_TEXT, OnKillfocusText)
	ON_BN_CLICKED(IDC_TAGGED, OnTagged)
	ON_CBN_KILLFOCUS(IDC_SLOTPICKER, OnKillfocusSlotpicker)
	ON_CBN_SELCHANGE(IDC_SLOTPICKER, OnSelchangeSlotpicker)
	ON_COMMAND(ID_FILE_NEW, OnNew)
	ON_EN_CHANGE(IDC_BUDDY, OnChangeBuddy)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CSRCEdit::NewSRC()
{
	itemname="new SRC";
  the_src.KillSlots();
  m_stringnum=0;
}

/////////////////////////////////////////////////////////////////////////////
// CSRCEdit message handlers

void CSRCEdit::RefreshControl()
{
  CString tmpstr, tmp;
  int i;

  SetWindowText("Edit string resource: "+itemname);
  m_cntstr.Format("/ %d",the_src.m_cnt);
  if(m_stringnum<0) m_stringnum=0;
  m_slotpicker.ResetContent();
  m_slotpicker.InitStorage(the_src.m_cnt,20);
  for(i=0;i<the_src.m_cnt;i++)
  {
    tmp=resolve_tlk_text(the_src.m_slots[i]);
    if(tmp.GetLength()>82) tmp=tmp.Left(80)+"...";
    tmpstr.Format("%3d. %s",i,tmp);
    m_slotpicker.AddString(tmpstr);
  }
  if(m_stringnum>=i) m_stringnum=i-1;
  m_stringnum=m_slotpicker.SetCurSel(m_stringnum);
  if(m_stringnum>=0)
  {
    m_ref=the_src.m_slots[m_stringnum];
    m_text=resolve_tlk_text(m_ref);
    m_tagged=resolve_tlk_tag(m_ref);
    m_soundref=resolve_tlk_soundref(m_ref);
  }
  else
  {
    m_tagged=false;
    m_text="";
    m_soundref="";
  }
  m_spincontrol.SetRange32(1,the_src.m_cnt);
  UpdateData(UD_DISPLAY);
}

BOOL CSRCEdit::OnInitDialog() 
{
	CDialog::OnInitDialog();
	RefreshControl();
  m_spincontrol.SetBuddy(&m_buddycontrol);

	return TRUE;  
}

void CSRCEdit::OnAdd() 
{
  long *newslots;

  m_stringnum=the_src.m_cnt;
	the_src.m_cnt++;
	newslots=new long[the_src.m_cnt];
  if(!newslots)
  {
    the_src.m_cnt--;
    return;
  }
  memcpy(newslots,the_src.m_slots,(m_stringnum)*sizeof(long));
  newslots[m_stringnum]=-1;
  if(the_src.m_slots) delete [] the_src.m_slots;
  the_src.m_slots=newslots;
  RefreshControl();
}

void CSRCEdit::OnRemove() 
{
  long *newslots;

  if(!the_src.m_cnt) return;
	the_src.m_cnt--;
	newslots=new long[the_src.m_cnt];
  if(!newslots)
  {
    the_src.m_cnt++;
    return;
  }
  memcpy(newslots,the_src.m_slots,m_stringnum*sizeof(long));
  memcpy(newslots+m_stringnum,the_src.m_slots+m_stringnum+1,(the_src.m_cnt-m_stringnum)*sizeof(long));
  if(the_src.m_slots) delete [] the_src.m_slots;
  the_src.m_slots=newslots;
  RefreshControl();
}

void CSRCEdit::OnKillfocusText() 
{
  CString tmpstr;
  
  UpdateData(UD_RETRIEVE);
  tmpstr=resolve_tlk_text(m_ref);
  if(tmpstr!=m_text)
  {
    if(editflg&TLKCHANGE)
    {
      if(MessageBox("Do you want to update dialog.tlk?","String resource editor",MB_YESNO)!=IDYES)
      {
        UpdateData(UD_DISPLAY);
        return;
      }
    }
    the_src.m_slots[m_stringnum]=store_tlk_text(m_ref,m_text);
  }
  RefreshControl();
  UpdateData(UD_DISPLAY);
}

void CSRCEdit::OnPlay() 
{
  play_acm(m_soundref,false,false);
}

void CSRCEdit::OnLoad() 
{
	int res;
	
  pickerdlg.m_restype=REF_SRC;
  pickerdlg.m_picked=itemname;
	res=pickerdlg.DoModal();
	if(res==IDOK)
	{
		res=read_src(pickerdlg.m_picked);
    switch(res)
    {
    case 0:
  		itemname=pickerdlg.m_picked;
      break;
    default:
      MessageBox("Cannot read user interface!","Error",MB_ICONEXCLAMATION|MB_OK);
      NewSRC();
      break;
    }
    RefreshControl();
    UpdateData(UD_DISPLAY);
	}
}

static char BASED_CODE szFilter[] = "String resource files (*.src)|*.src|All files (*.*)|*.*||";

void CSRCEdit::OnLoadex() 
{
  CString filepath;
  int fhandle;
  int res;
  
  res=OFN_FILEMUSTEXIST|OFN_ENABLESIZING|OFN_EXPLORER;
  if(readonly) res|=OFN_READONLY;  
  CFileDialog m_getfiledlg(TRUE, "src", makeitemname(".src",0), res, szFilter);

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
    res=the_src.ReadStringFromFile(fhandle,-1);
    close(fhandle);
    lastopenedoverride=filepath.Left(filepath.ReverseFind('\\'));
    switch(res)
    {
    case -4:
      MessageBox("String resource loaded with errors.","Warning",MB_ICONEXCLAMATION|MB_OK);
      itemname=m_getfiledlg.GetFileTitle();
      itemname.MakeUpper();
      break;
    case 1:
      MessageBox("String resource will be reordered (harmless inconsistency).","Warning",MB_ICONEXCLAMATION|MB_OK);
      itemname=m_getfiledlg.GetFileTitle();
      itemname.MakeUpper();
      break;
    case 0:
      itemname=m_getfiledlg.GetFileTitle();
      itemname.MakeUpper();
      break;
    default:
      MessageBox("Cannot read string resource!","Error",MB_ICONEXCLAMATION|MB_OK);
      NewSRC();
      break;
    }
    RefreshControl();
    UpdateData(UD_DISPLAY);
  }
}


void CSRCEdit::OnSave() 
{
  SaveSrc(1);
}

void CSRCEdit::OnSaveas() 
{
  SaveSrc(0);
}

void CSRCEdit::SaveSrc(int save) 
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
  CFileDialog m_getfiledlg(FALSE, "src", makeitemname(".src",0), res, szFilter);

  if(save)
  {
    newname=itemname;
    filepath=makeitemname(".src",0);
    goto gotname;
  }    
restart:
  if( m_getfiledlg.DoModal() == IDOK )
  {
    filepath=m_getfiledlg.GetPathName();
    filepath.MakeLower();
    if(filepath.Right(4)!=".src")
    {
      filepath+=".src";
    }
    newname=m_getfiledlg.GetFileName();
    newname.MakeUpper();
    if(newname.Right(4)==".SRC") newname=newname.Left(newname.GetLength()-4);
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
    fhandle=open(filepath, O_BINARY|O_RDWR|O_CREAT|O_TRUNC,S_IREAD|S_IWRITE);
    if(fhandle<1)
    {
      MessageBox("Can't write file!","Error",MB_ICONEXCLAMATION|MB_OK);
      goto restart;
    }
    res=the_src.WriteStringToFile(fhandle,0);
    close(fhandle);
    lastopenedoverride=filepath.Left(filepath.ReverseFind('\\'));
    switch(res)
    {
    case 0:
      itemname=newname;
      break; //saved successfully
    case -2:
      MessageBox("Error while writing file!","Error",MB_ICONEXCLAMATION|MB_OK);
      break;
    default:
      MessageBox("Unhandled error!","Error",MB_ICONEXCLAMATION|MB_OK);
    }
  }
  RefreshControl();
  UpdateData(UD_DISPLAY);
}

void CSRCEdit::OnNew() 
{
	NewSRC();
  RefreshControl();
}

void CSRCEdit::OnCheck() 
{
  CString tmpstr;
  int i;
  int ret;

	for(i=0;i<the_src.m_cnt;i++)
  {
    ret=check_reference(the_src.m_slots[i],1);
    if(!ret) continue;
    switch(ret)
    {
    case 1:
      tmpstr.Format("Invalid TLK reference: %d",the_src.m_slots[i]);
      break;
    case 2:
      tmpstr.Format("Deleted TLK reference: %d",the_src.m_slots[i]);
      break;         
    }
    MessageBox(tmpstr,"String resource editor",MB_ICONEXCLAMATION|MB_OK);
  }
}

void CSRCEdit::OnChangeBuddy() 
{
  CString tmpstr;

  if(m_buddycontrol)
  {
    m_buddycontrol.GetWindowText(tmpstr);
    m_stringnum=the_src.m_cnt-atoi(tmpstr);
    RefreshControl();
    UpdateData(UD_DISPLAY);
  }
}

void CSRCEdit::OnKillfocusSlotpicker() 
{
  CString tmpstr;

  UpdateData(UD_RETRIEVE);
  m_slotpicker.GetWindowText(tmpstr);
  m_stringnum=strtonum(tmpstr);
  m_stringnum=m_slotpicker.SetCurSel(m_stringnum);
  if(m_stringnum>=0)
  {
    tmpstr.Format("%d",the_src.m_cnt-m_stringnum);
    m_buddycontrol.SetWindowText(tmpstr);    
  }
  RefreshControl();
  UpdateData(UD_DISPLAY);
}

void CSRCEdit::OnSelchangeSlotpicker() 
{
  CString tmpstr;

  m_stringnum=m_slotpicker.GetCurSel();
  tmpstr.Format("%d",the_src.m_cnt-m_stringnum);
  m_buddycontrol.SetWindowText(tmpstr);
	RefreshControl();
  UpdateData(UD_DISPLAY);
}

void CSRCEdit::OnKillfocusRef() 
{
  UpdateData(UD_RETRIEVE);
  the_src.m_slots[m_stringnum]=m_ref;
	RefreshControl();
  UpdateData(UD_DISPLAY);
}

void CSRCEdit::OnTagged() 
{
  if((editflg&TLKCHANGE) )
  {
    if(MessageBox("Do you want to update dialog.tlk?","String resource editor",MB_YESNO)!=IDYES)
    {
      return;
    }
  }
  toggle_tlk_tag(m_ref);
  RefreshControl();
  UpdateData(UD_DISPLAY);
}

void CSRCEdit::OnKillfocusSoundref() 
{
  CString tmpstr;

  UpdateData(UD_RETRIEVE);
  tmpstr=resolve_tlk_soundref(m_ref);
  if(tmpstr!=m_soundref)
  {
    if(editflg&TLKCHANGE)
    {
      if(MessageBox("Do you want to update dialog.tlk?","String resource editor",MB_YESNO)!=IDYES)
      {
        UpdateData(UD_DISPLAY);
        return;
      }
    }
    m_ref=store_tlk_soundref(m_ref, m_soundref);
  }
  RefreshControl();
  UpdateData(UD_DISPLAY);
}

void CSRCEdit::PostNcDestroy() 
{
	if(the_src.m_slots) delete [] the_src.m_slots;
  the_src.m_slots=NULL;
	CDialog::PostNcDestroy();
}
