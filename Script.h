// script.h: interface for the script class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCRIPT_H__53B2709A_F74F_43C4_9A0B_F9932963C9F9__INCLUDED_)
#define AFX_SCRIPT_H__53B2709A_F74F_43C4_9A0B_F9932963C9F9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Cscript  
{
public:
	Cscript();
	virtual ~Cscript();
  int BuildScriptTree();
  void NukeBlocks();
  int get_next_scriptitem(CString &item, int function);
  int find_itemtype(search_data &searchdata, search_data &finddata, int itemtype);
  int ReadScriptFile(int fhandle, int maxlen);
  int CloseScriptFile();
  int WriteScriptToFile(FILE *fpoi);
  CString GetErrLine();
  inline int ErrLine()
  {
    return lineno;
  }
  inline void RollBack()
  {
    lineno=1;
    cnt=0;
    bcnt=tcnt=rcnt=acnt=0;
  }
  bool m_changed;
  //parsed script data (blocks[triggers[], responses[actions[]]])
  int bcnt, tcnt, rcnt, acnt;
  int blockcount;
  block *blocks;
private:
  int SkipChar(char chr);
  int ReadToken(char *token, bool crlf);
  int ReadString(int store, char *where);
  int ReadNumber(int store, int &where);
  int ReadObjectToB(int store, object &object);
  int ReadArea(int store, int *area); //only for IWD/IWD2
  int ReadObjectPST(int store, object &object);
  int ReadObjectIWD(int store, object &object);
  int ReadObjectIWD2(int store, object &object);
  int ReadObject(int store, object &object);
  int ReadPSTPoint(int store, int *where4); //only for PST
  int ReadTrigger(int store, trigger &trigger);
  int ReadAction(int store, action &action);
  int ReadResponse(int store, response &response);
  int ReadBlock(int store, block &block);

  int WriteString(FILE *fpoi, char *str);
  int WriteNumber(FILE *fpoi, int number);
  int WriteObjectToB(FILE *fpoi, object &object);
  int WriteArea(FILE *fpoi, int *area); //only for IWD2
  int WriteObjectPST(FILE *fpoi, object &object);
  int WriteObjectIWD(FILE *fpoi, object &object);
  int WriteObjectIWD2(FILE *fpoi, object &object);
  int WriteObject(FILE *fpoi, object &object);
  int WriteTrigger(FILE *fpoi, trigger &trigger);
  int WriteAction(FILE *fpoi, action &action);
  int WriteResponse(FILE *fpoi, response &response);
  int WriteBlock(FILE *fpoi, block &block);
  int cgets(char *buf, int max);
  int lineno, lastln;
  int cnt, maxlen;
  char *scrdata;
};

#define StorePosition() \
    cnt_bak=cnt; \
    lineno_bak=lineno
#define RestorePosition() \
    cnt=cnt_bak;  \
    lineno=lineno_bak

#endif // !defined(AFX_SCRIPT_H__53B2709A_F74F_43C4_9A0B_F9932963C9F9__INCLUDED_)
