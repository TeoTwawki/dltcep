// progressbar.cpp : implementation file
//

#include "stdafx.h"
#include "chitem.h"
#include "progressbar.h"
#include "options.h"
#include <mmsystem.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// progressbar dialog


progressbar::progressbar(CWnd* pParent /*=NULL*/)
	: CDialog(progressbar::IDD, pParent)
{
	//{{AFX_DATA_INIT(progressbar)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
  m_gran=PROGRESS_GRANULARITY;
}

void progressbar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(progressbar)
	DDX_Control(pDX, IDC_PROGRESS, m_progress_control);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(progressbar, CDialog)
	//{{AFX_MSG_MAP(progressbar)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// progressbar message handlers

void progressbar::SetRange(int range, CString title)
{
  m_range=range;
  m_progress_control.SetRange32(0,m_range);
  if(m_range<PROGRESS_GRANULARITY_SM)
  {
    m_gran=1;
  }
  else if(m_range<PROGRESS_GRANULARITY*5)
  {
    m_gran=PROGRESS_GRANULARITY_SM;
  }
  else
  {
    m_gran=PROGRESS_GRANULARITY;
  }
  SetWindowText(title);
}
void progressbar::SetProgress(int progr)
{
  if(!(progr&m_gran) )
  {
    m_progress_control.SetPos(progr);
  }
}

/////////////////////////////////////////////////////////////////////////////
// panicbutton dialog

panicbutton::panicbutton(CWnd* pParent /*=NULL*/)
	: CDialog(panicbutton::IDD, pParent)
{
	//{{AFX_DATA_INIT(panicbutton)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
  waitsound=true;
}

void panicbutton::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(panicbutton)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
  if(waitsound) SetTimer(0,1,NULL);
}

BEGIN_MESSAGE_MAP(panicbutton, CDialog)
	//{{AFX_MSG_MAP(panicbutton)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void panicbutton::OnTimer(UINT nIDEvent) 
{
  int ret;

  //ret=sndPlaySound(NULL,SND_NOSTOP|SND_ASYNC);

  ret=sndPlaySound((editflg&W98)?NULL:"",SND_NOSTOP|SND_ASYNC|SND_NOWAIT);
	if(ret)
  {
    OnOK();
  }
	CDialog::OnTimer(nIDEvent);
}
