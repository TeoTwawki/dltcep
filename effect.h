// effect.h: interface for the Ceffect class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EFFECT_H__710F015A_A78B_4914_AB5D_C1A17BA3B770__INCLUDED_)
#define AFX_EFFECT_H__710F015A_A78B_4914_AB5D_C1A17BA3B770__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "structs.h"

class Ceffect  
{
public:
  eff_header header;

	Ceffect();
	virtual ~Ceffect();
  int WriteEffectToFile(int fhandle, int calculate);
  int ReadEffectFromFile(int fhandle, int maxlen);
};

#endif // !defined(AFX_EFFECT_H__710F015A_A78B_4914_AB5D_C1A17BA3B770__INCLUDED_)
