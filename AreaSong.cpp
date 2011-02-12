// AreaSong.cpp : implementation file
//

#include "stdafx.h"
#include "chitem.h"
#include "AreaSong.h"
#include "2DAEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAreaSong dialog

CAreaSong::CAreaSong(CWnd* pParent /*=NULL*/) : CDialog(CAreaSong::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAreaSong)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

BOOL CAreaSong::OnInitDialog() 
{
  CString tmpstr;
  POSITION pos;
  int i;

	CDialog::OnInitDialog();
  m_daycontrol.ResetContent();
  m_nightcontrol.ResetContent();
	m_u1control.ResetContent();
  m_battlecontrol.ResetContent();
  m_u2control.ResetContent();
	pos=songlist.GetHeadPosition();
  i=0;
  while(pos)
  {
    tmpstr.Format("%d %s",i++,songlist.GetNext(pos));
    m_daycontrol.AddString(tmpstr);
    m_nightcontrol.AddString(tmpstr);
    m_u1control.AddString(tmpstr);
    m_battlecontrol.AddString(tmpstr);
    m_u2control.AddString(tmpstr);
  }
  //tooltips
  {
    m_tooltip.Create(this,TTS_NOPREFIX);
    m_tooltip.SetMaxTipWidth(200);
    m_tooltip.SetTipBkColor(RGB(240,224,160));
    m_tooltip.AddTool(GetDlgItem(IDOK), IDS_CANCEL);
  }
  UpdateData(UD_DISPLAY);
	return TRUE;
}

void CAreaSong::DoDataExchange(CDataExchange* pDX)
{
  CString tmpstr;

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAreaSong)
	DDX_Control(pDX, IDC_BATTLE, m_battlecontrol);
	DDX_Control(pDX, IDC_UNKNOWN2, m_u2control);
	DDX_Control(pDX, IDC_UNKNOWN1, m_u1control);
	DDX_Control(pDX, IDC_NIGHT, m_nightcontrol);
	DDX_Control(pDX, IDC_DAY, m_daycontrol);
	//}}AFX_DATA_MAP
  tmpstr=get_songname(the_area.songheader.songs[0]);
  DDX_Text(pDX, IDC_DAY, tmpstr);
  the_area.songheader.songs[0]=strtonum(tmpstr);

  tmpstr=get_songname(the_area.songheader.songs[1]);
  DDX_Text(pDX, IDC_NIGHT, tmpstr);
  the_area.songheader.songs[1]=strtonum(tmpstr);

  tmpstr=get_songname(the_area.songheader.songs[2]);
  DDX_Text(pDX, IDC_UNKNOWN1, tmpstr);
  the_area.songheader.songs[2]=strtonum(tmpstr);

  tmpstr=get_songname(the_area.songheader.songs[3]);
  DDX_Text(pDX, IDC_BATTLE, tmpstr);
  the_area.songheader.songs[3]=strtonum(tmpstr);

  tmpstr=get_songname(the_area.songheader.songs[4]);
  DDX_Text(pDX, IDC_UNKNOWN2,tmpstr);
  the_area.songheader.songs[4]=strtonum(tmpstr);

  DDX_Text(pDX, IDC_UNKNOWN3, the_area.songheader.songs[5]);
  DDX_Text(pDX, IDC_UNKNOWN4, the_area.songheader.songs[6]);
  DDX_Text(pDX, IDC_UNKNOWN5, the_area.songheader.songs[7]);
  DDX_Text(pDX, IDC_UNKNOWN6, the_area.songheader.songs[8]);
  DDX_Text(pDX, IDC_UNKNOWN7, the_area.songheader.songs[9]);

  RetrieveResref(tmpstr,the_area.songheader.dayambi1);
  DDX_Text(pDX, IDC_AMBI1D, tmpstr);
  DDV_MaxChars(pDX, tmpstr, 8);
  StoreResref(tmpstr,the_area.songheader.dayambi1);

  RetrieveResref(tmpstr,the_area.songheader.dayambi2);
  DDX_Text(pDX, IDC_AMBI2D, tmpstr);
  DDV_MaxChars(pDX, tmpstr, 8);
  StoreResref(tmpstr,the_area.songheader.dayambi2);

  RetrieveResref(tmpstr,the_area.songheader.nightambi1);
  DDX_Text(pDX, IDC_AMBI1N, tmpstr);
  DDV_MaxChars(pDX, tmpstr, 8);
  StoreResref(tmpstr,the_area.songheader.nightambi1);

  RetrieveResref(tmpstr,the_area.songheader.nightambi2);
  DDX_Text(pDX, IDC_AMBI2N, tmpstr);
  DDV_MaxChars(pDX, tmpstr, 8);
  StoreResref(tmpstr,the_area.songheader.nightambi2);

  DDX_Text(pDX, IDC_VOLUMED, the_area.songheader.volumed);
  DDX_Text(pDX, IDC_VOLUMEN, the_area.songheader.volumen);
  DDX_Text(pDX, IDC_FLAGS, the_area.songheader.songflag);
}

BEGIN_MESSAGE_MAP(CAreaSong, CDialog)
	//{{AFX_MSG_MAP(CAreaSong)
	ON_EN_KILLFOCUS(IDC_AMBI1D, DefaultKillfocus)
	ON_BN_CLICKED(IDC_CLEAR, OnClear)
	ON_BN_CLICKED(IDC_MUSDAY, OnMusday)
	ON_BN_CLICKED(IDC_MUSNIGHT, OnMusnight)
	ON_BN_CLICKED(IDC_MUSWIN, OnMuswin)
	ON_BN_CLICKED(IDC_MUSBATTLE, OnMusbattle)
	ON_BN_CLICKED(IDC_MUSLOSE, OnMuslose)
	ON_BN_CLICKED(IDC_PLAY, OnPlay)
	ON_BN_CLICKED(IDC_PLAY1, OnPlay1)
	ON_BN_CLICKED(IDC_PLAY2, OnPlay2)
	ON_BN_CLICKED(IDC_PLAY3, OnPlay3)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_BROWSE1, OnBrowse1)
	ON_BN_CLICKED(IDC_BROWSE3, OnBrowse3)
	ON_BN_CLICKED(IDC_BROWSE2, OnBrowse2)
	ON_EN_KILLFOCUS(IDC_AMBI2D, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_VOLUMED, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_AMBI1N, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_AMBI2N, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_VOLUMEN, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_FLAGS, DefaultKillfocus)
	ON_CBN_KILLFOCUS(IDC_DAY, DefaultKillfocus)
	ON_CBN_KILLFOCUS(IDC_NIGHT, DefaultKillfocus)
	ON_CBN_KILLFOCUS(IDC_UNKNOWN1, DefaultKillfocus)
	ON_CBN_KILLFOCUS(IDC_BATTLE, DefaultKillfocus)
	ON_CBN_KILLFOCUS(IDC_UNKNOWN2, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_UNKNOWN3, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_UNKNOWN4, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_UNKNOWN5, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_UNKNOWN6, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_UNKNOWN7, DefaultKillfocus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAreaSong message handlers

void CAreaSong::DefaultKillfocus() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CAreaSong::OnClear() 
{
  memset(&the_area.songheader,0,sizeof(area_song));
  memset(the_area.songheader.songs,-1,sizeof(the_area.songheader.songs) );
  the_area.songheader.volumed=the_area.songheader.volumen=100;
  
  if(pst_compatible_var())
  {
    the_area.songheader.songs[0]=17; 
    the_area.songheader.songs[1]=17; 
    the_area.songheader.songs[3]=27; 
  }
  else if(iwd2_structures())//iwd2 specific
  {
    the_area.songheader.songs[0]=4; 
    the_area.songheader.songs[1]=4; 
    the_area.songheader.songs[3]=0; 
  }
  else if(has_xpvar()) //iwd1
  {
    the_area.songheader.songs[0]=0; 
    the_area.songheader.songs[1]=-1;
    the_area.songheader.songs[3]=37;
  }
  else//bg2 specific
  {
    the_area.songheader.songs[0]=11; //day song
    the_area.songheader.songs[1]=45; //night song
    the_area.songheader.songs[3]=55; //battlesong
  }
	UpdateData(UD_DISPLAY);
}

void CAreaSong::Musiclist(int idx)
{
  CString tmpname;
	CMUSEdit dlg;

  tmpname=itemname;
  itemname=get_songfile(idx);
  if(read_mus(itemname))
  {
    MessageBox("Unknown song","Area editor",MB_ICONEXCLAMATION|MB_OK);
    itemname=tmpname;
    return;
  }
  dlg.DoModal();
  itemname=tmpname;
}

void CAreaSong::OnMusday() 
{
  Musiclist(the_area.songheader.songs[0]);
}

void CAreaSong::OnMusnight() 
{
  Musiclist(the_area.songheader.songs[1]);
}

void CAreaSong::OnMuswin() 
{
  Musiclist(the_area.songheader.songs[2]);
}

void CAreaSong::OnMusbattle() 
{
  Musiclist(the_area.songheader.songs[3]);
}

void CAreaSong::OnMuslose() 
{
  Musiclist(the_area.songheader.songs[4]);
}

void CAreaSong::OnPlay() 
{
	play_acm(the_area.songheader.dayambi1,false,false);
}

void CAreaSong::OnBrowse() 
{
  pickerdlg.m_restype=REF_WAV;
  RetrieveResref(pickerdlg.m_picked,the_area.songheader.dayambi1);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_area.songheader.dayambi1);
  }
	UpdateData(UD_DISPLAY);	
}

void CAreaSong::OnPlay1() 
{
	play_acm(the_area.songheader.nightambi1,false,false);
}

void CAreaSong::OnBrowse1() 
{
  pickerdlg.m_restype=REF_WAV;
  RetrieveResref(pickerdlg.m_picked,the_area.songheader.nightambi1);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_area.songheader.nightambi1);
  }
	UpdateData(UD_DISPLAY);	
}

void CAreaSong::OnPlay2() 
{
	play_acm(the_area.songheader.dayambi2,false,false);
}

void CAreaSong::OnBrowse2() 
{
  pickerdlg.m_restype=REF_WAV;
  RetrieveResref(pickerdlg.m_picked,the_area.songheader.dayambi2);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_area.songheader.dayambi2);
  }
	UpdateData(UD_DISPLAY);	
}

void CAreaSong::OnPlay3() 
{
	play_acm(the_area.songheader.nightambi2,false,false);
}

void CAreaSong::OnBrowse3() 
{
  pickerdlg.m_restype=REF_WAV;
  RetrieveResref(pickerdlg.m_picked,the_area.songheader.nightambi2);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_area.songheader.nightambi2);
  }
	UpdateData(UD_DISPLAY);	
}

BOOL CAreaSong::PreTranslateMessage(MSG* pMsg) 
{
  m_tooltip.RelayEvent(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}
