// AreaGeneral.cpp : implementation file
//
  
#include "stdafx.h"
#include "chitem.h"
#include "AreaProperties.h"
#include "AreaSong.h"
#include "AreaInt.h"
#include "Schedule.h"
#include "options.h"
#include "ImageView.h"
#include "WedEdit.h"
#include "CreatureEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CAreaGeneral, CPropertyPage)
IMPLEMENT_DYNCREATE(CAreaActor, CPropertyPage)
IMPLEMENT_DYNCREATE(CAreaTrigger, CPropertyPage)
IMPLEMENT_DYNCREATE(CAreaSpawn, CPropertyPage)
IMPLEMENT_DYNCREATE(CAreaEntrance, CPropertyPage)
IMPLEMENT_DYNCREATE(CAreaContainer, CPropertyPage)
IMPLEMENT_DYNCREATE(CAreaAmbient, CPropertyPage)
IMPLEMENT_DYNCREATE(CAreaDoor, CPropertyPage)
IMPLEMENT_DYNCREATE(CAreaVariable, CPropertyPage)
IMPLEMENT_DYNCREATE(CAreaAnim, CPropertyPage)
IMPLEMENT_DYNCREATE(CAreaMap, CPropertyPage)

static unsigned long defschedule;

int SetupSelectPoint()
{
  CString tmpstr;

  if(the_area.overlayheaders)
  {
    RetrieveResref(tmpstr, the_area.overlayheaders[0].tis);
  }
  if(read_tis(tmpstr,&the_mos,true))
  {
    MessageBox(0,"Cannot load TIS.","Warning",MB_ICONEXCLAMATION|MB_OK);
    return -1;
  }
  the_mos.TisToMos(the_area.overlayheaders[0].width,the_area.overlayheaders[0].height);
  return 0;
}

static void RecalcBox(int count, int idx, POINTS &min, POINTS &max)
{
  POINTS tmp;
  POSITION pos;
  area_vertex *poi;
  int i;

  if(!count)
  {
    max.x=-1;
    max.y=-1;
    min.x=-1;
    min.y=-1;
    return;
  }
  pos=the_area.vertexheaderlist.FindIndex(idx);
  poi=(area_vertex *) the_area.vertexheaderlist.GetAt(pos);
  min=max=poi[0].point;
  for(i=1;i<count;i++)
  {
    tmp=poi[i].point;
    if(max.x<tmp.x) max.x=tmp.x;
    if(max.y<tmp.y) max.y=tmp.y;
    if(min.x>tmp.x) min.x=tmp.x;
    if(min.y>tmp.y) min.y=tmp.y;
  }
}

/////////////////////////////////////////////////////////////////////////////
// CAreaGeneral dialog

CAreaGeneral::CAreaGeneral() : CPropertyPage(CAreaGeneral::IDD)
{
	//{{AFX_DATA_INIT(CAreaGeneral)
	//}}AFX_DATA_INIT
}

CAreaGeneral::~CAreaGeneral()
{
}

static int attrboxids[8]={
  IDC_OUTDOOR,IDC_DAYNIGHT,IDC_WEATHER,IDC_CITY,IDC_FOREST,
    IDC_DUNGEON,IDC_EXTENDEDNIGHT,IDC_CANREST};

void CAreaGeneral::DoDataExchange(CDataExchange* pDX)
{
  CButton *cb, *cb2;
  CString tmpstr, old;
  int i,j;

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAreaGeneral)
	DDX_Control(pDX, IDC_AREATYPE, m_areatype_control);
	//}}AFX_DATA_MAP
  RetrieveResref(tmpstr, the_area.header.wed);
  old=tmpstr;
  DDX_Text(pDX, IDC_WED, tmpstr);
  DDV_MaxChars(pDX, tmpstr, 8); //night wedfile!! areas should be 6 character length
  if(tmpstr.IsEmpty()) tmpstr=itemname;
  if(tmpstr!=old) the_area.wedchanged=true;
  StoreResref(tmpstr, the_area.header.wed);

  tmpstr=get_areatype(the_area.header.areatype);
  DDX_Text(pDX, IDC_AREATYPE, tmpstr);
  the_area.header.areatype=strtonum(tmpstr);

  DDX_Text(pDX, IDC_LASTSAVED, the_area.header.lastsaved);

  RetrieveResref(tmpstr, the_area.header.northref);
  DDX_Text(pDX, IDC_NORTHRES, tmpstr);
  DDV_MaxChars(pDX, tmpstr, 8);
  StoreResref(tmpstr, the_area.header.northref);

  RetrieveResref(tmpstr, the_area.header.westref);
  DDX_Text(pDX, IDC_WESTRES, tmpstr);
  DDV_MaxChars(pDX, tmpstr, 8);
  StoreResref(tmpstr, the_area.header.westref);

  RetrieveResref(tmpstr, the_area.header.southref);
  DDX_Text(pDX, IDC_SOUTHRES, tmpstr);
  DDV_MaxChars(pDX, tmpstr, 8);
  StoreResref(tmpstr, the_area.header.southref);

  RetrieveResref(tmpstr, the_area.header.eastref);
  DDX_Text(pDX, IDC_EASTRES, tmpstr);
  DDV_MaxChars(pDX, tmpstr, 8);
  StoreResref(tmpstr, the_area.header.eastref);

  RetrieveResref(tmpstr, the_area.header.scriptref);
  DDX_Text(pDX, IDC_SCRIPT, tmpstr);
  DDV_MaxChars(pDX, tmpstr, 8);
  StoreResref(tmpstr, the_area.header.scriptref);

  DDX_Text(pDX, IDC_NORTHFLAG, the_area.header.northflags);
  DDX_Text(pDX, IDC_WESTFLAG, the_area.header.westflags);
  DDX_Text(pDX, IDC_SOUTHFLAG, the_area.header.southflags);
  DDX_Text(pDX, IDC_EASTFLAG, the_area.header.eastflags);

  DDX_Text(pDX, IDC_RAIN, the_area.header.rain);
  DDX_Text(pDX, IDC_SNOW, the_area.header.snow);
  DDX_Text(pDX, IDC_FOG, the_area.header.fog);
  DDX_Text(pDX, IDC_LIGHTNING, the_area.header.lightning);
  DDX_Text(pDX, IDC_UNKNOWN52, the_area.header.unknown52);

  DDX_Text(pDX, IDC_UNKFLAG, the_area.header.unkflags);

  DDX_Text(pDX, IDC_UNKNOWN90, the_area.header.unknown90);

  cb=(CButton *) GetDlgItem(IDC_SONG);
  cb2=(CButton *) GetDlgItem(IDC_SONGS);
  if(the_area.header.songoffset)
  {
    cb->SetCheck(true);
    cb2->EnableWindow(true);
  }
  else
  {
    cb->SetCheck(false);
    cb2->EnableWindow(false);
  }

  cb=(CButton *) GetDlgItem(IDC_REST);
  cb2=(CButton *) GetDlgItem(IDC_INT);
  if(the_area.header.intoffset)
  {
    cb->SetCheck(true);
    cb2->EnableWindow(true);
  }
  else
  {
    cb->SetCheck(false);
    cb2->EnableWindow(false);
  }

  cb2=(CButton *) GetDlgItem(IDC_DELEXP); //remove explored bitmap
  cb2->EnableWindow(!!the_area.header.exploredsize);

  cb2=(CButton *) GetDlgItem(IDC_UNKNOWN); //extra unknowns for IWD2
  cb2->EnableWindow(the_area.revision==91);

  cb2=(CButton *) GetDlgItem(IDC_VIEW); 
  cb2->EnableWindow(memcmp(the_area.header.wed,"NEW AREA",8) );

  if(pDX->m_bSaveAndValidate==UD_DISPLAY)
  {
    j=1;
    for(i=0;i<8;i++)
    {
      cb=(CButton *) GetDlgItem(attrboxids[i]);
      if(the_area.header.areaflags&j) cb->SetCheck(true);
      else cb->SetCheck(false);
      j<<=1;
    }
  }
}

static int radioids[4]={IDC_V10,IDC_V91};

//move itemdata to local variables
void CAreaGeneral::RefreshGeneral()
{
  int i;
  int id;
  CButton *cb;
  CWnd *cw;

  switch(the_area.revision)
  {
  case 10:
    id=0;
    break;
  case 91:
    id=1;
    break;
  }
  for(i=0;i<2;i++)
  {
    cb=(CButton *) GetDlgItem(radioids[i]);
    if(i==id) cb->SetCheck(true);
    else cb->SetCheck(false);
  }
  cw=GetDlgItem(IDC_EDIT);
  cw->EnableWindow(the_area.WedAvailable());
  cw=GetDlgItem(IDC_NIGHT);
  //night wed seems to be present only in BG2/ToB
  cw->EnableWindow(bg2_weaprofs() && the_area.WedAvailable()&&(the_area.header.areaflags&EXTENDED_NIGHT) );
}

static int bg1areaids[8]={IDC_NORTHRES, IDC_NORTHFLAG,IDC_WESTRES, IDC_WESTFLAG,
IDC_SOUTHRES,IDC_SOUTHFLAG,IDC_EASTRES,IDC_EASTFLAG};

BOOL CAreaGeneral::OnInitDialog() 
{
  int i;
  int flg;
  CWnd *cb;

	CPropertyPage::OnInitDialog();
	RefreshGeneral();
  m_areatype_control.ResetContent();
  flg=bg1_compatible_area();
  for(i=0;i<8;i++)
  {
    cb=GetDlgItem(bg1areaids[i]);
    cb->EnableWindow(flg);
  }

  m_areatype_control.AddString(get_areatype(0) );
  flg=1;
  for(i=0;i<NUM_ARTYPE;i++)
  {
    m_areatype_control.AddString(get_areatype(flg) );
    flg<<=1;
  }
  UpdateData(UD_DISPLAY);
	return TRUE;
}

BEGIN_MESSAGE_MAP(CAreaGeneral, CPropertyPage)
	//{{AFX_MSG_MAP(CAreaGeneral)
	ON_BN_CLICKED(IDC_OUTDOOR, OnOutdoor)
	ON_BN_CLICKED(IDC_DAYNIGHT, OnDaynight)
	ON_BN_CLICKED(IDC_WEATHER, OnWeather)
	ON_BN_CLICKED(IDC_CITY, OnCity)
	ON_BN_CLICKED(IDC_FOREST, OnForest)
	ON_BN_CLICKED(IDC_DUNGEON, OnDungeon)
	ON_BN_CLICKED(IDC_EXTENDEDNIGHT, OnExtendednight)
	ON_BN_CLICKED(IDC_CANREST, OnCanrest)
	ON_BN_CLICKED(IDC_V10, OnV10)
	ON_BN_CLICKED(IDC_V91, OnV91)
	ON_BN_CLICKED(IDC_SONGS, OnSongs)
	ON_BN_CLICKED(IDC_INT, OnInt)
	ON_BN_CLICKED(IDC_SONG, OnSong)
	ON_BN_CLICKED(IDC_REST, OnRest)
	ON_BN_CLICKED(IDC_DELEXP, OnDelexp)
	ON_BN_CLICKED(IDC_UNKNOWN, OnUnknown)
	ON_BN_CLICKED(IDC_VIEW, OnView)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	ON_BN_CLICKED(IDC_BROWSE2, OnBrowse2)
	ON_EN_KILLFOCUS(IDC_WED, DefaultKillfocus)
	ON_CBN_KILLFOCUS(IDC_AREATYPE, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_LASTSAVED, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_SCRIPT, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_UNKNOWN90, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_NORTHRES, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_WESTRES, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_SOUTHRES, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_EASTRES, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_NORTHFLAG, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_WESTFLAG, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_SOUTHFLAG, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_EASTFLAG, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_RAIN, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_SNOW, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_FOG, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_LIGHTNING, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_UNKNOWN52, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_UNKFLAG, DefaultKillfocus)
	ON_BN_CLICKED(IDC_NIGHT, OnNight)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAreaGeneral message handlers

void CAreaGeneral::DefaultKillfocus()
{
  UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CAreaGeneral::OnOutdoor() 
{
	the_area.header.areaflags^=1;
	UpdateData(UD_DISPLAY);
}

void CAreaGeneral::OnDaynight() 
{
	the_area.header.areaflags^=2;
	UpdateData(UD_DISPLAY);
}

void CAreaGeneral::OnWeather() 
{
	the_area.header.areaflags^=4;
	UpdateData(UD_DISPLAY);
}

void CAreaGeneral::OnCity() 
{
	the_area.header.areaflags^=8;
	UpdateData(UD_DISPLAY);
}

void CAreaGeneral::OnForest() 
{
	the_area.header.areaflags^=16;
	UpdateData(UD_DISPLAY);
}

void CAreaGeneral::OnDungeon() 
{
	the_area.header.areaflags^=32;
	UpdateData(UD_DISPLAY);
}

void CAreaGeneral::OnExtendednight() 
{
	the_area.header.areaflags^=EXTENDED_NIGHT; //64
	UpdateData(UD_DISPLAY);
}

void CAreaGeneral::OnCanrest() 
{
	the_area.header.areaflags^=128;
	UpdateData(UD_DISPLAY);
}

void CAreaGeneral::OnSongs() 
{
	CAreaSong dlg;
	
  dlg.DoModal();
}

void CAreaGeneral::OnInt() 
{
	CAreaInt dlg;
	
  dlg.DoModal();
}

void CAreaGeneral::OnSong() 
{
	the_area.header.songoffset=!the_area.header.songoffset;
  UpdateData(UD_DISPLAY);
}

void CAreaGeneral::OnRest() 
{
	the_area.header.intoffset=!the_area.header.intoffset;
  UpdateData(UD_DISPLAY);
}

void CAreaGeneral::OnDelexp() 
{
	if(the_area.explored)
  {
    the_area.KillExplored();
    the_area.header.exploredsize=0;
    the_area.header.exploredoffset=0;
  }
  UpdateData(UD_DISPLAY);
}

void CAreaGeneral::OnUnknown() 
{
	// TODO: Add your control notification handler code here
	//edit iwd unknowns
  UpdateData(UD_DISPLAY);
}

void CAreaGeneral::OnView() 
{
  CImageView dlg;
  CString tmpstr;

  RetrieveResref(tmpstr, the_area.header.wed);
  if(read_mos(tmpstr,&the_mos,true)<0)
  {
    MessageBox("Error while loading minimap.","Area editor",MB_OK);
    return;
  }
  dlg.InitView(IW_ENABLEBUTTON);
  dlg.DoModal();
}

void CAreaGeneral::OnV10() 
{
	the_area.revision=10;
  UpdateData(UD_DISPLAY);
}

void CAreaGeneral::OnV91() 
{
	the_area.revision=91;
  UpdateData(UD_DISPLAY);
}

void CAreaGeneral::OnBrowse() 
{
  pickerdlg.m_restype=REF_WED;
  RetrieveResref(pickerdlg.m_picked,the_area.header.wed);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_area.header.wed);
  }
	UpdateData(UD_DISPLAY);	
}

void CAreaGeneral::OnBrowse2() 
{
  pickerdlg.m_restype=REF_BCS;
  RetrieveResref(pickerdlg.m_picked,the_area.header.scriptref);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_area.header.scriptref);
  }
	UpdateData(UD_DISPLAY);	
}

void CAreaGeneral::OnEdit() 
{
	CWedEdit dlg;
	
  dlg.m_night=FALSE;
  dlg.DoModal();
}

void CAreaGeneral::OnNight() 
{
	CWedEdit dlg;
	  
  dlg.m_night=TRUE;
  dlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CAreaActor dialog

CAreaActor::CAreaActor() : CPropertyPage(CAreaActor::IDD)
{
	//{{AFX_DATA_INIT(CAreaActor)
	m_timeofday = _T("");
	//}}AFX_DATA_INIT
  m_actornum=-1;
  memset(&actorcopy,0,sizeof(actorcopy));
}

CAreaActor::~CAreaActor()
{
}

static int actorboxids[]={IDC_ACTORPICKER, IDC_CRERES, IDC_POSX, IDC_POSY,
IDC_DESTX,IDC_DESTY, IDC_VISIBLE, IDC_U2C, IDC_FACE, IDC_ANIMATION,
IDC_U36,IDC_U38,IDC_U3C,IDC_TALKNUM,IDC_DIALOG, IDC_OVERRIDE, IDC_CLASS,
IDC_RACE, IDC_GENERAL, IDC_DEFAULT, IDC_SPECIFIC, IDC_SAME,IDC_SCHEDULE,
IDC_CLEAR,IDC_FIELDS, IDC_BROWSE, IDC_COPY, IDC_PASTE, IDC_REMOVE,IDC_BROWSE2,
IDC_BROWSE3,IDC_BROWSE4,IDC_BROWSE5,IDC_BROWSE6,IDC_BROWSE7,IDC_BROWSE8,
IDC_SETPOS, IDC_SETDEST,
0};

void CAreaActor::DoDataExchange(CDataExchange* pDX)
{
  CWnd *cb;
  CString tmpstr;
  int flg;
  int i;

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAreaActor)
	DDX_Control(pDX, IDC_ACTORPICKER, m_actorpicker);
	DDX_Control(pDX, IDC_SCHEDULE, m_schedule_control);
	DDX_Text(pDX, IDC_TIMEOFDAY, m_timeofday);
	//}}AFX_DATA_MAP
  cb=GetDlgItem(IDC_MAXACTOR);
  flg=m_actornum>=0;
  tmpstr.Format("/ %d",the_area.actorcount);
  cb->SetWindowText(tmpstr);
  i=0;
  while(actorboxids[i])
  {
    cb=GetDlgItem(actorboxids[i++]);
    cb->EnableWindow(flg);
  }
  cb=GetDlgItem(IDC_EDIT);
  if(flg)
  {
    cb->EnableWindow(!!the_area.actorheaders[m_actornum].creoffset);
    tmpstr.Format("%d. %-.32s",m_actornum+1,the_area.actorheaders[m_actornum].actorname);
    DDX_Text(pDX, IDC_ACTORPICKER, tmpstr);    

    DDX_Text(pDX, IDC_POSX, the_area.actorheaders[m_actornum].posx);
    DDX_Text(pDX, IDC_POSY, the_area.actorheaders[m_actornum].posy);
    DDX_Text(pDX, IDC_DESTX, the_area.actorheaders[m_actornum].destx);
    DDX_Text(pDX, IDC_DESTY, the_area.actorheaders[m_actornum].desty);
    DDX_Text(pDX, IDC_VISIBLE, the_area.actorheaders[m_actornum].visible);

    if(the_area.actorheaders[m_actornum].unknown2c<10)
    {
      tmpstr.Format("%d",the_area.actorheaders[m_actornum].unknown2c);
    }
    else
    {
      tmpstr.Format("0x%x",the_area.actorheaders[m_actornum].unknown2c);
    }
    DDX_Text(pDX, IDC_U2C,tmpstr);
    the_area.actorheaders[m_actornum].unknown2c=strtonum(tmpstr);

    tmpstr.Format("0x%04x %s",the_area.actorheaders[m_actornum].animation,IDSToken("ANIMATE",the_area.actorheaders[m_actornum].animation) );
    DDX_Text(pDX, IDC_ANIMATION, tmpstr);
    the_area.actorheaders[m_actornum].animation=IDSKey("ANIMATE", tmpstr);
    if(the_area.actorheaders[m_actornum].animation==-1)
    {
      the_area.actorheaders[m_actornum].animation=strtonum(tmpstr);
    }
    
    //this is a short number
    tmpstr=format_direction(the_area.actorheaders[m_actornum].face);
    DDX_Text(pDX, IDC_FACE,tmpstr);
    the_area.actorheaders[m_actornum].face=(short) strtonum(tmpstr);

    tmpstr.Format("%d",the_area.actorheaders[m_actornum].unknown36);
    DDX_Text(pDX, IDC_U36,tmpstr);
    the_area.actorheaders[m_actornum].unknown36=(short) strtonum(tmpstr);

    tmpstr.Format("%d",the_area.actorheaders[m_actornum].unknown38);
    DDX_Text(pDX, IDC_U38,tmpstr);
    the_area.actorheaders[m_actornum].unknown38=strtonum(tmpstr);

    tmpstr.Format("%d",the_area.actorheaders[m_actornum].unknown3c);
    DDX_Text(pDX, IDC_U3C,tmpstr);
    the_area.actorheaders[m_actornum].unknown3c=strtonum(tmpstr);

    DDX_Text(pDX, IDC_TALKNUM,the_area.actorheaders[m_actornum].talknum);

    RetrieveResref(tmpstr,the_area.actorheaders[m_actornum].creresref);
    DDX_Text(pDX, IDC_CRERES, tmpstr);
    DDV_MaxChars(pDX, tmpstr, 8);
    StoreResref(tmpstr,the_area.actorheaders[m_actornum].creresref);

    RetrieveResref(tmpstr,the_area.actorheaders[m_actornum].scroverride);
    DDX_Text(pDX, IDC_OVERRIDE, tmpstr);
    DDV_MaxChars(pDX, tmpstr, 8);
    StoreResref(tmpstr,the_area.actorheaders[m_actornum].scroverride);

    RetrieveResref(tmpstr,the_area.actorheaders[m_actornum].scrclass);
    DDX_Text(pDX, IDC_CLASS, tmpstr);
    DDV_MaxChars(pDX, tmpstr, 8);
    StoreResref(tmpstr,the_area.actorheaders[m_actornum].scrclass);

    RetrieveResref(tmpstr,the_area.actorheaders[m_actornum].scrrace);
    DDX_Text(pDX, IDC_RACE, tmpstr);
    DDV_MaxChars(pDX, tmpstr, 8);
    StoreResref(tmpstr,the_area.actorheaders[m_actornum].scrrace);

    RetrieveResref(tmpstr,the_area.actorheaders[m_actornum].scrgeneral);
    DDX_Text(pDX, IDC_GENERAL, tmpstr);
    DDV_MaxChars(pDX, tmpstr, 8);
    StoreResref(tmpstr,the_area.actorheaders[m_actornum].scrgeneral);

    RetrieveResref(tmpstr,the_area.actorheaders[m_actornum].scrdefault);
    DDX_Text(pDX, IDC_DEFAULT, tmpstr);
    DDV_MaxChars(pDX, tmpstr, 8);
    StoreResref(tmpstr,the_area.actorheaders[m_actornum].scrdefault);

    RetrieveResref(tmpstr,the_area.actorheaders[m_actornum].scrspecific);
    DDX_Text(pDX, IDC_SPECIFIC, tmpstr);
    DDV_MaxChars(pDX, tmpstr, 8);
    StoreResref(tmpstr,the_area.actorheaders[m_actornum].scrspecific);

    RetrieveResref(tmpstr,the_area.actorheaders[m_actornum].dialog);
    DDX_Text(pDX, IDC_DIALOG, tmpstr);
    DDV_MaxChars(pDX, tmpstr, 8);
    StoreResref(tmpstr,the_area.actorheaders[m_actornum].dialog);
  }
  else cb->EnableWindow(false);
}

//move itemdata to local variables
void CAreaActor::RefreshActor()
{
  CString tmpstr;
	CSchedule tmp;
  int i;

  if(m_actornum<0 || m_actornum>=the_area.header.actorcnt)
  {
    if(the_area.header.actorcnt) m_actornum=0;
    else m_actornum=-1;
  }
  if(IsWindow(m_actorpicker) )
  {
    m_actorpicker.ResetContent();
    for(i=0;i<the_area.actorcount;i++)
    {
      tmpstr.Format("%d. %-.32s",i+1,the_area.actorheaders[i].actorname);
      m_actorpicker.AddString(tmpstr);
    }
    m_actornum=m_actorpicker.SetCurSel(m_actornum);
  }
  if(m_actornum>=0)
  {
    tmp.m_schedule=the_area.actorheaders[m_actornum].schedule;
    if(IsWindow(m_schedule_control) )
    {
      m_schedule_control.SetWindowText(tmp.GetCaption());
    }
    m_timeofday=tmp.GetTimeOfDay();
  }
  else
  {
    if(IsWindow(m_schedule_control) )
    {
      
      m_schedule_control.SetWindowText("-- not available --");    
    }
    m_timeofday="";
  }
}

BOOL CAreaActor::OnInitDialog() 
{
  CComboBox *cb;
  int i;

	CPropertyPage::OnInitDialog();
	RefreshActor();
  cb=(CComboBox *) GetDlgItem(IDC_FACE);
  for(i=0;i<NUM_FVALUES;i++)
  {
    cb->AddString(format_direction(i));
  }

	cb=(CComboBox *) GetDlgItem(IDC_ANIMATION);
  cb->SetDroppedWidth(200);
  FillCombo("ANIMATE",cb,4);

  UpdateData(UD_DISPLAY);
	return TRUE;
}

BEGIN_MESSAGE_MAP(CAreaActor, CPropertyPage)
	//{{AFX_MSG_MAP(CAreaActor)
	ON_BN_CLICKED(IDC_SCHEDULE, OnSchedule)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_COPY, OnCopy)
	ON_BN_CLICKED(IDC_PASTE, OnPaste)
	ON_CBN_KILLFOCUS(IDC_ACTORPICKER, OnKillfocusActorpicker)
	ON_CBN_SELCHANGE(IDC_ACTORPICKER, OnSelchangeActorpicker)
	ON_BN_CLICKED(IDC_FIELDS, OnFields)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_BROWSE2, OnBrowse2)
	ON_BN_CLICKED(IDC_BROWSE3, OnBrowse3)
	ON_BN_CLICKED(IDC_BROWSE4, OnBrowse4)
	ON_BN_CLICKED(IDC_BROWSE5, OnBrowse5)
	ON_BN_CLICKED(IDC_BROWSE6, OnBrowse6)
	ON_BN_CLICKED(IDC_BROWSE7, OnBrowse7)
	ON_BN_CLICKED(IDC_BROWSE8, OnBrowse8)
	ON_BN_CLICKED(IDC_SAME, OnSame)
	ON_BN_CLICKED(IDC_CLEAR, OnClear)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	ON_BN_CLICKED(IDC_SETPOS, OnSetpos)
	ON_BN_CLICKED(IDC_SETDEST, OnSetdest)
	ON_EN_KILLFOCUS(IDC_CRERES, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_POSX, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_POSY, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_DESTX, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_DESTY, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_VISIBLE, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U2C, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U38, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U3C, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_TALKNUM, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_DIALOG, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_OVERRIDE, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_CLASS, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_RACE, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_GENERAL, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_DEFAULT, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_SPECIFIC, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U36, DefaultKillfocus)
	ON_CBN_KILLFOCUS(IDC_FACE, DefaultKillfocus)
	ON_CBN_KILLFOCUS(IDC_ANIMATION, DefaultKillfocus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAreaActor message handlers

void CAreaActor::OnAdd() 
{
  area_actor *newactors;
  char **newdatapointers;
  CString tmpstr;
  
  if(the_area.header.actorcnt>999)
  {
    MessageBox("Cannot add more actors.","Warning",MB_ICONEXCLAMATION|MB_OK);
    return;
  }
  newactors=new area_actor[++the_area.header.actorcnt];
  if(!newactors)
  {
    the_area.header.actorcnt--;
    MessageBox("Not enough memory.","Error",MB_ICONSTOP|MB_OK);
    return;
  }
  newdatapointers=new char *[the_area.header.actorcnt];
  if(!newdatapointers)
  {
    the_area.header.actorcnt--;
    delete [] newactors;
    MessageBox("Not enough memory.","Error",MB_ICONSTOP|MB_OK);
    return;
  }

  if(the_area.actorheaders)
  {
    memcpy(newactors, the_area.actorheaders, the_area.actorcount*sizeof(area_actor));
    delete [] the_area.actorheaders;
  }
  if(the_area.credatapointers)
  {
    memcpy(newdatapointers,the_area.credatapointers,the_area.actorcount*sizeof(char *) );    
    delete [] the_area.credatapointers;
  }

  memset(newactors+the_area.actorcount,0,sizeof(area_actor) );
  newdatapointers[the_area.actorcount]=NULL;
  tmpstr.Format("Actor %d",the_area.header.actorcnt);
  StoreName(tmpstr,newactors[the_area.actorcount].actorname);
  newactors[the_area.actorcount].visible=1;
  newactors[the_area.actorcount].schedule=0xffffff;
  newactors[the_area.actorcount].unknown38=-1;

  the_area.actorheaders=newactors;
  the_area.credatapointers=newdatapointers;
  m_actornum=the_area.actorcount; //last element 
  the_area.actorcount=the_area.header.actorcnt;

  RefreshActor();
	UpdateData(UD_DISPLAY);
}

void CAreaActor::OnRemove() 
{
  area_actor *newactors;
  char **newdatapointers;

  if(m_actornum<0 || !the_area.header.actorcnt) return;
  
  newactors=new area_actor[--the_area.header.actorcnt];
  if(!newactors)
  {
    the_area.header.actorcnt++;
    return;
  }
  newdatapointers=new char *[the_area.header.actorcnt];
  if(!newdatapointers)
  {
    delete [] newactors;
    the_area.header.actorcnt++;
    return;
  }
  if(the_area.credatapointers)
  {
    memcpy(newdatapointers,the_area.credatapointers,m_actornum*sizeof(char *) );
    memcpy(newdatapointers+m_actornum,the_area.credatapointers+m_actornum+1,(the_area.header.actorcnt-m_actornum)*sizeof(char *) );
    delete [] the_area.credatapointers;
  }
  if(the_area.actorheaders)
  {
    memcpy(newactors,the_area.actorheaders,m_actornum*sizeof(area_actor) );
    memcpy(newactors+m_actornum,the_area.actorheaders+m_actornum+1,(the_area.header.actorcnt-m_actornum)*sizeof(area_actor) );
    delete [] the_area.actorheaders;
  }

  the_area.actorheaders=newactors;
  the_area.credatapointers=newdatapointers;
  the_area.actorcount=the_area.header.actorcnt;
	if(m_actornum>=the_area.actorcount) m_actornum--;
  RefreshActor();
	UpdateData(UD_DISPLAY);
}

void CAreaActor::OnCopy() 
{
  if(m_actornum>=0)
  {
    memcpy(&actorcopy,the_area.actorheaders+m_actornum,sizeof(actorcopy) );
  }
}

void CAreaActor::OnPaste() 
{
  if(m_actornum>=0)
  {
    memcpy(the_area.actorheaders+m_actornum,&actorcopy, sizeof(actorcopy) );
  }
  RefreshActor();
	UpdateData(UD_DISPLAY);
}

void CAreaActor::OnSetpos() 
{
  CImageView dlg;
  CPoint point;

  if(SetupSelectPoint()) return;
  dlg.InitView(IW_ENABLEBUTTON|IW_SETVERTEX|IW_PLACEIMAGE);
  dlg.SetupAnimationPlacement(&the_bam,  //the cursors are loaded in 'the_bam' now
    the_area.actorheaders[m_actornum].posx,
    the_area.actorheaders[m_actornum].posy);
  if(dlg.DoModal()==IDOK)
  {
    point=dlg.GetPoint(0);
    the_area.actorheaders[m_actornum].posx=(short) point.x;
    the_area.actorheaders[m_actornum].posy=(short) point.y;
  }
  RefreshActor();
  UpdateData(UD_DISPLAY);
}

void CAreaActor::OnSetdest() 
{
  CImageView dlg;
  CPoint point;

  if(SetupSelectPoint()) return;
  dlg.InitView(IW_ENABLEBUTTON|IW_SETVERTEX|IW_PLACEIMAGE);
  dlg.SetupAnimationPlacement(&the_bam,  //the cursors are loaded in 'the_bam' now
    the_area.actorheaders[m_actornum].destx,
    the_area.actorheaders[m_actornum].desty);
  if(dlg.DoModal()==IDOK)
  {
    point=dlg.GetPoint(0);
    the_area.actorheaders[m_actornum].destx=(short) point.x;
    the_area.actorheaders[m_actornum].desty=(short) point.y;
  }
  RefreshActor();
  UpdateData(UD_DISPLAY);
}

void CAreaActor::OnKillfocusActorpicker() 
{
  CString tmpstr;
  int x;

  m_actorpicker.GetWindowText(tmpstr);
  x=strtonum(tmpstr)-1;
  if(x>=0 && x<=the_area.actorcount)
  {
    m_actornum=m_actorpicker.SetCurSel(x);
  }
  else
  {
    if(m_actornum>=0 && m_actornum<=the_area.actorcount)
    {
      StoreName(tmpstr, the_area.actorheaders[m_actornum].actorname);
    }
  }
  RefreshActor();
	UpdateData(UD_DISPLAY);	
}

void CAreaActor::OnSelchangeActorpicker() 
{
  int x;

  x=m_actorpicker.GetCurSel();
  if(x>=0 && x<=the_area.actorcount) m_actornum=x;
  RefreshActor();
	UpdateData(UD_DISPLAY);	
}

void CAreaActor::OnClear() 
{
  the_area.actorheaders[m_actornum].animation=0;
  memset(the_area.actorheaders[m_actornum].dialog,0,8);
  memset(the_area.actorheaders[m_actornum].scroverride,0,8);
  memset(the_area.actorheaders[m_actornum].scrclass,0,8);
  memset(the_area.actorheaders[m_actornum].scrrace,0,8);
  memset(the_area.actorheaders[m_actornum].scrgeneral,0,8);
  memset(the_area.actorheaders[m_actornum].scrdefault,0,8);
  UpdateData(UD_DISPLAY);
}

void CAreaActor::OnFields() 
{
  creature_data credata;
  CString tmpstr;
  loc_entry fileloc;
  int fh;
  
  memset(&credata,0,sizeof(credata) );
  RetrieveResref(tmpstr,the_area.actorheaders[m_actornum].creresref);
  if(creatures.Lookup(tmpstr,fileloc))
  {
    fh=locate_file(fileloc, 0);
    the_creature.RetrieveCreData(fh, credata);
    the_area.actorheaders[m_actornum].animation=credata.animid;

    memcpy(the_area.actorheaders[m_actornum].dialog,credata.dialogresref,8);
    memcpy(the_area.actorheaders[m_actornum].scroverride,credata.scripts[0],8);
    memcpy(the_area.actorheaders[m_actornum].scrclass,credata.scripts[1],8);
    memcpy(the_area.actorheaders[m_actornum].scrrace,credata.scripts[2],8);
    memcpy(the_area.actorheaders[m_actornum].scrgeneral,credata.scripts[3],8);
    memcpy(the_area.actorheaders[m_actornum].scrdefault,credata.scripts[4],8);
  }
  UpdateData(UD_DISPLAY);
}

void CAreaActor::DefaultKillfocus() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);	
}

void CAreaActor::OnBrowse() 
{
  if(m_actornum<0) return;
  pickerdlg.m_restype=REF_CRE;
  RetrieveResref(pickerdlg.m_picked,the_area.actorheaders[m_actornum].creresref);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_area.actorheaders[m_actornum].creresref);
  }
  RefreshActor();
	UpdateData(UD_DISPLAY);	
}

void CAreaActor::OnEdit() 
{
  CCreatureEdit dlg;
  CString tmpname;
  int ret;

  tmpname=itemname;
  if(!the_area.credatapointers[m_actornum]) return;
  RetrieveResref(itemname,the_area.actorheaders[m_actornum].creresref);
  ret=WriteTempCreature(the_area.credatapointers[m_actornum],the_area.actorheaders[m_actornum].cresize);
  if(ret>=0)
  {
    dlg.DoModal();
  }
  itemname=tmpname;
}

void CAreaActor::OnBrowse2() 
{
  if(m_actornum<0) return;
  pickerdlg.m_restype=REF_DLG;
  RetrieveResref(pickerdlg.m_picked,the_area.actorheaders[m_actornum].dialog);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_area.actorheaders[m_actornum].dialog);
  }
  RefreshActor();
	UpdateData(UD_DISPLAY);	
}

void CAreaActor::OnBrowse3() 
{
  if(m_actornum<0) return;
  pickerdlg.m_restype=REF_BCS;
  RetrieveResref(pickerdlg.m_picked,the_area.actorheaders[m_actornum].scroverride);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_area.actorheaders[m_actornum].scroverride);
  }
  RefreshActor();
	UpdateData(UD_DISPLAY);	
}

void CAreaActor::OnBrowse4() 
{
  if(m_actornum<0) return;
  pickerdlg.m_restype=REF_BCS;
  RetrieveResref(pickerdlg.m_picked,the_area.actorheaders[m_actornum].scrclass);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_area.actorheaders[m_actornum].scrclass);
  }
  RefreshActor();
	UpdateData(UD_DISPLAY);	
}

void CAreaActor::OnBrowse5() 
{
  if(m_actornum<0) return;
  pickerdlg.m_restype=REF_BCS;
  RetrieveResref(pickerdlg.m_picked,the_area.actorheaders[m_actornum].scrrace);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_area.actorheaders[m_actornum].scrrace);
  }
  RefreshActor();
	UpdateData(UD_DISPLAY);	
}

void CAreaActor::OnBrowse6() 
{
  if(m_actornum<0) return;
  pickerdlg.m_restype=REF_BCS;
  RetrieveResref(pickerdlg.m_picked,the_area.actorheaders[m_actornum].scrgeneral);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_area.actorheaders[m_actornum].scrgeneral);
  }
  RefreshActor();
	UpdateData(UD_DISPLAY);	
}

void CAreaActor::OnBrowse7() 
{
  if(m_actornum<0) return;
  pickerdlg.m_restype=REF_BCS;
  RetrieveResref(pickerdlg.m_picked,the_area.actorheaders[m_actornum].scrdefault);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_area.actorheaders[m_actornum].scrdefault);
  }
  RefreshActor();
	UpdateData(UD_DISPLAY);	
}

void CAreaActor::OnBrowse8() 
{
  if(m_actornum<0) return;
  pickerdlg.m_restype=REF_BCS;
  RetrieveResref(pickerdlg.m_picked,the_area.actorheaders[m_actornum].scrspecific);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_area.actorheaders[m_actornum].scrspecific);
  }
  RefreshActor();
	UpdateData(UD_DISPLAY);	
}

void CAreaActor::OnSame() 
{
  if(m_actornum<0) return;
  the_area.actorheaders[m_actornum].destx=the_area.actorheaders[m_actornum].posx;
  the_area.actorheaders[m_actornum].desty=the_area.actorheaders[m_actornum].posy;
	UpdateData(UD_DISPLAY);	
}

void CAreaActor::OnSchedule() 
{
	CSchedule dlg;

  if(m_actornum<0) return;
  dlg.m_schedule=the_area.actorheaders[m_actornum].schedule;
  dlg.m_default=defschedule;
  if(dlg.DoModal()==IDOK)
  {
    the_area.actorheaders[m_actornum].schedule=dlg.m_schedule;
    m_schedule_control.SetWindowText(dlg.GetCaption());
    m_timeofday=dlg.GetTimeOfDay();
  }
  defschedule=dlg.m_default; //we copy this always
  UpdateData(UD_DISPLAY);
}

/////////////////////////////////////////////////////////////////////////////
// CAreaTrigger dialog

CAreaTrigger::CAreaTrigger() : CPropertyPage(CAreaTrigger::IDD)
{
	//{{AFX_DATA_INIT(CAreaTrigger)
	m_vx = 0;
	m_vy = 0;
	//}}AFX_DATA_INIT
  m_triggernum=-1;
  memset(&triggercopy,0,sizeof(triggercopy));
  vertexcopy=NULL;
  hbc = 0;
}

CAreaTrigger::~CAreaTrigger()
{
  if(hbc) DeleteObject(hbc);
  if(vertexcopy) delete [] vertexcopy;
  vertexcopy=NULL;
}

static int triggerboxids[]={IDC_TRIGGERPICKER, IDC_REGIONTYPE, IDC_POS1X, IDC_POS1Y,
IDC_POS2X, IDC_POS2Y, IDC_VERTICES, IDC_RECALCBOX, IDC_ADDVERTEX, IDC_REMOVEVERTEX,
IDC_CURSORIDX, IDC_STRREF, IDC_DESTAREA, IDC_PARTY, IDC_ENTRANCENAME, IDC_TPOSX,
IDC_TPOSY,IDC_KEY,IDC_DETECT, IDC_REMOVAL, IDC_SCRIPT, IDC_TRAPPED,IDC_DETECTED,
IDC_DIALOG,IDC_UNKNOWN30,IDC_REMOVE, IDC_COPY, IDC_PASTE, IDC_INFOSTR,
IDC_POS1X2, IDC_POS1Y2,IDC_POS2X2, IDC_POS2Y2,IDC_FLAGS,IDC_VERTEXNUM,

IDC_VX, IDC_VY,IDC_MODVERTEX,IDC_TUNDET, IDC_TRESET, IDC_PARTY, IDC_TDETECT,
IDC_TUNK1, IDC_TUNK2, IDC_TNPC, IDC_TUNK3, IDC_TDEACTIVATED, IDC_NONPC,
IDC_TOVERRIDE, IDC_TDOOR, IDC_TUNK4,IDC_TUNK5,IDC_TUNK6,IDC_TUNK7,

IDC_BROWSE,
IDC_COPY, IDC_PASTE, IDC_REMOVE, IDC_UNKNOWN, IDC_CURSOR, IDC_SET,
0};

static int vertexids[]={IDC_REMOVEVERTEX,IDC_MODVERTEX,IDC_VX,IDC_VY,-1};
static int triggerflagids[]={IDC_TUNDET, IDC_TRESET, IDC_PARTY, IDC_TDETECT,
IDC_TUNK1, IDC_TUNK2, IDC_TNPC, IDC_TUNK3, IDC_TDEACTIVATED, IDC_NONPC,
IDC_TOVERRIDE, IDC_TDOOR, IDC_TUNK4,IDC_TUNK5,IDC_TUNK6,IDC_TUNK7,
0};
#pragma warning(disable:4706)   

void CAreaTrigger::DoDataExchange(CDataExchange* pDX)
{
  CWnd *cw;
  CButton *cb;
  CString tmpstr;
  int flg, flg2;
  int i,j;
  int id;
  int fc;

	CPropertyPage::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_CURSOR, m_cursoricon);
	//{{AFX_DATA_MAP(CAreaTrigger)
	DDX_Control(pDX, IDC_SPINCURSOR, m_spin_control);
	DDX_Control(pDX, IDC_ENTRANCENAME, m_entrancenamepicker);
	DDX_Control(pDX, IDC_VERTICES, m_vertexpicker);
	DDX_Control(pDX, IDC_REGIONTYPE, m_regiontype_control);
	DDX_Control(pDX, IDC_TRIGGERPICKER, m_triggerpicker);
	DDX_Text(pDX, IDC_VX, m_vx);
	DDX_Text(pDX, IDC_VY, m_vy);
	//}}AFX_DATA_MAP
  cw=GetDlgItem(IDC_MAXTRIGGER);
  flg=m_triggernum>=0;
  if(flg) flg2=!!the_area.triggerheaders[m_triggernum].vertexcount;
  else flg2=false;
  tmpstr.Format("/ %d",the_area.triggercount);
  cw->SetWindowText(tmpstr);
  i=0;
  while(id=triggerboxids[i])
  {
    cw=GetDlgItem(id);
    if(member_array(id,vertexids)!=-1 ) cw->EnableWindow(flg2);
    else cw->EnableWindow(flg);
    i++;
  }
  if(flg)
  {
    cw=GetDlgItem(IDC_VERTEXNUM);
    tmpstr.Format("/ %d",the_area.triggerheaders[m_triggernum].vertexcount);
    cw->SetWindowText(tmpstr);

    tmpstr.Format("%d. %-.32s",m_triggernum+1,the_area.triggerheaders[m_triggernum].infoname);
    DDX_Text(pDX, IDC_TRIGGERPICKER, tmpstr);

    tmpstr=get_region_type(the_area.triggerheaders[m_triggernum].triggertype);
    DDX_Text(pDX, IDC_REGIONTYPE, tmpstr);
    the_area.triggerheaders[m_triggernum].triggertype=(short) strtonum(tmpstr);

    DDX_Text(pDX, IDC_POS1X, the_area.triggerheaders[m_triggernum].p1x);
    DDX_Text(pDX, IDC_POS1Y, the_area.triggerheaders[m_triggernum].p1y);
    DDX_Text(pDX, IDC_POS2X, the_area.triggerheaders[m_triggernum].p2x);
    DDX_Text(pDX, IDC_POS2Y, the_area.triggerheaders[m_triggernum].p2y);

    DDX_Text(pDX, IDC_UNKNOWN30, the_area.triggerheaders[m_triggernum].unknown30);
    DDX_Text(pDX, IDC_CURSORIDX, the_area.triggerheaders[m_triggernum].cursortype);
    
    fc=the_bam.GetFrameIndex(the_area.triggerheaders[m_triggernum].cursortype,0);
    the_bam.MakeBitmap(fc,RGB(32,32,32),hbc,BM_RESIZE,32,32);
    m_cursoricon.SetBitmap(hbc);
    
    DDX_Text(pDX, IDC_STRREF, the_area.triggerheaders[m_triggernum].strref);
    DDX_Text(pDX, IDC_INFOSTR, m_infostr);
    
    RetrieveResref(tmpstr, the_area.triggerheaders[m_triggernum].destref);
    DDX_Text(pDX, IDC_DESTAREA, tmpstr);
    DDV_MaxChars(pDX, tmpstr, 8);
    StoreResref(tmpstr, the_area.triggerheaders[m_triggernum].destref);

    RetrieveVariable(tmpstr, the_area.triggerheaders[m_triggernum].destname);
    DDX_Text(pDX, IDC_ENTRANCENAME, tmpstr);
    DDV_MaxChars(pDX, tmpstr, 32);
    StoreName(tmpstr, the_area.triggerheaders[m_triggernum].destname);

    j=1;
    for(i=0;i<16;i++)
    {
      cb=(CButton *) GetDlgItem(triggerflagids[i]);
      if(the_area.triggerheaders[m_triggernum].infoflags&j)
      {
        cb->SetCheck(true);
      }
      else
      {
        cb->SetCheck(false);
      }
      j<<=1;
    }
    DDX_Text(pDX, IDC_FLAGS, the_area.triggerheaders[m_triggernum].infoflags);

    DDX_Text(pDX, IDC_DETECT, the_area.triggerheaders[m_triggernum].trapdetect);
    DDX_Text(pDX, IDC_REMOVAL, the_area.triggerheaders[m_triggernum].trapremove);
    DDX_Text(pDX, IDC_TRAPPED, the_area.triggerheaders[m_triggernum].trapped);
    DDX_Text(pDX, IDC_DETECTED, the_area.triggerheaders[m_triggernum].detected);
    DDX_Text(pDX, IDC_TPOSX, the_area.triggerheaders[m_triggernum].launchx);
    DDX_Text(pDX, IDC_TPOSY, the_area.triggerheaders[m_triggernum].launchy);

    RetrieveResref(tmpstr, the_area.triggerheaders[m_triggernum].key);
    DDX_Text(pDX, IDC_KEY, tmpstr);
    DDV_MaxChars(pDX, tmpstr, 8);
    StoreResref(tmpstr, the_area.triggerheaders[m_triggernum].key);

    RetrieveResref(tmpstr, the_area.triggerheaders[m_triggernum].scriptref);
    DDX_Text(pDX, IDC_SCRIPT, tmpstr);
    DDV_MaxChars(pDX, tmpstr, 8);
    StoreResref(tmpstr, the_area.triggerheaders[m_triggernum].scriptref);

    RetrieveResref(tmpstr, the_area.triggerheaders[m_triggernum].dialogref);
    DDX_Text(pDX, IDC_DIALOG, tmpstr);
    DDV_MaxChars(pDX, tmpstr, 8);
    StoreResref(tmpstr, the_area.triggerheaders[m_triggernum].dialogref);

    DDX_Text(pDX, IDC_POS1X2, the_area.triggerheaders[m_triggernum].ovrp1x);
    DDX_Text(pDX, IDC_POS1Y2, the_area.triggerheaders[m_triggernum].ovrp1y);
    DDX_Text(pDX, IDC_POS2X2, the_area.triggerheaders[m_triggernum].ovrp2x);
    DDX_Text(pDX, IDC_POS2Y2, the_area.triggerheaders[m_triggernum].ovrp2y);

  }
}
#pragma warning(default:4706)   

//move itemdata to local variables
void CAreaTrigger::RefreshTrigger()
{
  CString tmpstr;
  POSITION pos;
  area_vertex *poi;
	CSchedule tmp;
  int i;
  int count;
  int saveidx;

  if(m_triggernum<0 || m_triggernum>=the_area.header.infocnt)
  {
    if(the_area.header.infocnt) m_triggernum=0;
    else m_triggernum=-1;
  }
  if(IsWindow(m_triggerpicker) )
  {
    m_spin_control.SetRange(0,the_bam.GetFrameCount() );
    if(m_triggernum<0) m_triggernum=0;
    m_triggerpicker.ResetContent();
    for(i=0;i<the_area.triggercount;i++)
    {
      tmpstr.Format("%d. %-.32s (%s)",i+1,the_area.triggerheaders[i].infoname,
        get_region_type(the_area.triggerheaders[i].triggertype) );
      m_triggerpicker.AddString(tmpstr);
    }
    m_triggernum=m_triggerpicker.SetCurSel(m_triggernum);
  }
  if(m_triggernum>=0)
  {
    m_infostr=resolve_tlk_text(the_area.triggerheaders[m_triggernum].strref);
    pos=the_area.vertexheaderlist.FindIndex(m_triggernum);
    count=the_area.triggerheaders[m_triggernum].vertexcount;
    if(IsWindow(m_vertexpicker) && pos)
    {
      poi=(area_vertex *) the_area.vertexheaderlist.GetAt(pos);
      saveidx=m_vertexpicker.GetCurSel();
      if(saveidx<0) saveidx=0;
      m_vertexpicker.ResetContent();
      for(i=0;i<count;i++)
      {
        tmpstr.Format("%d. [%d.%d]",i+1,poi[i].point.x,poi[i].point.y);
        m_vertexpicker.AddString(tmpstr);
      }
      m_vertexpicker.SetCurSel(saveidx);
    }
    if(IsWindow(m_entrancenamepicker))
    {
      RetrieveResref(tmpstr, the_area.triggerheaders[m_triggernum].destref);
      fill_destination(tmpstr, &m_entrancenamepicker);
    }
  }
}

BOOL CAreaTrigger::OnInitDialog() 
{
  int i;

	CPropertyPage::OnInitDialog();

  for(i=0;i<NUM_RTTYPE;i++)
  {
    m_regiontype_control.AddString(get_region_type(i));
  }
	RefreshTrigger();
  UpdateData(UD_DISPLAY);
	return TRUE;
}

BEGIN_MESSAGE_MAP(CAreaTrigger, CPropertyPage)
	//{{AFX_MSG_MAP(CAreaTrigger)
	ON_CBN_KILLFOCUS(IDC_TRIGGERPICKER, OnKillfocusTriggerpicker)
	ON_CBN_SELCHANGE(IDC_TRIGGERPICKER, OnSelchangeTriggerpicker)
	ON_BN_CLICKED(IDC_RECALCBOX, OnRecalcbox)
	ON_BN_CLICKED(IDC_ADDVERTEX, OnAddvertex)
	ON_BN_CLICKED(IDC_REMOVEVERTEX, OnRemovevertex)
	ON_BN_CLICKED(IDC_PARTY, OnParty)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_COPY, OnCopy)
	ON_BN_CLICKED(IDC_PASTE, OnPaste)
	ON_BN_CLICKED(IDC_NONPC, OnNonpc)
	ON_BN_CLICKED(IDC_TUNDET, OnTundet)
	ON_BN_CLICKED(IDC_TRESET, OnTreset)
	ON_BN_CLICKED(IDC_TDETECT, OnTdetect)
	ON_BN_CLICKED(IDC_TUNK1, OnTunk1)
	ON_BN_CLICKED(IDC_TUNK2, OnTunk2)
	ON_BN_CLICKED(IDC_TNPC, OnTnpc)
	ON_BN_CLICKED(IDC_TUNK3, OnTunk3)
	ON_BN_CLICKED(IDC_TDEACTIVATED, OnTdeactivated)
	ON_BN_CLICKED(IDC_TDOOR, OnTdoor)
	ON_BN_CLICKED(IDC_TOVERRIDE, OnToverride)
	ON_BN_CLICKED(IDC_TUNK4, OnTunk4)
	ON_BN_CLICKED(IDC_TUNK5, OnTunk5)
	ON_BN_CLICKED(IDC_TUNK6, OnTunk6)
	ON_BN_CLICKED(IDC_TUNK7, OnTunk7)
	ON_EN_KILLFOCUS(IDC_INFOSTR, OnKillfocusInfostr)
	ON_BN_CLICKED(IDC_CURSOR, OnCursor)
	ON_BN_CLICKED(IDC_UNKNOWN, OnUnknown)
	ON_CBN_SELCHANGE(IDC_VERTICES, OnSelchangeVertices)
	ON_BN_CLICKED(IDC_MODVERTEX, OnModvertex)
	ON_CBN_DROPDOWN(IDC_ENTRANCENAME, OnDropdownEntrancename)
	ON_EN_CHANGE(IDC_CURSORIDX, OnChangeCursoridx)
	ON_CBN_KILLFOCUS(IDC_VERTICES, OnKillfocusVertices)
	ON_BN_CLICKED(IDC_SET, OnSet)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_BROWSE2, OnBrowse2)
	ON_BN_CLICKED(IDC_BROWSE3, OnBrowse3)
	ON_EN_KILLFOCUS(IDC_UNKNOWN30, DefaultKillfocus)
	ON_CBN_KILLFOCUS(IDC_REGIONTYPE, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_POS1X, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_POS1Y, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_POS2X, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_POS2Y, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_CURSORIDX, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_STRREF, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_DESTAREA, DefaultKillfocus)
	ON_CBN_KILLFOCUS(IDC_ENTRANCENAME, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_TPOSX, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_TPOSY, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_KEY, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_DETECT, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_REMOVAL, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_SCRIPT, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_TRAPPED, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_DETECTED, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_DIALOG, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_FLAGS, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_POS1X2, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_POS1Y2, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_POS2X2, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_POS2Y2, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_VX, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_VY, DefaultKillfocus)
	ON_BN_CLICKED(IDC_BROWSE4, OnBrowse4)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAreaTrigger message handlers

void CAreaTrigger::OnKillfocusTriggerpicker() 
{
  CString tmpstr;
  int x;

  m_triggerpicker.GetWindowText(tmpstr);
  x=strtonum(tmpstr)-1;
  if(x>=0 && x<=the_area.triggercount)
  {
    m_triggernum=m_triggerpicker.SetCurSel(x);
  }
  else
  {
    if(m_triggernum>=0 && m_triggernum<=the_area.triggercount)
    {
      StoreName(tmpstr, the_area.triggerheaders[m_triggernum].infoname);
    }
  }
  RefreshTrigger();
	UpdateData(UD_DISPLAY);
}

void CAreaTrigger::OnSelchangeTriggerpicker() 
{
  int x;

  x=m_triggerpicker.GetCurSel();
	// TODO: Add your control notification handler code here
  if(x>=0 && x<=the_area.triggercount) m_triggernum=x;
  RefreshTrigger();
	UpdateData(UD_DISPLAY);
}

void CAreaTrigger::DefaultKillfocus() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CAreaTrigger::OnChangeCursoridx() 
{
  if(m_cursoricon.m_hWnd)
  {
  	UpdateData(UD_RETRIEVE);
	  UpdateData(UD_DISPLAY);
  }
}

void CAreaTrigger::OnBrowse() 
{
  if(m_triggernum<0) return;
  pickerdlg.m_restype=REF_ITM;
  RetrieveResref(pickerdlg.m_picked,the_area.triggerheaders[m_triggernum].key);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_area.triggerheaders[m_triggernum].key);
  }
  RefreshTrigger();
	UpdateData(UD_DISPLAY);	
}

void CAreaTrigger::OnBrowse2() 
{
  if(m_triggernum<0) return;
  pickerdlg.m_restype=REF_BCS;
  RetrieveResref(pickerdlg.m_picked,the_area.triggerheaders[m_triggernum].scriptref);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_area.triggerheaders[m_triggernum].scriptref);
  }
  RefreshTrigger();
	UpdateData(UD_DISPLAY);	
}

void CAreaTrigger::OnBrowse3() 
{
  if(m_triggernum<0) return;
  pickerdlg.m_restype=REF_DLG;
  RetrieveResref(pickerdlg.m_picked,the_area.triggerheaders[m_triggernum].dialogref);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_area.triggerheaders[m_triggernum].dialogref);
  }
  RefreshTrigger();
	UpdateData(UD_DISPLAY);	
}

void CAreaTrigger::OnBrowse4() 
{
  if(m_triggernum<0) return;
  pickerdlg.m_restype=REF_ARE;
  RetrieveResref(pickerdlg.m_picked,the_area.triggerheaders[m_triggernum].destname);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_area.triggerheaders[m_triggernum].destname);
  }
  RefreshTrigger();
	UpdateData(UD_DISPLAY);	
}

void CAreaTrigger::OnKillfocusVertices() 
{
  area_vertex *poi;
  int vertexnum;

	UpdateData(UD_RETRIEVE);
  if(m_triggernum<0) return;
  poi=(area_vertex *) the_area.vertexheaderlist.GetAt(the_area.vertexheaderlist.FindIndex(m_triggernum));
  vertexnum=m_vertexpicker.GetCurSel();
  if(vertexnum<0)
  {
    m_vx=-1;
    m_vy=-1;
  }
  else
  {
    m_vx=poi[vertexnum].point.x;
    m_vy=poi[vertexnum].point.y;
  }
	UpdateData(UD_DISPLAY);
}

void CAreaTrigger::OnSelchangeVertices() 
{
  area_vertex *poi;
  int vertexnum;

  if(m_triggernum<0) return;
  poi=(area_vertex *) the_area.vertexheaderlist.GetAt(the_area.vertexheaderlist.FindIndex(m_triggernum));
  vertexnum=m_vertexpicker.GetCurSel();
  if(vertexnum>=the_area.triggerheaders[m_triggernum].vertexcount)
  {
    MessageBox("Screwed it up!","Error",MB_OK);
  }
  if(vertexnum<0)
  {
    m_vx=-1;
    m_vy=-1;
  }
  else
  {
    m_vx=poi[vertexnum].point.x;
    m_vy=poi[vertexnum].point.y;
  }
	UpdateData(UD_DISPLAY);
}

void CAreaTrigger::OnModvertex() 
{
  area_vertex *poi;
  int vertexnum;

	UpdateData(UD_RETRIEVE);
  poi=(area_vertex *) the_area.vertexheaderlist.GetAt(the_area.vertexheaderlist.FindIndex(m_triggernum));
  if(!poi) return;
  vertexnum=m_vertexpicker.GetCurSel();
  if(vertexnum<0) return;
  poi[vertexnum].point.x=(short) m_vx;
  poi[vertexnum].point.y=(short) m_vy;
  RefreshTrigger();
	OnRecalcbox();
}

void CAreaTrigger::OnRecalcbox() 
{
  POINTS max, min;

  RecalcBox(the_area.triggerheaders[m_triggernum].vertexcount,m_triggernum,min,max);
  the_area.triggerheaders[m_triggernum].p1x=min.x;
  the_area.triggerheaders[m_triggernum].p1y=min.y;
  the_area.triggerheaders[m_triggernum].p2x=max.x;
  the_area.triggerheaders[m_triggernum].p2y=max.y;
	UpdateData(UD_DISPLAY);
}

void CAreaTrigger::OnAddvertex() 
{
  POSITION pos;
  area_vertex *poi;
  area_vertex *poi2;
  int vertexsize;
  int vertexnum;

	UpdateData(UD_RETRIEVE);
  vertexnum=m_vertexpicker.GetCurSel();
  if(m_triggernum<0)
  {
    return;
  }
  pos=the_area.vertexheaderlist.FindIndex(m_triggernum);
  poi=(area_vertex *) the_area.vertexheaderlist.GetAt(pos);
  vertexsize=the_area.triggerheaders[m_triggernum].vertexcount;
  poi2=new area_vertex[vertexsize+1];
  if(!poi2)
  {
    return;
  }
  vertexnum++;
  memcpy(poi2,poi,vertexnum*sizeof(area_vertex) );
  memcpy(poi2+vertexnum+1,poi+vertexnum,(vertexsize-vertexnum)*sizeof(area_vertex) );
  poi2[vertexnum].point.x=(short) m_vx;
  poi2[vertexnum].point.y=(short) m_vy;
  the_area.vertexheaderlist.SetAt(pos,poi2); //this will free memory at *poi as well
  the_area.triggerheaders[m_triggernum].vertexcount=(short) (vertexsize+1);
  RefreshTrigger();
  OnRecalcbox(); //this calls updatedata 	
}

void CAreaTrigger::OnRemovevertex() 
{
  POSITION pos;
  area_vertex *poi;
  area_vertex *poi2;
  int vertexsize;
  int vertexnum;

	UpdateData(UD_RETRIEVE);
  vertexnum=m_vertexpicker.GetCurSel();
  if(m_triggernum<0 || vertexnum<0)
  {
    return;
  }
  pos=the_area.vertexheaderlist.FindIndex(m_triggernum);
  poi=(area_vertex *) the_area.vertexheaderlist.GetAt(pos);
  vertexsize=the_area.triggerheaders[m_triggernum].vertexcount;
  poi2=new area_vertex[--vertexsize];
  if(!poi2)
  {
    return;
  }
  memcpy(poi2,poi,vertexnum*sizeof(area_vertex) );
  memcpy(poi2+vertexnum,poi+vertexnum+1,(vertexsize-vertexnum)*sizeof(area_vertex) );
  the_area.vertexheaderlist.SetAt(pos,poi2); //this will free memory at *poi as well
  the_area.triggerheaders[m_triggernum].vertexcount=(short) vertexsize;
  RefreshTrigger();
  OnRecalcbox(); //calls updatedata
}

void CAreaTrigger::OnKillfocusDestarea() 
{
  CString tmpstr;

	UpdateData(UD_RETRIEVE);
  RetrieveResref(tmpstr, the_area.triggerheaders[m_triggernum].destref);
  fill_destination(tmpstr, &m_entrancenamepicker);
  //refresh destname picker
	UpdateData(UD_DISPLAY);
}

void CAreaTrigger::OnDropdownEntrancename() 
{
  CString tmpstr;
  int pos;

  tmpstr.Format("FROM%s",itemname);
	pos=m_entrancenamepicker.FindString(0,tmpstr);
  if(pos>=0) m_entrancenamepicker.SetCurSel(pos);
}

void CAreaTrigger::OnAdd() 
{
  area_trigger *newtriggers;
  area_vertex *newvertex;
  CString tmpstr;
  
  if(the_area.header.infocnt>999)
  {
    MessageBox("Cannot add more triggers.","Warning",MB_ICONEXCLAMATION|MB_OK);
    return;
  }
  newtriggers=new area_trigger[++the_area.header.infocnt];
  if(!newtriggers)
  {
    the_area.header.infocnt--;
    MessageBox("Not enough memory.","Error",MB_ICONSTOP|MB_OK);
    return;
  }
  newvertex=new area_vertex[0];
  if(!newvertex)
  {
    the_area.header.infocnt--;
    MessageBox("Not enough memory.","Error",MB_ICONSTOP|MB_OK);
    return;
  }
  the_area.vertexheaderlist.InsertAfter(the_area.vertexheaderlist.FindIndex(the_area.triggercount), newvertex);
  memcpy(newtriggers, the_area.triggerheaders, the_area.triggercount*sizeof(area_trigger));
  memset(newtriggers+the_area.triggercount,0,sizeof(area_trigger) );
  tmpstr.Format("Info Point %d",the_area.header.infocnt);
  StoreName(tmpstr,newtriggers[the_area.triggercount].infoname);
  newtriggers[the_area.triggercount].strref=-1;
  
  if(the_area.triggerheaders)
  {
    delete [] the_area.triggerheaders;
  }
  the_area.triggerheaders=newtriggers;
  m_triggernum=the_area.triggercount; //last element 
  the_area.triggercount=the_area.header.infocnt;

  RefreshTrigger();
	OnRecalcbox();
}

void CAreaTrigger::OnRemove() 
{
  area_trigger *newtriggers;

  if(m_triggernum<0 || !the_area.header.infocnt) return;
  
  newtriggers=new area_trigger[--the_area.header.infocnt];
  if(!newtriggers)
  {
    the_area.header.infocnt++;
    return;
  }
  memcpy(newtriggers,the_area.triggerheaders,m_triggernum*sizeof(area_trigger) );
  memcpy(newtriggers+m_triggernum,the_area.triggerheaders+m_triggernum+1,(the_area.header.infocnt-m_triggernum)*sizeof(area_trigger) );
  if(the_area.triggerheaders)
  {
    delete [] the_area.triggerheaders;
  }
  the_area.triggerheaders=newtriggers;
  the_area.triggercount=the_area.header.infocnt;
  the_area.vertexheaderlist.RemoveAt(the_area.vertexheaderlist.FindIndex(m_triggernum) );
	if(m_triggernum>=the_area.triggercount) m_triggernum--;
  RefreshTrigger();
	UpdateData(UD_DISPLAY);
}

void CAreaTrigger::OnCopy() 
{
  POSITION pos;

  if(m_triggernum>=0)
  {
    pos=the_area.vertexheaderlist.FindIndex(m_triggernum);
    vertexsize=the_area.triggerheaders[m_triggernum].vertexcount;
    if(vertexcopy) delete [] vertexcopy;
    vertexcopy=new area_vertex[vertexsize]; //don't check, we just freed some memory
    memcpy(&triggercopy,the_area.triggerheaders+m_triggernum,sizeof(triggercopy) );
    memcpy(vertexcopy,the_area.vertexheaderlist.GetAt(pos),vertexsize*sizeof(area_vertex));
  }
}

void CAreaTrigger::OnPaste() 
{
  POSITION pos;
  area_vertex *poi;

  if(m_triggernum>=0)
  {
    pos=the_area.vertexheaderlist.FindIndex(m_triggernum);
    poi=new area_vertex[vertexsize];
    if(poi)
    {
      memcpy(poi,vertexcopy,vertexsize*sizeof(area_vertex));
    }
    the_area.vertexheaderlist.SetAt(pos,poi); //this will also free the old element
    memcpy(the_area.triggerheaders+m_triggernum,&triggercopy, sizeof(triggercopy) );
  }
  RefreshTrigger();
	UpdateData(UD_DISPLAY);
}

void CAreaTrigger::OnTundet() 
{
	the_area.triggerheaders[m_triggernum].infoflags^=1;
	UpdateData(UD_DISPLAY);
}

void CAreaTrigger::OnTreset() 
{
	the_area.triggerheaders[m_triggernum].infoflags^=2;
	UpdateData(UD_DISPLAY);
}

void CAreaTrigger::OnParty() 
{
	the_area.triggerheaders[m_triggernum].infoflags^=4;
	UpdateData(UD_DISPLAY);
}

void CAreaTrigger::OnTdetect() 
{
	the_area.triggerheaders[m_triggernum].infoflags^=8;
	UpdateData(UD_DISPLAY);
}

void CAreaTrigger::OnTunk1() 
{
	the_area.triggerheaders[m_triggernum].infoflags^=16;
	UpdateData(UD_DISPLAY);
}

void CAreaTrigger::OnTunk2() 
{
	the_area.triggerheaders[m_triggernum].infoflags^=32;
	UpdateData(UD_DISPLAY);
}

void CAreaTrigger::OnTnpc() 
{
	the_area.triggerheaders[m_triggernum].infoflags^=64;
	UpdateData(UD_DISPLAY);
}

void CAreaTrigger::OnTunk3() 
{
	the_area.triggerheaders[m_triggernum].infoflags^=128;
	UpdateData(UD_DISPLAY);
}

void CAreaTrigger::OnTdeactivated() 
{
	the_area.triggerheaders[m_triggernum].infoflags^=256;
	UpdateData(UD_DISPLAY);
}
void CAreaTrigger::OnNonpc() 
{
	the_area.triggerheaders[m_triggernum].infoflags^=512;
	UpdateData(UD_DISPLAY);
}

void CAreaTrigger::OnToverride() 
{
	the_area.triggerheaders[m_triggernum].infoflags^=1024;
	UpdateData(UD_DISPLAY);
}

void CAreaTrigger::OnTdoor() 
{
	the_area.triggerheaders[m_triggernum].infoflags^=2048;
	UpdateData(UD_DISPLAY);
}

void CAreaTrigger::OnTunk4() 
{
	the_area.triggerheaders[m_triggernum].infoflags^=0x1000;
	UpdateData(UD_DISPLAY);
}

void CAreaTrigger::OnTunk5() 
{
	the_area.triggerheaders[m_triggernum].infoflags^=0x2000;
	UpdateData(UD_DISPLAY);
}

void CAreaTrigger::OnTunk6() 
{
	the_area.triggerheaders[m_triggernum].infoflags^=0x4000;
	UpdateData(UD_DISPLAY);
}

void CAreaTrigger::OnTunk7() 
{
	the_area.triggerheaders[m_triggernum].infoflags^=0x8000;
	UpdateData(UD_DISPLAY);
}

void CAreaTrigger::OnKillfocusInfostr() 
{
  CString old;

	UpdateData(UD_RETRIEVE);
  old=resolve_tlk_text(the_area.triggerheaders[m_triggernum].strref);
  if(old!=m_infostr)
  {
    if(editflg&TLKCHANGE)
    {
      if(MessageBox("Do you want to update dialog.tlk?","Area editor",MB_YESNO)!=IDYES)
      {
        return;
      }
    }
    the_area.triggerheaders[m_triggernum].strref=store_tlk_text(the_area.triggerheaders[m_triggernum].strref, m_infostr);
  }
  RefreshTrigger();
	UpdateData(UD_DISPLAY);
}

void CAreaTrigger::OnCursor() 
{
  /*
  CBamPicker picker("CURSORS"); //this is a separate object

  if(picker.DoModal()==IDOK)
  {
    the_area.triggerheaders[m_triggernum].cursortype=pickerdlg.m_picked;
  }
  */
	UpdateData(UD_DISPLAY);
}

void CAreaTrigger::OnSet() 
{
  CImageView dlg;
  CPoint point;

  if(SetupSelectPoint()) return;
  dlg.InitView(IW_ENABLEBUTTON|IW_SETVERTEX|IW_PLACEIMAGE);
  dlg.SetupAnimationPlacement(&the_bam,  //the cursors are loaded in 'the_bam' now
    the_area.triggerheaders[m_triggernum].launchx,
    the_area.triggerheaders[m_triggernum].launchy);
  if(dlg.DoModal()==IDOK)
  {
    point=dlg.GetPoint(0);
    the_area.triggerheaders[m_triggernum].launchx=(short) point.x;
    the_area.triggerheaders[m_triggernum].launchy=(short) point.y;
  }
  RefreshTrigger();
  UpdateData(UD_DISPLAY);
}

void CAreaTrigger::OnUnknown() 
{
  //edit unknowns
  /*
  CUnknownEdit unknowns(20); //length of unknown field
  if(unknowns.DoModal()==IDOK)
  {
    memcpy(the_area.triggerheaders[m_triggernum].unknowns,unknowns.m_data,20);
  }
  */
  UpdateData(UD_DISPLAY);
}

/////////////////////////////////////////////////////////////////////////////
// CAreaSpawn dialog

CAreaSpawn::CAreaSpawn() : CPropertyPage(CAreaSpawn::IDD)
{
	//{{AFX_DATA_INIT(CAreaSpawn)
	m_timeofday = _T("");
	//}}AFX_DATA_INIT
  m_spawnnum=-1;
  m_crenum=-1;
  memset(&spawncopy,0,sizeof(spawncopy));
}

CAreaSpawn::~CAreaSpawn()
{
}

static int spawnboxids[]={IDC_SPAWNPICKER, IDC_POSX, IDC_POSY, IDC_SCHEDULE, IDC_TIMEOFDAY,
IDC_FREQUENCY,IDC_DELAY,IDC_METHOD,IDC_UNKNOWN7C, IDC_UNKNOWN80, IDC_UNKNOWN82,
IDC_MIN,IDC_MAX, IDC_LOW, IDC_HIGH, IDC_CRERES, IDC_SPAWNNUMPICKER,IDC_ADDCRE, IDC_DELCRE,
IDC_MAXCRE, IDC_BROWSE, IDC_UNKNOWN, IDC_COPY, IDC_PASTE, IDC_REMOVE, IDC_SET,
0};

#pragma warning(disable:4706)   
void CAreaSpawn::DoDataExchange(CDataExchange* pDX)
{
  CWnd *cb;
  CString tmpstr;
  int flg, flg2;
  int i;
  int id;

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAreaSpawn)
	DDX_Control(pDX, IDC_SPAWNNUMPICKER, m_spawnnumpicker);
	DDX_Control(pDX, IDC_SCHEDULE, m_schedule_control);
	DDX_Control(pDX, IDC_SPAWNPICKER, m_spawnpicker);
	DDX_Text(pDX, IDC_TIMEOFDAY, m_timeofday);
	//}}AFX_DATA_MAP
  cb=GetDlgItem(IDC_MAXSPAWN);
  flg=m_spawnnum>=0;
  if(flg) flg2=!!the_area.spawnheaders[m_spawnnum].creaturecnt;      
  else flg2=false;
  tmpstr.Format("/ %d",the_area.spawncount);
  cb->SetWindowText(tmpstr);
  i=0;
  while(id=spawnboxids[i])
  {
    cb=GetDlgItem(id);
    if(flg && (id==IDC_CRERES || id==IDC_BROWSE || id==IDC_DELCRE))
    {
      cb->EnableWindow(flg2);
    }
    else
    {
      cb->EnableWindow(flg);
    }
    i++;
  }
  if(flg)
  {
    tmpstr.Format("%d. %-.32s",m_spawnnum+1,the_area.spawnheaders[m_spawnnum].spawnname);
    DDX_Text(pDX, IDC_SPAWNPICKER, tmpstr);

    DDX_Text(pDX, IDC_POSX, the_area.spawnheaders[m_spawnnum].px);
    DDX_Text(pDX, IDC_POSY, the_area.spawnheaders[m_spawnnum].py);
    tmpstr.Format("/ %d",the_area.spawnheaders[m_spawnnum].creaturecnt);
    DDX_Text(pDX, IDC_MAXCRE, tmpstr);
    DDX_Text(pDX,IDC_FREQUENCY,the_area.spawnheaders[m_spawnnum].frequency);
    DDX_Text(pDX,IDC_DELAY,the_area.spawnheaders[m_spawnnum].delay);
    if(!the_area.spawnheaders[m_spawnnum].delay) the_area.spawnheaders[m_spawnnum].delay=1;
    tmpstr=get_spawntype(the_area.spawnheaders[m_spawnnum].method);
    DDX_Text(pDX,IDC_METHOD,tmpstr);
    the_area.spawnheaders[m_spawnnum].method=(short) strtonum(tmpstr);
    DDX_Text(pDX,IDC_UNKNOWN7C,the_area.spawnheaders[m_spawnnum].unknown7c);
    DDX_Text(pDX,IDC_UNKNOWN80,the_area.spawnheaders[m_spawnnum].unknown80);
    DDX_Text(pDX,IDC_UNKNOWN82,the_area.spawnheaders[m_spawnnum].unknown82);
    DDX_Text(pDX,IDC_MIN,the_area.spawnheaders[m_spawnnum].min);
    DDX_Text(pDX,IDC_MAX,the_area.spawnheaders[m_spawnnum].max);
    DDX_Text(pDX,IDC_LOW,the_area.spawnheaders[m_spawnnum].percent1);
    DDX_Text(pDX,IDC_HIGH,the_area.spawnheaders[m_spawnnum].percent2);
  }
  if(flg2)
  {
    RetrieveResref(tmpstr, the_area.spawnheaders[m_spawnnum].creatures[m_crenum]);
    DDX_Text(pDX, IDC_CRERES, tmpstr);
    DDV_MaxChars(pDX, tmpstr, 8);
    StoreResref(tmpstr, the_area.spawnheaders[m_spawnnum].creatures[m_crenum]);
  }
}

//move itemdata to local variables
void CAreaSpawn::RefreshSpawn()
{
  CString tmpstr;
	CSchedule tmp;
  int i;

  if(IsWindow(m_spawnpicker) )
  {
    m_spawnnum=m_spawnpicker.GetCurSel();
    if(m_spawnnum<0) m_spawnnum=0;
    m_spawnpicker.ResetContent();
    for(i=0;i<the_area.spawncount;i++)
    {
      tmpstr.Format("%d. %-.32s",i+1,the_area.spawnheaders[i].spawnname);
      m_spawnpicker.AddString(tmpstr);
    }
    if(m_spawnnum>=i) m_spawnnum=i-1;
    m_spawnnum=m_spawnpicker.SetCurSel(m_spawnnum);
  }
  else m_spawnnum=-1;

  if(m_spawnnum>=0)
  {   
    m_crenum=m_spawnnumpicker.GetCurSel();
    if(m_crenum<0) m_crenum=0;
    m_spawnnumpicker.ResetContent();
    for(i=0;i<the_area.spawnheaders[m_spawnnum].creaturecnt;i++)
    {
      tmpstr.Format("%d. %-.8s",i+1,the_area.spawnheaders[m_spawnnum].creatures[i]);
      m_spawnnumpicker.AddString(tmpstr);
    }
    if(m_crenum>=i) m_crenum=i-1;
    m_crenum=m_spawnnumpicker.SetCurSel(m_crenum);
    tmp.m_schedule=the_area.spawnheaders[m_spawnnum].schedule;
    if(IsWindow(m_schedule_control) )
    {
      m_schedule_control.SetWindowText(tmp.GetCaption());
    }
    m_timeofday=tmp.GetTimeOfDay();
  }
  else
  {
    if(IsWindow(m_schedule_control) )
    {
      
      m_schedule_control.SetWindowText("-- not available --");    
    }
    m_timeofday="";
  }
}
#pragma warning(default:4706)   
BOOL CAreaSpawn::OnInitDialog() 
{
  int i;
  CComboBox *cb;

	CPropertyPage::OnInitDialog();
	RefreshSpawn();
  cb=(CComboBox *) GetDlgItem(IDC_METHOD);
  cb->ResetContent();
  for(i=0;i<NUM_SPTYPE;i++)
  {
    cb->AddString(get_spawntype(i));
  }
  UpdateData(UD_DISPLAY);
	return TRUE;
}

BEGIN_MESSAGE_MAP(CAreaSpawn, CPropertyPage)
	//{{AFX_MSG_MAP(CAreaSpawn)
	ON_EN_KILLFOCUS(IDC_POSX, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_POSY, DefaultKillfocus)
	ON_CBN_KILLFOCUS(IDC_SPAWNPICKER, OnKillfocusSpawnpicker)
	ON_CBN_SELCHANGE(IDC_SPAWNPICKER, OnSelchangeSpawnpicker)
	ON_CBN_KILLFOCUS(IDC_SPAWNNUMPICKER, OnKillfocusSpawnnumpicker)
	ON_CBN_SELCHANGE(IDC_SPAWNNUMPICKER, OnSelchangeSpawnnumpicker)
	ON_EN_KILLFOCUS(IDC_DELAY, DefaultKillfocus)
	ON_BN_CLICKED(IDC_SCHEDULE, OnSchedule)
	ON_EN_KILLFOCUS(IDC_CRERES, OnKillfocusCreres)
	ON_BN_CLICKED(IDC_COPY, OnCopy)
	ON_BN_CLICKED(IDC_PASTE, OnPaste)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_ADDCRE, OnAddcre)
	ON_BN_CLICKED(IDC_DELCRE, OnDelcre)
	ON_EN_KILLFOCUS(IDC_FREQUENCY, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_UNKNOWN80, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_UNKNOWN7C, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_HIGH, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_LOW, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_MAX, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_MIN, DefaultKillfocus)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_EN_KILLFOCUS(IDC_UNKNOWN82, DefaultKillfocus)
	ON_BN_CLICKED(IDC_UNKNOWN, OnUnknown)
	ON_CBN_KILLFOCUS(IDC_METHOD, DefaultKillfocus)
	ON_BN_CLICKED(IDC_SET, OnSet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAreaSpawn message handlers

void CAreaSpawn::OnKillfocusSpawnpicker() 
{
  CString tmpstr;
  int x;

  m_spawnpicker.GetWindowText(tmpstr);
  x=strtonum(tmpstr)-1;
  if(x>=0 && x<=the_area.spawncount)
  {
    m_spawnnum=m_spawnpicker.SetCurSel(x);
  }
  else
  {
    if(m_spawnnum>=0 && m_spawnnum<=the_area.spawncount)
    {
      StoreName(tmpstr, the_area.spawnheaders[m_spawnnum].spawnname);
    }
  }
  RefreshSpawn();
	UpdateData(UD_DISPLAY);		
}

void CAreaSpawn::OnSelchangeSpawnpicker() 
{
  int x;

  x=m_spawnpicker.GetCurSel();
  if(x>=0 && x<=the_area.spawncount) m_spawnnum=x;
  RefreshSpawn();
	UpdateData(UD_DISPLAY);		
}

void CAreaSpawn::DefaultKillfocus() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CAreaSpawn::OnKillfocusSpawnnumpicker() 
{
  CString tmpstr;
  int x;

  m_spawnnumpicker.GetWindowText(tmpstr);
  x=strtonum(tmpstr)-1;
  if(x>=0 && x<=the_area.spawnheaders[m_spawnnum].creaturecnt)
  {
    m_crenum=x;
    m_spawnnumpicker.SetCurSel(m_crenum);
  }
  RefreshSpawn();
	UpdateData(UD_DISPLAY);
}

void CAreaSpawn::OnSelchangeSpawnnumpicker() 
{
  int x;

  x=m_spawnnumpicker.GetCurSel();
  if(x>=0 && x<=the_area.spawnheaders[m_spawnnum].creaturecnt) m_crenum=x;
  RefreshSpawn();
	UpdateData(UD_DISPLAY);
}

void CAreaSpawn::OnSchedule() 
{
	CSchedule dlg;

  if(m_spawnnum<0) return;
  dlg.m_schedule=the_area.spawnheaders[m_spawnnum].schedule;
  dlg.m_default=defschedule;
  if(dlg.DoModal()==IDOK)
  {
    the_area.spawnheaders[m_spawnnum].schedule=dlg.m_schedule;
    m_schedule_control.SetWindowText(dlg.GetCaption());
    m_timeofday=dlg.GetTimeOfDay();
  }
  defschedule=dlg.m_default; //we copy this always
  UpdateData(UD_DISPLAY);
}

void CAreaSpawn::OnKillfocusCreres() 
{
	if(m_spawnnum<0 || m_crenum<0) return;
  UpdateData(UD_RETRIEVE);
  RefreshSpawn();
  UpdateData(UD_DISPLAY);
}

void CAreaSpawn::OnAdd() 
{
  area_spawn *newspawns;
  CString tmpstr;
  
  if(the_area.header.spawncnt>999)
  {
    MessageBox("Cannot add more spawns.","Warning",MB_ICONEXCLAMATION|MB_OK);
    return;
  }
  newspawns=new area_spawn[++the_area.header.spawncnt];
  if(!newspawns)
  {
    the_area.header.spawncnt--;
    MessageBox("Not enough memory.","Error",MB_ICONSTOP|MB_OK);
    return;
  }
  memcpy(newspawns, the_area.spawnheaders, the_area.spawncount*sizeof(area_spawn));
  memset(newspawns+the_area.spawncount,0,sizeof(area_spawn) );
  tmpstr.Format("Spawn %d",the_area.header.spawncnt);
  StoreName(tmpstr,newspawns[the_area.spawncount].spawnname);
  //all seen spawnheaders had these values, so we set them
  newspawns[the_area.spawncount].percent1=100;
  newspawns[the_area.spawncount].percent2=100;
  newspawns[the_area.spawncount].method=1;
  newspawns[the_area.spawncount].unknown7c=1000;
  newspawns[the_area.spawncount].unknown80=1000;
  newspawns[the_area.spawncount].unknown82=1000;

  if(the_area.spawnheaders)
  {
    delete [] the_area.spawnheaders;
  }
  the_area.spawnheaders=newspawns;
  m_spawnnum=the_area.spawncount; //last element
  the_area.spawncount=the_area.header.spawncnt;

  RefreshSpawn();
	UpdateData(UD_DISPLAY);
}

void CAreaSpawn::OnRemove() 
{
  area_spawn *newspawns;

  if(m_spawnnum<0 || !the_area.header.spawncnt) return;
  
  newspawns=new area_spawn[--the_area.header.spawncnt];
  if(!newspawns)
  {
    the_area.header.spawncnt++;
    return;
  }
  memcpy(newspawns,the_area.spawnheaders,m_spawnnum*sizeof(area_spawn) );
  memcpy(newspawns+m_spawnnum,the_area.spawnheaders+m_spawnnum+1,(the_area.header.spawncnt-m_spawnnum)*sizeof(area_spawn) );
  if(the_area.spawnheaders)
  {
    delete [] the_area.spawnheaders;
  }
  the_area.spawnheaders=newspawns;
  the_area.spawncount=the_area.header.spawncnt;
	if(m_spawnnum>=the_area.spawncount) m_spawnnum--;
  RefreshSpawn();
	UpdateData(UD_DISPLAY);
}

void CAreaSpawn::OnCopy() 
{
  if(m_spawnnum>=0)
  {
    memcpy(&spawncopy,the_area.spawnheaders+m_spawnnum,sizeof(spawncopy) );
  }
}

void CAreaSpawn::OnPaste() 
{
  if(m_spawnnum>=0)
  {
    memcpy(the_area.spawnheaders+m_spawnnum,&spawncopy, sizeof(spawncopy) );
  }
  RefreshSpawn();
	UpdateData(UD_DISPLAY);
}

void CAreaSpawn::OnSet() 
{
  CImageView dlg;
  CPoint point;

  if(SetupSelectPoint()) return;
  dlg.InitView(IW_ENABLEBUTTON|IW_SETVERTEX|IW_PLACEIMAGE);
  dlg.SetupAnimationPlacement(&the_bam,  //the cursors are loaded in 'the_bam' now
    the_area.spawnheaders[m_spawnnum].px,
    the_area.spawnheaders[m_spawnnum].py);
  if(dlg.DoModal()==IDOK)
  {
    point=dlg.GetPoint(0);
    the_area.spawnheaders[m_spawnnum].px=(short) point.x;
    the_area.spawnheaders[m_spawnnum].py=(short) point.y;
  }
  RefreshSpawn();
  UpdateData(UD_DISPLAY);
}

void CAreaSpawn::OnBrowse() 
{
  if(m_spawnnum<0 || m_crenum<0) return;
  pickerdlg.m_restype=REF_CRE;
  RetrieveResref(pickerdlg.m_picked,the_area.spawnheaders[m_spawnnum].creatures[m_crenum]);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_area.spawnheaders[m_spawnnum].creatures[m_crenum]);
  }
  RefreshSpawn();
	UpdateData(UD_DISPLAY);	
}

void CAreaSpawn::OnUnknown() 
{
	// TODO: Add your control notification handler code here
	
}

void CAreaSpawn::OnAddcre() 
{
  if(the_area.spawnheaders[m_spawnnum].creaturecnt<10)
  {
    the_area.spawnheaders[m_spawnnum].creaturecnt++;
  }
  RefreshSpawn();
	UpdateData(UD_DISPLAY);
}

// 8 --> resource size
// 9 --> there are 10 creature slots (0-9)
void CAreaSpawn::OnDelcre() 
{
  if(m_crenum>=0 && m_crenum<=9)
  {
    memcpy(the_area.spawnheaders[m_spawnnum].creatures+m_crenum,
           the_area.spawnheaders[m_spawnnum].creatures+m_crenum+1,
           (9-m_crenum)*8);
    memset(the_area.spawnheaders[m_spawnnum].creatures[9],0,8);
  }
  the_area.spawnheaders[m_spawnnum].creaturecnt--;
  m_crenum=the_area.spawnheaders[m_spawnnum].creaturecnt-1;
  RefreshSpawn();
	UpdateData(UD_DISPLAY);
}

/////////////////////////////////////////////////////////////////////////////
// CAreaEntrance dialog

CAreaEntrance::CAreaEntrance() : CPropertyPage(CAreaEntrance::IDD)
{
	//{{AFX_DATA_INIT(CAreaEntrance)
	//}}AFX_DATA_INIT
  m_entrancenum=-1;
  memset(&entrancecopy,0,sizeof(entrancecopy));
}

CAreaEntrance::~CAreaEntrance()
{
}

static int entranceboxids[]={IDC_ENTRANCEPICKER,IDC_POSX,IDC_POSY,IDC_FACE,
IDC_UNKNOWN26, IDC_UNKNOWN28, IDC_UNKNOWN2A, IDC_UNKNOWN, IDC_SET,
IDC_COPY, IDC_PASTE, IDC_REMOVE,
0};

void CAreaEntrance::DoDataExchange(CDataExchange* pDX)
{
  CWnd *cb;
  CString tmpstr;
  int flg;
  int i;

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAreaEntrance)
	DDX_Control(pDX, IDC_ENTRANCEPICKER, m_entrancepicker);
	//}}AFX_DATA_MAP
  cb=GetDlgItem(IDC_MAXENTRANCE);
  flg=m_entrancenum>=0;
  tmpstr.Format("/ %d",the_area.entrancecount);
  cb->SetWindowText(tmpstr);
  i=0;
  while(entranceboxids[i])
  {
    cb=GetDlgItem(entranceboxids[i++]);
    cb->EnableWindow(flg);
  }
  if(flg)
  {
    tmpstr.Format("%d. %-.32s",m_entrancenum+1,the_area.entranceheaders[m_entrancenum].entrancename);
    DDX_Text(pDX, IDC_ENTRANCEPICKER, tmpstr);

    DDX_Text(pDX, IDC_POSX, the_area.entranceheaders[m_entrancenum].px);
    DDX_Text(pDX, IDC_POSY, the_area.entranceheaders[m_entrancenum].py);

    tmpstr=format_direction(the_area.entranceheaders[m_entrancenum].face);
    DDX_Text(pDX, IDC_FACE,tmpstr);
    the_area.entranceheaders[m_entrancenum].face=(short) strtonum(tmpstr);

    DDX_Text(pDX, IDC_UNKNOWN26, the_area.entranceheaders[m_entrancenum].unknown26);
    DDX_Text(pDX, IDC_UNKNOWN28, the_area.entranceheaders[m_entrancenum].unknown28);
    DDX_Text(pDX, IDC_UNKNOWN2A, the_area.entranceheaders[m_entrancenum].unknown2a);
  }
}

//move itemdata to local variables
void CAreaEntrance::RefreshEntrance()
{
  CString tmpstr;
	CSchedule tmp;
  int i;

  if(IsWindow(m_entrancepicker) )
  {
    m_entrancenum=m_entrancepicker.GetCurSel();
    if(m_entrancenum<0) m_entrancenum=0;
    m_entrancepicker.ResetContent();
    for(i=0;i<the_area.entrancecount;i++)
    {
      tmpstr.Format("%d. %-.32s",i+1,the_area.entranceheaders[i].entrancename);
      m_entrancepicker.AddString(tmpstr);
    }
    if(m_entrancenum>=i) m_entrancenum=i-1;
    m_entrancenum=m_entrancepicker.SetCurSel(m_entrancenum);
  }
}

BOOL CAreaEntrance::OnInitDialog() 
{
  CComboBox *cb;
  int i;

	CPropertyPage::OnInitDialog();
	RefreshEntrance();
  cb=(CComboBox *) GetDlgItem(IDC_FACE);
  for(i=0;i<NUM_FVALUES;i++)
  {
    cb->AddString(format_direction(i));
  }
  UpdateData(UD_DISPLAY);
	return TRUE;
}

BEGIN_MESSAGE_MAP(CAreaEntrance, CPropertyPage)
	//{{AFX_MSG_MAP(CAreaEntrance)
	ON_CBN_KILLFOCUS(IDC_ENTRANCEPICKER, OnKillfocusEntrancepicker)
	ON_CBN_SELCHANGE(IDC_ENTRANCEPICKER, OnSelchangeEntrancepicker)
	ON_EN_KILLFOCUS(IDC_POSX, OnKillfocusPosx)
	ON_EN_KILLFOCUS(IDC_POSY, OnKillfocusPosy)
	ON_CBN_KILLFOCUS(IDC_FACE, OnKillfocusFace)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_COPY, OnCopy)
	ON_BN_CLICKED(IDC_PASTE, OnPaste)
	ON_EN_KILLFOCUS(IDC_UNKNOWN26, OnKillfocusUnknown26)
	ON_EN_KILLFOCUS(IDC_UNKNOWN28, OnKillfocusUnknown28)
	ON_BN_CLICKED(IDC_UNKNOWN, OnUnknown)
	ON_EN_KILLFOCUS(IDC_UNKNOWN2A, OnKillfocusUnknown2a)
	ON_BN_CLICKED(IDC_SET, OnSet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CAreaEntrance::OnKillfocusEntrancepicker() 
{
  CString tmpstr;
  int x;

  m_entrancepicker.GetWindowText(tmpstr);
  x=strtonum(tmpstr)-1;
  if(x>=0 && x<=the_area.entrancecount)
  {
    m_entrancenum=m_entrancepicker.SetCurSel(x);
  }
  else
  {
    if(m_entrancenum>=0 && m_entrancenum<=the_area.entrancecount)
    {
      StoreName(tmpstr, the_area.entranceheaders[m_entrancenum].entrancename);
    }
  }
  RefreshEntrance();
	UpdateData(UD_DISPLAY);	
}

void CAreaEntrance::OnSelchangeEntrancepicker() 
{
  int x;

  x=m_entrancepicker.GetCurSel();
  if(x>=0 && x<=the_area.entrancecount) m_entrancenum=x;
  RefreshEntrance();
	UpdateData(UD_DISPLAY);	
}

void CAreaEntrance::OnKillfocusPosx() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CAreaEntrance::OnKillfocusPosy() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CAreaEntrance::OnKillfocusFace() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CAreaEntrance::OnKillfocusUnknown26() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CAreaEntrance::OnKillfocusUnknown28() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CAreaEntrance::OnKillfocusUnknown2a() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CAreaEntrance::OnAdd() 
{
  area_entrance *newentrances;
  CString tmpstr;
  
  if(the_area.header.entrancecnt>999)
  {
    MessageBox("Cannot add more entrances.","Warning",MB_ICONEXCLAMATION|MB_OK);
    return;
  }
  newentrances=new area_entrance[++the_area.header.entrancecnt];
  if(!newentrances)
  {
    the_area.header.entrancecnt--;
    MessageBox("Not enough memory.","Error",MB_ICONSTOP|MB_OK);
    return;
  }
  memcpy(newentrances, the_area.entranceheaders, the_area.entrancecount*sizeof(area_entrance));
  memset(newentrances+the_area.entrancecount,0,sizeof(area_entrance) );
  tmpstr.Format("Entrance %d",the_area.header.entrancecnt);
  StoreName(tmpstr,newentrances[the_area.entrancecount].entrancename);
  
  if(the_area.entranceheaders)
  {
    delete [] the_area.entranceheaders;
  }
  the_area.entranceheaders=newentrances;
  m_entrancenum=the_area.entrancecount; //last element
  the_area.entrancecount=the_area.header.entrancecnt;

  RefreshEntrance();
	UpdateData(UD_DISPLAY);
}

void CAreaEntrance::OnRemove() 
{
  area_entrance *newentrances;

  if(m_entrancenum<0 || !the_area.header.entrancecnt) return;
  
  newentrances=new area_entrance[--the_area.header.entrancecnt];
  if(!newentrances)
  {
    the_area.header.entrancecnt++;
    return;
  }
  memcpy(newentrances,the_area.entranceheaders,m_entrancenum*sizeof(area_entrance) );
  memcpy(newentrances+m_entrancenum,the_area.entranceheaders+m_entrancenum+1,(the_area.header.entrancecnt-m_entrancenum)*sizeof(area_entrance) );
  if(the_area.entranceheaders)
  {
    delete [] the_area.entranceheaders;
  }
  the_area.entranceheaders=newentrances;
  the_area.entrancecount=the_area.header.entrancecnt;
	if(m_entrancenum>=the_area.entrancecount) m_entrancenum--;
  RefreshEntrance();
	UpdateData(UD_DISPLAY);
}

void CAreaEntrance::OnCopy() 
{
  if(m_entrancenum>=0)
  {
    memcpy(&entrancecopy,the_area.entranceheaders+m_entrancenum,sizeof(entrancecopy) );
  }
}

void CAreaEntrance::OnPaste() 
{
  if(m_entrancenum>=0)
  {
    memcpy(the_area.entranceheaders+m_entrancenum,&entrancecopy, sizeof(entrancecopy) );
  }
  RefreshEntrance();
	UpdateData(UD_DISPLAY);
}

void CAreaEntrance::OnSet() 
{
  CImageView dlg;
  CPoint point;

  if(SetupSelectPoint()) return;
  dlg.InitView(IW_ENABLEBUTTON|IW_SETVERTEX|IW_PLACEIMAGE);
  dlg.SetupAnimationPlacement(&the_bam,  //the cursors are loaded in 'the_bam' now
    the_area.entranceheaders[m_entrancenum].px,
    the_area.entranceheaders[m_entrancenum].py);
  if(dlg.DoModal()==IDOK)
  {
    point=dlg.GetPoint(0);
    the_area.entranceheaders[m_entrancenum].px=(short) point.x;
    the_area.entranceheaders[m_entrancenum].py=(short) point.y;
  }
  RefreshEntrance();
  UpdateData(UD_DISPLAY);
}

void CAreaEntrance::OnUnknown() 
{
	// TODO: Add your control notification handler code here
	
}

/////////////////////////////////////////////////////////////////////////////
// CAreaAmbient dialog

CAreaAmbient::CAreaAmbient() : CPropertyPage(CAreaAmbient::IDD)
{
	//{{AFX_DATA_INIT(CAreaAmbient)
	m_timeofday = _T("");
	//}}AFX_DATA_INIT
  m_ambientnum=-1;
  m_wavnum=-1;
  memset(&ambientcopy,0,sizeof(ambientcopy));
}

CAreaAmbient::~CAreaAmbient()
{
}

static int ambientboxids[]={IDC_AMBIENTPICKER,IDC_WAVRES, IDC_POSX,IDC_POSY,
IDC_RADIUS,IDC_HEIGHT, IDC_VOLUME, IDC_AMBINUMPICKER,IDC_ADDWAV, IDC_DELWAV, IDC_PLAY,
IDC_BROWSE, IDC_NUM, IDC_GAP, IDC_SOUNDNUM, IDC_FLAG1,IDC_FLAG2,IDC_FLAG3,IDC_FLAG4,
IDC_U28, IDC_U2C, IDC_FLAGS,IDC_U90, IDC_SCHEDULE, IDC_UNKNOWN,IDC_MAXWAV, IDC_SET,
IDC_COPY, IDC_PASTE, IDC_REMOVE,
0};

#pragma warning(disable:4706)   
void CAreaAmbient::DoDataExchange(CDataExchange* pDX)
{
  CWnd *cb;
  CButton *cb2;
  CString tmpstr;
  int flg, flg2;
  int i,j;
  int id;

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAreaAmbient)
	DDX_Control(pDX, IDC_AMBINUMPICKER, m_ambinumpicker);
	DDX_Control(pDX, IDC_SCHEDULE, m_schedule_control);
	DDX_Control(pDX, IDC_AMBIENTPICKER, m_ambientpicker);
	DDX_Text(pDX, IDC_TIMEOFDAY, m_timeofday);
	//}}AFX_DATA_MAP
  cb=GetDlgItem(IDC_MAXAMBIENT);
  flg=m_ambientnum>=0;
  if(flg) flg2=!!the_area.ambientheaders[m_ambientnum].ambientnum;
  else flg2=false;
  tmpstr.Format("/ %d",the_area.ambientcount);
  cb->SetWindowText(tmpstr);

  i=0;  
  while(id=ambientboxids[i])
  {
    cb=GetDlgItem(ambientboxids[i]);
    if(flg && (id==IDC_WAVRES || id==IDC_PLAY || id==IDC_BROWSE || id==IDC_DELWAV))
    {
      cb->EnableWindow(flg2);
    }
    else
    {
      cb->EnableWindow(flg);
    }
    i++;
  }
  if(flg)
  {
    tmpstr.Format("%d. %-.32s",m_ambientnum+1,the_area.ambientheaders[m_ambientnum].ambiname);
    DDX_Text(pDX, IDC_AMBIENTPICKER, tmpstr);

    DDX_Text(pDX, IDC_POSX, the_area.ambientheaders[m_ambientnum].posx);
    DDX_Text(pDX, IDC_POSY, the_area.ambientheaders[m_ambientnum].posy);
    tmpstr.Format("/ %d",the_area.ambientheaders[m_ambientnum].ambientnum);
    DDX_Text(pDX, IDC_MAXWAV, tmpstr);
    DDX_Text(pDX, IDC_RADIUS, the_area.ambientheaders[m_ambientnum].radius);
    DDX_Text(pDX, IDC_HEIGHT, the_area.ambientheaders[m_ambientnum].height);
    DDX_Text(pDX, IDC_U28, the_area.ambientheaders[m_ambientnum].unknown28);
    DDX_Text(pDX, IDC_U2C, the_area.ambientheaders[m_ambientnum].unknown2c);
    DDX_Text(pDX, IDC_VOLUME, the_area.ambientheaders[m_ambientnum].volume);

    DDX_Text(pDX, IDC_NUM, the_area.ambientheaders[m_ambientnum].ambientnum2);
    DDX_Text(pDX, IDC_GAP, the_area.ambientheaders[m_ambientnum].silence);
    DDX_Text(pDX, IDC_SOUNDNUM, the_area.ambientheaders[m_ambientnum].soundnum);

    DDX_Text(pDX, IDC_FLAGS, the_area.ambientheaders[m_ambientnum].flags);
    DDX_Text(pDX, IDC_U90, the_area.ambientheaders[m_ambientnum].unknown90);
    j=1;
    for(i=0;i<4;i++)
    {
      cb2=(CButton *) GetDlgItem(IDC_FLAG1+i);
      if(the_area.ambientheaders[m_ambientnum].flags&j) cb2->SetCheck(true);
      else cb2->SetCheck(false);
      j<<=1;
    }
  }
  if(flg2)
  {
    RetrieveResref(tmpstr,the_area.ambientheaders[m_ambientnum].ambients[m_wavnum]);
    DDX_Text(pDX, IDC_WAVRES, tmpstr);
    DDV_MaxChars(pDX, tmpstr, 8);
    StoreResref(tmpstr,the_area.ambientheaders[m_ambientnum].ambients[m_wavnum]);
  }
}
#pragma warning(default:4706)   
void CAreaAmbient::RefreshAmbient()
{
  CString tmpstr;
	CSchedule tmp;
  int i;

  if(m_ambientnum<0 || m_ambientnum>=the_area.header.ambicnt)
  {
    if(the_area.header.ambicnt) m_ambientnum=0;
    else m_ambientnum=-1;
  }
  if(m_ambientnum==-1)
  {
    m_wavnum=-1;
  }
  else
  {
    if(m_wavnum<0 || m_wavnum>=the_area.ambientheaders[m_ambientnum].ambientnum)
    {
      if(the_area.ambientheaders[m_ambientnum].ambientnum) m_wavnum=0;
      else m_wavnum=-1;
    }
  }

  if(IsWindow(m_ambientpicker) )
  {
    m_ambientpicker.ResetContent();
    for(i=0;i<the_area.ambientcount;i++)
    {
      tmpstr.Format("%d. %-.32s",i+1,the_area.ambientheaders[i].ambiname);
      m_ambientpicker.AddString(tmpstr);
    }
    m_ambientnum=m_ambientpicker.SetCurSel(m_ambientnum);
  }
  if(m_ambientnum>=0 && IsWindow(m_ambinumpicker) )
  {   
    m_ambinumpicker.ResetContent();
    for(i=0;i<the_area.ambientheaders[m_ambientnum].ambientnum;i++)
    {
      tmpstr.Format("%d. %-.8s",i+1,the_area.ambientheaders[m_ambientnum].ambients[i]);
      m_ambinumpicker.AddString(tmpstr);
    }
    m_ambinumpicker.SetCurSel(m_wavnum);
  }
  if(m_ambientnum>=0)
  {
    tmp.m_schedule=the_area.ambientheaders[m_ambientnum].schedule;
    if(IsWindow(m_schedule_control) )
    {
      m_schedule_control.SetWindowText(tmp.GetCaption());
    }
    m_timeofday=tmp.GetTimeOfDay();
  }
  else
  {
    if(IsWindow(m_schedule_control) )
    {
      
      m_schedule_control.SetWindowText("-- not available --");    
    }
    m_timeofday="";
  }
}

BOOL CAreaAmbient::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
  RefreshAmbient();
  UpdateData(UD_DISPLAY);
	return TRUE;
}

BEGIN_MESSAGE_MAP(CAreaAmbient, CPropertyPage)
	//{{AFX_MSG_MAP(CAreaAmbient)
	ON_CBN_KILLFOCUS(IDC_AMBIENTPICKER, OnKillfocusAmbientpicker)
	ON_CBN_SELCHANGE(IDC_AMBIENTPICKER, OnSelchangeAmbientpicker)
	ON_EN_KILLFOCUS(IDC_POSX, OnKillfocusPosx)
	ON_EN_KILLFOCUS(IDC_POSY, OnKillfocusPosy)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_COPY, OnCopy)
	ON_BN_CLICKED(IDC_PASTE, OnPaste)
	ON_BN_CLICKED(IDC_UNKNOWN, OnUnknown)
	ON_EN_KILLFOCUS(IDC_RADIUS, OnKillfocusRadius)
	ON_EN_KILLFOCUS(IDC_HEIGHT, OnKillfocusHeight)
	ON_EN_KILLFOCUS(IDC_VOLUME, OnKillfocusVolume)
	ON_BN_CLICKED(IDC_SCHEDULE, OnSchedule)
	ON_CBN_KILLFOCUS(IDC_AMBINUMPICKER, OnKillfocusAmbinumpicker)
	ON_CBN_SELCHANGE(IDC_AMBINUMPICKER, OnSelchangeAmbinumpicker)
	ON_EN_KILLFOCUS(IDC_WAVRES, OnKillfocusWavres)
	ON_BN_CLICKED(IDC_ADDWAV, OnAddwav)
	ON_BN_CLICKED(IDC_DELWAV, OnDelwav)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_PLAY, OnPlay)
	ON_EN_KILLFOCUS(IDC_NUM, OnKillfocusNum)
	ON_EN_KILLFOCUS(IDC_GAP, OnKillfocusGap)
	ON_EN_KILLFOCUS(IDC_SOUNDNUM, OnKillfocusSoundnum)
	ON_BN_CLICKED(IDC_FLAG1, OnFlag1)
	ON_BN_CLICKED(IDC_FLAG2, OnFlag2)
	ON_BN_CLICKED(IDC_FLAG3, OnFlag3)
	ON_BN_CLICKED(IDC_FLAG4, OnFlag4)
	ON_EN_KILLFOCUS(IDC_U28, OnKillfocusU28)
	ON_EN_KILLFOCUS(IDC_U2C, OnKillfocusU2c)
	ON_EN_KILLFOCUS(IDC_U90, OnKillfocusU90)
	ON_EN_KILLFOCUS(IDC_FLAGS, OnKillfocusFlags)
	ON_BN_CLICKED(IDC_SET, OnSet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAreaAmbient message handlers

void CAreaAmbient::OnKillfocusAmbientpicker() 
{
  CString tmpstr;
  int x;

  m_ambientpicker.GetWindowText(tmpstr);
  x=strtonum(tmpstr)-1;
  if(x>=0 && x<=the_area.ambientcount)
  {
    m_ambientnum=m_ambientpicker.SetCurSel(x);
  }
  else
  {
    if(m_ambientnum>=0 && m_ambientnum<=the_area.ambientcount)
    {
      StoreName(tmpstr, the_area.ambientheaders[m_ambientnum].ambiname);
    }
  }
  RefreshAmbient();
	UpdateData(UD_DISPLAY);	
}

void CAreaAmbient::OnSelchangeAmbientpicker() 
{
  int x;

  x=m_ambientpicker.GetCurSel();
  if(x>=0 && x<=the_area.ambientcount) m_ambientnum=x;
  RefreshAmbient();
	UpdateData(UD_DISPLAY);	
}

void CAreaAmbient::OnKillfocusPosx() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CAreaAmbient::OnKillfocusPosy() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CAreaAmbient::OnKillfocusRadius() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CAreaAmbient::OnKillfocusHeight() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CAreaAmbient::OnKillfocusU28() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CAreaAmbient::OnKillfocusU2c() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CAreaAmbient::OnKillfocusVolume() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CAreaAmbient::OnAdd() 
{
  area_ambient *newambients;
  CString tmpstr;
  
  if(the_area.header.ambicnt>999)
  {
    MessageBox("Cannot add more ambients.","Warning",MB_ICONEXCLAMATION|MB_OK);
    return;
  }
  newambients=new area_ambient[++the_area.header.ambicnt];
  if(!newambients)
  {
    the_area.header.ambicnt--;
    MessageBox("Not enough memory.","Error",MB_ICONSTOP|MB_OK);
    return;
  }
  memcpy(newambients, the_area.ambientheaders, the_area.ambientcount*sizeof(area_ambient));
  memset(newambients+the_area.ambientcount,0,sizeof(area_ambient) );
  tmpstr.Format("Ambient %d",the_area.header.ambicnt);
  StoreName(tmpstr,newambients[the_area.ambientcount].ambiname);
  
  if(the_area.ambientheaders)
  {
    delete [] the_area.ambientheaders;
  }
  the_area.ambientheaders=newambients;
  m_ambientnum=the_area.ambientcount; //last element
  the_area.ambientcount=the_area.header.ambicnt;

  RefreshAmbient();
	UpdateData(UD_DISPLAY);
}

void CAreaAmbient::OnRemove() 
{
  area_ambient *newambients;

  if(m_ambientnum<0 || !the_area.header.ambicnt) return;
  
  newambients=new area_ambient[--the_area.header.ambicnt];
  if(!newambients)
  {
    the_area.header.ambicnt++;
    return;
  }
  memcpy(newambients,the_area.ambientheaders,m_ambientnum*sizeof(area_ambient) );
  memcpy(newambients+m_ambientnum,the_area.ambientheaders+m_ambientnum+1,(the_area.header.ambicnt-m_ambientnum)*sizeof(area_ambient) );
  if(the_area.ambientheaders)
  {
    delete [] the_area.ambientheaders;
  }
  the_area.ambientheaders=newambients;
  the_area.ambientcount=the_area.header.ambicnt;
	if(m_ambientnum>=the_area.ambientcount) m_ambientnum--;
  RefreshAmbient();
	UpdateData(UD_DISPLAY);
}

void CAreaAmbient::OnCopy() 
{
  if(m_ambientnum>=0)
  {
    memcpy(&ambientcopy,the_area.ambientheaders+m_ambientnum,sizeof(ambientcopy) );
  }
}

void CAreaAmbient::OnPaste() 
{
  if(m_ambientnum>=0)
  {
    memcpy(the_area.ambientheaders+m_ambientnum,&ambientcopy, sizeof(ambientcopy) );
  }
  RefreshAmbient();
	UpdateData(UD_DISPLAY);
}

void CAreaAmbient::OnSet() 
{
  CImageView dlg;
  CPoint point;

  if(SetupSelectPoint()) return;
  dlg.InitView(IW_ENABLEBUTTON|IW_SETVERTEX|IW_PLACEIMAGE);
  dlg.SetupAnimationPlacement(&the_bam,  //the cursors are loaded in 'the_bam' now
    the_area.ambientheaders[m_ambientnum].posx,
    the_area.ambientheaders[m_ambientnum].posy);
  if(dlg.DoModal()==IDOK)
  {
    point=dlg.GetPoint(0);
    the_area.ambientheaders[m_ambientnum].posx=(short) point.x;
    the_area.ambientheaders[m_ambientnum].posy=(short) point.y;
  }
  RefreshAmbient();
  UpdateData(UD_DISPLAY);
}

void CAreaAmbient::OnSchedule() 
{
	CSchedule dlg;

  if(m_ambientnum<0) return;
  dlg.m_schedule=the_area.ambientheaders[m_ambientnum].schedule;
  dlg.m_default=defschedule;
  if(dlg.DoModal()==IDOK)
  {
    the_area.ambientheaders[m_ambientnum].schedule=dlg.m_schedule;
    m_schedule_control.SetWindowText(dlg.GetCaption());
    m_timeofday=dlg.GetTimeOfDay();
  }
  defschedule=dlg.m_default; //we copy this always
  UpdateData(UD_DISPLAY);
}

void CAreaAmbient::OnUnknown() 
{
	// TODO: Add your control notification handler code here
	
}

void CAreaAmbient::OnKillfocusAmbinumpicker() 
{
  CString tmpstr;
  int x;

  m_ambinumpicker.GetWindowText(tmpstr);
  x=strtonum(tmpstr)-1;
  if(x>=0 && x<=the_area.ambientheaders[m_ambientnum].ambientnum)
  {
    m_wavnum=x;
    m_ambinumpicker.SetCurSel(m_wavnum);
  }
  RefreshAmbient();
	UpdateData(UD_DISPLAY);
}

void CAreaAmbient::OnSelchangeAmbinumpicker() 
{
  int x;

  x=m_ambinumpicker.GetCurSel();
  if(x>=0 && x<=the_area.ambientheaders[m_ambientnum].ambientnum) m_wavnum=x;
  RefreshAmbient();
	UpdateData(UD_DISPLAY);
}

void CAreaAmbient::OnKillfocusWavres() 
{
	if(m_ambientnum<0 || m_wavnum<0) return;
  UpdateData(UD_RETRIEVE);
  RefreshAmbient();
	UpdateData(UD_DISPLAY);
}

void CAreaAmbient::OnKillfocusNum() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CAreaAmbient::OnKillfocusGap() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CAreaAmbient::OnKillfocusSoundnum() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CAreaAmbient::OnKillfocusFlags() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CAreaAmbient::OnKillfocusU90() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CAreaAmbient::OnFlag1() 
{
	the_area.ambientheaders[m_ambientnum].flags^=1;
	UpdateData(UD_DISPLAY);
}

void CAreaAmbient::OnFlag2() 
{
	the_area.ambientheaders[m_ambientnum].flags^=2;
	UpdateData(UD_DISPLAY);
}

void CAreaAmbient::OnFlag3() 
{
	the_area.ambientheaders[m_ambientnum].flags^=4;
	UpdateData(UD_DISPLAY);
}

void CAreaAmbient::OnFlag4() 
{
	the_area.ambientheaders[m_ambientnum].flags^=8;
	UpdateData(UD_DISPLAY);
}

void CAreaAmbient::OnAddwav() 
{
  if(the_area.ambientheaders[m_ambientnum].ambientnum<10)
  {
    the_area.ambientheaders[m_ambientnum].ambientnum++;
  }
  RefreshAmbient();
	UpdateData(UD_DISPLAY);
}

void CAreaAmbient::OnDelwav() 
{
  if(m_wavnum>=0 && m_wavnum<=9)
  {
    memcpy(the_area.ambientheaders[m_ambientnum].ambients+m_wavnum,
           the_area.ambientheaders[m_ambientnum].ambients+m_wavnum+1,
           (9-m_wavnum)*8);
    memset(the_area.ambientheaders[m_ambientnum].ambients[9],0,8);
  }
  the_area.ambientheaders[m_ambientnum].ambientnum--;
  m_wavnum=the_area.ambientheaders[m_ambientnum].ambientnum-1;
  RefreshAmbient();
	UpdateData(UD_DISPLAY);
}

void CAreaAmbient::OnBrowse() 
{
  if(m_ambientnum<0 || m_wavnum<0) return;
  pickerdlg.m_restype=REF_WAV;
  RetrieveResref(pickerdlg.m_picked,the_area.ambientheaders[m_ambientnum].ambients[m_wavnum]);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_area.ambientheaders[m_ambientnum].ambients[m_wavnum]);
  }
  RefreshAmbient();
	UpdateData(UD_DISPLAY);	
}

void CAreaAnim::OnTimer(UINT nIDEvent) 
{
  int nFrameIndex;

  if(!play || playindex>playmax)
  {
    KillTimer(nIDEvent);
    return;
  }
  if(playindex==playmax)
  {
    playindex=0;
    play=false;
  }
  nFrameIndex=the_anim.GetFrameIndex(the_area.animheaders[m_animnum].cyclenum, playindex);
  the_anim.MakeBitmap(nFrameIndex,bgcolor,hbanim,BM_RESIZE,1,1);
  m_bamframe.SetBitmap(hbanim);
  playindex++;
	CPropertyPage::OnTimer(nIDEvent);
}

void CAreaAmbient::OnPlay() 
{
  CString tmpstr;

  UpdateData(UD_RETRIEVE);
	if(!the_area.ambientheaders[m_ambientnum].ambients[m_wavnum]) return;
  RetrieveResref(tmpstr, the_area.ambientheaders[m_ambientnum].ambients[m_wavnum]);
  play_acm(tmpstr,false, false);
}

/////////////////////////////////////////////////////////////////////////////
// CAreaContainer dialog

CAreaContainer::CAreaContainer() : CPropertyPage(CAreaContainer::IDD)
{
	//{{AFX_DATA_INIT(CAreaContainer)
	m_vx = 0;
	m_vy = 0;
	//}}AFX_DATA_INIT
  m_containernum=-1;
  vertexcopy=NULL;
  itemcopy=NULL;
}

CAreaContainer::~CAreaContainer()
{
  if(vertexcopy) delete [] vertexcopy;
  vertexcopy=NULL;
  if(itemcopy) delete [] itemcopy;
  itemcopy=NULL;
}

static int itemflagids[]={IDC_IDENTIFIED,IDC_NOSTEAL,IDC_STOLEN};

static int containerboxids[]={IDC_CONTAINERPICKER,IDC_POSX,IDC_POSY, IDC_TYPE,
IDC_LOCKED,IDC_DIFF,IDC_KEY,IDC_TRAPPED, IDC_DETECT,IDC_REMOVAL,IDC_DETECTED,
IDC_TPOSX, IDC_TPOSY, IDC_SCRIPT, IDC_POS1X, IDC_POS1Y, IDC_POS2X, IDC_POS2Y,
IDC_ITEMNUMPICKER,IDC_MAXITEM,IDC_ITEMRES,IDC_USE1,IDC_USE2,IDC_USE3,IDC_FLAGS,
IDC_UNKNOWN80,IDC_UNKNOWN82,IDC_STRREF, IDC_TEXT, IDC_VX,IDC_VY, IDC_MODVERTEX,
IDC_VERTICES,IDC_VERTEXNUM,IDC_RECALCBOX, IDC_ADDVERTEX, IDC_REMOVEVERTEX,
IDC_SCRIPTNAME,IDC_UNKNOWN8,IDC_IDENTIFIED,IDC_NOSTEAL,IDC_STOLEN,
IDC_BROWSE, IDC_BROWSE2,IDC_BROWSE3,IDC_ADDITEM,IDC_DELITEM,IDC_UNKNOWN,
IDC_COPY, IDC_PASTE, IDC_REMOVE,IDC_HIDDEN, IDC_NOPC,
0};

//all except additem
static int itemboxids[]={IDC_IDENTIFIED,IDC_NOSTEAL,IDC_STOLEN,IDC_BROWSE,IDC_DELITEM,
IDC_ITEMNUMPICKER,IDC_ITEMRES,IDC_USE1,IDC_USE2,IDC_USE3,IDC_FLAGS,IDC_UNKNOWN8,
-1};
#pragma warning(disable:4706)   
void CAreaContainer::DoDataExchange(CDataExchange* pDX)
{
  CWnd *cb;
  CButton *cb2;
  CString tmpstr;
  int flg, flg2, flg3;
  int i,j;
  int id;
  int itemnum;

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAreaContainer)
	DDX_Control(pDX, IDC_VERTICES, m_vertexpicker);
	DDX_Control(pDX, IDC_ITEMNUMPICKER, m_itemnumpicker);
	DDX_Control(pDX, IDC_TYPE, m_containertype_control);
	DDX_Control(pDX, IDC_CONTAINERPICKER, m_containerpicker);
	DDX_Text(pDX, IDC_VX, m_vx);
	DDX_Text(pDX, IDC_VY, m_vy);
	//}}AFX_DATA_MAP
  cb=GetDlgItem(IDC_MAXCONTAINER);
  flg=m_containernum>=0;
  if(flg) flg2=!!the_area.containerheaders[m_containernum].itemcount;
  else flg2=false;
  if(flg) flg3=!!the_area.containerheaders[m_containernum].vertexcount;
  else flg3=false;
  tmpstr.Format("/ %d",the_area.containercount);
  cb->SetWindowText(tmpstr);
  i=0;
  while(id=containerboxids[i])
  {
    cb=GetDlgItem(id);
    if(member_array(id,itemboxids)!=-1)
    {
      cb->EnableWindow(flg2);
    }
    else
    {
      if(member_array(id,vertexids)!=-1) cb->EnableWindow(flg3);
      else cb->EnableWindow(flg);
    }
    i++;
  }
  if(flg)
  {
    cb=GetDlgItem(IDC_VERTEXNUM);
    tmpstr.Format("/ %d",the_area.containerheaders[m_containernum].vertexcount);
    cb->SetWindowText(tmpstr);

    tmpstr.Format("%d. %-.32s",m_containernum+1,the_area.containerheaders[m_containernum].containername);
    DDX_Text(pDX, IDC_CONTAINERPICKER, tmpstr);

    DDX_Text(pDX, IDC_POSX, the_area.containerheaders[m_containernum].posx);
    DDX_Text(pDX, IDC_POSY, the_area.containerheaders[m_containernum].posy);
    tmpstr.Format("/ %d",the_area.containerheaders[m_containernum].itemcount);
    DDX_Text(pDX, IDC_MAXITEM, tmpstr);
    DDX_Text(pDX, IDC_DIFF, the_area.containerheaders[m_containernum].lockdiff);

    RetrieveResref(tmpstr,the_area.containerheaders[m_containernum].keyitem);
    DDX_Text(pDX, IDC_KEY, tmpstr);
    DDV_MaxChars(pDX, tmpstr,8);
    StoreResref(tmpstr,the_area.containerheaders[m_containernum].keyitem);

    DDX_Text(pDX, IDC_LOCKED, the_area.containerheaders[m_containernum].locked);
    DDX_Text(pDX, IDC_DETECT, the_area.containerheaders[m_containernum].trapdetect);
    DDX_Text(pDX, IDC_REMOVAL, the_area.containerheaders[m_containernum].trapremove);
    DDX_Text(pDX, IDC_TRAPPED, the_area.containerheaders[m_containernum].trapped);
    DDX_Text(pDX, IDC_DETECTED, the_area.containerheaders[m_containernum].trapvisible);
    DDX_Text(pDX, IDC_TPOSX, the_area.containerheaders[m_containernum].launchx);
    DDX_Text(pDX, IDC_TPOSY, the_area.containerheaders[m_containernum].launchy);

    RetrieveResref(tmpstr,the_area.containerheaders[m_containernum].trapscript);
    DDX_Text(pDX, IDC_SCRIPT, tmpstr);
    DDV_MaxChars(pDX, tmpstr,8);
    StoreResref(tmpstr,the_area.containerheaders[m_containernum].trapscript);

    RetrieveVariable(tmpstr,the_area.containerheaders[m_containernum].trapname);
    DDX_Text(pDX, IDC_SCRIPTNAME, tmpstr);
    DDV_MaxChars(pDX, tmpstr,32);
    StoreName(tmpstr,the_area.containerheaders[m_containernum].trapname);

    DDX_Text(pDX, IDC_UNKNOWN80, the_area.containerheaders[m_containernum].unknown80);
    DDX_Text(pDX, IDC_UNKNOWN82, the_area.containerheaders[m_containernum].unknown82);

    DDX_Text(pDX, IDC_POS1X, the_area.containerheaders[m_containernum].p1x);
    DDX_Text(pDX, IDC_POS1Y, the_area.containerheaders[m_containernum].p1y);
    DDX_Text(pDX, IDC_POS2X, the_area.containerheaders[m_containernum].p2x);
    DDX_Text(pDX, IDC_POS2Y, the_area.containerheaders[m_containernum].p2y);
    DDX_Text(pDX, IDC_STRREF, the_area.containerheaders[m_containernum].strref);
    DDX_Text(pDX, IDC_TEXT, m_text);

    tmpstr=get_container_icon(the_area.containerheaders[m_containernum].type);
    DDX_Text(pDX, IDC_TYPE, tmpstr);
    the_area.containerheaders[m_containernum].type=(short) strtonum(tmpstr);
  }
  if(flg2)
  {
    itemnum=the_area.containerheaders[m_containernum].firstitem+m_itemnum;

    RetrieveResref(tmpstr, the_area.itemheaders[itemnum].itemname);
    DDX_Text(pDX, IDC_ITEMRES, tmpstr);
    DDV_MaxChars(pDX, tmpstr, 8);
    StoreResref(tmpstr, the_area.itemheaders[itemnum].itemname);

    j=1;
    for(i=0;i<3;i++)
    {
      cb2=(CButton *) GetDlgItem(itemflagids[i]);
      if(the_area.itemheaders[itemnum].flags&j)
      {
        cb2->SetCheck(true);
      }
      else
      {
        cb2->SetCheck(false);
      }
      j<<=1;
    }
    DDX_Text(pDX, IDC_FLAGS, the_area.itemheaders[itemnum].flags);
    for(i=0;i<3;i++)
    {
      DDX_Text(pDX, IDC_USE1+i, the_area.itemheaders[itemnum].usages[i]);
    }
    DDX_Text(pDX, IDC_UNKNOWN8, the_area.itemheaders[itemnum].unknown8);
  }
}
#pragma warning(default:4706)   
void CAreaContainer::RefreshContainer()
{
  CString tmpstr;
  POSITION pos;
  area_vertex *poi;
	CSchedule tmp;
  int i;
  int count;
  int saveidx;

  if(IsWindow(m_containerpicker) )
  {
    m_containernum=m_containerpicker.GetCurSel();
    if(m_containernum<0) m_containernum=0;
    m_containerpicker.ResetContent();
    for(i=0;i<the_area.containercount;i++)
    {
      tmpstr.Format("%d. %-.32s",i+1,the_area.containerheaders[i].containername);
      m_containerpicker.AddString(tmpstr);
    }
    if(m_containernum>=i) m_containernum=i-1;
    m_containernum=m_containerpicker.SetCurSel(m_containernum);
    if(m_containernum>=0)
    {
      ((CButton *) GetDlgItem(IDC_HIDDEN))->SetCheck(the_area.containerheaders[m_containernum].locked&8);
      ((CButton *) GetDlgItem(IDC_NOPC))->SetCheck(the_area.containerheaders[m_containernum].locked&32);
    }
  }
  else m_containernum=-1;

  if(m_containernum>=0)
  {   
    m_itemnum=m_itemnumpicker.GetCurSel();
    if(m_itemnum<0) m_itemnum=0;
    m_itemnumpicker.ResetContent();
    for(i=0;i<the_area.containerheaders[m_containernum].itemcount;i++)
    {
      tmpstr.Format("%d. %-.8s",i+1,the_area.itemheaders[the_area.containerheaders[m_containernum].firstitem+i].itemname);
      m_itemnumpicker.AddString(tmpstr);
    }
    if(m_itemnum>=i) m_itemnum=i-1;
    m_itemnumpicker.SetCurSel(m_itemnum);

    m_text=resolve_tlk_text(the_area.containerheaders[m_containernum].strref);
    pos=the_area.vertexheaderlist.FindIndex(the_area.triggercount+m_containernum);
    count=the_area.containerheaders[m_containernum].vertexcount;
    if(IsWindow(m_vertexpicker) && pos)
    {
      poi=(area_vertex *) the_area.vertexheaderlist.GetAt(pos);
      saveidx=m_vertexpicker.GetCurSel();
      if(saveidx<0) saveidx=0;
      m_vertexpicker.ResetContent();
      for(i=0;i<count;i++)
      {
        tmpstr.Format("%d. [%d.%d]",i+1,poi[i].point.x,poi[i].point.y);
        m_vertexpicker.AddString(tmpstr);
      }
      if(saveidx>=i) saveidx=i-1;
      m_vertexpicker.SetCurSel(saveidx);
    }
  }
}

BOOL CAreaContainer::OnInitDialog() 
{
  int i;

	CPropertyPage::OnInitDialog();	
	RefreshContainer();
  m_containertype_control.ResetContent();
  for(i=0;i<NUM_CITYPE;i++)
  {
    m_containertype_control.AddString(get_container_icon(i));
  }
  UpdateData(UD_DISPLAY);
	return TRUE;
}

BEGIN_MESSAGE_MAP(CAreaContainer, CPropertyPage)
	//{{AFX_MSG_MAP(CAreaContainer)
	ON_CBN_KILLFOCUS(IDC_CONTAINERPICKER, OnKillfocusContainerpicker)
	ON_CBN_SELCHANGE(IDC_CONTAINERPICKER, OnSelchangeContainerpicker)
	ON_EN_KILLFOCUS(IDC_POSX, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_POSY, DefaultKillfocus)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_COPY, OnCopy)
	ON_BN_CLICKED(IDC_PASTE, OnPaste)
	ON_CBN_KILLFOCUS(IDC_TYPE, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_DIFF, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_ITEMRES, DefaultKillfocus)
	ON_BN_CLICKED(IDC_IDENTIFIED, OnIdentified)
	ON_BN_CLICKED(IDC_NOSTEAL, OnNosteal)
	ON_BN_CLICKED(IDC_STOLEN, OnStolen)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_CBN_KILLFOCUS(IDC_ITEMNUMPICKER, OnKillfocusItemnumpicker)
	ON_CBN_SELCHANGE(IDC_ITEMNUMPICKER, OnSelchangeItemnumpicker)
	ON_EN_KILLFOCUS(IDC_FLAGS, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_USE1, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_USE2, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_USE3, DefaultKillfocus)
	ON_BN_CLICKED(IDC_ADDITEM, OnAdditem)
	ON_BN_CLICKED(IDC_DELITEM, OnDelitem)
	ON_EN_KILLFOCUS(IDC_TRAPPED, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_DETECT, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_REMOVAL, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_TPOSX, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_TPOSY, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_SCRIPT, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_POS1X, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_POS1Y, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_POS2X, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_POS2Y, DefaultKillfocus)
	ON_CBN_KILLFOCUS(IDC_VERTICES, OnKillfocusVertices)
	ON_BN_CLICKED(IDC_RECALCBOX, OnRecalcbox)
	ON_BN_CLICKED(IDC_ADDVERTEX, OnAddvertex)
	ON_BN_CLICKED(IDC_REMOVEVERTEX, OnRemovevertex)
	ON_EN_KILLFOCUS(IDC_UNKNOWN8, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_STRREF, OnKillfocusStrref)
	ON_EN_KILLFOCUS(IDC_TEXT, OnKillfocusText)
	ON_BN_CLICKED(IDC_BROWSE2, OnBrowse2)
	ON_BN_CLICKED(IDC_BROWSE3, OnBrowse3)
	ON_EN_KILLFOCUS(IDC_UNKNOWN80, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_UNKNOWN82, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_SCRIPTNAME, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_VX, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_VY, DefaultKillfocus)
	ON_CBN_SELCHANGE(IDC_VERTICES, OnSelchangeVertices)
	ON_BN_CLICKED(IDC_MODVERTEX, OnModvertex)
	ON_EN_KILLFOCUS(IDC_KEY, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_LOCKED, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_DETECTED, DefaultKillfocus)
	ON_BN_CLICKED(IDC_HIDDEN, OnHidden)
	ON_BN_CLICKED(IDC_NOPC, OnNopc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAreaContainer message handlers

void CAreaContainer::OnKillfocusContainerpicker() 
{
  CString tmpstr;
  int x;

  m_containerpicker.GetWindowText(tmpstr);
  x=strtonum(tmpstr)-1;
  if(x>=0 && x<=the_area.containercount)
  {
    m_containernum=m_containerpicker.SetCurSel(x);
  }
  else
  {
    if(m_containernum>=0 && m_containernum<=the_area.containercount)
    {
      StoreName(tmpstr, the_area.containerheaders[m_containernum].containername);
    }
  }
  RefreshContainer();
	UpdateData(UD_DISPLAY);	
}

void CAreaContainer::OnSelchangeContainerpicker() 
{
  int x;

  x=m_containerpicker.GetCurSel();
  if(x>=0 && x<=the_area.containercount) m_containernum=x;
  RefreshContainer();
	UpdateData(UD_DISPLAY);	
}

void CAreaContainer::DefaultKillfocus() 
{
  UpdateData(UD_RETRIEVE);	
  UpdateData(UD_DISPLAY);	
}
/*
void CAreaContainer::OnKillfocusPosy() 
{
  UpdateData(UD_RETRIEVE);	
  UpdateData(UD_DISPLAY);	
}

void CAreaContainer::OnKillfocusType() 
{
  UpdateData(UD_RETRIEVE);	
  UpdateData(UD_DISPLAY);	
}

void CAreaContainer::OnKillfocusKey() 
{
  UpdateData(UD_RETRIEVE);	
  UpdateData(UD_DISPLAY);	
}

void CAreaContainer::OnKillfocusLocked() 
{
  UpdateData(UD_RETRIEVE);	
  UpdateData(UD_DISPLAY);	
}

void CAreaContainer::OnKillfocusDiff() 
{
  UpdateData(UD_RETRIEVE);	
  UpdateData(UD_DISPLAY);	
}

void CAreaContainer::OnKillfocusTrapped() 
{
  UpdateData(UD_RETRIEVE);	
  UpdateData(UD_DISPLAY);	
}

void CAreaContainer::OnKillfocusDetected() 
{
  UpdateData(UD_RETRIEVE);	
  UpdateData(UD_DISPLAY);	
}

void CAreaContainer::OnKillfocusDetect() 
{
  UpdateData(UD_RETRIEVE);	
  UpdateData(UD_DISPLAY);	
}

void CAreaContainer::OnKillfocusRemoval() 
{
  UpdateData(UD_RETRIEVE);	
  UpdateData(UD_DISPLAY);	
}

void CAreaContainer::OnKillfocusTposx() 
{
  UpdateData(UD_RETRIEVE);	
  UpdateData(UD_DISPLAY);	
}

void CAreaContainer::OnKillfocusTposy() 
{
  UpdateData(UD_RETRIEVE);	
  UpdateData(UD_DISPLAY);	
}

void CAreaContainer::OnKillfocusScript() 
{
  UpdateData(UD_RETRIEVE);	
  UpdateData(UD_DISPLAY);	
}

void CAreaContainer::OnKillfocusScriptname() 
{
  UpdateData(UD_RETRIEVE);	
  UpdateData(UD_DISPLAY);	
}

void CAreaContainer::OnKillfocusUnknown80() 
{
  UpdateData(UD_RETRIEVE);	
  UpdateData(UD_DISPLAY);	
}

void CAreaContainer::OnKillfocusUnknown82() 
{
  UpdateData(UD_RETRIEVE);	
  UpdateData(UD_DISPLAY);	
}

void CAreaContainer::OnKillfocusPos1x() 
{
  UpdateData(UD_RETRIEVE);	
  UpdateData(UD_DISPLAY);	
}

void CAreaContainer::OnKillfocusPos1y() 
{
  UpdateData(UD_RETRIEVE);	
  UpdateData(UD_DISPLAY);	
}

void CAreaContainer::OnKillfocusPos2x() 
{
  UpdateData(UD_RETRIEVE);	
  UpdateData(UD_DISPLAY);	
}

void CAreaContainer::OnKillfocusPos2y() 
{
  UpdateData(UD_RETRIEVE);	
  UpdateData(UD_DISPLAY);	
}

void CAreaContainer::OnKillfocusVx() 
{
  UpdateData(UD_RETRIEVE);	
  UpdateData(UD_DISPLAY);	
}

void CAreaContainer::OnKillfocusVy() 
{
  UpdateData(UD_RETRIEVE);	
  UpdateData(UD_DISPLAY);	
}
*/
void CAreaContainer::OnHidden() 
{
	the_area.containerheaders[m_containernum].locked^=1;
  RefreshContainer();
	UpdateData(UD_DISPLAY);
}

void CAreaContainer::OnNopc() 
{
	the_area.containerheaders[m_containernum].locked^=32;
  RefreshContainer();
	UpdateData(UD_DISPLAY);
}

void CAreaContainer::OnKillfocusVertices() 
{
  POSITION pos;
  area_vertex *poi;
  int vertexnum;

  UpdateData(UD_RETRIEVE);	
  if(m_containernum<0) return;
  pos=the_area.vertexheaderlist.FindIndex(the_area.triggercount+m_containernum);
  poi=(area_vertex *) the_area.vertexheaderlist.GetAt(pos);
  vertexnum=m_vertexpicker.GetCurSel();
  if(vertexnum<0)
  {
    m_vx=-1;
    m_vy=-1;
  }
  else
  {
    m_vx=poi[vertexnum].point.x;
    m_vy=poi[vertexnum].point.y;
  }
  UpdateData(UD_DISPLAY);	
}

void CAreaContainer::OnSelchangeVertices() 
{
  POSITION pos;
  area_vertex *poi;
  int vertexnum;

  if(m_containernum<0) return;
  pos=the_area.vertexheaderlist.FindIndex(the_area.triggercount+m_containernum);
  poi=(area_vertex *) the_area.vertexheaderlist.GetAt(pos);
  vertexnum=m_vertexpicker.GetCurSel();
  if(vertexnum<0)
  {
    m_vx=-1;
    m_vy=-1;
  }
  else
  {
    m_vx=poi[vertexnum].point.x;
    m_vy=poi[vertexnum].point.y;
  }
  UpdateData(UD_DISPLAY);	
}

void CAreaContainer::OnRecalcbox() 
{
  POINTS min, max;

  RecalcBox(the_area.containerheaders[m_containernum].vertexcount,
            the_area.triggercount+m_containernum,min,max);
  the_area.containerheaders[m_containernum].p1x=min.x;
  the_area.containerheaders[m_containernum].p1y=min.y;
  the_area.containerheaders[m_containernum].p2x=max.x;
  the_area.containerheaders[m_containernum].p2y=max.y;
  UpdateData(UD_DISPLAY);	
}

void CAreaContainer::OnAddvertex() 
{
  POSITION pos;
  area_vertex *poi;
  area_vertex *poi2;
  int vertexsize;
  int vertexnum;

	UpdateData(UD_RETRIEVE);
  vertexnum=m_vertexpicker.GetCurSel();
  if(m_containernum<0)
  {
    return;
  }
  pos=the_area.vertexheaderlist.FindIndex(the_area.triggercount+m_containernum);
  poi=(area_vertex *) the_area.vertexheaderlist.GetAt(pos);
  vertexsize=the_area.containerheaders[m_containernum].vertexcount;
  poi2=new area_vertex[vertexsize+1];
  if(!poi2)
  {
    return;
  }
  vertexnum++;
  memcpy(poi2,poi,vertexnum*sizeof(area_vertex) );
  memcpy(poi2+vertexnum+1,poi+vertexnum,(vertexsize-vertexnum)*sizeof(area_vertex) );
  poi2[vertexnum].point.x=(short) m_vx;
  poi2[vertexnum].point.y=(short) m_vy;
  the_area.vertexheaderlist.SetAt(pos,poi2); //this will free memory at *poi as well
  the_area.containerheaders[m_containernum].vertexcount=(short) (vertexsize+1);
  RefreshContainer();
  OnRecalcbox(); //this calls updatedata 	
}

void CAreaContainer::OnRemovevertex() 
{
  POSITION pos;
  area_vertex *poi;
  area_vertex *poi2;
  int vertexsize;
  int vertexnum;

	UpdateData(UD_RETRIEVE);
  vertexnum=m_vertexpicker.GetCurSel();
  if(m_containernum<0 || vertexnum<0)
  {
    return;
  }
  pos=the_area.vertexheaderlist.FindIndex(the_area.triggercount+m_containernum);
  poi=(area_vertex *) the_area.vertexheaderlist.GetAt(pos);
  vertexsize=the_area.containerheaders[m_containernum].vertexcount;
  poi2=new area_vertex[--vertexsize];
  if(!poi2)
  {
    return;
  }
  memcpy(poi2,poi,vertexnum*sizeof(area_vertex) );
  memcpy(poi2+vertexnum,poi+vertexnum+1,(vertexsize-vertexnum)*sizeof(area_vertex) );
  the_area.vertexheaderlist.SetAt(pos,poi2); //this will free memory at *poi as well
  the_area.containerheaders[m_containernum].vertexcount=(short) vertexsize;
  RefreshContainer();
  OnRecalcbox(); //calls updatedata
}

void CAreaContainer::OnModvertex() 
{
  POSITION pos;
  area_vertex *poi;
  int vertexnum;

	UpdateData(UD_RETRIEVE);
  pos=the_area.vertexheaderlist.FindIndex(the_area.triggercount+m_containernum);
  poi=(area_vertex *) the_area.vertexheaderlist.GetAt(pos);
  if(!poi) return;
  vertexnum=m_vertexpicker.GetCurSel();
  if(vertexnum<0) return;
  poi[vertexnum].point.x=(short) m_vx;
  poi[vertexnum].point.y=(short) m_vy;
  RefreshContainer();
  OnRecalcbox(); //calls updatedata
}

void CAreaContainer::OnKillfocusStrref() 
{
  UpdateData(UD_RETRIEVE);	
  RefreshContainer();
  UpdateData(UD_DISPLAY);	
}

void CAreaContainer::OnKillfocusText() 
{
  CString old;

	UpdateData(UD_RETRIEVE);
  old=resolve_tlk_text(the_area.containerheaders[m_containernum].strref);
  if(old!=m_text)
  {
    if(editflg&TLKCHANGE)
    {
      if(MessageBox("Do you want to update dialog.tlk?","Area editor",MB_YESNO)!=IDYES)
      {
        return;
      }
    }
    the_area.containerheaders[m_containernum].strref=store_tlk_text(the_area.containerheaders[m_containernum].strref, m_text);
  }
  RefreshContainer();
	UpdateData(UD_DISPLAY);
}

void CAreaContainer::OnKillfocusItemnumpicker() 
{
  CString tmpstr;
  int x;

  m_itemnumpicker.GetWindowText(tmpstr);
  x=strtonum(tmpstr)-1;
  if(x>=0 && x<=the_area.containerheaders[m_containernum].itemcount)
  {
    m_itemnum=x;
    m_itemnumpicker.SetCurSel(m_itemnum);
  }
  RefreshContainer();
	UpdateData(UD_DISPLAY);
}

void CAreaContainer::OnSelchangeItemnumpicker() 
{
  int x;

  x=m_itemnumpicker.GetCurSel();
  if(x>=0 && x<=the_area.containerheaders[m_containernum].itemcount) m_itemnum=x;
  RefreshContainer();
	UpdateData(UD_DISPLAY);
}

void CAreaContainer::OnIdentified() 
{
  int itemnum;
	
  itemnum=the_area.containerheaders[m_containernum].firstitem+m_itemnum;
  the_area.itemheaders[itemnum].flags^=1;
  UpdateData(UD_DISPLAY);	
}

void CAreaContainer::OnNosteal() 
{
  int itemnum;
	
  itemnum=the_area.containerheaders[m_containernum].firstitem+m_itemnum;
  the_area.itemheaders[itemnum].flags^=2;
  UpdateData(UD_DISPLAY);	
}

void CAreaContainer::OnStolen() 
{
  int itemnum;
	
  itemnum=the_area.containerheaders[m_containernum].firstitem+m_itemnum;
  the_area.itemheaders[itemnum].flags^=4;
  UpdateData(UD_DISPLAY);	
}

void CAreaContainer::OnBrowse() 
{
  int itemnum;
  
  if(m_containernum<0 || m_itemnum<0) return;
  itemnum=the_area.containerheaders[m_containernum].firstitem+m_itemnum;
  pickerdlg.m_restype=REF_ITM;
  RetrieveResref(pickerdlg.m_picked,the_area.itemheaders[itemnum].itemname);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_area.itemheaders[itemnum].itemname);
  }
  RefreshContainer();
  UpdateData(UD_DISPLAY);	
}

void CAreaContainer::OnAdditem() 
{
  area_item *newitems;
  int itemnum;
  int i;

  if(m_containernum<0) return;
  if(((unsigned int) the_area.header.itemcnt)>32000)
  {
    MessageBox("Cannot add more items to area.","Area Editor",MB_OK);
    return;
  }
  itemnum=the_area.containerheaders[m_containernum].firstitem+m_itemnum+1;
  newitems=new area_item[++the_area.header.itemcnt];
  if(!newitems)
  {
    the_area.header.itemcnt--;
    return;
  }
  memcpy(newitems,the_area.itemheaders,itemnum*sizeof(area_item) );
  memcpy(newitems+itemnum+1,the_area.itemheaders+itemnum,(the_area.itemcount-itemnum)*sizeof(area_item) );
  memset(newitems+itemnum,0,sizeof(area_item) );

  if(the_area.itemheaders) delete [] the_area.itemheaders;
  the_area.itemheaders=newitems;
  the_area.itemcount=the_area.header.itemcnt;

  //adjusting item references
  for(i=0;i<the_area.containercount;i++)
  {
    if(i==m_containernum)
    {
      the_area.containerheaders[i].itemcount++;
    }
    else
    {
      if(the_area.containerheaders[i].firstitem>=itemnum)
      {
        the_area.containerheaders[i].firstitem++;
      }
    }
  }
  m_itemnum++;
  RefreshContainer();
  UpdateData(UD_DISPLAY);	
}

void CAreaContainer::OnDelitem() 
{
  area_item *newitems;
  int itemnum;
  int i;

  if(m_containernum<0) return;
  itemnum=the_area.containerheaders[m_containernum].firstitem+m_itemnum;
  newitems=new area_item[--the_area.header.itemcnt];
  if(!newitems)
  {
    the_area.header.itemcnt++;
    return;
  }
  memcpy(newitems,the_area.itemheaders,itemnum*sizeof(area_item) );
  memcpy(newitems+itemnum,the_area.itemheaders+itemnum+1,(the_area.header.itemcnt-itemnum)*sizeof(area_item) );

  if(the_area.itemheaders) delete [] the_area.itemheaders;
  the_area.itemheaders=newitems;
  the_area.itemcount=the_area.header.itemcnt;

  //adjusting item references
  for(i=0;i<the_area.containercount;i++)
  {
    if(m_containernum==i)
    {
      the_area.containerheaders[i].itemcount--;
    }
    else
    {
      if(the_area.containerheaders[i].firstitem>=itemnum)
      {
        the_area.containerheaders[i].firstitem--;
      }
    }
  }	
  RefreshContainer();
  UpdateData(UD_DISPLAY);	
}

void CAreaContainer::OnAdd() 
{
  area_container *newcontainers;
  area_vertex *newvertex;
  CString tmpstr;
  int count;
  
  if(the_area.header.containercnt>999)
  {
    MessageBox("Cannot add more containers.","Warning",MB_ICONEXCLAMATION|MB_OK);
    return;
  }
  count=the_area.containercount;
  newcontainers=new area_container[++the_area.header.containercnt];
  if(!newcontainers)
  {
    the_area.header.containercnt--;
    MessageBox("Not enough memory.","Error",MB_ICONSTOP|MB_OK);
    return;
  }
  newvertex=new area_vertex[0];
  the_area.vertexheaderlist.InsertAfter(the_area.vertexheaderlist.FindIndex(the_area.triggercount+count), newvertex);

  memcpy(newcontainers, the_area.containerheaders, count*sizeof(area_container));
  memset(newcontainers+count,0,sizeof(area_container) );
  tmpstr.Format("Container %d",the_area.header.containercnt);
  StoreName(tmpstr,newcontainers[count].containername);
  if(count)
  {
    newcontainers[count].firstitem=newcontainers[count-1].firstitem+newcontainers[count-1].itemcount;
  }
  else
  {
    newcontainers[count].firstitem=0;
  }
  newcontainers[count].strref=-1;
  
  if(the_area.containerheaders)
  {
    delete [] the_area.containerheaders;
  }
  the_area.containerheaders=newcontainers;
  m_containernum=count; //last element
  the_area.containercount=the_area.header.containercnt;

  RefreshContainer();
  UpdateData(UD_DISPLAY);	
}

void CAreaContainer::OnRemove() 
{
  area_container *newcontainers;
  area_item *newitems;
  int first, count;

  if(m_containernum<0 || !the_area.header.containercnt) return;  
  newcontainers=new area_container[--the_area.header.containercnt];
  if(!newcontainers)
  {
    the_area.header.containercnt++;
    return;
  }
  count=the_area.containerheaders[m_containernum].itemcount;  
  if(count)
  {
    first=the_area.containerheaders[m_containernum].firstitem;
    the_area.header.itemcnt=(short) (the_area.header.itemcnt-count);
    newitems=new area_item[the_area.header.itemcnt];
    if(!newitems) //rollback fully
    {
      the_area.header.containercnt++;
      the_area.header.itemcnt=(short) (the_area.header.itemcnt+count);
      return;
    }
    memcpy(newitems, the_area.itemheaders, first * sizeof(area_item) );
    memcpy(newitems+first, the_area.itemheaders+first+count, (the_area.itemcount-first-count)*sizeof(area_item) );
    if(the_area.itemheaders) delete [] the_area.itemheaders;
    the_area.itemheaders=newitems;
    the_area.itemcount=the_area.header.itemcnt;

    for(first=0;first<the_area.containercount;first++)
    {
      if(the_area.containerheaders[first].firstitem>=first)
      {
        the_area.containerheaders[first].firstitem-=count;
      }
    }
  }

  memcpy(newcontainers,the_area.containerheaders,m_containernum*sizeof(area_container) );
  memcpy(newcontainers+m_containernum,the_area.containerheaders+m_containernum+1,
         (the_area.header.containercnt-m_containernum)*sizeof(area_container) );
  if(the_area.containerheaders)
  {
    delete [] the_area.containerheaders;
  }
  the_area.containerheaders=newcontainers;
  the_area.containercount=the_area.header.containercnt;
  the_area.vertexheaderlist.RemoveAt(the_area.vertexheaderlist.FindIndex(the_area.triggercount+m_containernum) );
	if(m_containernum>=the_area.containercount) m_containernum--;
  RefreshContainer();
  UpdateData(UD_DISPLAY);	
}

void CAreaContainer::OnCopy() 
{
  int vertexnum;
  POSITION pos;

  if(m_containernum>=0)
  {
    vertexnum=the_area.triggercount+m_containernum;
    pos=the_area.vertexheaderlist.FindIndex(vertexnum);
    vertexsize=the_area.containerheaders[m_containernum].vertexcount;
    if(vertexcopy) delete [] vertexcopy;
    vertexcopy=new area_vertex[vertexsize];
    memcpy(&containercopy,the_area.containerheaders+m_containernum,sizeof(containercopy) );
    memcpy(vertexcopy,the_area.vertexheaderlist.GetAt(pos),vertexsize*sizeof(area_vertex));
  }
}

void CAreaContainer::OnPaste() 
{
  POSITION pos;
  area_vertex *poi;
  int vertexnum;

  if(m_containernum>=0)
  {
    vertexnum=the_area.triggercount+m_containernum;
    pos=the_area.vertexheaderlist.FindIndex(vertexnum);
    poi=new area_vertex[vertexsize];
    if(poi)
    {
      memcpy(poi,vertexcopy,vertexsize*sizeof(area_vertex));
    }
    the_area.vertexheaderlist.SetAt(pos,poi); //this will also free the old element
    memcpy(the_area.containerheaders+m_containernum,&containercopy, sizeof(containercopy) );
  }
  RefreshContainer();
  UpdateData(UD_DISPLAY);	
}

void CAreaContainer::OnBrowse2() 
{
  if(m_containernum<0) return;
  pickerdlg.m_restype=REF_ITM;
  RetrieveResref(pickerdlg.m_picked,the_area.containerheaders[m_containernum].keyitem);
  if(pickerdlg.DoModal()==IDOK)
  {    
    StoreResref(pickerdlg.m_picked,the_area.containerheaders[m_containernum].keyitem);
  }
  UpdateData(UD_DISPLAY);	
}

void CAreaContainer::OnBrowse3() 
{
  if(m_containernum<0) return;
  pickerdlg.m_restype=REF_BCS;
  RetrieveResref(pickerdlg.m_picked,the_area.containerheaders[m_containernum].trapscript);
  if(pickerdlg.DoModal()==IDOK)
  {    
    StoreResref(pickerdlg.m_picked,the_area.containerheaders[m_containernum].trapscript);
  }
  UpdateData(UD_DISPLAY);	
}

/////////////////////////////////////////////////////////////////////////////
// CAreaVariable dialog

CAreaVariable::CAreaVariable() : CPropertyPage(CAreaVariable::IDD)
{
	//{{AFX_DATA_INIT(CAreaVariable)
	//}}AFX_DATA_INIT
  m_variablenum=-1;
  memset(&variablecopy,0,sizeof(variablecopy));
  memset(&notecopy,0,sizeof(notecopy));
}

CAreaVariable::~CAreaVariable()
{
}

static int variableboxids[]={IDC_VARIABLEPICKER, IDC_VARIABLE, IDC_VALUE,
IDC_UNKNOWN20, IDC_UNKNOWN24, IDC_UNKNOWN, IDC_REMOVE, IDC_COPY, IDC_PASTE,
0};

static int noteboxids[]={IDC_NOTEPICKER,IDC_POS1X, IDC_POS1Y,IDC_STRREF,IDC_NOTE,
IDC_COLOR,IDC_UNKNOWNC, IDC_UNKNOWN2, IDC_REMOVE2, IDC_COPY2, IDC_PASTE2, IDC_TAGGED,
IDC_SET,
0};

void CAreaVariable::DoDataExchange(CDataExchange* pDX)
{
  CWnd *cb;
  CString tmpstr;
  int flg;
  int i;

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAreaVariable)
	DDX_Control(pDX, IDC_NOTEPICKER, m_notepicker);
	DDX_Control(pDX, IDC_VARIABLEPICKER, m_variablepicker);
	//}}AFX_DATA_MAP
  cb=GetDlgItem(IDC_MAXVARIABLE);
  flg=m_variablenum>=0;
  tmpstr.Format("/ %d",the_area.variablecount);
  cb->SetWindowText(tmpstr);
  i=0;
  while(variableboxids[i])
  {
    cb=GetDlgItem(variableboxids[i++]);
    cb->EnableWindow(flg);
  }
  if(flg)
  {
    tmpstr.Format("%d. %-.32s",m_variablenum+1,the_area.variableheaders[m_variablenum].variablename);
    DDX_Text(pDX, IDC_VARIABLEPICKER, tmpstr);    

    RetrieveVariable(tmpstr,the_area.variableheaders[m_variablenum].variablename);
    DDX_Text(pDX,IDC_VARIABLE,tmpstr);
    DDV_MaxChars(pDX,tmpstr,32);
    StoreName(tmpstr,the_area.variableheaders[m_variablenum].variablename);

    DDX_Text(pDX, IDC_VALUE, the_area.variableheaders[m_variablenum].value);
    DDX_Text(pDX, IDC_UNKNOWN20, the_area.variableheaders[m_variablenum].unknown20);
    DDX_Text(pDX, IDC_UNKNOWN24, the_area.variableheaders[m_variablenum].unknown24);
  }
  ////other side
  cb=GetDlgItem(IDC_MAXNOTE);
  flg=m_notenum>=0;
  tmpstr.Format("/ %d",the_area.mapnotecount);
  cb->SetWindowText(tmpstr);
  i=0;
  while(noteboxids[i])
  {
    cb=GetDlgItem(noteboxids[i++]);
    cb->EnableWindow(flg);
  }
  if(flg)
  {
    tmpstr.Format("%d. %-.40s",m_notenum+1,resolve_tlk_text(the_area.mapnoteheaders[m_notenum].strref));
    DDX_Text(pDX, IDC_NOTEPICKER, tmpstr);    

    DDX_Text(pDX, IDC_POS1X, the_area.mapnoteheaders[m_notenum].px);
    DDX_Text(pDX, IDC_POS1Y, the_area.mapnoteheaders[m_notenum].py);
    DDX_Text(pDX, IDC_STRREF, the_area.mapnoteheaders[m_notenum].strref);
    DDX_Text(pDX, IDC_NOTE, m_infostr);

    i=the_area.mapnoteheaders[m_notenum].colour;
    DDX_CBIndex(pDX, IDC_COLOR, i);
    the_area.mapnoteheaders[m_notenum].colour=(short) i;
    DDX_Text(pDX, IDC_UNKNOWNC, the_area.mapnoteheaders[m_notenum].unknownc);
 }
}

void CAreaVariable::RefreshVariable()
{
  CString tmpstr;
	CSchedule tmp;
  int i;

  if(IsWindow(m_variablepicker) )
  {
    m_variablenum=m_variablepicker.GetCurSel();
    if(m_variablenum<0) m_variablenum=0;
    m_variablepicker.ResetContent();
    for(i=0;i<the_area.variablecount;i++)
    {
      tmpstr.Format("%d. %-.32s",i+1,the_area.variableheaders[i].variablename);
      m_variablepicker.AddString(tmpstr);
    }
    if(m_variablenum>=i) m_variablenum=i-1;
    m_variablenum=m_variablepicker.SetCurSel(m_variablenum);
  }
  else m_variablenum=-1;
  if(IsWindow(m_notepicker) )
  {
    m_notenum=m_notepicker.GetCurSel();
    if(m_notenum<0) m_notenum=0;
    m_notepicker.ResetContent();
    for(i=0;i<the_area.mapnotecount;i++)
    {
      tmpstr.Format("%d. %-.40s",i+1,resolve_tlk_text(the_area.mapnoteheaders[i].strref));
      m_notepicker.AddString(tmpstr);
    }
    if(m_notenum>=i) m_notenum=i-1;
    m_notenum=m_notepicker.SetCurSel(m_notenum);
  }
  else m_notenum=-1;

  if(m_notenum>=0)
  {
    m_infostr=resolve_tlk_text(the_area.mapnoteheaders[m_notenum].strref);
  }
  else
  {
    m_infostr="-- not available --";
  }
}

BOOL CAreaVariable::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
  RefreshVariable();
  UpdateData(UD_DISPLAY);
	return TRUE;
}

BEGIN_MESSAGE_MAP(CAreaVariable, CPropertyPage)
	//{{AFX_MSG_MAP(CAreaVariable)
	ON_CBN_KILLFOCUS(IDC_VARIABLEPICKER, OnKillfocusVariablepicker)
	ON_CBN_SELCHANGE(IDC_VARIABLEPICKER, OnSelchangeVariablepicker)
	ON_CBN_KILLFOCUS(IDC_NOTEPICKER, OnKillfocusNotepicker)
	ON_CBN_SELCHANGE(IDC_NOTEPICKER, OnSelchangeNotepicker)
	ON_EN_KILLFOCUS(IDC_VARIABLE, OnKillfocusVariable)
	ON_EN_KILLFOCUS(IDC_UNKNOWN20, OnKillfocusUnknown20)
	ON_EN_KILLFOCUS(IDC_VALUE, OnKillfocusValue)
	ON_EN_KILLFOCUS(IDC_POS1X, OnKillfocusPos1x)
	ON_EN_KILLFOCUS(IDC_POS1Y, OnKillfocusPos1y)
	ON_EN_KILLFOCUS(IDC_STRREF, OnKillfocusStrref)
	ON_CBN_KILLFOCUS(IDC_COLOR, OnKillfocusColor)
	ON_EN_KILLFOCUS(IDC_NOTE, OnKillfocusNote)
	ON_EN_KILLFOCUS(IDC_UNKNOWNC, OnKillfocusUnknownc)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_COPY, OnCopy)
	ON_BN_CLICKED(IDC_PASTE, OnPaste)
	ON_BN_CLICKED(IDC_UNKNOWN, OnUnknown)
	ON_BN_CLICKED(IDC_ADD2, OnAdd2)
	ON_BN_CLICKED(IDC_REMOVE2, OnRemove2)
	ON_BN_CLICKED(IDC_COPY2, OnCopy2)
	ON_BN_CLICKED(IDC_PASTE2, OnPaste2)
	ON_BN_CLICKED(IDC_UNKNOWN2, OnUnknown2)
	ON_EN_KILLFOCUS(IDC_UNKNOWN24, OnKillfocusUnknown24)
	ON_BN_CLICKED(IDC_TAGGED, OnTagged)
	ON_BN_CLICKED(IDC_SET, OnSet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAreaVariable message handlers

void CAreaVariable::OnKillfocusVariablepicker() 
{
  CString tmpstr;
  int x;

  m_variablepicker.GetWindowText(tmpstr);
  x=strtonum(tmpstr)-1;
  if(x>=0 && x<=the_area.variablecount)
  {
    m_variablenum=m_variablepicker.SetCurSel(x);
  }
  else
  {
    if(m_variablenum>=0 && m_variablenum<=the_area.variablecount)
    {
      StoreName(tmpstr, the_area.variableheaders[m_variablenum].variablename);
    }
  }
  RefreshVariable();
	UpdateData(UD_DISPLAY);		
}

void CAreaVariable::OnSelchangeVariablepicker() 
{
  int x;

  x=m_variablepicker.GetCurSel();
  if(x>=0 && x<=the_area.variablecount) m_variablenum=x;
  RefreshVariable();
	UpdateData(UD_DISPLAY);	
}

void CAreaVariable::OnKillfocusNotepicker() 
{
  CString tmpstr;
  int x;

  m_notepicker.GetWindowText(tmpstr);
  x=strtonum(tmpstr)-1;
  if(x>=0 && x<=the_area.mapnotecount)
  {
    m_notenum=x;
    m_notepicker.SetCurSel(m_notenum);
  }
  RefreshVariable();
	UpdateData(UD_DISPLAY);		
}

void CAreaVariable::OnSelchangeNotepicker() 
{
  int x;

  x=m_notepicker.GetCurSel();
  if(x>=0 && x<=the_area.mapnotecount) m_notenum=x;
  RefreshVariable();
	UpdateData(UD_DISPLAY);	
}

void CAreaVariable::OnKillfocusVariable() 
{
	UpdateData(UD_RETRIEVE);		
	UpdateData(UD_DISPLAY);		
}

void CAreaVariable::OnKillfocusUnknown20() 
{
	UpdateData(UD_RETRIEVE);		
	UpdateData(UD_DISPLAY);		
}

void CAreaVariable::OnKillfocusUnknown24() 
{
	UpdateData(UD_RETRIEVE);		
	UpdateData(UD_DISPLAY);		
}

void CAreaVariable::OnKillfocusValue() 
{
	UpdateData(UD_RETRIEVE);		
	UpdateData(UD_DISPLAY);		
}

void CAreaVariable::OnKillfocusPos1x() 
{
	UpdateData(UD_RETRIEVE);		
	UpdateData(UD_DISPLAY);		
}

void CAreaVariable::OnKillfocusPos1y() 
{
	UpdateData(UD_RETRIEVE);		
	UpdateData(UD_DISPLAY);		
}

void CAreaVariable::OnKillfocusStrref() 
{
	UpdateData(UD_RETRIEVE);
	RefreshVariable();	
	UpdateData(UD_DISPLAY);		
}

void CAreaVariable::OnTagged() 
{
  if(editflg&TLKCHANGE)
  {
    if(MessageBox("Do you want to update dialog.tlk?","Area editor",MB_YESNO)!=IDYES)
    {
      return;
    }
  }
  toggle_tlk_tag(the_area.mapnoteheaders[m_notenum].strref);
	UpdateData(UD_DISPLAY);		
}

void CAreaVariable::OnKillfocusNote() 
{
  CString old;

	UpdateData(UD_RETRIEVE);
  old=resolve_tlk_text(the_area.mapnoteheaders[m_notenum].strref);
  if(old!=m_infostr)
  {
    if(editflg&TLKCHANGE)
    {
      if(MessageBox("Do you want to update dialog.tlk?","Area editor",MB_YESNO)!=IDYES)
      {
        return;
      }
    }
    the_area.mapnoteheaders[m_notenum].strref=store_tlk_text(the_area.mapnoteheaders[m_notenum].strref, m_infostr);
  }
  RefreshVariable();
	UpdateData(UD_DISPLAY);		
}

void CAreaVariable::OnKillfocusColor() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);		
}

void CAreaVariable::OnKillfocusUnknownc() 
{
	UpdateData(UD_RETRIEVE);		
	UpdateData(UD_DISPLAY);		
}

void CAreaVariable::OnAdd() 
{
  area_variable *newvariables;
  CString tmpstr;
  
  if(the_area.header.variablecnt>999)
  {
    MessageBox("Cannot add more variables.","Warning",MB_ICONEXCLAMATION|MB_OK);
    return;
  }
  newvariables=new area_variable[++the_area.header.variablecnt];
  if(!newvariables)
  {
    the_area.header.variablecnt--;
    MessageBox("Not enough memory.","Error",MB_ICONSTOP|MB_OK);
    return;
  }
  memcpy(newvariables, the_area.variableheaders, the_area.variablecount*sizeof(area_variable));
  memset(newvariables+the_area.variablecount,0,sizeof(area_variable) );
  tmpstr.Format("Variable %d",the_area.header.variablecnt);
  StoreName(tmpstr,newvariables[the_area.variablecount].variablename);
  
  if(the_area.variableheaders)
  {
    delete [] the_area.variableheaders;
  }
  the_area.variableheaders=newvariables;
  m_variablenum=the_area.variablecount; //last element
  the_area.variablecount=the_area.header.variablecnt;

  RefreshVariable();
	UpdateData(UD_DISPLAY);		
}

void CAreaVariable::OnRemove() 
{
  area_variable *newvariables;

  if(m_variablenum<0 || !the_area.header.variablecnt) return;
  
  newvariables=new area_variable[--the_area.header.variablecnt];
  if(!newvariables)
  {
    the_area.header.variablecnt++;
    return;
  }
  memcpy(newvariables,the_area.variableheaders,m_variablenum*sizeof(area_variable) );
  memcpy(newvariables+m_variablenum,the_area.variableheaders+m_variablenum+1,(the_area.header.variablecnt-m_variablenum)*sizeof(area_variable) );
  if(the_area.variableheaders)
  {
    delete [] the_area.variableheaders;
  }
  the_area.variableheaders=newvariables;
  the_area.variablecount=the_area.header.variablecnt;
	if(m_variablenum>=the_area.variablecount) m_variablenum--;
  RefreshVariable();
	UpdateData(UD_DISPLAY);
}

void CAreaVariable::OnCopy() 
{
  if(m_variablenum>=0)
  {
    memcpy(&variablecopy,the_area.variableheaders+m_variablenum,sizeof(variablecopy) );
  }
}

void CAreaVariable::OnPaste() 
{
  if(m_variablenum>=0)
  {
    memcpy(the_area.variableheaders+m_variablenum,&variablecopy, sizeof(variablecopy) );
  }
  RefreshVariable();
	UpdateData(UD_DISPLAY);		
}

void CAreaVariable::OnUnknown() 
{
	// TODO: Add your control notification handler code here
	
	UpdateData(UD_DISPLAY);			
}

void CAreaVariable::OnAdd2() 
{
  area_mapnote *newnotes;
  CString tmpstr;
  
  if(the_area.header.mapnotecnt>999)
  {
    MessageBox("Cannot add more notes.","Warning",MB_ICONEXCLAMATION|MB_OK);
    return;
  }
  newnotes=new area_mapnote[++the_area.header.mapnotecnt];
  if(!newnotes)
  {
    the_area.header.mapnotecnt--;
    MessageBox("Not enough memory.","Error",MB_ICONSTOP|MB_OK);
    return;
  }
  memcpy(newnotes, the_area.mapnoteheaders, the_area.mapnotecount*sizeof(area_mapnote));
  memset(newnotes+the_area.mapnotecount,0,sizeof(area_mapnote) );
  newnotes[the_area.mapnotecount].unknownc=1; //marks original/custom note ??
  newnotes[the_area.mapnotecount].strref=-1;
  
  if(the_area.mapnoteheaders)
  {
    delete [] the_area.mapnoteheaders;
  }
  the_area.mapnoteheaders=newnotes;
  m_notenum=the_area.mapnotecount;
  the_area.mapnotecount=the_area.header.mapnotecnt;

  RefreshVariable(); //notes are refreshed here too
	UpdateData(UD_DISPLAY);		
}

void CAreaVariable::OnRemove2() 
{
  area_mapnote *newmapnotes;

  if(m_notenum<0 || !the_area.header.mapnotecnt) return;
  
  newmapnotes=new area_mapnote[--the_area.header.mapnotecnt];
  if(!newmapnotes)
  {
    the_area.header.mapnotecnt++;
    return;
  }
  memcpy(newmapnotes,the_area.mapnoteheaders,m_notenum*sizeof(area_mapnote) );
  memcpy(newmapnotes+m_notenum,the_area.mapnoteheaders+m_notenum+1,(the_area.header.mapnotecnt-m_notenum)*sizeof(area_mapnote) );
  if(the_area.mapnoteheaders)
  {
    delete [] the_area.mapnoteheaders;
  }
  the_area.mapnoteheaders=newmapnotes;
  the_area.mapnotecount=the_area.header.mapnotecnt;
	if(m_notenum>=the_area.mapnotecount) m_notenum--;
  RefreshVariable();
	UpdateData(UD_DISPLAY);
}

void CAreaVariable::OnCopy2() 
{
  if(m_notenum>=0)
  {
    memcpy(&notecopy,the_area.mapnoteheaders+m_notenum,sizeof(notecopy) );
  }
}

void CAreaVariable::OnPaste2() 
{
  if(m_notenum>=0)
  {
    memcpy(the_area.mapnoteheaders+m_notenum,&notecopy, sizeof(notecopy) );
  }
  RefreshVariable();
	UpdateData(UD_DISPLAY);			
}

void CAreaVariable::OnUnknown2() 
{
	UpdateData(UD_DISPLAY);			
}

void CAreaVariable::OnSet() 
{
  CImageView dlg;
  CPoint point;

  if(SetupSelectPoint()) return;
  dlg.InitView(IW_ENABLEBUTTON|IW_SETVERTEX|IW_PLACEIMAGE);
  dlg.SetupAnimationPlacement(&the_bam,  //the cursors are loaded in 'the_bam' now
    the_area.mapnoteheaders[m_notenum].px,
    the_area.mapnoteheaders[m_notenum].py);
  if(dlg.DoModal()==IDOK)
  {
    point=dlg.GetPoint(0);
    the_area.mapnoteheaders[m_notenum].px=(short) point.x;
    the_area.mapnoteheaders[m_notenum].py=(short) point.y;
  }
  RefreshVariable();
  UpdateData(UD_DISPLAY);
}

/////////////////////////////////////////////////////////////////////////////
// CAreaDoor dialog

CAreaDoor::CAreaDoor()	: CPropertyPage(CAreaDoor::IDD)
{
	//{{AFX_DATA_INIT(CAreaDoor)
	m_openclose = TRUE;
	m_vx = 0;
	m_vy = 0;
	//}}AFX_DATA_INIT
  m_doornum=-1;
  memset(&doorcopy,0,sizeof(doorcopy));
  hbd=0;
}

CAreaDoor::~CAreaDoor()
{
  if(hbd) DeleteObject(hbd);
}

void CAreaDoor::RefreshDoor()
{
  CString tmpstr;
  POSITION pos;
  area_vertex *poi;
  int i;
  int count;
  int saveidx;

  if(IsWindow(m_doorpicker) )
  {
    m_doornum=m_doorpicker.GetCurSel();
    if(m_doornum<0) m_doornum=0;
    m_spin_control.SetRange(0,the_bam.GetFrameCount() );
    m_doorpicker.ResetContent();
    for(i=0;i<the_area.doorcount;i++)
    {
      tmpstr.Format("%d. %-.32s",i+1,the_area.doorheaders[i].doorname);
      m_doorpicker.AddString(tmpstr);
    }
    if(m_doornum>=i) m_doornum=i-1;
    m_doornum=m_doorpicker.SetCurSel(m_doornum);
  }
  else m_doornum=-1;

  if(m_doornum>=0)
  {
    m_infostr=resolve_tlk_text(the_area.doorheaders[m_doornum].strref);
    m_text=resolve_tlk_text(the_area.doorheaders[m_doornum].nameref);
    if(m_openclose) //open
    {
      count=the_area.doorheaders[m_doornum].countvertexopen;
      pos=the_area.vertexheaderlist.FindIndex(the_area.triggercount+the_area.containercount+m_doornum*4);
    }
    else
    {
      count=the_area.doorheaders[m_doornum].countvertexclose;
      pos=the_area.vertexheaderlist.FindIndex(the_area.triggercount+the_area.containercount+m_doornum*4+1);
    }
    if(IsWindow(m_vertexpicker) && pos)
    {
      poi=(area_vertex *) the_area.vertexheaderlist.GetAt(pos);
      saveidx=m_vertexpicker.GetCurSel();
      if(saveidx<0) saveidx=0;
      m_vertexpicker.ResetContent();
      for(i=0;i<count;i++)
      {
        tmpstr.Format("%d. [%d.%d]",i+1,poi[i].point.x,poi[i].point.y);
        m_vertexpicker.AddString(tmpstr);
      }
      if(saveidx>=i) saveidx=i-1;
      m_vertexpicker.SetCurSel(saveidx);      
    }
    if(m_openclose) //open
    {
      count=the_area.doorheaders[m_doornum].countblockopen;
      pos=the_area.vertexheaderlist.FindIndex(the_area.triggercount+the_area.containercount+m_doornum*4+2);
    }
    else
    {
      count=the_area.doorheaders[m_doornum].countblockclose;
      pos=the_area.vertexheaderlist.FindIndex(the_area.triggercount+the_area.containercount+m_doornum*4+3);
    }
    if(IsWindow(m_blockpicker) && pos)
    {
      poi=(area_vertex *) the_area.vertexheaderlist.GetAt(pos);
      saveidx=m_blockpicker.GetCurSel();
      if(saveidx<0) saveidx=0;
      m_blockpicker.ResetContent();
      for(i=0;i<count;i++)
      {
        tmpstr.Format("%d. [%d.%d]",i+1,poi[i].point.x,poi[i].point.y);
        m_blockpicker.AddString(tmpstr);
      }
      if(saveidx>=i) saveidx=i-1;
      m_blockpicker.SetCurSel(saveidx);      
    }
  }
}

static int doorboxids[]={IDC_DOORPICKER, IDC_DOORID, IDC_FLAGS,
IDC_FLAG1,IDC_FLAG2,IDC_FLAG3,IDC_FLAG4,IDC_FLAG5,IDC_FLAG6,IDC_FLAG7,IDC_FLAG8,
IDC_FLAG9,IDC_FLAG10,IDC_FLAG11,IDC_FLAG12,IDC_FLAG13,IDC_FLAG14,IDC_FLAG15,
IDC_VERTEXNUM, IDC_BLOCKNUM,IDC_POS1X,IDC_POS1Y,IDC_POS2X,IDC_POS2Y,
IDC_RECALCBOX,IDC_OPEN,IDC_SOUND1,IDC_SOUND2,IDC_SHORTREF, IDC_LONGNAME,IDC_TAGGED,
IDC_AREA,IDC_CURSORIDX, IDC_CURSOR,IDC_TPOSX, IDC_TPOSY,IDC_DETECT,IDC_REMOVAL,
IDC_U54,IDC_TRAPPED,IDC_DETECTED,IDC_KEY,IDC_SCRIPT,IDC_DIALOG,IDC_LOCKED,IDC_DIFF,
IDC_STRREF,IDC_INFOSTR, IDC_BROWSE,IDC_BROWSE2,IDC_BROWSE3, IDC_BROWSE4,IDC_BROWSE5,
IDC_VX,IDC_VY, IDC_MODVERTEX,IDC_PLAY,IDC_PLAYSOUND,IDC_ADDVERTEX,IDC_REMOVEVERTEX,
IDC_ADD3, IDC_REMOVE3,IDC_UNKNOWN, IDC_REMOVE, IDC_COPY, IDC_PASTE,
0};

#define DOORFLAGNUM  15

#pragma warning(disable:4706)   
void CAreaDoor::DoDataExchange(CDataExchange* pDX)
{
  CWnd *cb;
  CButton *cb2;
  CString tmpstr;
  int flg, flg2, flg3;
  int id;
  int i,j;
  int fc;

	CPropertyPage::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_CURSOR, m_cursoricon);
	//{{AFX_DATA_MAP(CAreaDoor)
	DDX_Control(pDX, IDC_SPINCURSOR, m_spin_control);
	DDX_Control(pDX, IDC_BLOCKS, m_blockpicker);
	DDX_Control(pDX, IDC_VERTICES, m_vertexpicker);
	DDX_Control(pDX, IDC_DOORPICKER, m_doorpicker);
	DDX_Check(pDX, IDC_OPEN, m_openclose);
	DDX_Text(pDX, IDC_VX, m_vx);
	DDX_Text(pDX, IDC_VY, m_vy);
	//}}AFX_DATA_MAP
  flg=m_doornum>=0;
  if(flg)
  {
    if(m_openclose) flg3=!!the_area.doorheaders[m_doornum].countvertexopen;
    else flg3=!!the_area.doorheaders[m_doornum].countvertexclose;
  }
  else flg3=false;
  if(flg3)
  {
    m_vertexpicker.EnableWindow(true);
  }
  else
  {
    m_vertexpicker.EnableWindow(false);
  }
  if(flg)
  {
    if(m_openclose) flg2=!!the_area.doorheaders[m_doornum].countblockopen;
    else flg2=!!the_area.doorheaders[m_doornum].countblockclose;
  }
  else flg2=false;
  if(flg2)
  {
    m_blockpicker.EnableWindow(true);
  }
  else
  {
    m_blockpicker.EnableWindow(false);
  }
  i=0;
  while(id=doorboxids[i])
  {
    cb=GetDlgItem(id);
    if(member_array(id,vertexids)!=-1) cb->EnableWindow(flg3);
    else cb->EnableWindow(flg);
    i++;
  }

  cb=GetDlgItem(IDC_MAXDOOR);
  tmpstr.Format("/ %d",the_area.doorcount);
  cb->SetWindowText(tmpstr);

  if(flg)
  {
    tmpstr.Format("%d. %-.32s",m_doornum+1,the_area.doorheaders[m_doornum].doorname);
    DDX_Text(pDX, IDC_DOORPICKER, tmpstr);

    RetrieveResref(tmpstr, the_area.doorheaders[m_doornum].doorid);
    DDX_Text(pDX, IDC_DOORID, tmpstr);
    DDV_MaxChars(pDX, tmpstr, 8);
    StoreResref(tmpstr, the_area.doorheaders[m_doornum].doorid);

    DDX_Text(pDX, IDC_TPOSX, the_area.doorheaders[m_doornum].launchx);
    DDX_Text(pDX, IDC_TPOSY, the_area.doorheaders[m_doornum].launchy);
    DDX_Text(pDX, IDC_DETECT, the_area.doorheaders[m_doornum].trapdetect);
    DDX_Text(pDX, IDC_REMOVAL, the_area.doorheaders[m_doornum].trapremoval);
    DDX_Text(pDX, IDC_TRAPPED, the_area.doorheaders[m_doornum].trapflags);
    DDX_Text(pDX, IDC_DETECTED, the_area.doorheaders[m_doornum].trapdetflags);
    DDX_Text(pDX, IDC_LOCKED, the_area.doorheaders[m_doornum].locked);
    DDX_Text(pDX, IDC_DIFF, the_area.doorheaders[m_doornum].lockremoval);
    DDX_Text(pDX, IDC_U54, the_area.doorheaders[m_doornum].unknown54);
    DDX_Text(pDX, IDC_SHORTREF, the_area.doorheaders[m_doornum].nameref);
    DDX_Text(pDX, IDC_LONGNAME, m_text);

    RetrieveResref(tmpstr, the_area.doorheaders[m_doornum].key);
    DDX_Text(pDX, IDC_KEY, tmpstr);
    DDV_MaxChars(pDX, tmpstr, 8);
    StoreResref(tmpstr, the_area.doorheaders[m_doornum].key);

    RetrieveResref(tmpstr, the_area.doorheaders[m_doornum].regionlink); //travel region link
    DDX_Text(pDX, IDC_AREA, tmpstr);
    DDV_MaxChars(pDX, tmpstr, 8);
    StoreResref(tmpstr, the_area.doorheaders[m_doornum].regionlink);

    RetrieveResref(tmpstr, the_area.doorheaders[m_doornum].openres);
    DDX_Text(pDX, IDC_SOUND1, tmpstr);
    DDV_MaxChars(pDX, tmpstr, 8);
    StoreResref(tmpstr, the_area.doorheaders[m_doornum].openres);
    RetrieveResref(tmpstr, the_area.doorheaders[m_doornum].closeres);
    DDX_Text(pDX, IDC_SOUND2, tmpstr);
    DDV_MaxChars(pDX, tmpstr, 8);
    StoreResref(tmpstr, the_area.doorheaders[m_doornum].closeres);

    DDX_Text(pDX, IDC_CURSORIDX, the_area.doorheaders[m_doornum].cursortype);
    fc=the_bam.GetFrameIndex(the_area.doorheaders[m_doornum].cursortype,0);
    the_bam.MakeBitmap(fc,RGB(32,32,32),hbd,BM_RESIZE,32,32);
    m_cursoricon.SetBitmap(hbd);
    
    DDX_Text(pDX, IDC_STRREF, the_area.doorheaders[m_doornum].strref);
    DDX_Text(pDX, IDC_INFOSTR, m_infostr);

    DDX_Text(pDX, IDC_FLAGS, the_area.doorheaders[m_doornum].flags);
    j=1;
    for(i=0;i<DOORFLAGNUM;i++)
    {
      cb2=(CButton *) GetDlgItem(IDC_FLAG1+i);
      if(the_area.doorheaders[m_doornum].flags&j) cb2->SetCheck(true);
      else cb2->SetCheck(false);
      j<<=1;
    }
    if(m_openclose) //open
    {
      DDX_Text(pDX, IDC_POS1X, the_area.doorheaders[m_doornum].op1x);
      DDX_Text(pDX, IDC_POS1Y, the_area.doorheaders[m_doornum].op1y);
      DDX_Text(pDX, IDC_POS2X, the_area.doorheaders[m_doornum].op2x);
      DDX_Text(pDX, IDC_POS2Y, the_area.doorheaders[m_doornum].op2y);
      tmpstr.Format("/ %d",the_area.doorheaders[m_doornum].countblockopen);
      DDX_Text(pDX, IDC_BLOCKNUM,tmpstr);
      tmpstr.Format("/ %d",the_area.doorheaders[m_doornum].countvertexopen);
      DDX_Text(pDX, IDC_VERTEXNUM,tmpstr);
      tmpstr="Open";
    }
    else //open
    {
      DDX_Text(pDX, IDC_POS1X, the_area.doorheaders[m_doornum].cp1x);
      DDX_Text(pDX, IDC_POS1Y, the_area.doorheaders[m_doornum].cp1y);
      DDX_Text(pDX, IDC_POS2X, the_area.doorheaders[m_doornum].cp2x);
      DDX_Text(pDX, IDC_POS2Y, the_area.doorheaders[m_doornum].cp2y);
      tmpstr.Format("/ %d",the_area.doorheaders[m_doornum].countblockclose);
      DDX_Text(pDX, IDC_BLOCKNUM,tmpstr);
      tmpstr.Format("/ %d",the_area.doorheaders[m_doornum].countvertexclose);
      DDX_Text(pDX, IDC_VERTEXNUM,tmpstr);
      tmpstr="Close";
    }
    DDX_Text(pDX, IDC_OPEN, tmpstr);
  }
}
#pragma warning(default:4706)   

BOOL CAreaDoor::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
  RefreshDoor();
  UpdateData(UD_DISPLAY);
  return TRUE;
}

BEGIN_MESSAGE_MAP(CAreaDoor, CPropertyPage)
	//{{AFX_MSG_MAP(CAreaDoor)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_COPY, OnCopy)
	ON_BN_CLICKED(IDC_PASTE, OnPaste)
	ON_CBN_KILLFOCUS(IDC_DOORPICKER, OnKillfocusDoorpicker)
	ON_CBN_SELCHANGE(IDC_DOORPICKER, OnSelchangeDoorpicker)
	ON_EN_KILLFOCUS(IDC_DOORID, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_FLAGS, DefaultKillfocus)
	ON_BN_CLICKED(IDC_OPEN, OnOpenClose)
	ON_BN_CLICKED(IDC_RECALCBOX, OnRecalcbox)
	ON_EN_KILLFOCUS(IDC_U54, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_CURSORIDX, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_SOUND1, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_SOUND2, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_POS1X, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_POS1Y, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_POS2X, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_POS2Y, DefaultKillfocus)
	ON_CBN_KILLFOCUS(IDC_BLOCKS, DefaultKillfocus)
	ON_CBN_SELCHANGE(IDC_BLOCKS, DefaultKillfocus)
	ON_CBN_KILLFOCUS(IDC_VERTICES, OnKillfocusVertices)
	ON_CBN_SELCHANGE(IDC_VERTICES, OnSelchangeVertices)
	ON_EN_KILLFOCUS(IDC_TPOSX, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_TPOSY, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_DETECT, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_REMOVAL, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_TRAPPED, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_DETECTED, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_KEY, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_SCRIPT, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_DIALOG, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_LOCKED, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_DIFF, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_STRREF, OnKillfocusStrref)
	ON_EN_KILLFOCUS(IDC_INFOSTR, OnKillfocusInfostr)
	ON_BN_CLICKED(IDC_FLAG1, OnFlag1)
	ON_BN_CLICKED(IDC_FLAG2, OnFlag2)
	ON_BN_CLICKED(IDC_FLAG3, OnFlag3)
	ON_BN_CLICKED(IDC_FLAG4, OnFlag4)
	ON_BN_CLICKED(IDC_FLAG5, OnFlag5)
	ON_BN_CLICKED(IDC_FLAG6, OnFlag6)
	ON_BN_CLICKED(IDC_FLAG7, OnFlag7)
	ON_BN_CLICKED(IDC_FLAG8, OnFlag8)
	ON_EN_KILLFOCUS(IDC_SHORTREF, OnKillfocusShortref)
	ON_EN_KILLFOCUS(IDC_AREA, OnKillfocusArea)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_BROWSE2, OnBrowse2)
	ON_BN_CLICKED(IDC_PLAY, OnPlay)
	ON_BN_CLICKED(IDC_PLAYSOUND, OnPlaysound)
	ON_EN_KILLFOCUS(IDC_LONGNAME, OnKillfocusLongname)
	ON_BN_CLICKED(IDC_BROWSE3, OnBrowse3)
	ON_BN_CLICKED(IDC_BROWSE4, OnBrowse4)
	ON_BN_CLICKED(IDC_ADD3, OnAdd3)
	ON_BN_CLICKED(IDC_REMOVE3, OnRemove3)
	ON_BN_CLICKED(IDC_UNKNOWN, OnUnknown)
	ON_BN_CLICKED(IDC_BROWSE5, OnBrowse5)
	ON_EN_KILLFOCUS(IDC_VX, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_VY, DefaultKillfocus)
	ON_BN_CLICKED(IDC_MODVERTEX, OnModvertex)
	ON_BN_CLICKED(IDC_ADDVERTEX, OnAddvertex)
	ON_BN_CLICKED(IDC_REMOVEVERTEX, OnRemovevertex)
	ON_BN_CLICKED(IDC_FLAG9, OnFlag9)
	ON_BN_CLICKED(IDC_FLAG10, OnFlag10)
	ON_BN_CLICKED(IDC_FLAG11, OnFlag11)
	ON_BN_CLICKED(IDC_FLAG12, OnFlag12)
	ON_BN_CLICKED(IDC_FLAG13, OnFlag13)
	ON_BN_CLICKED(IDC_FLAG14, OnFlag14)
	ON_BN_CLICKED(IDC_FLAG15, OnFlag15)
	ON_BN_CLICKED(IDC_TAGGED, OnTagged)
	ON_EN_CHANGE(IDC_CURSORIDX, OnChangeCursoridx)
	ON_BN_CLICKED(IDC_SET, OnSet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAreaDoor message handlers

void CAreaDoor::OnKillfocusDoorpicker() 
{
  CString tmpstr;
  int x;

  m_doorpicker.GetWindowText(tmpstr);
  x=strtonum(tmpstr)-1;
  if(x>=0 && x<=the_area.doorcount)
  {
    m_doornum=m_doorpicker.SetCurSel(x);
  }
  else
  {
    if(m_doornum>=0 && m_doornum<=the_area.doorcount)
    {
      StoreName(tmpstr, the_area.doorheaders[m_doornum].doorname);
    }
  }
  RefreshDoor();
	UpdateData(UD_DISPLAY);	
}

void CAreaDoor::OnSelchangeDoorpicker() 
{
  int x;

  x=m_doorpicker.GetCurSel();
  if(x>=0 && x<=the_area.doorcount) m_doornum=x;
  RefreshDoor();
	UpdateData(UD_DISPLAY);	
}

void CAreaDoor::DefaultKillfocus() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CAreaDoor::OnChangeCursoridx() 
{
  if(m_cursoricon.m_hWnd)
  {
  	UpdateData(UD_RETRIEVE);
	  UpdateData(UD_DISPLAY);
  }
}

void CAreaDoor::OnKillfocusVertices() 
{
  POSITION pos;
  area_vertex *poi;
  int vertexnum;
  int vertexcount;

	UpdateData(UD_RETRIEVE);
  vertexcount=the_area.triggercount+the_area.containercount+m_doornum*4+!m_openclose;
  pos=the_area.vertexheaderlist.FindIndex(vertexcount);
  poi=(area_vertex *) the_area.vertexheaderlist.GetAt(pos);
  if(!poi) return;
  vertexnum=m_vertexpicker.GetCurSel();
  if(vertexnum<0)
  {
    m_vx=-1;
    m_vy=-1;
  }
  else
  {
    m_vx=poi[vertexnum].point.x;
    m_vy=poi[vertexnum].point.y;
  }
	UpdateData(UD_DISPLAY);
}

void CAreaDoor::OnSelchangeVertices() 
{
  POSITION pos;
  area_vertex *poi;
  int vertexnum;
  int vertexcount;

  vertexcount=the_area.triggercount+the_area.containercount+m_doornum*4+!m_openclose;
  pos=the_area.vertexheaderlist.FindIndex(vertexcount);
  poi=(area_vertex *) the_area.vertexheaderlist.GetAt(pos);
  if(!poi) return;
  vertexnum=m_vertexpicker.GetCurSel();
  if(vertexnum<0)
  {
    m_vx=-1;
    m_vy=-1;
  }
  else
  {
    m_vx=poi[vertexnum].point.x;
    m_vy=poi[vertexnum].point.y;
  }
	UpdateData(UD_DISPLAY);
}

void CAreaDoor::OnKillfocusShortref() 
{
	UpdateData(UD_RETRIEVE);
  RefreshDoor();
	UpdateData(UD_DISPLAY);
}

void CAreaDoor::OnTagged() 
{
  if(editflg&TLKCHANGE)
  {
    if(MessageBox("Do you want to update dialog.tlk?","Area editor",MB_YESNO)!=IDYES)
    {
      return;
    }
  }
	toggle_tlk_tag(the_area.doorheaders[m_doornum].nameref);
	UpdateData(UD_DISPLAY);
}

void CAreaDoor::OnKillfocusLongname() 
{
  CString old;

	UpdateData(UD_RETRIEVE);
  old=resolve_tlk_text(the_area.doorheaders[m_doornum].nameref);
  if(old!=m_text)
  {
    if(editflg&TLKCHANGE)
    {
      if(MessageBox("Do you want to update dialog.tlk?","Area editor",MB_YESNO)!=IDYES)
      {
        return;
      }
    }
    the_area.doorheaders[m_doornum].nameref=store_tlk_text(the_area.doorheaders[m_doornum].nameref, m_text);
  }
  RefreshDoor();
	UpdateData(UD_DISPLAY);
}

void CAreaDoor::OnKillfocusArea() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CAreaDoor::OnKillfocusStrref() 
{
	UpdateData(UD_RETRIEVE);
  RefreshDoor();
	UpdateData(UD_DISPLAY);
}

void CAreaDoor::OnKillfocusInfostr() 
{
  CString old;

	UpdateData(UD_RETRIEVE);
  old=resolve_tlk_text(the_area.doorheaders[m_doornum].strref);
  if(old!=m_infostr)
  {
    if(editflg&TLKCHANGE)
    {
      if(MessageBox("Do you want to update dialog.tlk?","Area editor",MB_YESNO)!=IDYES)
      {
        return;
      }
    }
    the_area.doorheaders[m_doornum].strref=store_tlk_text(the_area.doorheaders[m_doornum].strref, m_infostr);
  }
  RefreshDoor();
	UpdateData(UD_DISPLAY);
}

void CAreaDoor::OnFlag1() 
{
	the_area.doorheaders[m_doornum].flags^=1;
	UpdateData(UD_DISPLAY);
}

void CAreaDoor::OnFlag2() 
{
	the_area.doorheaders[m_doornum].flags^=2;
	UpdateData(UD_DISPLAY);
}

void CAreaDoor::OnFlag3() 
{
	the_area.doorheaders[m_doornum].flags^=4;
	UpdateData(UD_DISPLAY);
}

void CAreaDoor::OnFlag4() 
{
	the_area.doorheaders[m_doornum].flags^=8;
	UpdateData(UD_DISPLAY);
}

void CAreaDoor::OnFlag5() 
{
	the_area.doorheaders[m_doornum].flags^=16;
	UpdateData(UD_DISPLAY);
}

void CAreaDoor::OnFlag6() 
{
	the_area.doorheaders[m_doornum].flags^=32;
	UpdateData(UD_DISPLAY);
}

void CAreaDoor::OnFlag7() 
{
	the_area.doorheaders[m_doornum].flags^=64;
	UpdateData(UD_DISPLAY);
}

void CAreaDoor::OnFlag8() 
{
	the_area.doorheaders[m_doornum].flags^=128;
	UpdateData(UD_DISPLAY);
}

void CAreaDoor::OnFlag9() 
{
	the_area.doorheaders[m_doornum].flags^=256;
	UpdateData(UD_DISPLAY);
}

void CAreaDoor::OnFlag10() 
{
	the_area.doorheaders[m_doornum].flags^=512;
	UpdateData(UD_DISPLAY);
}

void CAreaDoor::OnFlag11() 
{
	the_area.doorheaders[m_doornum].flags^=0x400;
	UpdateData(UD_DISPLAY);
}

void CAreaDoor::OnFlag12() 
{
	the_area.doorheaders[m_doornum].flags^=0x800;
	UpdateData(UD_DISPLAY);
}

void CAreaDoor::OnFlag13() 
{
	the_area.doorheaders[m_doornum].flags^=0x1000;
	UpdateData(UD_DISPLAY);
}

void CAreaDoor::OnFlag14() 
{
	the_area.doorheaders[m_doornum].flags^=0x2000;
	UpdateData(UD_DISPLAY);
}

void CAreaDoor::OnFlag15() 
{
	the_area.doorheaders[m_doornum].flags^=0x4000;
	UpdateData(UD_DISPLAY);
}

void CAreaDoor::OnOpenClose() 
{
  m_openclose=!m_openclose;
	RefreshDoor();
	UpdateData(UD_DISPLAY);
}

void CAreaDoor::OnModvertex() 
{
  POSITION pos;
  area_vertex *poi;
  int vertexnum;
  int vertexcount;

	UpdateData(UD_RETRIEVE);
  vertexcount=the_area.triggercount+the_area.containercount+m_doornum*4+!m_openclose;
  pos=the_area.vertexheaderlist.FindIndex(vertexcount);
  poi=(area_vertex *) the_area.vertexheaderlist.GetAt(pos);
  if(!poi) return;
  vertexnum=m_vertexpicker.GetCurSel();
  if(vertexnum<0) return;
  poi[vertexnum].point.x=(short) m_vx;
  poi[vertexnum].point.y=(short) m_vy;
  RefreshDoor();
	OnRecalcbox();
}

void CAreaDoor::OnRecalcbox() 
{
  POINTS min,max;
  int vertexcount;

  vertexcount=the_area.triggercount+the_area.containercount+m_doornum*4;
  RecalcBox(the_area.doorheaders[m_doornum].countvertexopen,vertexcount,min,max);
  the_area.doorheaders[m_doornum].op1x=min.x;
  the_area.doorheaders[m_doornum].op1y=min.y;
  the_area.doorheaders[m_doornum].op2x=max.x;
  the_area.doorheaders[m_doornum].op2y=max.y;
  RecalcBox(the_area.doorheaders[m_doornum].countvertexclose,vertexcount+1,min,max);
  the_area.doorheaders[m_doornum].cp1x=min.x;
  the_area.doorheaders[m_doornum].cp1y=min.y;
  the_area.doorheaders[m_doornum].cp2x=max.x;
  the_area.doorheaders[m_doornum].cp2y=max.y;
	UpdateData(UD_DISPLAY);
}

int CAreaDoor::AddWedDoor(char *doorid)
{
  int i;
  int newcount;
  wed_door *newdoors;

  for(i=0;i<the_area.wedheader.doorcnt;i++)
  {
    if(!strnicmp(doorid, the_area.weddoorheaders[i].doorid,8) )
    {
      MessageBox("This door already exists in the wedfile.","Area editor",MB_ICONINFORMATION|MB_OK);
      return 1;
    }
  }
  newcount=the_area.wedheader.doorcnt+1;
  newdoors=new wed_door[newcount];
  if(!newdoors) return -3;
  memcpy(newdoors,the_area.weddoorheaders,the_area.weddoorcount*sizeof(wed_door) );
  memset(newdoors+the_area.weddoorcount,0,sizeof(wed_door) );
  memcpy(newdoors[the_area.weddoorcount].doorid,doorid,8);
  the_area.wedchanged=true;
  if(the_area.weddoorheaders)
  {
    delete [] the_area.weddoorheaders;
  }
  the_area.weddoorheaders=newdoors;
  the_area.weddoorcount=the_area.wedheader.doorcnt=newcount;
  return 0;
}

void CAreaDoor::OnAdd() 
{
  area_door *newdoors;
  area_vertex *newvertex;
  CString tmpstr;

  if(!the_area.WedAvailable())
  {
    MessageBox("Cannot add door, because the wed file isn't available.","Warning",MB_ICONEXCLAMATION|MB_OK);
    return;
  }
  if(the_area.header.doorcnt>999)
  {
    MessageBox("Cannot add more doors.","Warning",MB_ICONEXCLAMATION|MB_OK);
    return;
  }
  newdoors=new area_door[++the_area.header.doorcnt];
  if(!newdoors)
  {
    the_area.header.doorcnt--;
    MessageBox("Not enough memory.","Error",MB_ICONSTOP|MB_OK);
    return;
  }
  memcpy(newdoors, the_area.doorheaders, the_area.doorcount*sizeof(area_door));
  memset(newdoors+the_area.doorcount,0,sizeof(area_door) );
  tmpstr.Format("Door %02d",the_area.header.doorcnt);
  StoreName(tmpstr,newdoors[the_area.doorcount].doorname);
  StoreResref(tmpstr,newdoors[the_area.doorcount].doorid);
  newdoors[the_area.doorcount].firstvertexopen=the_area.vertexcount;
  newdoors[the_area.doorcount].firstvertexclose=the_area.vertexcount;
  newdoors[the_area.doorcount].firstblockopen=the_area.vertexcount;
  newdoors[the_area.doorcount].firstblockclose=the_area.vertexcount;
  
  if(the_area.doorheaders)
  {
    delete [] the_area.doorheaders;
  }
  the_area.doorheaders=newdoors;
  m_doornum=the_area.doorcount; //last element
  the_area.doorcount=the_area.header.doorcnt;
  //add four entries
  newvertex=new area_vertex[0];
  the_area.vertexheaderlist.AddTail(newvertex);
  newvertex=new area_vertex[0];
  the_area.vertexheaderlist.AddTail(newvertex);
  newvertex=new area_vertex[0];
  the_area.vertexheaderlist.AddTail(newvertex);
  newvertex=new area_vertex[0];
  the_area.vertexheaderlist.AddTail(newvertex);

  RefreshDoor();
	OnRecalcbox();
}

int CAreaDoor::RemoveWedDoor(char *doorid)
{
  int i,j;
  int newcount;
  wed_door *newdoors;

  newcount=0;
  for(i=0;i<the_area.wedheader.doorcnt;i++)
  {
    //the door isn't about to remove
    if(strnicmp(doorid, the_area.weddoorheaders[i].doorid,8) )
    {
      newcount++;
    }
  }
  if(newcount==the_area.wedheader.doorcnt) return 1; //wed didn't need any change
  newdoors=new wed_door[newcount];
  if(!newdoors) return -3;
  j=0;
  for(i=0;i<the_area.wedheader.doorcnt;i++)
  {
    //we keep this door
    if(strnicmp(doorid, the_area.weddoorheaders[i].doorid,8) )
    {
      newdoors[j++]=the_area.weddoorheaders[i];
    }
  }
  the_area.wedchanged=true;
  if(the_area.weddoorheaders)
  {
    delete [] the_area.weddoorheaders;
  }
  the_area.weddoorheaders=newdoors;
  the_area.weddoorcount=the_area.wedheader.doorcnt=newcount;
  the_area.ConsolidateDoortiles();
  //need to remove polygons from wed too
  the_area.ConsolidateVertices();
  return 0;
}

void CAreaDoor::OnRemove() 
{
  POSITION pos;
  area_door *newdoors;
  int vertexnum;
  int i;

  if(m_doornum<0 || !the_area.header.doorcnt) return;

  if(!the_area.WedAvailable())
  {
    MessageBox("Cannot completely remove wed door, because the wed file isn't available.","Warning",MB_ICONEXCLAMATION|MB_OK);
  }
  else
  {
    switch(RemoveWedDoor(the_area.doorheaders[m_doornum].doorid) )
    {
    case 1:
      MessageBox("There was no such door referenced in the .wed file.","Warning",MB_ICONEXCLAMATION|MB_OK);
      break;
    case 0:
      break;
    default:
      MessageBox("Error while removing .wed door.","Error",MB_OK|MB_ICONSTOP);
      return;
    }
  }
  newdoors=new area_door[--the_area.header.doorcnt];
  if(!newdoors)
  {
    the_area.header.doorcnt++;
    return;
  }
  memcpy(newdoors,the_area.doorheaders,m_doornum*sizeof(area_door) );
  memcpy(newdoors+m_doornum,the_area.doorheaders+m_doornum+1,(the_area.header.doorcnt-m_doornum)*sizeof(area_door) );
  //remove all 4 vertexheaders of the door starting at vertexnum
  //2 of these are actually impeded block lists
  vertexnum=the_area.triggercount+the_area.containercount+m_doornum*4;
  for(i=0;i<4;i++)
  {
    pos=the_area.vertexheaderlist.FindIndex(vertexnum);
    the_area.vertexheaderlist.RemoveAt(pos);
  }
  
  if(the_area.doorheaders)
  {
    delete [] the_area.doorheaders;
  }
  the_area.doorheaders=newdoors;
  the_area.doorcount=the_area.header.doorcnt;
	if(m_doornum>=the_area.doorcount) m_doornum--;
  RefreshDoor();
	UpdateData(UD_DISPLAY);
}

void CAreaDoor::OnCopy() 
{
  int vertexnum;
  POSITION pos;
  int i;

  if(m_doornum>=0)
  {
    vertexsize[0]=the_area.doorheaders[m_doornum].countvertexopen;
    vertexsize[1]=the_area.doorheaders[m_doornum].countvertexclose;
    vertexsize[2]=the_area.doorheaders[m_doornum].countblockopen;
    vertexsize[3]=the_area.doorheaders[m_doornum].countblockclose;
    memcpy(&doorcopy,the_area.doorheaders+m_doornum,sizeof(doorcopy) );
    vertexnum=the_area.triggercount+the_area.containercount+m_doornum*4;
    for(i=0;i<4;i++)
    {
      pos=the_area.vertexheaderlist.FindIndex(vertexnum+i);
      if(vertexcopy[i])
      {
        delete [] vertexcopy[i];
      }
      vertexcopy[i]=new area_vertex[vertexsize[i]];
      memcpy(vertexcopy[i],the_area.vertexheaderlist.GetAt(pos),vertexsize[i]*sizeof(area_vertex) );
    }
  }
}

void CAreaDoor::OnPaste() 
{
  POSITION pos;
  area_vertex *poi;
  int vertexnum;
  int i;

  if(m_doornum>=0)
  {
    vertexnum=the_area.triggercount+the_area.containercount+m_doornum*4;
    for(i=0;i<4;i++)
    {
      pos=the_area.vertexheaderlist.FindIndex(vertexnum+i);
      poi=new area_vertex[vertexsize[i]];
      if(poi)
      {
        memcpy(poi,vertexcopy[i],vertexsize[i]*sizeof(area_vertex));
      }
      the_area.vertexheaderlist.SetAt(pos,poi); //this will also free the old element
    }
    memcpy(the_area.doorheaders+m_doornum,&doorcopy, sizeof(doorcopy) );
  }
  RefreshDoor();
	UpdateData(UD_DISPLAY);
}

void CAreaDoor::OnBrowse() 
{
  if(m_doornum<0) return;
  pickerdlg.m_restype=REF_WAV;
  RetrieveResref(pickerdlg.m_picked,the_area.doorheaders[m_doornum].openres);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_area.doorheaders[m_doornum].openres);
  }
	UpdateData(UD_DISPLAY);	
}

void CAreaDoor::OnBrowse2() 
{
  if(m_doornum<0) return;
  pickerdlg.m_restype=REF_WAV;
  RetrieveResref(pickerdlg.m_picked,the_area.doorheaders[m_doornum].closeres);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_area.doorheaders[m_doornum].closeres);
  }
	UpdateData(UD_DISPLAY);	
}

void CAreaDoor::OnBrowse3() 
{
  if(m_doornum<0) return;
  pickerdlg.m_restype=REF_ITM;
  RetrieveResref(pickerdlg.m_picked,the_area.doorheaders[m_doornum].key);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_area.doorheaders[m_doornum].key);
  }
	UpdateData(UD_DISPLAY);	
}

void CAreaDoor::OnBrowse4() 
{
  if(m_doornum<0) return;
  pickerdlg.m_restype=REF_DLG;
  RetrieveResref(pickerdlg.m_picked,the_area.doorheaders[m_doornum].dlgref);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_area.doorheaders[m_doornum].dlgref);
  }
	UpdateData(UD_DISPLAY);	
}


void CAreaDoor::OnBrowse5() 
{
  if(m_doornum<0) return;
  pickerdlg.m_restype=REF_BCS;
  RetrieveResref(pickerdlg.m_picked,the_area.doorheaders[m_doornum].openscript);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_area.doorheaders[m_doornum].openscript);
  }
	UpdateData(UD_DISPLAY);	
}

void CAreaDoor::OnPlay() 
{
  CString tmpstr;

  UpdateData(UD_RETRIEVE);
  RetrieveResref(tmpstr, the_area.doorheaders[m_doornum].openres);
  play_acm(tmpstr,false, false);
}

void CAreaDoor::OnPlaysound() 
{
  CString tmpstr;

  UpdateData(UD_RETRIEVE);
  RetrieveResref(tmpstr, the_area.doorheaders[m_doornum].closeres);
  play_acm(tmpstr,false, false);
}

void CAreaDoor::OnAddvertex() //add vertex
{
  POSITION pos;
  area_vertex *poi;
  area_vertex *poi2;
  int vertexsize;
  int vertexnum;
  int openclose;

	UpdateData(UD_RETRIEVE);
  vertexnum=m_vertexpicker.GetCurSel();
  if(m_doornum<0)
  {
    return;
  }
  if(m_openclose)    
  {
    openclose=0;
    vertexsize=the_area.doorheaders[m_doornum].countvertexopen;
  }
  else
  {
    openclose=1;
    vertexsize=the_area.doorheaders[m_doornum].countvertexclose;
  }
  pos=the_area.vertexheaderlist.FindIndex(the_area.triggercount+the_area.containercount+m_doornum*4+openclose);
  poi=(area_vertex *) the_area.vertexheaderlist.GetAt(pos);
  poi2=new area_vertex[vertexsize+1];
  if(!poi2)
  {
    return;
  }
  vertexnum++;
  memcpy(poi2,poi,vertexnum*sizeof(area_vertex) );
  if(vertexnum>0)
  {
    memcpy(poi2+vertexnum+1,poi+vertexnum,(vertexsize-vertexnum)*sizeof(area_vertex) );
  }
  poi2[vertexnum].point.x=(short) m_vx;
  poi2[vertexnum].point.y=(short) m_vy;
  the_area.vertexheaderlist.SetAt(pos,poi2); //this will free memory at *poi as well
  if(openclose) the_area.doorheaders[m_doornum].countvertexclose=(short) (vertexsize+1);
  else the_area.doorheaders[m_doornum].countvertexopen=(short) (vertexsize+1);
  RefreshDoor();
	OnRecalcbox();
}

void CAreaDoor::OnRemovevertex() 
{
  POSITION pos;
  area_vertex *poi;
  area_vertex *poi2;
  int vertexsize;
  int vertexnum;
  int openclose;

	UpdateData(UD_RETRIEVE);
  vertexnum=m_vertexpicker.GetCurSel();
  if(m_doornum<0 || vertexnum<0)
  {
    return;
  }
  if(m_openclose)    
  {
    openclose=0;
    vertexsize=the_area.doorheaders[m_doornum].countvertexopen;
  }
  else
  {
    openclose=1;
    vertexsize=the_area.doorheaders[m_doornum].countvertexclose;
  }
  pos=the_area.vertexheaderlist.FindIndex(the_area.triggercount+the_area.containercount+m_doornum*4+openclose);
  poi=(area_vertex *) the_area.vertexheaderlist.GetAt(pos);
  poi2=new area_vertex[--vertexsize];
  if(!poi2)
  {
    return;
  }
  memcpy(poi2,poi,vertexnum*sizeof(area_vertex) );
  memcpy(poi2+vertexnum,poi+vertexnum+1,(vertexsize-vertexnum)*sizeof(area_vertex) );
  the_area.vertexheaderlist.SetAt(pos,poi2); //this will free memory at *poi as well
  if(openclose) the_area.doorheaders[m_doornum].countvertexclose=(short) (vertexsize);
  else the_area.doorheaders[m_doornum].countvertexopen=(short) (vertexsize);
  RefreshDoor();
	OnRecalcbox();
}

void CAreaDoor::OnAdd3() //add impeded block
{
  POSITION pos;
  area_vertex *poi;
  area_vertex *poi2;
  int vertexsize;
  int vertexnum;
  int openclose;

	UpdateData(UD_RETRIEVE);
  vertexnum=m_blockpicker.GetCurSel();
  if(m_doornum<0)
  {
    return;
  }
  if(m_openclose)    
  {
    openclose=2;
    vertexsize=the_area.doorheaders[m_doornum].countblockopen;
  }
  else
  {
    openclose=3;
    vertexsize=the_area.doorheaders[m_doornum].countblockclose;
  }
  pos=the_area.vertexheaderlist.FindIndex(the_area.triggercount+the_area.containercount+m_doornum*4+openclose);
  poi=(area_vertex *) the_area.vertexheaderlist.GetAt(pos);
  poi2=new area_vertex[vertexsize+1];
  if(!poi2)
  {
    return;
  }
  memcpy(poi2,poi,vertexnum*sizeof(area_vertex) );
  memcpy(poi2+vertexnum+1,poi+vertexnum,(vertexsize-vertexnum)*sizeof(area_vertex) );
  poi2[vertexnum].point.x=(short) m_vx;
  poi2[vertexnum].point.y=(short) m_vy;
  the_area.vertexheaderlist.SetAt(pos,poi2); //this will free memory at *poi as well
  if(openclose==3) the_area.doorheaders[m_doornum].countblockclose=(short) (vertexsize+1);
  else the_area.doorheaders[m_doornum].countblockopen=(short) (vertexsize+1);
  RefreshDoor();
	UpdateData(UD_DISPLAY);	
}

void CAreaDoor::OnRemove3() 
{
  POSITION pos;
  area_vertex *poi;
  area_vertex *poi2;
  int vertexsize;
  int vertexnum;
  int openclose;

	UpdateData(UD_RETRIEVE);
  vertexnum=m_blockpicker.GetCurSel();
  if(m_doornum<0 || vertexnum<0)
  {
    return;
  }
  if(m_openclose)    
  {
    openclose=2;
    vertexsize=the_area.doorheaders[m_doornum].countvertexopen;
  }
  else
  {
    openclose=3;
    vertexsize=the_area.doorheaders[m_doornum].countvertexclose;
  }
  pos=the_area.vertexheaderlist.FindIndex(the_area.triggercount+the_area.containercount+m_doornum*4+openclose);
  poi=(area_vertex *) the_area.vertexheaderlist.GetAt(pos);
  poi2=new area_vertex[--vertexsize];
  if(!poi2)
  {
    return;
  }
  memcpy(poi2,poi,vertexnum*sizeof(area_vertex) );
  memcpy(poi2+vertexnum,poi+vertexnum+1,(vertexsize-vertexnum)*sizeof(area_vertex) );
  the_area.vertexheaderlist.SetAt(pos,poi2); //this will free memory at *poi as well
  if(openclose==3) the_area.doorheaders[m_doornum].countvertexclose=(short) (vertexsize+1);
  else the_area.doorheaders[m_doornum].countvertexopen=(short) (vertexsize+1);
  RefreshDoor();
	UpdateData(UD_DISPLAY);	
}

void CAreaDoor::OnSet() 
{
  CImageView dlg;
  CPoint point;

  if(SetupSelectPoint()) return;
  dlg.InitView(IW_ENABLEBUTTON|IW_SETVERTEX|IW_PLACEIMAGE);
  dlg.SetupAnimationPlacement(&the_bam,  //the cursors are loaded in 'the_bam' now
    the_area.doorheaders[m_doornum].launchx,
    the_area.doorheaders[m_doornum].launchy);
  if(dlg.DoModal()==IDOK)
  {
    point=dlg.GetPoint(0);
    the_area.doorheaders[m_doornum].launchx=(short) point.x;
    the_area.doorheaders[m_doornum].launchy=(short) point.y;
  }
  RefreshDoor();
  UpdateData(UD_DISPLAY);
}

void CAreaDoor::OnUnknown() 
{
	// TODO: Add your control notification handler code here
	
	UpdateData(UD_DISPLAY);	
}

/////////////////////////////////////////////////////////////////////////////
// CAreaAnim dialog

CAreaAnim::CAreaAnim()	: CPropertyPage(CAreaAnim::IDD)
{
	//{{AFX_DATA_INIT(CAreaAnim)
	m_timeofday = _T("");
	//}}AFX_DATA_INIT
  m_animnum=-1;
  memset(&animcopy,0,sizeof(animcopy));
  play=false;
  bgcolor=RGB(32,32,32);
  hbanim = 0;
  hbma = 0;
}

CAreaAnim::~CAreaAnim()
{
  if(hbanim) DeleteObject(hbanim);
  if(hbma) DeleteObject(hbma);
}

static int animboxids[]={IDC_ANIMPICKER, IDC_POSX, IDC_POSY, IDC_BAM,
IDC_FRAME, IDC_CYCLE, IDC_FLAGS, IDC_U36, IDC_U38,IDC_U3C,IDC_U3E,
IDC_BMP,IDC_U48,IDC_FLAG1,IDC_FLAG2,IDC_FLAG3,IDC_FLAG4,IDC_FLAG5,
IDC_FLAG6,IDC_FLAG7,IDC_FLAG8,IDC_FLAG9,IDC_FLAG10,IDC_FLAG11,IDC_FLAG12,
IDC_FLAG13,IDC_FLAG14,IDC_FLAG15,IDC_FLAG16,IDC_BAMFRAME,IDC_PLAY,
IDC_SCHEDULE,IDC_REMOVE, IDC_COPY, IDC_PASTE, IDC_BROWSE, IDC_BROWSE2,
IDC_FIT,
0};
void CAreaAnim::DoDataExchange(CDataExchange* pDX)
{
  CWnd *cb;
  CButton *cb2;
  CString tmpstr;
  int flg;
  int nFrameIndex;
  int i,j;

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAreaAnim)
	DDX_Control(pDX, IDC_BAMFRAME, m_bamframe);
	DDX_Control(pDX, IDC_SCHEDULE, m_schedule_control);
	DDX_Control(pDX, IDC_ANIMPICKER, m_animpicker);
	DDX_Text(pDX, IDC_TIMEOFDAY, m_timeofday);
	//}}AFX_DATA_MAP
  cb=GetDlgItem(IDC_MAXANIM);
  flg=m_animnum>=0;
  tmpstr.Format("/ %d",the_area.animcount);
  cb->SetWindowText(tmpstr);
  i=0;
  while(animboxids[i])
  {
    cb=GetDlgItem(animboxids[i++]);
    cb->EnableWindow(flg);
  }
  if(flg)
  {
    tmpstr.Format("%d. %-.32s",m_animnum+1,the_area.animheaders[m_animnum].animname);
    DDX_Text(pDX, IDC_ANIMPICKER, tmpstr);    

    DDX_Text(pDX, IDC_POSX, the_area.animheaders[m_animnum].posx);
    DDX_Text(pDX, IDC_POSY, the_area.animheaders[m_animnum].posy);
    DDX_Text(pDX, IDC_CYCLE, the_area.animheaders[m_animnum].cyclenum);
    DDX_Text(pDX, IDC_FRAME, the_area.animheaders[m_animnum].framenum);

    RetrieveResref(tmpstr, the_area.animheaders[m_animnum].bam);
    DDX_Text(pDX, IDC_BAM, tmpstr);
    DDV_MaxChars(pDX, tmpstr, 8);
    StoreResref(tmpstr, the_area.animheaders[m_animnum].bam);
    if(bma!=tmpstr)
    {
      bma=tmpstr;
      if(!read_bam(tmpstr,&the_anim))
      {
        nFrameIndex=the_anim.GetFrameIndex(the_area.animheaders[m_animnum].cyclenum, the_area.animheaders[m_animnum].framenum);
        the_anim.MakeBitmap(nFrameIndex,bgcolor,hbma,BM_RESIZE,32,32);
        m_bamframe.SetBitmap(hbma);
      }
      else
      {
        m_bamframe.SetBitmap(0);
      }
    }

    DDX_Text(pDX, IDC_FLAGS, the_area.animheaders[m_animnum].flags);
    DDX_Text(pDX, IDC_U36,the_area.animheaders[m_animnum].unknown36);
    DDX_Text(pDX, IDC_U38,the_area.animheaders[m_animnum].unknown38);
    DDX_Text(pDX, IDC_U3C,the_area.animheaders[m_animnum].unknown3c);
    DDX_Text(pDX, IDC_U3E,the_area.animheaders[m_animnum].unknown3e);

    RetrieveResref(tmpstr, the_area.animheaders[m_animnum].bmp);
    DDX_Text(pDX, IDC_BMP,tmpstr);
    DDV_MaxChars(pDX, tmpstr, 8);
    StoreResref(tmpstr, the_area.animheaders[m_animnum].bmp);

    DDX_Text(pDX, IDC_U48,the_area.animheaders[m_animnum].unknown48);
    j=1;
    for(i=0;i<16;i++)
    {
      cb2=(CButton *) GetDlgItem(IDC_FLAG1+i);
      if(the_area.animheaders[m_animnum].flags&j) cb2->SetCheck(true);
      else cb2->SetCheck(false);
      j<<=1;
    }
  }
}

void CAreaAnim::RefreshAnim()
{
  CString tmpstr;
	CSchedule tmp;
  int i;

  if(m_animnum<0 || m_animnum>=the_area.header.animationcnt)
  {
    if(the_area.header.animationcnt) m_animnum=0;
    else m_animnum=-1;
  }
  if(IsWindow(m_animpicker) )
  {
    m_animpicker.ResetContent();
    for(i=0;i<the_area.animcount;i++)
    {
      tmpstr.Format("%d. %-.32s",i+1,the_area.animheaders[i].animname);
      m_animpicker.AddString(tmpstr);
    }
    m_animnum=m_animpicker.SetCurSel(m_animnum);
  }
  if(m_animnum>=0)
  {
    tmp.m_schedule=the_area.animheaders[m_animnum].schedule;
    if(IsWindow(m_schedule_control) )
    {
      m_schedule_control.SetWindowText(tmp.GetCaption());
    }
    m_timeofday=tmp.GetTimeOfDay();
  }
  else
  {
    if(IsWindow(m_schedule_control) )
    {
      
      m_schedule_control.SetWindowText("-- not available --");    
    }
    m_timeofday="";
  }
}

BOOL CAreaAnim::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	RefreshAnim();
	return TRUE;  
}

BEGIN_MESSAGE_MAP(CAreaAnim, CPropertyPage)
	//{{AFX_MSG_MAP(CAreaAnim)
	ON_CBN_KILLFOCUS(IDC_ANIMPICKER, OnKillfocusAnimpicker)
	ON_CBN_SELCHANGE(IDC_ANIMPICKER, OnSelchangeAnimpicker)
	ON_EN_KILLFOCUS(IDC_POSX, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_POSY, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_BAM, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_FRAME, OnKillfocusFrame)
	ON_EN_KILLFOCUS(IDC_CYCLE, OnKillfocusCycle)
	ON_BN_CLICKED(IDC_FLAG1, OnFlag1)
	ON_BN_CLICKED(IDC_FLAG2, OnFlag2)
	ON_BN_CLICKED(IDC_FLAG3, OnFlag3)
	ON_BN_CLICKED(IDC_FLAG4, OnFlag4)
	ON_BN_CLICKED(IDC_FLAG5, OnFlag5)
	ON_BN_CLICKED(IDC_FLAG6, OnFlag6)
	ON_BN_CLICKED(IDC_FLAG7, OnFlag7)
	ON_BN_CLICKED(IDC_FLAG8, OnFlag8)
	ON_BN_CLICKED(IDC_FLAG9, OnFlag9)
	ON_BN_CLICKED(IDC_FLAG10, OnFlag10)
	ON_BN_CLICKED(IDC_FLAG11, OnFlag11)
	ON_BN_CLICKED(IDC_FLAG12, OnFlag12)
	ON_BN_CLICKED(IDC_FLAG13, OnFlag13)
	ON_BN_CLICKED(IDC_FLAG14, OnFlag14)
	ON_BN_CLICKED(IDC_FLAG15, OnFlag15)
	ON_BN_CLICKED(IDC_FLAG16, OnFlag16)
	ON_EN_KILLFOCUS(IDC_FLAGS, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U36, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U38, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U3C, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U3E, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_BMP, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U48, DefaultKillfocus)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_COPY, OnCopy)
	ON_BN_CLICKED(IDC_PASTE, OnPaste)
	ON_BN_CLICKED(IDC_BROWSE2, OnBrowse2)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_SCHEDULE, OnSchedule)
	ON_BN_CLICKED(IDC_PLAY, OnPlay)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_FIT, OnFit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAreaAnim message handlers

void CAreaAnim::OnKillfocusAnimpicker() 
{
  CString tmpstr;
  int x;

  m_animpicker.GetWindowText(tmpstr);
  x=strtonum(tmpstr)-1;
  if(x>=0 && x<=the_area.animcount)
  {
    m_animnum=m_animpicker.SetCurSel(x);
  }
  else
  {
    if(m_animnum>=0 && m_animnum<=the_area.animcount)
    {
      StoreName(tmpstr, the_area.animheaders[m_animnum].animname);
    }
  }
  RefreshAnim();
	UpdateData(UD_DISPLAY);	
}

void CAreaAnim::OnSelchangeAnimpicker() 
{
  int x;

  x=m_animpicker.GetCurSel();
  if(x>=0 && x<=the_area.animcount) m_animnum=x;
  RefreshAnim();
	UpdateData(UD_DISPLAY);	
}

void CAreaAnim::DefaultKillfocus() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CAreaAnim::OnKillfocusFrame() 
{
	UpdateData(UD_RETRIEVE);
  bma="";
	UpdateData(UD_DISPLAY);
}

void CAreaAnim::OnKillfocusCycle() 
{
	UpdateData(UD_RETRIEVE);
  bma="";
	UpdateData(UD_DISPLAY);
}

void CAreaAnim::OnFlag1() 
{
	the_area.animheaders[m_animnum].flags^=1;
	UpdateData(UD_DISPLAY);
}

void CAreaAnim::OnFlag2() 
{
	the_area.animheaders[m_animnum].flags^=2;
	UpdateData(UD_DISPLAY);
}

void CAreaAnim::OnFlag3() 
{
	the_area.animheaders[m_animnum].flags^=4;
	UpdateData(UD_DISPLAY);
}

void CAreaAnim::OnFlag4() 
{
	the_area.animheaders[m_animnum].flags^=8;
	UpdateData(UD_DISPLAY);
}

void CAreaAnim::OnFlag5() 
{
	the_area.animheaders[m_animnum].flags^=16;
	UpdateData(UD_DISPLAY);
}

void CAreaAnim::OnFlag6() 
{
	the_area.animheaders[m_animnum].flags^=32;
	UpdateData(UD_DISPLAY);
}

void CAreaAnim::OnFlag7() 
{
	the_area.animheaders[m_animnum].flags^=64;
	UpdateData(UD_DISPLAY);
}

void CAreaAnim::OnFlag8() 
{
	the_area.animheaders[m_animnum].flags^=128;
	UpdateData(UD_DISPLAY);
}

void CAreaAnim::OnFlag9() 
{
	the_area.animheaders[m_animnum].flags^=256;
	UpdateData(UD_DISPLAY);
}

void CAreaAnim::OnFlag10() 
{
	the_area.animheaders[m_animnum].flags^=512;
	UpdateData(UD_DISPLAY);
}

void CAreaAnim::OnFlag11() 
{
	the_area.animheaders[m_animnum].flags^=1024;
	UpdateData(UD_DISPLAY);
}

void CAreaAnim::OnFlag12() 
{
	the_area.animheaders[m_animnum].flags^=AA_MIRROR;
	UpdateData(UD_DISPLAY);
}

void CAreaAnim::OnFlag13() 
{
	the_area.animheaders[m_animnum].flags^=4096;
	UpdateData(UD_DISPLAY);
}

void CAreaAnim::OnFlag14() 
{
	the_area.animheaders[m_animnum].flags^=8192;
	UpdateData(UD_DISPLAY);
}

void CAreaAnim::OnFlag15() 
{
	the_area.animheaders[m_animnum].flags^=0x4000;
	UpdateData(UD_DISPLAY);
}

void CAreaAnim::OnFlag16() 
{
	the_area.animheaders[m_animnum].flags^=0x8000;
	UpdateData(UD_DISPLAY);
}

void CAreaAnim::OnBrowse() 
{
  if(m_animnum<0) return;
  pickerdlg.m_restype=REF_BAM;
  RetrieveResref(pickerdlg.m_picked,the_area.animheaders[m_animnum].bam);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_area.animheaders[m_animnum].bam);
  }
	UpdateData(UD_DISPLAY);	
}

void CAreaAnim::OnBrowse2() 
{
  if(m_animnum<0) return;
  pickerdlg.m_restype=REF_BMP;
  RetrieveResref(pickerdlg.m_picked,the_area.animheaders[m_animnum].bmp);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_area.animheaders[m_animnum].bmp);
  }
	UpdateData(UD_DISPLAY);	
}

void CAreaAnim::OnPlay() 
{
  if(m_animnum<0) return;
  playmax=the_anim.GetCycleData(0).y;
  playindex=0;
  play=true;
  SetTimer(0,100,NULL);	
}

void CAreaAnim::OnSchedule() 
{
	CSchedule dlg;

  if(m_animnum<0) return;
  dlg.m_schedule=the_area.animheaders[m_animnum].schedule;
  dlg.m_default=defschedule;
  if(dlg.DoModal()==IDOK)
  {
    the_area.animheaders[m_animnum].schedule=dlg.m_schedule;
    m_schedule_control.SetWindowText(dlg.GetCaption());
    m_timeofday=dlg.GetTimeOfDay();
  }
  defschedule=dlg.m_default; //we copy this always
  UpdateData(UD_DISPLAY);
}

void CAreaAnim::OnAdd() 
{
  area_anim *newanims;
  CString tmpstr;
  
  if(the_area.header.animationcnt>999)
  {
    MessageBox("Cannot add more anims.","Warning",MB_ICONEXCLAMATION|MB_OK);
    return;
  }
  newanims=new area_anim[++the_area.header.animationcnt];
  if(!newanims)
  {
    the_area.header.animationcnt--;
    MessageBox("Not enough memory.","Error",MB_ICONSTOP|MB_OK);
    return;
  }
  memcpy(newanims, the_area.animheaders, the_area.animcount*sizeof(area_anim));
  memset(newanims+the_area.animcount,0,sizeof(area_anim) );
  tmpstr.Format("Animation %d",the_area.header.animationcnt);
  StoreName(tmpstr,newanims[the_area.animcount].animname);
  
  if(the_area.animheaders)
  {
    delete [] the_area.animheaders;
  }
  the_area.animheaders=newanims;
  m_animnum=the_area.animcount; //last element
  the_area.animcount=the_area.header.animationcnt;

  RefreshAnim();
	UpdateData(UD_DISPLAY);
}

void CAreaAnim::OnRemove() 
{
  area_anim *newanims;

  if(m_animnum<0 || !the_area.header.animationcnt) return;
  
  newanims=new area_anim[--the_area.header.animationcnt];
  if(!newanims)
  {
    the_area.header.animationcnt++;
    return;
  }
  memcpy(newanims,the_area.animheaders,m_animnum*sizeof(area_anim) );
  memcpy(newanims+m_animnum,the_area.animheaders+m_animnum+1,(the_area.header.animationcnt-m_animnum)*sizeof(area_anim) );
  if(the_area.animheaders)
  {
    delete [] the_area.animheaders;
  }
  the_area.animheaders=newanims;
  the_area.animcount=the_area.header.animationcnt;
	if(m_animnum>=the_area.animcount) m_animnum--;
  RefreshAnim();
	UpdateData(UD_DISPLAY);
}

void CAreaAnim::OnCopy() 
{
  if(m_animnum>=0)
  {
    memcpy(&animcopy,the_area.animheaders+m_animnum,sizeof(animcopy) );
  }
}

void CAreaAnim::OnPaste() 
{
  if(m_animnum>=0)
  {
    memcpy(the_area.animheaders+m_animnum,&animcopy, sizeof(animcopy) );
  }
  RefreshAnim();
	UpdateData(UD_DISPLAY);
}

void CAreaAnim::OnFit() 
{
  CImageView dlg;
  CPoint point;

  if(SetupSelectPoint()) return;
  dlg.InitView(IW_ENABLEBUTTON|IW_SETVERTEX|IW_PLACEIMAGE|IW_MATCH);
  dlg.SetupAnimationPlacement(&the_anim,
    the_area.animheaders[m_animnum].posx,
    the_area.animheaders[m_animnum].posy);
  if(dlg.DoModal()==IDOK)
  {
    point=dlg.GetPoint(0);
    the_area.animheaders[m_animnum].posx=(short) point.x;
    the_area.animheaders[m_animnum].posy=(short) point.y;
  }
  RefreshAnim();
  UpdateData(UD_DISPLAY);
}

/////////////////////////////////////////////////////////////////////////////
// CAreaMap dialog

CAreaMap::CAreaMap()	: CPropertyPage(CAreaMap::IDD)
{
	//{{AFX_DATA_INIT(CAreaMap)
	m_maptype = 0;
	//}}AFX_DATA_INIT
}

CAreaMap::~CAreaMap()
{
  
}

void CAreaMap::RefreshMap()
{
}

void CAreaMap::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAreaMap)
	DDX_Radio(pDX, IDC_HEIGHTMAP, m_maptype);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAreaMap, CPropertyPage)
	//{{AFX_MSG_MAP(CAreaMap)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAreaMap message handlers

/////////////////////////////////////////////////////////////////////////////
// CAreaPropertySheet

IMPLEMENT_DYNAMIC(CAreaPropertySheet, CPropertySheet)

//ids_aboutbox is a fake ID, the sheet has no caption
CAreaPropertySheet::CAreaPropertySheet(CWnd* pWndParent)
: CPropertySheet(IDS_ABOUTBOX, pWndParent)
{
  AddPage(&m_PageGeneral);
  AddPage(&m_PageActor);
  AddPage(&m_PageTrigger);
  AddPage(&m_PageSpawn);
  AddPage(&m_PageEntrance);
  AddPage(&m_PageContainer);
  AddPage(&m_PageAmbient);
  AddPage(&m_PageDoor);
  AddPage(&m_PageAnim);
  AddPage(&m_PageVariable);
  AddPage(&m_PageMap);
}

CAreaPropertySheet::~CAreaPropertySheet()
{
}

void CAreaPropertySheet::RefreshDialog()
{
  CPropertyPage *page;

  m_PageGeneral.RefreshGeneral();
  m_PageActor.RefreshActor();
  m_PageTrigger.RefreshTrigger();
  m_PageSpawn.RefreshSpawn();
  m_PageEntrance.RefreshEntrance();
  m_PageContainer.RefreshContainer();
  m_PageAmbient.RefreshAmbient();
  m_PageDoor.RefreshDoor();
  m_PageVariable.RefreshVariable();
  m_PageAnim.RefreshAnim();
  m_PageMap.RefreshMap();
  page=GetActivePage();
  page->UpdateData(UD_DISPLAY);
  read_bam("CURSORS",&the_bam); //pre-reading the cursors bam
}

BEGIN_MESSAGE_MAP(CAreaPropertySheet, CPropertySheet)
//{{AFX_MSG_MAP(CAreaPropertySheet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
