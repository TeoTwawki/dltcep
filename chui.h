// chui.h: interface for the Cchui class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHUI_H__7FF0AA38_56CA_40F3_87E2_70786A296470__INCLUDED_)
#define AFX_CHUI_H__7FF0AA38_56CA_40F3_87E2_70786A296470__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Cchui  
{
private:
  int fhandle;
  int startpoint;
  int maxlen;
  int fullsize;

public:
  chui_header header;
  chui_window *windows;
  POINT *controltable; //x: offset, y: size
  chui_control_common *controls;
  BYTE **extensions;
  int windowcnt, controlcnt;
  int revision;
  bool m_changed;

	Cchui();
	virtual ~Cchui();
  int ReadChuiFromFile(int fhandle, long maxlen);
  int WriteChuiToFile(int fhandle, int calculate);
  CString RetrieveMosRef(int fh);
  inline void KillWindows()
  {
    if(windows)
    {
      delete [] windows;
      windows=NULL;
      windowcnt=0;
    }
  }

  inline void KillControls()
  {
    int i;

    if(controls)
    {
      delete [] controls;
      controls=NULL;
    }
    if(controltable)
    {
      delete [] controltable;
      controltable=NULL;
    }
    if(extensions)
    {
      for(i=0;i<controlcnt;i++)
      {
        if(extensions[i])
        {
          delete [] extensions[i];
          extensions[i]=NULL;
        }
      }
      delete [] extensions;
      extensions=NULL;
    }
    controlcnt=0;
  }

private:
  int adjust_actpoint(long offset);

  inline long myseek(long pos)
  {
    return lseek(fhandle, pos+startpoint,SEEK_SET)-startpoint;
  }
  
  inline long actpoint()
  {
    return tell(fhandle)-startpoint;
  }
};

#endif // !defined(AFX_CHUI_H__7FF0AA38_56CA_40F3_87E2_70786A296470__INCLUDED_)
