// 2da.h: interface for the C2da class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_2DA_H__D3555E4F_E3B8_492D_ABEA_638AF2B91177__INCLUDED_)
#define AFX_2DA_H__D3555E4F_E3B8_492D_ABEA_638AF2B91177__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "chitem.h"

class CStringMapCompiler: public CStringMapToCompiler
{
public:
  ~CStringMapCompiler()
  {
    CString key;
    compiler_data value;
    POSITION pos;

    pos=GetStartPosition();
    while(pos)
    {
      GetNextAssoc(pos, key, value);
      if(value.parameters)
      {
        delete [] value.parameters;
        value.parameters=NULL;
      }
    }
  }
};

class C2da
{
public:
	C2da();
	virtual ~C2da();
  void new_2da();
  void AddRow(int row, CString label, CString def);
  int AddColumn(int col, CString label, CString def);
  int RemoveRow(int row);
  int RemoveColumn(int col);
  int Read2DAFromFile(int fhandle, int ml);
  int Write2DAToFile(int fhandle);
  int CopyColumn(int col);
  int CopyRow(int row);
  int PasteColumn(int col);
  int PasteRow(int row);
  CString GetValue(int row, int col);
  void inline killcopyrow()
  {
    if(copyrow) delete [] copyrow;
    copyrow=0;
    crows=0;
  }
  void inline killcopycol()
  {
    if(copycol) delete [] copycol;
    copycol=0;
    ccols=0;
  }

  CString header;
  CString defvalue;
  int rows;
  int cols;
  CString *copyrow;
  int crows;
  CString *copycol;
  int ccols;
  CString *collabels;
  CPtrList data;
};

class Cids
{
public:
	Cids();
	virtual ~Cids();
  void new_ids();
  void AddRow(int row, CString label, CString def);
  int RemoveRow(int row);
  int ReadIDSFromFile(int fhandle, int ml);
  int WriteIDSToFile(int fhandle);

  CString header;
  bool label;
  int rows;
  CPtrList *data;
};

#define MUSCOLUMN 3  //the number of columns

class Cmus  
{
public:
	Cmus();
	virtual ~Cmus();
  void new_mus();
  void AddRow(int row, CString arg1, CString arg2, CString arg3);
  int RemoveRow(int row);
  int ReadMusFromFile(int fhandle, int ml);
  int WriteMusToFile(int fhandle);

  CString internal;
  int rows;
  CPtrList *data;
};

extern CStringMapCompiler action_data;
extern CStringMapCompiler trigger_data;
extern C2da the_2da;
extern Cids the_ids;
extern Cmus the_mus;

extern long creature_strrefs[SND_SLOT_COUNT]; //for copy/paste operation (persistent)

int Write2daArrayToFile(CString daname, CStringMapArray &refs, CString *columnames, int columns);
int Write2daMapToFile(CString daname, CStringMapInt &refs, CString columname);
int Write2daListToFile(CString daname, CStringList &refs, CString columname);

int ReadWeapProfs(loc_entry entry);
int Read2da(loc_entry entry, CStringList &refs);
int Read2daRow(loc_entry entry, CStringList &refs, int first);
int Read2daString(loc_entry entry, CString2List &refs, int startcolumn);
int Read2daStrref(loc_entry entry, CIntMapInt &refs, int column);
int Read2daArray(loc_entry entry, CStringMapArray &refs, int columns); //maximum 3
int Read2daInt(loc_entry entry, CStringMapInt &refs, int removeall);
int Read2daField(CString daname, int line, int column);
int Read2daColumn(loc_entry entry, CStringList &refs, int column, bool caps);
int ReadIds(loc_entry entry, CStringList &refs, int add);
int ReadSongIds(loc_entry entry, CString2List &refs);
int ReadIds2(loc_entry entry, CString *refs, int maxindex);
int ReadIds3(loc_entry entry, CString *refs, int maxindex, CStringMapCompiler &atdata, int trigger_or_action);
int ReadIds4(loc_entry entry, CStringMapInt &refs);
int read_effect_descs(); // setting up opcode descriptions
int add_compiler_data(CString prototype, int cnt, CStringMapCompiler &atdata, int trigger_or_action);

#endif // !defined(AFX_2DA_H__D3555E4F_E3B8_492D_ABEA_638AF2B91177__INCLUDED_)
