// WeiduLog.cpp : implementation file
//

#include "stdafx.h"

#include <fcntl.h>
#include "chitem.h"
#include "WeiduLog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWeiduLog dialog


CWeiduLog::CWeiduLog(CWnd* pParent /*=NULL*/)
	: CDialog(CWeiduLog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWeiduLog)
	m_log = _T("");
	//}}AFX_DATA_INIT
}


void CWeiduLog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWeiduLog)
	DDX_Text(pDX, IDC_LOG, m_log);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWeiduLog, CDialog)
	//{{AFX_MSG_MAP(CWeiduLog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWeiduLog message handlers

BOOL CWeiduLog::OnInitDialog() 
{
  int fhandle;
  int len;
  char *poi;

	CDialog::OnInitDialog();
	
  fhandle=open(WEIDU_LOG,O_RDONLY|O_BINARY);
  if(fhandle>0)
  {
    len=filelength(fhandle);
    poi=m_log.GetBufferSetLength(len);
    read(fhandle,poi,len);
    m_log.ReleaseBuffer();
    close(fhandle);
  }
  UpdateData(UD_DISPLAY);
	return TRUE;
}
