// proj.h: interface for the Cproj class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROJ_H__99A8ECB0_AA73_4BA3_A7FD_95D0DBC3250F__INCLUDED_)
#define AFX_PROJ_H__99A8ECB0_AA73_4BA3_A7FD_95D0DBC3250F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "structs.h"

class Cproj  
{
public:
  proj_header header;
  proj_extension extension;
  int hasextension;

	Cproj();
	virtual ~Cproj();
  int ReadProjectileFromFile(int fhandle, long maxlen);
  int WriteProjectileToFile(int fhandle, int calculate);
};

#endif // !defined(AFX_PROJ_H__99A8ECB0_AA73_4BA3_A7FD_95D0DBC3250F__INCLUDED_)
