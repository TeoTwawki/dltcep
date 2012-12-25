// UTF8.h: interface for the UTF8 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UTF8_H__61F4C020_24FD_4394_97FD_3FFBCD6135E1__INCLUDED_)
#define AFX_UTF8_H__61F4C020_24FD_4394_97FD_3FFBCD6135E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

extern WCHAR *ConvertUTF8ToUTF16( CString pszTextUTF8 );
extern CString ConvertUTF16ToUTF8( const WCHAR * pszTextUTF16, int size );

#endif // !defined(AFX_UTF8_H__61F4C020_24FD_4394_97FD_3FFBCD6135E1__INCLUDED_)
