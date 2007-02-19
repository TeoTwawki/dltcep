// IWD2Creature.cpp : implementation file
//

#include "stdafx.h"
#include "chitem.h"
#include "IWD2Creature.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// IWD2Creature dialog


IWD2Creature::IWD2Creature(CWnd* pParent /*=NULL*/)
	: CDialog(IWD2Creature::IDD, pParent)
{
	//{{AFX_DATA_INIT(IWD2Creature)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void IWD2Creature::DoDataExchange(CDataExchange* pDX)
{
  CString tmpstr;
  int i;

	CDialog::DoDataExchange(pDX);
  RetrieveVariable(tmpstr, the_creature.iwd2header.scriptname, false);
	DDX_Text(pDX, IDC_SCRIPTNAME, tmpstr);
	DDV_MaxChars(pDX, tmpstr, 32);
  StoreVariable(tmpstr, the_creature.iwd2header.scriptname, false);

  RetrieveVariable(tmpstr, the_creature.iwd2header.scriptname2, false);
	DDX_Text(pDX, IDC_SCRIPTNAME2, tmpstr);
	DDV_MaxChars(pDX, tmpstr, 32);
  StoreVariable(tmpstr, the_creature.iwd2header.scriptname2, false);

  for(i=0;i<5;i++)
  {
    DDX_Text(pDX, IDC_U1+i,the_creature.iwd2header.internals[i]);
  }
  DDX_Text(pDX, IDC_XPLIST, the_creature.iwd2header.cr);
}


BEGIN_MESSAGE_MAP(IWD2Creature, CDialog)
	//{{AFX_MSG_MAP(IWD2Creature)
	ON_EN_KILLFOCUS(IDC_SCRIPTNAME, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_SCRIPTNAME2, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U5, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U4, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U3, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U2, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U1, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_XPLIST, DefaultKillfocus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// IWD2Creature message handlers

void IWD2Creature::DefaultKillfocus() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

BOOL IWD2Creature::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	UpdateData(UD_DISPLAY);
	
	return TRUE;
}
