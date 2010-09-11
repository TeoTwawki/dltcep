// MyFileDialog.cpp: implementation of the CMyFileDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "chitem.h"
#include "MyFileDialog.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMyFileDialog::CMyFileDialog(BOOL bOpenFileDialog,LPCTSTR lpszDefExt,LPCTSTR lpszFileName,
                             DWORD dwFlags,LPCTSTR lpszFilter, CWnd *pParentWnd) :
CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{
	//use the newer commdlg version
  if ((DWORD)(LOBYTE(LOWORD(::GetVersion())))>=5) {
    if (m_ofn.lStructSize==76) {
      m_ofn.lStructSize = 88;
    }
  }

	//increase the buffer size
  if (m_ofn.nMaxFile<MAXBUFFSIZE)
  {
    strncpy(external,m_ofn.lpstrFile,MAXBUFFSIZE);
    m_ofn.lpstrFile=external;
    m_ofn.nMaxFile=MAXBUFFSIZE;
  }
	
};

CMyFileDialog::~CMyFileDialog()
{

}
