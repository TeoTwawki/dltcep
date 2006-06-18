// AreaTriggerString.cpp : implementation file
//

#include "stdafx.h"
#include "chitem.h"
#include "AreaTriggerString.h"
#include "Options.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAreaTriggerString dialog


CAreaTriggerString::CAreaTriggerString(CWnd* pParent /*=NULL*/)
	: CDialog(CAreaTriggerString::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAreaTriggerString)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CAreaTriggerString::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAreaTriggerString)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_STRREF, the_area.triggerheaders[parent->m_triggernum].strref);
	DDX_Text(pDX, IDC_INFOSTR, parent->m_infostr);
	DDX_Text(pDX, IDC_STRREF2, the_area.triggerheaders[parent->m_triggernum].dialogstrref);
	DDX_Text(pDX, IDC_INFOSTR2, parent->m_dlgstr);    
}


BEGIN_MESSAGE_MAP(CAreaTriggerString, CDialog)
	//{{AFX_MSG_MAP(CAreaTriggerString)
	ON_EN_KILLFOCUS(IDC_STRREF, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_INFOSTR, OnKillfocusInfostr)
	ON_EN_KILLFOCUS(IDC_STRREF2, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_INFOSTR2, OnKillfocusInfostr2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAreaTriggerString message handlers

BOOL CAreaTriggerString::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	return TRUE;
}

void CAreaTriggerString::OnDefaultKillfocus() 
{
	UpdateData(UD_RETRIEVE);
	parent->m_infostr=resolve_tlk_text(the_area.triggerheaders[parent->m_triggernum].strref);
	parent->m_dlgstr=resolve_tlk_text(the_area.triggerheaders[parent->m_triggernum].dialogstrref);
	UpdateData(UD_DISPLAY);
}

void CAreaTriggerString::OnKillfocusInfostr() 
{
  CString old;

	UpdateData(UD_RETRIEVE);
  old=resolve_tlk_text(the_area.triggerheaders[parent->m_triggernum].strref);
  if(old!=parent->m_infostr)
  {
    if(editflg&TLKCHANGE)
    {
      if(MessageBox("Do you want to update dialog.tlk?","Area editor",MB_YESNO)!=IDYES)
      {
        return;
      }
    }
    the_area.triggerheaders[parent->m_triggernum].strref=store_tlk_text(the_area.triggerheaders[parent->m_triggernum].strref, parent->m_infostr);
  }
	parent->m_infostr=resolve_tlk_text(the_area.triggerheaders[parent->m_triggernum].strref);
	UpdateData(UD_DISPLAY);
}

void CAreaTriggerString::OnKillfocusInfostr2() 
{
  CString old;

	UpdateData(UD_RETRIEVE);
  old=resolve_tlk_text(the_area.triggerheaders[parent->m_triggernum].dialogstrref);
  if(old!=parent->m_dlgstr)
  {
    if(editflg&TLKCHANGE)
    {
      if(MessageBox("Do you want to update dialog.tlk?","Area editor",MB_YESNO)!=IDYES)
      {
        return;
      }
    }
    the_area.triggerheaders[parent->m_triggernum].dialogstrref=store_tlk_text(the_area.triggerheaders[parent->m_triggernum].dialogstrref, parent->m_dlgstr);
  }
	parent->m_dlgstr=resolve_tlk_text(the_area.triggerheaders[parent->m_triggernum].dialogstrref);
	UpdateData(UD_DISPLAY);
}
