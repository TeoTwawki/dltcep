// ProjGemRB.cpp : implementation file
//

#include "stdafx.h"
#include "chitem.h"
#include "ProjGemRB.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProjGemRB dialog


CProjGemRB::CProjGemRB(CWnd* pParent /*=NULL*/)
	: CDialog(CProjGemRB::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProjGemRB)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CProjGemRB::DoDataExchange(CDataExchange* pDX)
{
  CButton *cb;
  CString tmpstr;
  int i,j;
  int flg;

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProjGemRB)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
  tmpstr.Format("%d", the_projectile.header.extflags);
  DDX_Text(pDX, IDC_FLAGS, tmpstr);
  the_projectile.header.extflags=strtonum(tmpstr);
  j=1;
  for(i=0;i<16;i++)
  {
    cb=(CButton *) GetDlgItem(IDC_FLAG1+i);
    if (cb)
    {
      flg=!!(the_projectile.header.extflags&j);
      cb->SetCheck(flg);
      j<<=1;
    }
  }
}


BEGIN_MESSAGE_MAP(CProjGemRB, CDialog)
	//{{AFX_MSG_MAP(CProjGemRB)
	ON_BN_CLICKED(IDC_FLAG1, OnFlag1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProjGemRB message handlers


void CProjGemRB::OnFlag1() 
{
  the_projectile.header.extflags^=PROJ_BOUNCE;
	UpdateData(UD_DISPLAY);
}
