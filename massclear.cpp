// massclear.cpp : implementation file
//

#include "stdafx.h"
#include "chitem.h"
#include "massclear.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// massclear dialog


massclear::massclear(CWnd* pParent /*=NULL*/)
	: CDialog(massclear::IDD, pParent)
{
	//{{AFX_DATA_INIT(massclear)
	m_from = 0;
	m_to = 0;
	//}}AFX_DATA_INIT
}


void massclear::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(massclear)
	DDX_Control(pDX, IDC_FROM, m_from_control);
	DDX_Control(pDX, IDC_TOSP, m_tosp_control);
	DDX_Control(pDX, IDC_FROMSP, m_fromsp_control);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_FROM, m_from);
  if(m_from>m_maxval) m_from=m_maxval;
  if(m_from<m_minval) m_from=m_minval;
	DDV_MinMaxInt(pDX, m_from, m_minval, m_maxval);

	DDX_Text(pDX, IDC_TO, m_to);
  if(m_to>m_maxval) m_to=m_maxval;
  if(m_to<m_from) m_to=m_from;
  DDV_MinMaxInt(pDX, m_to, m_from, m_maxval);

  m_fromsp_control.SetRange32(m_minval,m_maxval);
  m_tosp_control.SetRange32(m_from,m_maxval);
}


BEGIN_MESSAGE_MAP(massclear, CDialog)
	//{{AFX_MSG_MAP(massclear)
	ON_EN_KILLFOCUS(IDC_FROM, OnKillfocusFrom)
	ON_EN_KILLFOCUS(IDC_TO, OnKillfocusTo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// massclear message handlers
void massclear::SetRange(int minval, int maxval, int curval)
{
  m_minval=minval;
  m_maxval=maxval;
  m_from=minval;
  m_to=max(minval,curval);
}

void massclear::SetText(CString str)
{
  m_caption=str;
}

void massclear::OnKillfocusFrom() 
{
	UpdateData(UD_RETRIEVE);		
  if(m_to<m_from) m_to=m_from;
	UpdateData(UD_DISPLAY);
}

void massclear::OnKillfocusTo() 
{
	UpdateData(UD_RETRIEVE);		
  if(m_to<m_from) m_to=m_from;
	UpdateData(UD_DISPLAY);		
}

BOOL massclear::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SetWindowText(m_caption);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
