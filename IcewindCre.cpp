// IcewindCre.cpp : implementation file
//

#include "stdafx.h"
#include "chitem.h"
#include "IcewindCre.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// IcewindCre dialog


IcewindCre::IcewindCre(CWnd* pParent /*=NULL*/)
	: CDialog(IcewindCre::IDD, pParent)
{
	//{{AFX_DATA_INIT(IcewindCre)
	//}}AFX_DATA_INIT
}


void IcewindCre::DoDataExchange(CDataExchange* pDX)
{
  CString tmpstr;
  int i;

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(IcewindCre)
	//}}AFX_DATA_MAP

  RetrieveVariable(tmpstr, the_creature.iwdheader.scriptname, false);
	DDX_Text(pDX, IDC_SCRIPTNAME, tmpstr);
	DDV_MaxChars(pDX, tmpstr, 32);
  StoreVariable(tmpstr, the_creature.iwdheader.scriptname, false);

  RetrieveVariable(tmpstr, the_creature.iwdheader.scriptname2, false);
	DDX_Text(pDX, IDC_SCRIPTNAME2, tmpstr);
	DDV_MaxChars(pDX, tmpstr, 32);
  StoreVariable(tmpstr, the_creature.iwdheader.scriptname2, false);

  for(i=0;i<5;i++)
  {
    DDX_Text(pDX, IDC_U1+i,the_creature.iwdheader.internals[i]);
  }
}


BEGIN_MESSAGE_MAP(IcewindCre, CDialog)
	//{{AFX_MSG_MAP(IcewindCre)
	ON_EN_KILLFOCUS(IDC_SCRIPTNAME, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_SCRIPTNAME2, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U1, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U2, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U3, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U4, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U5, DefaultKillfocus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// IcewindCre message handlers

void IcewindCre::DefaultKillfocus() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

BOOL IcewindCre::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  UpdateData(UD_DISPLAY);
	return TRUE; 
}
