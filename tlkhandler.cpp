#include "stdafx.h"

#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "chitem.h"
#include "chitemDlg.h"
#include "compat.h"

int truncate_references(long maxnumber)
{
  tlk_entry *newtlkentries;
  int i;

  if(maxnumber>=tlk_headerinfo.entrynum)
  {
    return false;
  }
  newtlkentries=new tlk_entry[maxnumber];
  if(!newtlkentries)
  {
    return false; //failed due to memory 
  }
  for(i=0;i<maxnumber;i++)
  {
    newtlkentries[i]=tlk_entries[i];
  }

  delete [] tlk_entries;
  tlk_entries=newtlkentries;
  tlk_headerinfo.entrynum=maxnumber;
  return true;
}

int CChitemDlg::write_file_progress(CString str)
{
  int ret;

  if(!m_progressdlg && do_progress)
  {
    m_progressdlg=new progressbar;
    if(m_progressdlg) m_progressdlg->Create(IDD_PROGRESS);
  }
  ret=write_file(str);
  if(m_progressdlg)
  {
    m_progressdlg->DestroyWindow();
    delete m_progressdlg;
    m_progressdlg=0;
  }
  return ret;
}
//this one flushes the memory to disk
int CChitemDlg::write_file(CString str)
{
  int fhandle;
  int ret;
  int i;
  bool backup;
  long len;
  long offset;
  CString old, tlkname;
  
  old=str+".old";
  ret=false;
  backup=false;
  if(global_changed)
  {
    if(!access(str,0) )
    {
retry:
    if(rename(str,old))
    {
      switch(MessageBox("Overwrite "+old+"?","Backup file already exists.",MB_YESNOCANCEL) )
      {
      case IDYES:
        unlink(old);
        goto retry;
      case IDNO:
        break;
      case IDCANCEL:
        return false;
      }
    }
    else backup=true;
    }  
    fhandle=creat(str,S_IREAD|S_IWRITE);
    if(fhandle<1) return false;  
    TRY
    {
      tlkname.Format("Writing dialog%s.tlk",optflg&DIALOGF?"F":"");
      if(m_progressdlg) m_progressdlg->SetRange(tlk_headerinfo.entrynum*2,tlkname);
      tlk_headerinfo.start=tlk_headerinfo.entrynum*sizeof(tlk_reference)+sizeof(tlk_headerinfo);
      if(write(fhandle,&tlk_headerinfo,sizeof(tlk_headerinfo) )!=sizeof(tlk_headerinfo) )
      {
        AfxThrowUserException();
      }
      offset=0;
      for(i=0;i<tlk_headerinfo.entrynum;i++)
      {
        set_progress(i);
        
        tlk_entries[i].reference.length=tlk_entries[i].text.GetLength();
        tlk_entries[i].reference.offset=offset;
        if(write(fhandle,&tlk_entries[i].reference,sizeof(tlk_reference) )!=sizeof(tlk_reference) )
        {
          AfxThrowUserException();
        }
        offset+=tlk_entries[i].reference.length;
      }
      for(i=0;i<tlk_headerinfo.entrynum;i++)
      {
        set_progress(tlk_headerinfo.entrynum+i);
        len=tlk_entries[i].text.GetLength();
        if(write(fhandle,tlk_entries[i].text, len )!=len)
        {
          AfxThrowUserException();
        }
      }
      global_changed=false;
      m_event.Empty();
      if(backup) log("Done writing! Old tlk file has been backed up.");  
      else log("Done writing!");  
      ret=true;
    }
    CATCH_ALL(e)
    {
      ret=false;
      MessageBox("Runtime exception...","Error",MB_ICONSTOP|MB_OK);
    }
    END_CATCH_ALL;
    close(fhandle);
  }

  return ret;
}
