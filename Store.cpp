// store.cpp: implementation of the Cstore class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <fcntl.h>

#include "chitem.h"
#include "store.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Cstore::Cstore()
{
  entries=NULL;
  entrynum=0;
  itemtypes=NULL;
  itemtypenum=0;
  drinks=NULL;
  drinknum=0;
  cures=NULL;
  curenum=0;
  fhandle=0;
  startpoint=-1;
  maxlen=-1;
  revision=10;
}

Cstore::~Cstore()
{
  KillHeaders();
  KillItemTypes();
  KillDrinks();
  KillCures();
}

int Cstore::adjust_actpoint(long offset)
{
  if(actpoint()==offset) return 0;
  if(maxlen<offset) return -2;
  if(myseek(offset)!=offset) return -2;
  return 1;
}

int Cstore::WriteStoreToFile(int fhandle, int calculate)
{
  int i;
  int esize;
  int fullsize;

  //drink offset
  fullsize=sizeof(store_header);
  switch(revision)
  {
  case 90:
    memcpy(header.filetype,"STORV9.0",8);
    fullsize+=sizeof(iwd2store_header);
    break;
  case 10:
    memcpy(header.filetype,"STORV1.0",8);
    break;
  case 11:
    memcpy(header.filetype,"STORV1.1",8); //has different stored item structure
    break;
  }
  header.drinkoffset=fullsize;
  //cure offset
  fullsize+=header.drinkcount*sizeof(store_drink);
  header.cureoffset=fullsize;
  //purchased offset
  fullsize+=header.curecount*sizeof(store_cure);
  header.pitemoffset=fullsize;
  //stored items offset
  fullsize+=header.pitemcount*sizeof(long);
  header.offset=fullsize;
  if(revision==11) fullsize+=header.itemcount*sizeof(store_item_entry11);
  else fullsize+=header.itemcount*sizeof(store_item_entry);

  if(calculate)
  {
    return fullsize;
  }
  if(fhandle<1) return -1;
  if(write(fhandle, &header,sizeof(store_header) )!=sizeof(store_header) )
  {
    return -2;
  }
  if(revision==90)
  {
    if(write(fhandle,&iwd2header,sizeof(iwd2store_header) )!=sizeof(iwd2store_header) )
    {
      return -2;
    }
  }
  //drink
  esize=header.drinkcount*sizeof(store_drink);
  if(write(fhandle,drinks,esize)!=esize)
  {
    return -2;
  }
  //cure
  esize=header.curecount*sizeof(store_cure);
  if(write(fhandle,cures,esize)!=esize)
  {
    return -2;
  }
  //purchased items
  esize=header.pitemcount*sizeof(long);
  if(write(fhandle,itemtypes,esize)!=esize)
  {
    return -2;
  }
  //stored items, doing it one by one because of the different
  //structure sizes
  if(revision==11) esize=sizeof(store_item_entry11);
  else esize=sizeof(store_item_entry);
  for(i=0;i<header.itemcount;i++)
  {
    if(write(fhandle,entries+i,esize)!=esize)
    {
      return -2;
    }
  }
  m_changed=false;
  return 0;
}

long Cstore::RetrieveNameRef(int fh)
{
  int required;

  if(fh<1) return -1;
  required=(BYTE *) &header.strref-(BYTE *) &header+sizeof(long);
  if(read(fh, &header, required)!=required)
  {
    close(fh);
    return -1;
  }
  close(fh);
  return header.strref;
}

int Cstore::ReadStoreFromFile(int fh, int ml)
{
  int i;
  int esize;
  int fullsize;

  m_changed=false;
  if(fh<1) return -1;
  fhandle=fh;
  if(ml==-1) maxlen=filelength(fhandle);
  else maxlen=ml;
  if(maxlen<1) return -2; //short file, invalid item
  startpoint=tell(fhandle);
  if(read(fhandle,&header,sizeof(store_header) )!=sizeof(store_header) )
  {	
  	return -2; // short file, invalid item
  }
  if(memcmp(header.filetype,"STOR",4) )
  {
	  return -2;
  }
  if(header.revision[0]!='V')
  {
	  return -2;
  }  
  if(header.revision[1]=='9')
  {
    revision=90;
    if(read(fhandle,&iwd2header,sizeof(iwd2store_header) )!=sizeof(iwd2store_header) )
    {	
  	  return -2; // short file, invalid item
    }
    fullsize=sizeof(header)+sizeof(iwd2header);
  }
  else
  {
    fullsize=sizeof(header);
    if(header.revision[3]=='1') revision=11;
    else revision=10;
  }

  //drinks
  adjust_actpoint(header.drinkoffset);
  esize=header.drinkcount*sizeof(store_drink);
  if(drinknum!=header.drinkcount)
  {
    KillDrinks();
    drinks=new store_drink[header.drinkcount];
    if(!drinks) return -3;
    memset(drinks,0,esize);
  }
  drinknum=header.drinkcount;
  if(read(fhandle,drinks,esize)!=esize)
  {
    return -2;
  }
  fullsize+=esize;

  //items, doing it one by one because of the different structure sizes
  adjust_actpoint(header.offset);
  if(entrynum!=header.itemcount)
  {
    KillHeaders();
    //allocating the bigger structure always, so switching between them
    //is no problem, only reading/writing needs to be done one by one
    entries=new store_item_entry11[header.itemcount];
    if(!entries) return -3;
    memset(entries,0,header.itemcount*sizeof(store_item_entry11));
  }
  entrynum=header.itemcount;

  if(revision==11) esize=sizeof(store_item_entry11);
  else esize=sizeof(store_item_entry);
  for(i=0;i<entrynum;i++)
  {
    memset(entries+i,0,sizeof(store_item_entry11) );
    if(read(fhandle,entries+i,esize)!=esize)
    {
      return -2;
    }
    fullsize+=esize;
  }

//purchased itemtypes
  adjust_actpoint(header.pitemoffset);
  esize=header.pitemcount*sizeof(long);
  if(itemtypenum!=header.pitemcount)
  {
    KillItemTypes();
    itemtypes=new long[header.pitemcount];
    if(!itemtypes) return -3;
    memset(itemtypes,0,esize);
  }
  itemtypenum=header.pitemcount;
  if(read(fhandle,itemtypes,esize)!=esize)
  {
    return -2;
  }
  fullsize+=esize;

//cure spells
  adjust_actpoint(header.cureoffset);
  esize=header.curecount*sizeof(store_cure);
  if(curenum!=header.curecount)
  {
    KillCures();
    cures=new store_cure[header.curecount];
    if(!cures) return -3;
    memset(cures,0,esize);
  }
  curenum=header.curecount;
  if(read(fhandle,cures,esize)!=esize)
  {
    return -2;
  }
  fullsize+=esize;

  if(fullsize!=maxlen)
  {
    return -1;
  }
  return 0;
}
