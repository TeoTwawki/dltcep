// spell.h: interface for the Cspell class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPELL_H__CEA978D3_9FC9_436F_9CAF_FC3EEBF80A56__INCLUDED_)
#define AFX_SPELL_H__CEA978D3_9FC9_436F_9CAF_FC3EEBF80A56__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "structs.h"

class Cspell  
{
public:
  int revision;
	spell_header header;
  iwd2_header iwd2header;
  int extheadcount;
  int featblkcount;
  spl_ext_header *extheaders;
  feat_block *featblocks;

	Cspell();
	virtual ~Cspell();
  int WriteSpellToFile(int fhandle, int calculate);
	int ReadSpellFromFile(int fhandle, long maxlen);
  CString RetrieveResourceRef(int fh);
  long RetrieveNameRef(int fh);
  long RetrieveTypeAndLevel(int fh);
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
};

#endif // !defined(AFX_SPELL_H__CEA978D3_9FC9_436F_9CAF_FC3EEBF80A56__INCLUDED_)
