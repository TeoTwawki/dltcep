#include "stdafx.h"

#include <fcntl.h>
#include <sys/stat.h>
#include "chitem.h"
#include "chitemDlg.h"
#include "options.h"
#include "variables.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///CChitemDlg remaining part
int CChitemDlg::check_integers(const int *bytes, int storeflags, int opcode, int trigger, int block)
{
  CString tmp;
  int jr, ret;

  tmp=resolve_scriptelement(opcode, trigger, block);
  ret=0;
  if(storeflags&CHECK_STRREF)
  {
    if(storeflags&CHECK_ONLYJ) jr=2;
    else jr=0;
    switch(check_reference(bytes[0],jr))
    {
    case 1:
      ret|=BAD_STRREF;
      log("Invalid string reference: %d for %s",bytes[0], tmp);
      break;
    case 2:
      ret|=BAD_STRREF;
      log("Deleted string reference: %d for %s",bytes[0], tmp);
      break;
    case 3:
        ret|=BAD_STRREF;
        log("Titleless journal string reference: %d for %s",bytes[0], tmp);
      break;
    }
  }
  if(storeflags&CHECK_POINT)
  {
    //check bytes[1]/bytes[2]
  }
  if(storeflags&CHECK_BYTE)
  {
    if(bytes[0]>255)
    {
      ret|=BAD_ATTR;
      log("Value %d is too large: %s",bytes[0],tmp);
    }
  }
  return ret;
}

int add_death_variable(char *varname)
{
  CString tmpstr, format;
  CString tmp;
  unsigned int varmax;

  if(pst_compatible_var())
  {
    varmax=27;
    format="KAPUTZ%.27s_dead";
  }
  else
  {
    varmax=18;
    format="GLOBALsprite_is_dead%.18s";
    format="GLOBALSPRITE_IS_DEAD%.18s";
  }
  if(varname[varmax] && (strlen(varname)>varmax)) return -1;
  tmp=varname;
  tmp.MakeLower();
  tmpstr.Format(format,tmp);
  //set it to 2 (so it won't bug out)
  variables[tmpstr]=2;
  return 0;
}

void load_variables(HWND parent, int verbose, int novalue, CStringMapInt &vars)
{
  CString filename;
  int fhandle;
  int len;
  CString area, var;
  char line[44];

  if(pst_compatible_var())
  {
    if(file_exists(filename) )
    {
      if(MessageBox(parent, filename+" exists, shall I load it instead of var.var?","Variables",MB_YESNO)==IDNO)
      {
        filename=bgfolder+"var.var";
      }
    }
    else filename=bgfolder+"var.var";
  }
  else filename=bgfolder+"itemchecker.var";
  fhandle=open(filename,O_RDONLY|O_BINARY);
  if(fhandle<1)
  {
    if(verbose) MessageBox(parent, "Cannot load "+filename,"Error",MB_OK);
    return;
  }
  len=filelength(fhandle);
  if((len<44) || (len%44) )
  {
    MessageBox(parent, "Crippled file:"+filename,"Error",MB_OK);
    close(fhandle);
    return;
  }
  len/=44;
  vars.RemoveAll();
  vars.InitHashTable(get_hash_size(len*2));
  read(fhandle,line,44); //skipping first fake line
  while(--len)
  {
    read(fhandle,line,44);
    area.Format("%.6s",line);
    var.Format("%.32s",line+8);
    area.MakeUpper();
    var.TrimRight();
    var.MakeUpper();
    if(novalue) vars[area+var]=2;
    else vars[area+var]=*(long *) (&line[40]);
  }
  close(fhandle);
}

void save_variables(HWND parent, CStringMapInt &vars)
{
  POSITION pos;
  CString key;
  int count;
  int fhandle;
  char line[44];
  CString area, var;
  int flg;

  fhandle=open(bgfolder+"itemchecker.var",O_RDWR|O_BINARY|O_TRUNC|O_CREAT,S_IREAD|S_IWRITE);
  if(fhandle<1)
  {
    MessageBox(parent, "Cannot save variable list.","Error",MB_OK);
    return;
  }
  sprintf(line,"%-8.8s%-32.32s","902","No_Anarchy");
  (*(long *) &line[40])=0;
  write(fhandle,line,44);
  //globals
  pos=vars.GetStartPosition();
  while(pos)
  {
    vars.GetNextAssoc(pos, key, count);
    area=key.Left(6);
    var=key.Mid(6);
    if(pst_compatible_var()) flg=area=="GLOBAL";
    else flg=(var.Left(14)!="SPRITE_IS_DEAD" && area=="GLOBAL");
    if(flg)
    {
      memset(line,0,sizeof(line));
      sprintf(line,"%-6.6s  %-32.32s",area,var);
      (*(long *) &line[40])=count;
      write(fhandle,line,sizeof(line) );
    }
  }
  //kaputz (only in PST)
  pos=vars.GetStartPosition();
  while(pos)
  {
    vars.GetNextAssoc(pos, key, count);
    area=key.Left(6);
    var=key.Mid(6);
    if(pst_compatible_var()) flg=area=="KAPUTZ";
    //else flg=(var.Left(14)=="sprite_is_dead" && area=="GLOBAL");
    else flg=(var.Left(14)=="SPRITE_IS_DEAD" && area=="GLOBAL");
    if(flg)
    {
      memset(line,0,sizeof(line));
      sprintf(line,"%-6.6s  %-32.32s",area,var);
      (*(long *) &line[40])=count;
      write(fhandle,line,sizeof(line) );
    }
  }
  pos=vars.GetStartPosition();
  while(pos)
  {
    vars.GetNextAssoc(pos, key, count);
    area=key.Left(6);
    var=key.Mid(6);
    if(area=="LOCALS")
    {
      memset(line,0,sizeof(line));
      sprintf(line,"%-6.6s  %-32.32s",area,var);
      (*(long *) &line[40])=count;
      write(fhandle,line,sizeof(line) );
    }
  }
  close(fhandle);
}

static int scanned_types[]={ADD_DEAD, ADD_DEAD2, ADD_GLOBAL, ADD_LOCAL, ADD_VAR, ADD_VAR2, ADD_VAR3, CHECK_SCOPE,
-1};

int CChitemDlg::store_variable(CString varname, int storeflags, int opcode, int trigger, int block)
{
  CString tmp;
  loc_entry dummyloc;
  int cnt;
  int i;
  int sf;

  sf=storeflags&0xff;
  if((trigger&0xff)==SCANNING)
  {
    if(member_array(sf, scanned_types)==-1) return 0;
  }
  tmp=resolve_scriptelement(opcode, trigger, block);
  
  switch(sf)
  {
  case NO_CHECK:
    return 0;
  case CHECK_GAME:
    if(varname.GetLength()>32)
    {
      log("Bad string constant: '%s' (%s)",varname,tmp);
      return 1;
    }
    return 0;
  case CHECK_NUM:
    if(chkflg&NORESCHK) return 0;
    cnt=varname.GetLength();
    if(cnt&3)
    {
      log("Bad spellnumber list, crippled number: '%s' (%s)",varname,tmp);
      return 1; //spell numbers are 4 digits
    }
    if(cnt>32 || cnt<1)
    {
      log("Bad spellnumber list, length: '%s' (%s)",varname,tmp);
      return 1;
    }
    while(cnt--)
    {
      if(!isdigit(varname.GetAt(cnt)) )
      {
        log("Bad spellnumber list, not numeric: '%s' (%s)",varname,tmp);
        return 1;
      }
    }
    return 0;
  case IS_VAR:
    if(chkflg&NOVARCHK) return 0;
    if(!varname.GetLength()) return 0;
    log("Unexpected variable: '%s' (%s)",varname,tmp);
    return 1;
  case CHECK_SRC:
    if(chkflg&NORESCHK) return 0;
    if(strings.Lookup(varname, dummyloc) ) return 0;
    log("Missing strref source: '%s' (%s)", varname, tmp);
    return 1;
  case CHECK_MOVIE:
    if(chkflg&NORESCHK) return 0;
    if(movies.Lookup(varname, dummyloc) ) return 0;
    if(wbms.Lookup(varname, dummyloc) ) return 0;
    log("Missing movie: '%s' (%s)", varname, tmp);
    return 1;
  case CHECK_SOUND2:
    if(varname.IsEmpty()) return 0; // may be empty (iwd2 playsoundeffect)
  case CHECK_SOUND:
    if(chkflg&NORESCHK) return 0;
    if(sounds.Lookup(varname, dummyloc) ) return 0;
    log("Missing sound: '%s' (%s)", varname, tmp);
    return 1;
  case CHECK_DSOUND:
    if(chkflg&NORESCHK) return 0;
    i=varname.Find(':');
    if(i>8 || i<0)
    {
      log("Invalid sound resource: '%s' (%s)",varname, tmp);
      return 1;
    }
    varname2=varname.Left(i);
    varname=varname.Mid(i+1);
    if(varname.GetLength() && !sounds.Lookup(varname, dummyloc) )
    {
      log("Missing sound: '%s' (%s)", varname, tmp);
      return 1;
    }
    if(varname2.GetLength() && !sounds.Lookup(varname2, dummyloc) )
    {
      log("Missing sound: '%s' (%s)", varname2, tmp);
      return 1;
    }
    return 0;
  case CHECK_PAL:
    if(chkflg&NORESCHK) return 0;
    if(bitmaps.Lookup(varname, dummyloc) ) return 0;
    log("Missing palette: '%s' (%s)", varname, tmp);
    return 1;
  case CHECK_ITEM2:
    if(varname.IsEmpty()) return 0; // may be empty (useitemslot)
    //fallthrough
  case CHECK_ITEM:
    if(chkflg&NOITEMCH) return 0;
    if(items.Lookup(varname, dummyloc) ) return 0;
    log("Missing item: '%s' (%s)", varname, tmp);
    return 1;
  case CHECK_MOS2:
    if(varname.IsEmpty()) return 0; // may be empty (spellres actions are polymorph)
    //fallthrough
  case CHECK_MOS:
    return 0;
  case CHECK_2DA2:
    if(varname.IsEmpty()) return 0; // may be empty (incrementchapter)
    //fallthrough    
  case CHECK_2DA:
    if(darefs.Lookup(varname, dummyloc) ) return 0;
    log("Missing 2da: '%s' (%s)",varname, tmp);
    return 1;
  case CHECK_CRE:
    if(chkflg&NOCRECHK) return 0;
    if(creatures.Lookup(varname, dummyloc) ) return 0;
    log("Missing creature: '%s' (%s)",varname, tmp);
    return 1;
  case CHECK_STORE:
    if(chkflg&NOSTORCH) return 0;
    if(stores.Lookup(varname, dummyloc) ) return 0;
    log("Missing store: '%s' (%s)",varname, tmp);
    return 1;
  case CHECK_VVC2:
    if(varname.IsEmpty()) return 0; // may be empty (vvc for creature summoning)
    //fallthrough
  case CHECK_VVC:
    if(chkflg&NORESCHK) return 0;
    if(vvcs.Lookup(varname, dummyloc) ) return 0;
    if(icons.Lookup(varname, dummyloc) ) return 0;
    log("Missing visual effect (vvc or bam): '%s' (%s)",varname, tmp);
    return 1;
  case CHECK_SPL2:
    if(varname.IsEmpty()) return 0; // may be empty (spellres actions are polymorph)
    //fallthrough
  case CHECK_SPL:
    if(chkflg&NOSPLCHK) return 0;
    if(spells.Lookup(varname, dummyloc) ) return 0;
    log("Missing spell: '%s' (%s)",varname, tmp);
    return 1;
  case CHECK_BCS2:
    if(varname.IsEmpty()) return 0; // may be empty
    if(varname=="NONE") return 0;   // apparently used this way
    //fallthrough
  case CHECK_BCS:
    if(chkflg&NOSCRIPT) return 0;
    if(scripts.Lookup(varname, dummyloc)) return 0;
    log("Missing script: '%s' (%s)", varname, tmp);
    return 1;
  case CHECK_DLG2:
    if(varname.IsEmpty()) return 0; // may be empty
    if(varname=="NONE") return 0;
    //fallthrough
  case CHECK_DLG:
    if(chkflg&NODLGCHK) return 0;
    if(dialogs.Lookup(varname, dummyloc)) return 0;
    log("Missing dialog: '%s' (%s)", varname, tmp);
    return 1;
  case CHECK_DEAD2:
    if(storeflags&INANIMATE) return 0;
    //fallthrough
  case ADD_DEAD2:
    if(varname.IsEmpty()) return 0; // may be empty
    //fallthrough
  case ADD_DEAD:
    //check for death variables
    if(chkflg&NODVARCH) return 0;
    if(pst_compatible_var())
    {
      varname="KAPUTZ"+varname+"_dead";
    }
    else
    {
//      varname="GLOBALsprite_is_dead"+varname;
      varname="GLOBALSPRITE_IS_DEAD"+varname;
    }
    break;
  case CHECK_SCOPE:    
    if(areas.Lookup(varname, dummyloc))
    {
      varname2=varname; //storing area
      return 0;
    }
    if(varname!="LOCALS" && varname!="GLOBAL" && varname !="MYAREA")
    {
      log("Invalid scope: %s",varname);
      return 1;
    }
    varname2=varname;
    return 0;
  case CHECK_AREA2:
    if(varname.IsEmpty()) return 0; // may be empty (spellres actions are polymorph)
  case CHECK_AREA:
    if(areas.Lookup(varname, dummyloc))
    {
      varname2=varname; //storing area
      return 0;
    }
    varname2.Empty();
    log("Missing area: '%s' (%s)", varname, tmp);
    return 1;
  case VALID_TAG:
    /// no idea yet
    return 0;
  case ENTRY_POINT:
    if(CheckDestination(varname2,varname) )
    {
      log("Invalid entrance '%s' for area '%s' (%s)",varname,varname2,tmp);
      return 1;
    }    
    return 0;
  case CHECK_XPLIST:
    varname.MakeUpper();
    if(xplist.Find(varname) ) return 0;
    log("Missing xplist.2da entry: '%s' (%s)", varname, tmp);
    return 1;
  case ADD_GLOBAL:
    varname="GLOBAL"+varname;
    break;
  case ADD_LOCAL:
    varname="LOCALS"+varname;
    break;
  case ADD_VAR:
    break;
  case ADD_VAR2: //merge variables 
    if((varname.GetLength()>6) && (varname[6]==':'))
    {
      varname.Delete(6); //removing .
      break; //break for variable
    }
    break;
  case ADD_VAR3:
    varname=varname2+varname;
    break;
  default:
    log("Internal error for %s (cmd:%d)",tmp, storeflags);
    return -1;
  }
  if(!varname.GetLength() )
  {
    log("Empty variable name: (%s)",tmp);
    return 1;
  }
  if(varname.GetLength()<7)
  {
    log("Crippled variable name: %s (%s)", varname, tmp);
    return 1;
  }
  if(varname.FindOneOf("()")!=-1 )
  {
    log("Crippled variable name: %s (%s)", varname, tmp);
    return 1;
  }
  varname.MakeUpper();
  /*
  for(i=0;i<6;i++)
  {
    varname.SetAt(i,(char) toupper(varname[i]) );
  }
  */
  if(varname.Left(6)!="GLOBAL" && !pst_compatible_var() )
  {
    for(i=0;i<6;i++)
    {
      varname.SetAt(i,"LOCALS"[i]);
    }
  }

  switch(trigger&0xff)
  {
  case SCANNING:
    if(!variables[varname])
    {
      log("New variable: %s",varname);
    }
    variables[varname]++;
    break;
  case CHECKING:
    if(chkflg&NOVARCHK) return 0;
    cnt=0;
    variables.Lookup(varname,cnt);
    switch( cnt )    
    {
    case 0:
      log("Incorrect variable: %s (%s)",varname, tmp);
      return 1;
    case 1:
      log("Suspicious variable: %s (%s)",varname, tmp);
      return 0;
    }
  }
  return 0;
}

int CChitemDlg::check_script(int check_or_scan) //scans for variables
{
  int ret, gret;
  int bcnt, tcnt, rcnt, acnt;
  block *bpoi;
  trigger *tpoi;
  response *rpoi;
  action *apoi;
  int num_or;
  int checkflags;
  CString area;
  int opcode;
  int i;

  gret=0;
  if(!the_script.blockcount)
  {
    gret=-1;
    log("Empty script!");
  }
  for(bcnt=0;bcnt<the_script.blockcount; bcnt++)
  {
    bpoi=&the_script.blocks[bcnt];
    if(!bpoi->triggercount && (check_or_scan!=SCANNING) )
    {
      gret=-1;
      log("No trigger in block #%d",bcnt+1);
    }
    //triggers using variables
    num_or=0;
    opcode=0; //initialize opcode to non-TR_FALSE
    
    if(check_or_scan!=JOURNAL) //triggers don't have journals
    {
      for(tcnt=0;tcnt<bpoi->triggercount; tcnt++)
      {
        tpoi=&bpoi->triggers[tcnt];
        opcode=tpoi->opcode&0x1fff;
        if(check_or_scan==CHECKING)
        {
          if((opcode>MAX_TRIGGER) || !trigger_defs[opcode].GetLength())
          {
            gret=-1;
            log("Invalid trigger: %d (%04x) in block #%d trigger #%d",tpoi->opcode, tpoi->opcode,bcnt+1,tcnt+1);
            continue;
          }
          if(pst_compatible_var()) continue;
          if(opcode==TR_OR)
          {
            if(num_or)
            {
              gret=-1;
              log("Bad OR count in block #%d trigger #%d",bcnt+1,tcnt+1);
            }
            num_or=tpoi->bytes[0];
            if (num_or<2 && check_or_scan!=SCANNING)
            {
              gret=-1;
              log("Incorrect or redundant Or() in block #%d trigger #%d", bcnt+1, tcnt+1);
            }
          }
          else if(num_or) num_or--;
        }
        
        checkflags=handle_trigger(tpoi->opcode);
        // not only death variable but: waypoints, triggers, doors etc
        if(store_variable(tpoi->trobj.var, CHECK_DEAD2|(checkflags&0xffff0000),0,OBJECT|TRIGGER|check_or_scan,bcnt))
        {
          log(resolve_scriptelement(opcode, TRIGGER, bcnt));
        }
        if((checkflags&0xff00)==0xff00)
        {
          if(checkflags&1) area=CString(tpoi->var2)+tpoi->var1;
          else area=CString(tpoi->var1)+tpoi->var2;
          ret=store_variable(area, ADD_VAR, tpoi->opcode, TRIGGER|check_or_scan, bcnt);
          gret|=ret;
        }
        else
        {
          varname2=tpoi->var2;
          ret=store_variable(tpoi->var1, checkflags, tpoi->opcode, TRIGGER|check_or_scan, bcnt);
          gret|=ret;
          ret=store_variable(tpoi->var2, checkflags>>8, tpoi->opcode, TRIGGER|check_or_scan, bcnt);
          gret|=ret;
        }
        if(check_or_scan==CHECKING)
        {
          ret=check_integers(&tpoi->bytes[0], checkflags, tpoi->opcode, TRIGGER|check_or_scan, bcnt);
          gret|=ret;
        }
      }
      if(check_or_scan==CHECKING)
      {
        if(num_or)
        {
          gret=-1;
          log("Bad OR count in block #%d trigger #%d",bcnt+1,tcnt);
        }
        
        if(!bpoi->responsecount )
        {
          log("No response in block #%d",bcnt+1);
        }
      }
    }

    for(rcnt=0;rcnt<bpoi->responsecount; rcnt++)
    {
      rpoi=&bpoi->responses[rcnt];      
    //if last opcode is False() then we don't care about the empty response
      if(opcode!=TR_FALSE && !rpoi->actioncount && (check_or_scan==CHECKING) )
      {
        gret=-1;
        log("No action in block #%d response #%d",bcnt+1,rcnt+1);
      }
      //actions using variables
      num_or=0;
      for(acnt=0;acnt<rpoi->actioncount; acnt++)
      {
        apoi=&rpoi->actions[acnt];
        // not only death variable but: waypoints, triggers, doors etc
        
        for(i=0;i<3;i++)
        {
          if(store_variable(apoi->obj[i].var, CHECK_DEAD2|(0xffff0000&handle_action(apoi->opcode)),i,OBJECT|ACTION|check_or_scan,bcnt))
          {
            log("%s in response #%d",resolve_scriptelement(apoi->opcode, ACTION, acnt), rcnt+1);
          }
        }        

        if(check_or_scan==JOURNAL)
        {
          switch(apoi->opcode)
          {
            //this is a solved journal entry
          case AC_REMOVE_JOURNAL_IWD:case AC_QUESTDONE_IWD:
            if(has_xpvar())
              journals[apoi->bytes[0]]="";
            break;
          case AC_REMOVE_JOURNAL_BG: case AC_QUESTDONE_BG:
            if(!has_xpvar())
              journals[apoi->bytes[0]]="";
            break;
          case AC_ADD_JOURNAL:
            
            //this is an undone quest journal entry
            if(has_xpvar() || (apoi->bytes[3]==JT_QUEST))
            {
              if(journals.Lookup(apoi->bytes[0],area))
              {
                if(!area.IsEmpty()) journals[apoi->bytes[0]]=area+", "+itemname;
              }
              else
              {
                journals[apoi->bytes[0]]=itemname;
              }
            }
            else
            {
              journals[apoi->bytes[0]]="";
            }
            break;
          }
          
          continue;
        }
        if((apoi->opcode>MAX_ACTION) || !action_defs[apoi->opcode].GetLength())
        {
          gret=-1;
          log("Invalid action: %d in block #%d response #%d action #%d",apoi->opcode,bcnt+1, rcnt+1, acnt+1);
          continue;
        }
        checkflags=handle_action(apoi->opcode);
        if((checkflags&0xff00)==0xff00)
        {
          if(checkflags&1) area=CString(apoi->var2)+apoi->var1;
          else area=CString(apoi->var1)+apoi->var2;
          ret=store_variable(area, ADD_VAR, apoi->opcode, ACTION|check_or_scan, bcnt);
          gret|=ret;
        }
        else
        {
          varname2=apoi->var2;
          ret=store_variable(apoi->var1, checkflags, apoi->opcode, ACTION|check_or_scan, bcnt);
          gret|=ret;
          ret=store_variable(apoi->var2, checkflags>>8, apoi->opcode, ACTION|check_or_scan, bcnt);
          gret|=ret;
        }
        if(check_or_scan==CHECKING)
        {
          ret=check_integers(apoi->bytes, checkflags, apoi->opcode, ACTION|check_or_scan, bcnt);
          gret|=ret;
          if(chkflg&NOCUT) continue;
          if(apoi->opcode==AC_STARTCUT)
          {            
            if(num_or!=AC_STCUTMD)
            {
              log("StartCutScene() without StartCutSceneMode() in block #%d response #%d action #%d", bcnt+1,rcnt+1,acnt+1);
              gret|=1;
            }
          }
          if((apoi->opcode==AC_STCUTMD) || (apoi->opcode==AC_STARTCUT) || (apoi->opcode==AC_CLRACT) || (apoi->opcode==AC_CLRALLACT)) num_or=apoi->opcode;
        }
      }
    }
  }  
  return gret;
}


int CChitemDlg::check_or_scan_trigger(const trigger *tpoi, int checkflags, int check_or_scan, int bcnt)
{
  CString area;
  int ret, gret;

  gret=0;
  if((checkflags&0xff00)==0xff00)
  {
    if(checkflags&1) area=CString(tpoi->var2)+tpoi->var1;
    else area=CString(tpoi->var1)+tpoi->var2;
    ret=store_variable(area, ADD_VAR, tpoi->opcode, TRIGGER|check_or_scan, bcnt);
    gret|=ret;
  }
  else
  {
    varname2=tpoi->var2;
    ret=store_variable(tpoi->var1, checkflags, tpoi->opcode, TRIGGER|check_or_scan, bcnt);
    gret|=ret;
    ret=store_variable(tpoi->var2, checkflags>>8, tpoi->opcode, TRIGGER|check_or_scan, bcnt);
    gret|=ret;
  }
  if(check_or_scan==SCANNING) return gret;

  //don't store this if just scanning, these are checked, but never SET, so they must be invalid if used alone
  if (tpoi->trobj.var[0])
  {
    store_variable(tpoi->trobj.var, ADD_DEAD, 0, OBJECT|TRIGGER|check_or_scan, bcnt);
  }

  ret=check_integers(&tpoi->bytes[0], checkflags, tpoi->opcode, TRIGGER|check_or_scan, bcnt);
  gret|=ret;
  return gret;
}

int CChitemDlg::check_or_scan_action(const action *apoi, int checkflags, int check_or_scan, int bcnt)
{
  CString area;
  int i;
  int ret, gret;
  
  gret=0;
  if((checkflags&0xff00)==0xff00)
  {
    if(checkflags&1) area=CString(apoi->var2)+apoi->var1;
    else area=CString(apoi->var1)+apoi->var2;
    ret=store_variable(area, ADD_VAR, apoi->opcode, ACTION|check_or_scan, bcnt);
    gret|=ret;
  }
  else
  {
    varname2=apoi->var2;
    ret=store_variable(apoi->var1, checkflags, apoi->opcode, ACTION|check_or_scan, bcnt);
    gret|=ret;
    ret=store_variable(apoi->var2, checkflags>>8, apoi->opcode, ACTION|check_or_scan, bcnt);
    gret|=ret;
  }

  //don't store these if just scanning, it is checked, but never SET, so it must be invalid if used alone
  for(i=0;i<3;i++)
  {
    if (check_or_scan==SCANNING && !i) continue; //the override variable is never written, so it shouldn't be stored here
    store_variable(apoi->obj[i].var, ADD_DEAD, i, OBJECT|ACTION|check_or_scan, bcnt);
  }
  if(check_or_scan==SCANNING) return gret;

  ret=check_integers(&apoi->bytes[0], checkflags, apoi->opcode, ACTION|check_or_scan, bcnt);
  gret|=ret;
  return gret;
}

int CChitemDlg::check_dialog(int check_or_scan)
{
  CString *lines;
  CString tmp;
  int fh;
  int ret, gret;
  int linecount;
  int i,j;
  trigger trigger;
  action action;
  int num_or, strref;
  int first, last;
  loc_entry tmploc;
  bool got_continue;
  bool warn = (chkflg&WARNINGS)==0;

  gret=0;
  if(check_or_scan!=SCANNING)
  {
    if(check_or_scan==CHECKING) //no JOURNAL
    {
      //checking state headers
      for(i=0;i<the_dialog.statecount;i++)
      {
        switch(check_reference(the_dialog.dlgstates[i].actorstr,1,1,0))
        {
        case 1:
          gret=TREESTATE|i;
          log("Actor has invalid text in state #%d",i);
          break;
        case 2: case 5:
          gret=TREESTATE|i;
          log("Actor has no included text in state #%d",i);
        }
        
        if(chkflg&NOSTRUCT) continue;
        if(the_dialog.dlgstates[i].trnumber<1)
        {        
          log("No transitions in state #%d",i);
          gret=TREESTATE|i;
          continue;
        }
        num_or=the_dialog.dlgstates[i].trnumber+the_dialog.dlgstates[i].trindex;
        if(num_or<1 || num_or>the_dialog.transcount)
        {
          log("Invalid last index of transitions (%d) in state #%d",num_or,i);
          gret=TREESTATE|i;
          continue;
        }

        got_continue = false;
        first = the_dialog.dlgstates[i].trindex;
        last = first + the_dialog.dlgstates[i].trnumber;
        
        for (j=first;j<last;j++)
        {
          num_or=the_dialog.dlgtrans[j].flags;
          if(!(num_or&HAS_TEXT))
          {
            if ((j!=first) && got_continue && !(num_or&HAS_TRIGGER) ) {
              log("Unconditional continue in state #%d overlaps a previous continue!", i);
              gret=TREESTATE|i;
            }
            got_continue = true;
          }
        }
      }
    }
    //checking transition headers
    for(i=0;i<the_dialog.transcount;i++)
    {
      num_or=the_dialog.dlgtrans[i].flags;
      if(the_dialog.dlgtrans[i].playerstr<=0)
      {
        if(num_or&HAS_TEXT)
        {
          log("Transition #%d has no player string set, despite flags.",i);
          gret=TRANSSTATE|i;
        }
      }
      else
      {
        if(!(num_or&HAS_TEXT) )
        {
          log("Transition #%d has player string set, but no flags.",i);
          gret=TRANSSTATE|i;
        }
      }
      
      //checking transition trigger reference
      j=the_dialog.dlgtrans[i].trtrindex;
      if(j<0 || j>=the_dialog.header.numtrtrigger)
      {
        if(num_or&HAS_TRIGGER)
        {
          log("Transition #%d has a wrong trigger index (%d).",i,j);
          gret=TRANSSTATE|i;
        }
      }
      else if(j)
      {
        if(!(num_or&HAS_TRIGGER) && warn)
        {
          log("Transition #%d has trigger index (%d), but no flags.",i,j);
          gret=TRANSSTATE|i;
        }
      }
      
      //checking action reference
      j=the_dialog.dlgtrans[i].actionindex;
      if(j<0 || j>=the_dialog.header.numaction)
      {
        if((num_or&HAS_ACTION) )
        {
          log("Transition #%d has a wrong action index (%d).",i,j);
          gret=TRANSSTATE|i;
        }
      }
      else if(j)
      {
        if(!(num_or&HAS_ACTION) && warn)
        {
          log("Transition #%d has action index (%d), but no flags.",i,j);
          gret=TRANSSTATE|i;
        }
      }
      
      //checking journal string ref
      strref=the_dialog.dlgtrans[i].journalstr;
      if(strref<=0)
      {
        if(num_or&HAS_JOURNAL)
        {
          log("Transition #%d has no journal string set, despite flags.",i);
          gret=TRANSSTATE|i;
        }
      }
      else
      {
        if(!(num_or&HAS_JOURNAL) )
        {
          log("Transition #%d has journal string set, but no flags.",i);
          gret=TRANSSTATE|i;
        }
        else
        {
          if((check_or_scan==JOURNAL))
          {
            if((num_or&(HAS_QUEST|HAS_SOLVED))==HAS_QUEST)
            {
              if(journals.Lookup(strref,tmp))
              {
                if(!tmp.IsEmpty()) journals[strref]=tmp+", "+itemname;
              }
              else
              {
                journals[strref]=itemname;
              }
            }
            else //solved
            {
              journals[strref]="";
            }
            continue;
          }
        }
        
        switch(check_reference(strref,2,10,0) )
        {
        case 1:
          gret=TRANSSTATE|i;
          log("Invalid journal string in transition #%d (reference: %d).",i, strref);
          break;
        case 2:
          gret=TRANSSTATE|i;
          log("Deleted journal string in transition #%d (reference: %d).",i, strref);
          break;
        case 3:
          //if this isn't a quest journal entry, don't complain about missing title
          if ((num_or&(HAS_QUEST|HAS_SOLVED))!=0) {
            gret=TRANSSTATE|i;
            log("Titleless journal string in transition #%d (reference: %d).",i, strref);
          }
          break;
        }
      }
      
      //checking dialog linkage
      RetrieveResref(tmp,the_dialog.dlgtrans[i].nextdlg);
      num_or=the_dialog.dlgtrans[i].stateindex;
      if(the_dialog.dlgtrans[i].flags&LEAF_NODE)
      {
        if(!tmp.IsEmpty())
        {
          log("External dialog linkage %s:%d for leaf node (state #%d)",tmp,num_or,i);
          gret=TRANSSTATE|i;
        }
      }
      else
      {
        if(tmp==SELF_REFERENCE)
        {
          if(the_dialog.header.numstates<=num_or)
          {
            log("Invalid internal link to state %d in transition %d",num_or,i);
            gret=TRANSSTATE|i;
          }
        }
        else
        {
          if(dialogs.Lookup(tmp,tmploc) )
          {
            fh=locate_file(tmploc,0);
            if(the_dialog.CheckExternalLink(fh,num_or) )
            {
              log("Invalid external link to %s:state %d in transition %d",tmp,num_or,i);
              gret=TRANSSTATE|i;
            }
            close(fh);
          }
          else
          {
            log("Invalid dialog reference: %s in transition %d",tmp,i);
            gret=TRANSSTATE|i;
          }
        }
      }
    }
  }
  //syntax checking scripts
  if(check_or_scan!=JOURNAL)
  {
    for(i=0;i<the_dialog.sttriggercount;i++)
    {
      lines=explode(the_dialog.sttriggers[i], '\n', linecount);
      if(linecount==-1)
      {
        log("Out of memory");
        return -1;
      }
      num_or=0;
      for(j=0;j<linecount;j++)
      {
        if(lines[j].IsEmpty()) ret=-44; //empty top level condition
        else
        {
          ret=compile_trigger(lines[j], trigger);
          if(!pst_compatible_var())
          {
            if((trigger.opcode&0x3fff)==TR_OR)
            {
              if(num_or) ret=-42;
              num_or=trigger.bytes[0];
              if (num_or<2 && check_or_scan!=SCANNING)
              {
                ret=-47;
              }
            }
            else if(num_or) num_or--;
          }
        }
        if(ret)
        {
          if(check_or_scan!=SCANNING)
          {
            log("Invalid top level condition: %s at line #%d in %d. state trigger block (%s)", lines[j],j+1,i, get_compile_error(ret));
            gret=STATETR|i; //found errors
          }
        }
        else
        {
          ret=check_or_scan_trigger(&trigger, handle_trigger(trigger.opcode), check_or_scan, j);
          if(ret && (check_or_scan!=SCANNING) )
          {
            log("Top level condition was: %s", lines[j]);
            gret=STATETR|i;
          }
        }
      }
      if(num_or)
      {
        log("Error: %s",get_compile_error(-42));
        gret=STATETR|i;
      }
      if(lines) delete[] lines;
    }
    
    for(i=0;i<the_dialog.trtriggercount;i++)
    {
      lines=explode(the_dialog.trtriggers[i], '\n', linecount);
      if(linecount==-1)
      {
        log("Out of memory");
        return -1;
      }
      num_or=0;
      for(j=0;j<linecount;j++)
      {
        ret=compile_trigger(lines[j], trigger);
        if(!pst_compatible_var())
        {
          if((trigger.opcode&0x3fff)==TR_OR)
          {
            if(num_or) ret=-42;
            num_or=trigger.bytes[0];
            if (num_or<2 && check_or_scan!=SCANNING)
            {
              ret=-47;
            }
          }
          else if(num_or) num_or--;
        }
        if(ret)
        {        
          if(check_or_scan!=SCANNING)
          {
            log("Invalid trigger: %s in line #%d in %d. transition trigger block (%s)",lines[j],j+1,i,  get_compile_error(ret));
            gret=TRANSTR|i;
          }
        }
        else
        {
          ret=check_or_scan_trigger(&trigger, handle_trigger(trigger.opcode), check_or_scan, j);
          if(ret && (check_or_scan!=SCANNING) )
          {
            log("Transition trigger was: %s", lines[j]);
            gret=TRANSTR|i;
          }
        }
      }
      if(num_or)
      {
        log("Invalid trigger: in line #%d in %d. transition trigger block (%s)",j, i, get_compile_error(-42));
        gret=TRANSTR|i;
      }
      if(lines) delete[] lines;
    }
  }
  //num_or stores the previous action opcode initializing it to non-special
  num_or=0; 

  for(i=0;i<the_dialog.actioncount;i++)
  {
    lines=explode(the_dialog.actions[i], '\n', linecount);
    if(linecount==-1)
    {
      log("Out of memory");
      return -1;
    }

    for(j=0;j<linecount;j++)
    {
      ret=compile_action(lines[j], action, false);
      if(ret)
      {
        if(check_or_scan!=CHECKING) continue;
        log("Invalid action: %s in line #%d in %d. action block (%s)", lines[j],j+1,i,get_compile_error(ret));
        gret=ACTIONBL|i;
      }
      else
      {
        if(check_or_scan!=JOURNAL)
        {
          ret=check_or_scan_action(&action, handle_action(action.opcode), check_or_scan, j);
          if(check_or_scan!=CHECKING) continue;
          if(ret)
          {
            log("Action was: %s", lines[j]);
            gret=ACTIONBL|i;
            continue;
          }
        }
      }
      
      if(check_or_scan==JOURNAL)
      {
        switch(action.opcode)
        {
        case AC_REMOVE_JOURNAL_IWD:case AC_QUESTDONE_IWD:
          if(has_xpvar())
            journals[action.bytes[0]]="";
          break;
        case AC_REMOVE_JOURNAL_BG: case AC_QUESTDONE_BG:
          if(!has_xpvar())
            journals[action.bytes[0]]="";
          break;
        case AC_ADD_JOURNAL:
          if(has_xpvar() || (action.bytes[3]==JT_QUEST))
          {
            if(journals.Lookup(action.bytes[0],tmp))
            {
              if(!tmp.IsEmpty()) journals[action.bytes[0]]=tmp+", "+itemname;
            }
            else
            {
              journals[action.bytes[0]]=itemname;
            }
          }
          else
          {
            journals[action.bytes[0]]="";
          }    
        }
        continue;
      }

      if(chkflg&NOCUT) continue;
      if(action.opcode==AC_STCUTMD)
      {
        if((num_or!=AC_CLRACT) && (num_or!=AC_CLRALLACT))
        {
          log("Invalid action: StartCutSceneMode() without ClearActions() in line #%d in %d. action block", j+1,i);
          gret=ACTIONBL|i;
        }
        else if(action.opcode==AC_STARTCUT)
        {
          if(num_or!=AC_STCUTMD)
          {
            log("StartCutScene() without StartCutSceneMode() in line #%d in %d. action block", j+1,i);
            gret=ACTIONBL|i;
          }
        }
      }
      if((action.opcode==AC_STCUTMD) || (action.opcode==AC_STARTCUT) || (action.opcode==AC_CLRACT) || (action.opcode==AC_CLRALLACT)) num_or=action.opcode;
    }
    if(lines) delete[] lines;
  }

  return gret;
}

/////////////////////////////////////////////////////////////////////////////
// Cvariable dialog

Cvariable::Cvariable(CWnd* pParent /*=NULL*/)
	: CDialog(Cvariable::IDD, pParent)
{
	//{{AFX_DATA_INIT(Cvariable)
	m_value = 0;
	m_varpicker = _T("");
	//}}AFX_DATA_INIT
  m_prefix="LOCALS";
  m_area="LOCALS";
}

void Cvariable::PositionHead()
{
  CString varname;
  int value;
  POSITION pos;

  if(localvars.Lookup(varname, m_value) || m_prefix!="LOCALS") goto endofquest;
  pos=localvars.GetStartPosition();
  while(pos)
  {
    localvars.GetNextAssoc(pos, varname, value);
    if(varname.Mid(6)==m_varpicker)
    {
      m_area=varname.Left(6);
      m_value=value;
      goto endofquest;
    }
  }
  return;
endofquest:
  UpdateData(UD_DISPLAY);	
}

void Cvariable::RefreshVarpicker()
{
  CString varname;
  int value;
  CString prefix;
  int prefixlen;
  POSITION pos;
  int flg, loc;

  flg=0;
  loc=m_prefix=="LOCALS";
  m_varpicker_control.ResetContent();
  pos=localvars.GetStartPosition();
  prefixlen=m_prefix.GetLength();
  while(pos)
  {
    localvars.GetNextAssoc(pos,varname,value);
    prefix=varname.Left(prefixlen);
    varname.TrimRight();
    if(prefix==m_prefix)
    {
      m_varpicker_control.AddString(varname.Mid(prefixlen) );
    }
    else
    {
      if(loc && prefix!="KAPUTZ" && prefix!="GLOBAL")
      {
        m_varpicker_control.AddString(varname.Mid(prefixlen) );
      }
    }
  }
  if(m_varpicker_control.GetCount())
  {
    m_varpicker_control.GetLBText(0,m_varpicker);
    PositionHead();
  }
}

void Cvariable::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Cvariable)
	DDX_Control(pDX, IDC_AREA, m_area_control);
	DDX_Control(pDX, IDC_VARPICKER, m_varpicker_control);
	DDX_Text(pDX, IDC_VALUE, m_value);
	DDX_CBString(pDX, IDC_VARPICKER, m_varpicker);
	DDV_MaxChars(pDX, m_varpicker, 32);
	DDX_Text(pDX, IDC_AREA, m_area);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(Cvariable, CDialog)
	//{{AFX_MSG_MAP(Cvariable)
	ON_EN_KILLFOCUS(IDC_VALUE, OnKillfocusValue)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_VTYPE, OnVtype)
	ON_BN_CLICKED(IDC_RADIO2, OnVtype2)
	ON_BN_CLICKED(IDC_RADIO3, OnVType3)
	ON_CBN_KILLFOCUS(IDC_VARPICKER, OnKillfocusVarpicker)
	ON_CBN_SELCHANGE(IDC_VARPICKER, OnSelchangeVarpicker)
	ON_EN_KILLFOCUS(IDC_AREA, OnKillfocusArea)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Cvariable message handlers

void Cvariable::OnKillfocusValue() 
{
  UpdateData(UD_RETRIEVE);	
  UpdateData(UD_DISPLAY);	
}

void Cvariable::OnRemove() 
{
  CString varname;

  if(m_prefix=="LOCALS") varname=m_area+m_varpicker;
  else varname=m_prefix+m_varpicker;
  localvars.RemoveKey(varname.Left(38));
  m_varpicker="";
  m_value=0;
  RefreshVarpicker();
}

void Cvariable::OnAdd() 
{
  CString varname;

  if(m_varpicker.IsEmpty()) return;
  if(m_prefix=="LOCALS")
  {
    if(m_area.GetLength()!=6)
    {
      MessageBox("Area must be exactly 6 characters long.","Warning",MB_OK);
      return;
    }
    varname=m_area+m_varpicker;
  }
  else varname=m_prefix+m_varpicker;
  localvars[varname.Left(38)]=m_value;
  RefreshVarpicker();
}

void Cvariable::OnOK() 
{
	save_variables(m_hWnd, localvars);
	CDialog::OnOK();
}

BOOL Cvariable::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  load_variables(	m_hWnd, 1, 0, localvars);
  ((CButton *) GetDlgItem(IDC_VTYPE))->SetCheck(1);
  RefreshVarpicker();
	return TRUE;
}

void Cvariable::OnVtype() 
{
  CString varname;

  m_prefix="LOCALS";
  m_varpicker="";
  m_value=0;
  m_area_control.EnableWindow(true);
  RefreshVarpicker();
}

void Cvariable::OnVtype2() 
{
  CString varname;

  m_prefix="GLOBAL";
  m_varpicker="";
  m_value=0;
  RefreshVarpicker();
  m_area_control.EnableWindow(false);
	UpdateData(UD_DISPLAY);	
}

void Cvariable::OnVType3() 
{
  CString varname;

  if(pst_compatible_var()) m_prefix="KAPUTZ";
  else m_prefix="GLOBALSPRITE_IS_DEAD";
  //else m_prefix="GLOBALsprite_is_dead";
  m_varpicker="";
  m_value=0;
  m_area_control.EnableWindow(false);
  RefreshVarpicker();
}

void Cvariable::OnKillfocusArea() 
{
  UpdateData(UD_RETRIEVE);
  PositionHead();
}

void Cvariable::OnKillfocusVarpicker() 
{
	UpdateData(UD_RETRIEVE);
  PositionHead();
}

void Cvariable::OnSelchangeVarpicker() 
{
  int sel;

  sel=m_varpicker_control.GetCurSel();
  if(sel>=0)
  {
    m_varpicker_control.GetLBText(sel,m_varpicker);
    PositionHead();
  }
}
