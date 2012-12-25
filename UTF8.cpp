// UTF8.cpp: implementation of the UTF8 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "chitem.h"
#include "UTF8.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


//----------------------------------------------------------------------------
// FUNCTION: ConvertUTF8ToUTF16
// DESC: Converts Unicode UTF-8 text to Unicode UTF-16 (Windows default).
//----------------------------------------------------------------------------
WCHAR *ConvertUTF8ToUTF16( CString pszTextUTF8 )
{
 
    //
    // Get size of destination UTF-16 buffer, in WCHAR's
    //
    int cchUTF16 = ::MultiByteToWideChar(
        CP_UTF8,                // convert from UTF-8
        MB_ERR_INVALID_CHARS,   // error on invalid chars
        pszTextUTF8,            // source UTF-8 string
        pszTextUTF8.GetLength()+1,                 // total length of source UTF-8 string,
                                // in CHAR's (= bytes), including end-of-string \0
        NULL,                   // unused - no conversion done in this step
        0                       // request size of destination buffer, in WCHAR's
        );
 
    //
    // Allocate destination buffer to store UTF-16 string
    //
    WCHAR * pszUTF16 = (WCHAR *) malloc( cchUTF16*sizeof(WCHAR) );
 
    //
    // Do the conversion from UTF-8 to UTF-16
    //
    ::MultiByteToWideChar(
        CP_UTF8,                // convert from UTF-8
        MB_ERR_INVALID_CHARS,   // error on invalid chars
        pszTextUTF8,            // source UTF-8 string
        pszTextUTF8.GetLength()+1,// total length of source UTF-8 string,
                                // in CHAR's (= bytes), including end-of-string \0
        pszUTF16,               // destination buffer
        cchUTF16                // size of destination buffer, in WCHAR's
        );
    // Return resulting UTF16 string
    return pszUTF16;
}
 
 
 
//----------------------------------------------------------------------------
// FUNCTION: ConvertUTF16ToUTF8
// DESC: Converts Unicode UTF-16 (Windows default) text to Unicode UTF-8.
//----------------------------------------------------------------------------

CString ConvertUTF16ToUTF8( const WCHAR * pszTextUTF16, int size )
{
    //
    // Get size of destination UTF-8 buffer, in CHAR's (= bytes)
    //
    int cbUTF8 = ::WideCharToMultiByte(
        CP_UTF8,                // convert to UTF-8
        0,      // specify conversion behavior
        pszTextUTF16,           // source UTF-16 string
        size,   // total source string length, in WCHAR's,
                                        // including end-of-string \0
        NULL,                   // unused - no conversion required in this step
        0,                      // request buffer size
        NULL, NULL              // unused
        );

 
    //
    // Allocate destination buffer for UTF-8 string
    //
    CString strUTF8;
    int cchUTF8 = cbUTF8; // sizeof(CHAR) = 1 byte
    CHAR * pszUTF8 = strUTF8.GetBuffer( cchUTF8 );
 
 
    //
    // Do the conversion from UTF-16 to UTF-8
    //
    ::WideCharToMultiByte(
        CP_UTF8,                // convert to UTF-8
        0,      // specify conversion behavior
        pszTextUTF16,           // source UTF-16 string
        size,   // total source string length, in WCHAR's,
                                        // including end-of-string \0
        pszUTF8,                // destination buffer
        cbUTF8,                 // destination buffer size, in bytes
        NULL, NULL              // unused
        ); 
 
    // Release internal CString buffer
    strUTF8.ReleaseBuffer();
 
    // Return resulting UTF-8 string
    return strUTF8;
}
