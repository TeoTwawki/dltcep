// creature.cpp: implementation of the Ccreature class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <fcntl.h>

#include "chitem.h"
#include "creature.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Ccreature::Ccreature()
{
  books=NULL;
  selects=NULL;
  memos=NULL;
  items=NULL;
  effects=NULL;
  oldeffects=NULL;
  bookcount=0;
  selectcount=0;
  memocount=0;
  itemcount=0;
  effectcount=0;
  revision=10;
  memset(iwd2_spells,0,sizeof(iwd2_spells));
}

Ccreature::~Ccreature()
{
  KillBooks();
  KillSelects();
  KillMemos();
  KillItems();
  KillEffects();
  KillIwd2Spells();
}
int Ccreature::adjust_actpoint(long offset)
{
  if(actpoint()==offset) return 0;
  if(maxlen<offset) return -2;
  if(myseek(offset)!=offset) return -2;
  return 1;
}
//bg2 character
void Ccreature::SetupCharacter(int fullsize)
{
  CString tmpstr;

  ischr=1;
  memset(&chrheader,0,sizeof(chrheader));
  memset(&chriwd2header,0,sizeof(chriwd2header));
  switch(revision)
  {
  case 10: //bg1, bg2/tob or soa
    if(bg1_compatible_area()) memcpy(chrheader.filetype,"CHR V1.0",8); //bg1
    if(tob_specific()) memcpy(chrheader.filetype,"CHR V2.1",8); //tob
    else memcpy(chrheader.filetype,"CHR V2.0",8);    //soa
    break;
  case 11: case 12: case 90: //pst, iwd
    memcpy(chrheader.filetype,"CHR V1.0",8);
    break;
  case 22:
    memcpy(chrheader.filetype,"CHR V2.2",8);
    break;
  }
  tmpstr=resolve_tlk_text(header.shortname);
  StoreName(tmpstr,chrheader.name);
  chrheader.creoffset=sizeof(chrheader);
  chrheader.cresize=fullsize;
  memcpy(&chriwd2header,&chrheader,sizeof(chrheader)); //iwd2 header is bigger
}

int Ccreature::WriteCreatureToFile(int fhandle, int calculate)
{
  int chrsize, esize;

  //book offset
  switch(revision)
  {
  case 10:
    fullsize=sizeof(creature_header);
  	memcpy(header.filetype,"CRE V1.0",8);
    break;
  case 11: case 12:
    fullsize=sizeof(creature_pst_header);
  	memcpy(header.filetype,"CRE V1.2",8);
    memcpy(&pstheader,&header,pstheader.unknown270-(char *) &pstheader);
    memcpy(&pstheader.idsea,&header.idsea,header.dialogresref-(char *) &header.idsea+8);
    break;
  case 22:
    fullsize=sizeof(creature_iwd2_header);
  	memcpy(header.filetype,"CRE V2.2",8);
    memcpy(&iwd2header,&header,((char *) &iwd2header.crushac)-(char *) &iwd2header);
    memcpy(&iwd2header.crushac,&header.crushac,13);
    memcpy(&iwd2header.resfire,&header.resfire,12);
    memcpy(&iwd2header.strrefs,&header.strrefs,64*sizeof(int) );
    iwd2header.strstat=header.strstat;
    memcpy(&iwd2header.intstat,&header.intstat,5);
    memcpy(iwd2header.scripts,header.scripts,sizeof(iwd2header.scripts) );
    memcpy(&iwd2header.idsea,&header.idsea,((unsigned char *) iwd2header.unknown8)-&iwd2header.idsea);
    memcpy(&iwd2header.itemslots,&header.itemslots,(IWD2_SLOT_COUNT+2)*sizeof(short) );
    break;
  case 90:
    fullsize=sizeof(creature_iwd_header);
  	memcpy(header.filetype,"CRE V9.0",8);
    memcpy(&iwdheader,&header,iwdheader.unknown270-(char *) &iwdheader);
    memcpy(&iwdheader.idsea,&header.idsea,header.dialogresref-(char *) &header.idsea+8);
    break;
  }
  if(revision==22)
  {
    if(calculate_iwd2(fullsize)) return -1;
  }
  else
  {
    header.bookoffs=fullsize;
    //select
    fullsize+=header.bookcnt*sizeof(creature_book);
    header.selectoffs=fullsize;
    //memo
    fullsize+=header.selectcnt*sizeof(creature_select);
    header.memoffs=fullsize;
    //effect
    fullsize+=header.memcnt*sizeof(creature_memory);
  }
  header.effectoffs=fullsize;
  //item
  if(header.effbyte)
  {
    fullsize+=header.effectcnt*sizeof(creature_effect);
  }
  else //bg1 effects are smaller
  {
    fullsize+=header.effectcnt*sizeof(feat_block);
  }
  header.itemoffs=fullsize;
  //itemslots
  fullsize+=header.itemcnt*sizeof(creature_item);
  header.itemslots=fullsize;
  esize=sizeof(short)*slotcount+sizeof(long);
  fullsize+=esize;

  if(calculate&2)
  {
    switch(revision)
    {
    case 22:
      fullsize=sizeof(character_iwd2_header);
      break;
    case 10:case 11: case 12: case 90:
      chrsize=sizeof(character_header);
      break;
    default:
      return -4; //unsupported feature
    }
  }
  else chrsize=0;
  if(calculate&1)
  {
    return fullsize+chrsize;
  }
  if(fhandle<1) return -1;
  if(calculate&2)
  {
    if(!ischr) SetupCharacter(fullsize);
    switch(revision)
    {
    case 22:
      write(fhandle,&chriwd2header,sizeof(character_iwd2_header) );
      break;
    case 10: case 11: case 12: case 90:
      write(fhandle,&chrheader,sizeof(character_header) );
      break;
    default:
      break;
    }
  }
  switch(revision)
  {
  case 22:
    if(write(fhandle,&iwd2header,sizeof(creature_iwd2_header) )!=sizeof(creature_iwd2_header) )
    { 	
   	  return -2; // short file, invalid item
    }
    break;
  case 11: case 12:
    if(write(fhandle,&pstheader,sizeof(creature_pst_header) )!=sizeof(creature_pst_header) )
    { 	
   	  return -2; // short file, invalid item
    }
    break;
  case 90:
    if(write(fhandle,&iwdheader,sizeof(creature_iwd_header) )!=sizeof(creature_iwd_header) )
    { 	
   	  return -2; // short file, invalid item
    }
    break;
  case 10:
    if(write(fhandle,&header,sizeof(creature_header) )!=sizeof(creature_header) )
    { 	
   	  return -2; // short file, invalid item
    }
    break;
  }

  if(revision==22)
  {
    write_iwd2_spells(fhandle);
  }
  else
  {
    esize=header.bookcnt*sizeof(creature_book);
    if(write(fhandle, books, esize )!=esize )
    {
      return -2;
    }
    esize=header.selectcnt*sizeof(creature_select); //spell slots
    if(write(fhandle, selects, esize )!=esize )
    {
      return -2;
    }
    esize=header.memcnt*sizeof(creature_memory); //memorised/selected spells
    if(write(fhandle, memos, esize )!=esize )
    {
      return -2;
    }
  }

  if(header.effbyte)
  {
    esize=header.effectcnt*sizeof(creature_effect);
    if(write(fhandle, effects, esize )!=esize )
    {
      return -2;
    }
  }
  else
  {
    esize=header.effectcnt*sizeof(feat_block);
    if(write(fhandle, oldeffects, esize )!=esize )
    {
      return -2;
    }
  }

  esize=header.itemcnt*sizeof(creature_item);
  if(write(fhandle, items, esize )!=esize )
  {
    return -2;
  }
  esize=sizeof(short)*slotcount+sizeof(long);
  if(write(fhandle, itemslots, esize )!=esize )
  {
    return -2;
  }
  return 0;
}

long Ccreature::RetrieveNameRef(int fh)
{
  int required;

  if(fh<1) return -1;
  required=(BYTE *) &header.longname-(BYTE *) &header+sizeof(long);
  if(read(fh, &header, required)!=required)
  {
    close(fh);
    return -1;
  }
  close(fh);
  return header.longname;
}

int Ccreature::RetrieveCreData(int fh, creature_data &credata)
{
  int esize;
  int ret;

  memset(&credata,0,sizeof(credata) );
  if(fh<1) return -1;
  fhandle=fh; //for safe string reads
  ret=0;
  if(read(fhandle,&header,8 )!=8 )
  {	
  	return -2; // short file, invalid item
  }
  if(memcmp(header.filetype,"CRE ",4) )
  {
	  ret=-2;
    goto endofquest;
  }
  //we can load only bg2 creatures
  //pst: v1.2,  iwd: v9.0 iwd2: v2.2
  if(!memcmp(header.revision,"V1.0",4) ) 
  {
    esize=sizeof(creature_header)-8;
    if(read(fhandle,((BYTE *) &header)+8,esize ) !=esize )
    {
      ret=-2;
      goto endofquest;
    }
    credata.animid=the_creature.header.animid;
    memcpy(credata.scripts,the_creature.header.scripts,5*8);
    memcpy(credata.dialogresref,the_creature.header.dialogresref,8);
    revision=10;
    goto endofquest;
  }
  if(!memcmp(header.revision,"V1.2",4) )
  {
    esize=sizeof(creature_pst_header)-8;
    if(read(fhandle,((BYTE *) &pstheader)+8,esize ) !=esize )
    {
      ret=-2;
      goto endofquest;
    }
    credata.animid=the_creature.pstheader.animid;
    memcpy(credata.scripts,the_creature.pstheader.scripts,5*8);
    memcpy(credata.dialogresref,the_creature.pstheader.dialogresref,8);
    revision=12;
    goto endofquest;
  }
  if(!memcmp(header.revision,"V9.0",4) )
  {
    esize=sizeof(creature_iwd_header)-8;
    if(read(fhandle,((BYTE *) &iwdheader)+8,esize ) !=esize )
    {
      ret=-2;
      goto endofquest;
    }
    credata.animid=the_creature.iwdheader.animid;
    memcpy(credata.scripts,the_creature.iwdheader.scripts,5*8);
    memcpy(credata.dialogresref,the_creature.iwdheader.dialogresref,8);
    revision=90;
    goto endofquest;
  }
  if(!memcmp(header.revision,"V2.2",4) )
  {
    memcpy(&iwd2header,&header,8);
    esize=sizeof(creature_iwd2_header)-8;
    if(read(fhandle,((BYTE *) &iwd2header)+8,esize ) !=esize )
    {
      ret=-2;
      goto endofquest;
    }
    credata.animid=the_creature.iwd2header.animid;
    memcpy(credata.scripts,the_creature.iwd2header.scripts,5*8);
    memcpy(credata.dialogresref,the_creature.iwd2header.dialogresref,8);
    revision=22;
    goto endofquest;
  }

  ret=-1;
endofquest:
  close(fh);
  return ret;
}

void Ccreature::order_items()
{
  int i,j;
  short tmp;
  creature_item tmpitem;
  int found[IWD2_SLOT_COUNT];

  memset(found,0,sizeof(found));
  for(i=0;i<slotcount;i++)
  {
    //clearing up possible crashers
    if((itemslots[i]>=the_creature.itemcount) || (itemslots[i]<-1))
    {
      itemslots[i]=-1;
    }
    else
    {
      if(found[itemslots[i]]) itemslots[i]=-1;
      else found[itemslots[i]]=1;
    }
  }
  for(i=0;i<slotcount-1;i++)
  {
    if(itemslots[i]<0) continue;
    for(j=i+1;j<slotcount;j++)
    {
      if(itemslots[j]<0) continue;
      if(itemslots[i]>itemslots[j])
      {
        tmp=itemslots[i];
        itemslots[i]=itemslots[j];
        itemslots[j]=tmp;
        tmpitem=items[itemslots[i]];
        items[itemslots[i]]=items[itemslots[j]];
        items[itemslots[j]]=tmpitem;
      }
    }
  }
}

int Ccreature::handle_iwd2_spells(int position, long offset, long count)
{
  int i;
  int total, remaining, unknown;
  int esize;
  int flg;

  flg=adjust_actpoint(offset);
  if(flg<0)
  {
    return flg;
  }
  iwd2_spells[position]=new creature_iwd2_spell[count];
  if(!iwd2_spells[position]) return -3;
  iwd2_counts[position]=count;
  esize=count*sizeof(creature_iwd2_spell);
  if(read(fhandle,iwd2_spells[position],esize)!=esize)
  {
    return -2;
  }
  if(read(fhandle,&iwd2_free[position],sizeof(iwd2_trail) )!=sizeof(iwd2_trail) )
  {
    return -2;
  }
  fullsize+=esize+sizeof(iwd2_trail);
  //mem total
  total=0;
  remaining=0;
  unknown=0;
  for(i=0;i<count;i++)
  {
    total+=iwd2_spells[position][i].total;
    remaining+=iwd2_spells[position][i].remaining;
    unknown+=iwd2_spells[position][i].unknown;
  }
  return flg;
}

int Ccreature::write_iwd2_spells(int fhandle)
{
  int position, esize;

  for(position=0;position<IWD2_SPELLCOUNT;position++)
  {
    esize=iwd2_counts[position]*sizeof(creature_iwd2_spell);
    if(write(fhandle,iwd2_spells[position],esize)!=esize)
    {
      return -2;
    }
    if(write(fhandle,&iwd2_free[position],sizeof(iwd2_trail) ) !=sizeof(iwd2_trail) )
    {
      return -2;
    }
  }
  return 0;
}

int Ccreature::calculate_iwd2_spells(int position, long &offset, long &maxcount, int &fullsize)
{
  int count, esize;

  count=iwd2_counts[position];
  maxcount=count;
  esize=count*sizeof(creature_iwd2_spell);
  offset=fullsize;
  fullsize+=esize+8;
  return 0;
}

int Ccreature::handle_iwd2()
{
  int flg, ret;
  int i,j,k;

  //copying the last part which is common
  KillIwd2Spells();
  memcpy(&header,&iwd2header,((char *) &iwd2header.crushac)-(char *) &iwd2header);
  memcpy(&header.crushac,&iwd2header.crushac,13);
  memcpy(&header.resfire,&iwd2header.resfire,12);
  memcpy(header.strrefs,iwd2header.strrefs,64*sizeof(int) );
  header.strstat=iwd2header.strstat;
  header.strbon=0;
  memcpy(&header.intstat,&iwd2header.intstat,5);
  memcpy(header.scripts,iwd2header.scripts,sizeof(iwd2header.scripts) );
  memcpy(&header.idsea,&iwd2header.idsea,((unsigned char *) iwd2header.unknown8)-&iwd2header.idsea);
  memcpy(&header.itemslots,&iwd2header.itemslots,5*sizeof(long)+8);
  k=0;
  flg=0;
  for(i=0;i<IWD2_SPSLOTNUM;i++) //7
  {
    for(j=0;j<9;j++)
    {
      ret=handle_iwd2_spells(k++,iwd2header.spelloffs[i][j],iwd2header.spellcnt[i][j]);
      if(ret<0)
      {
        return ret;
      }
      flg|=ret;
    }
  }
  for(j=0;j<9;j++)
  {
    ret=handle_iwd2_spells(k++,iwd2header.domainoffs[j],iwd2header.domaincnt[j]);
    if(ret<0) return ret;
    flg|=ret;
  }
  ret=handle_iwd2_spells(k++,iwd2header.innateoffs,iwd2header.innatecnt);
  if(ret<0) return ret;
  flg|=ret;
  ret=handle_iwd2_spells(k++,iwd2header.songoffs,iwd2header.songcnt);
  if(ret<0) return ret;
  flg|=ret;
  ret=handle_iwd2_spells(k++,iwd2header.shapeoffs,iwd2header.shapecnt);
  if(ret<0) return ret;
  flg|=ret;  
  return flg;
}

int Ccreature::calculate_iwd2(int &fullsize)
{
  int flg, ret;
  int i,j,k;
//IWD2_SPELLCOUNT
  //copying the last part which is common
//  KillIwd2Spells();
  memcpy(&iwd2header,&header,((char *) &iwd2header.sfort)-(char *) &iwd2header);
  memcpy(iwd2header.scripts,header.scripts,sizeof(iwd2header.scripts) );
  memcpy(&iwd2header.idsea,&header.idsea,((unsigned char *) iwd2header.unknown8)-&iwd2header.idsea);
  memcpy(&iwd2header.itemslots,&header.itemslots,5*sizeof(long)+8);
  k=0;
  flg=0;
  for(i=0;i<IWD2_SPSLOTNUM;i++) //7
  {
    for(j=0;j<9;j++)
    {
      ret=calculate_iwd2_spells(k++,iwd2header.spelloffs[i][j],iwd2header.spellcnt[i][j],fullsize);
      if(ret<0)
      {
        return ret;
      }
      flg|=ret;
    }
  }
  for(j=0;j<9;j++)
  {
    ret=calculate_iwd2_spells(k++,iwd2header.domainoffs[j],iwd2header.domaincnt[j],fullsize);
    if(ret<0) return ret;
    flg|=ret;
  }
  ret=calculate_iwd2_spells(k++,iwd2header.innateoffs,iwd2header.innatecnt,fullsize);
  if(ret<0) return ret;
  flg|=ret;
  ret=calculate_iwd2_spells(k++,iwd2header.songoffs,iwd2header.songcnt,fullsize);
  if(ret<0) return ret;
  flg|=ret;
  ret=calculate_iwd2_spells(k++,iwd2header.shapeoffs,iwd2header.shapecnt,fullsize);
  if(ret<0) return ret;
  flg|=ret;  
  return flg;
}

int Ccreature::ReadCreatureFromFile(int fh, long ml)
{
  int flg, ret;
  int esize;

  if(fh<1) return -1;
  fhandle=fh; //for safe string reads
  if(ml==-1) maxlen=filelength(fhandle);
  else maxlen=ml;
  if(maxlen<1) return -1; //short file, invalid item
  startpoint=tell(fhandle);
  ischr=0;
redo:
  if(read(fhandle,&header,8 )!=8 )
  {	
  	return -2; // short file, invalid item
  }
  if(memcmp(header.filetype,"CRE ",4) )
  {
    if(!memcmp(header.filetype,"CHR ",4) )
    {
      memcpy(&chrheader,header.filetype,8);
      read(fhandle, ((BYTE *) &chrheader)+8,sizeof(chrheader)-8);
      maxlen-=sizeof(chrheader);
      startpoint=tell(fhandle);
      ischr=1;
      goto redo;
    }
	  return -2;
  }

  //pst: v1.2,  iwd: v9.0 iwd2: v2.2
  if(!memcmp(header.revision,"V1.0",4) ) 
  {
    esize=sizeof(creature_header)-8;
    if(read(fhandle,((BYTE *) &header)+8,esize ) !=esize )
    {
      return -2;
    }
    revision=10;
  }
  else if(!memcmp(header.revision,"V1.2",4) || !memcmp(header.revision,"V1.1",4) )
  {
    esize=sizeof(creature_pst_header)-8;
    if(read(fhandle,((BYTE *) &pstheader)+8,esize ) !=esize )
    {
      return -2;
    }
    if(header.revision[3]=='1') revision=11;
    else revision=12;
    memcpy(&header,&pstheader,pstheader.unknown270-(char *) &pstheader);
    memcpy(&header.idsea,&pstheader.idsea,header.dialogresref-(char *) &header.idsea+8);
    if(header.memcnt==65536)
    {
      header.memcnt=0;
    }
  }
  else if(!memcmp(header.revision,"V9.0",4) )
  {
    esize=sizeof(creature_iwd_header)-8;
    if(read(fhandle,((BYTE *) &iwdheader)+8,esize ) !=esize )
    {
      return -2;
    }
    revision=90;
    memcpy(&header,&iwdheader,iwdheader.unknown270-(char *) &iwdheader);
    memcpy(&header.idsea,&iwdheader.idsea,header.dialogresref-(char *) &header.idsea+8);
  }
  else if(!memcmp(header.revision,"V2.2",4) )
  {
    memcpy(&iwd2header,&header,8);
    esize=sizeof(creature_iwd2_header)-8;
    if(read(fhandle,((BYTE *) &iwd2header)+8,esize ) !=esize )
    {
      return -2;
    }
    revision=22;
  }
  else
  {
    return -1;
  }
  fullsize=esize+8;
  
  if(revision==22)
  { 
    ret=handle_iwd2(); //we read in the special headers, and convert header
    if(ret<0) return ret;
  }
  else
  {
    //creature spellbook
    flg=adjust_actpoint(header.bookoffs);
    if(flg<0) return flg;
    ret=flg;
    if(header.bookcnt!=bookcount)
    {
      KillBooks();
      books=new creature_book[header.bookcnt];
      if(!books) return -3;
      bookcount=header.bookcnt;
    }
    esize=bookcount*sizeof(creature_book);
    if(read(fhandle, books, esize )!=esize )
    {
      return -2;
    }
    fullsize+=esize;
    
    //creature selected spells
    flg=adjust_actpoint(header.selectoffs);
    if(flg<0) return flg;
    ret|=flg;
    if(header.selectcnt!=selectcount)
    {
      KillSelects();
      selects=new creature_select[header.selectcnt];
      if(!selects) return -3;
      selectcount=header.selectcnt;
    }
    esize=selectcount*sizeof(creature_select);
    if(read(fhandle, selects, esize)!=esize )
    {
      return -2;
    }
    fullsize+=esize;
    
    //creature memory
    flg=adjust_actpoint(header.memoffs);
    if(flg<0) return flg;
    ret|=flg;
    if(header.memcnt!=memocount)
    {
      KillMemos();
      memos=new creature_memory[header.memcnt];
      if(!memos) return -3;
      memocount=header.memcnt;
    }
    esize=memocount*sizeof(creature_memory);
    if(read(fhandle, memos, esize )!=esize )
    {
      return -2;
    }
    fullsize+=esize;
  }
  //resume to common part
  
  //creature effects
  flg=adjust_actpoint(header.effectoffs);
  if(flg<0) return flg;
  ret|=flg;

  //delete the previous block always coz this is getting complicated
  KillEffects();
  if(header.effbyte==1)
  {
    effects=new creature_effect[header.effectcnt];
    if(!effects) return -3;
    effectcount=header.effectcnt;
    esize=effectcount*sizeof(creature_effect);
    if(read(fhandle, effects, esize )!=esize )
    {
      return -2;
    }
  }
  else
  {
    oldeffects=new feat_block[header.effectcnt];
    if(!oldeffects) return -3;
    effectcount=header.effectcnt;
    esize=effectcount*sizeof(feat_block);
    if(read(fhandle, oldeffects, esize )!=esize )
    {
      return -2;
    }
  }
  fullsize+=esize;

  //creature items
  flg=adjust_actpoint(header.itemoffs);
  if(flg<0) return flg;
  ret|=flg;
  if(header.itemcnt!=itemcount)
  {
    KillItems();
    items=new creature_item[header.itemcnt];
    if(!items) return -3;
    itemcount=header.itemcnt;
  }
  esize=itemcount*sizeof(creature_item);
  if(read(fhandle, items, esize )!=esize )
  {
    return -2;
  }
  fullsize+=esize;

  if(header.itemslots)
  {
    flg=adjust_actpoint(header.itemslots);
    if(flg<0) return flg;
    ret|=flg;
    switch(revision)
    {
    case 11: //old PST
      slotcount = SLOT_COUNT;
      revision=12;
      break;
    case 12: slotcount = PST_SLOT_COUNT; break;
    case 22: slotcount = IWD2_SLOT_COUNT; break;
    default: slotcount = SLOT_COUNT; break;
    }
    esize=slotcount*sizeof(short)+sizeof(long);
    if(read(fhandle,&itemslots,esize )!=esize )
    {
      return -2;
    }
    fullsize+=esize;
  }
  order_items();
  if(maxlen!=fullsize)
  {
    return 1; //incorrect length
  }

  return ret;
}
