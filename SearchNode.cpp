// SearchNode.cpp : implementation file
//

#include "stdafx.h"
#include "chitem.h"
#include "SearchNode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSearchNode dialog


CSearchNode::CSearchNode(CWnd* pParent /*=NULL*/)
	: CDialog(CSearchNode::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSearchNode)
	m_text = _T("");
	m_nodetype = 2;
	m_type = FALSE;
	m_node = 0;
	m_ignorecase = TRUE;
	m_part = 2;
	//}}AFX_DATA_INIT
}


void CSearchNode::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSearchNode)
	DDX_Text(pDX, IDC_TEXT, m_text);
	DDX_Radio(pDX, IDC_TRAN, m_nodetype);
	DDX_Check(pDX, IDC_TYPE, m_type);
	DDX_Text(pDX, IDC_NODE, m_node);
	DDX_Check(pDX, IDC_IGNORECASE, m_ignorecase);
	DDX_Radio(pDX, IDC_STRING, m_part);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSearchNode, CDialog)
	//{{AFX_MSG_MAP(CSearchNode)
	ON_EN_KILLFOCUS(IDC_NODE, OnKillfocusNode)
	ON_BN_CLICKED(IDC_TOP, OnTop)
	ON_EN_KILLFOCUS(IDC_TEXT, OnKillfocusText)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSearchNode message handlers

void CSearchNode::OnKillfocusNode() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CSearchNode::OnTop() 
{
  UpdateData(UD_RETRIEVE);
  m_node=0;
  m_type=true;
  UpdateData(UD_DISPLAY);
}

void CSearchNode::OnKillfocusText() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CSearchNode::OnOK() 
{
  UpdateData(UD_RETRIEVE);
  m_nodepart=m_part+1;
	CDialog::OnOK();
}
