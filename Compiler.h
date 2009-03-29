// Compiler.h: interface for the Compiler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMPILER_H__7DCEF5CB_14D9_4D38_997E_DADA9256476E__INCLUDED_)
#define AFX_COMPILER_H__7DCEF5CB_14D9_4D38_997E_DADA9256476E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define READ_BUFFERLENGTH 4096

//compiler states
#define TA_ACTION    0
#define TA_TRIGGER   1
#define TA_IF        2
#define TA_RESPONSE  3

class Compiler  
{
public:
	Compiler(int arg_log);
	virtual ~Compiler();
public:
  bool Compile(CString filepath, CString output);
  void CloseAll();
  void ReadLine(int maxlength);
  bool HandleTrigger();
  bool HandleResponse();
  bool HandleAction();
  bool ProcessLine();
  void WriteObject(object &ob);
  void WriteString(CString tmp);
  void WriteError(CString tmp);

private:
  int logging;
  int flog, fhandle, fout;
  char *buffer;
  CString line;
  int num_or;
  int bpoi, valid;
  int ObjectFieldsCount;
  int MaxObjectNesting;
  bool HasAdditionalRect;
  bool HasTriggerPoint;
  int ExtraParametersCount;
  int state;
  int linecount;
};

#endif // !defined(AFX_COMPILER_H__7DCEF5CB_14D9_4D38_997E_DADA9256476E__INCLUDED_)
