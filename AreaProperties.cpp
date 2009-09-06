// AreaGeneral.cpp : implementation file
//
  
#include "stdafx.h"
#include "chitem.h"
#include "AreaProperties.h"
#include "AreaSong.h"
#include "AreaInt.h"
#include "AreaTriggerString.h"
#include "Schedule.h"
#include "options.h"
#include "ImageView.h"
#include "WedEdit.h"
#include "CreatureEdit.h"
#include "Polygon.h"
#include "PaletteEdit.h"
//
#include "chitemDlg.h"
//

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
IMPLEMENT_DYNCREATE(CAreaProj, CPropertyPage)

static unsigned long defschedule;

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
  area_header tmp;
  CButton *cb, *cb2;
  CString tmpstr, old;
  int i,j;

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAreaGeneral)
	DDX_Control(pDX, IDC_AREATYPE, m_areaflag_control);
	//}}AFX_DATA_MAP
  RetrieveResref(tmpstr, the_area.header.wed);
  old=tmpstr;
  DDX_Text(pDX, IDC_WED, tmpstr);
  DDV_MaxChars(pDX, tmpstr, 8); //night wedfile!! areas should be 6 character length
  if(tmpstr.IsEmpty()) tmpstr=itemname;
  if(tmpstr!=old) the_area.wedchanged=true;
  StoreResref(tmpstr, the_area.header.wed);

  memcpy(&tmp, &the_area.header, sizeof(area_header));
  tmpstr=get_areaflag(the_area.header.areaflags);
  DDX_Text(pDX, IDC_AREATYPE, tmpstr);
  the_area.header.areaflags=strtonum(tmpstr);

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
      if(the_area.header.areatype&j) cb->SetCheck(true);
      else cb->SetCheck(false);
      j<<=1;
    }
  }
  if(memcmp(&tmp, &the_area.header, sizeof(area_header)))
  {
    the_area.m_changed=true;
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
  default:
    id=0; //just to make the compiler happy
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
  cw->EnableWindow(bg2_weaprofs() && the_area.WedAvailable()&&(the_area.header.areatype&EXTENDED_NIGHT) );
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
  m_areaflag_control.ResetContent();
  flg=bg1_compatible_area();
  for(i=0;i<8;i++)
  {
    cb=GetDlgItem(bg1areaids[i]);
    cb->EnableWindow(flg);
  }

  m_areaflag_control.AddString(get_areaflag(0) );
  flg=1;
  for(i=0;i<NUM_ARFLAGS;i++)
  {
    m_areaflag_control.AddString(get_areaflag(flg) );
    flg<<=1;
  }
  //tooltips
  {
    m_tooltip.Create(this,TTS_NOPREFIX);
    m_tooltip.SetMaxTipWidth(200);
    m_tooltip.SetTipBkColor(RGB(240,224,160));
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
	ON_BN_CLICKED(IDC_NIGHT, OnNight)
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
	ON_BN_CLICKED(IDC_EXPLORED, OnExplored)
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
	the_area.header.areatype^=1;
  the_area.m_changed=true;
	UpdateData(UD_DISPLAY);
}

void CAreaGeneral::OnDaynight() 
{
	the_area.header.areatype^=2;
  the_area.m_changed=true;
	UpdateData(UD_DISPLAY);
}

void CAreaGeneral::OnWeather() 
{
	the_area.header.areatype^=4;
  the_area.m_changed=true;
	UpdateData(UD_DISPLAY);
}

void CAreaGeneral::OnCity() 
{
	the_area.header.areatype^=8;
  the_area.m_changed=true;
	UpdateData(UD_DISPLAY);
}

void CAreaGeneral::OnForest() 
{
	the_area.header.areatype^=16;
  the_area.m_changed=true;
	UpdateData(UD_DISPLAY);
}

void CAreaGeneral::OnDungeon() 
{
	the_area.header.areatype^=32;
  the_area.m_changed=true;
	UpdateData(UD_DISPLAY);
}

void CAreaGeneral::OnExtendednight() 
{
	the_area.header.areatype^=EXTENDED_NIGHT; //64
  the_area.m_changed=true;
	UpdateData(UD_DISPLAY);
}

void CAreaGeneral::OnCanrest() 
{
	the_area.header.areatype^=128;
  the_area.m_changed=true;
	UpdateData(UD_DISPLAY);
}

void CAreaGeneral::OnSongs() 
{
	CAreaSong dlg;
  area_song tmp;
	
  memcpy(&tmp,&the_area.songheader,sizeof(area_song));
  dlg.DoModal();
  if(memcmp(&tmp,&the_area.songheader,sizeof(area_song)) )
  {
    the_area.m_changed=true;
  }
}

void CAreaGeneral::OnInt() 
{
	CAreaInt dlg;
  area_int tmp;
	
  memcpy(&tmp,&the_area.intheader,sizeof(area_int));
  dlg.DoModal();
  if(memcmp(&tmp,&the_area.intheader,sizeof(area_int)) )
  {
    the_area.m_changed=true;
  }
}

void CAreaGeneral::OnSong() 
{
	the_area.header.songoffset=!the_area.header.songoffset;
  the_area.m_changed=true;
  UpdateData(UD_DISPLAY);
}

void CAreaGeneral::OnRest() 
{
  the_area.header.intoffset=!the_area.header.intoffset;
  the_area.m_changed=true;
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

void CAreaGeneral::OnExplored() 
{
  CImageView dlg;
  CPoint point;
  unsigned int maxx,maxy;
  unsigned int i,j;
  int dest, idx;

  if(SetupSelectPoint(0)) return;
  maxx=the_area.m_width/32;
  maxy=the_area.m_height/32;
//this is not for PST!
  if(!pst_compatible_var())
  {
    maxx++;
    maxy++;
  }
  
  unsigned char *bitmap = new unsigned char[maxx*maxy];
  if(!bitmap)
  {
    MessageBox("Out of memory","Error",MB_ICONSTOP|MB_OK);
    return;
  }
  memset(bitmap,0,maxx*maxy); //unsigned char
  if(the_area.explored)
  {
    for(j=0;j<maxy;j++)
    {
      for(i=0;i<maxx;i++)
      {
        dest = i+j*maxx;
        idx = dest>>3;
        
        if(the_area.exploredsize>idx)
        {
          bitmap[dest]=!!(the_area.explored[idx]&(1<<(dest&7) ));
        }
      }
    }
  }
  dlg.SetMapType(MT_EXPLORED, bitmap);
  dlg.InitView(IW_OKBUTTON|IW_ENABLEBUTTON|IW_EDITMAP|IW_SHOWALL|IW_SHOWGRID|IW_ENABLEFILL, &the_mos);
  
  dlg.m_value=1;
  dlg.DoModal();
  if(the_area.explored)
  {
    the_area.KillExplored();
  }

  //checking if the bitmap was completely erased (remove the bitmap then)
  for(i=0;i<maxx*maxy && !bitmap[i];i++);
  if(i!=maxx*maxy)
  {
    the_area.exploredsize=(maxx*maxy+7)/8;
    the_area.header.exploredoffset=0;//recalculated on save
    the_area.explored = new char[the_area.exploredsize];
    memset(the_area.explored,0,the_area.exploredsize);
    for(j=0;j<maxy;j++)
    {
      for(i=0;i<maxx;i++)
      {
        dest = i+j*maxx;
        idx = dest>>3;
        
        if(the_area.exploredsize>idx)
        {
          if(bitmap[dest])
          {
            the_area.explored[idx]|=1<<(dest&7);
          }
        }
      }
    }
  }
  delete [] bitmap;
  the_area.header.exploredsize=the_area.exploredsize;
  UpdateData(UD_DISPLAY);
}

void CAreaGeneral::OnUnknown() 
{
	// TODO: Add your control notification handler code here
	//edit iwd unknowns
  UpdateData(UD_DISPLAY);
}

int CAreaGeneral::CantMakeMinimap(CString tmpstr)
{
  loc_entry dummy;

  if(!the_area.overlaycount) return true;
  //this is a wed name
  if(mos.Lookup(tmpstr, dummy) ) return true;
  //this is a tis name
  RetrieveResref(tmpstr,the_area.overlayheaders[0].tis);
  if(!tis.Lookup(tmpstr, dummy) ) return true;
  if(MessageBox("Minimap doesn't exist yet, do you want to create it now?","Area editor",MB_ICONQUESTION|MB_YESNO)!=IDYES)
  {
    return true;
  }
  read_tis(tmpstr,&the_mos,true); //this is loaded into the common mos file
  the_mos.mosheader.wColumn=the_area.overlayheaders[0].width;
  the_mos.mosheader.wRow=the_area.overlayheaders[0].height;
  CreateMinimap(GetSafeHwnd());
  return false; //we made the minimap now
}

void CAreaGeneral::OnView() 
{
  Cmos my_mos;
  CImageView dlg;
  CString tmpstr;

  RetrieveResref(tmpstr, the_area.header.wed);
retry:
  if(read_mos(tmpstr,&my_mos,true)<0)
  {
    if(CantMakeMinimap(tmpstr))
    {
      MessageBox("Error while loading minimap.","Area editor",MB_OK);
      return;
    }
    goto retry;
  }
  dlg.InitView(IW_ENABLEBUTTON, &my_mos);
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
  CString tmpstr;
	CWedEdit dlg;

  RetrieveResref(tmpstr,the_area.header.wed);
  if(tmpstr=="NEW AREA")
  {
    MessageBox("Give a wed name first!","Area editor",MB_OK);
    return;
  }
  if(GetWed(false)) return;
  dlg.DoModal();
}

void CAreaGeneral::OnNight() 
{
  CString tmpstr;
	CWedEdit dlg;
	  
  RetrieveResref(tmpstr,the_area.header.wed);
  if(tmpstr=="NEW AREA")
  {
    MessageBox("Give a wed name first!","Area editor",MB_OK);
    return;
  }

  if(GetWed(true)) return;
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
IDC_DESTX, IDC_DESTY, IDC_VISIBLE, IDC_U2C, IDC_FACE, IDC_ANIMATION,
IDC_U36, IDC_U38,IDC_U3C,IDC_TALKNUM,IDC_DIALOG, IDC_OVERRIDE, IDC_CLASS,
IDC_RACE, IDC_GENERAL, IDC_DEFAULT, IDC_SPECIFIC, IDC_AREA, IDC_SAME,
IDC_SCHEDULE, IDC_CLEAR, IDC_FIELDS, IDC_BROWSE, IDC_COPY, IDC_PASTE, 
IDC_REMOVE, IDC_BROWSE2,IDC_BROWSE3, IDC_BROWSE4,IDC_BROWSE5 ,IDC_BROWSE6,
IDC_BROWSE7, IDC_BROWSE8, IDC_BROWSE9, IDC_SETPOS, IDC_SETDEST,
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
  if(flg)
  {
    cb=GetDlgItem(IDC_EDIT);
    cb->EnableWindow(!!the_area.actorheaders[m_actornum].creoffset);
    cb=GetDlgItem(IDC_EMBED);
    cb->EnableWindow(!the_area.actorheaders[m_actornum].creoffset);
    tmpstr.Format("%d. %-.32s",m_actornum+1,the_area.actorheaders[m_actornum].actorname);
    DDX_Text(pDX, IDC_ACTORPICKER, tmpstr);    

    DDX_Text(pDX, IDC_POSX, the_area.actorheaders[m_actornum].posx);
    DDX_Text(pDX, IDC_POSY, the_area.actorheaders[m_actornum].posy);
    DDX_Text(pDX, IDC_DESTX, the_area.actorheaders[m_actornum].destx);
    DDX_Text(pDX, IDC_DESTY, the_area.actorheaders[m_actornum].desty);
    DDX_Text(pDX, IDC_VISIBLE, the_area.actorheaders[m_actornum].fields);

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

    tmpstr.Format("0x%04x %s",the_area.actorheaders[m_actornum].animation,IDSToken("ANIMATE",the_area.actorheaders[m_actornum].animation, false) );
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

    RetrieveResref(tmpstr,the_area.actorheaders[m_actornum].scrarea);
    DDX_Text(pDX, IDC_AREA, tmpstr);
    DDV_MaxChars(pDX, tmpstr, 8);
    StoreResref(tmpstr,the_area.actorheaders[m_actornum].scrarea);

    RetrieveResref(tmpstr,the_area.actorheaders[m_actornum].dialog);
    DDX_Text(pDX, IDC_DIALOG, tmpstr);
    DDV_MaxChars(pDX, tmpstr, 8);
    StoreResref(tmpstr,the_area.actorheaders[m_actornum].dialog);
  }
  else
  {
    cb=GetDlgItem(IDC_EDIT);
    cb->EnableWindow(false);
    cb=GetDlgItem(IDC_EMBED);
    cb->EnableWindow(false);
  }
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

  //tooltips
  {
    m_tooltip.Create(this,TTS_NOPREFIX);
    m_tooltip.SetMaxTipWidth(200);
    m_tooltip.SetTipBkColor(RGB(240,224,160));

    m_tooltip.AddTool(GetDlgItem(IDC_ACTORPICKER), IDS_LABEL);
    m_tooltip.AddTool(GetDlgItem(IDC_VISIBLE), IDS_FIELDS);
    m_tooltip.AddTool(GetDlgItem(IDC_U38), IDS_VISIBLE);
  }

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
	ON_BN_CLICKED(IDC_BROWSE9, OnBrowse9)
	ON_BN_CLICKED(IDC_EMBED, OnEmbed)
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
	ON_EN_CHANGE(IDC_AREA, DefaultKillfocus)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
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
  newactors[the_area.actorcount].fields=1; //use the creresref field
  newactors[the_area.actorcount].schedule=0xffffff;
  newactors[the_area.actorcount].unknown38=-1;

  the_area.actorheaders=newactors;
  the_area.credatapointers=newdatapointers;
  m_actornum=the_area.actorcount; //last element 
  the_area.actorcount=the_area.header.actorcnt;

  the_area.m_changed=true;
  RefreshActor();
	UpdateData(UD_DISPLAY);
}
  
void CAreaActor::OnButton1() 
{
  area_actor *newactors;
  char **newdatapointers;

  if(m_actornum<0 || !the_area.header.actorcnt) return;
	
  the_area.header.actorcnt=(short) m_actornum;
  newactors=new area_actor[the_area.header.actorcnt];
  if (!newactors)
  {
    the_area.header.actorcnt=(short) the_area.actorcount;
    MessageBox("Not enough memory.","Error",MB_ICONSTOP|MB_OK);
    return;
  }
  newdatapointers=new char *[the_area.header.actorcnt];
  if(!newdatapointers)
  {
    delete [] newactors;
    the_area.header.actorcnt=(short) the_area.actorcount;
    MessageBox("Not enough memory.","Error",MB_ICONSTOP|MB_OK);
    return;
  }
  if(the_area.credatapointers)
  {
    memcpy(newdatapointers,the_area.credatapointers,m_actornum*sizeof(char *) );
    for(int i=m_actornum;i<the_area.actorcount;i++)
    {
      delete the_area.credatapointers[i];
    }
    delete [] the_area.credatapointers;
  }
  if(the_area.actorheaders)
  {
    memcpy(newactors,the_area.actorheaders,m_actornum*sizeof(area_actor) );
    delete [] the_area.actorheaders;
  }

  the_area.actorheaders=newactors;
  the_area.credatapointers=newdatapointers;
  the_area.actorcount=the_area.header.actorcnt;
	if(m_actornum>=the_area.actorcount) m_actornum=the_area.actorcount-1;

  the_area.m_changed=true;
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
    MessageBox("Not enough memory.","Error",MB_ICONSTOP|MB_OK);
    return;
  }
  newdatapointers=new char *[the_area.header.actorcnt];
  if(!newdatapointers)
  {
    delete [] newactors;
    the_area.header.actorcnt++;
    MessageBox("Not enough memory.","Error",MB_ICONSTOP|MB_OK);
    return;
  }
  if(the_area.credatapointers)
  {
    memcpy(newdatapointers,the_area.credatapointers,m_actornum*sizeof(char *) );
    memcpy(newdatapointers+m_actornum,the_area.credatapointers+m_actornum+1,(the_area.header.actorcnt-m_actornum)*sizeof(char *) );
    delete the_area.credatapointers[m_actornum];
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

  the_area.m_changed=true;
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

  if(SetupSelectPoint(0)) return;
  dlg.InitView(IW_SHOWGRID|IW_ENABLEBUTTON|IW_PLACEIMAGE|IW_SHOWALL, &the_mos);
  //the cursors are loaded in 'the_bam' now
  dlg.SetupAnimationPlacement(&the_bam, the_area.actorheaders[m_actornum].posx,
    the_area.actorheaders[m_actornum].posy,0);
  if(dlg.DoModal()==IDOK)
  {
    point=dlg.GetPoint(GP_POINT);
    the_area.actorheaders[m_actornum].posx=(short) point.x;
    the_area.actorheaders[m_actornum].posy=(short) point.y;
  }

  the_area.m_changed=true;
  RefreshActor();
  UpdateData(UD_DISPLAY);
}

void CAreaActor::OnSetdest() 
{
  CImageView dlg;
  CPoint point;

  if(SetupSelectPoint(0)) return;
  dlg.InitView(IW_SHOWGRID|IW_ENABLEBUTTON|IW_PLACEIMAGE, &the_mos);
  //the cursors are loaded in 'the_bam' now
  dlg.SetupAnimationPlacement(&the_bam, the_area.actorheaders[m_actornum].destx,
    the_area.actorheaders[m_actornum].desty,0);
  if(dlg.DoModal()==IDOK)
  {
    point=dlg.GetPoint(GP_POINT);
    the_area.actorheaders[m_actornum].destx=(short) point.x;
    the_area.actorheaders[m_actornum].desty=(short) point.y;
  }

  the_area.m_changed=true;
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
      StoreVariable(tmpstr, the_area.actorheaders[m_actornum].actorname);
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
  memset(the_area.actorheaders[m_actornum].scrspecific,0,8);
  memset(the_area.actorheaders[m_actornum].scrarea,0,8);
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

    memcpy(the_area.actorheaders[m_actornum].actorname,credata.scriptname,32);
    memcpy(the_area.actorheaders[m_actornum].dialog,credata.dialogresref,8);
    memcpy(the_area.actorheaders[m_actornum].scroverride,credata.scripts[0],8);
    memcpy(the_area.actorheaders[m_actornum].scrclass,credata.scripts[1],8);
    memcpy(the_area.actorheaders[m_actornum].scrrace,credata.scripts[2],8);
    memcpy(the_area.actorheaders[m_actornum].scrgeneral,credata.scripts[3],8);
    memcpy(the_area.actorheaders[m_actornum].scrdefault,credata.scripts[4],8);
    memcpy(the_area.actorheaders[m_actornum].scrspecific,credata.iwd2scripts[0],8);
    memcpy(the_area.actorheaders[m_actornum].scrarea,credata.iwd2scripts[1],8);
  }
  the_area.m_changed=true;
  RefreshActor();
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

void CAreaActor::OnEmbed() 
{
  CString tmpname;

	if(the_area.credatapointers[m_actornum]) return; //already embedded
  RetrieveResref(tmpname,the_area.actorheaders[m_actornum].creresref);
  if(read_creature(tmpname)<0)
  {
    return;
  }
  the_area.m_changed=true;
  the_area.actorheaders[m_actornum].fields&=~1;
  //reading creature into creature structure
  //moving creature structure into area data
  ReadTempCreature(the_area.credatapointers[m_actornum],the_area.actorheaders[m_actornum].cresize);
  the_area.actorheaders[m_actornum].creoffset=0xcdcdcdcd; //some fake nonzero value
  tmpname.SetAt(0,'*');
  StoreResref(tmpname,the_area.actorheaders[m_actornum].creresref);
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
    the_creature.m_savechanges=false;
    dlg.DoModal();
    the_creature.m_savechanges=true;
  }
  itemname=tmpname;
  if(MessageBox("Do you want to keep the changes made on the creature?","Area editor",MB_YESNO)==IDYES)
  {
    the_area.m_changed=true;
    ReadTempCreature(the_area.credatapointers[m_actornum],the_area.actorheaders[m_actornum].cresize);
//    tmpname.SetAt(0,'*');
//    StoreResref(itemname,the_area.actorheaders[m_actornum].creresref);
    RefreshActor();
    UpdateData(UD_DISPLAY);	
  }
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

void CAreaActor::OnBrowse9() 
{
  if(m_actornum<0) return;
  pickerdlg.m_restype=REF_BCS;
  RetrieveResref(pickerdlg.m_picked,the_area.actorheaders[m_actornum].scrarea);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_area.actorheaders[m_actornum].scrarea);
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
    if(the_area.actorheaders[m_actornum].schedule!=dlg.m_schedule)
    {
      the_area.m_changed=true;
    }
    the_area.actorheaders[m_actornum].schedule=dlg.m_schedule;
    m_schedule_control.SetWindowText(dlg.GetCaption());
    m_timeofday=dlg.GetTimeOfDay();
  }
  defschedule=dlg.m_default; //we copy this always
  UpdateData(UD_DISPLAY);
}

BOOL CAreaActor::PreTranslateMessage(MSG* pMsg) 
{
	m_tooltip.RelayEvent(pMsg);
	return CPropertyPage::PreTranslateMessage(pMsg);
}

/////////////////////////////////////////////////////////////////////////////
// CAreaTrigger dialog

CAreaTrigger::CAreaTrigger() : CPropertyPage(CAreaTrigger::IDD)
{
	//{{AFX_DATA_INIT(CAreaTrigger)
	//}}AFX_DATA_INIT
  m_triggernum=-1;
  memset(&triggercopy,0,sizeof(triggercopy));
  vertexcopy=NULL;
  vertexsize=0;
  hbc = NULL;
}

CAreaTrigger::~CAreaTrigger()
{
  if(hbc) DeleteObject(hbc);
  if(vertexcopy) delete [] vertexcopy;
  vertexcopy=NULL;
}

static int triggerboxids[]={IDC_TRIGGERPICKER, IDC_REGIONTYPE, IDC_POS1X, IDC_POS1Y,
IDC_POS2X, IDC_POS2Y, IDC_RECALCBOX, IDC_EDITPOLYGON, IDC_CURSORIDX,
IDC_DESTAREA, IDC_PARTY, IDC_ENTRANCENAME, IDC_TPOSX, IDC_TPOSY,
IDC_KEY, IDC_DETECT, IDC_REMOVAL, IDC_SCRIPT, IDC_TRAPPED, IDC_DETECTED,
IDC_DIALOG, IDC_UNKNOWN30, IDC_POS1X2, IDC_POS1Y2, IDC_STRING,
IDC_FLAGS, IDC_TUNDET, IDC_TRESET, IDC_PARTY, IDC_TDETECT,
IDC_TUNK1, IDC_TUNK2, IDC_TNPC, IDC_TUNK3, IDC_TDEACTIVATED, IDC_NONPC,
IDC_TOVERRIDE, IDC_TDOOR, IDC_TUNK4,IDC_TUNK5,IDC_TUNK6,IDC_TUNK7,
IDC_BROWSE, IDC_BROWSE2, IDC_BROWSE3, IDC_BROWSE4, IDC_SELECTION,
IDC_COPY, IDC_PASTE, IDC_REMOVE, IDC_UNKNOWN, IDC_CURSOR, IDC_SET,
0};

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
	DDX_Control(pDX, IDC_REGIONTYPE, m_regiontype_control);
	DDX_Control(pDX, IDC_TRIGGERPICKER, m_triggerpicker);
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
    cw->EnableWindow(flg);
    i++;
  }
  if(flg)
  {
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
    
    RetrieveResref(tmpstr, the_area.triggerheaders[m_triggernum].destref);
    DDX_Text(pDX, IDC_DESTAREA, tmpstr);
    DDV_MaxChars(pDX, tmpstr, 8);
    StoreResref(tmpstr, the_area.triggerheaders[m_triggernum].destref);

    RetrieveVariable(tmpstr, the_area.triggerheaders[m_triggernum].destname);
    DDX_Text(pDX, IDC_ENTRANCENAME, tmpstr);
    DDV_MaxChars(pDX, tmpstr, 32);
    StoreName(tmpstr, the_area.triggerheaders[m_triggernum].destname);

    tmpstr.Format("%d", the_area.triggerheaders[m_triggernum].infoflags);
    DDX_Text(pDX, IDC_FLAGS, tmpstr);
    the_area.triggerheaders[m_triggernum].infoflags=strtonum(tmpstr);
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

    if (pst_compatible_var())
    {
      DDX_Text(pDX, IDC_POS1X2, the_area.triggerheaders[m_triggernum].pstpointx);
      DDX_Text(pDX, IDC_POS1Y2, the_area.triggerheaders[m_triggernum].pstpointy);
    }
    else
    {
      DDX_Text(pDX, IDC_POS1X2, the_area.triggerheaders[m_triggernum].pointx);
      DDX_Text(pDX, IDC_POS1Y2, the_area.triggerheaders[m_triggernum].pointy);
    }
  }
}
#pragma warning(default:4706)   

//move itemdata to local variables
void CAreaTrigger::RefreshTrigger()
{
  CString tmpstr;
	CSchedule tmp;
  int i;

  if(m_triggernum<0 || m_triggernum>=the_area.header.infocnt)
  {
    if(the_area.header.infocnt) m_triggernum=0;
    else m_triggernum=-1;
  }
  if(IsWindow(m_triggerpicker) )
  {
    m_spin_control.SetRange32(0,the_bam.GetFrameCount()-1 );
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
    m_dlgstr=resolve_tlk_text(the_area.triggerheaders[m_triggernum].dialogstrref);
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
	RefreshTrigger();
  for(i=0;i<NUM_RTTYPE;i++)
  {
    m_regiontype_control.AddString(get_region_type(i));
  }
  //tooltips
  {
    m_tooltip.Create(this,TTS_NOPREFIX);
    m_tooltip.SetMaxTipWidth(200);
    m_tooltip.SetTipBkColor(RGB(240,224,160));

    m_tooltip.AddTool(GetDlgItem(IDC_TRIGGERPICKER), IDS_LABEL);
  }
  UpdateData(UD_DISPLAY);
	return TRUE;
}

BEGIN_MESSAGE_MAP(CAreaTrigger, CPropertyPage)
	//{{AFX_MSG_MAP(CAreaTrigger)
	ON_CBN_KILLFOCUS(IDC_TRIGGERPICKER, OnKillfocusTriggerpicker)
	ON_CBN_SELCHANGE(IDC_TRIGGERPICKER, OnSelchangeTriggerpicker)
	ON_BN_CLICKED(IDC_RECALCBOX, OnRecalcbox)
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
	ON_BN_CLICKED(IDC_CURSOR, OnCursor)
	ON_BN_CLICKED(IDC_UNKNOWN, OnUnknown)
	ON_CBN_DROPDOWN(IDC_ENTRANCENAME, OnDropdownEntrancename)
	ON_EN_CHANGE(IDC_CURSORIDX, OnChangeCursoridx)
	ON_EN_KILLFOCUS(IDC_DESTAREA, OnKillfocusDestarea)
	ON_BN_CLICKED(IDC_SET, OnSet)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_BROWSE2, OnBrowse2)
	ON_BN_CLICKED(IDC_BROWSE3, OnBrowse3)
	ON_BN_CLICKED(IDC_BROWSE4, OnBrowse4)
	ON_CBN_KILLFOCUS(IDC_REGIONTYPE, OnKillfocusRegiontype)
	ON_BN_CLICKED(IDC_EDITPOLYGON, OnEditpolygon)
	ON_BN_CLICKED(IDC_SELECTION, OnSelection)
	ON_BN_CLICKED(IDC_STRING, OnString)
	ON_EN_KILLFOCUS(IDC_UNKNOWN30, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_POS1X, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_POS1Y, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_POS2X, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_POS2Y, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_CURSORIDX, DefaultKillfocus)
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
	ON_BN_CLICKED(IDC_SET2, OnSet2)
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
  if(x>=0 && x<the_area.triggercount) m_triggernum=x;
  RefreshTrigger();
	UpdateData(UD_DISPLAY);
}

void CAreaTrigger::DefaultKillfocus() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CAreaTrigger::OnKillfocusRegiontype() 
{
	UpdateData(UD_RETRIEVE);
  if(m_triggernum<0 || m_triggernum>=the_area.triggercount) return;
  if(!the_area.triggerheaders[m_triggernum].cursortype)
  {
    switch(the_area.triggerheaders[m_triggernum].triggertype)
    {
    case RT_TRAP://trap    
      break;
    case RT_INFO://info
      the_area.triggerheaders[m_triggernum].cursortype=22;
      break;
    case RT_TRAVEL://travel
      the_area.triggerheaders[m_triggernum].cursortype=42;
      break;
    }
  }
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
  RetrieveResref(pickerdlg.m_picked,the_area.triggerheaders[m_triggernum].destref);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_area.triggerheaders[m_triggernum].destref);
  }
  fill_destination(pickerdlg.m_picked, &m_entrancenamepicker);
  RefreshTrigger();
	UpdateData(UD_DISPLAY);	
}

void CAreaTrigger::OnEditpolygon() 
{
	CPolygon dlg;
	
  if(m_triggernum<0) return;
  dlg.SetPolygon(MT_REGION,m_triggernum, &the_area.triggerheaders[m_triggernum].vertexcount, &the_area.triggerheaders[m_triggernum].p1x);
  dlg.DoModal();
	UpdateData(UD_DISPLAY);
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

void CAreaTrigger::OnKillfocusDestarea() 
{
  CString tmpstr;

	UpdateData(UD_RETRIEVE);
  RetrieveResref(tmpstr, the_area.triggerheaders[m_triggernum].destref);
  //refresh destname picker
  fill_destination(tmpstr, &m_entrancenamepicker);
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
  if(the_area.triggercount)
  {
    //we should insert our stuff after the last trigger
    the_area.vertexheaderlist.InsertAfter(the_area.vertexheaderlist.FindIndex(the_area.triggercount-1), newvertex);
  }
  else
  {
    //we should insert our stuff before anything
    the_area.vertexheaderlist.AddHead(newvertex);
  }
  
  memcpy(newtriggers, the_area.triggerheaders, the_area.triggercount*sizeof(area_trigger));
  memset(newtriggers+the_area.triggercount,0,sizeof(area_trigger) );
  tmpstr.Format("Info Point %d",the_area.header.infocnt);
  newtriggers[the_area.triggercount].cursortype=42;
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
    vertexcopy=new area_vertex[vertexsize];
    if(!vertexcopy) return;
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
/*
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
*/

void CAreaTrigger::OnString() 
{
	CAreaTriggerString dlg;
	
	dlg.parent = this;
	dlg.DoModal();
}

void CAreaTrigger::OnCursor() 
{
	UpdateData(UD_DISPLAY);
}

void CAreaTrigger::OnSelection() 
{
  CImageView dlg;
  CPoint point;

  if(SetupSelectPoint(0)) return;
  dlg.SetMapType(MT_REGION, (LPBYTE) &the_area.vertexheaderlist);
  dlg.m_max=the_area.triggercount;
  dlg.m_value=m_triggernum;
  dlg.InitView(IW_SHOWGRID|IW_ENABLEBUTTON|IW_GETPOLYGON, &the_mos);
	dlg.DoModal();
  m_triggernum=dlg.m_value;
  RefreshTrigger();
  UpdateData(UD_DISPLAY);
}

void CAreaTrigger::OnSet() 
{
  CImageView dlg;
  CPoint point;

  if(SetupSelectPoint(0)) return;
  dlg.InitView(IW_SHOWGRID|IW_ENABLEBUTTON|IW_PLACEIMAGE, &the_mos);
  //the cursors are loaded in 'the_bam' now
  dlg.SetupAnimationPlacement(&the_bam, the_area.triggerheaders[m_triggernum].launchx,
    the_area.triggerheaders[m_triggernum].launchy,0);
  if(dlg.DoModal()==IDOK)
  {
    point=dlg.GetPoint(GP_POINT);
    the_area.triggerheaders[m_triggernum].launchx=(short) point.x;
    the_area.triggerheaders[m_triggernum].launchy=(short) point.y;
  }
  RefreshTrigger();
  UpdateData(UD_DISPLAY);
}

void CAreaTrigger::OnSet2() 
{
  CImageView dlg;
  CPoint point;

  if(SetupSelectPoint(0)) return;
  dlg.InitView(IW_SHOWGRID|IW_ENABLEBUTTON|IW_PLACEIMAGE, &the_mos);
  //the cursors are loaded in 'the_bam' now
  if(pst_compatible_var())
  {
    point.x=the_area.triggerheaders[m_triggernum].pstpointx;
    point.y=the_area.triggerheaders[m_triggernum].pstpointy;
  }
  else
  {
    point.x=the_area.triggerheaders[m_triggernum].pointx;
    point.y=the_area.triggerheaders[m_triggernum].pointy;
  }
  dlg.SetupAnimationPlacement(&the_bam, point.x, point.y,0);
  if(dlg.DoModal()==IDOK)
  {
    point=dlg.GetPoint(GP_POINT);
    if (pst_compatible_var())
    {
      the_area.triggerheaders[m_triggernum].pstpointx=(short) point.x;
      the_area.triggerheaders[m_triggernum].pstpointy=(short) point.y;
    }
    else
    {
      the_area.triggerheaders[m_triggernum].pointx=(short) point.x;
      the_area.triggerheaders[m_triggernum].pointy=(short) point.y;
    }
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

BOOL CAreaTrigger::PreTranslateMessage(MSG* pMsg) 
{
  m_tooltip.RelayEvent(pMsg);
	return CPropertyPage::PreTranslateMessage(pMsg);
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
//    if(!the_area.spawnheaders[m_spawnnum].delay) the_area.spawnheaders[m_spawnnum].delay=1;
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
#pragma warning(default:4706)   
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
  //tooltips
  {
    m_tooltip.Create(this,TTS_NOPREFIX);
    m_tooltip.SetMaxTipWidth(200);
    m_tooltip.SetTipBkColor(RGB(240,224,160));

    m_tooltip.AddTool(GetDlgItem(IDC_SPAWNPICKER), IDS_LABEL);
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
    if(the_area.spawnheaders[m_spawnnum].schedule!=dlg.m_schedule)
    {
      the_area.m_changed=true;
    }
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
  newspawns[the_area.spawncount].min=1;
  newspawns[the_area.spawncount].schedule=0xffffff;
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

  if(SetupSelectPoint(0)) return;
  dlg.InitView(IW_SHOWGRID|IW_ENABLEBUTTON|IW_PLACEIMAGE, &the_mos);
  //the cursors are loaded in 'the_bam' now
  dlg.SetupAnimationPlacement(&the_bam, the_area.spawnheaders[m_spawnnum].px,
    the_area.spawnheaders[m_spawnnum].py, 0);
  if(dlg.DoModal()==IDOK)
  {
    point=dlg.GetPoint(GP_POINT);
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
	
	UpdateData(UD_DISPLAY);	
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

BOOL CAreaSpawn::PreTranslateMessage(MSG* pMsg) 
{
  m_tooltip.RelayEvent(pMsg);
	return CPropertyPage::PreTranslateMessage(pMsg);
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

  if(m_entrancenum<0 || m_entrancenum>=the_area.entrancecount)
  {
    if(the_area.entrancecount) m_entrancenum=0;
    else m_entrancenum=-1;
  }

  if(IsWindow(m_entrancepicker) )
  {
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
  //tooltips
  {
    m_tooltip.Create(this,TTS_NOPREFIX);
    m_tooltip.SetMaxTipWidth(200);
    m_tooltip.SetTipBkColor(RGB(240,224,160));

    m_tooltip.AddTool(GetDlgItem(IDC_ENTRANCEPICKER), IDS_LABEL);
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

  if(SetupSelectPoint(0)) return;
  dlg.InitView(IW_SHOWGRID|IW_ENABLEBUTTON|IW_PLACEIMAGE, &the_mos);
  //the cursors are loaded in 'the_bam' now
  dlg.SetupAnimationPlacement(&the_bam, the_area.entranceheaders[m_entrancenum].px,
    the_area.entranceheaders[m_entrancenum].py, 0);
  if(dlg.DoModal()==IDOK)
  {
    point=dlg.GetPoint(GP_POINT);
    the_area.entranceheaders[m_entrancenum].px=(short) point.x;
    the_area.entranceheaders[m_entrancenum].py=(short) point.y;
  }
  RefreshEntrance();
  UpdateData(UD_DISPLAY);
}

void CAreaEntrance::OnUnknown() 
{
	// TODO: Add your control notification handler code here
	
	UpdateData(UD_DISPLAY);	
}

BOOL CAreaEntrance::PreTranslateMessage(MSG* pMsg) 
{
  m_tooltip.RelayEvent(pMsg);
	return CPropertyPage::PreTranslateMessage(pMsg);
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
IDC_FLAG5,IDC_U28, IDC_U2C, IDC_FLAGS,IDC_U90, IDC_SCHEDULE, IDC_UNKNOWN,
IDC_MAXWAV, IDC_SET,IDC_COPY, IDC_PASTE, IDC_REMOVE,
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
    for(i=0;i<5;i++)
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
  //tooltips
  {
    m_tooltip.Create(this,TTS_NOPREFIX);
    m_tooltip.SetMaxTipWidth(200);
    m_tooltip.SetTipBkColor(RGB(240,224,160));

    m_tooltip.AddTool(GetDlgItem(IDC_AMBIENTPICKER), IDS_LABEL);
  }
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
	ON_BN_CLICKED(IDC_FLAG5, OnFlag5)
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
  newambients[the_area.ambientcount].flags|=1; //enable it
  newambients[the_area.ambientcount].schedule=0xffffff;
  newambients[the_area.ambientcount].volume=100;
  
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

  if(SetupSelectPoint(0)) return;
  dlg.InitView(IW_SHOWGRID|IW_ENABLEBUTTON|IW_PLACEIMAGE, &the_mos);
   //the cursors are loaded in 'the_bam' now
  dlg.SetupAnimationPlacement(&the_bam, the_area.ambientheaders[m_ambientnum].posx,
    the_area.ambientheaders[m_ambientnum].posy, 0);
  if(dlg.DoModal()==IDOK)
  {
    point=dlg.GetPoint(GP_POINT);
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
    if(the_area.ambientheaders[m_ambientnum].schedule!=dlg.m_schedule)
    {
      the_area.m_changed=true;
    }
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
	
	UpdateData(UD_DISPLAY);	
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

void CAreaAmbient::OnFlag5() 
{
	the_area.ambientheaders[m_ambientnum].flags^=16;
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

void CAreaAmbient::OnPlay() 
{
  CString tmpstr;

  UpdateData(UD_RETRIEVE);
	if(!the_area.ambientheaders[m_ambientnum].ambients[m_wavnum]) return;
  RetrieveResref(tmpstr, the_area.ambientheaders[m_ambientnum].ambients[m_wavnum]);
  play_acm(tmpstr,false, false);
}

BOOL CAreaAmbient::PreTranslateMessage(MSG* pMsg) 
{
  m_tooltip.RelayEvent(pMsg);
	return CPropertyPage::PreTranslateMessage(pMsg);
}

/////////////////////////////////////////////////////////////////////////////
// CAreaContainer dialog

CAreaContainer::CAreaContainer() : CPropertyPage(CAreaContainer::IDD)
{
	//{{AFX_DATA_INIT(CAreaContainer)
	//}}AFX_DATA_INIT
  m_containernum=-1;
  memset(&containercopy,0,sizeof(containercopy));
  vertexcopy=NULL;
  vertexsize=0;
  itemcopy=NULL;
  itemsize=0;
}

CAreaContainer::~CAreaContainer()
{
  if(vertexcopy) delete [] vertexcopy;
  vertexcopy=NULL;
  if(itemcopy) delete [] itemcopy;
  itemcopy=NULL;
}

static int itemflagids[]={IDC_IDENTIFIED,IDC_NOSTEAL,IDC_STOLEN,IDC_NODROP};

static int containerboxids[]={IDC_CONTAINERPICKER,IDC_POSX,IDC_POSY, IDC_TYPE,
IDC_LOCKED,IDC_DIFF,IDC_KEY,IDC_TRAPPED, IDC_DETECT,IDC_REMOVAL,IDC_DETECTED,
IDC_TPOSX, IDC_TPOSY, IDC_SCRIPT, IDC_POS1X, IDC_POS1Y, IDC_POS2X, IDC_POS2Y,
IDC_ITEMNUMPICKER,IDC_MAXITEM,IDC_ITEMRES,IDC_USE1,IDC_USE2,IDC_USE3,IDC_FLAGS,
IDC_UNKNOWN80,IDC_UNKNOWN56,IDC_STRREF, IDC_TEXT,IDC_RECALCBOX, IDC_EDITPOLYGON,
IDC_SCRIPTNAME,IDC_UNKNOWN8,IDC_IDENTIFIED,IDC_NOSTEAL,IDC_STOLEN,IDC_NODROP,
IDC_BROWSE, IDC_BROWSE2,IDC_BROWSE3,IDC_ADDITEM,IDC_DELITEM,IDC_UNKNOWN,
IDC_COPY, IDC_PASTE, IDC_REMOVE, IDC_SET, IDC_SELECTION,IDC_HIDDEN, IDC_NOPC,
IDC_TRESET, IDC_FIT,
0};

//all except additem
static int itemboxids[]={IDC_IDENTIFIED,IDC_NOSTEAL,IDC_STOLEN,IDC_NODROP,
IDC_BROWSE,IDC_DELITEM, IDC_ITEMNUMPICKER,IDC_ITEMRES,IDC_USE1,IDC_USE2,IDC_USE3,
IDC_FLAGS,IDC_UNKNOWN8,
-1};
#pragma warning(disable:4706)   
void CAreaContainer::DoDataExchange(CDataExchange* pDX)
{
  CWnd *cb;
  CButton *cb2;
  CString tmpstr;
  int flg, flg2;
  int i,j;
  int id;
  int itemnum;

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAreaContainer)
	DDX_Control(pDX, IDC_ITEMNUMPICKER, m_itemnumpicker);
	DDX_Control(pDX, IDC_TYPE, m_containertype_control);
	DDX_Control(pDX, IDC_CONTAINERPICKER, m_containerpicker);
	//}}AFX_DATA_MAP
  cb=GetDlgItem(IDC_MAXCONTAINER);
  flg=m_containernum>=0;
  if(flg) flg2=!!the_area.containerheaders[m_containernum].itemcount;
  else flg2=false;
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
      cb->EnableWindow(flg);
    }
    i++;
  }
  if(flg)
  {
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

    DDX_Text(pDX, IDC_UNKNOWN56, the_area.containerheaders[m_containernum].unknown56);
    DDX_Text(pDX, IDC_UNKNOWN80, the_area.containerheaders[m_containernum].unknown80);

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
    for(i=0;i<4;i++)
    {
      cb2=(CButton *) GetDlgItem(itemflagids[i]);
      if(cb2)
      {
        cb2->SetCheck(!!(the_area.itemheaders[itemnum].flags&j) );
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

int lockids[8]={IDC_HIDDEN, 0, 0, IDC_TRESET, 0, IDC_NOPC,0,0};

void CAreaContainer::RefreshContainer()
{
  CString tmpstr;
	CSchedule tmp;
  int i,j;
  CButton *cb;

  if(m_containernum<0 || m_containernum>=the_area.containercount)
  {
    if(the_area.containercount) m_containernum=0;
    else m_containernum=-1;
  }
  if(IsWindow(m_containerpicker) )
  {
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
      j=1;
      for(i=0;i<8;i++)
      {
        cb=(CButton *) GetDlgItem(lockids[i]);
        if(cb) cb->SetCheck(the_area.containerheaders[m_containernum].locked&j);
        j<<=1;
      }      
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
  //tooltips
  {
    m_tooltip.Create(this,TTS_NOPREFIX);
    m_tooltip.SetMaxTipWidth(200);
    m_tooltip.SetTipBkColor(RGB(240,224,160));

    m_tooltip.AddTool(GetDlgItem(IDC_CONTAINERPICKER), IDS_LABEL);
  }
  UpdateData(UD_DISPLAY);
	return TRUE;
}

BEGIN_MESSAGE_MAP(CAreaContainer, CPropertyPage)
	//{{AFX_MSG_MAP(CAreaContainer)
	ON_CBN_KILLFOCUS(IDC_CONTAINERPICKER, OnKillfocusContainerpicker)
	ON_CBN_SELCHANGE(IDC_CONTAINERPICKER, OnSelchangeContainerpicker)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_COPY, OnCopy)
	ON_BN_CLICKED(IDC_PASTE, OnPaste)
	ON_BN_CLICKED(IDC_IDENTIFIED, OnIdentified)
	ON_BN_CLICKED(IDC_NOSTEAL, OnNosteal)
	ON_BN_CLICKED(IDC_STOLEN, OnStolen)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_CBN_KILLFOCUS(IDC_ITEMNUMPICKER, OnKillfocusItemnumpicker)
	ON_CBN_SELCHANGE(IDC_ITEMNUMPICKER, OnSelchangeItemnumpicker)
	ON_BN_CLICKED(IDC_ADDITEM, OnAdditem)
	ON_BN_CLICKED(IDC_DELITEM, OnDelitem)
	ON_BN_CLICKED(IDC_RECALCBOX, OnRecalcbox)
	ON_EN_KILLFOCUS(IDC_STRREF, OnKillfocusStrref)
	ON_EN_KILLFOCUS(IDC_TEXT, OnKillfocusText)
	ON_BN_CLICKED(IDC_BROWSE2, OnBrowse2)
	ON_BN_CLICKED(IDC_BROWSE3, OnBrowse3)
	ON_BN_CLICKED(IDC_HIDDEN, OnHidden)
	ON_BN_CLICKED(IDC_NOPC, OnNopc)
	ON_BN_CLICKED(IDC_SET, OnSet)
	ON_BN_CLICKED(IDC_EDITPOLYGON, OnEditpolygon)
	ON_BN_CLICKED(IDC_NODROP, OnNodrop)
	ON_BN_CLICKED(IDC_SELECTION, OnSelection)
	ON_BN_CLICKED(IDC_FIT, OnFit)
	ON_BN_CLICKED(IDC_TRESET, OnTreset)
	ON_EN_KILLFOCUS(IDC_POSX, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_POSY, DefaultKillfocus)
	ON_CBN_KILLFOCUS(IDC_TYPE, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_DIFF, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_ITEMRES, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_FLAGS, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_USE1, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_USE2, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_USE3, DefaultKillfocus)
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
	ON_EN_KILLFOCUS(IDC_UNKNOWN8, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_UNKNOWN56, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_UNKNOWN80, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_SCRIPTNAME, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_KEY, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_DETECTED, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_LOCKED, OnKillfocusLocked)
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

void CAreaContainer::OnKillfocusLocked() 
{
  UpdateData(UD_RETRIEVE);
  RefreshContainer();
  UpdateData(UD_DISPLAY);	
}

void CAreaContainer::OnHidden() 
{
	the_area.containerheaders[m_containernum].locked^=1;
  RefreshContainer();
	UpdateData(UD_DISPLAY);
}

void CAreaContainer::OnTreset() 
{
	the_area.containerheaders[m_containernum].locked^=8;
  RefreshContainer();
	UpdateData(UD_DISPLAY);
}

void CAreaContainer::OnNopc() 
{
	the_area.containerheaders[m_containernum].locked^=32;
  RefreshContainer();
	UpdateData(UD_DISPLAY);
}

void CAreaContainer::OnEditpolygon() 
{
	CPolygon dlg;
	
  if(m_containernum<0) return;
  dlg.SetPolygon(MT_CONTAINER,the_area.triggercount+m_containernum, &the_area.containerheaders[m_containernum].vertexcount, &the_area.containerheaders[m_containernum].p1x);
  dlg.DoModal();
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
  the_area.itemheaders[itemnum].flags^=STI_IDENTIFIED;
  UpdateData(UD_DISPLAY);	
}

void CAreaContainer::OnNosteal() 
{
  int itemnum;
	
  itemnum=the_area.containerheaders[m_containernum].firstitem+m_itemnum;
  the_area.itemheaders[itemnum].flags^=STI_NOSTEAL;
  UpdateData(UD_DISPLAY);	
}

void CAreaContainer::OnStolen() 
{
  int itemnum;
	
  itemnum=the_area.containerheaders[m_containernum].firstitem+m_itemnum;
  the_area.itemheaders[itemnum].flags^=STI_STOLEN;
  UpdateData(UD_DISPLAY);	
}

void CAreaContainer::OnNodrop() 
{
  int itemnum;
	
  itemnum=the_area.containerheaders[m_containernum].firstitem+m_itemnum;
  the_area.itemheaders[itemnum].flags^=STI_NODROP;
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
  //this is to avoid crashes due to bad areas
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
  //this is strange, but i couldn't do it simpler
  //beware of any combination of door/trigger/container counts
  if(the_area.doorcount)
  {
    //we should insert our stuff before the first door
    the_area.vertexheaderlist.InsertBefore(the_area.vertexheaderlist.FindIndex(the_area.triggercount+count), newvertex);
  }
  else
  {
    //if there is no door, then we insert after the last element
    the_area.vertexheaderlist.InsertAfter(NULL, newvertex);
  }

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

bool CAreaContainer::removeitems(int cnum)
{
  area_item *newitems;
  int first, count, i;

  count=the_area.containerheaders[cnum].itemcount;  
  if(count)
  {
    first=the_area.containerheaders[cnum].firstitem;
    the_area.header.itemcnt=(short) (the_area.header.itemcnt-count);
    newitems=new area_item[the_area.header.itemcnt];
    if(!newitems) //rollback fully
    {
      the_area.header.itemcnt=(short) (the_area.header.itemcnt+count);
      return true;
    }
    the_area.containerheaders[cnum].itemcount=0;
    memcpy(newitems, the_area.itemheaders, first * sizeof(area_item) );
    memcpy(newitems+first, the_area.itemheaders+first+count, (the_area.itemcount-first-count)*sizeof(area_item) );
    if(the_area.itemheaders) delete [] the_area.itemheaders;
    the_area.itemheaders=newitems;
    the_area.itemcount=the_area.header.itemcnt;

    for(i=0;i<the_area.containercount;i++)
    {
      if(the_area.containerheaders[i].firstitem>=first)
      {
        the_area.containerheaders[i].firstitem-=count;
      }
    }
  }
  return false;
}

void CAreaContainer::OnRemove() 
{
  area_container *newcontainers;

  if(m_containernum<0 || !the_area.header.containercnt) return;  
  newcontainers=new area_container[--the_area.header.containercnt];
  if(!newcontainers)
  {
    the_area.header.containercnt++;
    return;
  }
  bool failed = removeitems(m_containernum);
  if (failed)
  {
    the_area.header.containercnt++;
    return;
  }
//
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
    itemsize=the_area.containerheaders[m_containernum].itemcount;
    if(vertexcopy) delete [] vertexcopy;
    vertexcopy=new area_vertex[vertexsize];
    if(itemcopy) delete [] itemcopy;
    itemcopy=new area_item[itemsize];
    memcpy(&containercopy,the_area.containerheaders+m_containernum,sizeof(containercopy) );
    memcpy(vertexcopy,the_area.vertexheaderlist.GetAt(pos),vertexsize*sizeof(area_vertex) );
    memcpy(itemcopy,the_area.itemheaders+the_area.containerheaders[m_containernum].firstitem, itemsize*sizeof(area_item) );
  }
}

void CAreaContainer::OnPaste() 
{
  POSITION pos;
  area_vertex *poi;
  area_item *poi2;
  int vertexnum;

  if(m_containernum>=0)
  {
    vertexnum=the_area.triggercount+m_containernum;
    pos=the_area.vertexheaderlist.FindIndex(vertexnum);
    if(itemsize+the_area.itemcount>32767)
    {
      MessageBox("Too many items.","Area editor",MB_OK);
      return;
    }
    poi=new area_vertex[vertexsize];
    if(!poi)
    {
      return;
    }
    poi2=new area_item[itemsize+the_area.itemcount];
    if(!poi2)
    {
      delete poi;
      return;
    }
    bool failed = removeitems(m_containernum);
    if (failed)
    {
      delete poi;
      return;
    }
    if(poi)
    {
      memcpy(poi,vertexcopy,vertexsize*sizeof(area_vertex));
    }
    memcpy(poi2,the_area.itemheaders, the_area.itemcount*sizeof(area_item));
    memcpy(poi2+the_area.itemcount, itemcopy, itemsize*sizeof(area_item));
    the_area.itemcount+=itemsize;
    if(the_area.itemheaders)
    {
      delete [] the_area.itemheaders;
    }
    the_area.itemheaders=poi2;
    the_area.vertexheaderlist.SetAt(pos,poi); //this will also free the old element
    memcpy(the_area.containerheaders+m_containernum,&containercopy, sizeof(containercopy) );
    the_area.containerheaders[m_containernum].firstitem=the_area.header.itemcnt;
    the_area.header.itemcnt=(short) the_area.itemcount;
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


void CAreaContainer::OnSelection() 
{
  CImageView dlg;
  CPoint point;

  if(SetupSelectPoint(0)) return;
  dlg.SetMapType(MT_CONTAINER, (LPBYTE) &the_area.vertexheaderlist);
  dlg.m_max=the_area.containercount;
  dlg.m_value=m_containernum;
  dlg.InitView(IW_SHOWGRID|IW_ENABLEBUTTON|IW_GETPOLYGON, &the_mos);
	dlg.DoModal();
  m_containernum=dlg.m_value;
  RefreshContainer();
  UpdateData(UD_DISPLAY);
}

void CAreaContainer::OnSet() 
{
  CImageView dlg;
  CPoint point;

  if(SetupSelectPoint(0)) return;
  dlg.SetMapType(MT_CONTAINER, (LPBYTE) &the_area.vertexheaderlist);
  dlg.m_max=the_area.containercount;
  dlg.m_value=m_containernum;

  dlg.InitView(IW_SHOWGRID|IW_ENABLEBUTTON|IW_PLACEIMAGE, &the_mos);
  dlg.SetupAnimationPlacement(&the_bam,  //the cursors are loaded in 'the_bam' now
    the_area.containerheaders[m_containernum].posx,
    the_area.containerheaders[m_containernum].posy,0);
  if(dlg.DoModal()==IDOK)
  {
    point=dlg.GetPoint(GP_POINT);
    the_area.containerheaders[m_containernum].posx=(short) point.x;
    the_area.containerheaders[m_containernum].posy=(short) point.y;
  }
  RefreshContainer();
  UpdateData(UD_DISPLAY);
}

void CAreaContainer::OnFit() 
{
  CString tmpstr;
  int i, cnt;

  if(the_area.containercount>999)
  {
    MessageBox("Too many containers","Area editor",MB_OK);
    return;
  }
  
  for(i=0;i<the_area.header.itemcnt;i++)
  {
    for(cnt=0;cnt<the_area.containercount;cnt++)
    {
      if(CheckIntervallum(i,the_area.containerheaders[cnt].firstitem,the_area.containerheaders[cnt].itemcount)!=-1)
      {
        break;
      }
    }
    if(cnt==the_area.containercount)
    {
      OnAdd();
      tmpstr.Format("Reclaimed item %d",i);
      StoreVariable(tmpstr,the_area.containerheaders[cnt].containername);
      the_area.containerheaders[cnt].firstitem=i;
      the_area.containerheaders[cnt].itemcount=1;
    }
  }
  RefreshContainer();
  UpdateData(UD_DISPLAY);
}

BOOL CAreaContainer::PreTranslateMessage(MSG* pMsg) 
{
  m_tooltip.RelayEvent(pMsg);
	return CPropertyPage::PreTranslateMessage(pMsg);
}

/////////////////////////////////////////////////////////////////////////////
// CAreaVariable dialog

CAreaVariable::CAreaVariable() : CPropertyPage(CAreaVariable::IDD)
{
	//{{AFX_DATA_INIT(CAreaVariable)
	//}}AFX_DATA_INIT
  m_variablenum=-1;
  m_notenum=-1;
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
  CComboBox *cb;

	CPropertyPage::OnInitDialog();
  RefreshVariable();
  if(pst_compatible_var())
  {
    cb = (CComboBox *) GetDlgItem(IDC_COLOR);
    cb->ResetContent();
    cb->AddString("0 - User note");
    cb->AddString("1 - Read only");
  }
  //tooltips
  {
    m_tooltip.Create(this,TTS_NOPREFIX);
    m_tooltip.SetMaxTipWidth(200);
    m_tooltip.SetTipBkColor(RGB(240,224,160));
  }
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

  if(SetupSelectPoint(0)) return;
  dlg.InitView(IW_SHOWGRID|IW_ENABLEBUTTON|IW_PLACEIMAGE, &the_mos);
  dlg.SetupAnimationPlacement(&the_bam, the_area.mapnoteheaders[m_notenum].px,
    the_area.mapnoteheaders[m_notenum].py,0);
  if(dlg.DoModal()==IDOK)
  {
    point=dlg.GetPoint(GP_POINT);
    the_area.mapnoteheaders[m_notenum].px=(short) point.x;
    the_area.mapnoteheaders[m_notenum].py=(short) point.y;
  }
  RefreshVariable();
  UpdateData(UD_DISPLAY);
}

BOOL CAreaVariable::PreTranslateMessage(MSG* pMsg) 
{
  m_tooltip.RelayEvent(pMsg);
	return CPropertyPage::PreTranslateMessage(pMsg);
}

/////////////////////////////////////////////////////////////////////////////
// CAreaDoor dialog

CAreaDoor::CAreaDoor()	: CPropertyPage(CAreaDoor::IDD)
{
	//{{AFX_DATA_INIT(CAreaDoor)
	m_openclose = TRUE;
	//}}AFX_DATA_INIT
  m_doornum=-1;
  memset(&doorcopy,0,sizeof(doorcopy));
  for(int i=0;i<4;i++)
  {
    vertexcopy[i]=NULL;
    vertexsizes[i]=0;
  }
  hbd=NULL;
}

CAreaDoor::~CAreaDoor()
{
  for(int i=0;i<4;i++)
  {
    if (vertexcopy[i]) delete vertexcopy[i];
  }
  if(hbd) DeleteObject(hbd);
}

void CAreaDoor::RefreshDoor()
{
  CString tmpstr;
  int i;

  if(IsWindow(m_doorpicker) )
  {
//    m_doornum=m_doorpicker.GetCurSel();
    if(m_doornum<0) m_doornum=0;
    m_spin_control.SetRange32(0,the_bam.GetFrameCount()-1 );
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
  }
  if(IsWindow(m_regionpicker) )
  {
    m_regionpicker.ResetContent();
    for(i=0;i<the_area.triggercount;i++)
    {
      if(the_area.triggerheaders[i].triggertype==RT_TRAVEL)
      {
        tmpstr.Format("%-.16s",the_area.triggerheaders[i].infoname);
        m_regionpicker.AddString(tmpstr);
      }
    }
  }
}

static int doorboxids[]={IDC_DOORPICKER, IDC_DOORID, IDC_FLAGS, IDC_SET,
IDC_FLAG1,IDC_FLAG2,IDC_FLAG3,IDC_FLAG4,IDC_FLAG5,IDC_FLAG6,IDC_FLAG7,IDC_FLAG8,
IDC_FLAG9,IDC_FLAG10,IDC_FLAG11,IDC_FLAG12,
IDC_POS1X,IDC_POS1Y,IDC_POS2X,IDC_POS2Y, IDC_POS1X2,IDC_POS1Y2,IDC_POS2X2,IDC_POS2Y2,
IDC_RECALCBOX,IDC_EDITPOLYGON, IDC_EDITBLOCK,IDC_SELECTION,
IDC_OPEN,IDC_SOUND1,IDC_SOUND2, IDC_AREA, IDC_CURSORIDX, IDC_CURSOR, IDC_TPOSX, IDC_TPOSY,
IDC_DETECT,IDC_REMOVAL,IDC_U54, IDC_TRAPPED,IDC_DETECTED,IDC_KEY, IDC_SCRIPT,IDC_DIALOG,
IDC_LOCKED,IDC_DIFF,IDC_STRREF, IDC_INFOSTR, IDC_BROWSE,IDC_BROWSE2,IDC_BROWSE3, 
IDC_BROWSE4,IDC_BROWSE5, IDC_PLAY,IDC_PLAYSOUND, IDC_UNKNOWN, 
IDC_REMOVE, IDC_COPY, IDC_PASTE,
0};

#define DOORFLAGNUM  12

#pragma warning(disable:4706)   
void CAreaDoor::DoDataExchange(CDataExchange* pDX)
{
  CWnd *cb;
  CButton *cb2;
  CString tmpstr;
  int flg;
  int id;
  int i,j;
  int fc;

	CPropertyPage::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_CURSOR, m_cursoricon);
	//{{AFX_DATA_MAP(CAreaDoor)
	DDX_Control(pDX, IDC_AREA, m_regionpicker);
	DDX_Control(pDX, IDC_SPINCURSOR, m_spin_control);
	DDX_Control(pDX, IDC_DOORPICKER, m_doorpicker);
	DDX_Check(pDX, IDC_OPEN, m_openclose);
	//}}AFX_DATA_MAP
  flg=m_doornum>=0;
  i=0;
  while(id=doorboxids[i])
  {
    cb=GetDlgItem(id);
    cb->EnableWindow(flg);
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

    RetrieveResref(tmpstr, the_area.doorheaders[m_doornum].key);
    DDX_Text(pDX, IDC_KEY, tmpstr);
    DDV_MaxChars(pDX, tmpstr, 8);
    StoreResref(tmpstr, the_area.doorheaders[m_doornum].key);

    RetrieveResref(tmpstr, the_area.doorheaders[m_doornum].openscript);
    DDX_Text(pDX, IDC_SCRIPT, tmpstr);
    DDV_MaxChars(pDX, tmpstr, 8);
    StoreResref(tmpstr, the_area.doorheaders[m_doornum].openscript);

    RetrieveResref(tmpstr, the_area.doorheaders[m_doornum].dialogref);
    DDX_Text(pDX, IDC_DIALOG, tmpstr);
    DDV_MaxChars(pDX, tmpstr, 8);
    StoreResref(tmpstr, the_area.doorheaders[m_doornum].dialogref);

    RetrieveVariable(tmpstr, the_area.doorheaders[m_doornum].regionlink); //travel region link
    DDX_Text(pDX, IDC_AREA, tmpstr);
    DDV_MaxChars(pDX, tmpstr, 16);
    StoreResref16(tmpstr, the_area.doorheaders[m_doornum].regionlink);

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

    tmpstr.Format("%d",the_area.doorheaders[m_doornum].flags);
    DDX_Text(pDX, IDC_FLAGS, tmpstr);
    the_area.doorheaders[m_doornum].flags=strtonum(tmpstr);
    j=1;
    for(i=0;i<DOORFLAGNUM;i++)
    {
      cb2=(CButton *) GetDlgItem(IDC_FLAG1+i);
      if(the_area.doorheaders[m_doornum].flags&j) cb2->SetCheck(true);
      else cb2->SetCheck(false);
      j<<=1;
    }
    DDX_Text(pDX, IDC_POS1X2, the_area.doorheaders[m_doornum].locp1x);
    DDX_Text(pDX, IDC_POS1Y2, the_area.doorheaders[m_doornum].locp1y);
    DDX_Text(pDX, IDC_POS2X2, the_area.doorheaders[m_doornum].locp2x);
    DDX_Text(pDX, IDC_POS2Y2, the_area.doorheaders[m_doornum].locp2y);

    if(m_openclose) //open
    {
      DDX_Text(pDX, IDC_POS1X, the_area.doorheaders[m_doornum].op1x);
      DDX_Text(pDX, IDC_POS1Y, the_area.doorheaders[m_doornum].op1y);
      DDX_Text(pDX, IDC_POS2X, the_area.doorheaders[m_doornum].op2x);
      DDX_Text(pDX, IDC_POS2Y, the_area.doorheaders[m_doornum].op2y);
      tmpstr="Open";
    }
    else //open
    {
      DDX_Text(pDX, IDC_POS1X, the_area.doorheaders[m_doornum].cp1x);
      DDX_Text(pDX, IDC_POS1Y, the_area.doorheaders[m_doornum].cp1y);
      DDX_Text(pDX, IDC_POS2X, the_area.doorheaders[m_doornum].cp2x);
      DDX_Text(pDX, IDC_POS2Y, the_area.doorheaders[m_doornum].cp2y);
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
  //tooltips
  {
    m_tooltip.Create(this,TTS_NOPREFIX);
    m_tooltip.SetMaxTipWidth(200);
    m_tooltip.SetTipBkColor(RGB(240,224,160));

    m_tooltip.AddTool(GetDlgItem(IDC_DOORPICKER), IDS_LABEL);
  }
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
	ON_BN_CLICKED(IDC_OPEN, OnOpenClose)
	ON_BN_CLICKED(IDC_RECALCBOX, OnRecalcbox)
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
	ON_BN_CLICKED(IDC_FLAG9, OnFlag9)
	ON_BN_CLICKED(IDC_FLAG10, OnFlag10)
	ON_BN_CLICKED(IDC_FLAG11, OnFlag11)
	ON_BN_CLICKED(IDC_FLAG12, OnFlag12)
	ON_EN_KILLFOCUS(IDC_AREA, OnKillfocusArea)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_BROWSE2, OnBrowse2)
	ON_BN_CLICKED(IDC_PLAY, OnPlay)
	ON_BN_CLICKED(IDC_PLAYSOUND, OnPlaysound)
	ON_BN_CLICKED(IDC_BROWSE3, OnBrowse3)
	ON_BN_CLICKED(IDC_BROWSE4, OnBrowse4)
	ON_BN_CLICKED(IDC_UNKNOWN, OnUnknown)
	ON_BN_CLICKED(IDC_BROWSE5, OnBrowse5)
	ON_EN_CHANGE(IDC_CURSORIDX, OnChangeCursoridx)
	ON_BN_CLICKED(IDC_SET, OnSet)
	ON_BN_CLICKED(IDC_EDITBLOCK, OnEditblock)
	ON_BN_CLICKED(IDC_EDITPOLYGON, OnEditpolygon)
	ON_BN_CLICKED(IDC_SET2, OnSet2)
	ON_BN_CLICKED(IDC_SET3, OnSet3)
	ON_BN_CLICKED(IDC_SELECTION, OnSelection)
	ON_EN_KILLFOCUS(IDC_DOORID, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_FLAGS, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U54, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_CURSORIDX, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_SOUND1, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_SOUND2, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_POS1X, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_POS1Y, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_POS2X, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_POS2Y, DefaultKillfocus)
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
	ON_EN_KILLFOCUS(IDC_POS1X2, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_POS1Y2, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_POS2X2, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_POS2Y2, DefaultKillfocus)
	ON_CBN_KILLFOCUS(IDC_AREA, DefaultKillfocus)
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

void CAreaDoor::OnOpenClose() 
{
  m_openclose=!m_openclose;
  the_mos.m_drawclosed=!m_openclose;
	RefreshDoor();
	UpdateData(UD_DISPLAY);
}

//editing the impeded blocks of a door
//the blocks are listed in a polygon list
void CAreaDoor::OnEditblock() 
{
	CImageView dlg;
  POSITION pos;
  area_vertex *poi;
  int openclose;
  int vertexsize, tmpsize;
  unsigned int x,y,maxx,maxy;
  unsigned int minx,miny;

  if(SetupSelectPoint(0)) return;
  the_mos.m_drawclosed=!m_openclose;
  maxx=the_area.m_width/GR_WIDTH;
  maxy=(the_area.m_height+GR_HEIGHT-1)/GR_HEIGHT;
  unsigned char *bitmap = new unsigned char[maxx*maxy];
  if(!bitmap)
  {
    MessageBox("Out of memory","Error",MB_ICONSTOP|MB_OK);
    return;
  }
  memset(bitmap,0,maxx*maxy); //unsigned char
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
  //getting the pointer for the right polygon
  poi=(area_vertex *) the_area.vertexheaderlist.GetAt(pos);  
  if(!poi)
  {
    delete [] bitmap;
    return;
  }
  tmpsize=vertexsize;
  if(tmpsize)
  {
    minx=maxx;
    miny=maxy;
    while(tmpsize--)
    {
      x=poi[tmpsize].x;
      y=poi[tmpsize].y;
      if(x>=maxx || y>maxy) continue; //don't set this
      bitmap[y*maxx+x]=1;
      if(minx>x) minx=x;
      if(miny>y) miny=y;
    }
  }
  else
  {    
    //there are 2 opening locations
    minx=the_area.doorheaders[m_doornum].locp1x/GR_WIDTH;
    miny=the_area.doorheaders[m_doornum].locp1y/GR_HEIGHT;
    if(minx>maxx)
    {
      minx=the_area.doorheaders[m_doornum].locp2x/GR_WIDTH;
      miny=the_area.doorheaders[m_doornum].locp2y/GR_HEIGHT;
    }
    if(minx>maxx)
    {
      minx=miny=0;
    }
  }
  dlg.SetMapType(MT_BLOCK, bitmap);
  dlg.InitView(IW_ENABLEBUTTON|IW_EDITMAP|IW_SHOWALL|IW_SHOWGRID|IW_ENABLEFILL, &the_mos);
  dlg.m_clipx=minx*GR_WIDTH/the_mos.mosheader.dwBlockSize-dlg.m_maxextentx/2;
  dlg.m_clipy=miny*GR_HEIGHT/the_mos.mosheader.dwBlockSize-dlg.m_maxextentx/2;

  dlg.m_value=1;
  dlg.DoModal();
  //calculating the new polygon size
  tmpsize=0;
  for(x=0;x<maxx;x++) for(y=0;y<maxy;y++)
  {
    if(bitmap[y*maxx+x])
    {
      tmpsize++;
      if(tmpsize==10000) goto da_break1;
    }
  }
da_break1:
  //reallocating the polygon if size has changed
  if(tmpsize!=vertexsize)
  {
    //no need to delete the old pointer, SetAt does this favour for us
    poi = new area_vertex[tmpsize];
    the_area.vertexheaderlist.SetAt(pos, poi);
  }
  tmpsize=0;
  for(x=0;x<maxx;x++) for(y=0;y<maxy;y++)
  {
    if(bitmap[y*maxx+x])
    {
      poi[tmpsize].x=(short) x;
      poi[tmpsize++].y=(short) y;
      if(tmpsize==10000)
      {
        MessageBox("Only the first 10000 impeded blocks were stored.","Area editor",MB_ICONINFORMATION|MB_OK);
        goto da_break2;
      }
    }
  }
da_break2:
  delete [] bitmap;
  if(m_openclose)    
  {
    the_area.doorheaders[m_doornum].countblockopen=(short) tmpsize;
  }
  else
  {
    the_area.doorheaders[m_doornum].countblockclose=(short) tmpsize;
  }
  RefreshDoor();
  UpdateData(UD_DISPLAY);
}

void CAreaDoor::OnEditpolygon() 
{
	CPolygon dlg;
  int openclose;

  if(m_doornum<0) return;
  if(m_openclose)    
  {
    openclose=m_doornum*4;
    dlg.SetPolygon(MT_DOOR,the_area.triggercount+the_area.containercount+openclose, 
      &the_area.doorheaders[m_doornum].countvertexopen, &the_area.doorheaders[m_doornum].op1x);
  }
  else
  {
    openclose=m_doornum*4+1;   
    dlg.SetPolygon(MT_DOOR,the_area.triggercount+the_area.containercount+openclose,
      &the_area.doorheaders[m_doornum].countvertexclose, &the_area.doorheaders[m_doornum].cp1x);
  }
  dlg.m_open=m_openclose;
  dlg.DoModal();
	UpdateData(UD_DISPLAY);
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

int CAreaDoor::AddWedDoor(CString doorid)
{
  int ret;

  if(!the_area.WedAvailable())
  {
    MessageBox("There is no wedfile!","Area editor",MB_ICONSTOP|MB_OK);
    return 2;
  }

  ret=the_area.AddWedDoor(doorid);
  if(ret==1)
  {
    MessageBox("This door already exists in the wedfile.","Area editor",MB_ICONINFORMATION|MB_OK);
  }
  return ret;
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
  newdoors[the_area.doorcount].cursortype=30; //door
  newdoors[the_area.doorcount].firstvertexopen=the_area.vertexcount;
  newdoors[the_area.doorcount].firstvertexclose=the_area.vertexcount;
  newdoors[the_area.doorcount].firstblockopen=the_area.vertexcount;
  newdoors[the_area.doorcount].firstblockclose=the_area.vertexcount;

  AddWedDoor(tmpstr);
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

void CAreaDoor::OnRemove() 
{
  area_door *newdoors;
  POSITION pos;
  int vertexnum;
  int i;

  if(m_doornum<0 || !the_area.header.doorcnt) return;

  if(!the_area.WedAvailable())
  {
    MessageBox("Cannot completely remove wed door, because the wed file isn't available.","Warning",MB_ICONEXCLAMATION|MB_OK);
  }
  else
  {
    switch(the_area.RemoveWedDoor(the_area.doorheaders[m_doornum].doorid) )
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
    vertexsizes[0]=the_area.doorheaders[m_doornum].countvertexopen;
    vertexsizes[1]=the_area.doorheaders[m_doornum].countvertexclose;
    vertexsizes[2]=the_area.doorheaders[m_doornum].countblockopen;
    vertexsizes[3]=the_area.doorheaders[m_doornum].countblockclose;
    memcpy(&doorcopy,the_area.doorheaders+m_doornum,sizeof(doorcopy) );
    vertexnum=the_area.triggercount+the_area.containercount+m_doornum*4;
    for(i=0;i<4;i++)
    {
      pos=the_area.vertexheaderlist.FindIndex(vertexnum+i);
      if(vertexcopy[i])
      {
        delete [] vertexcopy[i];
      }
      vertexcopy[i]=new area_vertex[vertexsizes[i]];
      memcpy(vertexcopy[i],the_area.vertexheaderlist.GetAt(pos),vertexsizes[i]*sizeof(area_vertex) );
    }
  }
}

void CAreaDoor::OnPaste() 
{
  area_vertex *poi;
  POSITION pos;
  int vertexnum;
  int i;

  if(m_doornum>=0)
  {
    vertexnum=the_area.triggercount+the_area.containercount+m_doornum*4;
    for(i=0;i<4;i++)
    {
      pos=the_area.vertexheaderlist.FindIndex(vertexnum+i);
      poi=new area_vertex[vertexsizes[i]];
      if(poi)
      {
        memcpy(poi,vertexcopy[i],vertexsizes[i]*sizeof(area_vertex));
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
  RetrieveResref(pickerdlg.m_picked,the_area.doorheaders[m_doornum].dialogref);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_area.doorheaders[m_doornum].dialogref);
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

void CAreaDoor::OnSet() 
{
  CImageView dlg;
  CPoint point;

  if(SetupSelectPoint(0)) return;
  dlg.InitView(IW_SHOWGRID|IW_ENABLEBUTTON|IW_PLACEIMAGE, &the_mos);
  dlg.SetupAnimationPlacement(&the_bam,  //the cursors are loaded in 'the_bam' now
    the_area.doorheaders[m_doornum].launchx,
    the_area.doorheaders[m_doornum].launchy,0);
  if(dlg.DoModal()==IDOK)
  {
    point=dlg.GetPoint(GP_POINT);
    the_area.doorheaders[m_doornum].launchx=(short) point.x;
    the_area.doorheaders[m_doornum].launchy=(short) point.y;
  }
  RefreshDoor();
  UpdateData(UD_DISPLAY);
}

void CAreaDoor::OnSet2() 
{
  CImageView dlg;
  CPoint point;

  if(SetupSelectPoint(0)) return;
  dlg.InitView(IW_SHOWGRID|IW_ENABLEBUTTON|IW_PLACEIMAGE, &the_mos);
  dlg.SetupAnimationPlacement(&the_bam,  //the cursors are loaded in 'the_bam' now
    the_area.doorheaders[m_doornum].locp1x,
    the_area.doorheaders[m_doornum].locp1y,0);
  if(dlg.DoModal()==IDOK)
  {
    point=dlg.GetPoint(GP_POINT);
    the_area.doorheaders[m_doornum].locp1x=(short) point.x;
    the_area.doorheaders[m_doornum].locp1y=(short) point.y;
    if (!the_area.doorheaders[m_doornum].locp2x && !the_area.doorheaders[m_doornum].locp2y)
    {
      the_area.doorheaders[m_doornum].locp2x=(short) point.x;
      the_area.doorheaders[m_doornum].locp2y=(short) point.y;
    }
  }
  RefreshDoor();
  UpdateData(UD_DISPLAY);
}

void CAreaDoor::OnSet3() 
{
  CImageView dlg;
  CPoint point;

  if(SetupSelectPoint(0)) return;
  dlg.InitView(IW_SHOWGRID|IW_ENABLEBUTTON|IW_PLACEIMAGE, &the_mos);
  dlg.SetupAnimationPlacement(&the_bam,  //the cursors are loaded in 'the_bam' now
    the_area.doorheaders[m_doornum].locp2x,
    the_area.doorheaders[m_doornum].locp2y,0);
  if(dlg.DoModal()==IDOK)
  {
    point=dlg.GetPoint(GP_POINT);
    the_area.doorheaders[m_doornum].locp2x=(short) point.x;
    the_area.doorheaders[m_doornum].locp2y=(short) point.y;
    if (!the_area.doorheaders[m_doornum].locp1x && !the_area.doorheaders[m_doornum].locp1y)
    {
      the_area.doorheaders[m_doornum].locp1x=(short) point.x;
      the_area.doorheaders[m_doornum].locp1y=(short) point.y;
    }
  }
  RefreshDoor();
  UpdateData(UD_DISPLAY);
}

void CAreaDoor::OnSelection() 
{
  CImageView dlg;
  CPoint point;

  if(SetupSelectPoint(0)) return;
  dlg.SetMapType(MT_DOOR, (LPBYTE) &the_area.vertexheaderlist);
  dlg.m_max=the_area.doorcount*2;
  dlg.m_value=m_doornum*2+!m_openclose;
  dlg.InitView(IW_SHOWGRID|IW_ENABLEBUTTON|IW_GETPOLYGON, &the_mos);
  dlg.DoModal();
  m_doornum=dlg.m_value/2;
  m_openclose=!(dlg.m_value&1);
  RefreshDoor();
  UpdateData(UD_DISPLAY);
}
void CAreaDoor::OnUnknown() 
{
	// TODO: Add your control notification handler code here
	
	UpdateData(UD_DISPLAY);	
}

BOOL CAreaDoor::PreTranslateMessage(MSG* pMsg) 
{
  m_tooltip.RelayEvent(pMsg);
	return CPropertyPage::PreTranslateMessage(pMsg);
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
  hbanim = NULL;
  hbma = NULL;
}

CAreaAnim::~CAreaAnim()
{
  if(hbanim) DeleteObject(hbanim);
  if(hbma) DeleteObject(hbma);
}

static int animboxids[]={IDC_ANIMPICKER, IDC_POSX, IDC_POSY, IDC_BAM,
IDC_FRAME, IDC_CYCLE, IDC_FLAGS, IDC_U36, IDC_U38, IDC_TRANSPARENT, IDC_U3C,
IDC_CHANCE, IDC_SKIPEXT, IDC_BMP, IDC_FLAG1, IDC_FLAG2, IDC_FLAG3,
IDC_FLAG4, IDC_FLAG5, IDC_FLAG6, IDC_FLAG7, IDC_FLAG8, IDC_FLAG9,
IDC_FLAG10, IDC_FLAG11, IDC_FLAG12, IDC_FLAG13, IDC_FLAG14, IDC_FLAG15,
IDC_FLAG16, IDC_BAMFRAME, IDC_PLAY, IDC_SCHEDULE, IDC_REMOVE, IDC_COPY,
IDC_PASTE, IDC_BROWSE, IDC_BROWSE2, IDC_FIT,
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
    DDX_Text(pDX, IDC_U38,the_area.animheaders[m_animnum].height);
		DDX_Text(pDX, IDC_TRANSPARENT,the_area.animheaders[m_animnum].transparency);
		DDX_Text(pDX, IDC_U3C,the_area.animheaders[m_animnum].current);
    DDX_Text(pDX, IDC_CHANCE,the_area.animheaders[m_animnum].progress);
    DDX_Text(pDX, IDC_SKIPEXT,the_area.animheaders[m_animnum].skipcycle);

    RetrieveResref(tmpstr, the_area.animheaders[m_animnum].bmp);
    DDX_Text(pDX, IDC_BMP,tmpstr);
    DDV_MaxChars(pDX, tmpstr, 8);
    StoreResref(tmpstr, the_area.animheaders[m_animnum].bmp);

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
  //tooltips
  {
    m_tooltip.Create(this,TTS_NOPREFIX);
    m_tooltip.SetMaxTipWidth(200);
    m_tooltip.SetTipBkColor(RGB(240,224,160));

    m_tooltip.AddTool(GetDlgItem(IDC_ANIMPICKER), IDS_LABEL);
    m_tooltip.AddTool(GetDlgItem(IDC_FLAG1), IDS_AVISIBLE);
    m_tooltip.AddTool(GetDlgItem(IDC_FLAG2), IDS_ABLEND);
    m_tooltip.AddTool(GetDlgItem(IDC_FLAG3), IDS_ANOLIGHT);
    m_tooltip.AddTool(GetDlgItem(IDC_FLAG4), IDS_APARTIAL);
    m_tooltip.AddTool(GetDlgItem(IDC_FLAG5), IDS_ASYNC);
    m_tooltip.AddTool(GetDlgItem(IDC_FLAG7), IDS_ANOWALL);
    m_tooltip.AddTool(GetDlgItem(IDC_FLAG8), IDS_AFOG);
    m_tooltip.AddTool(GetDlgItem(IDC_FLAG9), IDS_GROUND);
    m_tooltip.AddTool(GetDlgItem(IDC_FLAG10), IDS_AALL);
    m_tooltip.AddTool(GetDlgItem(IDC_FLAG11), IDS_APALETTE);
    m_tooltip.AddTool(GetDlgItem(IDC_BMP), IDS_APALETTE);
    m_tooltip.AddTool(GetDlgItem(IDC_BROWSE2), IDS_APALETTE);
    m_tooltip.AddTool(GetDlgItem(IDC_FLAG12), IDS_AMIRROR);
    m_tooltip.AddTool(GetDlgItem(IDC_FLAG13), IDS_ACOMBAT);
    m_tooltip.AddTool(GetDlgItem(IDC_FLAG14), IDS_CHANCE);
    m_tooltip.AddTool(GetDlgItem(IDC_TRANSPARENT), IDS_ATRANSP);
  }
	return TRUE;  
}

BEGIN_MESSAGE_MAP(CAreaAnim, CPropertyPage)
	//{{AFX_MSG_MAP(CAreaAnim)
	ON_EN_KILLFOCUS(IDC_FRAME, OnKillfocusFrame)
	ON_EN_KILLFOCUS(IDC_CYCLE, OnKillfocusCycle)
	ON_CBN_KILLFOCUS(IDC_ANIMPICKER, OnKillfocusAnimpicker)
	ON_CBN_SELCHANGE(IDC_ANIMPICKER, OnSelchangeAnimpicker)
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
	ON_EN_KILLFOCUS(IDC_POSX, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_POSY, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_BAM, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_FLAGS, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U36, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U38, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U3C, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_BMP, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_TRANSPARENT, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_SKIPEXT, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_CHANCE, DefaultKillfocus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAreaAnim message handlers

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
    if(the_area.animheaders[m_animnum].schedule!=dlg.m_schedule)
    {
      the_area.m_changed=true;
    }
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
  newanims[the_area.animcount].flags|=1;
  newanims[the_area.animcount].schedule=0xffffff;
  
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

  if(SetupSelectPoint(0)) return;
  dlg.InitView(IW_SHOWGRID|IW_ENABLEBUTTON|IW_PLACEIMAGE|IW_MATCH, &the_mos);
  dlg.SetupAnimationPlacement(&the_anim,
    the_area.animheaders[m_animnum].posx,
    the_area.animheaders[m_animnum].posy,0);
  if(dlg.DoModal()==IDOK)
  {
    point=dlg.GetPoint(GP_POINT);
    the_area.animheaders[m_animnum].posx=(short) point.x;
    the_area.animheaders[m_animnum].posy=(short) point.y;
  }
  RefreshAnim();
  UpdateData(UD_DISPLAY);
}

BOOL CAreaAnim::PreTranslateMessage(MSG* pMsg) 
{
  m_tooltip.RelayEvent(pMsg);
	return CPropertyPage::PreTranslateMessage(pMsg);
}

/////////////////////////////////////////////////////////////////////////////
// CAreaMap dialog

CAreaMap::CAreaMap()	: CPropertyPage(CAreaMap::IDD)
{
	//{{AFX_DATA_INIT(CAreaMap)
	//}}AFX_DATA_INIT
  bgcolor=RGB(32,32,32);
  m_set = 0;
  hbmap = NULL;
  the_map = NULL;
  the_palette = NULL;
  m_function=0;
  m_adjust=0;
  if(the_area.m_smallpalette) m_maptype=0;
  else m_maptype=4;
}

CAreaMap::~CAreaMap()
{
  if(hbmap) DeleteObject(hbmap); 
}

void CAreaMap::RefreshMap()
{
  CWnd *cw;

  if(!IsWindow(m_hWnd)) return;
  cw=GetDlgItem(IDC_NIGHTMAP);
  if(IsWindow(cw->m_hWnd))
  {
    cw->EnableWindow(!iwd2_structures() && the_area.WedAvailable()&&(the_area.header.areatype&EXTENDED_NIGHT) );
  }
  if(the_area.m_smallpalette) m_maptype=0;
  else m_maptype=4;
}

BOOL CAreaMap::OnInitDialog() 
{
  CRect tmprect;

	CPropertyPage::OnInitDialog();
	RefreshMap();
  GetDlgItem(IDC_MAP)->GetWindowRect(tmprect);
  m_oladjust=tmprect.TopLeft();
  ScreenToClient(&m_oladjust);
  //tooltips
  {
    m_tooltip.Create(this,TTS_NOPREFIX);
    m_tooltip.SetMaxTipWidth(200);
    m_tooltip.SetTipBkColor(RGB(240,224,160));
  }
  UpdateData(UD_DISPLAY);
	return TRUE;
}

static int maptypes[5]={MT_HEIGHT,MT_LIGHT, MT_LIGHT,MT_SEARCH,MT_HEIGHT8};

void CAreaMap::ResetCombo()
{
  int i,max;
  int maptype;

  maptype=maptypes[m_maptype];
  if(maptype==MT_LIGHT || maptype==MT_HEIGHT8) max=256;
  else max=16;
  m_value_control.ResetContent();
  for(i=0;i<max;i++)
  {
    m_value_control.AddString(GetMapTypeValue(maptype,i));
  }
}

void CAreaMap::DoDataExchange(CDataExchange* pDX)
{
  CString tmpstr;

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAreaMap)
	DDX_Control(pDX, IDC_SPECIAL, m_special_control);
	DDX_Control(pDX, IDC_VALUE, m_value_control);
	DDX_Control(pDX, IDC_MAP, m_bitmap);
	DDX_Radio(pDX, IDC_HEIGHTMAP, m_maptype);
	//}}AFX_DATA_MAP
  switch(m_maptype)
  {
  case 0:
    the_map=the_area.heightmap;
    the_palette=the_area.htpal;
    m_special_control.SetWindowText("");
    m_special_control.EnableWindow(false);
    break;
  case 4: //height 8 bits
    the_map=the_area.heightmap;
    the_palette=the_area.ht8pal;
    m_special_control.SetWindowText("");
    m_special_control.EnableWindow(false);
    break;
  case 1:
    the_area.m_night=false;
    the_map=the_area.lightmap;
    the_palette=the_area.lmpal;
    //not finished
    m_special_control.SetWindowText("Generate lightmap");
    m_special_control.EnableWindow(false);
    break;
  case 2:
    the_area.m_night=true;
    the_map=the_area.lightmap;
    the_palette=the_area.lmpal;
    //not finished
    m_special_control.SetWindowText("Generate nightmap");
    m_special_control.EnableWindow(false);
    break;
  case 3:
    the_map=the_area.searchmap;
    the_palette=the_area.srpal;
    m_special_control.SetWindowText("Add travel regions");
    m_special_control.EnableWindow(true);
    break;
  default:
    the_map=NULL;
    the_palette=NULL;
  }
  tmpstr=GetMapTypeValue(maptypes[m_maptype],m_set);
  if(pDX->m_bSaveAndValidate==UD_DISPLAY) ResetCombo();
  DDX_Text(pDX, IDC_VALUE, tmpstr);
  m_set=strtonum(tmpstr);

  if(the_map)
  {
    int w, h;

    w=the_area.m_width/GR_WIDTH;
    h=(the_area.m_height+GR_HEIGHT-1)/GR_HEIGHT;
    DDX_Text(pDX, IDC_WIDTH,w);
    DDX_Text(pDX, IDC_HEIGHT,h);
    if (MakeBitmapExternal(the_map, the_palette, w, h, hbmap))
    {
      m_bitmap.SetBitmap(hbmap);
    }
  }
  else m_bitmap.SetBitmap(0);
}

BEGIN_MESSAGE_MAP(CAreaMap, CPropertyPage)
	//{{AFX_MSG_MAP(CAreaMap)
	ON_BN_CLICKED(IDC_HEIGHTMAP, OnHeightmap)
	ON_BN_CLICKED(IDC_LIGHTMAP, OnLightmap)
	ON_BN_CLICKED(IDC_SEARCHMAP, OnSearchmap)
	ON_CBN_KILLFOCUS(IDC_VALUE, OnDefaultKillfocus)
	ON_BN_CLICKED(IDC_CLEAR, OnClear)
	ON_BN_CLICKED(IDC_SET, OnSet)
	ON_BN_CLICKED(IDC_INIT, OnInit)
	ON_BN_CLICKED(IDC_MAP, OnMap)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	ON_BN_CLICKED(IDC_UNDO, OnUndo)
	ON_BN_CLICKED(IDC_PALETTE, OnPalette)
	ON_BN_CLICKED(IDC_SPECIAL, OnSpecial)
	ON_COMMAND(ID_REFRESH, OnRefresh)
	ON_BN_CLICKED(IDC_NIGHTMAP, OnNightmap)
	ON_BN_CLICKED(IDC_HEIGHTMAP2, OnHeightmap2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAreaMap message handlers

void CAreaMap::OnHeightmap() 
{
  the_area.m_smallpalette=true;
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CAreaMap::OnHeightmap2() 
{
  the_area.m_smallpalette=false;
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CAreaMap::OnLightmap() 
{
  if(GetLightMap(false)) m_maptype=2;
  else UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CAreaMap::OnNightmap() 
{
  if(GetLightMap(true)) m_maptype=1;
  else UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CAreaMap::OnSearchmap() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CAreaMap::OnDefaultKillfocus() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CAreaMap::Allocatemap(bool allocate)
{
  if(allocate)
  {
    int size=(the_area.m_width/GR_WIDTH)*((the_area.m_height+GR_HEIGHT-1)/GR_HEIGHT);
    the_map=new BYTE[size];
  }
  int maptype = maptypes[m_maptype];
  if(maptype==MT_HEIGHT8) maptype=MT_HEIGHT;
  switch(m_maptype)
  {
  case 0: case 4:
    the_area.heightmap = the_map;
    break;
  case 1:
    the_area.m_night=false;
    the_area.lightmap = the_map;
    break;
  case 2:
    the_area.m_night=true;
    the_area.lightmap = the_map;
    break;
  case 3:
    the_area.searchmap = the_map;
    break;
  }
  the_area.changedmap[maptype]=TRUE;
}

void CAreaMap::OnClear() 
{
  UpdateData(UD_RETRIEVE);
  if(the_map) delete [] the_map;
	Allocatemap(true);
  memset(the_map,0,(the_area.m_width/GR_WIDTH)*((the_area.m_height+GR_HEIGHT-1)/GR_HEIGHT) );
  int maptype = maptypes[m_maptype];
  if(maptype==MT_HEIGHT8) maptype=MT_HEIGHT;
  the_area.changedmap[maptype]=TRUE;
	UpdateData(UD_DISPLAY);
}

void CAreaMap::OnSet() 
{
  UpdateData(UD_RETRIEVE);
	if(!the_map) Allocatemap(true);
  if(maptypes[m_maptype]!=MT_LIGHT)
  {
    if(m_set>15) m_set=15;
  }
	memset(the_map,m_set,(the_area.m_width/GR_WIDTH)*((the_area.m_height+GR_HEIGHT-1)/GR_HEIGHT) );
  int maptype = maptypes[m_maptype];
  if(maptype==MT_HEIGHT8) maptype=MT_HEIGHT;
  the_area.changedmap[maptype]=TRUE;
	UpdateData(UD_DISPLAY);
}

static COLORREF srpalette[16]={0,RGB(0,0,128), RGB(0,128,0), RGB(0,128,128),
RGB(128,0,0),RGB(128,0,128),RGB(128,128,0), RGB(192,192,192),
RGB(128,128,128),RGB(0,0,255),RGB(0,255,0),RGB(0,255,255),
RGB(255,0,0),RGB(255,0,255),RGB(255,255,0), RGB(255,255,255)
};

void CAreaMap::OnInit() 
{
  int i;
  int maptype;

	UpdateData(UD_RETRIEVE);
  maptype=maptypes[m_maptype];
  switch(maptype)
  {
  case MT_HEIGHT8:
    //FIXME: this is not correct, but something
    for(i=0;i<256;i++)
    {
      the_palette[i]=RGB(i, i, i);
    }
    break;
  case MT_HEIGHT:
    for(i=0;i<16;i++)
    {
      the_palette[i]=RGB(i*15, i*15, i*15);
    }
    break;
  case MT_LIGHT:
    for(i=0;i<256;i++)
    {
      the_palette[255-i]=RGB((i&15)*15, (i&15)*15, (i&15)*15);
    }
    for(i=16;i<32;i++)
    {
      the_palette[i]|=0xff;
    }
    for(;i<48;i++)
    {
      the_palette[i]|=0xff00;
    }
    for(;i<64;i++)
    {
      the_palette[i]|=0xff0000;
    }
    for(;i<80;i++)
    {
      the_palette[i]|=0xffff;
    }
    for(;i<96;i++)
    {
      the_palette[i]|=0xffff00;
    }
    for(;i<112;i++)
    {
      the_palette[i]|=0xff00ff;
    }
    for(;i<128;i++)
    {
      the_palette[i]&=0xff;
    }
    for(;i<144;i++)
    {
      the_palette[i]&=0xff00;
    }
    for(;i<160;i++)
    {
      the_palette[i]&=0xff0000;
    }
    break;
  case MT_SEARCH:
    memcpy(the_palette, srpalette, sizeof(srpalette) );
    break;
  }
  if(maptype==MT_HEIGHT8)
  {
    the_area.changedmap[MT_HEIGHT]=TRUE;
  }
  else
  {
    the_area.changedmap[maptype]=TRUE;
  }
	UpdateData(UD_DISPLAY);
}

void CAreaMap::OnPalette() 
{
	CPaletteEdit dlg;

  if(maptypes[m_maptype]!=MT_LIGHT && maptypes[m_maptype]!=MT_HEIGHT8)
  {
    dlg.m_max=16;
  }  
  dlg.SetPalette(the_palette, m_function, m_adjust);
  if(dlg.DoModal()==IDOK)
  {
    m_function=dlg.m_function;
    m_adjust=dlg.GetAdjustment();
  }
}

void CAreaMap::OnMap() 
{
  CImageView dlg;
  CPoint point;

  if(SetupSelectPoint(0)) return;
  dlg.m_value=m_set;
  dlg.SetMapType(maptypes[m_maptype], the_map);

  dlg.InitView(IW_ENABLEBUTTON|IW_EDITMAP|IW_SHOWGRID|IW_ENABLEFILL|IW_SHOWALL, &the_mos);
  dlg.m_clipx=m_mousepoint.x*GR_WIDTH/the_mos.mosheader.dwBlockSize-dlg.m_maxextentx/2;
  dlg.m_clipy=m_mousepoint.y*GR_HEIGHT/the_mos.mosheader.dwBlockSize-dlg.m_maxextenty/2;
  dlg.DoModal();
  m_set=dlg.m_value;
	UpdateData(UD_DISPLAY);
}

void CAreaMap::OnEdit() 
{
  CImageView dlg;
  CPoint point;

  if(SetupSelectPoint(0)) return;
  dlg.m_value=m_set;
  dlg.SetMapType(maptypes[m_maptype], the_map);

  dlg.InitView(IW_ENABLEBUTTON|IW_EDITMAP|IW_SHOWGRID|IW_ENABLEFILL|IW_SHOWALL, &the_mos);
  dlg.DoModal();
  m_set=dlg.m_value;
  UpdateData(UD_DISPLAY);
}

void CAreaMap::OnRefresh() 
{
}

void CAreaMap::OnUndo() 
{ 
  the_area.ReadMap("TMP",the_map, the_palette, maptypes[m_maptype]==MT_LIGHT?256*4:16*4);
  Allocatemap(false);
  UpdateData(UD_DISPLAY);
}

void CAreaMap::AddTravelRegions()
{
  int i,j;
  int width, height;

  width=the_area.m_width/GR_WIDTH;
  height=(the_area.m_height+GR_HEIGHT-1)/GR_HEIGHT;
  for(i=0;i<3;i++)
  {
    for(j=0;j<width;j++)
    {
      the_map[j+i*width]=14;
      the_map[(height-i-1)*width+j]=14;
    }
  }
  for(i=0;i<3;i++)
  {
    for(j=0;j<height;j++)
    {
      if(j<=3 || j>=height-3)
      {
        the_map[j*width+i]=8;
        the_map[j*width+width-i-1]=8;
      }
      else
      {
        the_map[j*width+i]=14;
        the_map[j*width+width-i-1]=14;
      }
    }
  }
}

void CAreaMap::OnSpecial() 
{
  if(!the_map) Allocatemap(true);
	switch(maptypes[m_maptype])
  {
  case MT_LIGHT:
    //generate lightmap
    break;
  case MT_HEIGHT:
    break;
  case MT_SEARCH:
    AddTravelRegions();
    break;
  }	
  UpdateData(UD_DISPLAY);
}

BOOL CAreaMap::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->hwnd==m_bitmap.m_hWnd) 
  {
    switch(pMsg->message)
    {
    case WM_RBUTTONDOWN:
      if(!the_map) break;
      m_set=the_map[(the_area.m_width/GR_WIDTH)*m_mousepoint.y+m_mousepoint.x];
      UpdateData(UD_DISPLAY);
      break;
    case WM_MOUSEMOVE:
      m_mousepoint=pMsg->pt;
      ScreenToClient(&m_mousepoint);
      m_mousepoint-=m_oladjust;
      break;
    }
  }
  m_tooltip.RelayEvent(pMsg);
	return CPropertyPage::PreTranslateMessage(pMsg);
}


/////////////////////////////////////////////////////////////////////////////
// CAreaProj dialog

void CAreaProj::RefreshProj()
{
  CString tmpstr;
  int offset;
  int i;

  if(m_trapnum<0 || m_trapnum>=the_area.header.trapcnt)
  {
    if(the_area.header.trapcnt) m_trapnum=0;
    else m_trapnum=-1;
  }
  if(IsWindow(m_trappicker) )
  {
    m_trappicker.ResetContent();
    for(i=0;i<the_area.trapcount;i++)
    {
      tmpstr.Format("%d. %-.8s",i+1,the_area.trapheaders[i].projectile);
      m_trappicker.AddString(tmpstr);
    }
    m_trapnum=m_trappicker.SetCurSel(m_trapnum);
  }

  offset=0;
  for(i=0;i<the_area.trapcount;i++)
  {
    the_area.trapheaders[i].offset=offset;
    offset+=the_area.trapheaders[i].size/0x108;
  }

  if(m_effect_control && (m_trapnum>=0) )
  {
    m_effect_control.ResetContent();
    offset=the_area.trapheaders[m_trapnum].offset;
    for(i=0;i<the_area.trapheaders[m_trapnum].size/0x108;i++)
    {
      //
      if(i+offset>=the_area.effectcount) 
      {
        MessageBox("Something is inconsistent here!\n",MB_OK);
        break;
      }

      tmpstr.Format("%d %s (0x%x 0x%x) %.8s",i+1,
        get_opcode_text(the_area.effects[i+offset].feature),
        the_area.effects[i+offset].par1.parl,
        the_area.effects[i+offset].par2.parl,
        the_area.effects[i+offset].vvc);      
      m_effect_control.AddString(tmpstr);
    }
    if(effectnum<0 || effectnum>=i) effectnum=0;
    effectnum=m_effect_control.SetCurSel(effectnum);
  }
}

BOOL CAreaProj::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	RefreshProj();
  //tooltips
  {
    m_tooltip.Create(this,TTS_NOPREFIX);
    m_tooltip.SetMaxTipWidth(200);
    m_tooltip.SetTipBkColor(RGB(240,224,160));
  }
  UpdateData(UD_DISPLAY);
	return TRUE;
}

BEGIN_MESSAGE_MAP(CAreaProj, CPropertyPage)
	//{{AFX_MSG_MAP(CAreaProj)
	ON_CBN_KILLFOCUS(IDC_PROJPICKER, OnKillfocusProjpicker)
	ON_CBN_SELCHANGE(IDC_PROJPICKER, OnSelchangeProjpicker)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CAreaProj::CAreaProj()	: CPropertyPage(CAreaProj::IDD)
{
	//{{AFX_DATA_INIT(CAreaProj)

	//}}AFX_DATA_INIT
}

CAreaProj::~CAreaProj()
{
}

static int trapboxids[]={
  //fields
  IDC_BAM,IDC_PROJECTILE,IDC_POSX,IDC_POSY,IDC_UNKNOWN10,
  IDC_UNKNOWN18,IDC_UNKNOWN1A,
  IDC_EQUIPNUM, 
  //buttons
  IDC_REMOVE, IDC_REMOVE2, IDC_ADD2, 0};

void CAreaProj::DoDataExchange(CDataExchange* pDX)
{
  CWnd *cb;
  CString tmpstr;
  int flg;
  int i;

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAreaProj)
	DDX_Control(pDX, IDC_PROJPICKER, m_trappicker);
  DDX_Control(pDX, IDC_EQUIPNUM, m_effect_control);
	//}}AFX_DATA_MAP
  cb=GetDlgItem(IDC_MAX);
  flg=m_trapnum>=0;
  tmpstr.Format("/ %d",the_area.trapcount);
  cb->SetWindowText(tmpstr);
  i=0;
  while(trapboxids[i])
  {
    cb=GetDlgItem(trapboxids[i++]);
    cb->EnableWindow(flg);
  }
  if(flg)
  {
    DDX_Text(pDX, IDC_PROJECTILE, the_area.trapheaders[m_trapnum].proj);
    DDX_Text(pDX, IDC_POSX, the_area.trapheaders[m_trapnum].posx);
    DDX_Text(pDX, IDC_POSY, the_area.trapheaders[m_trapnum].posy);
    RetrieveResref(tmpstr, the_area.trapheaders[m_trapnum].projectile);
    DDX_Text(pDX, IDC_BAM, tmpstr);
    DDV_MaxChars(pDX, tmpstr, 8);
    StoreResref(tmpstr, the_area.trapheaders[m_trapnum].projectile);
    DDX_Text(pDX, IDC_UNKNOWN10, the_area.trapheaders[m_trapnum].unknown10);
    DDX_Text(pDX, IDC_UNKNOWN18, the_area.trapheaders[m_trapnum].unknown18);
    DDX_Text(pDX, IDC_UNKNOWN1A, the_area.trapheaders[m_trapnum].unknown1a);
  }
}

void CAreaProj::OnSelchangeProjpicker() 
{
  int x;

  x=m_trappicker.GetCurSel();
  if(x>=0 && x<=the_area.trapcount) m_trapnum=x;
  RefreshProj();
	UpdateData(UD_DISPLAY);	
}

void CAreaProj::OnKillfocusProjpicker() 
{
  CString tmpstr;
  int x;

  m_trappicker.GetWindowText(tmpstr);
  x=strtonum(tmpstr)-1;
  if(x>=0 && x<=the_area.trapcount)
  {
    m_trapnum=m_trappicker.SetCurSel(x);
  }
  RefreshProj();
	UpdateData(UD_DISPLAY);	
}

void CAreaProj::OnBrowse() 
{
  if(m_trapnum<0) return;
  pickerdlg.m_restype=REF_PRO;
  RetrieveResref(pickerdlg.m_picked,the_area.trapheaders[m_trapnum].projectile);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_area.trapheaders[m_trapnum].projectile);
  }
  RefreshProj();
	UpdateData(UD_DISPLAY);	
}

void CAreaProj::OnAdd() 
{
	// TODO: Add your control notification handler code here
	
}

BOOL CAreaProj::PreTranslateMessage(MSG* pMsg) 
{
  m_tooltip.RelayEvent(pMsg);
	return CPropertyPage::PreTranslateMessage(pMsg);
}

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
  AddPage(&m_PageProj);
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
  m_PageProj.RefreshProj();
  page=GetActivePage();
  page->UpdateData(UD_DISPLAY);
}

BEGIN_MESSAGE_MAP(CAreaPropertySheet, CPropertySheet)
//{{AFX_MSG_MAP(CAreaPropertySheet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

