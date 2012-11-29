// dialog.h: interface for the Cdialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIALOG_H__A73F5549_F90F_4642_92A6_A13FF3F97FE5__INCLUDED_)
#define AFX_DIALOG_H__A73F5549_F90F_4642_92A6_A13FF3F97FE5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "structs.h"

class Cdialog  
{
private:
  int fhandle;
  int startpoint;
  int maxlen;

public:
  dlg_header header;
  long statecount;
  dlg_state *dlgstates;
  long transcount;
  dlg_trans *dlgtrans;
  long sttriggercount;
  CString *sttriggers;
  long trtriggercount;
  CString *trtriggers;
  long actioncount;
  CString *actions;
  int changed;
  int extlinkcnt;
  dlg_extlink *extlinks;

	Cdialog();
	virtual ~Cdialog();
  int adjust_actpoint(long offset);
  inline long myseek(long pos)
  {
    return lseek(fhandle, pos+startpoint,SEEK_SET)-startpoint;
  }
  
  inline long actpoint()
  {
    return tell(fhandle)-startpoint;
  }
  void new_dialog();
  CString GetPartName(int stateindex);
  int AddExternals();
  int ResolveLinks(dlg_extlink *baselinks, int baselinkcount, CString basename);
  int MergeDialog(Cdialog &add, CString key);
  bool GetExternalLink(dlg_trans &extlink);
  int CheckExternalLink(int fh, int stateidx); //checks if stateidx state exists in dialog
  int ReadDialogFromFile(int fh, long ml, CString self);
  int WriteDialogToFile(int fh, int calculate);
  int findtrigger(int trnumber);
  inline void KillStates()
  {
    if(dlgstates)
    {
      delete[] dlgstates;
      dlgstates=NULL;
      statecount=0;
    }
  }
  inline void KillTrans()
  {
    if(dlgtrans)
    {
      delete[] dlgtrans;
      dlgtrans=NULL;
      transcount=0;
    }
  }
  inline void KillStateTriggers()
  {
    if(sttriggers)
    {
      delete[] sttriggers;
      sttriggers=NULL;
      sttriggercount=0;
    }
  }
  inline void KillTransTriggers()
  {
    if(trtriggers)
    {
      delete[] trtriggers;
      trtriggers=NULL;
      trtriggercount=0;
    }
  }
  inline void KillActions()
  {
    if(actions)
    {
      delete[] actions;
      actions=NULL;
      actioncount=0;
    }
  }
  inline void KillExternalLinks()
  {
    if(extlinks)
    {
      delete[] extlinks;
      extlinks=NULL;
      extlinkcnt=0;
    }
  }
};

#endif // !defined(AFX_DIALOG_H__A73F5549_F90F_4642_92A6_A13FF3F97FE5__INCLUDED_)
