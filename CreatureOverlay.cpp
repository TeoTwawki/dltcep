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
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CCreatureOverlay::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCreatureOverlay)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCreatureOverlay, CDialog)
	//{{AFX_MSG_MAP(CCreatureOverlay)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCreatureOverlay message handlers

BOOL CCreatureOverlay::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  UpdateData(UD_DISPLAY);
	return TRUE; 
}

BOOL CCreatureOverlay::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::PreTranslateMessage(pMsg);
}
