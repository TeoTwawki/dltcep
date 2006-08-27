// creature.h: interface for the Ccreature class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CREATURE_H__540165B6_9B90_4BAC_8102_2D3C0FA0C672__INCLUDED_)
#define AFX_CREATURE_H__540165B6_9B90_4BAC_8102_2D3C0FA0C672__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "structs.h"

#define IWD2_SPELLCOUNT   (IWD2_SPSLOTNUM*9+9+3)
extern CString iwd2_spell_levels[IWD2_SPELLCOUNT];

typedef struct 
{
  long unknown;
  long free;
} iwd2_trail;

class Ccreature  
{
private:
  int fhandle;
  int startpoint;
  int maxlen;
  int fullsize;

public:
  int ischr;
  int revision;
  int slotcount;
 	creature_header header;
  creature_iwd2_header iwd2header;
  creature_iwd_header iwdheader;
  creature_pst_header pstheader;
  character_header chrheader;
  character_iwd2_header chriwd2header;
  creature_itemslots itemslots;
  creature_book *books;
  creature_select *selects;
  creature_memory *memos;
  creature_effect *effects;
  feat_block *oldeffects;
  creature_item *items;
  char *pst_overlay_data;
  creature_iwd2_spell *iwd2_spells[IWD2_SPELLCOUNT]; //7*9 spell, 9 domain, innates, shapes, songs
  long iwd2_counts[IWD2_SPELLCOUNT];
  iwd2_trail iwd2_free[IWD2_SPELLCOUNT];
  int bookcount;
  int selectcount;
  int memocount;
  int pst_overlay_size;
  int effectcount;
  int itemcount;
  int m_changed;
  int m_savechanges;

	Ccreature();
	virtual ~Ccreature();
  int ReadCreatureFromFile(int fhandle, long maxlen);
  long RetrieveNameRef(int fh);
  int RetrieveCreData(int fh, creature_data &credata);
  void SetupCharacter(int fullsize);
  int WriteCreatureToFile(int fhandle, int calculate);
  inline void KillBooks()
  {
    if(books)
    {
      delete[] books;
      books=NULL;
      bookcount=0;
    }
  }
  inline void KillSelects()
  {
    if(selects)
    {
      delete[] selects;
      selects=NULL;
      selectcount=0;
    }
  }
  inline void KillMemos()
  {
    if(memos)
    {
      delete[] memos;
      memos=NULL;
      memocount=0;
    }
  }
  inline void KillEffects()
  {
    if(effects)
    {
      delete[] effects;
    }
    if(oldeffects)
    {
      delete [] oldeffects;
    }
    effects=NULL;
    oldeffects=NULL;
    effectcount=0;
  }
  inline void KillItems()
  {
    if(items)
    {
      delete[] items;
      items=NULL;
      itemcount=0;
    }
  }
  inline void KillPstOverlay()
  {
    if(pst_overlay_data)
    {
      delete[] pst_overlay_data;
      pst_overlay_size = 0;
    }
  }

  inline void KillIwd2Spells()
  {
    int i;

    for(i=0;i<IWD2_SPELLCOUNT;i++)
    {
      if(iwd2_spells[i])
      {
        delete [] iwd2_spells[i];
        iwd2_spells[i]=NULL;
        iwd2_counts[i]=0;
      }
    }
  }

private:
  int fix_items();
  int adjust_actpoint(long offset);
  int handle_iwd2_spells(int position, long offset, long count);
  int handle_iwd2();
  int write_iwd2_spells();
  int calculate_iwd2_spells(int position, long &offset, long &maxcount);
  int calculate_iwd2();
  inline long myseek(long pos)
  {
    return lseek(fhandle, pos+startpoint,SEEK_SET)-startpoint;
  }
  
  inline long actpoint()
  {
    return tell(fhandle)-startpoint;
  }
};

#endif // !defined(AFX_CREATURE_H__540165B6_9B90_4BAC_8102_2D3C0FA0C672__INCLUDED_)
