// item.h: interface for the Citem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ITEM_H__F0B0122C_EF02_46C7_B3F4_3B5A7BC68CC0__INCLUDED_)
#define AFX_ITEM_H__F0B0122C_EF02_46C7_B3F4_3B5A7BC68CC0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "structs.h"

#define REV_BG   1
#define REV_PST  11
#define REV_IWD2 20

class Citem  
{
private:
  int fhandle;
  int startpoint;
  int maxlen;

public:
  int revision;
	item_header header;
  iwd2_header iwd2header;
  pst_header pstheader;
  int extheadcount;
  int featblkcount;
  ext_header *extheaders;
  feat_block *featblocks;
  bool m_changed;

	Citem();
	virtual ~Citem();
  int adjust_actpoint(long offset);
  inline long myseek(long pos)
  {
    return lseek(fhandle, pos+startpoint,SEEK_SET)-startpoint;
  }
  
  inline long actpoint()
  {
    return tell(fhandle)-startpoint;
  }
	int ReadItemFromFile(int fh, long ml);
  CString RetrieveResourceRef(int fh);
  long RetrieveNameRef(int fh);
  int RetrieveItemType(int fh);
  int WriteItemToFile(int fhandle, int calculate=0);
  inline void KillExtHeaders()
  {
    if(extheaders)
    {
      delete[] extheaders;
      extheaders=NULL;
      extheadcount=0;
    }
  }
  inline void KillFeatHeaders()
  {
    if(featblocks)
    {
      delete[] featblocks;
      featblocks=NULL;
      featblkcount=0;
    }
  }

private:
};

#endif // !defined(AFX_ITEM_H__F0B0122C_EF02_46C7_B3F4_3B5A7BC68CC0__INCLUDED_)
