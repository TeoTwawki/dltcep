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
  int value;
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

  for(i=0;i<8;i++)
  {
    tmpstr.Format("0x%x %s",the_creature.iwd2header.enemy[i],IDSToken("RACE",the_creature.iwd2header.enemy[i], true) );
    DDX_Text(pDX, IDC_U10+i, tmpstr);
    value=IDSKey("RACE", tmpstr);
    if(value==-1)
    {
      the_creature.iwd2header.enemy[i]=(BYTE) strtonum(tmpstr);
    }
    else
    {
      the_creature.iwd2header.enemy[i]=(BYTE) value;
    }
  }

  value = (the_creature.header.idsrace<<16)+the_creature.iwd2header.subrace;
  tmpstr.Format("0x%x %s",the_creature.iwd2header.subrace,IDSToken("SUBRACE", value, true) );
  DDX_Text(pDX, IDC_SUBRACE, tmpstr);
  value=IDSKey("SUBRACE", tmpstr);
  if(value==-1)
  {
    the_creature.iwd2header.subrace=(BYTE) strtonum(tmpstr);
  }
  else
  {
    the_creature.iwd2header.subrace=(BYTE) value;
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
	ON_EN_KILLFOCUS(IDC_U10, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U11, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U15, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U14, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U13, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U12, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_SUBRACE, DefaultKillfocus)
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
