// store.h: interface for the Cstore class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STORE_H__2EF510EC_E28E_4A54_9913_C3C70DAEAFCA__INCLUDED_)
#define AFX_STORE_H__2EF510EC_E28E_4A54_9913_C3C70DAEAFCA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <io.h>
#include "structs.h"

class Cstore  
{
private:
  int fhandle;
  int startpoint;
  int maxlen;

public:
  int revision;
  store_header header;
  iwd2store_header iwd2header;
  //always using the bigger structure!!!
  store_item_entry11 *entries;
  store_drink *drinks;
  store_cure *cures;
  long *itemtypes;
  
  int entrynum;
  int drinknum;
  int itemtypenum;
  int curenum;
  bool m_changed;
  
  Cstore();
  virtual ~Cstore();
  int adjust_actpoint(long offset);

  int WriteStoreToFile(int fhandle, int calculate);
  long RetrieveNameRef(int fh);
  int ReadStoreFromFile(int fhandle, int ml);
  inline long myseek(long pos)
  {
    return lseek(fhandle, pos+startpoint,SEEK_SET)-startpoint;
  }
  
  inline long actpoint()
  {
    return tell(fhandle)-startpoint;
  }
  
  inline void KillHeaders()
  {
    if(entries)
    {
      delete[] entries;
      entries=NULL;
      entrynum=0;
    }
  }
  inline void KillDrinks()
  {
    if(drinks)
    {
      delete[] drinks;
      drinks=NULL;
      drinknum=0;
    }
  }
  inline void KillCures()
  {
    if(cures)
    {
      delete[] cures;
      cures=NULL;
      curenum=0;
    }
  }
  inline void KillItemTypes()
  {
    if(itemtypes)
    {
      delete[] itemtypes;
      itemtypes=NULL;
      itemtypenum=0;
    }
  }
  
};

#endif // !defined(AFX_STORE_H__2EF510EC_E28E_4A54_9913_C3C70DAEAFCA__INCLUDED_)
