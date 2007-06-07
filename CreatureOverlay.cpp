// CreatureOverlay.cpp : implementation file
//

#include "stdafx.h"
#include "chitem.h"
#include "CreatureOverlay.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCreatureOverlay dialog


CCreatureOverlay::CCreatureOverlay(CWnd* pParent /*=NULL*/)
	: CDialog(CCreatureOverlay::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCreatureOverlay)
	m_bamres = _T("");
	//}}AFX_DATA_INIT
}

static int ovrboxids[]={IDC_BAM,IDC_BROWSE,IDC_CLEAR,IDC_TIMING, IDC_TYPE,
IDC_DURATION,
0};

void CCreatureOverlay::DoDataExchange(CDataExchange* pDX)
{
  CComboBox *cb;
  int pos;
  int flg;
  int i;

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCreatureOverlay)
	//}}AFX_DATA_MAP
  cb=(CComboBox *) GetDlgItem(IDC_OVERLAYPICKER);
  pos=cb->GetCurSel();
  flg=(pos>=0) && (pos<m_itemcount);
  for(i=0;ovrboxids[i];i++)
  {
    GetDlgItem(ovrboxids[i])->EnableWindow(flg);
  }
  if(flg)
  {
    creature_pst_overlay *poi = (creature_pst_overlay *) the_creature.pst_overlay_data;
    RetrieveResref(m_bamres, poi[pos].resref);
    DDX_Text(pDX, IDC_BAM, m_bamres);
    DDV_MaxChars(pDX, m_bamres, 8);
    StoreResref(m_bamres, poi[pos].resref);

    DDX_Text(pDX, IDC_TIMING, poi[pos].timing);
    DDX_Text(pDX, IDC_TYPE, poi[pos].type);
    DDX_Text(pDX, IDC_DURATION, poi[pos].duration);
  }
}


BEGIN_MESSAGE_MAP(CCreatureOverlay, CDialog)
	//{{AFX_MSG_MAP(CCreatureOverlay)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_CLEAR, OnClear)
	ON_CBN_KILLFOCUS(IDC_OVERLAYPICKER, OnKillfocusOverlaypicker)
	ON_CBN_SELCHANGE(IDC_OVERLAYPICKER, OnSelchangeOverlaypicker)
	ON_EN_KILLFOCUS(IDC_TIMING, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_TYPE, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_BAM, OnKillfocusBam)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCreatureOverlay message handlers
/*
void CCreatureOverlay::RefreshOverlay()
{
  CComboBox *cb;
  cb=(CComboBox *) GetDlgItem(IDC_OVERLAYPICKER);

  pos = cb->GetCurSel();
  if (pos<0) return;
  
}
*/
void CCreatureOverlay::ResetCombo()
{
  CString tmpstr;
  CComboBox *cb;
  int pos;
  int i;

  cb=(CComboBox *) GetDlgItem(IDC_OVERLAYPICKER);
  pos = cb->GetCurSel();
  if (pos<0) pos = 0;
  cb->ResetContent();
  m_itemcount = the_creature.pst_overlay_size/sizeof(creature_pst_overlay);
  creature_pst_overlay *poi = (creature_pst_overlay *) the_creature.pst_overlay_data;

  for(i=0;i<m_itemcount;i++)
  {
    tmpstr.Format("%d %-.8s",i+1,poi[i].resref);
    cb->AddString(tmpstr);
  }
  cb->SetCurSel(pos);

}

BOOL CCreatureOverlay::OnInitDialog() 
{
	CDialog::OnInitDialog();
  ResetCombo();

  //tooltips
  {
    m_tooltip.Create(this,TTS_NOPREFIX);
    m_tooltip.SetMaxTipWidth(200);
    m_tooltip.SetTipBkColor(RGB(240,224,160));

    m_tooltip.AddTool(GetDlgItem(IDCANCEL), IDS_CANCEL);
  }

  UpdateData(UD_DISPLAY);
	return TRUE; 
}

void CCreatureOverlay::OnBrowse() 
{
  pickerdlg.m_restype=REF_BAM;
  pickerdlg.m_picked=m_bamres;
  if(pickerdlg.DoModal()==IDOK)
  {
    CEdit *ce = (CEdit *) GetDlgItem(IDC_BAM);
    ce->SetWindowText(pickerdlg.m_picked);
    UpdateData(UD_RETRIEVE);    
    ResetCombo();
    UpdateData(UD_DISPLAY);
  }
}

void CCreatureOverlay::OnDefaultKillfocus() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CCreatureOverlay::OnClear() 
{
	the_creature.KillPstOverlay();
  ResetCombo();
  UpdateData(UD_DISPLAY);
}

void CCreatureOverlay::OnKillfocusOverlaypicker() 
{
	UpdateData(UD_DISPLAY);		
}

void CCreatureOverlay::OnSelchangeOverlaypicker() 
{
	UpdateData(UD_DISPLAY);		
}

void CCreatureOverlay::OnKillfocusBam() 
{
	UpdateData(UD_RETRIEVE);
  ResetCombo();
	UpdateData(UD_DISPLAY);
}

BOOL CCreatureOverlay::PreTranslateMessage(MSG* pMsg) 
{
  m_tooltip.RelayEvent(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}
