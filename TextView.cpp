// TextView.cpp : implementation file
//

#include "stdafx.h"
#include "chitem.h"
#include "TextView.h"
#include <fcntl.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTextView dialog

CTextView::CTextView(CWnd* pParent /*=NULL*/)
	: CDialog(CTextView::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTextView)
	//}}AFX_DATA_INIT
}

void CTextView::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTextView)
	DDX_Control(pDX, IDC_TEXT, m_text_control);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTextView, CDialog)
	//{{AFX_MSG_MAP(CTextView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTextView message handlers

BOOL CTextView::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  SetWindowText("View text ("+m_file+")");
  if(m_text.IsEmpty())
  {
    int fhandle;
    int len;
    char *poi;

    fhandle=open(m_file,O_RDONLY|O_BINARY);
    m_file.Empty();
    if(fhandle>0)
    {
      len=filelength(fhandle);
      poi=m_text.GetBufferSetLength(len);
      read(fhandle, poi, len);
      close(fhandle);
      m_text.ReleaseBuffer(-1);
      m_text.Replace("\r\n","\n");
    }
    else
    {
      OnCancel();
      return FALSE;
    }
  }
  
  m_text_control.SetOptions(ECOOP_OR, ECO_SAVESEL); //prevent auto highlight crap
	m_text_control.SetWindowText(m_text);
	
	return TRUE;
}

BOOL CTextView::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::PreTranslateMessage(pMsg);
}
