// Bif.h: interface for the CBif class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BIF_H__31C58D81_6869_4B93_B788_E7A2CB00D662__INCLUDED_)
#define AFX_BIF_H__31C58D81_6869_4B93_B788_E7A2CB00D662__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CBif  
{
public:
	CBif();
	virtual ~CBif();

  bif_header header;
  bif_entry *entries;
  tis_entry *tisentries;
  long startpos;
//  char *data; //raw binary data

  int ReadBifHeader(CString filename); //reading header, entries, tisentries
  int WriteBifHeader(CString filename); //writing out the indices ordered, the data part needs to be appended then
  int CopyData(CString infile, CString outfile); //copying the rest
};

#endif // !defined(AFX_BIF_H__31C58D81_6869_4B93_B788_E7A2CB00D662__INCLUDED_)
