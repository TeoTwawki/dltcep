// tbg.h: interface for the Ctbg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TBG_H__5F1252F4_705E_44C1_83B3_8F64BF30969A__INCLUDED_)
#define AFX_TBG_H__5F1252F4_705E_44C1_83B3_8F64BF30969A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef struct
{
  char signature[4]; //IAP
  long unused;
  long tbgcount;
  long othercount;
  unsigned char genre; //we don't care about this too much
  long totallen;
} iap_header;

typedef struct
{
  char signature[4]; //PK<ver>
  short headersize;  //20
  char unused[20];
} zip_header;

typedef struct
{
  long nameoffset;
  long namelength;
  long launchflag;
  long fileoffset;
  long filelength;
} iap_file_header;

class Ctbg  
{
public:
	Ctbg();
	virtual ~Ctbg();
  int WriteIap(int fhandle);
  int WriteZip(int fhandle);
  int ExportFile(int filetype, CString filepath);
  int Readtbg(CString filepath, CStringList &filelist);
  void new_iap();
  //new feature: modify iap
  //new feature: list of postprocessed files
  int read_iap(int fhandle, int maxlen, int onlyopen, CStringList &filelist);  
  int AddTbgFile(int pos,CString &newname);
  int AddOtherFile(int pos, CString &newname);
  int AllocateHeaders(int tbgnum, int othernum);

  tbg4_header header;
  tbgn_header tbgnheader;
  iap_header iapheader;
  zip_header zipheader;
  int revision;

  tbg_tlk_reference *tlkentries;
  unsigned long *strrefs;
  long *ascii;
  char *filedata;
  char *textdata;
  iap_file_header *iapfileheaders;
  CString *m_tbgnames, *m_othernames;

  int tlkentrycount, strrefcount, asciicount;
  long textdatasize;
  int actfilecount, iapfilecount;
  int tbgnamecount, othernamecount;

private:
  //strref collectors
  int collect_itemrefs();
  int collect_spellrefs();
  int collect_storerefs();
  int collect_effectrefs(); //only 0 or 1 effect string possible, we still export it
  int collect_arearefs();
  int collect_creaturerefs(int alternate);
  int collect_worldmaprefs();
  int collect_chuirefs();
  int collect_srcrefs();

  int AddToFile(int fhandle, CString filename, CString internalname, int ml);
  int ImportFile();
  int get_genre();
  int get_strref(long offset, long length, CString &text);
  int read_tbg(int fhandle, int maxlen);
  int patch_baldur_gam(int fhandle);
  inline KillFileData()
  {
    if(filedata)
    {
      delete [] filedata;
      filedata=NULL;
    }
  }

  inline KillTlkEntries()
  {
    if(tlkentries)
    {
      delete [] tlkentries;
      tlkentries=NULL;
    }
    tlkentrycount=0;
  }

  inline KillStrrefs()
  {
    if(strrefs)
    {
      delete [] strrefs;
      strrefs=NULL;
    }
    strrefcount=0;
  }

  inline KillAscii()
  {
    if(ascii)
    {
      delete [] ascii;
      ascii=NULL;
    }
    ascii=0;
  }

  inline KillTextData()
  {
    if(textdata)
    {
      delete [] textdata;
      textdata=NULL;
    }
    textdatasize=0;
  }

  inline KillIapFileHeaders()
  {
    if(iapfileheaders)
    {
      delete [] iapfileheaders;
      iapfileheaders=NULL;
    }
    actfilecount=iapfilecount=0;
  }

  inline KillTbgFilenames()
  {
    if(m_tbgnames)
    {
      delete [] m_tbgnames;
      m_tbgnames=NULL;
    }
    tbgnamecount=0;
  }

  inline KillOtherFilenames()
  {
    if(m_othernames)
    {
      delete [] m_othernames;
      m_othernames=NULL;
    }
    othernamecount=0;
  }
};

void ExportTBG(CWnd *pwnd, int filetype);

#endif // !defined(AFX_TBG_H__5F1252F4_705E_44C1_83B3_8F64BF30969A__INCLUDED_)
