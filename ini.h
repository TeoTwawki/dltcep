// ini.h: interface for the Cini class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INI_H__14AB31A7_EC8E_41A0_A456_EE34709F8C5F__INCLUDED_)
#define AFX_INI_H__14AB31A7_EC8E_41A0_A456_EE34709F8C5F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Cini:public CStringMapToCStringMapString
{
private:
  int fhandle;
  int startpoint;
  int maxlen;

public:
	Cini();
  ~Cini();

  CStringList valid;

  bool IsValid(CString what);
  void AddValid(CString what);
  void AddValid(CString *what, int cnt);
  bool HasSection(CString what);
  CString GetValue(CString section, CString key);
  void RemoveAll();
  int ReadIniFromFile(int fh, long ml);
};

#endif // !defined(AFX_INI_H__14AB31A7_EC8E_41A0_A456_EE34709F8C5F__INCLUDED_)
