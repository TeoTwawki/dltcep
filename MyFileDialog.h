// MyFileDialog.h: interface for the CMyFileDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYFILEDIALOG_H__0AEABC04_9EEE_4D60_91FF_D764D224BECD__INCLUDED_)
#define AFX_MYFILEDIALOG_H__0AEABC04_9EEE_4D60_91FF_D764D224BECD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxdlgs.h>

class CMyFileDialog : public CFileDialog  
{
public:
  CMyFileDialog(BOOL bOpenFileDialog,LPCTSTR lpszDefExt,LPCTSTR lpszFileName,
    DWORD dwFlags,LPCTSTR lpszFilter, CWnd *pParentWnd = NULL);
	virtual ~CMyFileDialog();

};

#endif // !defined(AFX_MYFILEDIALOG_H__0AEABC04_9EEE_4D60_91FF_D764D224BECD__INCLUDED_)
