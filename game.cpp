// game.cpp: implementation of the Cgame class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "chitem.h"
#include "game.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Cgame::Cgame()
{
  fhandle=-1;
  startpoint=-1;
  maxlen=-1;
  npcs=NULL;
  pcs=NULL;
  variables=NULL;
  unknowns1=NULL;
  mazedata=NULL;
  journals=NULL;
  npcextensions=NULL;
  npcstructs=NULL;
  pcstructs=NULL;
  pcextensions=NULL;
  deathvariables=NULL;
  slocs=NULL;
  pplocs=NULL;

	//memset(qslots,0,sizeof(qslots));
  npccount=0;
  pccount=0;
  npcstructcount=0;
  pcstructcount=0;
  npcextensioncount=0;
  pcextensioncount=0;
  variablecount=0;
  unknown1count=0;
  journalcount=0;
  deathvariablecount=0;
  sloccount=0;
  ppcount=0;
  revision=20; //bg2 revision is 2.0
  m_changed=false;
}

Cgame::~Cgame()
{
  KillVariables();
  KillPCs();
  KillNPCs();
  KillPCStructs();
  KillNPCStructs();
  KillPCExtensions();
  KillNPCExtensions();
  KillUnknowns1();
  KillMazeData();
  KillJournals();
  KillSavedLocs();
  KillPPLocs();
  KillDeathVariables();
}

int Cgame::insert_npc_header(int pos)
{
  gam_npc *newnpcs;
  gam_npc_extension *newextensions;
  charpoi *newstructs;

  if(pos>header.npccount) pos=header.npccount;
  newnpcs=new gam_npc[npccount+1];
  if(!newnpcs) return -3;
  newstructs=new charpoi[npccount+1];
  if(!newstructs) return -3;
  
  memcpy(newnpcs,npcs,pos*sizeof(gam_npc) );
  memcpy(newnpcs+pos+1,npcs+pos, (npccount-pos)*sizeof(gam_npc) );
  memcpy(newstructs,npcstructs,pos*sizeof(charpoi) );
  memcpy(newstructs+pos+1,npcstructs+pos, (npccount-pos)*sizeof(charpoi) );
  newextensions = new gam_npc_extension[npccount+1];
  if(!newextensions) return -3;
  memcpy(newextensions,npcextensions,pos*sizeof(gam_npc_extension) );
  memcpy(newextensions+pos+1, npcextensions+pos,(npccount-pos)*sizeof(gam_npc_extension) );
  memset(newextensions+pos,0,sizeof(gam_npc_extension) );
  if(npcextensions)
  {
    delete [] npcextensions;
  }
  npcextensions = newextensions;
  if(npcs)
  {
    delete [] npcs;
  }
  memset(newnpcs+pos,0,sizeof(gam_npc));
  npcs=newnpcs;
  if(npcstructs)
  {
    delete [] npcstructs;
  }
  newstructs[pos]=0;
  npcstructs=newstructs;
  npccount++;
  header.npccount=npccount;
  npcstructcount=npccount;
  npcextensioncount=npccount;
  return 0;
}

int Cgame::AddNpc(CString crename, CString area, int x, int y)
{
  CString tmpstr;
  int i;
  int ret;

  for(i=0;i<header.npccount;i++)
  {
    RetrieveResref(tmpstr,npcs[i].creresref);
    if(!tmpstr.CompareNoCase(crename) ) break;
  }
  if(i==header.npccount)
  {
    ret=insert_npc_header(i);
    if(ret) return ret;
    StoreResref(crename,npcs[i].creresref);
  }

  StoreResref(area,npcs[i].curarea);
  npcs[i].xpos=(short) x;
  npcs[i].ypos=(short) y;
  return 0;
}

int Cgame::adjust_actpoint(long offset)
{
  if(actpoint()==offset) return 0;
  if(maxlen<offset) return -2;
  if(myseek(offset)!=offset) return -2;
  return 1;
}

int Cgame::WriteGameToFile(int fhandle, int calculate)
{
  int esize;
  int fullsize;
  int mysize;
  int i;

  if (is_this_bgee() && revision<20) revision = 20;

  //precalculating fullsize
  switch(revision)
  {
  case 10: //this is totsc (maybe bg1 too)
    mysize=sizeof(gam_bg_npc);
    memcpy(&header,"GAMEV1.1",8);
    header.areaviewed=1;
    break;
  case 11://this is iwd1
    mysize=sizeof(gam_iwd_npc);
    memcpy(&header,"GAMEV1.1",8);
    break;
  case 12:
    mysize=sizeof(gam_pst_npc);
    memcpy(&header,"GAMEV1.1",8); //be aware, pst is 1.1 too!
    break;
  case 20: //bg2, or soa part of tob
    mysize=sizeof(gam_bg_npc); 
    memcpy(&header,"GAMEV2.0",8);
    break;
  case 21: //tob
    mysize=sizeof(gam_bg_npc);
    memcpy(&header,"GAMEV2.1",8);
    break;
  case 22:
    mysize=sizeof(gam_iwd2_npc);
    memcpy(&header,"GAMEV2.2",8);
    break;
  default:
    return -2;
  }
  esize=((char *)&header.reputation-(char *)&header);
  memcpy(&pstheader,&header, esize);
  memcpy(pstheader.curarea,header.masterarea,8); //hack
  if(revision==12)
  {
    fullsize=sizeof(gam_pst_header);
  }
  else
  {
    fullsize=sizeof(gam_header);
  }
  header.pcoffset=fullsize;
  pstheader.pcoffset=fullsize;
  fullsize+=header.pccount*(sizeof(gam_npc)+mysize);

  for(i=0;i<header.pccount;i++)
  {
    if(pcs[i].cresize)
    {
      pcs[i].creoffset=fullsize;
      fullsize+=pcs[i].cresize;
    }
  }

  header.npcoffset=fullsize;
  pstheader.npcoffset=fullsize;
  fullsize+=header.npccount*(sizeof(gam_npc)+mysize);

  for(i=0;i<header.npccount;i++)
  {
    if(npcs[i].cresize)
    {
      npcs[i].creoffset=fullsize;
      fullsize+=npcs[i].cresize;
    }
  }

  if(revision==12)
  {
    if(!mazedata)
    {
      mazedata=new gam_mazedata;
			memset(mazedata,0,sizeof(mazedata));
      if(!mazedata) return -3;
    }
    pstheader.mazeoffset=fullsize;
    fullsize+=sizeof(gam_mazedata);
	  pstheader.variableoffset=fullsize;
		pstheader.variablecount=header.variablecount;
	  fullsize+=pstheader.variablecount*sizeof(gam_variable);
    pstheader.dvaroffset=fullsize;
    fullsize+=pstheader.dvarcount*sizeof(gam_variable);
	  pstheader.journaloffset=fullsize;
  }
  else
  {
    header.unknown1offset=fullsize;
    pstheader.unknown1offset=fullsize;
    fullsize+=header.unknown1count*sizeof(gam_unknown1);
	  header.variableoffset=fullsize;
	  fullsize+=header.variablecount*sizeof(gam_variable);
	  header.journaloffset=fullsize;
  }

  fullsize+=header.journalcount*sizeof(gam_journal);
  if(revision==12)
  {
    pstheader.familiaroffset=fullsize;
    fullsize+=260; //additional size for pst (bestiary)
  }
  else
  {
    if(header.familiaroffset)
    {
      header.familiaroffset=fullsize;
      fullsize+=sizeof(familiardata)+sizeof(familiar);
      familiar.offset=fullsize;
    }
    if(revision==22) fullsize+=4;
    header.slocoffset=fullsize;
    fullsize+=header.sloccount*sizeof(gam_sloc);
    header.ppoffset=fullsize;
    fullsize+=header.ppcount*sizeof(gam_sloc);
  }

  if(calculate)
  {
    return fullsize;
  }
  if(fhandle<1) return -1;
  //writing data
  if(revision==12)
  {
    if(write(fhandle,&pstheader,sizeof(gam_pst_header) )!=sizeof(gam_pst_header) )
    {
      return -2;
    }
  }
  else
  {
    if(write(fhandle,&header,sizeof(gam_header) )!=sizeof(gam_header) )
    {
      return -2;
    }
  }
  for(i=0;i<header.pccount;i++)
  {
    if(pcs[i].cresize) memset(pcs[i].creresref,0,sizeof(pcs[i].creresref) );
    if(write(fhandle,pcs+i,sizeof(gam_npc) )!=sizeof(gam_npc) )
    {
      return -2;
    }
    if(write(fhandle,pcextensions+i,mysize)!=mysize)
    {
      return -2;
    }
  }

  for(i=0;i<header.pccount;i++)
  {
    if(write(fhandle,pcstructs[i],pcs[i].cresize) !=pcs[i].cresize)
    {
      return -2;
    }
  }

  for(i=0;i<header.npccount;i++)
  {
    if(npcs[i].cresize) memset(npcs[i].creresref,0,sizeof(npcs[i].creresref) );
    if(write(fhandle,npcs+i,sizeof(gam_npc) )!=sizeof(gam_npc) )
    {
      return -2;
    }
    if(write(fhandle,npcextensions+i,mysize)!=mysize)
    {
      return -2;
    }
  }

  for(i=0;i<header.npccount;i++)
  {
    if(write(fhandle,npcstructs[i],npcs[i].cresize) !=npcs[i].cresize)
    {
      return -2;
    }
  }

  if(revision==12)
  {
    if(write(fhandle,mazedata,sizeof(gam_mazedata) )!=sizeof(gam_mazedata) )
    {
      return -2;
    }
	  esize=header.variablecount*sizeof(gam_variable);
		if(write(fhandle,variables,esize)!=esize)
		{
			return -2;
		}
    esize=pstheader.dvarcount*sizeof(gam_variable);
    if(write(fhandle,deathvariables,esize)!=esize)
    {
      return -2;
    }
  }
  else
  {
    esize=header.unknown1count*sizeof(gam_unknown1);
    if(write(fhandle,unknowns1,esize)!=esize)
    {
      return -2;
    }
	  esize=header.variablecount*sizeof(gam_variable);
		if(write(fhandle,variables,esize)!=esize)
		{
			return -2;
		}
  }

  esize=header.journalcount*sizeof(gam_journal);
  if(write(fhandle,journals,esize)!=esize)
  {
    return -2;
  }
  if(revision==12)
  {
    esize=260;
    if(write(fhandle,familiardata,esize)!=esize)
    {
      return -2;
    }
  }
  else
  {
    if(header.familiaroffset)
    {
      if(write(fhandle,&familiar,sizeof(familiar) )!=sizeof(familiar) )
      {
        return -2;
      }
      if(write(fhandle,familiardata,sizeof(familiardata) )!=sizeof(familiardata) )
      {
        return -2;
      }
    }
    if(revision==22)
    {
      write(fhandle,familiardata,4);
    }
  }
  esize=sloccount*sizeof(gam_sloc);
  if(write(fhandle,slocs,esize)!=esize)
  {
    return -2;
  }
  esize=ppcount*sizeof(gam_sloc);
  if(write(fhandle,pplocs,esize)!=esize)
  {
    return -2;
  }
  the_game.m_changed=false;
  return 0;
}

int Cgame::ReadCreData(int offset, int size, char *&pointer,int &fullsize)
{
  int flg;
  int tmpoffset;

  if(pointer) delete [] pointer;
  pointer=new char[size];
  if(!pointer) return -3;
  if(!size) return 0;
  tmpoffset=actpoint();
  flg=adjust_actpoint(offset);
  if(flg<0) goto endofquest;
  if(read(fhandle,pointer,size)!=size) flg=-2;
  else fullsize+=size;
endofquest:
  adjust_actpoint(tmpoffset);
  return flg;
}

int Cgame::ReadExtension(char *pointer,int &fullsize, int mysize)
{
  if(!mysize) return 0;
  if(read(fhandle,pointer,mysize)!=mysize) return -2;
  fullsize+=mysize;
  return 0;
}

int Cgame::ReadGameFromFile(int fh, long ml)
{
  int flg, ret;
  int esize;
  int fullsize, mysize;
  int i;

  ret=0;
  fhandle=fh; //for safe string reads
  if(ml==-1) maxlen=filelength(fhandle);
  else maxlen=ml;
  if(maxlen<1) return -1; //short file, invalid item
  the_game.m_changed=false;
  startpoint=tell(fhandle);
  fullsize=sizeof(header);
  if(read(fhandle,&header,fullsize )!=fullsize )
  {	
  	return -2; // short file
  }
  if(memcmp(header.filetype,"GAME",4) )
  {
    return -2;
  }
  
  if(!memcmp(header.revision,"V2.2",4) )
  {
    revision=22;      
    mysize=sizeof(gam_iwd2_npc);
    goto read_pc;
  }
  if(!memcmp(header.revision,"V2.1",4) )
  {
    revision=21;
    mysize=sizeof(gam_bg_npc);
    goto read_pc;
  }
  if(!memcmp(header.revision,"V2.0",4) )
  {
    revision=20;
    mysize=sizeof(gam_bg_npc);
    goto read_pc;
  }
  if(!memcmp(header.revision,"V1.1",4) ||
     !memcmp(header.revision,"V1.0",4))
  { //this is a hack for PST
    if((header.pcoffset==184) || (header.variableoffset==184) )
    {
      adjust_actpoint(0);
      fullsize=sizeof(pstheader);
      if(read(fhandle,&pstheader,fullsize) !=fullsize) return -2;
      esize=sizeof(header)-(header.masterarea-header.filetype);
      memcpy(header.masterarea,pstheader.curarea,esize);
      revision=12;
      mysize=sizeof(gam_pst_npc);
    }
    else
    {
      if(bg1_compatible_area())
      {
        revision=10;
        mysize=sizeof(gam_bg_npc);
      }
      else
      {
        revision=11;
        mysize=sizeof(gam_iwd_npc);
      }      
    }
    goto read_pc;
  }
  return -2;
read_pc:
  //read pcs
  flg=adjust_actpoint(header.pcoffset);
  if(flg<0)
  {
    return flg;
  }
  if(flg) ret|=flg;

  if(pccount!=header.pccount)
  {
    KillPCs();
    pcs=new gam_npc[header.pccount];
    if(!pcs) return -3;
    pccount=header.pccount;
  }
  if(pccount!=pcstructcount)
  {
    KillPCStructs();
    pcstructs=new char *[pccount];
    if(!pcstructs)
    {
      return -3;
    }
    memset(pcstructs,0,pccount*sizeof(char *) );
    pcstructcount=pccount;
  }
  if(pccount!=pcextensioncount)
  {
    KillPCExtensions();
    pcextensions=new gam_npc_extension[pccount];
    if(!pcextensions)
    {
      return -3;
    }
    memset(pcextensions,0,pccount*sizeof(gam_npc_extension) );
    pcextensioncount=pccount;
  }

  for(i=0;i<pccount;i++)
  {
    esize=sizeof(gam_npc);
    if(read(fhandle,pcs+i, esize)!=esize)
    {
      return -2;
    }
    fullsize+=esize;
    flg=ReadExtension((char *) (pcextensions+i),fullsize, mysize);
    if(flg<0) return flg;
    flg=ReadCreData(pcs[i].creoffset, pcs[i].cresize,pcstructs[i],fullsize);
    if(flg<0) return flg;
  }

  //read non-party npcs
  flg=adjust_actpoint(header.npcoffset);
  if(flg<0)
  {
    return flg;
  }
  if(flg) ret|=flg;

  if(npccount!=header.npccount)
  {
    KillNPCs();
    npcs=new gam_npc[header.npccount];
    if(!npcs) return -3;
    npccount=header.npccount;
  }
  if(npccount!=npcstructcount)
  {
    KillNPCStructs();
    npcstructs=new char *[npccount];
    if(!npcstructs)
    {
      return -3;
    }
    memset(npcstructs,0,npccount*sizeof(char *) );
    npcstructcount=npccount;
  }
  if(npccount!=npcextensioncount)
  {
    KillNPCExtensions();
    npcextensions=new gam_npc_extension[npccount];
    if(!npcextensions)
    {
      return -3;
    }
    memset(npcextensions,0,npccount*sizeof(gam_npc_extension) );
    npcextensioncount=npccount;
  }

  for(i=0;i<npccount;i++)
  {
    esize=sizeof(gam_npc);
    if(read(fhandle,npcs+i, esize)!=esize)
    {
      return -2;
    }
    fullsize+=esize;
    flg=ReadExtension((char *) (npcextensions+i),fullsize, mysize);
    if(flg<0) return flg;
    flg=ReadCreData(npcs[i].creoffset, npcs[i].cresize,npcstructs[i],fullsize);
    if(flg<0) return flg;
 }

  //read unknownstruct
  flg=adjust_actpoint(header.unknown1offset);
  if(flg<0)
  {
    return flg;
  }
  if(flg) ret|=flg;

  if(unknown1count!=header.unknown1count)
  {
    KillUnknowns1();
    unknowns1=new gam_unknown1[header.unknown1count];
    if(!unknowns1) return -3;
    unknown1count=header.unknown1count;
  }
  esize=sizeof(gam_unknown1)*header.unknown1count;
  if(read(fhandle,unknowns1, esize)!=esize)
  {
    return -2;
  }
  fullsize+=esize;
  
  //read variables
  flg=adjust_actpoint(header.variableoffset);
  if(flg<0)
  {
    return flg;
  }
  if(variablecount!=header.variablecount)
  {
    KillVariables();
    variables=new gam_variable[header.variablecount];
    if(!variables) return -3;
    variablecount=header.variablecount;
  }
  esize=sizeof(gam_variable)*header.variablecount;
  if(read(fhandle,variables, esize)!=esize)
  {
    return -2;
  }
  fullsize+=esize;

  if(revision==12)
  {
    //read maze data (only in pst)
    flg=adjust_actpoint(pstheader.mazeoffset);
    if(flg<0)
    {
      return flg;
    }
    if(flg) ret|=flg;
    
    if(!mazedata)
    {
      mazedata=new gam_mazedata;
      if(!mazedata) return -3;
    }
    
    esize=sizeof(gam_mazedata);
    if(read(fhandle,mazedata, esize)!=esize)
    {
      return -2;
    }
    fullsize+=esize;
    
    //read deathvariables
    flg=adjust_actpoint(pstheader.dvaroffset);
    if(flg<0)
    {
      return flg;
    }
    if(deathvariablecount!=pstheader.dvarcount)
    {
      KillDeathVariables();
      deathvariables=new gam_variable[pstheader.dvarcount];
      if(!deathvariables) return -3;
      deathvariablecount=pstheader.dvarcount;
    }
    esize=sizeof(gam_variable)*pstheader.dvarcount;
    if(read(fhandle,deathvariables, esize)!=esize)
    {
      return -2;
    }
    fullsize+=esize;
  }

  //read journal entries
  flg=adjust_actpoint(header.journaloffset);
  if(flg<0)
  {
    return flg;
  }
  if(flg) ret|=flg;

  if(journalcount!=header.journalcount)
  {
    KillJournals();
    journals=new gam_journal[header.journalcount];
    if(!journals) return -3;
    journalcount=header.journalcount;
  }
  esize=sizeof(gam_journal)*header.journalcount;
  if(read(fhandle,journals, esize)!=esize)
  {
    return -2;
  }
  fullsize+=esize;

  if(header.familiaroffset)
  {
    flg=adjust_actpoint(header.familiaroffset);
    if(flg<0)
    {
      return flg;
    }
    if(flg) ret|=flg;
    if(revision==12)
    {
      esize=260; //another blatant hack
      if(read(fhandle,&familiardata,esize)!=esize)
      {
        return -2;
      }
    }
    else
    {
      esize=sizeof(gam_familiar);
      if(read(fhandle,&familiar,esize)!=esize)
      {
        return -2;
      }
      fullsize+=esize;
      esize=sizeof(familiardata);
      if(esize!=familiar.offset-actpoint())
      {
        //fix this minor inconsistency (found in tortured souls)
        familiar.offset=esize+actpoint(); 
        return -1;
      }
      if(read(fhandle,&familiardata,esize)!=esize)
      {
        return -2;
      }
    }
    fullsize+=esize;
  }
  else memset(&familiar,0,sizeof(familiar) );

  if(revision==20 || revision==21)
  {
    flg=adjust_actpoint(header.slocoffset);
    if(flg<0)
    {
      return flg;
    }
    if(flg) ret|=flg;

    if(sloccount!=header.sloccount)
    {
      KillSavedLocs();
      slocs = new gam_sloc[header.sloccount];
      if(!slocs) return -3;
      sloccount=header.sloccount;
    }
    esize =sizeof(gam_sloc)*header.sloccount;
    if(read(fhandle,slocs, esize)!=esize)
    {
      return -2;
    }
    fullsize+=esize;


    flg=adjust_actpoint(header.ppoffset);
    if(flg<0)
    {
      return flg;
    }
    if(flg) ret|=flg;

    if(ppcount!=header.ppcount)
    {
      KillPPLocs();
      pplocs = new gam_sloc[header.ppcount];
      if(!pplocs) return -3;
      ppcount=header.ppcount;
    }
    esize =sizeof(gam_sloc)*header.ppcount;
    if(read(fhandle,pplocs, esize)!=esize)
    {
      return -2;
    }
    fullsize+=esize;
  }
  else
  {
    sloccount = 0;
    ppcount = 0;
  }

  if(revision==22) //this is a hack for iwd2
  {
    fullsize+=4;
  }
  if(fullsize!=maxlen)
  {
    return -1;
  }
  return 0;
}
