// Schedule.cpp : implementation file
//

#include "stdafx.h"
#include "chitem.h"
#include "Schedule.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSchedule dialog

CSchedule::CSchedule(CWnd* pParent /*=NULL*/)
	: CDialog(CSchedule::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSchedule)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
  m_schedule=0xffffff;
}

void CSchedule::DoDataExchange(CDataExchange* pDX)
{
  int i,j;
  CButton *cb;

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSchedule)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
  j=1;
  for(i=0;i<24;i++)
  {
    cb=(CButton *) GetDlgItem(IDC_HOUR1+i);
    if(m_schedule&j)
    {
      cb->SetCheck(true);
    }
    else
    {
      cb->SetCheck(false);
    }
    j<<=1;
  }
}


BEGIN_MESSAGE_MAP(CSchedule, CDialog)
	//{{AFX_MSG_MAP(CSchedule)
	ON_BN_CLICKED(IDC_NIGHT, OnNight)
	ON_BN_CLICKED(IDC_TWILIGHT, OnTwilight)
	ON_BN_CLICKED(IDC_NONE, OnNone)
	ON_BN_CLICKED(IDC_ALL, OnAll)
	ON_BN_CLICKED(IDC_DAWN, OnDawn)
	ON_BN_CLICKED(IDC_DAY, OnDay)
	ON_BN_CLICKED(IDC_MORNING, OnMorning)
	ON_BN_CLICKED(IDC_AFTERNOON, OnAfternoon)
	ON_BN_CLICKED(IDC_COPY, OnCopy)
	ON_BN_CLICKED(IDC_PASTE, OnPaste)
	ON_BN_CLICKED(IDC_HOUR1, OnHour1)
	ON_BN_CLICKED(IDC_HOUR2, OnHour2)
	ON_BN_CLICKED(IDC_HOUR3, OnHour3)
	ON_BN_CLICKED(IDC_HOUR4, OnHour4)
	ON_BN_CLICKED(IDC_HOUR5, OnHour5)
	ON_BN_CLICKED(IDC_HOUR6, OnHour6)
	ON_BN_CLICKED(IDC_HOUR7, OnHour7)
	ON_BN_CLICKED(IDC_HOUR8, OnHour8)
	ON_BN_CLICKED(IDC_HOUR9, OnHour9)
	ON_BN_CLICKED(IDC_HOUR10, OnHour10)
	ON_BN_CLICKED(IDC_HOUR11, OnHour11)
	ON_BN_CLICKED(IDC_HOUR12, OnHour12)
	ON_BN_CLICKED(IDC_HOUR13, OnHour13)
	ON_BN_CLICKED(IDC_HOUR14, OnHour14)
	ON_BN_CLICKED(IDC_HOUR15, OnHour15)
	ON_BN_CLICKED(IDC_HOUR16, OnHour16)
	ON_BN_CLICKED(IDC_HOUR17, OnHour17)
	ON_BN_CLICKED(IDC_HOUR18, OnHour18)
	ON_BN_CLICKED(IDC_HOUR19, OnHour19)
	ON_BN_CLICKED(IDC_HOUR20, OnHour20)
	ON_BN_CLICKED(IDC_HOUR21, OnHour21)
	ON_BN_CLICKED(IDC_HOUR22, OnHour22)
	ON_BN_CLICKED(IDC_HOUR23, OnHour23)
	ON_BN_CLICKED(IDC_HOUR24, OnHour24)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSchedule message handlers

#define TOD_NIGHT     0xfe001f
#define TOD_TWILIGHT  0xfe0000
#define TOD_NEVER     0
#define TOD_ALWAYS    0xffffff
#define TOD_DAWN      0x1f
#define TOD_DAY       0x1ffe0
#define TOD_MORNING   0xfe0
#define TOD_AFTERNOON 0x1f000

void CSchedule::OnNight() 
{
	m_schedule=TOD_NIGHT;
  UpdateData(UD_DISPLAY);
}

void CSchedule::OnTwilight() 
{
	m_schedule=TOD_TWILIGHT;
  UpdateData(UD_DISPLAY);
}

void CSchedule::OnNone() 
{
	m_schedule=TOD_NEVER;
  UpdateData(UD_DISPLAY);
}

void CSchedule::OnAll() 
{
	m_schedule=TOD_ALWAYS;
  UpdateData(UD_DISPLAY);
}

void CSchedule::OnDawn() 
{
	m_schedule=TOD_DAWN;
  UpdateData(UD_DISPLAY);
}

void CSchedule::OnDay() 
{
  m_schedule=TOD_DAY;
  UpdateData(UD_DISPLAY);
}

void CSchedule::OnMorning() 
{
	m_schedule=TOD_MORNING;
  UpdateData(UD_DISPLAY);
}

void CSchedule::OnAfternoon() 
{
	m_schedule=TOD_AFTERNOON;
  UpdateData(UD_DISPLAY);
}

void CSchedule::OnCopy() 
{
  m_default=m_schedule;	
}

void CSchedule::OnPaste() 
{
	m_schedule=m_default;
  UpdateData(UD_DISPLAY);
}

void CSchedule::OnHour1() 
{
  m_schedule^=1;
  UpdateData(UD_DISPLAY);
}

void CSchedule::OnHour2() 
{
  m_schedule^=2;
  UpdateData(UD_DISPLAY);
}

void CSchedule::OnHour3() 
{
  m_schedule^=4;
  UpdateData(UD_DISPLAY);
}

void CSchedule::OnHour4() 
{
  m_schedule^=8;
  UpdateData(UD_DISPLAY);
}

void CSchedule::OnHour5() 
{
  m_schedule^=0x10;
  UpdateData(UD_DISPLAY);
}

void CSchedule::OnHour6() 
{
  m_schedule^=0x20;
  UpdateData(UD_DISPLAY);
}

void CSchedule::OnHour7() 
{
  m_schedule^=0x40;
  UpdateData(UD_DISPLAY);
}

void CSchedule::OnHour8() 
{
  m_schedule^=0x80;
  UpdateData(UD_DISPLAY);
}

void CSchedule::OnHour9() 
{
  m_schedule^=0x100;
  UpdateData(UD_DISPLAY);
}

void CSchedule::OnHour10() 
{
  m_schedule^=0x200;
  UpdateData(UD_DISPLAY);
}

void CSchedule::OnHour11() 
{
  m_schedule^=0x400;
  UpdateData(UD_DISPLAY);
}

void CSchedule::OnHour12() 
{
  m_schedule^=0x800;
  UpdateData(UD_DISPLAY);
}

void CSchedule::OnHour13() 
{
  m_schedule^=0x1000;
  UpdateData(UD_DISPLAY);
}

void CSchedule::OnHour14() 
{
  m_schedule^=0x2000;
  UpdateData(UD_DISPLAY);
}

void CSchedule::OnHour15() 
{
  m_schedule^=0x4000;
  UpdateData(UD_DISPLAY);
}

void CSchedule::OnHour16() 
{
  m_schedule^=0x8000;
  UpdateData(UD_DISPLAY);
}

void CSchedule::OnHour17() 
{
  m_schedule^=0x10000;
  UpdateData(UD_DISPLAY);
}

void CSchedule::OnHour18() 
{
  m_schedule^=0x20000;
  UpdateData(UD_DISPLAY);
}

void CSchedule::OnHour19() 
{
  m_schedule^=0x40000;
  UpdateData(UD_DISPLAY);
}

void CSchedule::OnHour20() 
{
  m_schedule^=0x80000;
  UpdateData(UD_DISPLAY);
}

void CSchedule::OnHour21() 
{
  m_schedule^=0x100000;
  UpdateData(UD_DISPLAY);
}

void CSchedule::OnHour22() 
{
  m_schedule^=0x200000;
  UpdateData(UD_DISPLAY);
}

void CSchedule::OnHour23() 
{
  m_schedule^=0x400000;
  UpdateData(UD_DISPLAY);
}

void CSchedule::OnHour24() 
{
  m_schedule^=0x800000;
  UpdateData(UD_DISPLAY);
}

CString CSchedule::GetCaption()
{
  int i,j;
  CString tmp;

  j=1;
  tmp.Format("%24s","");
  for(i=0;i<24;i++)
  {
    if(m_schedule&j)
    {
      tmp.SetAt(i,'x');
    }
    j<<=1;
  }
  return tmp;
}

CString CSchedule::GetTimeOfDay()
{
  switch(m_schedule&TOD_ALWAYS)
  {
  case TOD_NEVER: 
    return "Never";
    break;
  case TOD_ALWAYS:
    return "Always";
    break;
  case TOD_DAY:
    return "Day";
    break;
  case TOD_NIGHT:
    return "Night";
  case TOD_DAWN:
    return "Dawn";
  case TOD_TWILIGHT:
    return "Twilight";
  case TOD_MORNING:
    return "Morning";
  case TOD_AFTERNOON:
    return "Afternoon";
  default:
    return "Custom";
  }
}
