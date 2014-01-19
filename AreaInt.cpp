// AreaInt.cpp : implementation file
//

#include "stdafx.h"
#include "chitem.h"
#include "AreaInt.h"
#include "options.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAreaInt dialog

CAreaInt::CAreaInt(CWnd* pParent /*=NULL*/)
	: CDialog(CAreaInt::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAreaInt)
	m_text = _T("");
	//}}AFX_DATA_INIT
  m_crenum=0;
}

void CAreaInt::RefreshInt()
{
  CString tmpstr;
  int i;

  m_spawnnumpicker.ResetContent();
  for(i=0;i<the_area.intheader.creaturecnt;i++)
  {
    tmpstr.Format("%d %-.8s",i+1,the_area.intheader.creatures[i]);
    m_spawnnumpicker.AddString(tmpstr);
  }
  m_spawnnumpicker.SetCurSel(m_crenum);
  m_text=resolve_tlk_text(the_area.intheader.strrefs[m_crenum]);
}

BOOL CAreaInt::OnInitDialog() 
{
	CDialog::OnInitDialog();
	RefreshInt();
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

void CAreaInt::DoDataExchange(CDataExchange* pDX)
{
  CString tmpstr;

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAreaInt)
	DDX_Control(pDX, IDC_SPAWNNUMPICKER, m_spawnnumpicker);
	DDX_Text(pDX, IDC_TEXT, m_text);
	//}}AFX_DATA_MAP

  RetrieveResref(tmpstr,the_area.header.sleep1);
  DDX_Text(pDX, IDC_DAYMOVIE, tmpstr);
  DDV_MaxChars(pDX, tmpstr, 8);
  StoreResref(tmpstr,the_area.header.sleep1);

  RetrieveResref(tmpstr,the_area.header.sleep2);
  DDX_Text(pDX, IDC_NIGHTMOVIE, tmpstr);
  DDV_MaxChars(pDX, tmpstr, 8);
  StoreResref(tmpstr,the_area.header.sleep2);

  RetrieveVariable(tmpstr,the_area.intheader.intname);
  DDX_Text(pDX, IDC_SECTION, tmpstr);
  DDV_MaxChars(pDX, tmpstr, 32);
  StoreVariable(tmpstr,the_area.intheader.intname);

  tmpstr.Format("/ %d", the_area.intheader.creaturecnt);
  DDX_Text(pDX, IDC_MAXCRE, tmpstr);
  RetrieveResref(tmpstr,the_area.intheader.creatures[m_crenum]);
  DDX_Text(pDX, IDC_CRERES, tmpstr);
  DDV_MaxChars(pDX, tmpstr, 8);
  StoreResref(tmpstr,the_area.intheader.creatures[m_crenum]);
  DDX_Text(pDX, IDC_STRREF, the_area.intheader.strrefs[m_crenum]);

  DDX_Text(pDX, IDC_UNKNOWN9A, the_area.intheader.difficulty);
  DDX_Text(pDX, IDC_UNKNOWN9C, the_area.intheader.lifespan);
  DDX_Text(pDX, IDC_UNKNOWNA0, the_area.intheader.huntingrange);
  DDX_Text(pDX, IDC_UNKNOWNA2, the_area.intheader.followrange);
  DDX_Text(pDX, IDC_MAX, the_area.intheader.maxnumber);
  DDX_Text(pDX, IDC_MIN, the_area.intheader.activated);
  DDX_Text(pDX, IDC_DAY, the_area.intheader.day);
  DDX_Text(pDX, IDC_NIGHT, the_area.intheader.night);
  DDX_Text(pDX, IDC_UNKNOWNAC, the_area.intheader.unknownac);
  DDX_Text(pDX, IDC_UNKNOWNAE, the_area.intheader.unknownae);
}

BEGIN_MESSAGE_MAP(CAreaInt, CDialog)
	//{{AFX_MSG_MAP(CAreaInt)
	ON_EN_KILLFOCUS(IDC_SECTION, OnKillfocusSection)
	ON_EN_KILLFOCUS(IDC_MIN, OnKillfocusMin)
	ON_EN_KILLFOCUS(IDC_MAX, OnKillfocusMax)
	ON_EN_KILLFOCUS(IDC_CRERES, OnKillfocusCreres)
	ON_EN_KILLFOCUS(IDC_STRREF, OnKillfocusStrref)
	ON_EN_KILLFOCUS(IDC_TEXT, OnKillfocusText)
	ON_EN_KILLFOCUS(IDC_UNKNOWN9A, OnKillfocusUnknown9a)
	ON_EN_KILLFOCUS(IDC_UNKNOWN9C, OnKillfocusUnknown9c)
	ON_EN_KILLFOCUS(IDC_UNKNOWNA0, OnKillfocusUnknowna0)
	ON_EN_KILLFOCUS(IDC_UNKNOWNA2, OnKillfocusUnknowna2)
	ON_EN_KILLFOCUS(IDC_DAY, OnKillfocusDay)
	ON_EN_KILLFOCUS(IDC_NIGHT, OnKillfocusNight)
	ON_CBN_KILLFOCUS(IDC_SPAWNNUMPICKER, OnKillfocusSpawnnumpicker)
	ON_CBN_SELCHANGE(IDC_SPAWNNUMPICKER, OnSelchangeSpawnnumpicker)
	ON_EN_KILLFOCUS(IDC_UNKNOWNAC, OnKillfocusUnknownac)
	ON_EN_KILLFOCUS(IDC_UNKNOWNAE, OnKillfocusUnknownae)
	ON_EN_KILLFOCUS(IDC_DAYMOVIE, OnKillfocusDaymovie)
	ON_EN_KILLFOCUS(IDC_NIGHTMOVIE, OnKillfocusNightmovie)
	ON_BN_CLICKED(IDC_CLEAR, OnClear)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_BROWSE2, OnBrowse2)
	ON_BN_CLICKED(IDC_BROWSE3, OnBrowse3)
	ON_BN_CLICKED(IDC_ADDCRE, OnAddcre)
	ON_BN_CLICKED(IDC_DELCRE, OnDelcre)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAreaInt message handlers

void CAreaInt::OnKillfocusSection() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}


void CAreaInt::OnKillfocusMin() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CAreaInt::OnKillfocusMax() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CAreaInt::OnKillfocusCreres() 
{
	UpdateData(UD_RETRIEVE);
  RefreshInt();
	UpdateData(UD_DISPLAY);
}

void CAreaInt::OnKillfocusStrref() 
{
	UpdateData(UD_RETRIEVE);
  RefreshInt();
	UpdateData(UD_DISPLAY);
}

void CAreaInt::OnKillfocusText() 
{
  CString tmpstr;

	UpdateData(UD_RETRIEVE);
  tmpstr=resolve_tlk_text(the_area.intheader.strrefs[m_crenum]);
  if(tmpstr!=m_text)
  {
    if(editflg&TLKCHANGE)
    {
      if(MessageBox("Do you want to update dialog.tlk?","Area editor",MB_YESNO)!=IDYES)
      {
        return;
      }      
    }
    the_area.intheader.strrefs[m_crenum]=store_tlk_text(the_area.intheader.strrefs[m_crenum],m_text);
  }
	UpdateData(UD_DISPLAY);
}

void CAreaInt::OnKillfocusUnknown9a() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CAreaInt::OnKillfocusUnknown9c() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CAreaInt::OnKillfocusUnknowna0() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CAreaInt::OnKillfocusUnknowna2() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CAreaInt::OnKillfocusDay() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CAreaInt::OnKillfocusNight() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CAreaInt::OnKillfocusUnknownac() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CAreaInt::OnKillfocusUnknownae() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CAreaInt::OnKillfocusDaymovie() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CAreaInt::OnKillfocusNightmovie() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}


void CAreaInt::OnBrowse() 
{
  if(m_crenum<0) return;
  pickerdlg.m_restype=REF_CRE;
  RetrieveResref(pickerdlg.m_picked,the_area.intheader.creatures[m_crenum]);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_area.intheader.creatures[m_crenum]);
  }
  RefreshInt();
	UpdateData(UD_DISPLAY);	
}

void CAreaInt::OnBrowse2() 
{
  pickerdlg.m_restype=REF_MVE;
  RetrieveResref(pickerdlg.m_picked,the_area.header.sleep1);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_area.header.sleep1);
  }
	UpdateData(UD_DISPLAY);	
}

void CAreaInt::OnBrowse3() 
{
  pickerdlg.m_restype=REF_MVE;
  RetrieveResref(pickerdlg.m_picked,the_area.header.sleep2);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_area.header.sleep2);
  }
	UpdateData(UD_DISPLAY);	
}

void CAreaInt::OnAddcre() 
{
  if(the_area.intheader.creaturecnt<10)
  {
    the_area.intheader.creaturecnt++;
  }
  RefreshInt();
	UpdateData(UD_DISPLAY);
}

void CAreaInt::OnDelcre() 
{
  if(m_crenum>=0 && m_crenum<=9)
  {
    memcpy(the_area.intheader.creatures+m_crenum,
           the_area.intheader.creatures+m_crenum+1,
           (9-m_crenum)*8);
    memset(the_area.intheader.creatures[9],0,8);
  }
  the_area.intheader.creaturecnt--;
  m_crenum=the_area.intheader.creaturecnt-1;
  RefreshInt();
	UpdateData(UD_DISPLAY);
}

void CAreaInt::OnKillfocusSpawnnumpicker() 
{
	// TODO: Add your control notification handler code here
  CString tmpstr;
  int x;

  m_spawnnumpicker.GetWindowText(tmpstr);
  x=strtonum(tmpstr)-1;
  if(x>=0 && x<=the_area.intheader.creaturecnt)
  {
    m_crenum=x;
  }
  RefreshInt();
  UpdateData(UD_DISPLAY);
}

void CAreaInt::OnSelchangeSpawnnumpicker() 
{
  int x;

  x=m_spawnnumpicker.GetCurSel();
  if(x>=0 && x<=the_area.intheader.creaturecnt) m_crenum=x;
  RefreshInt();
  UpdateData(UD_DISPLAY);
}

void CAreaInt::OnClear() 
{
  int i;

  memset(&the_area.intheader,0,sizeof(area_int));
  for(i=0;i<10;i++)
  {
    the_area.intheader.strrefs[i]=10134; //seems to be ok for BG2/IWD2
  }
  the_area.intheader.difficulty=2;
  the_area.intheader.lifespan=the_area.intheader.huntingrange=the_area.intheader.followrange=1000;
  the_area.intheader.activated=1;
  the_area.intheader.maxnumber=1;
  the_area.intheader.day=the_area.intheader.night=10;
  RefreshInt();
	UpdateData(UD_DISPLAY);
}

BOOL CAreaInt::PreTranslateMessage(MSG* pMsg) 
{
  m_tooltip.RelayEvent(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}
