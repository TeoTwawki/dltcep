// DialogLink.cpp : implementation file
//

#include "stdafx.h"
#include "chitem.h"
#include "DialogLink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogLink dialog


CDialogLink::CDialogLink(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogLink::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogLink)
	m_dialogres = _T("");
	m_state = 0;
	m_internal = FALSE;
	m_leafnode = FALSE;
	//}}AFX_DATA_INIT
}


void CDialogLink::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogLink)
	DDX_Control(pDX, IDC_DIALOG, m_dialogres_control);
	DDX_Text(pDX, IDC_DIALOG, m_dialogres);
	DDV_MaxChars(pDX, m_dialogres, 8);
	DDX_Text(pDX, IDC_STATE, m_state);
	DDX_Check(pDX, IDC_INTERNAL, m_internal);
	DDX_Check(pDX, IDC_REMOVE, m_leafnode);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogLink, CDialog)
	//{{AFX_MSG_MAP(CDialogLink)
	ON_BN_CLICKED(IDC_INTERNAL, OnInternal)
	ON_EN_KILLFOCUS(IDC_DIALOG, OnKillfocusDialog)
	ON_EN_KILLFOCUS(IDC_STATE, OnKillfocusState)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogLink message handlers

void CDialogLink::OnInternal() 
{
  if(m_internal) //was internal
  {
    m_dialogres="";
  }
  else m_dialogres=SELF_REFERENCE;
  RefreshDialog();
	UpdateData(UD_DISPLAY);
}

void CDialogLink::OnRemove() 
{
  if(!m_leafnode) //was normal node
  {
    m_internal=TRUE;
    m_dialogres="";
    m_state=0;
  }
  else
  {
    m_internal=TRUE;
    m_dialogres=SELF_REFERENCE;
  }
  m_leafnode=!m_leafnode;
  RefreshDialog();
	UpdateData(UD_DISPLAY);
}

void CDialogLink::OnKillfocusDialog() 
{
	UpdateData(UD_RETRIEVE);
  if(m_dialogres==itemname)
  {
    m_dialogres=SELF_REFERENCE;
  }
  RefreshDialog();
	UpdateData(UD_DISPLAY);
}

void CDialogLink::OnKillfocusState() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CDialogLink::OnOK() 
{
  UpdateData(UD_RETRIEVE);	
	CDialog::OnOK();
}

static int dialogboxids[]={IDC_DIALOG, IDC_STATE,0};

void CDialogLink::RefreshDialog()
{
  CWnd *cw;
  int i;

  m_internal=(m_dialogres==SELF_REFERENCE);
  for(i=0;dialogboxids[i];i++)
  {
    cw=GetDlgItem(dialogboxids[i]);
    if(cw) cw->EnableWindow(!m_leafnode);
  }
  m_dialogres_control.EnableWindow(!m_internal && !m_leafnode);
}

BOOL CDialogLink::OnInitDialog() 
{
	CDialog::OnInitDialog();
  RefreshDialog();
  UpdateData(UD_DISPLAY);
	return TRUE;
}

void CDialogLink::OnBrowse() 
{
  int res;

  pickerdlg.m_restype=REF_DLG;
  pickerdlg.m_picked=m_dialogres;
	res=pickerdlg.DoModal();
	if(res==IDOK)
	{
    m_dialogres=pickerdlg.m_picked;
  }
  RefreshDialog();
  UpdateData(UD_DISPLAY);
}

