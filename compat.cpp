// compat.cpp : implementation file
//

#include "stdafx.h"

#include <fcntl.h>
#include <direct.h>
#include "chitem.h"
#include "compat.h"
#include "options.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//dialog structure is old version/new version
bool tob_specific()
{
  if(optflg&COM_TOB) return true;
  return false;
}

bool old_version_dlg()
{
  if(optflg&COM_DLG) return false;
  return true;
}

//pst compatible var.var structure
bool pst_compatible_var()
{
  if(optflg&COM_PST) return true;
  return false;
}

bool bg1_compatible_area()
{
  if(optflg&COM_BG1) return true;
  return false;
}

bool iwd2_structures()
{
  if(optflg&COM_IWD2) return true;
  return false;
}

bool unlimited_itemtypes()
{
  if(optflg&COM_ITEM) return true;
  return false;
}

bool no_compress()
{
  if(optflg&COM_NOZIP) return true;
  return false;
}

bool bg2_weaprofs()
{
  if(optflg&COM_NOWP) return false;
  return true;
}

bool dltc_weaprofs()
{
  if(optflg&COM_NOWP) return false;
  if(optflg&COM_DLTC) return true;
  return false;
}

bool has_xpvar()
{
  if(optflg&COM_XPL) return true;
  return false;
}
/////////////////////////////////////////////////////////////////////////////
// Ccompat dialog

Ccompat::Ccompat(CWnd* pParent /*=NULL*/)	: CDialog(Ccompat::IDD, pParent)
{
	//{{AFX_DATA_INIT(Ccompat)
	m_descpath = _T("");
	m_filename = _T("");
	m_setupname = _T("settings");
	//}}AFX_DATA_INIT
}

void Ccompat::Transfer()
{
  theApp.select_setup(m_setupname);
  m_filename=bgfolder+"chitin.key";
  m_descpath=descpath;
  UpdateData(UD_DISPLAY);
}

void Ccompat::Refresh()
{
  POSITION pos;
  CString key;
  gameprops gp;
  int flg;

  flg=1;
  m_setupname_control.ResetContent();
  pos=allgameprops.GetStartPosition();
  while(pos)
  {
    allgameprops.GetNextAssoc(pos, key, gp);
    m_setupname_control.AddString(key);
    if(key==m_setupname) flg=0;
  }
  if(flg)
  {
    if(allgameprops.GetCount())
    {
      m_setupname_control.GetLBText(0,m_setupname);
    }
  }
  Transfer();
}

BOOL Ccompat::OnInitDialog() 
{
  CString tmpstr;
  CButton *chb;
  CComboBox *cob;

  m_setupname=setupname;
  m_filename=bgfolder+"chitin.key";
  m_descpath=descpath;
	CDialog::OnInitDialog();
  chb=(CButton *) GetDlgItem(IDC_PROGRESS);
  if(chb) chb->SetCheck(do_progress);
  chb=(CButton *) GetDlgItem(IDC_TOOLTIP);
  if(chb) chb->SetCheck(tooltips);
  chb=(CButton *) GetDlgItem(IDC_READONLY);
  if(chb) chb->SetCheck(readonly);
  cob=(CComboBox *) GetDlgItem(IDC_LOGTYPE);
  if(cob) cob->SetCurSel(logtype);
	Refresh();
  m_openfile.SetBitmap(theApp.m_bbmp);
  m_descopen.SetBitmap(theApp.m_bbmp);
  //tooltips
  {
    m_tooltip.Create(this,TTS_NOPREFIX);
    m_tooltip.SetMaxTipWidth(200);
    m_tooltip.SetTipBkColor(RGB(240,224,160));

    m_tooltip.AddTool(GetDlgItem(IDCANCEL), IDS_CANCEL);
    m_tooltip.AddTool(GetDlgItem(IDC_SAVE), IDS_SAVEBACK);
    m_tooltip.AddTool(GetDlgItem(IDC_OPTIONS), IDS_OPTIONS);
    m_tooltip.AddTool(GetDlgItem(IDC_OPTIONS1), IDS_EDIT);
    m_tooltip.AddTool(GetDlgItem(IDC_OPTIONS2), IDS_WEIDU);
    m_tooltip.AddTool(GetDlgItem(IDC_ADD), IDS_ADDSETUP);
    m_tooltip.AddTool(GetDlgItem(IDC_REMOVE), IDS_REMOVESETUP);
    tmpstr.LoadString(IDS_OPENEFFECT);
    m_tooltip.AddTool(GetDlgItem(IDC_DESCOPEN), tmpstr);
    m_tooltip.AddTool(GetDlgItem(IDC_FILENAME), IDS_RIGHT);
    m_tooltip.AddTool(GetDlgItem(IDC_DESCPATH), IDS_RIGHT);
    tmpstr.LoadString(IDS_GAMETYPE);
    m_tooltip.AddTool(GetDlgItem(IDC_GAMETYPE), tmpstr);
    m_tooltip.AddTool(GetDlgItem(IDC_DIALOG), IDS_DIALOGF);
    m_tooltip.AddTool(GetDlgItem(IDC_PSTVAR), IDS_PSTVAR);
    m_tooltip.AddTool(GetDlgItem(IDC_DLG30), IDS_DLG30);
    m_tooltip.AddTool(GetDlgItem(IDC_BG1AREA), IDS_BG1AREA);
    m_tooltip.AddTool(GetDlgItem(IDC_IWD2), IDS_IWD2);
    m_tooltip.AddTool(GetDlgItem(IDC_UNLIMIT), IDS_UNLIMIT);
    m_tooltip.AddTool(GetDlgItem(IDC_NOZIP), IDS_NOZIP);
    m_tooltip.AddTool(GetDlgItem(IDC_WEAPON), IDS_WEAPON);
    m_tooltip.AddTool(GetDlgItem(IDC_XPLIST), IDS_XPLIST);
    m_tooltip.AddTool(GetDlgItem(IDC_DLTC), IDS_DLTC);
    m_tooltip.AddTool(GetDlgItem(IDC_TOB), IDS_TOB);
    m_tooltip.AddTool(GetDlgItem(IDC_LOGTYPE), IDS_LOGTYPE);
    m_tooltip.AddTool(GetDlgItem(IDC_TOOLTIP), IDS_TOOLTIP);
    tmpstr.LoadString(IDS_PROGRESS);
    m_tooltip.AddTool(GetDlgItem(IDC_PROGRESS), tmpstr);
    m_tooltip.AddTool(GetDlgItem(IDC_READONLY), IDS_READONLY);
    m_tooltip.AddTool(GetDlgItem(IDC_SETUPNAME), IDS_SETUPNAME);
  }
	return TRUE;
}

static int DialogRefs[]=
{
  IDC_DLG30, IDC_PSTVAR, IDC_BG1AREA, IDC_IWD2, IDC_UNLIMIT,IDC_NOZIP,
  IDC_WEAPON, IDC_XPLIST,IDC_DLTC,IDC_TOB,-1,-1,-1,-1,-1,-1,
  IDC_DIALOG,
0};

void Ccompat::DoDataExchange(CDataExchange* pDX)
{
  int i,j;
  CButton *checkbox;
  int lastchar;

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Ccompat)
	DDX_Control(pDX, IDC_LOGTYPE, m_logtype_control);
	DDX_Control(pDX, IDC_SETUPNAME, m_setupname_control);
	DDX_Control(pDX, IDC_GAMETYPE, m_gametype_control);
	DDX_Control(pDX, IDC_FILENAME, m_filename_control);
	DDX_Control(pDX, IDC_DESCPATH, m_descpath_control);
	DDX_Control(pDX, IDC_OPENFILE, m_openfile);
	DDX_Control(pDX, IDC_DESCOPEN, m_descopen);
	DDX_Text(pDX, IDC_DESCPATH, m_descpath);
	DDX_Text(pDX, IDC_FILENAME, m_filename);
	DDX_CBString(pDX, IDC_SETUPNAME, m_setupname);
	DDV_MaxChars(pDX, m_setupname, 40);
	//}}AFX_DATA_MAP
  lastchar=m_filename_control.GetWindowTextLength();
  m_filename_control.SetSel(lastchar,lastchar);
  lastchar=m_descpath_control.GetWindowTextLength();
  m_descpath_control.SetSel(lastchar,lastchar);
  if(pDX->m_bSaveAndValidate==UD_RETRIEVE)
  {
  }
  else
  {
    j=1;
	  for(i=0;DialogRefs[i];i++)
    {
      checkbox=(class CButton *) GetDlgItem(DialogRefs[i]);
      if(checkbox) checkbox->SetCheck(!!(optflg&j));
      j<<=1;
    }
  }
}

BEGIN_MESSAGE_MAP(Ccompat, CDialog)
	//{{AFX_MSG_MAP(Ccompat)
	ON_BN_CLICKED(IDC_OPENFILE, OnOpenfile)
	ON_BN_CLICKED(IDC_DESCOPEN, OnDescopen)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_PSTVAR, OnPstvar)
	ON_BN_CLICKED(IDC_DLG30, OnDlg30)
	ON_CBN_SELCHANGE(IDC_GAMETYPE, OnSelchangeGametype)
	ON_BN_CLICKED(IDC_BG1AREA, OnBg1area)
	ON_BN_CLICKED(IDC_IWD2, OnIwd2)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_CBN_KILLFOCUS(IDC_SETUPNAME, OnKillfocusSetupname)
	ON_CBN_SELCHANGE(IDC_SETUPNAME, OnSelchangeSetupname)
	ON_BN_CLICKED(IDC_TOOLTIP, OnTooltip)
	ON_BN_CLICKED(IDC_PROGRESS, OnProgress)
	ON_CBN_KILLFOCUS(IDC_LOGTYPE, OnKillfocusLogtype)
	ON_BN_CLICKED(IDC_OPTIONS, OnOptions)
	ON_BN_CLICKED(IDC_READONLY, OnReadonly)
	ON_BN_CLICKED(IDC_UNLIMIT, OnUnlimit)
	ON_BN_CLICKED(IDC_NOZIP, OnNozip)
	ON_BN_CLICKED(IDC_WEAPON, OnWeapon)
	ON_BN_CLICKED(IDC_XPLIST, OnXplist)
	ON_BN_CLICKED(IDC_OPTIONS2, OnOptions2)
	ON_BN_CLICKED(IDC_OPTIONS1, OnOptions1)
	ON_BN_CLICKED(IDC_DIALOG, OnDialog)
	ON_BN_CLICKED(IDC_TOB, OnTob)
	ON_BN_CLICKED(IDC_DLTC, OnDltc)
	ON_BN_CLICKED(IDC_NEW, OnNew)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Ccompat message handlers

void Ccompat::OnOpenfile() 
{
  int flg;
  
  flg=OFN_FILEMUSTEXIST|OFN_ENABLESIZING|OFN_EXPLORER;
  if(readonly) flg|=OFN_READONLY;  
  CFileDialog m_getfiledlg(TRUE, NULL, bgfolder+"chitin.key", flg, szFilterKey);
  
restart:
  if( m_getfiledlg.DoModal() == IDOK )
  {
    bgfolder=m_getfiledlg.GetPathName();
    bgfolder.MakeLower();
    if(bgfolder.Right(10)!="chitin.key")
    {
      MessageBox("You should select chitin.key","Warning",MB_ICONEXCLAMATION|MB_OK);
    }
    if(!checkfile(bgfolder,"KEY"))
    {
      MessageBox("This is not a valid chitin.key file. (It must start with KEY)","Warning",MB_ICONEXCLAMATION|MB_OK);
      goto restart;
    }
    m_filename=bgfolder;
    bgfolder=bgfolder.Left(bgfolder.GetLength()-10); //cutting chitin.key now
    readonly=!!m_getfiledlg.GetReadOnlyPref();
  }
  OnAdd();
}

static char BASED_CODE szFilter1[] = "IEEP Effect Description files (*Effects.dat)|*Effects.dat|"
             ".dat files (*.dat)|*.dat|All files (*.*)|*.*||";

void Ccompat::OnSave() 
{
  if(m_setupname=="settings" || m_setupname.IsEmpty())
  {
    MessageBox("Please give a name to your setup.");
    return;
  }
  theApp.save_settings();
  Ccompat::OnOK();
}

void Ccompat::OnDescopen() 
{
  int flg;

  flg=OFN_FILEMUSTEXIST|OFN_READONLY|OFN_HIDEREADONLY|OFN_ENABLESIZING|OFN_EXPLORER;
  CFileDialog m_getfiledlg(TRUE, NULL, descpath, flg, szFilter1);
restart:
  if( m_getfiledlg.DoModal() == IDOK )
	{
		m_descpath=m_getfiledlg.GetPathName();
    if(!checkfile(m_descpath,"0,1,"))
    {
      flg=MessageBox("This is not a valid IEEP Effect Description file. (It must start with 0,1,)\nSet it anyway?","Warning",MB_ICONEXCLAMATION|MB_YESNO);
      if(flg==IDNO) goto restart;
    }
    descpath=m_descpath;
  }
  OnAdd();
}

void Ccompat::OnDialog() 
{
	optflg^=BOTHDIALOG;
	OnAdd();
}

void Ccompat::OnPstvar() 
{
	optflg^=COM_PST;
  OnAdd();
}

void Ccompat::OnDlg30() 
{
	optflg^=COM_DLG;
  OnAdd();
}

void Ccompat::OnBg1area() 
{
	optflg^=COM_BG1;
  OnAdd();
}

void Ccompat::OnIwd2() 
{
	optflg^=COM_IWD2;
  OnAdd();
}

void Ccompat::OnUnlimit() 
{
	optflg^=COM_ITEM;
  OnAdd();
}

void Ccompat::OnNozip() 
{
	optflg^=COM_NOZIP;
  OnAdd();
}

void Ccompat::OnWeapon() 
{
	optflg^=COM_NOWP;
  OnAdd();
}

void Ccompat::OnXplist() 
{
	optflg^=COM_XPL;
  OnAdd();
}

void Ccompat::OnDltc() 
{
	optflg^=COM_DLTC;
  OnAdd();
}

void Ccompat::OnTob() 
{
	optflg^=COM_TOB;
  OnAdd();
}

#define MAXGAME 9
static unsigned long gametypes[MAXGAME]={
  COM_DLG|COM_BG1|COM_NOZIP|COM_NOWP,           //baldur's gate
  COM_DLG|COM_BG1|COM_NOZIP|COM_NOWP,           //totsc
  0,                                            //SoA
  COM_TOB,                                      //ToB
  COM_TOB|COM_DLTC,                             //DLTC
  COM_DLG|COM_ITEM|COM_NOZIP|COM_ITEM|COM_NOWP|COM_XPL,  //iwd
  COM_DLG|COM_ITEM|COM_NOZIP|COM_ITEM|COM_NOWP|COM_XPL,  //HoW
  COM_DLG|COM_IWD2|COM_ITEM|COM_NOWP|COM_XPL,   //iwd 2
  COM_DLG|COM_PST|COM_ITEM|COM_NOWP|COM_NOZIP|COM_ITEM,  //pst
};

static CString deftypes[MAXGAME]={"BG1Effects.dat","BG1Effects.dat",
"BG2Effects.dat","BG2Effects.dat","BG2Effects.dat",
"IWD2Effects.dat","IWD2Effects.dat","IWD2Effects.dat","PST_Effects.dat"};


void Ccompat::OnNew() 
{
	m_setupname="";
  m_filename="";
	m_descpath="";
  descpath="";
  UpdateData(UD_DISPLAY);
}

void Ccompat::OnAdd() 
{
  gameprops gp;
  CString tmp;
  int gt;

  m_setupname.TrimLeft();
  m_setupname.TrimRight();
  tmp=m_setupname;
  tmp.MakeLower();
  if(descpath.IsEmpty())
  {
    gt=m_gametype_control.GetCurSel();
    if(gt>=0 && gt<MAXGAME)
    {
      m_descpath=theApp.m_defpath+"\\"+deftypes[gt];
      if(file_exists(m_descpath) )
      {
        descpath=m_descpath;
      }
    }
  }
  if(tmp!="settings" && tmp.GetLength())
  {
    gp.bgfolder=bgfolder;
    gp.checkopt=chkflg;
    gp.editopt=editflg;
    gp.gameopt=optflg;
    gp.descpath=descpath;
    allgameprops.SetAt(m_setupname,gp);
    Refresh();
  }	
  UpdateData(UD_DISPLAY);
}

void Ccompat::OnSelchangeGametype() 
{
  CString filename;
  int idx;

	idx=m_gametype_control.GetCurSel();
  if(idx>=0 && idx<MAXGAME)
  {
    optflg=gametypes[idx];
  }
  OnAdd();
}

void Ccompat::OnRemove() 
{
  int x;

  allgameprops.RemoveKey(m_setupname);
  x=m_setupname_control.GetCurSel();
  m_setupname_control.DeleteString(x);
  if(m_setupname_control.GetCount())
  {
    m_setupname_control.SetCurSel(0);
  }
	Refresh();
}

void Ccompat::OnKillfocusSetupname() 
{
	UpdateData(UD_RETRIEVE);
  Transfer();
}

void Ccompat::OnSelchangeSetupname() 
{
  int x;

  x=m_setupname_control.GetCurSel();
  if(x!=-1)
  {
    m_setupname_control.GetLBText(x,m_setupname);
    setupname=m_setupname;
  }
  Transfer();
}

void Ccompat::OnTooltip() 
{
  tooltips=!tooltips;
}

void Ccompat::OnProgress() 
{
  do_progress=!do_progress;
}

void Ccompat::OnReadonly() 
{
  readonly=!readonly;
}

void Ccompat::OnKillfocusLogtype() 
{
  logtype=m_logtype_control.GetCurSel();
}

void Ccompat::OnOptions() 
{
  int oldchk;
  Coptions optdialog;
  
  oldchk=chkflg;
  if(optdialog.DoModal()==IDCANCEL)
  {
    chkflg=oldchk;
  }
  else
  {
    OnAdd();
  }
}

void Ccompat::OnOptions1() 
{
  int oldchk;
  CEditOpt optdialog;
  
  oldchk=editflg;
  if(optdialog.DoModal()==IDCANCEL)
  {
    editflg=oldchk;
  }
  else
  {
    OnAdd();
  }
}

void Ccompat::OnOptions2() 
{
  int oldchk;
  CWeiDUOpt optdialog;
  
  oldchk=weiduflg;
  if(optdialog.DoModal()==IDCANCEL)
  {
    weiduflg=oldchk;
  }
  else
  {
    OnAdd();
  }
}

BOOL Ccompat::PreTranslateMessage(MSG* pMsg) 
{
	m_tooltip.RelayEvent(pMsg);	
	return CDialog::PreTranslateMessage(pMsg);
}
