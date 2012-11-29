// options.cpp : implementation file
//

#include "stdafx.h"

#include <fcntl.h>

#include "chitem.h"
#include "options.h"
#include "MyFileDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static int CheckDialogRefs[]=
{
  IDC_STRREF, IDC_ITEMREF, IDC_ICONCHK, IDC_ATTR, IDC_USABILITY, 
  IDC_ITEMTYPE, IDC_MPLUS, IDC_EXTHEAD, IDC_WPROF, IDC_NOCHITIN,
  IDC_STATR, IDC_OTHER, IDC_ANIM, IDC_WARN, IDC_NOSPEC, IDC_SKIPEXT,
  IDC_SOA, IDC_DLGCHK, IDC_DUPLO, IDC_SCRIPT, IDC_MISSING,
  IDC_CHECKSCRIPT, IDC_FEATURE, IDC_VARIABLES, IDC_SPELLREF, IDC_CREREF,
  IDC_STOREREF, IDC_NOCUT, IDC_STRUCT, IDC_SKIPDVAR, IDC_SLOT, IDC_DURATION,
0};

static int EditDialogRefs[]=
{
  IDC_RECYCLE, IDC_TLKCHANGE, IDC_RESOLVE, IDC_2DACHANGE, IDC_INDENT, //1-16
  IDC_SELECTION, IDC_REMBAF, IDC_CWDBAF, IDC_DECOMPRESS, IDC_DATAFOLDER, //32-512
  IDC_NOCHECK, IDC_STEREO, IDC_SORTEFFECT, IDC_EFFNUMBER, IDC_EATSPACE, //1024-16384
  IDC_WEIDUSTR, IDC_IDUSTR, IDC_PREVIEW, IDC_OCTREE, IDC_DITHER, //32768-0x80000
  IDC_W98,IDC_WEIDU, IDC_ZIP, IDC_WINDOWPICKER, IDC_CENTER, IDC_OVERRIDE,//0x100000-0x2000000
  IDC_SIZECHECK, IDC_CD,IDC_INTDECOMP,IDC_INTCOMP, IDC_FORCENEW,      //0x4000000-0x40000000
  IDC_SHADOW, //0x8000000  
0};

/////////////////////////////////////////////////////////////////////////////
// Coptions dialog

Coptions::Coptions(CWnd* pParent /*=NULL*/)
	: CDialog(Coptions::IDD, pParent)
{
	//{{AFX_DATA_INIT(Coptions)
	//}}AFX_DATA_INIT
}

void Coptions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Coptions)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(Coptions, CDialog)
	//{{AFX_MSG_MAP(Coptions)
	ON_BN_CLICKED(IDC_ITEMREF, OnItemref)
	ON_BN_CLICKED(IDC_ATTR, OnAttr)
	ON_BN_CLICKED(IDC_STRREF, OnStrref)
	ON_BN_CLICKED(IDC_ICONCHK, OnIconchk)
	ON_BN_CLICKED(IDC_USABILITY, OnUsability)
	ON_BN_CLICKED(IDC_ITEMTYPE, OnItemtype)
	ON_BN_CLICKED(IDC_MPLUS, OnMplus)
	ON_BN_CLICKED(IDC_EXTHEAD, OnExthead)
	ON_BN_CLICKED(IDC_ALLSET, OnAllset)
	ON_BN_CLICKED(IDC_ALLUNSET, OnAllunset)
	ON_BN_CLICKED(IDC_NOCHITIN, OnNochitin)
	ON_BN_CLICKED(IDC_WPROF, OnWprof)
	ON_BN_CLICKED(IDC_STATR, OnStatr)
	ON_BN_CLICKED(IDC_OTHER, OnOther)
	ON_BN_CLICKED(IDC_ANIM, OnAnim)
	ON_BN_CLICKED(IDC_WARN, OnWarn)
	ON_BN_CLICKED(IDC_NOSPEC, OnNospec)
	ON_BN_CLICKED(IDC_SKIPEXT, OnSkipext)
	ON_BN_CLICKED(IDC_SOA, OnSoa)
	ON_BN_CLICKED(IDC_DLGCHK, OnDlgchk)
	ON_BN_CLICKED(IDC_DUPLO, OnDuplo)
	ON_BN_CLICKED(IDC_SCRIPT, OnScript)
	ON_BN_CLICKED(IDC_MISSING, OnMissing)
	ON_BN_CLICKED(IDC_FEATURE, OnFeature)
	ON_BN_CLICKED(IDC_VARIABLES, OnVariables)
	ON_BN_CLICKED(IDC_SPELLREF, OnSpellref)
	ON_BN_CLICKED(IDC_CREREF, OnCreref)
	ON_BN_CLICKED(IDC_STOREREF, OnStoreref)
	ON_BN_CLICKED(IDC_SKIPDVAR, OnSkipdvar)
	ON_BN_CLICKED(IDC_STRUCT, OnStruct)
	ON_BN_CLICKED(IDC_CHECKSCRIPT, OnCheckscript)
	ON_BN_CLICKED(IDC_NOCUT, OnNocut)
	ON_BN_CLICKED(IDC_SLOT, OnSlot)
	ON_BN_CLICKED(IDC_DURATION, OnDuration)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void Coptions::Refresh()
{
  int i;
  int j;
  CButton *checkbox;

  j=1;
	for(i=0;CheckDialogRefs[i];i++)
  {
    checkbox=(class CButton *) GetDlgItem(CheckDialogRefs[i]);
    if(checkbox) checkbox->SetCheck(!(chkflg&j));
    j<<=1;
  }
}

BOOL Coptions::OnInitDialog() 
{
	CDialog::OnInitDialog();
  Refresh();
  //tooltips
  {
    m_tooltip.Create(this,TTS_NOPREFIX);
    m_tooltip.SetMaxTipWidth(200);
    m_tooltip.SetTipBkColor(RGB(240,224,160));
  }
  UpdateData(UD_DISPLAY);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// Coptions message handlers

void Coptions::OnAllset() 
{
	chkflg=0;
  Refresh();
}

void Coptions::OnAllunset() 
{
	chkflg=~0u;
  Refresh();
}

void Coptions::OnItemref() 
{
  chkflg^=NOITEMCH;
}

void Coptions::OnStrref() 
{
	chkflg^=NOREFCHK;
}

void Coptions::OnIconchk() 
{
	chkflg^=NORESCHK;
}

void Coptions::OnUsability() 
{
  chkflg^=NOUSECHK;	
}

void Coptions::OnAttr() 
{
	chkflg^=NOATTRCH;
}

void Coptions::OnItemtype() 
{
  chkflg^=NOTYPECH;	
}

void Coptions::OnMplus() 
{
  chkflg^=NOMUSCH;
}

void Coptions::OnExthead() 
{
  chkflg^=NOEXTCHK;
}

void Coptions::OnWprof() 
{
  chkflg^=NOWPROCH;
}

void Coptions::OnNochitin() 
{
  chkflg^=IGNORECHITIN;
}

void Coptions::OnStatr() 
{
  chkflg^=NOSTATCH;	
}

void Coptions::OnOther() 
{
  chkflg^=NOOTHERCH;	
}

void Coptions::OnAnim() 
{
  chkflg^=NOANIMCHK;	
}

void Coptions::OnWarn() 
{
  chkflg^=WARNINGS;	
}

void Coptions::OnNospec() 
{
  chkflg^=DROPPABLE;	
}

void Coptions::OnSkipext() 
{
  chkflg^=SKIPEXT;	
}

void Coptions::OnSoa() 
{
  chkflg^=SKIPSOA;
}

void Coptions::OnDlgchk() 
{
  chkflg^=NODLGCHK;
}

void Coptions::OnDuplo() 
{
  chkflg^=NODUPLO;	
}

void Coptions::OnScript() 
{
  chkflg^=DOSCRIPTS;	
}

void Coptions::OnNocut() 
{
  chkflg^=NOCUT;
}

void Coptions::OnMissing() 
{
  chkflg^=NOMISS;
}

void Coptions::OnFeature() 
{
  chkflg^=NOFEATCHK;	
}

void Coptions::OnVariables() 
{
  chkflg^=NOVARCHK;	
}

void Coptions::OnSpellref() 
{
  chkflg^=NOSPLCHK;	
}

void Coptions::OnCreref() 
{
  chkflg^=NOCRECHK;	
}

void Coptions::OnStoreref() 
{
	chkflg^=NOSTORCH;	
}

void Coptions::OnStruct() 
{
	chkflg^=NOSTRUCT;
}

void Coptions::OnSkipdvar() 
{
	chkflg^=NODVARCH;
}

void Coptions::OnSlot() 
{
	chkflg^=NOSLOTCH;	
}

void Coptions::OnCheckscript() 
{
  chkflg^=NOSCRIPT;
}

void Coptions::OnDuration() 
{
  chkflg^=NODUR;
}

BOOL Coptions::PreTranslateMessage(MSG* pMsg) 
{
  m_tooltip.RelayEvent(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}

//////////////////////////////////////////////////////////
CEditOpt::CEditOpt(CWnd* pParent /*=NULL*/)
	: CDialog(CEditOpt::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditOpt)
	//}}AFX_DATA_INIT
}

void CEditOpt::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditOpt)
	DDX_Text(pDX, IDC_WIDTH, winsize);
	DDV_MinMaxInt(pDX, winsize, 10, 32);
	//}}AFX_DATA_MAP
}

void CEditOpt::Refresh()
{
  int i;
  int j;
  CButton *checkbox;

  j=1;
	for(i=0;EditDialogRefs[i];i++)
  {
    checkbox=(class CButton *) GetDlgItem(EditDialogRefs[i]);
    if(checkbox) checkbox->SetCheck(!(editflg&j));
    j<<=1;
  }
  GetDlgItem(IDC_WIDTH)->EnableWindow(!(editflg&LARGEWINDOW));
}

BOOL CEditOpt::OnInitDialog() 
{
  CString tmpstr;

	CDialog::OnInitDialog();
	Refresh();	
  //tooltips
  {
    m_tooltip.Create(this,TTS_NOPREFIX);
    m_tooltip.SetMaxTipWidth(200);
    m_tooltip.SetTipBkColor(RGB(240,224,160));
    m_tooltip.AddTool(GetDlgItem(IDCANCEL), IDS_CANCEL);
    m_tooltip.AddTool(GetDlgItem(IDC_WIDTH), IDS_WINSIZE);
    m_tooltip.AddTool(GetDlgItem(IDC_TLKCHANGE), IDS_TLKCHANGE);
    m_tooltip.AddTool(GetDlgItem(IDC_2DACHANGE), IDS_2DACHANGE);
    m_tooltip.AddTool(GetDlgItem(IDC_RESOLVE), IDS_RESOLVE);
    m_tooltip.AddTool(GetDlgItem(IDC_RECYCLE), IDS_RECYCLE);
    m_tooltip.AddTool(GetDlgItem(IDC_INDENT), IDS_INDENT);
    m_tooltip.AddTool(GetDlgItem(IDC_REMBAF), IDS_REMBAF);
    m_tooltip.AddTool(GetDlgItem(IDC_SELECTION), IDS_SELECTION);
    m_tooltip.AddTool(GetDlgItem(IDC_CWDBAF), IDS_LASTOPENED);
    m_tooltip.AddTool(GetDlgItem(IDC_DECOMPRESS), IDS_DECOMPRESS);
    m_tooltip.AddTool(GetDlgItem(IDC_DATAFOLDER), IDS_DATAFOLDER);
    m_tooltip.AddTool(GetDlgItem(IDC_NOCHECK), IDS_NOCHECK);
    tmpstr.LoadString(IDS_FORCESTEREO);
    m_tooltip.AddTool(GetDlgItem(IDC_STEREO), tmpstr);
    m_tooltip.AddTool(GetDlgItem(IDC_SORTEFFECT), IDS_SORTEFF);
    m_tooltip.AddTool(GetDlgItem(IDC_EFFNUMBER), IDS_NUMEFF);
    m_tooltip.AddTool(GetDlgItem(IDC_EATSPACE), IDS_EATSPACE);
    m_tooltip.AddTool(GetDlgItem(IDC_WEIDUSTR), IDS_WEIDUSTRING);
    m_tooltip.AddTool(GetDlgItem(IDC_IDUSTR), IDS_IDUSTRING);
    m_tooltip.AddTool(GetDlgItem(IDC_PREVIEW), IDS_PREVIEW);
    m_tooltip.AddTool(GetDlgItem(IDC_OCTREE), IDS_OCTREE);
    m_tooltip.AddTool(GetDlgItem(IDC_DITHER), IDS_DITHER);
    m_tooltip.AddTool(GetDlgItem(IDC_W98), IDS_W98);
    m_tooltip.AddTool(GetDlgItem(IDC_WEIDU), IDS_RESLOC);
    m_tooltip.AddTool(GetDlgItem(IDC_ZIP), IDS_ZIP);
    m_tooltip.AddTool(GetDlgItem(IDC_WINDOWPICKER), IDS_LARGEWINDOW);
    m_tooltip.AddTool(GetDlgItem(IDC_CENTER), IDS_VERTCENTER);
    m_tooltip.AddTool(GetDlgItem(IDC_OVERRIDE), IDS_IGNOREOVER);
    m_tooltip.AddTool(GetDlgItem(IDC_SIZECHECK), IDS_CHECKSIZE);
    m_tooltip.AddTool(GetDlgItem(IDC_CD), IDS_CD);
    m_tooltip.AddTool(GetDlgItem(IDC_INTDECOMP), IDS_INTDECOMP);
    m_tooltip.AddTool(GetDlgItem(IDC_INTCOMP), IDS_INTCOMP);
    tmpstr.LoadString(IDS_SHADOW);
    m_tooltip.AddTool(GetDlgItem(IDC_SHADOW), tmpstr);
    m_tooltip.AddTool(GetDlgItem(IDC_FORCENEW), IDS_FORCENEW);
  }
	return TRUE;
}

BEGIN_MESSAGE_MAP(CEditOpt, CDialog)
	//{{AFX_MSG_MAP(CEditOpt)
	ON_BN_CLICKED(IDC_TLKCHANGE, OnTlkchange)
	ON_BN_CLICKED(IDC_2DACHANGE, On2dachange)
	ON_BN_CLICKED(IDC_RECYCLE, OnRecycle)
	ON_BN_CLICKED(IDC_RESOLVE, OnResolve)
	ON_BN_CLICKED(IDC_ALLSET, OnAllset)
	ON_BN_CLICKED(IDC_ALLUNSET, OnAllunset)
	ON_BN_CLICKED(IDC_INDENT, OnIndent)
	ON_BN_CLICKED(IDC_SELECTION, OnSelection)
	ON_BN_CLICKED(IDC_REMBAF, OnRembaf)
	ON_BN_CLICKED(IDC_CWDBAF, OnCwdbaf)
	ON_BN_CLICKED(IDC_DECOMPRESS, OnDecompress)
	ON_BN_CLICKED(IDC_NOCHECK, OnNocheck)
	ON_BN_CLICKED(IDC_DATAFOLDER, OnDatafolder)
	ON_BN_CLICKED(IDC_STEREO, OnStereo)
	ON_BN_CLICKED(IDC_SORTEFFECT, OnSorteffect)
	ON_BN_CLICKED(IDC_EFFNUMBER, OnEffnumber)
	ON_BN_CLICKED(IDC_EATSPACE, OnEatspace)
	ON_BN_CLICKED(IDC_WEIDUSTR, OnWeidustr)
	ON_BN_CLICKED(IDC_IDUSTR, OnIdustr)
	ON_BN_CLICKED(IDC_PREVIEW, OnPreview)
	ON_BN_CLICKED(IDC_OCTREE, OnOctree)
	ON_BN_CLICKED(IDC_DITHER, OnDither)
	ON_BN_CLICKED(IDC_W98, OnW98)
	ON_BN_CLICKED(IDC_WEIDU, OnWeidu)
	ON_BN_CLICKED(IDC_ZIP, OnZip)
	ON_BN_CLICKED(IDC_WINDOWPICKER, OnLargeWindow)
	ON_BN_CLICKED(IDC_CENTER, OnCenter)
	ON_BN_CLICKED(IDC_OVERRIDE, OnOverride)
	ON_BN_CLICKED(IDC_SIZECHECK, OnSizecheck)
	ON_BN_CLICKED(IDC_CD, OnCd)
	ON_BN_CLICKED(IDC_INTDECOMP, OnIntdecomp)
	ON_BN_CLICKED(IDC_INTCOMP, OnIntcomp)
	ON_BN_CLICKED(IDC_FORCENEW, OnForcenew)
	ON_BN_CLICKED(IDC_SHADOW, OnShadow)
	ON_EN_KILLFOCUS(IDC_WIDTH, OnKillfocusWidth)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditOpt message handlers

void CEditOpt::OnAllset() 
{
	editflg=0;
  Refresh();
}

void CEditOpt::OnAllunset() 
{
	editflg=~0u;
  Refresh();
}

void CEditOpt::OnTlkchange() 
{
	editflg^=TLKCHANGE;
}

void CEditOpt::On2dachange() 
{
	editflg^=DACHANGE;	
}

void CEditOpt::OnRecycle() 
{
  editflg^=RECYCLE;
}

void CEditOpt::OnResolve() 
{
  editflg^=RESOLVE;	
}

void CEditOpt::OnIndent() 
{
  editflg^=INDENT;	
}

void CEditOpt::OnSelection() 
{
  editflg^=SELECTION;	
}

void CEditOpt::OnRembaf() 
{
  editflg^=REMBAF;	
}

void CEditOpt::OnCwdbaf() 
{
  editflg^=LASTOPENED;	  
}

void CEditOpt::OnDecompress() 
{
  editflg^=DECOMPRESS;	
}

void CEditOpt::OnDatafolder() 
{
  editflg^=DATAFOLDER;	
}

void CEditOpt::OnNocheck() 
{
  editflg^=NOCHECK;
}

void CEditOpt::OnStereo() 
{
  editflg^=FORCESTEREO;
}

void CEditOpt::OnSorteffect() 
{
  editflg^=SORTEFF;	
}

void CEditOpt::OnEffnumber() 
{
  editflg^=NUMEFF;	
}

void CEditOpt::OnEatspace() 
{
  editflg^=EATSPACE;	
}

void CEditOpt::OnWeidustr() 
{
	editflg^=WEIDUSTRING;
}

void CEditOpt::OnIdustr() 
{
	editflg^=IDUSTRING;
}

void CEditOpt::OnPreview() 
{
	editflg^=PREVIEW;
}

void CEditOpt::OnOctree() 
{
	editflg^=OCTREE;
}

void CEditOpt::OnDither() 
{
	editflg^=DITHER;
}

void CEditOpt::OnW98() 
{
	editflg^=W98;
}

void CEditOpt::OnWeidu() 
{
	editflg^=RESLOC;
}

void CEditOpt::OnZip() 
{
	editflg^=USEIAP;
}

void CEditOpt::OnLargeWindow() 
{
	editflg^=LARGEWINDOW;
  Refresh();
}

void CEditOpt::OnCenter() 
{
	editflg^=CENTER;
}

void CEditOpt::OnOverride() 
{
	editflg^=IGNOREOVERRIDE;
}

void CEditOpt::OnSizecheck() 
{
	editflg^=CHECKSIZE;
}

void CEditOpt::OnCd() 
{
	editflg^=IGNORECD;
}

void CEditOpt::OnIntdecomp() 
{
	editflg^=INTERNALDECOMP;
}

void CEditOpt::OnIntcomp() 
{
	editflg^=INTERNALCOMPILER;
}

void CEditOpt::OnForcenew() 
{
  editflg^=FORCENEW;
}

void CEditOpt::OnShadow() 
{
  editflg^=KEEPSHADOW;
}

void CEditOpt::OnKillfocusWidth() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

BOOL CEditOpt::PreTranslateMessage(MSG* pMsg) 
{
  m_tooltip.RelayEvent(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}

/////////////////////////////////////////////////////////////////////////////
// CWeiDUOpt dialog


CWeiDUOpt::CWeiDUOpt(CWnd* pParent /*=NULL*/)
	: CDialog(CWeiDUOpt::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWeiDUOpt)
	m_command = _T("");
	//}}AFX_DATA_INIT
}

void CWeiDUOpt::DoDataExchange(CDataExchange* pDX)
{
  int i,j;
  CButton *cb;

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWeiDUOpt)
	DDX_Control(pDX, IDC_LANGUAGE, m_language_control);
	DDX_Control(pDX, IDC_OPENFILE, m_openfile);
	DDX_Text(pDX, IDC_COMMAND, m_command);
	//}}AFX_DATA_MAP
  DDX_Text(pDX, IDC_FILENAME, weidupath);
  DDX_Text(pDX, IDC_PARAM, weiduextra);
  DDX_Text(pDX, IDC_DECOMPILED, weidudecompiled);
  DDX_Text(pDX, IDC_LANGUAGE, language);
  j=1;
  for(i=0;i<5;i++)
  {
    cb=(CButton *) GetDlgItem(IDC_FLAG1+i);
    cb->SetCheck(!!(weiduflg&j));
    j<<=1;
  }
  cb=(CButton *) GetDlgItem(IDC_LOG);
  cb->SetCheck(!!(weiduflg&WEI_LOGGING));
}

BEGIN_MESSAGE_MAP(CWeiDUOpt, CDialog)
	//{{AFX_MSG_MAP(CWeiDUOpt)
	ON_EN_KILLFOCUS(IDC_PARAM, OnKillfocusParam)
	ON_BN_CLICKED(IDC_FLAG1, OnFlag1)
	ON_BN_CLICKED(IDC_FLAG2, OnFlag2)
	ON_BN_CLICKED(IDC_FLAG3, OnFlag3)
	ON_BN_CLICKED(IDC_FLAG4, OnFlag4)
	ON_BN_CLICKED(IDC_OPENFILE, OnOpenfile)
	ON_BN_CLICKED(IDC_LOG, OnLog)
	ON_EN_KILLFOCUS(IDC_DECOMPILED, OnKillfocusDecompiled)
	ON_BN_CLICKED(IDC_FLAG5, OnFlag5)
	ON_EN_KILLFOCUS(IDC_LANGUAGE, OnKillfocusLanguage)
	ON_CBN_KILLFOCUS(IDC_LANGUAGE, OnKillfocusLanguage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWeiDUOpt message handlers

void CWeiDUOpt::Refresh()
{
  m_command="Export: "+AssembleWeiduCommandLine("dialog",weidudecompiled, true)+
  "\r\nImport: "+AssembleWeiduCommandLine("weidu\\dialog.d","override", false);
  UpdateData(UD_DISPLAY);
}

void CWeiDUOpt::OnKillfocusParam() 
{
  UpdateData(UD_RETRIEVE);
	Refresh();
}

void CWeiDUOpt::OnKillfocusDecompiled() 
{
	UpdateData(UD_RETRIEVE);
	Refresh();
}

static char BASED_CODE szFilter[] = "weidu.exe|weidu.exe|All files (*.*)|*.*||";

void CWeiDUOpt::OnOpenfile() 
{
  int flg;
  CString tmp;
  
  flg=OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|OFN_ENABLESIZING|OFN_EXPLORER;
  CMyFileDialog m_getfiledlg(TRUE, NULL, weidupath, flg, szFilter);
  
restart:
  if( m_getfiledlg.DoModal() == IDOK )
  {
    tmp=m_getfiledlg.GetPathName();
    tmp.MakeLower();
    if(tmp.Right(9)!="weidu.exe")
    {
      MessageBox("You should select weidu.exe","Warning",MB_OK);
      goto restart;
    }
    if(!checkfile(tmp,"MZ"))
    {
      MessageBox("This is not a valid WeiDU.exe file. (It must be an executable)","Warning",MB_OK);
      goto restart;
    }
    weidupath=tmp;
  }
  Refresh();
}

void CWeiDUOpt::FillPath(CComboBox &box)
{
  CString path;

  box.ResetContent();
  path = bgfolder+"/lang/*";
  box.Dir(DDL_DIRECTORY, path);
}

BOOL CWeiDUOpt::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  m_openfile.SetBitmap(theApp.m_bbmp);
  FillPath(m_language_control);
  Refresh();
  //tooltips
  {
    m_tooltip.Create(this,TTS_NOPREFIX);
    m_tooltip.SetMaxTipWidth(200);
    m_tooltip.SetTipBkColor(RGB(240,224,160));
  }
	return TRUE;
}

void CWeiDUOpt::OnFlag1() 
{
	weiduflg^=WEI_NOHEADER;
  Refresh();
}

void CWeiDUOpt::OnFlag2() 
{
	weiduflg^=WEI_NOFROM;
  Refresh();
}

void CWeiDUOpt::OnFlag3() 
{
	weiduflg^=WEI_NOCOM;
  Refresh();
}

void CWeiDUOpt::OnFlag4() 
{
	weiduflg^=WEI_TEXT;
  Refresh();
}

void CWeiDUOpt::OnFlag5() 
{
	weiduflg^=WEI_GAMETYPE;
  Refresh();
}

void CWeiDUOpt::OnLog() 
{
	weiduflg^=WEI_LOGGING;
  Refresh();
}

void CWeiDUOpt::OnKillfocusLanguage() 
{
  UpdateData(UD_RETRIEVE);
  language.Remove('[');
  language.Remove('.');
  language.Remove(']');
	Refresh();
}

BOOL CWeiDUOpt::PreTranslateMessage(MSG* pMsg) 
{
  m_tooltip.RelayEvent(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}
