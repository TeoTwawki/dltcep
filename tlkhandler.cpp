#include "stdafx.h"

#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "chitem.h"
#include "chitemDlg.h"
#include "compat.h"

bool truncate_references(long maxnumber, int which)
{
  tlk_entry *newtlkentries;
  int i;

  if(!tlk_entries[which])
  {
    return false;
  }
  if(maxnumber>=tlk_headerinfo[which].entrynum)
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
    newtlkentries[i]=tlk_entries[which][i];
  }

  delete [] tlk_entries[which];
  tlk_entries[which]=newtlkentries;
  tlk_headerinfo[which].entrynum=maxnumber;
  return true;
}

CString CChitemDlg::GetTlkFileName(int which)
{
  CString folder;
  CString tlkfilename;

  if (language.GetLength())
  {
    folder = bgfolder+"lang/"+language+"/";
  }
  else
  {
    folder = bgfolder;
  }
  if(which==1)
  {
    tlkfilename.Format("%sdialogf.tlk",folder);
  }
  else
  {
    tlkfilename.Format("%sdialog.tlk",folder);
  }
  return tlkfilename;
}

int CChitemDlg::write_file_progress(int which)
{
  int ret;

  if(!m_progressdlg && do_progress)
  {
    m_progressdlg=new progressbar;
    if(m_progressdlg) m_progressdlg->Create(IDD_PROGRESS);
  }
  ret=write_file(GetTlkFileName(which),which);
  if(m_progressdlg)
  {
    m_progressdlg->DestroyWindow();
    delete m_progressdlg;
    m_progressdlg=0;
  }
  return ret;
}
//this one flushes the memory to disk
int CChitemDlg::write_file(CString str, int which)
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
  if(global_changed[which]==true)
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
      tlkname.Format("Writing %s",GetTlkFileName(which));
      if(m_progressdlg) m_progressdlg->SetRange(tlk_headerinfo[which].entrynum*2,tlkname);
      tlk_headerinfo[which].start=tlk_headerinfo[which].entrynum*sizeof(tlk_reference)+sizeof(tlk_header);
      if(write(fhandle,&tlk_headerinfo[which],sizeof(tlk_header) )!=sizeof(tlk_header) )
      {
        AfxThrowUserException();
      }
      offset=0;
      for(i=0;i<tlk_headerinfo[which].entrynum;i++)
      {
        set_progress(i);
        
        tlk_entries[which][i].reference.length=tlk_entries[which][i].text.GetLength();
        tlk_entries[which][i].reference.offset=offset;
        if(write(fhandle,&tlk_entries[which][i].reference,sizeof(tlk_reference) )!=sizeof(tlk_reference) )
        {
          AfxThrowUserException();
        }
        offset+=tlk_entries[which][i].reference.length;
      }
      for(i=0;i<tlk_headerinfo[which].entrynum;i++)
      {
        set_progress(tlk_headerinfo[which].entrynum+i);
        len=tlk_entries[which][i].text.GetLength();
        if(write(fhandle,tlk_entries[which][i].text, len )!=len)
        {
          AfxThrowUserException();
        }
      }
      global_changed[which]=false;
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
