// game.h: interface for the Cgame class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GAME_H__72B155BF_330B_4E5F_B321_BB863D4CC0E9__INCLUDED_)
#define AFX_GAME_H__72B155BF_330B_4E5F_B321_BB863D4CC0E9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Cgame  
{
private:
  int fhandle;
  int startpoint;
  int maxlen;

public:
  gam_header header;
  gam_pst_header pstheader;
  gam_familiar familiar;
  char familiardata[324]; //not parsed yet
  int revision;

  int variablecount, deathvariablecount;
  int pccount, npccount;
  int unknown1count, unknown2count;
  int journalcount;
  int pcstructcount, npcstructcount;
  int pcextensioncount, npcextensioncount;

  gam_variable *variables, *deathvariables;
  gam_npc *pcs, *npcs;
  gam_unknown1 *unknowns1;
  gam_unknown2 *unknowns2;
  gam_journal *journals;
  char **npcstructs;
  char **pcstructs;
  gam_npc_extension *pcextensions, *npcextensions;

	Cgame();
	virtual ~Cgame();
  int WriteGameToFile(int fhandle, int calculate);
  int ReadGameFromFile(int fh, long ml);
  int AddNpc(CString crename, CString area, int x, int y);
  int insert_npc_header(int pos); //used by editor too

  inline void KillPCStructs()
  {
    int i;
    
    if(pcstructs)
    {
      for(i=0;i<pcstructcount;i++)
      {
        if( pcstructs[i]) delete [] pcstructs[i];
        pcstructs[i]=0;
      }
      delete [] pcstructs;
      pcstructs=NULL;
      pcstructcount=0;
    }
  }
  inline void KillNPCStructs()
  {
    int i;

    if(npcstructs)
    {
      for(i=0;i<npcstructcount;i++)
      {
        if(npcstructs[i]) delete [] npcstructs[i];
        npcstructs[i]=0;
      }
      delete [] npcstructs;
      npcstructs=NULL;
      npcstructcount=0;
    }
  }

  inline void KillPCExtensions()
  {
    if(pcextensions)
    {
      delete [] pcextensions;
      pcextensions=NULL;
      pcextensioncount=0;
    }
  }
  inline void KillNPCExtensions()
  {
    if(npcextensions)
    {
      delete [] npcextensions;
      npcextensions=NULL;
      npcextensioncount=0;
    }
  }

  inline void KillVariables()
  {
    if(variables)
    {
      delete[] variables;
      variables=NULL;
      variablecount=0;
    }
  }
  inline void KillDeathVariables()
  {
    if(deathvariables)
    {
      delete[] deathvariables;
      deathvariables=NULL;
      deathvariablecount=0;
    }
  }
  inline void KillPCs()
  {
    if(pcs)
    {
      delete[] pcs;
      pcs=NULL;
      pccount=0;
    }
  }
  inline void KillNPCs()
  {
    if(npcs)
    {
      delete[] npcs;
      npcs=NULL;
      npccount=0;
    }
  }
  inline void KillUnknowns1()
  {
    if(unknowns1)
    {
      delete[] unknowns1;
      unknowns1=NULL;
      unknown1count=0;
    }
  }
  inline void KillUnknowns2()
  {
    if(unknowns2)
    {
      delete[] unknowns2;
      unknowns2=NULL;
      unknown2count=0;
    }
  }
  inline void KillJournals()
  {
    if(journals)
    {
      delete[] journals;
      journals=NULL;
      journalcount=0;
    }
  }
private:
  int ReadCreData(int offset, int size, char *&pointer,int &fullsize);
  int ReadExtension(char *pointer, int &fullsize, int mysize);
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

#endif // !defined(AFX_GAME_H__72B155BF_330B_4E5F_B321_BB863D4CC0E9__INCLUDED_)
