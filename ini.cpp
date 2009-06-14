// ini.cpp: implementation of the Cini class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "chitem.h"
#include "ini.h"
#include "ChitemDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

extern int read_string(FILE *fpoi, char *pattern, char *tmpbuff=NULL, int length=0);
extern void SetupReader(int max);

Cini::Cini()
{
  
}

void Cini::RemoveAll()
{
  CString key;
  CStringMapString *value = NULL;
  POSITION pos;
  
  pos=GetStartPosition();
  while(pos)
  {
    GetNextAssoc(pos, key, value);
    if(value)
    {
      delete value;
      value=NULL;
    }
  }
  CTypedPtrMap<CMapStringToOb, CString, CStringMapString*>::RemoveAll();
  valid.RemoveAll();
}

Cini::~Cini()
{
  RemoveAll();
}

void Cini::AddValid(CString what)
{
  valid.AddTail(what);
}

void Cini::AddValid(CString *what, int cnt)
{
  while(cnt--)
  {
    valid.AddTail(what[cnt]);
  }
}

bool Cini::IsValid(CString what)
{
  if(valid.Find(what)) return true;
  return false;
}

bool Cini::HasSection(CString what)
{
  CStringMapString *sect = NULL;

  return !!Lookup(what, sect);
}

CString Cini::GetValue(CString section, CString key)
{
  CStringMapString *sect = NULL;
  CString value;

  Lookup(section, sect);
  if(!sect) return value; //it is empty at this point
  sect->Lookup(key, value);
  return value;
}

int Cini::ReadIniFromFile(int fh, long ml)
{
  FILE *fpoi;
  int ret;
  int res;
  char line[MAXBUFFSIZE];
  CString tmpstr;
  CString section;
  CString key;
  CString value;
  CStringMapString *sect = NULL;

  RemoveAll();
  ret=0;
  fhandle=fh; //for safe string reads
  if(ml==-1) maxlen=filelength(fhandle);
  else maxlen=ml;
  if(maxlen<1) return -1; //short file, invalid item

  fpoi=fdopen(fhandle,"rb");
  if(!fpoi)
  {
    close(fhandle);
    return -1;
  }

  SetupReader(maxlen);  
  do
  {
    res=read_string(fpoi,"\n", line, sizeof(line));
    tmpstr=line;
    tmpstr.TrimLeft();
    tmpstr.TrimRight();
    if(tmpstr.IsEmpty()) continue;
    if(tmpstr[0]==';') continue;

    if(tmpstr[0]=='[' && tmpstr.GetLength()>3)
    {
      section=tmpstr.Mid(1,tmpstr.GetLength()-2);
      if(Lookup(section, sect) )
      {
        ((CChitemDlg *) AfxGetMainWnd())->log("duplicate section entry: %s", section);
      }
      else
      {
        section.MakeLower();
        SetAt(section, new CStringMapString);
      }
      continue;
    }

    if (section.GetLength())
    {
      int separator = tmpstr.Find("=");
      if (separator>1)
      {
        if (!Lookup(section, sect) || !sect)
        {
          fclose(fpoi);
          return -4; //internal error
        }
        key=tmpstr.Left(separator);
        value=tmpstr.Mid(separator+1);
        key.TrimRight();
        value.TrimLeft();
        if(key.GetLength())
        {
          key.MakeLower();
          value.MakeLower();
          sect->SetAt(key, value);
        }
      }
      else
      {
        res=-3; //invalid entry
      }
    }
    else
    {
      res=-2; //invalid spawn ini file starts without section
    }
  }
  while(res==1);
  if (res==2) res=0;
  fclose(fpoi);
  return res;
}
