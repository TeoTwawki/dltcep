// tbg.cpp: implementation of the Ctbg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <fcntl.h>
#include <sys/stat.h>
#include "chitem.h"
#include "options.h"
#include "compat.h" //bothdialog
#include "tbg.h"
#include "TextView.h"
#include "MyFileDialog.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Ctbg::Ctbg()
{
  tlkentries=NULL;
  strrefs=NULL;
  ascii=NULL;
  filedata=NULL;
  textdata=NULL;
  iapfileheaders=NULL;
  m_tbgnames=NULL;
  m_othernames=NULL;

  tlkentrycount=0;
  strrefcount=0;
  asciicount=0;
  textdatasize=0;
  iapfilecount=actfilecount=0;
  tbgnamecount=othernamecount=0;
}

Ctbg::~Ctbg()
{ 
  new_iap();
}

void Ctbg::new_iap()
{
  KillTlkEntries();
  KillStrrefs();
  KillAscii();
  KillFileData();
  KillTextData();
  KillIapFileHeaders();
  KillTbgFilenames();
  KillOtherFilenames();
  memset(&header,0,sizeof(header) );
  memset(&iapheader,0,sizeof(iapheader) );
}

int Ctbg::get_genre()
{
  if(iwd2_structures()) return 5; //IWD2
  else if(pst_compatible_var()) return 2; //PST
  else if(has_xpvar()) return 3; //IWD
  else if(bg1_compatible_area()) return 1; //BG1
  else return 4; //BG2
}

int Ctbg::AllocateHeaders(int tbgnum, int othernum)
{
  if(tbgnamecount!=tbgnum)
  {
    KillTbgFilenames();
    m_tbgnames=new CString[tbgnum];
    if(!m_tbgnames) return -3;
    tbgnamecount=tbgnum;
  }
  if(othernamecount!=othernum)
  {
    KillOtherFilenames();
    m_othernames=new CString[othernum];
    if(!m_othernames) return -3;
    othernamecount=othernum;
  }
  if(iapfilecount!=tbgnum+othernum)
  {
    KillIapFileHeaders();
    iapfileheaders=new iap_file_header[tbgnum+othernum];
    if(!iapfileheaders) return -3;
    iapfilecount=tbgnum+othernum;
  }
  memset(&iapheader,0,sizeof(iapheader));
  iapheader.othercount=othernum;
  iapheader.tbgcount=tbgnum;
  return 0;
}

int Ctbg::AddTbgFile(int pos, CString &newname)
{
  int pos1, pos2;
  CString tmpstr;

  iapfileheaders[pos].filelength=file_length(m_tbgnames[pos]);
  if(iapfileheaders[pos].filelength<1) return -2;
  pos1=m_tbgnames[pos].ReverseFind('\\')+1;
  pos2=m_tbgnames[pos].ReverseFind('/')+1;
  if(pos1<pos2) pos1=pos2;
  newname=m_tbgnames[pos].Mid(pos1);
  return 0;
}

int Ctbg::AddOtherFile(int pos, CString &newname)
{
  iapfileheaders[pos+iapheader.tbgcount].filelength=file_length(m_othernames[pos]);
  if(iapfileheaders[pos+iapheader.tbgcount].filelength<1) return -2;
  newname=m_othernames[pos].Mid(bgfolder.GetLength());
  return 0;
}

#define BUFFSIZE  65536l

int Ctbg::AddToFile(int fhandle, CString filename, CString internalname, int ml)
{
  char *tmpmem;
  int fhandle2;
  int size;
  int ret;

  if(write(fhandle,internalname,internalname.GetLength())!=internalname.GetLength())
  {
    return -2;
  }
  fhandle2=open(filename,O_RDONLY|O_BINARY);
  if(fhandle2<1) return -2;    
  ret=0;
  tmpmem=new char[BUFFSIZE];
  if(!tmpmem)
  {
    ret=-3;
    goto endofquest;
  }

  while(ml)
  {
    size=min(BUFFSIZE,ml);
    if(read(fhandle2,tmpmem,size)!=size)
    {
      ret=-2;
      goto endofquest;
    }
    if(write(fhandle,tmpmem,size)!=size)
    {
      ret=-2;
      goto endofquest;
    }
    ml-=size;
  }
endofquest:
  close(fhandle2);
  if(tmpmem) delete [] tmpmem;
  return ret;
}

int Ctbg::WriteZip(int fhandle)
{
  CString *tbgnames, *othernames;
  int i;
  int ret;

  ret=0;
  memcpy(zipheader.signature,"PK\3\4",4);
  zipheader.headersize=(unsigned short) (sizeof(zipheader)-6); //20

  othernames=tbgnames=NULL;  
  if(tbgnamecount!=iapheader.tbgcount) return -1;
  if(othernamecount!=iapheader.othercount) return -1;

  tbgnames=new CString[iapheader.tbgcount];
  if(!tbgnames)
  {
    return -3; //no need of quest
  }
  othernames=new CString[iapheader.othercount];
  if(!othernames)
  {
    ret=-3;
    goto endofquest;
  }

  for(i=0;i<iapheader.tbgcount;i++)
  {
    if(AddTbgFile(i,tbgnames[i]))
    {
      ret=-2;
      goto endofquest;
    }
  }
  for(i=0;i<iapheader.othercount;i++)
  {
    if(AddOtherFile(i,othernames[i]))
    {
      ret=-2;
      goto endofquest;
    }
  }
  if(write(fhandle,&zipheader,sizeof(zipheader) )!=sizeof(zipheader) )
  {
    ret=-2;
    goto endofquest;
  }

endofquest:
  if(tbgnames) delete [] tbgnames;
  if(othernames) delete [] othernames;
  return ret;
}

int Ctbg::WriteIap(int fhandle)
{
  CString tmpstr;
  CString *tbgnames, *othernames;
  int esize;
  int i;
  int ret;

  ret=0;
  memcpy(iapheader.signature,"IAP",4);
  iapheader.genre=(unsigned char) get_genre();

  othernames=tbgnames=NULL;
  esize=iapfilecount*sizeof(iap_file_header);
  iapheader.totallen=sizeof(iap_header)+esize;
  if(tbgnamecount!=iapheader.tbgcount) return -1;
  if(othernamecount!=iapheader.othercount) return -1;

  tbgnames=new CString[iapheader.tbgcount];
  if(!tbgnames)
  {
    return -3; //no need of quest
  }
  othernames=new CString[iapheader.othercount];
  if(!othernames)
  {
    ret=-3;
    goto endofquest;
  }

  for(i=0;i<iapheader.tbgcount;i++)
  {
    if(AddTbgFile(i,tbgnames[i]))
    {
      ret=-2;
      goto endofquest;
    }
  }
  for(i=0;i<iapheader.othercount;i++)
  {
    if(AddOtherFile(i,othernames[i]))
    {
      ret=-2;
      goto endofquest;
    }
  }

  for(i=0;i<iapfilecount;i++)
  {
    iapfileheaders[i].nameoffset=iapheader.totallen;
    if(i<tbgnamecount)
    {
      iapfileheaders[i].namelength=tbgnames[i].GetLength();
    }
    else
    {
      iapfileheaders[i].namelength=othernames[i-tbgnamecount].GetLength();
    }
    iapheader.totallen+=iapfileheaders[i].namelength;
    iapfileheaders[i].fileoffset=iapheader.totallen;
    iapheader.totallen+=iapfileheaders[i].filelength;
  }

  if(write(fhandle,&iapheader,sizeof(iapheader) )!=sizeof(iapheader) )
  {
    ret=-2;
    goto endofquest;
  }
  esize=iapfilecount*sizeof(iap_file_header);
  if(write(fhandle,iapfileheaders,esize)!=esize)
  {
    ret=-2;
    goto endofquest;
  }

  for(i=0;i<iapheader.tbgcount;i++)
  {
    ret=AddToFile(fhandle, m_tbgnames[i], tbgnames[i], iapfileheaders[i].filelength);
    if(ret<0) goto endofquest;
  }
  for(i=0;i<iapheader.othercount;i++)
  {
    ret=AddToFile(fhandle,m_othernames[i], othernames[i], iapfileheaders[i+iapheader.tbgcount].filelength);
    if(ret<0) goto endofquest;
  }
endofquest:
  if(tbgnames) delete [] tbgnames;
  if(othernames) delete [] othernames;
  return ret;
}

int Ctbg::read_iap(int fhandle, int onlyopen, CStringList &filelist)
{
  char *filedata;
  char tmpfilename[MAX_PATH];
  CString outfilepath, syscommand;
  int esize, iapfullsize;
  int fhandle2;
  int ret;
  int hasweidu;
  int maxlen;

  hasweidu=1;
  maxlen=filelength(fhandle);
  if(maxlen<sizeof(iapheader) ) return -1; //short file, invalid item
  if(read(fhandle,&iapheader,sizeof(iapheader) )!=sizeof(iapheader) )
  {
    return -2;
  }
  if(memcmp(iapheader.signature,"IAP",3) ) return -4; // not iap
  if(iapheader.totallen!=maxlen) return -1; //iap total length is invalid
  actfilecount=0;
  iapfullsize=sizeof(iapheader);
  
  KillIapFileHeaders();
  iapfileheaders=new iap_file_header[iapheader.tbgcount+iapheader.othercount];
  if(!iapfileheaders) return -3;
  iapfilecount=iapheader.tbgcount+iapheader.othercount;
  esize=iapfilecount*sizeof(iap_file_header);
  if(read(fhandle,iapfileheaders,esize)!=esize)
  {
    return -2;
  }
  iapfullsize+=esize;
  if(onlyopen)
  {
    AllocateHeaders(iapheader.tbgcount,iapheader.othercount);
  }

  //handle tbg files, iap files
  for(actfilecount=0;actfilecount<iapfilecount;actfilecount++)
  {
    if(iapfileheaders[actfilecount].nameoffset!=iapfullsize) return -2;
    esize=iapfileheaders[actfilecount].namelength;
    iapfullsize+=esize;
    if(iapfullsize>maxlen) return -2;
    if(esize>=MAX_PATH) return -3;
    if(read(fhandle,tmpfilename,esize)!=esize) return -2;
    outfilepath=bgfolder+CString(tmpfilename,esize);
    
    if(iapfileheaders[actfilecount].fileoffset!=iapfullsize) return -2;
    esize=iapfileheaders[actfilecount].filelength;
    iapfullsize+=esize;
    if(iapfullsize>maxlen) return -2;
    if(actfilecount<iapheader.tbgcount)
    { //tbg file
      if(onlyopen)
      {
        m_tbgnames[actfilecount]=outfilepath;
        lseek(fhandle,esize,SEEK_CUR);
        continue;
      }
      ret=read_tbg(fhandle,esize);
      if(ret<0) return ret;
      if(ret==9) return 0;  //handled in read_tbg already
      ret=ImportFile();
      if(ret<0) return ret;
      continue;
    }
    //plain file
    if(onlyopen)
    {
      m_othernames[actfilecount-iapheader.tbgcount]=outfilepath;
      lseek(fhandle,esize,SEEK_CUR);
      continue;
    }
    filedata=new char[esize+1];
    if(!filedata) return -3;
    filedata[esize]=0; //for using it as a string
    if(read(fhandle,filedata,esize)!=esize)
    {
      delete [] filedata;
      return -2;
    }
    fhandle2=creat(outfilepath,S_IWRITE);
    if(fhandle2<1) return -2;
    if(write(fhandle2,filedata,esize)!=esize) ret=-2;
    else ret=0;
    close(fhandle2);
    if(ret<0)
    {
      delete [] filedata;
      return ret;
    }
    if((iapfileheaders[actfilecount].launchflag&2) && hasweidu)
    {
      delete [] filedata;
      if(weidupath.IsEmpty())
      {
        MessageBox(0,"Can't import dialog source without WeiDU installed!","Warning",MB_ICONEXCLAMATION|MB_OK);
        hasweidu=0;
        continue;
      }
      filelist.AddTail(outfilepath);
      continue;
    }
    if((iapfileheaders[actfilecount].launchflag&1) )
    {
      //if(esize>4096) filedata[4096]=0;
      //MessageBox(0,filedata,"IAP import",MB_OK);
      CTextView dlg;
      dlg.m_text=filedata;
      dlg.m_file=outfilepath;
      dlg.DoModal();
      delete [] filedata;
      continue;
    }
    delete [] filedata;
  }

  if(iapfullsize!=maxlen) return -1;
  return 0;
}

int Ctbg::patch_baldur_gam(int fhandle)
{
  npc_header tbgnentry;
  int npccount;
  int ret;

  if(read(fhandle,&tbgnheader.unused,sizeof(tbgnheader)-4)!=sizeof(tbgnheader)-4)
  {
    return -2;
  }
  if(tbgnheader.npcoffset!=sizeof(tbgnheader))
  {
    return -2;
  }
  if(read_game("BALDUR"))
  {
    return -6; //current ie engine doesn't support npcs (only bg2/bg1)
  }
  for(npccount=0;npccount<tbgnheader.npccount;npccount++)
  {
    if(read(fhandle,&tbgnentry,sizeof(npc_header) )!=sizeof(npc_header) )
    {
      return -2;
    }
    ret=the_game.AddNpc(tbgnentry.npcname,tbgnentry.area, tbgnentry.x, tbgnentry.y);
    if(ret<0) return ret;
  }
  return write_game("BALDUR","");
}

int Ctbg::read_tbg(int fhandle, int maxlen)
{
  tbg3_tlk_reference tmpentry;
  int ret;
  int esize, fullsize;
  int i;

  memset(&header,0,sizeof(header));
  if(maxlen==-1) maxlen=filelength(fhandle);
  if(maxlen<4 ) return -1; //short file, invalid item
  if(read(fhandle,&header,4 )!=4 )
  {
    return -2;
  }
  if(!memcmp(header.signature,"TBGN",4) )
  { 
    ret=patch_baldur_gam(fhandle);
    close(fhandle);
    if(ret<0) return ret;
    return 9;
  }
  if(!memcmp(header.signature,"TBGC",4) )
  {
    fullsize=sizeof(tbg4_header);
    revision=12;
  }
  else if(!memcmp(header.signature,"TBG3",4) )
  {
    fullsize=sizeof(tbg3_header);
    revision=3;
  }
  else
  {
    if(memcmp(header.signature,"TBG4",4) )
    {
      if(header.signature[0]==0x28)
      {
        fullsize=0x28;
        revision=1;
      }
      else
      {
        if(header.signature[0]==0x35)
        {
          fullsize=0x35;
          revision=2;
        }
        else return -4; // not tbg4
      }
    }
    else
    {
      fullsize=sizeof(tbg4_header);
      revision=4;
    }
  }
  if(revision<3)
  {
// we must know the original filename
    if(read(fhandle,&header.fileoffset,fullsize-4)!=fullsize-4)
    {
      return -2;
    }
    return -5;
  }
  else
  {
    if(read(fhandle,&header.unused,fullsize-4)!=fullsize-4)
    {
      return -2;
    }
  }

  if(header.tlkentryoffset!=fullsize) return -2;
  if(header.tlkentrycount!=tlkentrycount)
  {
    KillTlkEntries();
    tlkentries=new tbg_tlk_reference[header.tlkentrycount];
    if(!tlkentries) return -3;
    tlkentrycount=header.tlkentrycount;
  }
  switch(revision)
  {
  case 4:case 12:
    esize=tlkentrycount*sizeof(tbg_tlk_reference);
    if(read(fhandle,tlkentries,esize)!=esize)
    {
      return -2;
    }
    fullsize+=esize;
    break;
  case 3:
    for(i=0;i<header.tlkentrycount;i++)
    {
      esize=sizeof(tbg3_tlk_reference);
      read(fhandle,&tmpentry,esize);
      memset(tlkentries+i,0,sizeof(tbg_tlk_reference) );
      tlkentries[i].offset=tmpentry.offset;
      tlkentries[i].length=tmpentry.length;
      memcpy(tlkentries[i].soundref,tmpentry.soundref,8);
      tlkentries[i].pitch=tmpentry.insert; //another hack
      fullsize+=esize;
    }
    break;
  default:
    return -2;
  }
  
  if(revision>=4)
  {
    if(header.strrefoffset!=fullsize) return -2;
    if(header.strrefcount!=strrefcount)
    {
      KillStrrefs();
      strrefs=new unsigned long[header.strrefcount];
      if(!strrefs) return -3;
      strrefcount=header.strrefcount;
    }
    esize=strrefcount*sizeof(long);
    if(read(fhandle,strrefs,esize)!=esize)
    {
      return -2;
    }
    fullsize+=esize;
    
    if(header.asciicount) //plain hack, but harmless
    {
      if(header.asciioffset!=fullsize) return -2;
      if(header.asciioffset!=asciicount)
      {
        KillAscii();
        ascii=new long[header.asciicount];
        if(!ascii) return -3;
        asciicount=header.asciicount;
      }
      esize=asciicount*sizeof(long);
      if(read(fhandle,ascii,esize)!=esize)
      {
        return -2;
      }
      fullsize+=esize;
    }
    else KillAscii();
  }
  if(header.fileoffset!=fullsize) return -2;
  KillFileData();
  filedata=new char[header.filelength];
  if(!filedata) return -3;
  if(read(fhandle,filedata,header.filelength)!=header.filelength)
  {
    return -2;
  }
  fullsize+=header.filelength;

  if(header.textentryoffset!=fullsize) return -2;
  if(maxlen<fullsize) return -2;
  KillTextData();
  textdatasize=maxlen-fullsize;
  textdata=new char[textdatasize];
  if(!textdata) return -3;
  if(read(fhandle,textdata,textdatasize)!=textdatasize)
  {
    return -2;
  }
  return 0;
}

int Ctbg::get_strref(long offset, long length, CString &text)
{
  if(textdatasize<offset+length) return -2;
  text=CString(textdata+offset,length);
  return 0;
}

#define calc_offset(poi, field) ( (char *) &(poi.field) - (char *) &(poi))
#define calc_offset2(poi, field) ( (char *) &(field) - (char *) &(poi))
#define calc_offset3(poi, field) ( (char *) &(field) - (char *) (poi))

int Ctbg::collect_itemrefs()
{
  int *strref_opcodes;
  int pos;
  int count;
  int i;

  strref_opcodes=get_strref_opcodes();
  pos=0;
  count=4; //item header contains 4 strrefs
  for(i=0;i<the_item.featblkcount;i++)//not header, we need ALL feature blocks
  {
    if(member_array(the_item.featblocks[i].feature,strref_opcodes)!=-1)
    {
      count++;
    }
  }
  tlkentries=new tbg_tlk_reference[count];
  if(!tlkentries) return -3;
  tlkentrycount=count;        //physical size
  strrefs=new unsigned long [count];
  if(!strrefs)
  {
    delete [] tlkentries;
    return -3;
  }
  strrefcount=count;

  strrefs[pos]=calc_offset(the_item.header, unidref);
  if(!resolve_tbg_entry(the_item.header.unidref,tlkentries[pos])) pos++;
  strrefs[pos]=calc_offset(the_item.header, idref);
  if(!resolve_tbg_entry(the_item.header.idref,tlkentries[pos])) pos++;
  strrefs[pos]=calc_offset(the_item.header, uniddesc);
  if(!resolve_tbg_entry(the_item.header.uniddesc,tlkentries[pos])) pos++;
  strrefs[pos]=calc_offset(the_item.header, iddesc);
  if(!resolve_tbg_entry(the_item.header.iddesc,tlkentries[pos])) pos++;
  for(i=0;i<the_item.featblkcount;i++)
  {
    if(member_array(the_item.featblocks[i].feature,strref_opcodes)!=-1)
    {
      strrefs[pos]=the_item.header.featureoffs+calc_offset2(the_item.featblocks[0],the_item.featblocks[i].par1.parl);
      resolve_tbg_entry(the_item.featblocks[i].par1.parl,tlkentries[pos++]);
    }
  }
  header.tlkentrycount=pos;   //logical size
  header.strrefcount=pos;
  return 0;
}

int Ctbg::collect_spellrefs()
{
  int *strref_opcodes;
  int pos;
  int count;
  int i;

  strref_opcodes=get_strref_opcodes();
  pos=0;
  count=4; //item header contains 4 strrefs
  for(i=0;i<the_spell.featblkcount;i++)//not header, we need ALL feature blocks
  {
    if(member_array(the_spell.featblocks[i].feature,strref_opcodes)!=-1)
    {
      count++;
    }
  }
  tlkentries=new tbg_tlk_reference[count];
  if(!tlkentries) return -3;
  tlkentrycount=count;        //physical size
  strrefs=new unsigned long[count];
  if(!strrefs)
  {
    delete [] tlkentries;
    return -3;
  }
  strrefcount=count;

  strrefs[pos]=calc_offset(the_spell.header, desc);
  if(!resolve_tbg_entry(the_spell.header.desc,tlkentries[pos])) pos++;
  strrefs[pos]=calc_offset(the_spell.header, spellname);
  if(!resolve_tbg_entry(the_spell.header.spellname,tlkentries[pos])) pos++;
//  strrefs[pos]=calc_offset(the_spell.header, unknown4c);
//  if(!resolve_tbg_entry(the_spell.header.unknown4c,tlkentries[pos])) pos++;
  strrefs[pos]=calc_offset(the_spell.header, idname);
  if(!resolve_tbg_entry(the_spell.header.idname,tlkentries[pos])) pos++;

  for(i=0;i<the_spell.featblkcount;i++)
  {
    if(member_array(the_spell.featblocks[i].feature,strref_opcodes)!=-1)
    {
      strrefs[pos]=the_spell.header.featureoffs+calc_offset2(the_spell.featblocks[0],the_spell.featblocks[i].par1.parl);
      resolve_tbg_entry(the_spell.featblocks[i].par1.parl,tlkentries[pos++]);
    }
  }
  header.tlkentrycount=pos;   //logical size
  header.strrefcount=pos;
  return 0;
}

int Ctbg::collect_storerefs()
{
  int pos;
  int count;
  int i;

  pos=0;
  count=1+the_store.header.drinkcount;
  tlkentries=new tbg_tlk_reference[count];
  if(!tlkentries) return -3;
  tlkentrycount=count;        //physical size
  strrefs=new unsigned long[count];
  if(!strrefs)
  {
    delete [] tlkentries;
    return -3;
  }
  strrefcount=count;

  strrefs[pos]=calc_offset(the_store.header, strref);
  if(!resolve_tbg_entry(the_store.header.strref,tlkentries[pos])) pos++;
  for(i=0;i<the_store.header.drinkcount;i++)
  {
    strrefs[pos]=the_store.header.drinkoffset+calc_offset2(the_store.drinks[0],the_store.drinks[i].drinkname);
    if(!resolve_tbg_entry(the_store.drinks[i].drinkname,tlkentries[pos]) ) pos++;
  }
  header.tlkentrycount=pos;   //logical size
  header.strrefcount=pos;
  return 0;
}

int Ctbg::collect_effectrefs()
{
  int *strref_opcodes;
  int pos;
  int count;

  strref_opcodes=get_strref_opcodes();
  pos=0;
  if(member_array(the_effect.header.feature,strref_opcodes)!=-1)
  {
    count=1;
  }
  else
  {
    count=0;
  }
  if(!count) return 0;
  tlkentries=new tbg_tlk_reference[count];
  if(!tlkentries) return -3;
  tlkentrycount=count;        //physical size
  strrefs=new unsigned long[count];
  if(!strrefs)
  {
    delete [] tlkentries;
    return -3;
  }
  strrefcount=count;

  strrefs[0]=calc_offset(the_effect.header,par1.parl);
  if(!resolve_tbg_entry(the_effect.header.par1.parl,tlkentries[0])) pos=1;
  header.tlkentrycount=pos;   //logical size
  header.strrefcount=pos;
  return 0;
}

int Ctbg::collect_arearefs()
{
  int pos;
  int count;
  int i;

  pos=0;
  count=the_area.containercount+the_area.triggercount+the_area.doorcount+the_area.mapnotecount+the_area.intheader.creaturecnt;
//additional counts
  tlkentries=new tbg_tlk_reference[count];
  if(!tlkentries) return -3;
  tlkentrycount=count;        //physical size
  strrefs=new unsigned long[count];
  if(!strrefs)
  {
    delete [] tlkentries;
    return -3;
  }
  strrefcount=count;

  for(i=0;i<the_area.containercount;i++)
  {
    strrefs[pos]=the_area.header.containeroffset+calc_offset2(the_area.containerheaders[0],the_area.containerheaders[i].strref);
    if(!resolve_tbg_entry(the_area.containerheaders[i].strref,tlkentries[pos]) ) pos++;
  }
  for(i=0;i<the_area.triggercount;i++)
  {
    strrefs[pos]=the_area.header.infooffset+calc_offset2(the_area.triggerheaders[0],the_area.triggerheaders[i].strref);
    if(!resolve_tbg_entry(the_area.triggerheaders[i].strref,tlkentries[pos]) ) pos++;
  }
  for(i=0;i<the_area.doorcount;i++)
  {
    strrefs[pos]=the_area.header.dooroffset+calc_offset2(the_area.doorheaders[0],the_area.doorheaders[i].strref);
    if(!resolve_tbg_entry(the_area.doorheaders[i].strref,tlkentries[pos]) ) pos++;
  }
  for(i=0;i<the_area.mapnotecount;i++)
  {
    strrefs[pos]=the_area.header.mapnoteoffset+calc_offset2(the_area.mapnoteheaders[0],the_area.mapnoteheaders[i].strref);
    if(!resolve_tbg_entry(the_area.mapnoteheaders[i].strref,tlkentries[pos]) ) pos++;
  }
  for(i=0;i<the_area.intheader.creaturecnt;i++)
  {
    strrefs[pos]=the_area.header.intoffset+calc_offset(the_area.intheader,strrefs[i]);
    if(!resolve_tbg_entry(the_area.intheader.strrefs[i],tlkentries[pos]) ) pos++;
  }
//additional stores
  header.tlkentrycount=pos;   //logical size
  header.strrefcount=pos;
  return 0;
}

int Ctbg::collect_creaturerefs(int alternate)
{
  int *strref_opcodes;
  int pos;
  int count;
  int i;
  int feat;

  strref_opcodes=get_strref_opcodes();
  pos=0;
  if(alternate) count=SND_SLOT_COUNT;
  else count=SND_SLOT_COUNT+2+the_creature.effectcount; //names+soundset+effects
  tlkentries=new tbg_tlk_reference[count];
  if(!tlkentries) return -3;
  tlkentrycount=count;        //physical size
  strrefs=new unsigned long [count];
  if(!strrefs)
  {
    delete [] tlkentries;
    return -3;
  }
  strrefcount=count;

  if(!alternate)
  {
    strrefs[pos]=calc_offset(the_creature.header, longname);
    if(!resolve_tbg_entry(the_creature.header.longname,tlkentries[pos])) pos++;
    strrefs[pos]=calc_offset(the_creature.header, shortname);
    if(!resolve_tbg_entry(the_creature.header.shortname,tlkentries[pos])) pos++;
  }

  for(i=0;i<100;i++)
  {    
    if(alternate) strrefs[pos]=i*sizeof(long);
    else strrefs[pos]=calc_offset(the_creature.header,strrefs[i]);
    if(!resolve_tbg_entry(the_creature.header.strrefs[i],tlkentries[pos]) ) pos++;
  }
  if(!alternate)
  {
    for(i=0;i<the_creature.effectcount;i++)
    {
      if(the_creature.header.effbyte)
      {
        feat=the_creature.effects[i].feature;
      }
      else
      {
        feat=the_creature.oldeffects[i].feature;
      }
      if(member_array(feat,strref_opcodes)!=-1)
      {
        if(the_creature.header.effbyte)
        {
          strrefs[pos]=the_creature.header.effectoffs+calc_offset2(the_creature.effects[0],the_creature.effects[i].par1.parl);
          if(!resolve_tbg_entry(the_creature.effects[i].par1.parl,tlkentries[pos]) ) pos++;
        }
        else
        {
          strrefs[pos]=the_creature.header.effectoffs+calc_offset2(the_creature.oldeffects[0],the_creature.oldeffects[i].par1.parl);
          if(!resolve_tbg_entry(the_creature.oldeffects[i].par1.parl,tlkentries[pos]) ) pos++;
        }
      }
    }
  }

  header.tlkentrycount=pos;   //logical size
  header.strrefcount=pos;
  return 0;
}

int Ctbg::collect_worldmaprefs()
{
  int pos;
  int count;
  int i,j;

  pos=0;
  count=the_map.mapcount;  //maps + all areas/maps
  for(i=0;i<the_map.mapcount;i++)
  {
    count+=the_map.headers[i].areacount*2;
  }
  tlkentries=new tbg_tlk_reference[count];
  if(!tlkentries) return -3;
  tlkentrycount=count;        //physical size
  strrefs=new unsigned long [count];
  if(!strrefs)
  {
    delete [] tlkentries;
    return -3;
  }
  strrefcount=count;

  for(i=0;i<the_map.mapcount;i++)
  {
    strrefs[pos]=sizeof(map_mainheader)+calc_offset2(the_map.headers[0],the_map.headers[i].mapname);
    if(!resolve_tbg_entry(the_map.headers[i].mapname,tlkentries[pos]) ) pos++;
    for(j=0;j<the_map.headers[i].areacount;j++)
    {
      strrefs[pos]=the_map.headers[i].areaoffset+calc_offset2(the_map.areas[i][0],the_map.areas[i][j].caption);
      if(!resolve_tbg_entry(the_map.areas[i][j].caption,tlkentries[pos]) ) pos++;

      strrefs[pos]=the_map.headers[i].areaoffset+calc_offset2(the_map.areas[i][0],the_map.areas[i][j].tooltip);
      if(!resolve_tbg_entry(the_map.areas[i][j].tooltip,tlkentries[pos]) ) pos++;
    }
  }
//additional stores
  header.tlkentrycount=pos;   //logical size
  header.strrefcount=pos;
  return 0;
}

int Ctbg::collect_chuirefs()
{
  chui_label *cc;
  int pos;
  int count;
  int i;

  pos=0;
  count=0;
  for(i=0;i<the_chui.controlcnt;i++)
  {
    if(the_chui.controls[i].controltype==CC_LABEL) count++;
  }
  tlkentries=new tbg_tlk_reference[count];
  if(!tlkentries) return -3;
  tlkentrycount=count;        //physical size
  strrefs=new unsigned long [count];
  if(!strrefs)
  {
    delete [] tlkentries;
    return -3;
  }
  strrefcount=count;

  for(i=0;i<the_chui.controlcnt;i++)
  {
    if(the_chui.controls[i].controltype!=CC_LABEL) continue;

    cc=(chui_label *) (the_chui.extensions[i]);
    strrefs[pos]=the_chui.controltable[i].x+sizeof(chui_control_common)+
      calc_offset3(cc,cc->strref);
    if(!resolve_tbg_entry(cc->strref,tlkentries[pos]) ) pos++;
  }

  header.tlkentrycount=pos;   //logical size
  header.strrefcount=pos;
  return 0;
}

int Ctbg::collect_srcrefs()
{
  int pos;
  int count;
  int i;

  pos=0;
  count=the_src.m_cnt;
  tlkentries=new tbg_tlk_reference[count];
  if(!tlkentries) return -3;
  tlkentrycount=count;
  strrefs=new unsigned long[count];
  if(!strrefs)
  {
    delete [] tlkentries;
    return -3;
  }
  strrefcount=count;
  for(i=0;i<the_src.m_cnt;i++)
  {
    strrefs[pos]=i*2+sizeof(long);
    if(!resolve_tbg_entry(the_src.m_slots[i],tlkentries[pos]) ) pos++;
  }
  header.tlkentrycount=pos;   //logical size
  header.strrefcount=pos;
  return 0;
}

int Ctbg::collect_dlgrefs()
{
  int pos;
  int count;
  int i;

  pos=0;
  count=the_dialog.statecount+the_dialog.transcount*2;
  tlkentries=new tbg_tlk_reference[count];
  if(!tlkentries) return -3;
  tlkentrycount=count;
  strrefs=new unsigned long[count];
  if(!strrefs)
  {
    delete [] tlkentries;
    return -3;
  }

  for(i=0;i<the_dialog.statecount;i++)
  {
    strrefs[pos]=the_dialog.header.offstates+calc_offset2(the_dialog.dlgstates[0],the_dialog.dlgstates[i].actorstr);
    if(!resolve_tbg_entry(the_dialog.dlgstates[i].actorstr,tlkentries[pos]) ) pos++;
  }
  for(i=0;i<the_dialog.transcount;i++)
  {
    strrefs[pos]=the_dialog.header.offtrans+calc_offset2(the_dialog.dlgtrans[0],the_dialog.dlgtrans[i].playerstr);
    if(!resolve_tbg_entry(the_dialog.dlgtrans[i].playerstr,tlkentries[pos]) ) pos++;
    strrefs[pos]=the_dialog.header.offtrans+calc_offset2(the_dialog.dlgtrans[0],the_dialog.dlgtrans[i].journalstr);
    if(!resolve_tbg_entry(the_dialog.dlgtrans[i].journalstr,tlkentries[pos]) ) pos++;
  }

  header.tlkentrycount=pos;   //logical size
  header.strrefcount=pos;
  return 0;
}

int Ctbg::collect_prorefs()
{ 
  int count, pos;

  if(!(the_projectile.header.extflags&PROJ_TEXT))
  {
    return 0;
  }

  count=1;
  pos=0;
  tlkentries=new tbg_tlk_reference[count];
  if(!tlkentries) return -3;
  tlkentrycount=count;        //physical size
  strrefs=new unsigned long[count];
  if(!strrefs)
  {
    delete [] tlkentries;
    return -3;
  }
  strrefcount=count;

  strrefs[0]=calc_offset(the_projectile.header,text);
  if(!resolve_tbg_entry(the_projectile.header.text,tlkentries[0])) pos=1;
  header.tlkentrycount=pos;   //logical size
  header.strrefcount=pos;
  return 0;
}

//alternative output, strrefs+offsets go to a tp2 file
int Ctbg::OutputTP2(CString outfilename)
{
	CString tmptext;
	CString tmpsound;
	int count;
	FILE *fpoi;
	int ret;

	fpoi = fopen(outfilename,"w");
	if (!fpoi)
	{
		return -2;
	}
	ret = 0;
	for(count=0;count<header.tlkentrycount;count++)
	{
		tmptext=resolve_tlk_text( tlkentries[count].offset );
    tmpsound=resolve_tlk_soundref(tlkentries[count].offset );
		if (tmpsound.IsEmpty())
		{
			fprintf(fpoi,"SAY %d ~%s~\n", strrefs[count], tmptext );
		}
		else
		{
			fprintf(fpoi,"SAY %d ~%s~ [%s]\n", strrefs[count], tmptext, tmpsound );
		}
	}
	fclose(fpoi);
	return ret;
}

//writes out a tbg file
int Ctbg::ExportFile(int filetype, CString outfilepath, int type)
{
  CString outfilename;
  CString tmpstr;
  int count,maxcount;
  int i,j;
  int idx;
  int esize;
  int fhandle;
  int ret;

  idx=determinetype(filetype&0xfff);
  if(idx>NUM_OBJTYPE) return -99; //filetype not supported
  if(!tbgext[idx]) return -99;
  if(filetype&TBG_ALT)
  {
    outfilename.Format("%s%s#%c",outfilepath,itemname,tbgext[idx]);
  }
  else
  {
    if(tbgext[idx]=='-') outfilename.Format("%s%s",bgfolder,itemname);
    else outfilename.Format("%s%s-%c",outfilepath,itemname,tbgext[idx]);
  }
	if(type)
	{
		outfilename+=".tp2";
	}
	else
	{
		outfilename+=".tbg";
	}
  new_iap(); //clears headers & counters  
  switch(filetype)
  {
  case REF_CRE|TBG_ALT: //soundrefs
    header.filelength=SND_SLOT_COUNT*sizeof(long);
    ret=collect_creaturerefs(TBG_ALT);
    break;
  case REF_ITM:
    header.filelength=the_item.WriteItemToFile(0,1);
    ret=collect_itemrefs();
    break;
  case REF_SPL:
    header.filelength=the_spell.WriteSpellToFile(0,1);
    ret=collect_spellrefs();
    break;
  case REF_STO:
    header.filelength=the_store.WriteStoreToFile(0,1);
    ret=collect_storerefs();
    break;
  case REF_EFF:
    header.filelength=the_effect.WriteEffectToFile(0,1);
    ret=collect_effectrefs();
    break;
  case REF_ARE:
    header.filelength=the_area.WriteAreaToFile(0,1);
    ret=collect_arearefs();
    break;
  case REF_CRE:
    header.filelength=the_creature.WriteCreatureToFile(0,1);
    ret=collect_creaturerefs(0);
    break;
  case REF_WMP:
    header.filelength=the_map.WriteMapToFile(0,1);
    ret=collect_worldmaprefs();
    break;
  case REF_CHU:
    header.filelength=the_chui.WriteChuiToFile(0,1);
    ret=collect_chuirefs();
    break;
  case REF_SRC:
    header.filelength=the_src.WriteStringToFile(0,1);
    ret=collect_srcrefs();
    break;
  case REF_DLG:
    header.filelength=the_dialog.WriteDialogToFile(0,1);
    ret=collect_dlgrefs();
    break;
  case REF_PRO:
    header.filelength=the_projectile.WriteProjectileToFile(0,1);
    ret=collect_prorefs();
    break;
  default:
    ret=-99;
  }
  if(ret<0) return ret;
	if (type)
	{
		return OutputTP2(outfilename);
	}
  memcpy(header.signature,"TBG4",4);
  if(filetype&TBG_ALT) header.signature[3]=(char) toupper(tbgext[idx]);

  maxcount=header.strrefcount=header.tlkentrycount;
  for(count=0;count<maxcount;count++) tlkentries[count].strrefoffset=count;
  for(count=0;count<header.tlkentrycount;count++)
  {
    for(i=0;i<count;i++)
    {
      //now we use this offset as strref (it will be used differently elsewhere)
      if(tlkentries[count].offset==tlkentries[i].offset)
      {
        //moving the strrref of the same type forward
        SwapPalette(strrefs, tlkentries[i].strrefoffset, tlkentries[count].strrefoffset);
        //removing the tlk header of the same type (# is in count)
        memmove(tlkentries+count, tlkentries+count+1, (maxcount-count-1)*sizeof(tbg_tlk_reference));
        memset(tlkentries+maxcount-1,-1,sizeof(tbg_tlk_reference) );
        //incrementing the ref. count of the previous entry
        tlkentries[i].strrefcount++;
        //adjusting the other entries
        for(j=i+1;j<count;j++)
        {
          tlkentries[j].strrefoffset++;
        }
        header.tlkentrycount--;
        count--;
        break;
      }
    }
  }

  tmpstr.Format("%s%s",itemname,objexts[idx]);
  strncpy(header.filename,tmpstr,sizeof(header.filename) );
  header.genre=get_genre();
  header.tlkentryoffset=sizeof(tbg4_header);
  header.strrefoffset=header.tlkentryoffset+header.tlkentrycount*sizeof(tbg_tlk_reference);
  header.asciioffset=header.strrefoffset+header.strrefcount*sizeof(unsigned long);
  header.fileoffset=header.asciioffset+header.asciicount*sizeof(unsigned long);
  header.textentryoffset=header.fileoffset+header.filelength;

  //adjusting strref offsets
  //setting string offsets (we used this as pointer offset)
  m_tbgnames=new CString[maxcount];
  if(!m_tbgnames) return -3;
  count=0;
  for(i=0;i<header.tlkentrycount;i++)
  {
    m_tbgnames[i]=tlk_entries[0][tlkentries[i].offset].text;
    tlkentries[i].offset=count;
    count+=m_tbgnames[i].GetLength();
  }
  fhandle=open(outfilename, O_RDWR|O_BINARY|O_TRUNC|O_CREAT,S_IREAD|S_IWRITE);
  if(fhandle<1) return -2;
  if(write(fhandle,&header,sizeof(header) )!=sizeof(header) )
  {
    ret=-2;
    goto endofquest;
  }
  esize=header.tlkentrycount*sizeof(tbg_tlk_reference);
  if(write(fhandle,tlkentries,esize )!=esize)
  {
    ret=-2;
    goto endofquest;
  }
  esize=header.strrefcount*sizeof(unsigned long);
  if(write(fhandle,strrefs,esize)!=esize)
  {
    ret=-2;
    goto endofquest;
  }
  switch(filetype)
  {
  case REF_CRE|TBG_ALT: write(fhandle,the_creature.header.strrefs,header.filelength); break;
  case REF_ITM: the_item.WriteItemToFile(fhandle,0); break;
  case REF_SPL: the_spell.WriteSpellToFile(fhandle,0); break;
  case REF_STO: the_store.WriteStoreToFile(fhandle,0); break;
  case REF_EFF: the_effect.WriteEffectToFile(fhandle,0); break;
  case REF_ARE: the_area.WriteAreaToFile(fhandle,0); break;
  case REF_CRE: the_creature.WriteCreatureToFile(fhandle,0); break;
  case REF_WMP: the_map.WriteMapToFile(fhandle,0); break;
  case REF_CHU: the_chui.WriteChuiToFile(fhandle,0); break;
  case REF_SRC: the_src.WriteStringToFile(fhandle,0); break;
  case REF_DLG: the_dialog.WriteDialogToFile(fhandle,0); break;
  case REF_PRO: the_projectile.WriteProjectileToFile(fhandle,0); break;
  default: ret=-99;
  }
  for(i=0;i<header.tlkentrycount;i++)
  {
    if(write(fhandle,m_tbgnames[i].GetBuffer(0),tlkentries[i].length)!=tlkentries[i].length)
    {
      ret=-2;
      goto endofquest;
    }
  }
endofquest:
  close(fhandle);
  return ret;
}

int Ctbg::ImportFile()
{
  char tmpfile[10];
  char tmpext[6];
  CString outfilepath;
  CString ext;
//  int filetype;
  int fhandle;
  int i,j;
  int ret;
  int reference;
  CString text, sound;
  CString tmptext, tmpsound;
  long offset;

  //convert to file
  for(i=0;i<tlkentrycount;i++)
  {
    ret=get_strref(tlkentries[i].offset,tlkentries[i].length,text);
    RetrieveResref(sound, tlkentries[i].soundref);

    switch(revision)
    {
    case 4:case 12:
      if(tlkentries[i].strrefcount)
      {
        //handle first instance specially
        if(tlkentries[i].strrefcount+tlkentries[i].strrefoffset>header.strrefcount)
        {
          return -2;
        }
        offset=strrefs[tlkentries[i].strrefoffset];
      }
      else offset=-1;
      break;
    case 3:
      offset=tlkentries[i].pitch;
      break;
    default:
      return -2;
    }
    if(offset==-1) continue;
    if(offset>=header.filelength)
    {
      return -2;
    }
    reference=*(long *) (filedata+offset);
    if(reference!=-1)
    { //keep the original strref if possible
      tmptext=resolve_tlk_text(reference );
      tmpsound=resolve_tlk_soundref(reference );
    }
    if(tmptext!=text || tmpsound!=sound)
    {
      if (!(editflg&FORCENEW))
      {
        reference=-1;
      }
      reference=store_tlk_data(reference, text, sound, 0);
      if (optflg&BOTHDIALOG)
      {
        store_tlk_data(reference, text, sound, 1);
      }
      *(long *) (filedata+offset)=reference;
    }
    
    //handle other instances of the same strref
    for(j=1;j<tlkentries[i].strrefcount;j++)
    {
      offset=strrefs[tlkentries[i].strrefoffset+j];
      if(offset>=header.filelength) return -2;
      if(* (long *) (filedata+offset)!=-1)
      { //keep the original strref if possible
        tmptext=resolve_tlk_text(*(long *) (filedata+offset) );
        tmpsound=resolve_tlk_soundref(*(long *) (filedata+offset) );
        if(tmptext!=text || tmpsound!=sound)
        {
          *(long *) (filedata+offset)=reference;
        }
      }
    }
    if(tlkentries[i].asciicount+tlkentries[i].asciioffset>header.asciicount)
    {
      return -2;
    }
    for(j=0;j<tlkentries[i].asciicount;j++)
    {
      offset=ascii[tlkentries[i].asciioffset+j];
      if(offset>=header.filelength) return -2;
      //ascii stuff ???
      return -5;
    }
  }
  switch(revision)
  {
  case 12: //tbgc (don't save it?)
    memcpy(the_creature.header.strrefs,filedata,header.filelength);
    return 0;
  }
  //output file
  for(i=0;i<8 && header.filename[i]!='.';i++) tmpfile[i]=header.filename[i];
  tmpfile[i]=0;
  tmpext[4]=0;
  for(j=0;j<4;j++)
  {
    tmpext[j]=header.filename[i++];
  }
  itemname=tmpfile;
  ext=tmpext;
  outfilepath=bgfolder+"override/"+itemname+ext;
  fhandle=creat(outfilepath,S_IWRITE);
  if(fhandle<1) return -2;
  if(write(fhandle,filedata,header.filelength)!=header.filelength)
  {
    close(fhandle); //closing the game file
    return -2;
  }
  close(fhandle);
  return 0;
}

int Ctbg::Readtbg(CString filepath, CStringList &filelist)
{
  int fhandle;
  int ret;

  //take tbg,
  fhandle=open(filepath,O_RDONLY);
  if(fhandle<1) return -2;
  ret=read_tbg(fhandle,-1);
  if(ret==9) return 0;        //handled in read_tbg already
  if(ret==-4)
  {
    lseek(fhandle,0,SEEK_SET);
    ret=read_iap(fhandle,0, filelist); //handle it as an IAP (bundled tbg)
  }
  else
  {
    if(ret<0)
    {
      close(fhandle);
      return ret;
    }
    ret=ImportFile();
  }
  close(fhandle);
  return ret;
}

void ExportTBG(CWnd *pwnd, int filetype, int type)
{
  CString filepath;
  CString newname;
  CString tmpstr;
  int res;
  Ctbg the_tbg;
	CString ext, cext;

	if (type) ext=".tp2";
	else ext=".tbg";
	cext=ext;
	cext.MakeUpper();
  res=OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_ENABLESIZING|OFN_EXPLORER;
	CMyFileDialog m_getfiledlg(FALSE, type?"tp2":"tbg", bgfolder+itemname, res, type?szFilterTp2:szFilterTbg, pwnd);

restart:  
  if( m_getfiledlg.DoModal() == IDOK )
  {
    filepath=m_getfiledlg.GetPathName();
    filepath.MakeLower();
    if(filepath.Right(4)!=ext)
    {
      filepath+=ext;
    }
    newname=m_getfiledlg.GetFileName();
    newname.MakeUpper();
    if(newname.Right(4)==cext) newname=newname.Left(newname.GetLength()-4);
    if(newname.GetLength()>8 || newname.GetLength()<1 || newname.Find(" ",0)!=-1)
    {
      tmpstr.Format("The filename '%s' is bad.",newname);
      MessageBox(pwnd->m_hWnd, tmpstr,"Warning",MB_OK);
      goto restart;
    }
    if(newname!=itemname && file_exists(filepath) )
    {
      res=MessageBox(pwnd->m_hWnd, "Do you want to overwrite "+newname+"?","Warning",MB_YESNO);
      if(res==IDNO) goto restart;
    }
    itemname=newname;
    filepath=filepath.Left(filepath.ReverseFind('\\')+1);
    res=the_tbg.ExportFile(filetype, filepath,type);
    switch(res)
    {
    case -99:
      MessageBox(pwnd->m_hWnd, "DLTCEP doesn't support this format.","Error",MB_ICONSTOP|MB_OK);
      break;
    case -3:
      MessageBox(pwnd->m_hWnd, "Out of memory","Error",MB_ICONSTOP|MB_OK);
      break;
    case -1:
      MessageBox(pwnd->m_hWnd, "Export failed...","Error",MB_ICONSTOP|MB_OK);
      break;
    }
  }
}
