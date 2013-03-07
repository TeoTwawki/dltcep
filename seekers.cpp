#include "stdafx.h"

#include <fcntl.h>
#include <sys/stat.h>
#include "chitem.h"
#include "2da.h"
#include "chitemDlg.h"
#include "options.h"
#include "variables.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///CChitemDlg checker parts

//special features that are going to be checked
//actually this information should be in *Effects.dat
#define F_DEATH           55//ids targeting
#define F_HOLDCREATURE   175
#define F_USEEFFFILE     177

#define F_NAME           103//param #1 = strref
#define F_DISPLAYSTRING  139   
#define F_CANTUSEITEM    180
#define F_NODISPLAYSTR   267

#define F_COLOR            7
#define F_COLORRGB         8
#define F_COLORGLOW        9

#define F_SUMMON           127 //summon creature (2da)

#define F_SELECT           214 //select spell

#define MP (1<<FLG_MPROJ)
#define MF (1<<FLG_MFEAT)
#define MR (1<<FLG_MRES)
#define MI (1<<FLG_MITEM)
#define MV (1<<FLG_MVAR)
#define MT (1<<FLG_MTYPE)
#define MS (1<<FLG_MSTRREF)

CString attr_types[32]={
  "Critical","Two handed","Movable","Displayable","Cursed",
    "<always set>","Magical","Bow","Silver","Cold iron","Stolen",
    "Conversable","UNKNOWN","UNKNOWN","UNKNOWN","UNKNOWN",
    "UNKNOWN","UNKNOWN","UNKNOWN","UNKNOWN",
    "UNKNOWN","UNKNOWN","UNKNOWN","UNKNOWN",
    "UNKNOWN","UNKNOWN","UNKNOWN","UNKNOWN",
    "UNKNOWN","UNKNOWN","UNKNOWN","UNKNOWN"
};

/*
"0-Invalid","1-Creature","2-Inventory/Crash", "3-Dead character","4-Area","5-Self","6-Unknown/Crash",
"7-None",
*/
int atypevsttype[NUM_ATYPE]={
  0,1,1,0x13457,1,
};

int ttypevstnum[NUM_TTYPE]={
  0,0xff,0,0xff,1,1,0,1,
};

//itemtype vs. weapon proficiency
//this works only for the original setting
unsigned __int64 itvswp[NUM_ITEMTYPE]={
  0,0,0,0,0,0,0,0,0,0,//9
    0,0,0,0,0,0x6869,0x60,0x6573,0x6b,0x5b5f,0x595d5a5e5f,//14
    0x61,0x64,0x64,0x6a,0x5c,0x66,0x67,0,0x62,0x635c,//1e
    0,0,0,0,0,0,0,0,0,0,//28
    0,0,0,0x73,0,0,0,0,0,//31
    0,0,0,0,0,0,0,0
};
//slings may be kender, barbarian or sling (65,66,67)
unsigned __int64 itvswp_dltc[NUM_ITEMTYPE]={
  0,0,0,0,0,0,0,0,0,0,//9
    0,0,0,0,0,0x68,0x5c,0x5e,0x656667,0x5a,0x595b,//14
    0x5d,0x5e,0x5e,0x6a,0x73,0x6265,0x63,0,0x60,0x6167,//1e
    0,0,0,0,0,0,0,0,0,0,//28
    0,0,0,0,0,0,0,0,0,//31
    0,0,0,0,0,0,0,0
};

CString what2h[2]={"","not "};

//lowest 3 bits contain expected atype for extended headers
//0 - none
//1 - melee
//2 - bullet/thrown
//3 - magic
//4 - bow

//daggers can be thrown/melee/magical (0x321)
//slings may or may not require bullet/magical (0x342)
int itvsatype;

int itvsatype_bg[NUM_ITEMTYPE]={
  3,3,3,3,3,0x32,3,3,0,3,   //9
    3,3,3,3,0x32,0x432,0x321,0x31,0x432,0x31,0x31, //14
    0x321,0x31,0x31,0x32,0x321,0x31,0x432,1,0x321,0x31,   //1e
    0x32,3,0,3,3,0,3,0,0,0,   //28
    3,3,0,0x31,0,0,3,0,3,     //31
    0,0,0,3,0,3,0x32,0x31        //39
};

int itvsatype_iwd[NUM_ITEMTYPE]={
  3,3,3,3,3,0x32,3,3,0,3,   //9
    3,3,3,3,0x32,0x432,0x321,0x31,0x432,0x31,0x31, //14
    0x321,0x31,0x31,0x32,0x321,0x31,0x43,1,0x321,0x31,   //1e
    0x32,3,0,3,3,0,3,0,0,0,                //28
    3,3,0,0x31,0,0,3,0,3,                  //31
    0,0,0,3,0,3,0x32,0x31,                 //39
    0,0,0,3,3,3,3,0,                       //41
    0,3,0,0x31,0,3,3,0,                       //49
};

int itvsatype_pst[NUM_ITEMTYPE]={
  3,3,3,3,3,0x32,3,3,0,3,   //9
    3,3,3,3,0x32,0x432,0x321,0x31,0x432,0x31,0x31, //14
    0x321,0x31,0x31,0x32,0x321,0x31,0x432,1,0x321,0x31,   //1e
    0x32,3,0,3,3,0,3,3,3,0,   //28
    0x31,3,0,0x31,0,0,3,0,3,     //31
    0,0,0,3,0,3,0x32,0x31        //39
};

unsigned long itemanim[NUM_ITEMTYPE]={0x2020,0x2020,0x57ff41ff,0x2020,//3
0x2020,0x2020,0x2020,0xff482020,0x2020,0x2020, //9
0x2020,0x2020,0xff44,0x2020,0x2020,0x5742, //f
0x4444,0x434d4c43,0x4c53,0x53535346,0xff535346,//14
0x48574248,0x534d,0x4c46,0x2020,0x58414248,//19
0xff515347,0x4243,0x2020,0x5053,0x4248,//1e
0x2020,0x202057ff,0x2020,0x2020,0x2020,0x2020, //24
0x2020,0x2020,0x2020,0x0000, //28
0x31442020,0x2020,0x0000,0x4c43,0x0000, //2d
0x0000,0x3444,0x0000,0x33443444, //31
0x2020,0x0000,0x0000,0x3244, //35
0x0000,0x2020,0x2020,0x32535346, //39
0x2020,0x2020,0x4132,0x4132, //3d
0x4133,0x4133,0x4134,0x4134, //41
0x4132,0x57ff,0x0000,0x31535346, //45
0x2020,0x2020,0xff482020,0x2020, //49
}; 

/*
//just general usability check
//1 - fighter - mp scroll ?
//2 - cleric  - no sharp
//4 - thief   - no halberd
//8 - mage    - no sword/bow/mace/halberd etc
int usable[NUM_ITEMTYPE]={
//0 1 2 3 4 5 6 7 8 9
0,0,0,0,0,8,0,0,0,0, //9
//a b c d e f 10 11 12 13 14
0,1,8,0,8,8, 0, 0, 0, 8, 8, //14
//15 16 7 18 19 1a 1b 1c 1d 1e
12,12,12,2,14, 0,14, 0,14,14, //1e
//1f 20 1 22 23 24 25 6 7 8 29 
14, 0,0, 0, 0, 0, 0,0,0,0, //28
//29 a b c d e  f  0 31 
8,0,0,8,0,0,12, 0,12, //31
// 2 3 4 5 6 7 8 9
0,0,0,8,0,0,0,14 //39
};
*/

int check_soundref(const char *poi, int needempty)
{
  CString tmpref;
  loc_entry fileloc;
  
  if(chkflg&NORESCHK) return 0;
  RetrieveResref(tmpref, poi);
  if(tmpref.IsEmpty())
  {
    if(needempty) return 0;
    if(chkflg&WARNINGS) return 0;
    else return -2;
  }
  if(needempty==1) return -2;
  if(sounds.Lookup(tmpref,fileloc) ) return 0;
  return -1;
}

// loretoid:
// 0 - allow cheesy reference
// 1 - check length constraints
// 2 - journal string
// 4 - soundset entry
// 8 - don't allow cr/lf in the end
int check_reference(long reference, int loretoid, int min, int max)
{
  CString snd;
  CString str;
  int tmp;
  
  if(chkflg&NOREFCHK) return 0;
  if(tlk_headerinfo[0].entrynum<0) return 0;
  if(!loretoid && (reference==9999999)) return 0; //allow cheesy reference
  if(reference==-1) return 0; //allow this too
  if(reference>=tlk_headerinfo[0].entrynum) return 1; //bad ref

  str = tlk_entries[0][reference].text;
  tmp = str.GetLength();
  if(loretoid&1)
  {
    if (min && tmp<min) return 5;
    if (max>min && tmp>max) return 5;
  }
  if(loretoid&2)
  {
    tmp=str.Find('\n');
    if(tmp<0 || tmp>50) return 3; // no title, or title is big
  }
  if(chkflg&WARNINGS) return 0;
  if(str==DELETED_REFERENCE)
  {
    return 2; //deleted entry
  }

  if (loretoid&4)
  {
    RetrieveResref(snd, tlk_entries[0][reference].reference.soundref);
    if (check_soundref(snd, 0)) return 4;
  }

  if (loretoid&8)
  {
    if (tmp>1) {
      char chr = str.GetAt(tmp-1);
      if(chr=='\r' || chr=='\n') return 6;
    }
  }
  return 0;
}

int check_itemref(const char *poi, int rndtres)
{
  CString tmpref;
  loc_entry dummy;
  int dummy2;
  
  if(chkflg&NOITEMCH) return 0;
  RetrieveResref(tmpref, poi);
  if(tmpref.IsEmpty()) 
  {
    if(chkflg&WARNINGS) return 0; //deleted tmpref, allow it
    else return -2;
  }
  if (tmpref.Find(' ')>=0)
  {
    return -1;
  }
  
  if(items.Lookup(tmpref,dummy)) return 0;
  if(rndtres && rnditems.Lookup(tmpref,dummy2)) return 0;
  return -1;
}

int check_resource(const char *poi, bool noneisnull)
{
  CString tmpref;
  loc_entry fileloc;
  
  if(chkflg&NORESCHK) return 0;
  RetrieveResref(tmpref, poi);
  if(noneisnull && (tmpref=="NONE")) tmpref.Empty();
  if(tmpref.IsEmpty())
  {
    if(chkflg&WARNINGS) return 0;
    else return -2;
  }
  if(icons.Lookup(tmpref,fileloc) ) return 0;
  return -1;
}

int check_bitmap(const char *poi, bool noneisnull)
{
  CString tmpref;
  loc_entry fileloc;
  
  if(chkflg&NORESCHK) return 0;
  RetrieveResref(tmpref, poi);
  if(noneisnull && (tmpref=="NONE")) tmpref.Empty();
  if(tmpref.IsEmpty())
  {
    if(chkflg&WARNINGS) return 0;
    else return -2;
  }
  if(bitmaps.Lookup(tmpref,fileloc) ) return 0;
  return -1;
}

int check_dialogres(const char *poi, bool noneisnull)
{
  CString tmpref;
  loc_entry dummy;
  
  if(chkflg&NODLGCHK) return 0;
  RetrieveResref(tmpref, poi);
  if(noneisnull && (tmpref=="NONE")) tmpref.Empty();
  if(tmpref.IsEmpty())
  {
    if(chkflg&WARNINGS) return 0;
    else return -2;
  }
  if(dialogs.Lookup(tmpref,dummy)) return 0;
  return -1;
}

int check_2da_reference(const char *poi, int flg)
{
  int ret;
  CString tmpref;
  loc_entry dummy;
  
  if(chkflg&NORESCHK) return 0;
  ret=0;
  if(flg)
  {
    RetrieveResref(tmpref,poi);
    if(darefs.Lookup(tmpref, dummy) ) return 0; //2da exists
    return -1; //2da doesn't exist
  }
  if(poi[0]) return 1; //2da set without use
  return 0;
}

int check_vvc_reference(const char *poi, int flg)
{
  int ret;
  CString tmpref;
  loc_entry dummy;
  
  if(chkflg&NORESCHK) return 0;
  ret=0;
  if(flg)
  {
    RetrieveResref(tmpref,poi);
    if(vvcs.Lookup(tmpref, dummy) ) return 0; //vvc exists
    return -1; //vvc doesn't exist
  }
  if(poi[0]) return 1; //vvc set without use
  return 0;
}

int check_area_reference(const char *poi, int flg)
{
  int ret;
  CString tmpref;
  loc_entry dummy;
  
  ret=0;
  RetrieveResref(tmpref,poi);
  if(flg)
  {
    if (flg==2) {
      if (!poi[0]) return 0;
      if (!masterareas.Find(tmpref)) return -2;
    }
    if(areas.Lookup(tmpref, dummy) ) return 0; //area exists
    return -1; //area doesn't exist, but it should
  }
  if(poi[0]) return 1; //area set without use
  return 0;
}

int check_wed_reference(CString tmpref, int flg)
{
  int ret;
  loc_entry dummy;
  
  if(chkflg&NOOTHERCH) return 0;
  ret=0;
  if(flg)
  {
    if(weds.Lookup(tmpref, dummy) ) return 0; //wed exists
    return -1; //area doesn't exist
  }
  if(weds.Lookup(tmpref, dummy) ) return -2; //wed exists but it shouldn't
  return 0;
}

int find_regionlink(const char *poi)
{
  int i;
  
  for(i=0;i<the_area.doorcount;i++)
  {
    if(!strnicmp(poi,the_area.doorheaders[i].regionlink,16)) return i;
  }
  return -1;
}

int check_triggerref(const char *poi, int open)
{
  int i;
  int f;
  
  if(!*poi) return 0;
  for(i=0;i<the_area.triggercount;i++)
  {
    if(!strnicmp(poi,the_area.triggerheaders[i].infoname,16))
    {
      //info triggers could also be blocked
      /*
      if(the_area.triggerheaders[i].triggertype!=2) f=2;
      else*/ f=0;
      if(!(the_area.triggerheaders[i].infoflags&2048) == !open) f|=1;
      return -f;
    }
  }
  return -4;
}

int check_scriptref(const char *poi, bool noneisnull)
{
  CString tmpref;
  loc_entry dummy;
  
  if(chkflg&NOSCRIPT) return 0;
  RetrieveResref(tmpref,poi);
  if(noneisnull && (tmpref=="NONE")) tmpref.Empty();
  if(tmpref.IsEmpty())
  {
    if(chkflg&WARNINGS) return 0;
    else return -2;
  }
  if(scripts.Lookup(tmpref, dummy) ) return 0;
  return -1; //script does not exist
}

int check_creature_reference(const char *poi, int spawns)
{
  CString tmpref;
  loc_entry dummy;
  
  if(chkflg&NOCRECHK) return 0;
  RetrieveResref(tmpref,poi);
  if(tmpref.IsEmpty())
  {
    if(chkflg&WARNINGS) return 0;
    else return -2;
  }
  if(creatures.Lookup(tmpref, dummy) ) return 0;
  if(spawns && spawngroup.Find(tmpref)) return 0;
  return -1; //creature does not exist
}

int check_spell_reference(const char *poi)
{
  CString tmpref;
  loc_entry dummy;
  
  if(chkflg&NOSPLCHK) return 0;
  RetrieveResref(tmpref,poi);
  if(tmpref.IsEmpty())
  {
    if(chkflg&WARNINGS) return 0;
    else return -2;
  }
  if(spells.Lookup(tmpref, dummy) ) return 0;
  return -1; //creature does not exist
}
//needempty : 0  must be filled
//            1  must be empty
//            2  don't care
int check_mos_reference(const char *poi, int needempty)
{
  CString tmpref;
  loc_entry dummy;
  
  if(chkflg&NOOTHERCH) return 0;
  RetrieveResref(tmpref,poi);
  if(tmpref.IsEmpty())
  {
    if(needempty) return 0;
    if(chkflg&WARNINGS) return 0;
    else return -2;
  }
  if(needempty==1) return -2;
  if(mos.Lookup(tmpref, dummy) ) return 0;
  return -1; //creature does not exist
}

int check_movie_reference(const char *poi)
{
  CString tmpref;
  loc_entry dummy;
  
  if(chkflg&NOSPLCHK) return 0;
  RetrieveResref(tmpref,poi);
  if(tmpref.IsEmpty())
  {
    if(chkflg&WARNINGS) return 0;
    else return -2;
  }
  if(movies.Lookup(tmpref, dummy) ) return 0;
  if(wbms.Lookup(tmpref, dummy) ) return 0;
  return -1; //movie does not exist
}

int CChitemDlg::check_mplus(unsigned long mplus)
{
  int ret;
  
  if(chkflg&NOOTHERCH) return 0;
  ret=0;
  if(mplus>10)
  {
    ret|=BAD_MPLUS;
    log("The magic level is higher than 10.");
  }
  
  return ret;
}

int CChitemDlg::check_weaprofs(int itemtype)
{
  int ret;
  int flg;
  int wp;
  unsigned __int64 x;
  
  if(chkflg&NOWPROCH) return 0;
  ret=0;
  wp=the_item.header.weaprof;
  if(!(chkflg&NOTYPECH) )
  {
    if(dltc_weaprofs()) x=itvswp_dltc[itemtype];
    else x=itvswp[itemtype];
    if(wp)
    {
      if(wp<0x59 || wp>29+0x59)
      {
        log("Invalid weapon proficiency: %d.",wp);
        ret|=BAD_WEAPROF;
      }
      else
      {
        flg=1;
        while((x&0xff) && flg)
        {
          if((x&0xff)==wp)
          {
            flg=0;
          }
          x>>=8;
        }
        if(flg)
        {
          log("This is a %s with a %s proficiency.",format_itemtype(itemtype),format_weaprofs(wp));
          ret|=BAD_ITEMTYPE;
        }
      }
    }
    else
    {
      if(x)
      {
        log("No weapon proficiency set for this %s.",format_itemtype(itemtype));
        ret|=BAD_WEAPROF;
      }
    }
  }
  if(!(chkflg&NOEXTCHK) )
  {
    if(wp && !the_item.header.extheadcount)
    {
      ret|=BAD_EXTHEAD;
      log("This is a weapon (%s) without extended header.",format_itemtype(itemtype));
    }
  }
  return ret;
}

int CChitemDlg::check_itemtype(int itemtype)
{
  loc_entry dummy;
  
  if(chkflg&NOTYPECH) return 0;
  if(!unlimited_itemtypes() && (itvs2h[itemtype]&IT2H_NOTBG2))
  {
    log("This itemtype (%s) causes BG2 to crash.",format_itemtype(itemtype));
    return BAD_ITEMTYPE;
  }
  switch(itemtype)
  {
  case IT_BAG:
    if(has_xpvar()) return 0; //this isn't a container in iwd
  case IT_CONTAINER: //iwd2 container type
    if(!stores.Lookup(itemname, dummy) )
    {
      log("This %s will crash the game due to a missing %s.STO file.",format_itemtype(itemtype),itemname);
      return BAD_ITEMTYPE;
    }
  }
  return 0;
}

#define LAWCHAOS 0x31
#define GOODEVIL 0xe
#define CLASSES  0x607fffc0
#define RACES    0x9f800000

int CChitemDlg::check_usability(unsigned long usability)
{
  int ret;
  
  if(chkflg&NOUSECHK) return 0;
  ret=0;
  if(!(usability&LAWCHAOS))
  {
    log("Law/chaos usability flags are inconsistent.");
    ret|=BAD_USE;
  }
  if(!(usability&GOODEVIL))
  {
    log("Good/evil usability flags are inconsistent.");
    ret|=BAD_USE;
  }
  if(!(usability&CLASSES) )
  {
    log("Class usability flags are inconsistent.");
    ret|=BAD_USE;
  }
  if(!(usability&RACES) )
  {
    log("Race usability flags are inconsistent.");
    ret|=BAD_USE;
  }

  return ret;
}

inline int goodletter(int letter)
{
  if(letter==' ') return 1;
  if(letter>='0' && letter<='9') return 1;
  if(letter>='A' && letter<='Z') return 1;
  return 0;
}

int CChitemDlg::check_anim(const char *animtype, int itemtype)
{
  int ret;
  int flg;
  unsigned __int64 x;
  int a1,b1;
  
  if(chkflg&NOANIMCHK) return 0;
  if(!*(unsigned short *) animtype) return 0;
  ret=0;
  if(!goodletter(animtype[0]) || !goodletter(animtype[1]) )
  {
    log("Invalid animation ID '%2.2s' (%04x)",animtype, (int) (*(unsigned short *) animtype) );
    return BAD_ANIM;
  }
  if(animtype[0]!=' ' && !getanimationidx(*(unsigned short *) animtype) )
  {
    log("Non standard animation %2.2s (%04x) is a possible crasher", animtype, (int) (*(unsigned short *) animtype) );
    flg=2;
  }
  else flg=1;
  x=itemanim[itemtype];
  if(!x)
  {
    log("Unknown itemtype");
  }
  
  while((x&0xffff) && flg)
  {
    a1=(int) x&0xff;
    b1=(int) (x>>8)&0xff;
    if((a1==0xff) || (a1==animtype[0]) )
    {
      if((b1==0xff) || (b1==animtype[1]) ) flg=0;
    }
    x>>=16;
  }
  if(flg)
  {
    if(flg==2)
    {
      log("This %s has an invalid animation ID of '%2.2s' (%04x).",format_itemtype(itemtype),animtype,(int) *(short *) animtype);
    }
    else
    {
      log("This %s has an unusual animation ID of '%2.2s' (%04x).",format_itemtype(itemtype),animtype,(int) *(short *) animtype);
    }
    ret|=BAD_ANIM;
  }
  if(animtype[1] && the_item.header.minlevel)
  {
    log("The animation with minimum level can cause a crash, delete the animation flag or the minimum level!");
    ret|=BAD_ANIM;
  }
  return ret;
}

int CChitemDlg::check_attribute(unsigned long attr, int itemtype)
{
  loc_entry dummy_loc;
  int ret;
  int val;
  int dummy;
  
  if(chkflg&NOATTRCH) return 0;
  ret=0;
  if(the_item.header.itmattr&ATTR_TWOHAND)
  {
    val=IT2H_2HANDED; //set
    if(!(chkflg&NOWPROCH) && bg2_weaprofs())
    {
      if(!the_item.header.weaprof)
      {
        log("The 2-handed flag is allowed only for weapons (no weapon proficiency set).");
        ret|=BAD_ATTR;
      }
    }
  }
  else val=IT2H_1HANDED; //not set
  // 1 - only 1 handed, 2 - only 2 handed, 3 - both
  if(!(chkflg&NOTYPECH))
  {
    if(!(itvs2h[itemtype]&val))
    {
      log("The 2-handed flag must %sbe set for this %s.",what2h[val-1],format_itemtype(itemtype));
      ret|=BAD_ATTR;
    }
  }
  if(the_item.header.weaprof) //this is a weapon of some kind
  {
    if(!(chkflg&NOOTHERCH))
    {
      if(!the_item.header.mplus!=!(attr&ATTR_MAGICAL))
      {
        log("The magic flag and the magic level are inconsistently set.");
        ret|=BAD_ATTR;
      }
    }
    
    if(pst_compatible_var()) val=!dialogs.Lookup(the_item.pstheader.dialog, dummy_loc);
    else val=!dial_references.Lookup(itemname, dummy);
    if(the_item.header.itmattr&ATTR_CONVERS)
    {
      if(val)
      {
        log("The conversible flag was set but the item is not registered in itemdial.2da");
        ret|=BAD_ATTR;
      }
    }
    else
    {
      if(!val)
      {
        log("The conversible flag isn't set though the item is registered in itemdial.2da");
        ret|=BAD_ATTR;
      }
    }    
  }
  return ret;
}

int CChitemDlg::check_statr()
{
  int ret;
  
  if(chkflg&NOSTATCH) return 0;
  ret=0;
  if(the_item.header.minlevel>40)
  {
    log("minimum level was set >40");
    ret|=BAD_STAT;
  }
  if(the_item.header.minstr>25)
  {
    log("Minimum strength was set >25");
    ret|=BAD_STAT;
  }
  if(the_item.header.mincha>25)
  {
    log("Minimum charisma was set >25");
    ret|=BAD_STAT;
  }
  if(the_item.header.mincon>25)
  {
    log("Minimum constitution was set >25");
    ret|=BAD_STAT;
  }
  if(the_item.header.mindex>25)
  {
    log("Minimum dexterity was set >25");
    ret|=BAD_STAT;
  }
  if(the_item.header.minint>25)
  {
    log("Minimum intelligence was set >25");
    ret|=BAD_STAT;
  }
  if(the_item.header.minwis>25)
  {
    log("Minimum wisdom was set >25");
    ret|=BAD_STAT;
  }
  if(the_item.header.minstrpl>100)
  {
    log("Minimum strength bonus was set >100");
    ret|=BAD_STAT;
  }
  return ret;
}

int CChitemDlg::check_other()
{
  int ret;
  
  if(chkflg&NOOTHERCH) return 0;
  ret=0;
  if(the_item.header.price>1000000)
  {
    log("Price was set more than 1 mil.");
    ret|=BAD_STAT;
  }
  if((the_item.header.maxstack>100) && (the_item.header.itemtype!=IT_GOLD))
  {
    log("Maximum stack was set more than 100.");
    ret|=BAD_STAT;
  }
  if(the_item.header.weight>1000)
  {
    log("Weight was set more than 1000.");
    ret|=BAD_STAT;
  }
  if(the_item.header.loreid>100)
  {
    log("Lore to ID was set more than 100.");
    ret|=BAD_STAT;
  }
  return ret;
}

int CChitemDlg::check_pro_num(int pronum)
{
  POSITION pos;
  int cnt;
  CString tmp;
  
  if(pronum<256) return 0; //no error, don't care about those
  cnt=pro_references.GetCount();
  if(cnt)
  {
    if(pronum>=cnt) return 1;
    pos=pro_references.FindIndex(pronum);
    if(!pos) return 1;
    tmp=pro_references.GetAt(pos);
    return pro_references.GetAt(pos).GetLength()==0;
  }
  return pronum>256; // no projectl.ids, internal projectile number
}

int CChitemDlg::check_spell_usability(CString spell, unsigned long school)
{
  loc_entry fileloc;
  Cspell myspell;
  unsigned long bits;
  int ret;

  if (!spells.Lookup(spell, fileloc)) return -1;
  ret = read_spell(spell, &myspell);
  if (ret<0) return ret;
  bits = myspell.header.school1 & 0x7fc0;
  if (bits!=school)
  {
    log("Item school:%s doesn't match spell school: %s", format_exclusion(school), format_exclusion(bits) );
    return 1;
  }
  return 0;
}

int CChitemDlg::check_item_effectblock(unsigned long itemtype, unsigned long school)
{
  int fbc;
  int ret;
  int equip_or_use;
  int ext;
  int actfbc, oldfbc;
  CString what;
  CString spell;
  
  ret=0;
  ext=0;
  actfbc=the_item.header.featblkcount;

  if(!actfbc && (chkflg&NOEXTCHK) ) return 0;
  for(fbc=0;fbc<the_item.featblkcount;fbc++)
  {
    equip_or_use=fbc<the_item.header.featblkcount;
    if(equip_or_use)
    {
      what.Format("equipping effect #%d",fbc+1);
    }
    else
    {
      while(actfbc<=fbc)
      {
        if(ext>=the_item.extheadcount)
        {
          log("Mixed up extension headers.");
          return -1;
        }
        else
        {
          oldfbc=actfbc;
          actfbc+=the_item.extheaders[ext].fbcount;
          ext++;
        }
      }
      what.Format("Extended header #%d/%d",ext,1+fbc-oldfbc); //this must be more precise
    }
    if(check_feature(the_item.featblocks[fbc].feature,
      the_item.featblocks[fbc].par1.parl,the_item.featblocks[fbc].par2.parl, 
      the_item.featblocks[fbc].vvc) )
    {
      ret|=BAD_EXTHEAD;
      log("Invalid effect (%s) in %s",get_opcode_text(the_item.featblocks[fbc].feature),what);
      return -1;
    }
    if(check_probability(the_item.featblocks[fbc].prob1,the_item.featblocks[fbc].prob2))
    {
      ret|=BAD_EXTHEAD;
      log("Invalid probability in %s",what);
    }
    if(equip_or_use)
    {
      if (the_item.featblocks[fbc].resist&1)
      {
        ret|=BAD_EXTHEAD;
        log("Dispellable effect in %s",what);
      }

      if(the_item.featblocks[fbc].target!=EFF_TARGET_SELF)
      {
        ret|=BAD_EXTHEAD;
        log("Target type in %s must be Self",what);
      }
    }
    else
    {
      if(the_item.featblocks[fbc].timing==TIMING_EQUIPED)
      {
        ret|=BAD_EXTHEAD;
        log("Equipped effect in %s?",what);
      }
    }
    if(!(chkflg&NODUR))
    {
      //these further checks are very optional
      if(member_array(the_item.featblocks[fbc].timing,has_duration)==-1)
      {
        if(the_item.featblocks[fbc].duration)
        {
          ret|=BAD_EXTHEAD;
          log("Useless duration set in %s",what);
        }
      }
      else
      {
        if(!the_item.featblocks[fbc].duration)
        {
          ret|=BAD_EXTHEAD;
          log("No duration set in %s",what);
        }
      }
      
      if(equip_or_use && (the_item.featblocks[fbc].timing!=TIMING_EQUIPED))
      {
        ret|=BAD_EXTHEAD;
        log("Non-equipping effect in %s",what);
      }
    }

    if (chkflg&NOUSECHK) continue;

    //usability check for scrolls
    if (!equip_or_use && (itemtype==ITM_SCROLL) )
    {
      if (the_item.featblocks[fbc].feature==OPC_LEARNSPELL)
      {
        RetrieveResref(spell, the_item.featblocks[fbc].vvc);
        switch (check_spell_usability(spell, school) )
        {
        case 0: break;
        case 1:
          log("Inconsistent scroll usability.");
          break;
        default:
          log("Invalid spell (%s) taught by scroll.", spell);
        }
      }
    }
  }
  return ret;
}

int CChitemDlg::check_spell_effectblock()
{
  int fbc;
  int ret;
  int equip_or_use;
  int ext;
  int actfbc, oldfbc;
  CString what;
  
  ret=0;
  ext=0;
  actfbc=the_spell.header.featblkcount;

  if(!actfbc && (chkflg&NOEXTCHK) ) return 0;
  for(fbc=0;fbc<the_spell.featblkcount;fbc++)
  {
    equip_or_use=fbc<the_spell.header.featblkcount;
    if(equip_or_use)
    {
      what.Format("casting effect #%d",fbc+1);
    }
    else
    {
      while(actfbc<=fbc)
      {
        if(ext>=the_spell.extheadcount)
        {
          log("Mixed up extension headers.");
          return -1;
        }
        else
        {
          oldfbc=actfbc;
          actfbc+=the_spell.extheaders[ext].fbcount;
          ext++;
        }
      }
      what.Format("Extended header #%d/%d",ext,1+fbc-oldfbc); //this must be more precise
    }
    if(check_feature(the_spell.featblocks[fbc].feature,
      the_spell.featblocks[fbc].par1.parl,the_spell.featblocks[fbc].par2.parl,
      the_spell.featblocks[fbc].vvc) )
    {
      ret|=BAD_EXTHEAD;
      log("Invalid effect (%s) in %s",get_opcode_text(the_spell.featblocks[fbc].feature),what);      
      return -1;
    }
    if(check_probability(the_spell.featblocks[fbc].prob1,the_spell.featblocks[fbc].prob2))
    {
      ret|=BAD_EXTHEAD;
      log("Invalid probability in %s",what);
    }
    if(equip_or_use)
    {
      if(the_spell.featblocks[fbc].target!=EFF_TARGET_SELF)
      {
        ret|=BAD_EXTHEAD;
        log("Target type in %s must be Self",what);
      }
    }
    else
    {
      if(the_spell.featblocks[fbc].timing==TIMING_EQUIPED)
      {
        ret|=BAD_EXTHEAD;
        log("Equipped effect in %s?",what);
      }
    }
    if(chkflg&NODUR) continue;
    //the further checks are very optional
    if(member_array(the_spell.featblocks[fbc].timing,has_duration)==-1)
    {
      if(the_spell.featblocks[fbc].duration)
      {
        ret|=BAD_EXTHEAD;
        log("Useless duration set in %s",what);
      }
    }
    else
    {
      if(!the_spell.featblocks[fbc].duration)
      {
        ret|=BAD_EXTHEAD;
        log("No duration set in %s",what);
      }
    }
  }
  return ret;
}

int CChitemDlg::check_extheader(int itemtype)
{
  int i,j;
  int ret;
  int atype, ttype;
  unsigned int tmp, val;
  
  if(the_item.header.extheadcount!=the_item.extheadcount)
  {
    return -1;
  }
  if(the_item.header.extheadcount>6)
  {
    log("Extended header count (%d) exceeds 6.",the_item.header.extheadcount);
    return BAD_EXTHEAD;
  }
  if(!the_item.header.extheadcount || (chkflg&NOEXTCHK) ) return 0;
  ret=0;
  atype=tooltipnumbers.query_count(itemname);
  if(atype && (the_item.header.extheadcount!=atype))
  {
    if(the_item.header.extheadcount<atype)
    {
      log("Tooltip.2da is inconsistent with extended header counter, more tooltips than expected");
    }
    else
    {
      log("Tooltip.2da is inconsistent with extended header counter, more headers than expected");
    }
    ret|=BAD_EXTHEAD;
  }
  
  for(i=0;i<the_item.header.extheadcount;i++)
  {
    if(the_item.extheaders[i].altdmgdicetype)
    {
      log("Alt damage dice type set:%d",the_item.extheaders[i].altdmgdicetype);
      ret|=BAD_EXTHEAD;
    }
    atype=the_item.extheaders[i].attack_type;
    if(!atype)
    {
      if(chkflg&SKIPEXT)
      {
        log("Dead extended header #%d.",i+1);
        ret|=BAD_EXTHEAD;
      }
      continue; //don't bother looking further
    }
    if(check_atype(atype))
    {
      log("This %s has a wrong attack type (%s) in extended header #%d",format_itemtype(itemtype), get_attack_type(atype), i+1);
      ret|=BAD_EXTHEAD;
    }
    if(atype>=NUM_ATYPE) atype=NUM_ATYPE-1;
    tmp=the_item.extheaders[i].force_id;
    if(tmp<0 || tmp>2)
    {
      log("Force identify flag is %d in extended header #%d.",tmp,i+1);
      ret|=BAD_EXTHEAD;
    }
    tmp=the_item.extheaders[i].location;
    if(tmp&0xff00)
    {
      log("Unknown bits (%x) set in location flag (high byte) in extended header #%d.",tmp>>8,i+1);
    }
    tmp&=0xff;
    if(tmp>=NUM_LFTYPE)
    {
      log("Location flag is %d for %s attack type in extended header #%d.",tmp,get_attack_type(atype),i+1);
      ret|=BAD_EXTHEAD;
    }
    else
    {
      if(check_location(atype,tmp))
      {
        log("Location flag is %s for %s attack type in extended header #%d.",get_location_type(tmp),get_attack_type(atype),i+1);
        ret|=BAD_EXTHEAD;
      }
    }
    switch(check_resource(the_item.extheaders[i].useicon, false) )
    {
    case -1:
      ret|=BAD_ICONREF;
      log("Invalid use icon reference: %-.8s in extended header #%d.",the_item.extheaders[i].useicon,i+1);
      break;
    case -2:
      ret|=BAD_ICONREF;
      log("Empty inventory icon reference in extended header #%d.",i+1);
      break;
    }
    ttype=the_item.extheaders[i].target_type;    
    tmp=the_item.extheaders[i].target_num;
    ret|=check_ttype(atype, ttype, tmp);
    ret|=check_charges(atype,i);    
    
    tmp=the_item.extheaders[i].range;
    if(check_range(atype,tmp))
    {
      log("Illegal range (%d) for %s attack type in extended header #%d.",tmp,get_attack_type(atype),i+1);
      ret|=BAD_EXTHEAD;
    }
    
    ret|=check_ammo(atype, the_item.extheaders[i].damtype, the_item.extheaders[i].projectile);
    
    if(the_item.extheaders[i].speed>20)
    {
      log("The speed factor is bigger than 20 in extended header #%d.",i+1);
      ret|=BAD_EXTHEAD;
    }
    //we accept 32767 as thac0 (auto hit)
    if((the_item.extheaders[i].thaco>20) && the_item.extheaders[i].thaco!=32767)
    {
      log("The THAC0 is bigger than 20 in extended header #%d.",i+1);
      ret|=BAD_EXTHEAD;
    }
    
    switch(atype)
    {
    case 1:
      tmp=0xfffffffc;
      break;
    case 2:
      tmp=0xfffffffc;
      break;
    case 3:
      tmp=0xfffff3ff;
      break;
    case 4:
      tmp=0xfffffffc;
      break;
    default:
      tmp=0xffffffff;
    }
    
    if (iwd2_structures()) {
      tmp&=0xfffcffff;   //Keen and ignore armor
    }
    
    if(the_item.extheaders[i].flags&tmp) {
      log("Unusual item flags: %08x for %s in extended header #%d.", the_item.extheaders[i].flags, get_attack_type(atype), i+1);
      ret|=BAD_EXTHEAD;
    }
    
    tmp=the_item.extheaders[i].proref;
    if(check_pro_num(tmp) )
    {
      log("Effect animation (%d) may be wrong in extended header #%d.",tmp,i+1);
      ret|=BAD_EXTHEAD;
    }
    
    tmp=0;
    for(j=0;j<3;j++) tmp+=the_item.extheaders[i].animation[j];
    switch(atype)
    {
    case A_MELEE:
      if(tmp!=100)
      {
        log("Melee animation numbers don't sum up as 100 in extended header #%d.",i+1);
        ret|=BAD_EXTHEAD;
      }
      break;
    case A_BOW:
      if(tmp)
      {
        log("Melee animation numbers should be 0 in extended header #%d.",i+1);
        ret|=BAD_EXTHEAD;
      }
      break;
    default:
      if(tmp && tmp!=100)
      {
        log("Melee animation numbers don't sum up as 100 in extended header #%d.",i+1);
        ret|=BAD_EXTHEAD;
      }
    }
    tmp=0;
    for(j=0;j<3;j++)
    {
      val=the_item.extheaders[i].missile[j];
      if(val)
      {
        if(tmp)
        {
          log("%s is set simultaneously with %s",get_missile_type(tmp-1),get_missile_type(j) );
          ret|=BAD_EXTHEAD;
        }
        if(val!=1)
        {
          log("%s is set to %d",get_missile_type(j),val);
          ret|=BAD_EXTHEAD;
        }
        tmp=j+1;
      }
    }
    switch (atype)
    {
    case A_PROJ:
      //some projectiles like throwing axes seem to be mixed up with qualifier
      //or not qualifier, more investigation needed
      break;
    case A_BOW:
      if (!tmp)
      {
        log("No projectile qualifier for %s", get_attack_type(atype) );
        ret|=BAD_EXTHEAD;
      }
      break;
    default:
      if (tmp)
      {
        log("Projectile qualifier %s for %s", get_missile_type(tmp-1), get_attack_type(atype) );
        ret|=BAD_EXTHEAD;
      }
    }
  }  
  return ret;
}

int CChitemDlg::check_spl_extheader()
{
  int i;
  int ret;
  int minlevel;
  int ttype, tmp;
  
  ret=0;
  if(the_spell.header.extheadcount!=the_spell.extheadcount)
  {
    return -1;
  }
  for(i=0;i<the_spell.header.extheadcount;i++)
  {
    if(check_spelllocation(the_spell.extheaders[i].location) )
    {
      ret=-1;
      log("Invalid location for spell, should be spell or innate (check header #%d).",i+1);
    }
    ttype=the_spell.extheaders[i].target_type;    
    tmp=the_spell.extheaders[i].target_num;
    ret|=check_ttype(-1, ttype, tmp);
    if(i)
    {
      if(minlevel>=the_spell.extheaders[i].level)
      {
        ret=-1;
        log("The extended headers are not ordered by level (check header #%d).",i+1);
      }
    }
    minlevel=the_spell.extheaders[i].level;
  }
  return ret;
}

///// extended header
int CChitemDlg::check_ammo(int atype, int dtype, int ammo)
{
  int i;
  int ret;
  // ammo can be: 0, 1, 2, 3, 0x28, 0x64
  ret=BAD_EXTHEAD;
  for(i=0;i<NUM_AMTYPE && ret;i++)
  {
    if(ammos[i]==ammo) ret=0;
  }
  if(ret)
  {
    log("The ammo qualifier (%d) is invalid.",ammo);
    return ret;
  }
  switch(atype)
  {
  case A_MAGIC:
    if(ammo)
    {
      log("Ammo type set for magic attack (%d).",ammo);
      return BAD_EXTHEAD;
    }
    break;
  case A_PROJ:
    if(dtype!=DT_RANGED)
    {
      if (iwd2_structures())
      {
        if (dtype!=DT_BLUNTRANGED)
        {
          log("Damage type is not set as %s or %s.",get_damage_type(DT_RANGED), get_damage_type(DT_BLUNTRANGED));
          return BAD_EXTHEAD;
        }
      }
      else
      {
        log("Damage type is not set as %s.",get_damage_type(DT_RANGED));
        return BAD_EXTHEAD;
      }      
    }
    break;
  }
  //
  //more checks, spear for spear, axe for axe etc
  //
  return 0;
}

int CChitemDlg::check_range(int atype, int range)
{
  switch(atype)
  {
  case A_MELEE:
    //iwd2 allows this
    if((range>3) && !iwd2_structures())
    {
      return BAD_EXTHEAD;
    }
  }
  return 0;
}

int CChitemDlg::check_charges(int atype, int i)
{
  int recharge, drain, charges;
  int ret;
  
  ret=0;
  ///
  ///
  recharge=the_item.extheaders[i].flags&~1;
  drain=the_item.extheaders[i].drained;
  charges=the_item.extheaders[i].charges;
  //hack to not complain about bgee files
  if(bg1_compatible_area() && !tob_specific())
  {
    if(recharge) log("BG1 item with recharge flags");
    if(drain==3) log("BG1 item with 'per day' charge type");
  }
  switch(atype)
  {
  case A_MAGIC:
    switch(drain)
    {
    case 0:
      if(charges || (recharge&2048) )
      {
        log("Extended header #%d has charges or recharges but doesn't destruct item.",i+1);
        ret|=BAD_EXTHEAD;
      }
      break;
    case 1: case 2:
    /* permanent magic items
    if(!charges)
    {
    log("Extended header #%d has no charges but drained.",i+1);
    ret|=BAD_EXTHEAD;
    }
      */
      if(recharge&8)
      {
        log("Extended header #%d destructs item but recharges?",i+1);
        ret|=BAD_EXTHEAD;
      }
      break;
    case 3:
      //2049
      if(recharge&(~3072)) //add iwd2 flags too
      {
        log("Extended header #%d has unknown recharge flag: %8x.",i+1,recharge);
        ret|=BAD_EXTHEAD;
      }
      break;
    default:
      log("Extended header #%d has unknown drained flag: %d.",i+1,drain);
      ret|=BAD_EXTHEAD;
      break;
    }
    break;
    case A_MELEE:
      if((drain==3) || charges || (recharge&2048))
      {
        log("Melee extended header #%d has charges, recharges or 'per day' flag.",i+1);
        ret|=BAD_EXTHEAD;
      }
      break;
  }
  return ret;
}

int CChitemDlg::check_spelllocation(int location)
{
  if((location==LF_SPELL) || (location==LF_INNATE)) return 0;
  return 1;
}

int CChitemDlg::check_location(int atype, int location)
{
  switch(atype)
  {
  case 1: if(location==LF_WEAPON) return 0;
    break;
  case 2: if(location==LF_WEAPON) return 0;
    break;
  case 3:
    if(location==LF_MAGIC) return 0;
    break;
  case 4: if(location==LF_WEAPON) return 0;
    break;
  }
  return 1;
}

int CChitemDlg::check_ttype(int atype, int ttype, int tnum)
{
  int ret;
  int x;
  
  if(atype==-1)
  {
    x=0x13457; //spell target types
  }
  else
  {
    x=atypevsttype[atype];
  }
  ret=BAD_EXTHEAD;
  while(x&15)
  {
    if(ttype==(x&15))
    {
      ret=0;
      break;
    }
    x>>=4;
  }
  if(ret)
  {
    log("Target type (%s) is invalid for %s.",get_target_type(ttype),get_attack_type(atype));
    return(BAD_EXTHEAD);
  }
  x=ttypevstnum[ttype];
  if(!tnum) tnum=1; // 0 is always accepted as 1
  if((x!=0xff) && (x!=tnum))
  {
    log("Target number (%d) is invalid for %s.",tnum,get_target_type(ttype));
    return(BAD_EXTHEAD);
  }
  return 0;
}

int CChitemDlg::check_atype(int atype)
{
  int x;
  
  if(atype>4) return BAD_EXTHEAD;
  //  x=itvsatype[itemtype];
  x=itvsatype;
  while(x&15)
  {
    if(atype==(x&15)) return 0;
    x>>=4;
  }
  return BAD_EXTHEAD;
}

//match the item on a pattern
int valid[]={1,2,
-1};
int valid2[]={1,4,5,
-1};

bool CChitemDlg::match_item()
{
  CString tmpstr;
  bool found;
  int exthead;
  int featblock, featblock2;
  search_data tmpdata;
  int loc, cnt;
  int *strref_opcodes=get_strref_opcodes();
  
#if 0
  for(int i=0;i<the_item.extheadcount;i++)
  {
    if (the_item.extheaders[i].damtype>9)
    {
      log("Unusual damage type: %d",the_item.extheaders[i].damtype); 
    }
  }
#endif
  
  memset(&tmpdata,0,sizeof(tmpdata) );
  exthead=featblock=featblock2=loc=0;
  if(searchflags&MS)
  {
    found=false;
    for(featblock=0;featblock<the_item.featblkcount && !found;featblock++)
    {
      if(member_array(the_item.featblocks[featblock].feature,strref_opcodes)!=-1)
      {
        if(the_item.featblocks[featblock].par1.parl==searchdata.strref) found=true;
      }
    }
    if(!found)
    {
      if(searchdata.strref!=the_item.header.unidref &&
        searchdata.strref!=the_item.header.uniddesc &&
        searchdata.strref!=the_item.header.idref &&
        searchdata.strref!=the_item.header.iddesc)
      {
        return false;
      }
    }
  }
  if(searchflags&MT)
  {
    tmpdata.itemtype=the_item.header.itemtype;
    if(tmpdata.itemtype<searchdata.itemtype ||
      tmpdata.itemtype>searchdata.itemtype2)
    {
      return false;
    }
  }
  if(searchflags&MP)
  { //looking for an effect in the extended header
    found=false;
    for(exthead=0;exthead<the_item.extheadcount && !found;exthead++)
    {
      tmpdata.projectile=the_item.extheaders[exthead].proref;
      //      tmpdata.projectile=the_item.extheaders[exthead].target_type;
      if(tmpdata.projectile>=searchdata.projectile &&
        tmpdata.projectile<=searchdata.projectile2)
      {
        found=true;
        break;
      }
    }
    if(!found) return false;
  }
  if(searchflags&MF)
  {
    found=false;
    for(featblock=0;featblock<the_item.featblkcount && !found;featblock++)
    {
      tmpdata.feature=the_item.featblocks[featblock].feature;
      tmpdata.param1=the_item.featblocks[featblock].par1.parl;
      tmpdata.param2=the_item.featblocks[featblock].par2.parl;
      if(tmpdata.feature>=searchdata.feature &&
        tmpdata.feature<=searchdata.feature2)
      {
        found=true;
        break;
      }
    }
    if(!found) return false;
  }
  if(searchflags&MR)
  {
    if (!strnicmp(the_item.header.descicon, searchdata.resource,8))
    {
      log("Found description icon %.s",searchdata.resource);
    }
    if (!strnicmp(the_item.header.grnicon, searchdata.resource,8))
    {
      log("Found ground icon %.s",searchdata.resource);
    }
    if (!strnicmp(the_item.header.invicon, searchdata.resource,8))
    {
      log("Found inventory icon %.s",searchdata.resource);
    }
    found=false;
    for(featblock2=0;featblock2<the_item.featblkcount && !found;featblock2++)
    {
      memcpy(tmpdata.resource,the_item.featblocks[featblock2].vvc,8);
      if(!searchdata.resource[0] || !strnicmp(tmpdata.resource,searchdata.resource,8))
      {
        found=true;
        break;
      }
    }
    if(!found) return false;
  }
  if (searchflags&MI)
  {
    memcpy(tmpdata.itemname,the_item.header.animtype,2);
    tmpdata.itemname[2]=0;
    if (searchdata.itemname[0])
    {
      if (strnicmp(the_item.header.animtype, searchdata.itemname,2))
      {
        return false;
      }
    }
  }
    
  if(searchflags&MS)
  {
    log("Using strref #%d",searchdata.strref);
  }
  if(searchflags&MT )
  {
    log("Found %s",format_itemtype(tmpdata.itemtype));
  }
  if(searchflags&MP)
  {
    tmpstr=get_projectile_id(tmpdata.projectile,0);
    log("Found projectile %d-%s in extended header #%d",tmpdata.projectile,tmpstr,exthead+1);
  }
  if(searchflags&MF)
  {
    for(exthead=0;exthead<the_item.extheadcount;exthead++)
    {
      loc=the_item.extheaders[exthead].fboffs;
      cnt=the_item.extheaders[exthead].fbcount;
      if(featblock>=loc && featblock<loc+cnt)
      {
        break;
      }
    }
    if(exthead==the_item.extheadcount)
    {
      log("Found opcode %s (%d %d) in feature block #%d of equipping effects",get_opcode_text(tmpdata.feature),tmpdata.param1,tmpdata.param2,featblock+1);
    }
    else
    {
      log("Found opcode %s (%d %d) in feature block #%d of extended header #%d",get_opcode_text(tmpdata.feature),tmpdata.param1, tmpdata.param2, featblock-loc+1, exthead+1);
    }
  }
  if(searchflags&MR)
  {
    for(exthead=0;exthead<the_item.extheadcount;exthead++)
    {
      loc=the_item.extheaders[exthead].fboffs;
      cnt=the_item.extheaders[exthead].fbcount;
      if(featblock2>=loc && featblock2<loc+cnt)
      {
        break;
      }
    }
    if(tmpdata.resource[0])
    {
      if(exthead==the_item.extheadcount)
      {
        log("Found resource '%-.8s' in feature block #%d of equipping effects",tmpdata.resource,featblock2+1);
      }
      else
      {
        log("Found resource '%-.8s' in feature block #%d of extended header #%d",tmpdata.resource,featblock2-loc+1, exthead+1);
      }
    }
  }
  if (searchflags&MI)
  {
    log("Found animation: %s", tmpdata.itemname);
  }

  if(searchflags&(1<<FLG_CHANGE) )
  {
    switch(searchdata.which)
    {
    case FLG_MRES:
      memcpy(the_item.featblocks[featblock].vvc,searchdata.newresource,8);
      break;
    case FLG_MTYPE: the_item.header.itemtype=(short) searchdata.newvalue; break;
      //FLG_MPROJ //cant do
    default:
      searchflags&=~((1<<FLG_CHANGE)|(1<<FLG_CHANGE2));
      return true;
    }
    write_item(itemname,"");    
  }
  return true;
}

//find external effects by opcode/resource
bool CChitemDlg::match_effect()
{
  int found;
  search_data tmpdata;
  int *strref_opcodes=get_strref_opcodes();    
  
  //to be expanded for other fields
  found=0;
  memset(&tmpdata,0,sizeof(tmpdata) );
  if(searchflags&MF)
  {
    tmpdata.feature=(short) the_effect.header.feature;
    tmpdata.param1=the_effect.header.par1.parl;
    tmpdata.param2=the_effect.header.par2.parl;
    if(tmpdata.feature>=searchdata.feature && 
      tmpdata.feature<=searchdata.feature2)
    {
      found=1; 
    }
    if(found==0) return false;
  }
  found=0;
  if(searchflags&MR)
  {
    memcpy(tmpdata.resource,the_effect.header.vvc,8);
    if(!searchdata.resource[0] || strnicmp(tmpdata.resource,searchdata.resource,8))
    {
      memcpy(tmpdata.resource,the_effect.header.resource,8);
      if(!strnicmp(tmpdata.resource,searchdata.resource,8) )
      {
        found=1;
      }
    }
    else
    {
      found=2;
    }
    if(found==0) return false;
  }
  if(searchflags&MS)
  {
    tmpdata.feature=(short) the_effect.header.feature;
    if(member_array(tmpdata.feature,strref_opcodes)==-1) return false;
    tmpdata.param1=the_effect.header.par1.parl;
    if(tmpdata.param1!=searchdata.strref) return false;
    tmpdata.param2=the_effect.header.par2.parl;
  }
  
  if(searchflags&(MF|MS) )
  {
    log("Found opcode %s (%d %d)",get_opcode_text(tmpdata.feature),tmpdata.param1,tmpdata.param2);
  }
  if(searchflags&MR)
  {
    if(tmpdata.resource[0])
    {
      log("Found resource '%-.8s' in %s",tmpdata.resource,found==1?"resref":"vvc");
    }
  }
#ifdef _DEBUG
  if(the_effect.header.par3)
  {
    log("Par3: %d",the_effect.header.par3);
  }
#endif
  return true;
}

//find spells by opcode/resource/projectile
bool CChitemDlg::match_spell()
{
  CString tmpstr;
  bool found;
  int exthead;
  int featblock, featblock2;
  search_data tmpdata;
  int loc, cnt;
  int *strref_opcodes=get_strref_opcodes();
  
#if 0
  if (the_spell.header.splattr) {
    log("Spell with special attribute: %x", the_spell.header.splattr);
  }
#endif
  exthead=featblock=featblock2=loc=0;
  memset(&tmpdata,0,sizeof(tmpdata) );
  if(searchflags&MS)
  {
    found=false;
    for(featblock=0;featblock<the_spell.featblkcount && !found;featblock++)
    {
      if(member_array(the_spell.featblocks[featblock].feature,strref_opcodes)!=-1)
      {
        if(the_spell.featblocks[featblock].par1.parl==searchdata.strref) found=true;
      }
    }
    if(!found)
    {
      if(searchdata.strref!=the_spell.header.spellname &&
        searchdata.strref!=the_spell.header.desc)
      {
        return false;
      }
    }
  }
  if(searchflags&MT)
  {
    tmpdata.itemtype=the_spell.header.spelltype;
    if(tmpdata.itemtype<searchdata.itemtype ||
      tmpdata.itemtype>searchdata.itemtype2)
    {
      return false;
    }
  }
  found=false;
  if(searchflags&MP)
  { //looking for an effect in the extended header
    for(exthead=0;exthead<the_spell.extheadcount && !found;exthead++)
    {
      tmpdata.projectile=the_spell.extheaders[exthead].proref;
      //      tmpdata.projectile=the_spell.extheaders[exthead].target_type;
      if(tmpdata.projectile>=searchdata.projectile &&
        tmpdata.projectile<=searchdata.projectile2)
      {
        found=true;
        break;
      }
    }
    if(!found) return false;
  }
  found=false;
  if(searchflags&MF)
  {
    for(featblock=0;featblock<the_spell.featblkcount && !found;featblock++)
    {
      tmpdata.feature=the_spell.featblocks[featblock].feature;
      tmpdata.param1=the_spell.featblocks[featblock].par1.parl;
      tmpdata.param2=the_spell.featblocks[featblock].par2.parl;
      if(tmpdata.feature>=searchdata.feature &&
        tmpdata.feature<=searchdata.feature2)
      {
        found=true;
        break;
      }
    }
    if(!found) return false;
  }
  found=false;
  if(searchflags&MR)
  {
    for(featblock2=0;featblock2<the_spell.featblkcount && !found;featblock2++)
    {
      memcpy(tmpdata.resource,the_spell.featblocks[featblock2].vvc,8);
      if(!searchdata.resource[0] || !strnicmp(tmpdata.resource,searchdata.resource,8))
      {
        found=true;
        break;
      }
    }
    if(!found) return false;
  }
  
  if(searchflags&MS)
  {
    log("Using strref #%d",searchdata.strref);
  }
  if(searchflags&MT)
  {
    log("Found %s",format_spelltype(tmpdata.itemtype));
  }
  if(searchflags&MP)
  {
    tmpstr=get_projectile_id(tmpdata.projectile,0);
    log("Found projectile %d-%s in extended header #%d",tmpdata.projectile,tmpstr,exthead+1);
  }
  if(searchflags&MF)
  {
    for(exthead=0;exthead<the_spell.extheadcount;exthead++)
    {
      loc=the_spell.extheaders[exthead].fboffs;
      cnt=the_spell.extheaders[exthead].fbcount;
      if(featblock>=loc && featblock<loc+cnt)
      {
        break;
      }
    }
    if(exthead==the_spell.extheadcount)
    {
      log("Found opcode %s (%d %d) in feature #%d of casting feature block",get_opcode_text(tmpdata.feature),tmpdata.param1,tmpdata.param2,featblock+1);
      strncpy(searchdata.foundres, the_spell.featblocks[featblock].vvc, 8);
      if (searchflags&MR)
      {
        log("Effect: %s", searchdata.foundres);
      }
    }
    else
    {
      log("Found opcode %s (%d %d) in feature #%d of extended header #%d",get_opcode_text(tmpdata.feature),tmpdata.param1,tmpdata.param2,featblock-loc+1, exthead+1);
    }
    strncpy(searchdata.foundres, the_spell.featblocks[featblock].vvc, 8);
    if (searchflags&MR)
    {
      log("Effect: %s", searchdata.foundres);
    }
  }
  if(searchflags&MR)
  {
    if(tmpdata.resource[0])
    {
      for(exthead=0;exthead<the_spell.extheadcount;exthead++)
      {
        loc=the_spell.extheaders[exthead].fboffs;
        cnt=the_spell.extheaders[exthead].fbcount;
        if(featblock2>=loc && featblock2<loc+cnt)
        {
          break;
        }
      }
      if(exthead==the_spell.extheadcount)
      {
        log("Found resource '%-.8s' in feature block #%d of equipping effects",tmpdata.resource,featblock2+1);
      }
      else
      {
        log("Found resource '%-.8s' in feature block #%d of extended header #%d",tmpdata.resource,featblock2-loc+1, exthead+1);
      }
    }
  }
  if(searchflags&(1<<FLG_CHANGE) )
  {
    switch(searchdata.which)
    {
    case FLG_MRES:
      memcpy(the_spell.featblocks[featblock].vvc,searchdata.newresource,8);
      break;
    case FLG_MTYPE: the_spell.header.spelltype=(short) searchdata.newvalue; break;
      //FLG_MPROJ //cant do
    default:
      searchflags&=~((1<<FLG_CHANGE)|(1<<FLG_CHANGE2));
      return true;
    }
    write_item(itemname,"");    
  }
  return true;
}

bool CChitemDlg::match_chui()
{
  CString tmpstr;
  int i,j,pos;
  int type;
  bool gret, ret;
  
  gret=false;
  for(i=0;i<the_chui.windowcnt;i++)
  {
    ret=false;
    if(searchflags&MR)
    {
      RetrieveResref(tmpstr,the_chui.windows[i].mos);
      if(tmpstr==searchdata.resource)
      {
        log("Found '%s' in window %d",tmpstr,i+1);
        ret=true;
      }
      
    }
    for(j=0;j<the_chui.windows[i].controlcount;j++)
    {
      ret=false;
      pos=the_chui.windows[i].controlidx+j;
      type=the_chui.controls[pos].controltype;
      
      if(searchflags&MT)
      {
        if(type>=searchdata.itemtype && type<=searchdata.itemtype2)
        {
          log("Found control %s (%d) in window %d",ChuiControlName(type),j+1,i+1);
          ret=true;
        }
      }
      if(searchflags&(MR|MS) )
      {
        switch(type)
        {
        case CC_BUTTON://    0
          {
            chui_button *cc=(chui_button *) the_chui.extensions[pos];
            if(searchflags&MR)
            {
              RetrieveResref(tmpstr,cc->bam);
              if(tmpstr==searchdata.resource)
              {
                log("Found '%s' in window %d, control: %d",tmpstr,i+1,j+1);
                ret=true;
              }
            }
            
          }
          break;
        case CC_SLIDER://    2
          {
            chui_slider *cc=(chui_slider *) the_chui.extensions[pos];
            if(searchflags&MR)
            {
              RetrieveResref(tmpstr,cc->bam);
              if(tmpstr==searchdata.resource)
              {
                log("Found '%s' in window %d, control: %d",tmpstr,i+1,j+1);
                ret=true;
              }
            }
          }
          break;
        case CC_EDITBOX://   3
          {
            chui_editbox *cc=(chui_editbox *) the_chui.extensions[pos];
            if(searchflags&MR)
            {
              RetrieveResref(tmpstr,cc->font);
              if(tmpstr==searchdata.resource)
              {
                log("Found '%s' in window %d, control: %d",tmpstr,i+1,j+1);
                ret=true;
              }
            }
          }
          break;
        case CC_TEXT://      5
          {
            chui_textarea *cc=(chui_textarea *) the_chui.extensions[pos];
            if(searchflags&MR)
            {
              RetrieveResref(tmpstr,cc->bam1);
              if(tmpstr==searchdata.resource)
              {
                log("Found '%s' in window %d, control: %d",tmpstr,i+1,j+1);
                ret=true;
              }
              
              RetrieveResref(tmpstr,cc->bam2);
              if(tmpstr==searchdata.resource)
              {
                log("Found '%s' in window %d, control: %d",tmpstr,i+1,j+1);
                ret=true;
              }
            }
          }
          break;
        case CC_LABEL: //    6
          {
            chui_label *cc=(chui_label *) the_chui.extensions[pos];
            if(searchflags&MR)
            {
              RetrieveResref(tmpstr,cc->font);
              if(tmpstr==searchdata.resource)
              {
                log("Found '%s' in window %d, control: %d",tmpstr,i+1,j+1);
                ret=true;
              }
            }
            if(searchflags&MS)
            {
              if(cc->strref==searchdata.strref)
              {
                log("Found strref #%d in window %d, control: %d",searchdata.strref,i+1,j+1);
                ret=true;
              }
            }
          }
          break;
        case CC_SCROLLBAR:// 7
          {
            chui_scrollbar *cc=(chui_scrollbar *) the_chui.extensions[pos];
            if(searchflags&MR)
            {
              RetrieveResref(tmpstr,cc->bam);
              if(tmpstr==searchdata.resource)
              {
                log("Found '%s' in window %d, control: %d",tmpstr,i+1,j+1);
                ret=true;
              }
            }
          }
          break;
        default:
          log("Unknown control type: %d in %d window",type, i+1);
          break;
        }
      }
    }
    if(ret) gret=1;
  }
  return gret;
}

//find creatures
bool CChitemDlg::match_creature()
{
  CString tmpstr;
  bool found;
  int featblock, itemidx;
  unsigned short idx;
  search_data tmpdata;
  bool id;
  int strreftype;
  int i;
  
  id=(the_creature.header.effbyte!=0);
  itemidx=featblock=0;
  idx=0;
  strreftype=0;
  memset(&tmpdata,0,sizeof(tmpdata) );
  tmpdata.which=-1;
  found=false;
  if(searchflags&MS)
  {
    if(searchdata.strref!=the_creature.header.shortname &&
      searchdata.strref!=the_creature.header.longname)
    {
      for (i=0;i<SND_SLOT_COUNT;i++)
      {
        if (searchdata.strref==the_creature.header.strrefs[i])
        {
          strreftype=i+1;
          break;
        }
      }
      if (i==SND_SLOT_COUNT)
      {
        return false;
      }
    }
  }
  if(searchflags&MT)
  {
    tmpdata.itemtype=(unsigned short) the_creature.header.animid;
    if((unsigned short) tmpdata.itemtype<(unsigned short) searchdata.itemtype ||
      (unsigned short) tmpdata.itemtype>(unsigned short) searchdata.itemtype2)
    {
      return false;
    }
  }
  
  if(searchflags&MP)
  {
    tmpdata.projectile=the_creature.header.flags;
    if (!(tmpdata.projectile&searchdata.projectile))
    {
      return false;
    }
  }
  
  if(searchflags&(1<<FLG_MEA) )
  {
    tmpdata.ea=the_creature.header.idsea;
    if(tmpdata.ea<searchdata.ea ||
      tmpdata.ea>searchdata.ea2)
    {
      return false;
    }
  }
  if(searchflags&(1<<FLG_MGENERAL) )
  {
    tmpdata.general=the_creature.header.idsgeneral;
    if(tmpdata.general<searchdata.general ||
      tmpdata.general>searchdata.general2)
    {
      return false;
    }
  }
  if(searchflags&(1<<FLG_MRACE) )
  {
    tmpdata.race=the_creature.header.idsrace;
    if(tmpdata.race<searchdata.race ||
      tmpdata.race>searchdata.race2)
    {
      return false;
    }
  }
  if(searchflags&(1<<FLG_MCLASS) )
  {
    tmpdata.class1=the_creature.header.idsclass;
    if(tmpdata.class1<searchdata.class1 ||
      tmpdata.class1>searchdata.class2)
    {
      return false;
    }
  }
  if(searchflags&(1<<FLG_MSPECIFIC) )
  {
    tmpdata.specific=the_creature.header.idsspecific;
    if(tmpdata.specific<searchdata.specific ||
      tmpdata.specific>searchdata.specific2)
    {
      return false;
    }
  }
  if(searchflags&(1<<FLG_MGENDER) )
  {
    tmpdata.gender=the_creature.header.idsgender;
    if(tmpdata.gender<searchdata.gender ||
      tmpdata.gender>searchdata.gender2)
    {
      return false;
    }
  }
  if(searchflags&(1<<FLG_MALIGN) )
  {
    tmpdata.align=the_creature.header.idsalign;
    if(tmpdata.align<searchdata.align ||
      tmpdata.align>searchdata.align2)
    {
      return false;
    }
  }
  
  if(searchflags&MI)
  {
    found=false;
    for(itemidx=0;itemidx<SLOT_COUNT;itemidx++)
    {
      idx=the_creature.itemslots[itemidx];
      if(idx>SLOT_COUNT) continue;
      memcpy(tmpdata.itemname,the_creature.items[idx].itemname,8);
      if(searchdata.itemname[0])
      {
        if(!strnicmp(tmpdata.itemname,searchdata.itemname,8) )
        {
          found=true;
          break;
        }
      }
      else
      {
        if(tmpdata.itemname[0])
        {
          found=true;
          break;
        }
      }
    }
    if(!searchdata.itemname[0]) found=true;
    if(!found) return false;
  }
  
  if(searchflags&MF)
  {
    found=false;
    for(featblock=0;featblock<the_creature.header.effectcnt;featblock++)
    {
      if(id)
      {
        tmpdata.feature=(short) the_creature.effects[featblock].feature;
        tmpdata.param1=the_creature.effects[featblock].par1.parl;
        tmpdata.param2=the_creature.effects[featblock].par2.parl;
      }
      else
      {
        tmpdata.feature=(short) the_creature.oldeffects[featblock].feature;
        tmpdata.param1=the_creature.oldeffects[featblock].par1.parl;
        tmpdata.param2=the_creature.oldeffects[featblock].par2.parl;
      }
      if((tmpdata.feature>=searchdata.feature &&
        tmpdata.feature<=searchdata.feature2) )
      {
        tmpdata.which=featblock;
        found=true;
        break;
      }
    }
    if(!found) return false; 
  }
  
  if(searchflags&MV)
  {
    found=false;
    memcpy(tmpdata.variable, the_creature.header.dvar,32);
    if(!strnicmp(tmpdata.variable, searchdata.variable, 32) )
    {
      tmpdata.which=0;
      featblock=-1;
      found=true;      
    }
    if (!found && id)
    {
      if(tmpdata.which==-1)
      {
        for(featblock=0;featblock<the_creature.header.effectcnt;featblock++)
        {
          memcpy(tmpdata.variable,the_creature.effects[featblock].variable,32);
          if(!strnicmp(tmpdata.variable,searchdata.variable,32) )
          {
            tmpdata.which=featblock;
            found=true;
            break;
          }
        }
      }
      else
      {
        memcpy(tmpdata.variable,the_creature.effects[featblock].variable,32);
        if(searchdata.variable[0])
        {
          if(!strnicmp(tmpdata.variable,searchdata.variable,32) )
          {
            found=true;
          }
        }
        else found=true;
      }
    }
    if(!found) return false;
  }
  
  if(searchflags&MR)
  {
    found=false;
    if(searchdata.resource[0])
    {
      if(tmpdata.which==-1)
      {
        for(featblock=0;featblock<5;featblock++)
        {
          memcpy(tmpdata.resource,the_creature.header.scripts[featblock],8);
          if(!strnicmp(tmpdata.resource,searchdata.resource,8) )
          {
            tmpdata.which=featblock;
            found=true;
            break;
          }
        }
      }
    }
    if(!found && searchdata.resource[0])
    {
      if(tmpdata.which==-1)
      {
        for(featblock=0;featblock<the_creature.header.effectcnt;featblock++)
        {
          if (id)
          {
            memcpy(tmpdata.resource,the_creature.effects[featblock].resource,8);
          }
          else
          {
            memcpy(tmpdata.resource,the_creature.oldeffects[featblock].vvc,8);
          }
          if(!strnicmp(tmpdata.resource,searchdata.resource,8) )
          {
            found=true;
            break;
          }
        }
      }
      else
      {
        if (id)
        {
          memcpy(tmpdata.resource,the_creature.effects[featblock].resource,8);
        }
        else
        {
          memcpy(tmpdata.resource,the_creature.oldeffects[featblock].vvc,8);
        }
        if(!strnicmp(tmpdata.resource,searchdata.resource,8) )
        {
          found=true;
        }
      }        
    }
    if (!found)
    {
      memcpy(tmpdata.resource, the_creature.header.iconl,8);
      if (searchdata.resource[0])
      {
        if (!strnicmp(tmpdata.resource, searchdata.resource,8) )
        {
          found=true;
          tmpdata.which=-2;
        }
        else
        {
          memcpy(tmpdata.resource, the_creature.header.iconm,8);
          if (!strnicmp(tmpdata.resource, searchdata.resource,8) )
          {
            found=true;
            tmpdata.which=-2;
          }
        }
      }
      else
      {
        //list only valuable fields
        if (tmpdata.resource[0] && strnicmp(tmpdata.resource,"NONE",8))
        {
          found=true;
          tmpdata.which=-2;
        }
      }
    }
    
    if (!found)
    {
      memcpy(tmpdata.resource, the_creature.header.dialogresref,8);
      if (searchdata.resource[0])
      {
        if (!strnicmp(tmpdata.resource, searchdata.resource,8) )
        {
          found = true;
          tmpdata.which=-3;
        }
      }
    }

    if (!found)
    {
      for(featblock=0;featblock<5;featblock++)
      {
        memcpy(tmpdata.resource, the_creature.header.scripts[featblock],8);
        if (searchdata.resource[0])
        {
          if (!strnicmp(tmpdata.resource, searchdata.resource,8) )
          {
            found = true;
            tmpdata.which=-4;
          }
        }
      }
    }
    if(!found) return false;
  }
  
  if(searchflags&MS)
  {
    if (strreftype)
    {
      log("Using strref #%d in sound slot #%d",searchdata.strref, strreftype);
    }
    else
    {
      log("Using strref #%d as name",searchdata.strref);
    }
  }
  if(searchflags&MT)
  {
    log("Found 0x%x-%s",(unsigned short) tmpdata.itemtype,IDSToken("ANIMATE",(unsigned short) tmpdata.itemtype, false) );
  }
  if(searchflags&MP)
  {
    log("Found MC flag: 0x%x",tmpdata.projectile);
  }
  if(searchflags&MF)
  {
    log("Found opcode %s (%d %d) in #%d. embedded effect",get_opcode_text(tmpdata.feature),tmpdata.param1,tmpdata.param2,featblock+1);
  }
  if(searchflags&MV)
  {
    if (featblock<0)
    {
      log("Found scripting name %-.32s", tmpdata.variable);
    }
    else if (id)
    {
      log("Found variable %-.32s (%d) in #%d. embedded effect",tmpdata.variable,the_creature.effects[featblock].par1,featblock+1);
    }
  }
  if(searchflags&MR)
  {
    if(tmpdata.which>=0)
    {
      log("Found resource '%-.8s' in %s script slot",tmpdata.resource, get_script_type(tmpdata.which));
    }
    else
    {
      switch(tmpdata.which)
      {
      case -1:
        log("Found resource '%-.8s' in #%d. embedded effect",tmpdata.resource, featblock+1);
        break;
      case -2:
        log("Found resource '%-.8s' in portrait icon", tmpdata.resource);
        break;
      case -3:
        log("Found resource '%-.8s' as dialog", tmpdata.resource);
        break;
      case -4:
        log("Found resource '%-.8s' in script slot: %s",tmpdata.resource,get_script_type(featblock));
        break;
      }
    }
  }
  if(searchflags&MI)
  {
    if(itemidx<SLOT_COUNT) tmpstr=slot_names[itemidx];
    else tmpstr.Format("Unknown (%d)",itemidx);
    log("Found item %s at position #%d (slot:%s)",tmpdata.itemname,idx,tmpstr);
  }
  if(searchflags&(1<<FLG_MEA) )
  {
    log("EA: %d",tmpdata.ea);
  }
  if(searchflags&(1<<FLG_MGENERAL) )
  {
    log("GENERAL: %d",tmpdata.general);
  }
  if(searchflags&(1<<FLG_MRACE) )
  {
    log("RACE: %d",tmpdata.race);
  }
  if(searchflags&(1<<FLG_MCLASS) )
  {
    log("CLASS: %d",tmpdata.class1);
  }
  if(searchflags&(1<<FLG_MSPECIFIC) )
  {
    log("SPECIFIC: %d",tmpdata.specific);
  }
  if(searchflags&(1<<FLG_MGENDER) )
  {
    log("GENDER: %d",tmpdata.gender);
  }
  if(searchflags&(1<<FLG_MALIGN) )
  {
    log("ALIGN: %d",tmpdata.align);
  }
  if(searchflags&(1<<FLG_CHANGE) )
  {
    switch(searchdata.which)
    {
    case FLG_MRES:
      if(tmpdata.which!=-1)
      {
        memcpy(the_creature.header.scripts[tmpdata.which],searchdata.newresource,8);
      }
      else
      {
        if(the_creature.header.effbyte)
        {
          memcpy(the_creature.effects[featblock].resource,searchdata.newresource,8);
        }
        else
        {
          memcpy(the_creature.oldeffects[featblock].vvc,searchdata.newresource,8);
        }
      }
      break;
    case FLG_MTYPE:
      the_creature.header.animid=(BYTE) searchdata.newvalue;
      the_creature.iwd2header.animid=(BYTE) searchdata.newvalue;
      break;
    case FLG_MEA:
      the_creature.header.idsea=(BYTE) searchdata.newvalue;
      the_creature.iwd2header.idsea=(BYTE) searchdata.newvalue;
      break;
    case FLG_MGENERAL:
      the_creature.header.idsgeneral=(BYTE) searchdata.newvalue;
      the_creature.iwd2header.idsgeneral=(BYTE) searchdata.newvalue;
      break;
    case FLG_MRACE:
      the_creature.header.idsrace=(BYTE) searchdata.newvalue;
      the_creature.iwd2header.idsrace=(BYTE) searchdata.newvalue;
      break;
    case FLG_MCLASS:
      the_creature.header.idsclass=(BYTE) searchdata.newvalue;
      the_creature.iwd2header.idsclass=(BYTE) searchdata.newvalue;
      break;
    case FLG_MSPECIFIC:
      the_creature.header.idsspecific=(BYTE) searchdata.newvalue;
      the_creature.iwd2header.idsspecific=(BYTE) searchdata.newvalue;
      break;
    case FLG_MGENDER:
      the_creature.header.idsgender=(BYTE) searchdata.newvalue;
      the_creature.iwd2header.idsgender=(BYTE) searchdata.newvalue;
      break;
    case FLG_MALIGN:
      the_creature.header.idsalign=(BYTE) searchdata.newvalue;
      the_creature.iwd2header.idsalign=(BYTE) searchdata.newvalue;
      break;    
    default:
      searchflags&=~((1<<FLG_CHANGE)|(1<<FLG_CHANGE2));
      return true;
    }
    write_creature(itemname,"");    
  }
  
  return true;
}

bool CChitemDlg::match_videocell()
{
  bool found;
  search_data tmpdata;
  
  memset(&tmpdata,0,sizeof(tmpdata) );
  if(searchflags&MR)
  {
    found=false;
    memcpy(tmpdata.resource,the_videocell.header.bam,8);    
    if(searchdata.resource[0])
    {
      if(!strnicmp(tmpdata.resource,searchdata.resource,8) )
      {
        found=true;
      }
    }
    if(!found) return false;
  }
  if(searchflags&MR)
  {
    if(tmpdata.resource[0]) log("Found resource '%-.8s'",tmpdata.resource);
  }
  return true;
}

bool CChitemDlg::match_script()
{
  search_data tmpdata;
  
  memset(&tmpdata,0,sizeof(tmpdata) );
  if(searchflags&MT)
  {
    the_script.RollBack();
    if(!the_script.find_itemtype(searchdata, tmpdata, FLG_MTYPE)) return false;
  }
  
  if(searchflags&MF)
  {
    the_script.RollBack();
    if(!the_script.find_itemtype(searchdata, tmpdata, FLG_MFEAT)) return false;
  }
  
  if(searchflags&MV )
  {
    the_script.RollBack();
    if(!the_script.find_itemtype(searchdata, tmpdata, FLG_MVAR)) return false;
  }
  if(searchflags&MR)
  {
    the_script.RollBack();
    if(!the_script.find_itemtype(searchdata, tmpdata, FLG_MRES)) return false;
  }
  if(searchflags&MS)
  {
    the_script.RollBack();
    if(!the_script.find_itemtype(searchdata, tmpdata, FLG_MSTRREF)) return false;
  }
  
  if(searchflags&MT)
  {
    log("Found in %s",resolve_scriptelement(tmpdata.itemtype, TRIGGER, tmpdata.itemtype2));
  }
  if(searchflags&MF)
  {
    log("Found in %s",resolve_scriptelement(tmpdata.feature, ACTION, tmpdata.feature2));
  }
  if(searchflags&MV )
  {
    log("Found in block #%d:%s",tmpdata.param1+1, tmpdata.variable);
  }
  if(searchflags&MR)
  {
    log("Found in block #%d:%s",tmpdata.param2+1, tmpdata.resource);
  }
  if(searchflags&MS)
  {
    log("Found strref in block #%d",tmpdata.param2+1);
  }
  return true;
}

bool CChitemDlg::match_2da()
{
  int i,j;
  CString tmpstr;
  
  if(searchflags&MV)
  {
    for(j=0;j<the_2da.cols;j++)
    {
      tmpstr=the_2da.collabels[j];
      tmpstr.MakeUpper();
      if(tmpstr.Find(searchdata.variable)!=-1)
      {
        log("Found heading %s in column %d.",tmpstr,j+1);
        return true;
      }
    }
    for(i=0;i<the_2da.rows;i++)
    {
      for(j=0;j<the_2da.cols;j++)
      {
        tmpstr=the_2da.GetValue(i,j);
        tmpstr.MakeUpper();
        if(tmpstr.Find(searchdata.variable)!=-1)
        {
          if(j) log("Found entry %s in row %d, column %d.",tmpstr,i+1,j);
          else log("Found label %s in row %d.",tmpstr,i+1);
          return true;
        }
      }
    }
  }
  if(searchflags&MS)
  {
    for(i=0;i<the_2da.rows;i++)
    {
      for(j=1;j<the_2da.cols;j++)
      {
        tmpstr=the_2da.GetValue(i,j);
        if(strtonum(tmpstr)==searchdata.strref)
        {
          log("Found entry %s in row %d, column %d.",tmpstr,i+1,j);
          return true;
        }
      }
    }
  }
  return false;
}

bool CChitemDlg::match_area()
{
  int actp, spawnp;
  int idx, cidx;
  bool found;
  search_data tmpdata;
  
  //#if 0
#if _DEBUG
  
  int i;
  
  for(i=0;i<the_area.doorcount;i++)
  {
    if(the_area.doorheaders[i].flags&32)
    {
      //log("Door %d (%s) has uncloseable flag",i,the_area.doorheaders[i].doorname);
    }
    if(the_area.doorheaders[i].dialogref[0]) {
      log("Door %d (%s) has dialog resref (%s)",i,the_area.doorheaders[i].doorname, the_area.doorheaders[i].dialogref);
    }
  }
  
  int min, max;

  min = -1;
  max = -1;
  for(i=0;i<the_area.spawncount;i++)
  {
    int x = the_area.spawnheaders[i].difficulty;
    if (i)
    {
      if (x<min) min=x;
      if (x>max) max=x;
    }
    else
    {
      min=x;
      max=x;
    }
  }
#endif
  //#endif
  memset(&tmpdata,0,sizeof(tmpdata) );
  found=true;
  
  if(searchflags&MS )
  {
    for(idx=0;idx<the_area.triggercount;idx++)
    {
      if(the_area.triggerheaders[idx].strref==searchdata.strref)
      {
        log("Found strref #%d in trigger #%d",searchdata.strref, idx+1);
      }
    }
    for(idx=0;idx<the_area.mapnotecount;idx++)
    {
      if(the_area.mapnoteheaders[idx].strref==searchdata.strref)
      {
        log("Found strref #%d in mapnote #%d",searchdata.strref, idx+1);
      }
    }
    for(idx=0;idx<the_area.doorcount;idx++)
    {
      if(the_area.doorheaders[idx].strref==searchdata.strref)
      {
        log("Found strref #%d in door #%d",searchdata.strref, idx+1);
      }
    }
  }
  
  if(searchflags&MT )
  {    
    tmpdata.itemtype=the_area.header.areatype; //dungeon/forest/etc
    if((tmpdata.itemtype<searchdata.itemtype) || (tmpdata.itemtype>searchdata.itemtype2)) return false;
  }

  if(searchflags&MP )
  {
    if (!the_area.header.songoffset) return false;

    for(idx=0;idx<10;idx++)
    {
      tmpdata.projectile = the_area.songheader.songs[idx];
      if ((tmpdata.projectile>=searchdata.projectile) && (tmpdata.projectile<=searchdata.projectile2)) break;
    }
    if (idx==10) return false;
    else tmpdata.projectile2=idx;
  }

  if(searchflags&MF )
  {
    tmpdata.feature=(short) the_area.header.areaflags; //save, tutorial, antimagic, timestop
    if((tmpdata.feature<searchdata.feature) || (tmpdata.feature>searchdata.feature2)) return false;
  }
  if(searchflags&MI)
  {
    for(idx=0;idx<the_area.triggercount;idx++)
    {
      if(!strnicmp(the_area.triggerheaders[idx].key, searchdata.itemname, 8) )
      {
        log("Using key: %.8s in trigger #%d",the_area.triggerheaders[idx].key, idx);
        break;
      }
    }
    for(idx=0;idx<the_area.doorcount;idx++)
    {
      if(!strnicmp(the_area.doorheaders[idx].key,searchdata.itemname, 8) )
      {
        log("Using key: %.8s in door #%d",the_area.doorheaders[idx].key, idx);
        break;
      }
    }
    
    found=false;
    for(idx=0;idx<the_area.itemcount;idx++)
    {
      memcpy(tmpdata.itemname,the_area.itemheaders[idx].itemname,8);
      if(searchdata.itemname[0])
      {
        if(!strnicmp(tmpdata.itemname,searchdata.itemname,8) )
        {
          found=true;
          break;
        }
      }
      else
      {
        if(tmpdata.itemname[0])
        {
          found=true;
          break;
        }
      }
    }
    if(!found) return false;
    for(cidx=0;cidx<the_area.containercount;cidx++)
    {
      if(idx>=the_area.containerheaders[cidx].firstitem && 
        idx<=the_area.containerheaders[cidx].firstitem+the_area.containerheaders[cidx].itemcount)
      {
        break;
      }
    }
  }
  if(searchflags&MV)
  {
    found=false;
    for(actp=0;actp<the_area.variablecount;actp++)
    {
      memcpy(tmpdata.variable,the_area.variableheaders,32);
      if(searchdata.variable[0])
      {
        if(!strnicmp(tmpdata.variable,searchdata.variable,32) )
        {
          found=true;
          break;
        }
      }
      else
      {
        if(tmpdata.variable[0])
        {
          found=true;
          break;
        }
      }
    }
  }

  spawnp = -1;
  if(searchflags&MR)
  {
    found=false;

    if(!strnicmp(the_area.header.scriptref,searchdata.resource,8) )
    {
      log("Using script: %.8s",the_area.header.scriptref);
    }

    for(actp=0;actp<the_area.animcount;actp++)
    {
      if(!strnicmp(the_area.animheaders[actp].bam, searchdata.resource,8) )
      {
        log("Using BAM: %.8s in animation #%d",the_area.animheaders[actp].bam, actp);
        break;
      }

      if(!strnicmp(the_area.animheaders[actp].bmp, searchdata.resource,8) )
      {
        log("Using BMP: %.8s in animation #%d",the_area.animheaders[actp].bmp, actp);
        break;
      }

    }

    for(actp=0;actp<the_area.triggercount;actp++)
    {
      if(!strnicmp(the_area.triggerheaders[actp].destref,searchdata.resource,8) )
      {
        log("Using destination: %.8s in trigger #%d",the_area.triggerheaders[actp].destref, actp);
        break;
      }
      if(!strnicmp(the_area.triggerheaders[actp].scriptref,searchdata.resource,8) )
      {
        log("Using trap script: %.8s in trigger #%d",the_area.triggerheaders[actp].scriptref, actp);
        break;
      }
    }
    
    for(actp=0;actp<the_area.overlaycount;actp++)
    {
      if(!strnicmp(the_area.overlayheaders[actp].tis,searchdata.resource,8) )
      {
        log("Using tileset: %.8s in overlay #%d",the_area.overlayheaders[actp].tis, actp);
        break;
      }
    }
    for(actp=0;actp<the_area.triggercount;actp++)
    {
      if(!strnicmp(the_area.triggerheaders[actp].dialogref,searchdata.resource,8) )
      {
        log("Using dialog: %.8s in trigger #%d",the_area.triggerheaders[actp].dialogref, actp);
        break;
      }
    }   
    
    for(actp=0;actp<the_area.actorcount;actp++)
    {
      memcpy(tmpdata.resource,the_area.actorheaders[actp].creresref,8);
      if(searchdata.resource[0])
      {
        if(!strnicmp(tmpdata.resource,searchdata.resource,8) )
        {
          found=true;
          break;
        }
      }
      else
      {
        if(tmpdata.resource[0])
        {
          found=true;
          break;
        }
      }

      if(!strnicmp(the_area.actorheaders[actp].scrarea,searchdata.resource,8) )
      {
        log("Using area script: %.8s in actor #%d",the_area.actorheaders[actp].scrarea, actp);
        break;
      }
      if(!strnicmp(the_area.actorheaders[actp].scrclass,searchdata.resource,8) )
      {
        log("Using class script: %.8s in actor #%d",the_area.actorheaders[actp].scrclass, actp);
        break;
      }
      if(!strnicmp(the_area.actorheaders[actp].scrdefault,searchdata.resource,8) )
      {
        log("Using default script: %.8s in actor #%d",the_area.actorheaders[actp].scrdefault, actp);
        break;
      }
      if(!strnicmp(the_area.actorheaders[actp].scrgeneral,searchdata.resource,8) )
      {
        log("Using general script: %.8s in actor #%d",the_area.actorheaders[actp].scrgeneral, actp);
        break;
      }
      if(!strnicmp(the_area.actorheaders[actp].scroverride,searchdata.resource,8) )
      {
        log("Using override script: %.8s in actor #%d",the_area.actorheaders[actp].scroverride, actp);
        break;
      }
      if(!strnicmp(the_area.actorheaders[actp].dialog,searchdata.resource,8) )
      {
        log("Using dialog: %.8s in actor #%d",the_area.actorheaders[actp].dialog, actp);
        break;
      }
    }

    if (!found)
    {
      for(spawnp=0;spawnp<the_area.spawncount;spawnp++)
      {
        int actp = the_area.spawnheaders[spawnp].creaturecnt;
        while(actp--)
        {
          if (!strnicmp(searchdata.resource, the_area.spawnheaders[spawnp].creatures[actp], 8) )
          {
            memcpy(tmpdata.resource,the_area.spawnheaders[spawnp].creatures[actp],8);
            found = true;
            break;
          }
        }
        if (found)
        {
          break;
        }
      }
    }
    if (!found)
    {
      spawnp=-1;
    }

    if(!found) return false;
  }
  
  if(searchflags&MT)
  {
    log("Found areatype:0x%x",tmpdata.itemtype);
  }
  if(searchflags&MF)
  {
    log("Found areaflags:0x%x",tmpdata.feature);
  }
  if(searchflags&MP)
  {
    log("Found area song: %d at index %d", tmpdata.projectile, tmpdata.projectile2);
  }
  if(searchflags&MI)
  {
    if(cidx!=the_area.containercount) log("Found item %s in position #%d in container #%d",tmpdata.itemname,idx+1,cidx+1);
    else log("Found item %s in position #%d (no container)",tmpdata.itemname,idx+1);
  }
  if(searchflags&MV)
  {
    log("Found variable %s", tmpdata.variable);
  }
  if(searchflags&MR)
  {
    if (spawnp!=-1)
    {
      log("Found creature %s in spawn header #%d at position #%d",tmpdata.resource, spawnp, actp);
    }
    else
    {
      log("Found creature %s in actor headers at position #%d",tmpdata.resource,actp);
    }
  }
#if _DEBUG
  switch(min)
  {
  case -1:
    //log("No spawns.");
    break;
  case 0:
    log("Difficulty zero");
    break;
  default:
    log("Minimum difficulty: %d, maximum difficulty: %d", min, max);
  }
#endif
  return found;
}

void CChitemDlg::which_transition(int idx)
{
  int i;
  
  for(i=0;i<the_dialog.transcount;i++)
  {
    if(the_dialog.dlgtrans[i].actionindex==idx)
    {
      log("Transition: %d",i);
      return;
    }
  }
  log("Not in any transition");
}

bool CChitemDlg::match_dialog()
{
  int i,j,k;
  int linecnt;
  int trn;
  int ret;
  CString *lines;
  trigger trigger;
  action action;
  
  ret=0;
  if(searchflags&MS)
  {
    for(i=0;i<the_dialog.statecount;i++)
    {
      if(the_dialog.dlgstates[i].actorstr==searchdata.strref)
      {
        ret=1;
        log("State #%d uses strref #%d as actor text",i, searchdata.strref);
        //break;
      }
    }
    if(!ret)
    {
      for(i=0;i<the_dialog.transcount;i++)
      {
        if(the_dialog.dlgtrans[i].playerstr==searchdata.strref)
        {
          ret=2;
          log("Transition #%d uses strref #%d as player text",i, searchdata.strref);
          //break;
        }
        if(the_dialog.dlgtrans[i].journalstr==searchdata.strref)
        {
          switch(the_dialog.dlgtrans[i].flags&(HAS_QUEST|HAS_SOLVED|REMOVE_QUEST))
          {
          case 0:
            ret=3;
            log("Transition #%d uses strref #%d as journal entry",i, searchdata.strref);
            break;
          case HAS_QUEST:
            ret=4;
            log("Transition #%d uses strref #%d as quest entry",i, searchdata.strref);
            break;
          case HAS_SOLVED:
            ret=5;
            log("Transition #%d uses strref #%d as done quest",i, searchdata.strref);
            break;
          case HAS_QUEST|HAS_SOLVED:
            ret=6;
            log("Transition #%d uses strref #%d as user journal",i, searchdata.strref);
            break;
          case REMOVE_QUEST:
            ret=7;
            log("Transition #%d uses strref #%d as remove quest",i, searchdata.strref);
            break;
          default:
            ret=8;
            log("Transition #%d uses strref #%d as unknown",i, searchdata.strref);
            break;
          }
          //break;
        }
      }
    }
    /*
    switch(ret)
    {
    case 1: log("State #%d uses strref #%d as actor text",i, searchdata.strref); break;
    case 2: log("Transition #%d uses strref #%d as player text",i, searchdata.strref); break;
    case 3: log("Transition #%d uses strref #%d as journal entry",i, searchdata.strref); break;
    case 4: log("Transition #%d uses strref #%d as quest entry",i, searchdata.strref); break;
    case 5: log("Transition #%d uses strref #%d as done quest",i, searchdata.strref); break;
    case 6: log("Transition #%d uses strref #%d as user journal",i, searchdata.strref); break;
    case 7: log("Transition #%d uses strref #%d as remove quest",i, searchdata.strref); break;
    case 8: log("Transition #%d uses strref #%d as unknown",i, searchdata.strref); break;
    }
    */
  }
  
  if(searchflags&(MR| MV| MT) )
  {
    ret=0;
    for(i=0;i<the_dialog.sttriggercount;i++)
    {
      lines=explode(the_dialog.sttriggers[i],'\n',linecnt);
      for(k=0;k<linecnt;k++)
      {
        if(compile_trigger(lines[k],trigger)) continue;
        
        if(searchflags&MT)
        {
          //cutting the weird flags from the trigger
          j=trigger.opcode&~0x4000;
          if(j>=searchdata.itemtype && j<=searchdata.itemtype2)
          {
            trn = the_dialog.findtrigger(i);
            ret=1;
            log("Found %s in state trigger with weight %d (#%d)",lines[k],i+1, trn);
          }
        }
        if(searchflags&MV)
        {
          if(!strnicmp(trigger.var1+6,searchdata.variable,32))
          {
            ret=1;
            log("Found %s in state trigger #%d",trigger.var1,i+1);
          }
          if(!strnicmp(trigger.var2+6,searchdata.variable,32))
          {
            ret=1;
            log("Found %s in state trigger #%d",trigger.var2,i+1);
          }
        }
        if(searchflags&MR)
        {
          if(!strnicmp(trigger.trobj.var,searchdata.resource,8))
          {
            ret=1;
            log("Found %s in state trigger #%d",trigger.trobj.var,i+1);
          }
        }
      }
      if(lines)
      {
        delete [] lines;
        lines=NULL;
      }
    } //end for
    
    for(i=0;i<the_dialog.trtriggercount;i++)
    {
      lines=explode(the_dialog.trtriggers[i],'\n',linecnt);
      for(k=0;k<linecnt;k++)
      {
        if(compile_trigger(lines[k],trigger)) continue;
        if(searchflags&MT)
        {
          //cutting the weird flags from the trigger
          j=trigger.opcode&~0x4000;
          if(j>=searchdata.itemtype && j<=searchdata.itemtype2)
          {
            ret=1;
            log("Found %s in transition trigger #%d",lines[k],i+1);
          }
        }
        if(searchflags&MV )
        {
          if(!strnicmp(trigger.var1+6,searchdata.variable,32))
          {
            ret=1;
            log("Found %s in transition trigger #%d",trigger.var1,i+1);
          }
          if(!strnicmp(trigger.var2+6,searchdata.variable,32))
          {
            ret=1;
            log("Found %s in transition trigger #%d",trigger.var2,i+1);
          }
        }
        if(searchflags&MR)
        {
          if(!strnicmp(trigger.trobj.var,searchdata.resource,8))
          {
            ret=1;
            log("Found %s in transition trigger #%d",trigger.trobj.var,i+1);
          }
        }
      }
      if(lines)
      {
        delete [] lines;
        lines=NULL;
      }
    } //end for
  } //end if
  
  if(searchflags&(MR|MV|MF|MS) )
  {
    for(i=0;i<the_dialog.actioncount;i++)
    {
      lines=explode(the_dialog.actions[i],'\n',linecnt);
      for(k=0;k<linecnt;k++)
      {
        if(compile_action(lines[k],action,false)) continue;
        
        if(searchflags&MS)
        {
          trn=-1;
          switch(action.opcode)
          {
          case AC_REMOVE_JOURNAL_IWD:case AC_QUESTDONE_IWD:
            if(has_xpvar())
            {
              trn = action.bytes[0];
            }
            break;
          case AC_REMOVE_JOURNAL_BG: case AC_QUESTDONE_BG:
            if(!has_xpvar())
            {
              trn = action.bytes[0];
            }
            break;
          case AC_ADD_JOURNAL:
            //always a string
            trn = action.bytes[0];
            break;
          }
          if (searchdata.strref == trn)
          {
            ret=1;
            log("Found %s in action %d", lines[k], i+1);
          }
        }

        if(searchflags&MF)
        {
          if(action.opcode>=searchdata.feature &&
            action.opcode<=searchdata.feature2)
          {
            ret=1;
            log("Found %s in action %d",lines[k],i+1);
            which_transition(i);
          }            
        }
        
        if(searchflags&MV )
        {
          if(!strnicmp(action.var1+6,searchdata.variable,32))
          {
            ret=1;
            log("Found %s in action #%d",action.var1,i+1);
            which_transition(i);
          }
          if(!strnicmp(action.var2+6,searchdata.variable,32))
          {
            ret=1;
            log("Found %s in action #%d",action.var2,i+1);
            which_transition(i);
          }
        }
        if(searchflags&MR)
        {
          if(!strnicmp(action.var1,searchdata.resource,8))
          {
            ret=1;
            log("Found %s in action #%d",action.var1,i+1);
            which_transition(i);
          }
          if(!strnicmp(action.var2,searchdata.resource,8))
          {
            ret=1;
            log("Found %s in action #%d",action.var2,i+1);
            which_transition(i);
          }
          for(j=0;j<3;j++)
          {
            if(!strnicmp(action.obj[j].var,searchdata.resource,8))
            {
              ret=1;
              log("Found %s in action #%d ob #%d",action.obj[j].var,i+1, j+1);
              which_transition(i);
            }
          }
        }
      }
      if(lines)
      {
        delete [] lines;
        lines=NULL;
      }
    } //end for
  }  //end if
  return !!ret;
}

bool CChitemDlg::match_bam()
{
  if(searchflags&MT)
  {
    if(the_bam.m_header.chCycleCount<searchdata.itemtype || the_bam.m_header.chCycleCount>searchdata.itemtype2)
    {
      return false;
    }
  }
  
  if(searchflags&MF)
  {
    if(the_bam.m_header.wFrameCount<searchdata.feature || the_bam.m_header.wFrameCount>searchdata.feature2)
    {
      return false;
    }
  }
  
  log("Cycle count: %d",the_bam.m_header.chCycleCount);
  log("Frame count: %d",the_bam.m_header.wFrameCount);
  return true;
}

bool CChitemDlg::match_projectile()
{
  CString tmpstr;
  int found;
  search_data tmpdata;
  
  memset(&tmpdata,0,sizeof(tmpdata) );
  found=0;
  if(searchflags&MR)
  {
    memcpy(tmpdata.resource,the_projectile.header.bam1,8);    
    if(searchdata.resource[0])
    {
      if(!strnicmp(tmpdata.resource,searchdata.resource,8) )
      {
        found=1;
      }
    }
    if(!found)
    {
      memcpy(tmpdata.resource,the_projectile.header.bam2,8);    
      if(searchdata.resource[0])
      {
        if(!strnicmp(tmpdata.resource,searchdata.resource,8) )
        {
          found=2;
        }
      }
    }
    if(!found)
    {
      memcpy(tmpdata.resource,the_projectile.header.wavc1,8);    
      if(searchdata.resource[0])
      {
        if(!strnicmp(tmpdata.resource,searchdata.resource,8) )
        {
          found=3;
        }
      }
    }
    if(!found)
    {
      memcpy(tmpdata.resource,the_projectile.header.wavc2,8);    
      if(searchdata.resource[0])
      {
        if(!strnicmp(tmpdata.resource,searchdata.resource,8) )
        {
          found=4;
        }
      }
    }
  }
  else found=1;
  
  if(found)
  {
    if(searchflags&MT)
    {
      if(searchdata.itemtype&the_projectile.extension.aoe)
      {
        tmpdata.itemtype=(short) (searchdata.itemtype&the_projectile.extension.aoe);
      }
      else found=0;
    }
  }
  
  if(found)
  {
    if(searchflags&MP)
    { 
      if(searchdata.projectile<=the_projectile.extension.projectile &&
        searchdata.projectile2>=the_projectile.extension.projectile)
      {
        tmpdata.projectile=the_projectile.extension.projectile;
      }
      else found = 0;
    }
  }
  if(!found) return false;
  
  if(searchflags&MR)
  {
    if(found>2)
    {
      if(tmpdata.resource[0]) log("Found resource '%-.8s' in sound%d",tmpdata.resource, found-2);
    }
    else
    {
      if(tmpdata.resource[0]) log("Found resource '%-.8s' in bam%d",tmpdata.resource, found);
    }
  }
  
  if(searchflags&MT)
  {
    log("Found AOE flag: %0x", tmpdata.itemtype);
  }
  
  if(searchflags&MP)
  {
    tmpstr=get_projectile_id(tmpdata.projectile,0);
    log("Found projectile %d-%s",tmpdata.projectile,tmpstr);
  }
  // debug
  if(the_projectile.header.type==3 && the_projectile.extension.expgr2!=0xff) {
    log("Projectile with areapro.2da index!");
  }
  //
  return true;
}

bool CChitemDlg::match_store()
{
  CString tmpstr;
  bool found;
  search_data tmpdata;
  int itemnum;
  
  memset(&tmpdata,0,sizeof(tmpdata));
  found=false;
  if(searchflags&MS)
  {
    if(searchdata.strref!=the_store.header.strref)
    {
      return false;
    }
  }
  if(searchflags&MT)
  {
    for(itemnum=0;itemnum<the_store.itemtypenum;itemnum++)
    {
      tmpdata.itemtype=(short) the_store.itemtypes[itemnum];
      if(tmpdata.itemtype>=searchdata.itemtype &&
        tmpdata.itemtype<=searchdata.itemtype2)
      {
        found=true;
        break;
      }
    }
    if(!found) return false;
  }
  
  found=false;
  if(searchflags&MI)
  {
    for(itemnum=0;itemnum<the_store.entrynum;itemnum++)
    {
      memcpy(tmpdata.itemname,the_store.entries[itemnum].itemname,8);
      if(searchdata.itemname[0])
      {
        if(!strnicmp(tmpdata.itemname,searchdata.itemname,8) )
        {
          found=true;
          break;
        }
      }
      else
      {
        if(tmpdata.itemname[0])
        {
          found=true;
          break;
        }
      }
    }
    if(!found) return false;
  }
  
  if(searchflags&MR)
  {
    memcpy(tmpdata.resource,the_store.header.dlgresref1,8);
    if(!strnicmp(tmpdata.resource,searchdata.resource,8) )
    {
      goto done;
    }
    memcpy(tmpdata.resource,the_store.header.dlgresref2,8);
    if(!strnicmp(tmpdata.resource,searchdata.resource,8) )
    {
      goto done;
    }
    memcpy(tmpdata.resource,the_store.header.dlgresref3,8);
    if(!strnicmp(tmpdata.resource,searchdata.resource,8) )
    {
      goto done;
    }
    return false;
  }
done:
  
  if(searchflags&MS)
  {
    log("Using strref #%d",searchdata.strref);
  }
  if(searchflags&MT)
  {
    log("Allows %s",format_itemtype(tmpdata.itemtype));
  }
  
  if(searchflags&MI)
  {
    log("Sells %s",tmpdata.itemname);
  }
  if(searchflags&MR)
  {
    log("References dialog resource: %s",tmpdata.resource);
  }
  return found;
}

//check the whole item
int CChitemDlg::check_item()
{
  int ret;
  CString tmpref;
  
  ret=0;
  if(!(the_item.header.itmattr&ATTR_MOVABLE) && !(chkflg&DROPPABLE))
  {
    return 0; //don't check special objects
  }
  //checking data type constraints
  if(the_item.header.itemtype>=NUM_ITEMTYPE)
  {
    log("The itemtype is larger than %d, this program cannot handle it.",NUM_ITEMTYPE);
    the_item.header.itemtype=0; //hacking it to zero
  }
  if(bg2_weaprofs() )
  {
    itvsatype=itvsatype_bg[the_item.header.itemtype];
  }
  else
  {
    if(pst_compatible_var()) itvsatype=itvsatype_pst[the_item.header.itemtype];
    else
    {
      itvsatype=itvsatype_iwd[the_item.header.itemtype];
    }
  }
  
  //checking semantical constraints
  switch(check_reference(the_item.header.unidref, the_item.header.loreid>0?9:8) )
  {
  case 1:
    ret|=BAD_STRREF;
    log("Invalid unidentified name (reference: %d).",the_item.header.unidref);
    break;
  case 2:
    ret|=BAD_STRREF;
    log("Deleted unidentified name (reference: %d).",the_item.header.unidref);
    break;
  case 6:
    ret|=BAD_STRREF;
    log("Item name contains cr/lf (reference: %d).",the_item.header.unidref);
  }

  switch(check_reference(the_item.header.uniddesc,1,32,0) )
  {
  case 1:
    ret|=BAD_STRREF;
    log("Invalid unidentified description (reference: %d).",the_item.header.unidref);
    break;
  case 2:
    ret|=BAD_STRREF;
    log("Deleted unidentified description (reference: %d).",the_item.header.unidref);
    break;
  case 5:
    ret|=BAD_STRREF;
    log("Name length constraint violated (reference: %d).",the_item.header.unidref);
    break;
  }
  
  switch(check_reference(the_item.header.idref,9,2,32) )
  {
  case 1:
    ret|=BAD_STRREF;
    log("Invalid identified name (reference: %d).",the_item.header.idref);
    break;
  case 2:
    ret|=BAD_STRREF;
    log("Deleted identified name (reference: %d).",the_item.header.idref);
    break;
  case 5:
    ret|=BAD_STRREF;
    log("Name length constraint violated (reference: %d).",the_item.header.idref);
    break;
  case 6:
    ret|=BAD_STRREF;
    log("Item name contains cr/lf (reference: %d).",the_item.header.idref);
  }

  switch(check_reference(the_item.header.iddesc) )
  {
  case 1:
    ret|=BAD_STRREF;
    log("Invalid identified description (reference: %d).",the_item.header.idref);
    break;
  case 2:
    ret|=BAD_STRREF;
    log("Deleted identified description (reference: %d).",the_item.header.idref);
    break;
  }

  if(pst_compatible_var())
  {
    switch(check_soundref(the_item.header.destname,0) )
    {
    case -1:
      ret|=BAD_RESREF;
      log("Invalid sound reference: '%-.8s'",the_item.header.destname);
      break;
    }
  }
  else
  {
    switch(check_itemref(the_item.header.destname, 0) )
    {
    case -1:
      ret|=BAD_ITEMREF;
      log("Invalid item reference: '%-.8s'",the_item.header.destname);
      break;
    }
  }
  
  switch(check_resource(the_item.header.invicon,false) )
  {
  case -1:
    ret|=BAD_ICONREF;
    log("Invalid inventory icon reference: '%-.8s'",the_item.header.invicon);
    break;
  case -2:
    ret|=BAD_ICONREF;
    log("Empty inventory icon reference.");
    break;
  }
  
  switch(check_resource(the_item.header.grnicon,false) )
  {
  case -1:
    ret|=BAD_ICONREF;
    log("Invalid ground icon reference: '%-.8s'",the_item.header.grnicon);
    break;
  case -2:
    ret|=BAD_ICONREF;
    log("Empty ground icon reference.");
    break;
  }
  
  if(pst_compatible_var())
  {
    switch(check_soundref(the_item.header.descicon,0) )
    {
    case -1:
      ret|=BAD_RESREF;
      log("Invalid sound reference: '%-.8s'",the_item.header.descicon);
      break;
    case -2:
      ret|=BAD_RESREF;
      log("Invalid sound reference: '%-.8s'",the_item.header.descicon);
      break;
    }
  }
  else
  {
    if(!has_xpvar()) //unused in iwd/iwd2
    {
      switch(check_resource(the_item.header.descicon,false) )
      {
      case -1:
        ret|=BAD_ICONREF;
        log("Invalid description icon reference: '%-.8s'",the_item.header.descicon);
        break;
      case -2:
        ret|=BAD_ICONREF;
        log("Empty description icon reference.");
        break;
      }
    }
  }
  
  if (!(chkflg&NOOTHERCH))
  {
    if (the_item.header.weight)
    {
      if (!the_item.header.price)
      {
        log("This item has weight but no price!");
      }
    }
    
    if (the_item.header.price)
    {
      if (!the_item.header.weight)
      {
        log("This item has price but no weight!");
      }
    }
  }
  
  
  ret|=check_itemtype(the_item.header.itemtype);
  
  ret|=check_usability(~*(unsigned long *) the_item.header.usability);
  
  if(bg2_weaprofs())
  {
    ret|=check_weaprofs(the_item.header.itemtype); //same argument
  }
  
  ret|=check_attribute(the_item.header.itmattr, the_item.header.itemtype);
  
  ret|=check_anim(the_item.header.animtype, the_item.header.itemtype);
  
  ret|=check_mplus(the_item.header.mplus);
  
  ret|=check_statr();
  
  ret|=check_other();

  ret|=check_extheader(the_item.header.itemtype);
  
  unsigned long school = the_item.header.kits4 | (the_item.header.kits3 <<8);  
  ret|=check_item_effectblock(the_item.header.itemtype, school&0x7fc0);
  return ret;
}

int CChitemDlg::check_creature_spells()
{
  int memory;
  int i;
  
  memory=0;
  for(i=0;i<the_creature.header.selectcnt;i++)
  {
    memory+=the_creature.selects[i].count;
  }
  if(memory>the_creature.header.memcnt)
  {
    log("Invalid spell memorisation count. (counted %d vs. stated %d)", memory, the_creature.header.memcnt);
    return -1;
  }
  return 0;
}

int CChitemDlg::check_creature_features()
{
  int i, feat;
  int ret, gret;
  
  gret=0;
  for(i=0;i<the_creature.effectcount;i++)
  {
    if(the_creature.header.effbyte)
    {
      feat=the_creature.effects[i].feature;
      ret=check_feature(feat,the_creature.effects[i].par1.parl,the_creature.effects[i].par2.parl,
        the_creature.effects[i].resource);
    }
    else
    {
      feat=the_creature.oldeffects[i].feature;
      ret=check_feature(feat,the_creature.oldeffects[i].par1.parl,the_creature.oldeffects[i].par2.parl,
        the_creature.oldeffects[i].vvc);
    }
    if(ret) log("Embedded effect (%s) #%d",get_opcode_text(feat), i);
    gret|=ret;
  }
  return gret;
}

int CChitemDlg::check_chui()
{
  int ret, flg;
  int i;
  
  ret=0;
  for(i=0;i<the_chui.windowcnt;i++)
  {
    flg=the_chui.windows[i].flags&1;
    switch(check_mos_reference(the_chui.windows[i].mos,!flg))
    {
    case -1:
      log("Missing mos resource '%.8s' for window #%d",the_chui.windows[i].mos,i+1);
      ret|=BAD_ICONREF;
    case -2:
      if(flg)
      {
        log("Invalid mos resource '%.8s' for window #%d",the_chui.windows[i].mos,i+1);
      }
      else
      {
        log("Empty mos resource for window #%d",i+1);
      }
      ret|=BAD_ICONREF;
    }
  }
  return ret;
}

int CChitemDlg::check_creature_pst()
{
  CString tmpstr;
  
  RetrieveVariable(tmpstr, the_creature.pstheader.bestiary);
  return 0;
}

//flags that make the creature die as soon as it was put in the game (useless)
#define FLAGS_THAT_HURT  (0x800)

int CChitemDlg::check_creature()
{
  CString tmpstr;
  int i;
  int ret;
  int idsrace;
  
  ret=0;
  switch(check_reference(the_creature.header.shortname,1,2,40) )
  {
  case 1:
    ret|=BAD_STRREF;
    log("Invalid short name (reference: %d).",the_creature.header.shortname);
    break;
  case 2:
    ret|=BAD_STRREF;
    log("Deleted short name (reference: %d).",the_creature.header.shortname);
    break;
  case 5:
    ret|=BAD_STRREF;
    log("Name length constraint violated (reference: %d).",the_creature.header.shortname);
    break;
  }
  
  switch(check_reference(the_creature.header.longname,1,2,40) )
  {
  case 1:
    ret|=BAD_STRREF;
    log("Invalid long name (reference: %d).",the_creature.header.longname);
    break;
  case 2:
    ret|=BAD_STRREF;
    log("Deleted long name (reference: %d).",the_creature.header.longname);
    break;
  case 5:
    ret|=BAD_STRREF;
    log("Name length constraint violated (reference: %d).",the_creature.header.longname);
    break;
  }
  
  if(!(chkflg&NOATTRCH))
  {
    if(the_creature.header.state&FLAGS_THAT_HURT)
    {
      ret|=BAD_ATTR;
      log("Flags: %08x (creature is dead)",the_creature.header.state);
    }
  }
  
  for(i=0;i<100;i++)
  {
    switch(check_reference(the_creature.header.strrefs[i], 4) )
    {
    case 1:
      ret|=BAD_STRREF;
      log("Invalid string ref #%d (reference: %d).",i, the_creature.header.strrefs[i]);
      break;
    case 2:
      ret|=BAD_STRREF;
      log("Deleted string ref #%d (reference: %d).",i, the_creature.header.strrefs[i]);
      break;
    case 4:
      //biography doesn't need sound
      if (i!=74) {
        ret|=BAD_RESREF;
        tmpstr = resolve_tlk_soundref(the_creature.header.strrefs[i], 0);
        log("Soundset entry #%d (reference: %d) misses sound resource (%s).", i, the_creature.header.strrefs[i], tmpstr);
      }
    }    
  }
  
  idsrace=the_creature.header.idsrace;
  if(!(chkflg&NOATTRCH))
  {
    if(idsrace!=255)
    {
      if(IDSToken("RACE",idsrace, true).IsEmpty())
      {
        ret|=BAD_ATTR;
        log("Value %d isn't listed in race.ids",idsrace);
      }
      
      if(IDSToken("EA",the_creature.header.idsea, true).IsEmpty())
      {
        ret|=BAD_ATTR;
        log("Value %d isn't listed in ea.ids",the_creature.header.idsea);
      }
      if(IDSToken("GENERAL",the_creature.header.idsgeneral, true).IsEmpty())
      {
        ret|=BAD_ATTR;
        log("Value %d isn't listed in general.ids",the_creature.header.idsgeneral);
      }
      if(IDSToken("CLASS",the_creature.header.idsclass, true).IsEmpty())
      {
        ret|=BAD_ATTR;
        log("Value %d isn't listed in class.ids",the_creature.header.idsclass);
      }
      if(IDSToken("GENDER",the_creature.header.idsgender, true).IsEmpty())
      {
        ret|=BAD_ATTR;
        log("Value %d isn't listed in gender.ids",the_creature.header.idsgender);
      }
      
      if(check_alignment(the_creature.header.idsalign) )  //can't use align.ids because of the alignment masks
      {
        ret|=BAD_ATTR;
        log("Invalid alignment (0x%02x).", the_creature.header.idsalign);
      }
      
      if(IDSToken("SPECIFIC",the_creature.header.idsspecific, true).IsEmpty())
      {
        ret|=BAD_ATTR;
        log("Value %d isn't listed in specific.ids",the_creature.header.idsspecific);
      }
      idsrace=the_creature.header.enemy;
      if(idsrace && (idsrace!=255) ) //lets accept 255 and 0 too
      {
        if(IDSToken("RACE",idsrace, true).IsEmpty())
        {
          ret|=BAD_ATTR;
          log("Value %d for racial enemy isn't listed in race.ids",idsrace);
        }
      }
      else
      {
        if((the_creature.header.idsclass==CLASS_RANGER) || (the_creature.header.idsclass==CLERIC_RANGER) ) //ranger
        {
          ret|=BAD_ATTR;
          log("Ranger without racial enemy!");
        }
      }
    }
  }
  if(bg2_weaprofs()) //present only in soa/tob/dltc
  {
    idsrace=the_creature.header.kit>>16;
    if(idsrace)
    {
      if(IDSToken("KIT", idsrace, false).IsEmpty() )
      {
        ret|=BAD_ATTR;
        log("Value 0x%0x for kit isn't listed in kit.ids", idsrace);
      }
    }
  }
  
  if(pst_compatible_var())
  {
    switch(check_resource(the_creature.header.iconl, true) )
    {
    case -1:
      ret|=BAD_ICONREF;
      log("Invalid large portrait reference: '%-.8s'",the_creature.header.iconl);
      break;
    }
    
    switch(check_resource(the_creature.header.iconm, true) )
    {
    case -1:
      ret|=BAD_ICONREF;
      log("Invalid mini portrait reference: '%-.8s'",the_creature.header.iconm);
      break;
    }
  }
  else
  {
    switch(check_bitmap(the_creature.header.iconl, true) )
    {
    case -1:
      ret|=BAD_ICONREF;
      log("Invalid large portrait reference: '%-.8s'",the_creature.header.iconl);
      break;
    }
    
    switch(check_bitmap(the_creature.header.iconm, true) )
    {
    case -1:
      ret|=BAD_ICONREF;
      log("Invalid mini portrait reference: '%-.8s'",the_creature.header.iconm);
      break;
    }
  }
  
  if(!(chkflg&NOOTHERCH))
  {
    if(the_creature.header.curhp>the_creature.header.maxhp)
    {
      ret|=BAD_ATTR;
      log("Current HP > Max HP");
    }
    if((the_creature.header.curhp<1) || (the_creature.header.maxhp<1) )
    {
      ret|=BAD_ATTR;
      log("Current or Max HP<1");
    }
  }
  
  if(!(chkflg&NOANIMCHK))
  {
    idsrace=the_creature.header.animid;
    if(IDSToken("ANIMATE",idsrace, false).IsEmpty())
    {
      ret|=BAD_ANIM;
      log("Animation ID 0x%4x cannot be found in animation.ids!",idsrace);
    }
  }
  
  for(i=0;i<5;i++)
  {
    switch(check_scriptref(the_creature.header.scripts[i],true) )
    {
    case -1:
      ret|=BAD_RESREF;
      log("Bad %s script reference: '%-.8s' !",get_script_type(i), the_creature.header.scripts[i] );
    }
  }
  
  switch(check_dialogres(the_creature.header.dialogresref,true) )
  {
  case -1:
    ret|=BAD_RESREF;
    log("Bad dialog reference '%-.8s' !", the_creature.header.dialogresref);
  }
  
  if(!(chkflg&NODVARCH))
  {
    RetrieveVariable(tmpstr,the_creature.header.dvar);
    if(tmpstr.IsEmpty())
    {
      if(!(chkflg&WARNINGS))
      {
        ret|=BAD_ATTR;
        log("Empty death variable.");
      }
    }
    if(!pst_compatible_var())
    {
      if(tmpstr.GetLength()>18)
      {
        ret|=BAD_ATTR;
        log("Too long death variable (>18)!");
      }
    }
  }
  ret|=check_weaponslots();
  
  if(pst_compatible_var())
  {
    ret|=check_creature_pst();
  }
  if(chkflg&NOEXTCHK) return ret;
  ret|=check_creature_spells();
  ret|=check_creature_features();
  return ret;
}

static bool check_twohanded(const char *poi)
{
  CString resref;
  
  RetrieveResref(resref, poi);
  the_item.header.itmattr=0;
  if(read_item(resref)<0) return false; //actually this is an invalid item
  return (the_item.header.itmattr&ATTR_TWOHAND)!=0;
}

int CChitemDlg::check_weaponslots()
{
  int ret;
  int i,j;
  
  if(chkflg&NOSLOTCH) return 0;
  
  ret=0;
  for(i=0;i<the_creature.itemcount;i++)
  {
    for(j=0;j<the_creature.slotcount;j++)
    {
      if(the_creature.itemslots[j]==i) break;
    }
    if(j==the_creature.slotcount)
    {
      log("Dangling item on creature: '%.8s'",the_creature.items[i].itemname);
      ret|=BAD_ITEMREF;
    }
    else
    {
      if(check_itemref(the_creature.items[i].itemname,0) )
      {
        log("Invalid item on creature: '%.8s'",the_creature.items[i].itemname);
        ret|=BAD_ITEMREF;
      }
    }
  }
  if(the_creature.itemslots[SLOT_SHIELD]!=-1)
  {
    for(int k=0;k<4;k++)
    {
      j=the_creature.itemslots[SLOT_WEAPON+k];
      if(check_twohanded(the_creature.items[j].itemname))
      {
        log("Two handed weapon (%.8s) equipped with shield!",the_creature.items[j].itemname);
      }
    }
  }
  
  i=*(short *) (the_creature.itemslots+the_creature.slotcount);
  switch(i)
  {
  case -22: case -23: case -24:
    //quiver
    break;
  case 0: case 1: case 2: case 3:
    j = the_creature.itemslots[SLOT_WEAPON+i];
    if(j==-1)
    {
      if(i!=0)
      {
        ret|=BAD_ATTR;
        log("Invalid equipped weapon index:%s, no weapon equipped there",get_slottype(i));
      }
    }
    break;
  case 1000:
    if(the_creature.itemslots[SLOT_WEAPON]!=-1)
    {
      ret|=BAD_ATTR;
      log("Creature has weapon, but doesn't wield it!");
    }
    break;
  default:
    ret|=BAD_ATTR;
    log("Invalid equipped weapon index:%s (should be 0-3 or 1000).",get_slottype(i));
    break;
  }
  return ret;
}

//special spell, applied only
int CChitemDlg::check_applyspell()
{
  int fbc;
  int ret;
  int equip_or_use;
  int ext;
  int actfbc, oldfbc;
  CString what;
  int warn;

  ret=0;
  ext=0;
  warn = 1;
  actfbc=the_spell.header.featblkcount;
  for(fbc=0;fbc<the_spell.featblkcount;fbc++)
  {
    equip_or_use=fbc<the_spell.header.featblkcount;
    if(equip_or_use)
    {
      if (warn)
      {
        log("Casting effect in applied spell!");
        warn = 0;
      }
    }
    else
    {
      while(actfbc<=fbc)
      {
        if(ext>=the_spell.extheadcount)
        {
          log("Mixed up extension headers.");
          return -1;
        }
        else
        {
          oldfbc=actfbc;
          actfbc+=the_spell.extheaders[ext].fbcount;
          ext++;
        }
      }
      what.Format("Extended header #%d/%d",ext,1+fbc-oldfbc); //this must be more precise
    }
    if(check_feature(the_spell.featblocks[fbc].feature,
      the_spell.featblocks[fbc].par1.parl,the_spell.featblocks[fbc].par2.parl,
      the_spell.featblocks[fbc].vvc) )
    {
      ret|=BAD_EXTHEAD;
      log("Invalid effect (%s) in %s",get_opcode_text(the_spell.featblocks[fbc].feature),what);      
      return -1;
    }
    if(check_probability(the_spell.featblocks[fbc].prob1,the_spell.featblocks[fbc].prob2))
    {
      ret|=BAD_EXTHEAD;
      log("Invalid probability in %s",what);
    }
    if(equip_or_use)
    {
      if(the_spell.featblocks[fbc].target!=EFF_TARGET_SELF)
      {
        ret|=BAD_EXTHEAD;
        log("Target type in %s must be Self",what);
      }
    }
    else
    {
      if(the_spell.featblocks[fbc].timing!=TIMING_PERMDEATH)
      {
        ret|=BAD_EXTHEAD;
        log("Not a really permanent effect in %s?",what);
      }
    }
  }
  return ret;
}

int CChitemDlg::check_spell()
{
  int spelltype;
  int ret;
  CString name, desc;
  int nameref, descref;
  
#if _DEBUG
  /*
  if(the_spell.header.unkattr2)
  {
    log("spell attributes: 0x%x",the_spell.header.unkattr2);
  }
  */
  /*
  if(the_spell.header.itmattr)
  {
  log("item attributes: 0x%x",the_spell.header.itmattr);
  }
  */
  /*
  if(the_spell.header.unknown4c)
  {
  log("Uses 0x4c");
  }
  if(the_spell.header.splattr&0x40)
  {
  log("simplified duration");
  }
  */
#endif
  ret=0;
  nameref = the_spell.header.spellname;

  switch(check_reference(nameref,1,2,32) )
  {
  case 1:
    ret|=BAD_STRREF;
    log("Invalid name (reference: %d).",the_spell.header.spellname);
    break;
  case 2:
    ret|=BAD_STRREF;
    log("Deleted name (reference: %d).",the_spell.header.spellname);
    break;
  case 5:
    ret|=BAD_STRREF;
    log("Name length constraint violated (reference: %d).",the_spell.header.spellname);
    break;
  }
  
  spelltype=the_spell.header.spelltype;
  if(spelltype==SP_CLERIC || spelltype==SP_WIZARD)
  {
    descref = the_spell.header.desc;
    switch(check_reference(descref,1,32,0) )
    {
    case 1:
      ret|=BAD_STRREF;
      log("Invalid description (reference: %d).",the_spell.header.desc);
      break;
    case 2:
      ret|=BAD_STRREF;
      log("Deleted description (reference: %d).",the_spell.header.desc);
      break;
    case 5:
      ret|=BAD_STRREF;
      log("Description length constraint violated (reference: %d).",the_spell.header.desc);
      break;
    }

    if (!ret && (nameref>0) && (descref>0) && !(chkflg&NOREFCHK))
    {
      name = resolve_tlk_text(nameref);
      desc = resolve_tlk_text(descref);
      if (desc.Left(name.GetLength())!=name)
      {
        log("Description (%d) doesn't start with the name (#%d/%s)", descref, nameref, name);
      }
    }

  }

  ret|=check_spl_extheader();
  
  ret|=check_spell_effectblock();
  return ret;
}

int CChitemDlg::check_effect()
{
  int ret;
  
  ret=0;
  if(check_feature(the_effect.header.feature,
    the_effect.header.par1.parl,the_effect.header.par2.parl,
    the_effect.header.resource) )
  {
    ret|=BAD_EXTHEAD;
    log("Invalid effect (%s)",get_opcode_text(the_effect.header.feature));
  }
  if(check_probability(the_effect.header.prob1,the_effect.header.prob2))
  {
    ret|=BAD_EXTHEAD;
    log("Feature probability is invalid.");
  }
  if(chkflg&NODUR) return ret;
  //the further checks are very optional
  if(member_array(the_effect.header.timing,has_duration)==-1)
  {
    if(the_effect.header.duration)
    {
      ret|=BAD_EXTHEAD;
      log("Useless duration.");
    }
  }
  else
  {
    if(!the_effect.header.duration)
    {
      ret|=BAD_EXTHEAD;
      log("No duration set.");
    }
  }
  
  return ret;
}

int CChitemDlg::check_videocell()
{
  int ret;
  
#ifdef _DEBUG
  
  if(the_videocell.header.colouring&1)
  {
    log("darken");
  }
  
  if(the_videocell.header.sequencing&2)
  {
    log("lightspot!");
  }
#endif  
  ret=0;
  if((the_videocell.header.trans2&1) && (the_videocell.header.transparency&128) )
  {
    ret|=BAD_ATTR;
    log("0x1 in trans2 and 0x80 in transparancy causes assertion failure!");
  }
  if(the_videocell.header.transparency&4)
  {
    if(!(the_videocell.header.shadow[0]) )
    {
      ret|=BAD_ATTR;
      log("0x4 in transparency causes crash if shadow isn't set!");
    }
  }
  if((the_videocell.header.transparency&10)==10)
  {
    ret|=BAD_ATTR;
    log("Transparency+brightest flag causes assertion failure!");
  }
  
  if(the_videocell.header.sequencing&8)
  {
    switch(check_resource(the_videocell.header.bam,false) )
    {
    case -1:
      ret|=BAD_ICONREF;
      log("Invalid bam reference: '%-.8s' in videocell.",the_videocell.header.bam);
      break;
    case -2:
      ret|=BAD_ICONREF;
      log("Empty bam reference in videocell.");
      break;
    }
  }
  else
  {
    if(the_videocell.header.bam[0])
    {
      ret|=BAD_ATTR;
      log("BAM is disabled!");
    }
  }
  
  switch(check_soundref(the_videocell.header.sound1, 0) )
  {
  case -1:
    ret|=BAD_RESREF;
    log("Invalid sound reference: '%-.8s' in videocell.",the_videocell.header.sound1);
  }
  switch(check_soundref(the_videocell.header.sound2, 0) )
  {
  case -1:
    ret|=BAD_RESREF;
    log("Invalid sound reference: '%-.8s' in videocell.",the_videocell.header.sound2);
  }
  return ret;
}

int CChitemDlg::check_area_actors()
{
  int i;
  int ret;
  unsigned int tmp;
  CString actorname;
  unsigned int maxx,maxy;
  unsigned int px,py;
  
  ret=0;
  maxx=the_area.m_width;
  maxy=the_area.m_height;
  for(i=0;i<the_area.header.actorcnt;i++)
  {
    RetrieveVariable(actorname,the_area.actorheaders[i].actorname);
    px=the_area.actorheaders[i].posx;
    py=the_area.actorheaders[i].posy;
    if(px>=maxx || py>=maxy)
    {
      ret|=BAD_ATTR;
      log("Actor #%d (%-.32s [%d.%d]) is out of map.",
        i+1,actorname,px,py);
    }
    if(!the_area.actorheaders[i].schedule)
    {
      ret|=BAD_ATTR;
      log("Actor #%d (%-.32s [%d.%d]) has no schedule.",
        i+1,actorname,px,py);
    }
    if(check_creature_reference(the_area.actorheaders[i].creresref, true))
    {
      ret|=BAD_RESREF;
      log("Bad creature reference '%-.8s' in actor header #%d (%-.32s [%d.%d])",
        the_area.actorheaders[i].creresref,i+1,actorname,px,py);
    }
    tmp=the_area.actorheaders[i].face;
    if(tmp>15)
    {
      ret|=BAD_ATTR;
      log("Bad direction (%x) for '%-.8s' in actor header #%d (%-.32s [%d.%d])",
        tmp,the_area.actorheaders[i].creresref,i+1,actorname,px,py);
    }
    
    if(!(chkflg&NOANIMCHK))
    {
      tmp=the_area.actorheaders[i].animation;
      if(tmp && IDSToken("ANIMATE",tmp, false).IsEmpty())
      {
        ret|=BAD_ANIM;
        log("Invalid animation (%04x) in actor header #%d (%-.32s [%d.%d]) may cause a crash!",
          tmp,i+1,actorname,px,py);
      }
    }
    switch(check_dialogres(the_area.actorheaders[i].dialog,true))
    {
    case -1:
      ret|=BAD_RESREF;
      log("Bad dialog reference '%-.8s' in actor header #%d (%-.32s [%d.%d])",
        the_area.actorheaders[i].dialog,i+1,actorname,px,py);
    }
    switch(check_scriptref(the_area.actorheaders[i].scrclass, true))
    {
    case -1:
      ret|=BAD_RESREF;
      log("Bad class script reference '%-.8s' in actor header #%d (%-.32s [%d.%d])",
        the_area.actorheaders[i].scrclass,i+1,actorname,px,py);
    }
    switch(check_scriptref(the_area.actorheaders[i].scrdefault, true))
    {
    case -1:
      ret|=BAD_RESREF;
      log("Bad default script reference '%-.8s' in actor header #%d (%-.32s [%d.%d])",
        the_area.actorheaders[i].scrdefault,i+1,actorname,px,py);
    }
    switch(check_scriptref(the_area.actorheaders[i].scrgeneral, true))
    {
    case -1:
      ret|=BAD_RESREF;
      log("Bad general script reference '%-.8s' in actor header #%d (%-.32s [%d.%d])",
        the_area.actorheaders[i].scrgeneral,i+1,actorname,px,py);
    }
    switch(check_scriptref(the_area.actorheaders[i].scroverride, true))
    {
    case -1:
      ret|=BAD_RESREF;
      log("Bad override script reference '%-.8s' in actor header #%d (%-.32s [%d.%d])",
        the_area.actorheaders[i].scroverride,i+1,actorname,px,py);
    }
    switch(check_scriptref(the_area.actorheaders[i].scrrace, true))
    {
    case -1:
      ret|=BAD_RESREF;
      log("Bad race script reference '%-.8s' in actor header #%d (%-.32s [%d.%d])",
        the_area.actorheaders[i].scrrace,i+1,actorname,px,py);
    }
    switch(check_scriptref(the_area.actorheaders[i].scrspecific, true))
    {
    case -1:
      ret|=BAD_RESREF;
      log("Bad specific script reference '%-.8s' in actor header #%d (%-.32s [%d.%d])",
        the_area.actorheaders[i].scrspecific,i+1,actorname,px,py);
    }
  }
  return ret;
}

int CChitemDlg::check_ambient_overlap(int exclude, CString name, int x, int y, int radius)
{
  int cnt;
  int px, py, rad;
  int i;
  int distance;
  CString tmpref;

  cnt = 0;
  for(i=0;i<the_area.ambientcount;i++)
  {
    if (!(the_area.ambientheaders[i].flags&1)) continue;
    if (i==exclude) continue;
    RetrieveResref(tmpref, the_area.ambientheaders[i].ambients[0]);
    if (tmpref!=name) continue;
    rad = the_area.ambientheaders[i].radius;
    px=the_area.ambientheaders[i].posx;
    py=the_area.ambientheaders[i].posy;
    distance = (px-x)*(px-x)+(py-y)*(py-y)-(rad+radius)*(rad+radius);
    if (distance<0)
    {
      cnt++;
    }
  }
  return cnt;
}

int CChitemDlg::check_area_ambient()
{
  int i,j;
  int ret;
  int cnt;
  int loc;
  CString tmpref;
  CString ambiname;
  int px,py,rad;

  ret=0;
  for(i=0;i<the_area.header.ambicnt;i++)
  {
    RetrieveVariable(ambiname,the_area.ambientheaders[i].ambiname);
    px=the_area.ambientheaders[i].posx;
    py=the_area.ambientheaders[i].posy;
    cnt=the_area.ambientheaders[i].ambientnum;

    for(j=0;j<10;j++)
    {
      RetrieveResref(tmpref, the_area.ambientheaders[i].ambients[j]);
      switch(check_soundref(tmpref,j>=cnt))
      {
      case -1:
        ret|=BAD_RESREF;
        if(tmpref.IsEmpty()) tmpref="<empty>";
        log("Missing ambient entry #%d in ambient #%d (%-.32s [%d.%d])",
          j+1,i+1,ambiname,px,py);
        log("Ambient reference is '%s', ambient counter is %d",
          tmpref, cnt,px,py);
        break;
      case -2:
        ret|=BAD_RESREF;
        log("Useless ambient entry #%d in ambient #%d (%-.32s [%d.%d])",
          j+1,i+1,ambiname,px,py);
        log("Ambient reference is '%s', ambient counter is %d",tmpref, cnt);
        break;
      }
    }

    if (chkflg&NOATTRCH) continue;

    if(!the_area.ambientheaders[i].schedule)
    {
      ret|=BAD_ATTR;
      log("Ambient #%d (%-.32s [%d.%d]) has no schedule!",
        i+1,ambiname,px,py);
    }
    if(!the_area.ambientheaders[i].volume)
    {
      ret|=BAD_ATTR;
      log("Ambient #%d (%-.32s [%d.%d]) has zero volume!",
        i+1,ambiname,px,py);
    }
    if(!cnt || cnt>10)
    {
      ret|=BAD_ATTR;
      log("Invalid ambient counter (%d) in ambient #%d (%-.32s [%d.%d])",
        cnt,i+1,ambiname,px,py);
    }
    if (the_area.ambientheaders[i].ambientnum==1 && (the_area.ambientheaders[i].flags&1))
    {
      RetrieveResref(tmpref, the_area.ambientheaders[i].ambients[0]);
      rad = the_area.ambientheaders[i].radius;
      loc = check_ambient_overlap(i, tmpref, px, py, rad);
      if (loc>=5)
      {
        log("Ambient (#%d) %s overlaps %d times with the same resource (%s).", i+1, ambiname, loc, tmpref);
        ret|=BAD_ATTR;
      }
    }
  }
  return ret;
}

int CChitemDlg::check_area_anim()
{
  int i;
  int ret;
  CString animname;
  unsigned int maxx,maxy;
  unsigned int px,py;
  
  ret=0;
  maxx=the_area.m_width;
  maxy=the_area.m_height;
  for(i=0;i<the_area.header.animationcnt;i++)
  {
    if (tob_specific() && !(the_area.animheaders[i].flags&AA_COMBAT) )
    {
      log("Animation is turned off in combat.");
    }

    RetrieveVariable(animname,the_area.animheaders[i].animname);
    px=the_area.animheaders[i].posx;
    py=the_area.animheaders[i].posy;
    if(px>=maxx || py>=maxy)
    {
      ret|=BAD_ATTR;
      log("Animation #%d (%-.32s [%d.%d]) is out of map.",
        i+1,animname,px,py);
    }
    if(!the_area.animheaders[i].schedule)
    {
      ret|=BAD_ATTR;
      log("Animation #%d (%-.32s [%d.%d]) has no schedule!",
        i+1,animname,px,py);
    }
    switch(check_resource(the_area.animheaders[i].bam,false) )
    {
    case -1:
      ret|=BAD_ICONREF;
      log("Invalid animation reference: '%-.8s' in animation #%d (%-.32s [%d.%d])",
        the_area.animheaders[i].bam,i+1,animname,px,py);
      break;
    case -2:
      ret|=BAD_ICONREF;
      log("Empty animation reference in animation #%d (%-.32s [%d.%d])",
        i+1,animname,px,py);
      break;
    }
  }
  return ret;
}

#define CONT_PILE  4

int CChitemDlg::check_area_container()
{
  int i;
  int ret;
  int first, last;
  CString contname;
  unsigned int maxx,maxy;
  unsigned int px,py;
  unsigned int b1x,b1y;
  unsigned int b2x,b2y;
  
  ret=0;
  maxx=the_area.m_width;
  maxy=the_area.m_height;
  for(i=0;i<the_area.header.containercnt;i++)
  {
    RetrieveVariable(contname,the_area.containerheaders[i].containername);
    px=the_area.containerheaders[i].posx;
    py=the_area.containerheaders[i].posy;
    if(!(ret&BAD_VERTEX) )
    {
      if(px>=maxx || py>=maxy)
      {
        ret|=BAD_VERTEX;
        log("Container #%d (%-.32s [%d.%d]) is out of map.",
          i+1,contname,px,py);
      }
      b1x=the_area.containerheaders[i].p1x;
      b1y=the_area.containerheaders[i].p1y;
      b2x=the_area.containerheaders[i].p2x;
      b2y=the_area.containerheaders[i].p2y;
      if (the_area.containerheaders[i].type==CONT_PILE)
      {
        if (b1x!=0xffffffff && b1y!=0xffffffff)
        {
          ret|=BAD_VERTEX;
          log("Container #%d (%-.32s [%d.%d]) is a pile with bounding box.",
            i+1,contname,px,py);
        }
      }
      else
      {
        if(((b1x!=-1) || (b1y!=-1)) && ( (b1x>=b2x) || (b1y>=b2y)))
        {
          ret|=BAD_VERTEX;
          log("Container #%d (%-.32s [%d.%d]) has invalid bounding box, possibly damaged polygon.",
            i+1,contname,px,py);
        }
        if((b1x+800<b2x) || (b1y+600<b2y))
        {
          ret|=BAD_VERTEX;
          log("Container #%d (%-.32s [%d.%d]) has too large bounding box, possibly damaged polygon.",
            i+1,contname,px,py);
        }
        b1x=(b1x+b2x)/2;
        b1y=(b1y+b2y)/2;
        if((px+800<b1x) || (py+600<b1y))
        {
          ret|=BAD_VERTEX;
          log("Container #%d (%-.32s [%d.%d]) has its opening location too far away.",
            i+1,contname,px,py);
        }
        if(the_area.containerheaders[i].triggerrange)
        {
          ret|=BAD_VERTEX;
          log("Container #%d has trigger range (%-.32s [%d.%d])",
            i+1,contname,px,py);
        }
      }
    }
    first=the_area.containerheaders[i].firstitem;
    last=first+the_area.containerheaders[i].itemcount;
    if(last>the_area.header.itemcnt)
    {
      ret|=BAD_ATTR;
      log("Bad subscript for item array in container #%d (%-.32s [%d.%d])",
        i+1,contname,px,py);
    }
    switch(check_scriptref(the_area.containerheaders[i].trapscript,false))
    {
    case -1:
      ret|=BAD_RESREF;
      log("Non existent script (%-.8s) for container #%d (%-.32s [%d.%d])",
        the_area.containerheaders[i].trapscript,i+1,contname,px,py);
    }
    switch(check_itemref(the_area.containerheaders[i].keyitem,0))
    {
    case -1:
      ret|=BAD_ITEMREF;
      log("Non existent key item (%-.8s) for container #%d (%-.32s [%d.%d])",
        the_area.containerheaders[i].keyitem,i+1,contname,px,py);
    }
  }
  return ret;
}

int CChitemDlg::check_trigger_overlap(CString name)
{
  int i;
  CString infoname;

  for(i=0;i<the_area.header.infocnt;i++)
  {
    RetrieveVariable(infoname,the_area.triggerheaders[i].infoname);
    if (infoname==name) return i;
  }
  return 0;
}

int CChitemDlg::check_area_door()
{
  int i;
  int ret;
  CString doorname;
  unsigned int maxx,maxy;
  unsigned int p1x,p1y;
  unsigned int p2x,p2y;
  unsigned int b1x,b1y;
  unsigned int b2x,b2y;
  int strref;
  int region;
  
  ret=0;
  maxx=the_area.m_width;
  maxy=the_area.m_height;
  if(the_area.WedAvailable())
  {
    if(the_area.wedheader.doorcnt!=the_area.header.doorcnt)
    {
      log("The door counts in the area and wed files are different!");
      ret=BAD_EXTHEAD;
    }
  }
  for(i=0;i<the_area.header.doorcnt;i++)
  {
    if (!(chkflg&NOATTRCH))
    {
      RetrieveVariable(doorname,the_area.doorheaders[i].doorname);
      region = check_trigger_overlap(doorname);
      if (region)
      {
        log("Door #%d (%-.32s) collides with region (#%d) of same name.", i+1, doorname, region);
        ret=BAD_VAR;
      }
    }
    
    p1x=the_area.doorheaders[i].locp1x; //location p1
    p1y=the_area.doorheaders[i].locp1y;
    p2x=the_area.doorheaders[i].locp2x; //location p2
    p2y=the_area.doorheaders[i].locp2y;
    if(!(ret&BAD_VERTEX) )
    {
      if(p1x>=maxx || p1y>=maxy)
      {
        ret|=BAD_VERTEX;
        log("Door #%d (%-.32s [%d.%d]) open location is out of map.",
          i+1,doorname,p1x,p1y);
      }
      if(p2x>=maxx || p2y>=maxy)
      {
        ret|=BAD_VERTEX;
        log("Door #%d (%-.32s [%d.%d]) open location is out of map.",
          i+1,doorname,p2x,p2y);
      }
      b1x=the_area.doorheaders[i].op1x; //open door, bounding rect
      b1y=the_area.doorheaders[i].op1y;
      b2x=the_area.doorheaders[i].op2x; 
      b2y=the_area.doorheaders[i].op2y;
      if(b1x!=-1 || b1y!=-1 || b2x!=-1 || b2y!=-1)
      {
        if(((b1x!=-1) || (b1y!=-1)) && ( (b1x>=b2x) || (b1y>=b2y)))
        {
          ret|=BAD_VERTEX;
          log("Door #%d (%-.32s [%d.%d]) has invalid open bounding box, possibly damaged polygon.",
            i+1,doorname,p1x,p1y);
        }
        if((b1x+800<b2x) || (b1y+600<b2y))
        {
          ret|=BAD_VERTEX;
          log("Door #%d (%-.32s [%d.%d]) has too large open bounding box, possibly damaged polygon.",
            i+1,doorname,p1x,p1y);
        }
      }
      
      b1x=the_area.doorheaders[i].cp1x; //closed door, bounding rect
      b1y=the_area.doorheaders[i].cp1y;
      b2x=the_area.doorheaders[i].cp2x; 
      b2y=the_area.doorheaders[i].cp2y;
      if(b1x!=-1 || b1y!=-1 || b2x!=-1 || b2y!=-1)
      {
        if(((b1x!=-1) || (b1y!=-1)) && ( (b1x>=b2x) || (b1y>=b2y)))
        {
          ret|=BAD_VERTEX;
          log("Door #%d (%-.32s [%d.%d]) has invalid closed bounding box, possibly damaged polygon.",
            i+1,doorname,p1x,p1y);
        }
        if((b1x+800<b2x) || (b1y+600<b2y))
        {
          ret|=BAD_VERTEX;
          log("Door #%d (%-.32s [%d.%d]) has too large closed bounding box, possibly damaged polygon.",
            i+1,doorname,p1x,p1y);
        }
      }
      
    }
    strref=the_area.doorheaders[i].strref;
    switch(check_reference(strref) )
    {
    case 1:
      ret|=BAD_STRREF;
      log("Invalid opening message (%d) in door #%d (%-.32s [%d.%d])",
        strref,i+1,doorname,p1x,p1y);
      break;
    case 2:
      ret|=BAD_STRREF;
      log("Deleted opening message (%d) in door #%d (%-.32s [%d.%d])",
        strref,i+1,doorname,p1x,p1y);
      break;
    }
    /* no dialogs in doors
    switch(check_dialogres(the_area.doorheaders[i].dialogref,false) )
    {
    case -1:
    ret|=BAD_RESREF;
    log("Non existent dialog (%-.8s) for door #%d (%-.32s [%d.%d])",
    the_area.doorheaders[i].dialogref,i+1,doorname,p1x,p1y);
    }
    */
    
    switch(check_triggerref(the_area.doorheaders[i].regionlink,the_area.doorheaders[i].flags&1))
    {
    case -4:
      ret|=BAD_RESREF;
      log("Non existent trigger %-.16s referenced by door #%d (%-.32s [%d.%d])",
        the_area.doorheaders[i].regionlink,i+1,doorname,p1x,p1y);
      break;
    case -3:
      ret|=BAD_RESREF;
      log("Referenced trigger %-.16s isn't a travel trigger and its blocked flag is incorrect, door #%d (%-.32s [%d.%d])",
        the_area.doorheaders[i].regionlink,i+1,doorname,p1x,p1y);
      break;
    case -2:
      ret|=BAD_RESREF;
      log("Referenced trigger %-.16s isn't a travel trigger, door #%d (%-.32s [%d.%d])",
        the_area.doorheaders[i].regionlink,i+1,doorname,p1x,p1y);
      break;
    case -1:
      ret|=BAD_RESREF;
      log("Referenced trigger %-.16s has invalid blocked flag, door #%d (%-.32s [%d.%d])",
        the_area.doorheaders[i].regionlink,i+1,doorname,p1x,p1y);
      break;
    }
    
    switch(check_scriptref(the_area.doorheaders[i].openscript,false) )
    {
    case -1:
      ret|=BAD_RESREF;
      log("Non existent open script (%-.8s) for door #%d (%-.32s [%d.%d])",
        the_area.doorheaders[i].openscript,i+1,doorname,p1x,p1y);
    }
    if(!(ret&BAD_VERTEX))
    {
      if(the_area.doorheaders[i].firstvertexopen+the_area.doorheaders[i].countvertexopen>the_area.header.vertexcnt)
      {
        ret|=BAD_VERTEX;
        log("Invalid vertex entries for opened door #%d (%-.32s [%d.%d])",
          i+1,doorname,p1x,p1y);
      }
      if(the_area.doorheaders[i].firstvertexclose+the_area.doorheaders[i].countvertexclose>the_area.header.vertexcnt)
      {
        ret|=BAD_VERTEX;
        log("Invalid vertex entries for closed door #%d (%-.32s [%d.%d])",
          i+1,doorname,p1x,p1y);
      }
    }
  }
  return ret;
}

int CChitemDlg::check_area_entrance()
{
  int i;
  int ret;
  CString entrancename;
  unsigned int maxx,maxy;
  unsigned int px,py;
  
  ret=0;
  maxx=the_area.m_width;
  maxy=the_area.m_height;
  for(i=0;i<the_area.header.entrancecnt;i++)
  {
    RetrieveVariable(entrancename,the_area.entranceheaders[i].entrancename);
    px=the_area.entranceheaders[i].px; //closed door, bounding rect
    py=the_area.entranceheaders[i].py;
    
    if(px>=maxx || py>=maxy)
    {
      ret|=BAD_ATTR;
      log("Entrance #%d (%-.32s [%d.%d]) is out of map.",
        i+1,entrancename,px,py);
    }
  }
  return ret;
}

int CChitemDlg::check_area_item()
{
  int i,j;
  int ret;
  int cnt, last;
  int ref;
  
  ret=0;
  for(i=0;i<the_area.header.itemcnt;i++)
  {
    ref=0;
    for(cnt=0;cnt<the_area.header.containercnt;cnt++)
    {
      j=the_area.containerheaders[cnt].firstitem;
      if(i>=j)
      {
        j+=the_area.containerheaders[cnt].itemcount;
        if(i<j)
        {
          last=cnt;
          ref++;
        }
      }
    }
    switch(ref)
    {
    case 0:
      ret|=BAD_INDEX;
      log("Item #%d isn't in any containers ???",i+1);
      break;
    case 1:
      break;
    default:
      ret|=BAD_INDEX;
      log("Item #%d is in multiple containers ???",i+1);
      break;
    }
    
    if(!ref)
    {
      continue;
    }
    switch(check_itemref(the_area.itemheaders[i].itemname,1)) //allow random items here
    {
    case -1:
      ret|=BAD_ITEMREF;
      log("Non existent item (%-.8s) in container #%d (%-.32s [%d.%d])",
        the_area.itemheaders[i].itemname,last+1,the_area.containerheaders[last].containername,
        the_area.containerheaders[last].posx,the_area.containerheaders[last].posy);
      break;
    case -2:
      ret|=BAD_ITEMREF;
      log("Deleted item in container #%d (%-.32s [%d.%d])",
        cnt+1,the_area.containerheaders[last].containername,
        the_area.containerheaders[last].posx,the_area.containerheaders[last].posy);
    }
  }
  return ret;
}

int CChitemDlg::check_area_mapnote()
{
  int i;
  int ret;
  
  ret=0;
  for(i=0;i<the_area.header.mapnotecnt;i++)
  {
    switch(check_reference(the_area.mapnoteheaders[i].strref) )
    {
    case 1:
      ret|=BAD_STRREF;
      log("Invalid mapnote #%d (reference: %d).",i+1, the_area.mapnoteheaders[i].strref);
      break;
    case 2:
      ret|=BAD_STRREF;
      log("Deleted mapnote #%d (reference: %d).",i+1, the_area.mapnoteheaders[i].strref);
      break;
    }
  }
  return ret;
}

int CChitemDlg::check_area_song()
{
  int ret;
  CString tmpref;
  
  ret=0;
  switch(check_soundref(the_area.songheader.dayambi1,1))
  {
  case -1:
    ret|=BAD_RESREF;
    log("Invalid ambient: '%-.8s'",the_area.songheader.dayambi1);
    break;
  case -2:
    if(!the_area.songheader.volumed)
    {
      log("Zero volume in songheader.");
    }
  }
  switch(check_soundref(the_area.songheader.dayambi2,1))
  {
  case -1:
    ret|=BAD_RESREF;
    log("Invalid ambient: '%-.8s'",the_area.songheader.dayambi2);
    break;
  }
  switch(check_soundref(the_area.songheader.nightambi1,1))
  {
  case -1:
    ret|=BAD_RESREF;
    log("Invalid ambient: '%-.8s'",the_area.songheader.nightambi1);
    break;
  case -2:
    if(!the_area.songheader.volumed)
    {
      log("Zero volume in songheader.");
    }
  }
  switch(check_soundref(the_area.songheader.nightambi2,1))
  {
  case -1:
    log("Invalid ambient: '%-.8s'",the_area.songheader.nightambi2);
    break;
  }
  return ret;
}

int CChitemDlg::check_area_spawn()
{
  int i,j;
  int ret;
  int cnt;
  CString tmpref;
  CString spawnname;
  unsigned int maxx,maxy;
  unsigned int px,py;
  
  ret=0;
  maxx=the_area.m_width;
  maxy=the_area.m_height;
  for(i=0;i<the_area.header.spawncnt;i++)
  {
    RetrieveVariable(spawnname, the_area.spawnheaders[i].spawnname);
    px=the_area.spawnheaders[i].px;
    py=the_area.spawnheaders[i].py;
    if(px>=maxx || py>=maxy)
    {
      ret|=BAD_ATTR;
      log("Spawnpoint #%d (%-.32s [%d.%d]) is out of map.",
        i+1,spawnname,px,py);
    }
    
    if(!the_area.spawnheaders[i].delay)
    {
      ret|=BAD_ATTR;
      log("Spawnpoint #%d (%-.32s [%d.%d]) has 0 delay, causing crash!",
        i+1,the_area.spawnheaders[i].spawnname,
        the_area.spawnheaders[i].px,the_area.spawnheaders[i].py);
    }
    if(!the_area.spawnheaders[i].schedule)
    {
      ret|=BAD_ATTR;
      log("Spawnpoint #%d (%-.32s [%d.%d]) has no schedule!",
        i+1,the_area.spawnheaders[i].spawnname,
        the_area.spawnheaders[i].px,the_area.spawnheaders[i].py);
    }
    cnt=the_area.spawnheaders[i].creaturecnt;
    for(j=0;j<10;j++)
    {
      RetrieveResref(tmpref,the_area.spawnheaders[i].creatures[j]);
      if(j<cnt)
      {
        switch(check_creature_reference(tmpref,true))
        {
        case -1:
          log("Invalid creature reference '%-.8s' in spawn header #%d/%d (%-.32s [%d.%d])",
            tmpref,i+1,j+1,spawnname, px,py);
        }
      }
      else
      {
        if(!tmpref.IsEmpty())
        {
          log("Useless creature reference '%-.8s' in spawn header #%d/%d (%-.32s [%d.%d])",
            tmpref,i+1,j+1,spawnname, px,py);
        }
      }
    }
  }
  return ret;
}

#define AR_PROXIMITY  0
#define AR_INFO       1
#define AR_TRAVEL     2
#define AR_TRAVELINFO 3 //unsure

int CChitemDlg::check_area_trigger()
{
  int i;
  int ret;
  int ttype;
  int tmp;
  int fhandle;
  loc_entry fileloc;
  CString infoname, destination, entrancename;
  unsigned int maxx,maxy;
  unsigned int px,py;
  unsigned int b1x,b1y;
  unsigned int b2x,b2y;
  Carea tmparea;
  
  ret=0;
  maxx=the_area.m_width;
  maxy=the_area.m_height;
  for(i=0;i<the_area.header.infocnt;i++)
  {
    RetrieveVariable(infoname,the_area.triggerheaders[i].infoname);
    px=the_area.triggerheaders[i].p1x; //bounding rect
    py=the_area.triggerheaders[i].p1y;
    if(!(ret&BAD_VERTEX))
    {
      if(px>=maxx || py>=maxy)
      {
        ret|=BAD_VERTEX;
        log("Trigger region #%d (%-.32s [%d.%d]) is out of map.",
          i+1,infoname,px,py);
      }
      
      b1x=the_area.triggerheaders[i].p1x;
      b1y=the_area.triggerheaders[i].p1y;
      b2x=the_area.triggerheaders[i].p2x;
      b2y=the_area.triggerheaders[i].p2y;
      if(((b1x!=-1) || (b1y!=-1)) && ( (b1x>=b2x) || (b1y>=b2y)))
      {
        ret|=BAD_VERTEX;
        log("Trigger region #%d (%-.32s [%d.%d]) has invalid bounding box, possibly damaged polygon.",
          i+1,infoname,px,py);
      }
      if((b1x+800<b2x) || (b1y+600<b2y))
      {
        ret|=BAD_VERTEX;
        log("Trigger region #%d (%-.32s [%d.%d]) has too large bounding box, possibly damaged polygon.",
          i+1,infoname,px,py);
      }
    }
    
    ttype=the_area.triggerheaders[i].triggertype;
    if(ttype>3)
    {
      ret|=BAD_ATTR;
      log("Invalid trigger type in active region #%d (%-.32s [%d.%d])",
        i+1,infoname,px,py);
    }
    
    tmp=the_area.triggerheaders[i].cursortype;
    if((tmp&1) || (tmp>64) )
    {
      ret|=BAD_ATTR;
      log("Cursor type (0x%x) is invalid (must be even and less than 0x40) in active region #%d (%-.32s [%d.%d])",
        tmp,i+1,infoname,px,py);
    }
    if(ttype&AR_TRAVEL)
    {
      RetrieveResref(destination,the_area.triggerheaders[i].destref);
      if(check_area_reference(the_area.triggerheaders[i].destref,1) )
      {
        ret|=BAD_RESREF;
        log("Invalid destination area '%s' (causes crash) in active region #%d (%-.32s [%d.%d])",
          destination, i+1,infoname,px,py);
      }
      else
      {
        areas.Lookup(destination,fileloc);   //this must succeed!
        fhandle=locate_file(fileloc, 0);  //hopefully this too
        if(fhandle)
        {
          RetrieveVariable(entrancename,the_area.triggerheaders[i].destname);
          if(tmparea.CheckDestination(fhandle, fileloc.size, entrancename))
          {
            ret|=BAD_ATTR;
            log("Invalid destination reference '%-.32s' for area '%s' (causes crash) in active region #%d (%-.32s [%d.%d])",
              entrancename, destination, i+1,infoname,px,py);
            //the destination area does not exist in the other area, this causes crash
          }
          close(fhandle);
        }
        else
        {
          ret|=BAD_ATTR;
          log("Can't load area: %s\n",destination);
        }
      }
      if(the_area.triggerheaders[i].infoflags&1024)
      {
        if(!the_area.triggerheaders[i].pointx && !the_area.triggerheaders[i].pointy)
        {
          ret|=BAD_ATTR;
          log("Trigger '%.32s' has a zero use point.", infoname);
        }
      }

      if(the_area.triggerheaders[i].infoflags&2048)
      {
        if(find_regionlink(the_area.triggerheaders[i].infoname)<0)
        {
          ret|=BAD_ATTR;
          log("Door travel trigger '%.32s' isn't referenced by door.",infoname);
        }
      }
    }
    /*what is this
    if(ttype&AR_INFO)
    {
    
      }
    */
    /* no dialog
    switch(check_dialogres(the_area.triggerheaders[i].dialogref,true) )
    {
    case -1:
    ret|=BAD_ATTR;
    log("Invalid dialog reference '%-.8s' in active region #%d (%-.32s [%d.%d])",
    the_area.triggerheaders[i].dialogref,i+1,infoname,px,py);
    }
    */
    switch(check_scriptref(the_area.triggerheaders[i].scriptref,true) )
    {
    case -1:
      ret|=BAD_ATTR;
      log("Invalid script reference '%-.8s' in active region #%d (%-.32s [%d.%d])",
        the_area.triggerheaders[i].scriptref,i+1,infoname,px,py);
    }
  }
  return ret;
}

int CChitemDlg::check_area_variable()
{
  int i;
  int ret;
  
  ret=0;
  for(i=0;i<the_area.header.variablecnt;i++)
  {
    if(!the_area.variableheaders[i].variablename[0])
    {
      ret|=BAD_ATTR;
      log("Empty variable entry #%d",i+1);
    }
  }
  return ret;
}

int CChitemDlg::check_area_vertex()
{
  int i;
  int ret;
  
  if(!iwd2_structures()) {
    return 0;
  }
  ret=0;
  for(i=0;i<the_area.wallpolygoncount;i++)
  {
    //wall
    if(the_area.wallpolygonheaders[i].flags&1)
    {
      if(the_area.wallpolygonheaders[i].countvertex>255)
      {
        ret|=BAD_VERTEX;
        log("Wall polygon #%d has more than 255 vertices.",i);
      }
    }
  }
  return ret;
}

int CChitemDlg::check_area_interrupt()
{
  int i;
  int ret;
  int cnt;
  
  ret=0;
  cnt=the_area.intheader.creaturecnt;
  for(i=0;i<10;i++)
  {
    if(i<cnt)
    {
      switch(check_reference(the_area.intheader.strrefs[i],0) )
      {
      case 1:
        ret|=BAD_STRREF;
        log("Invalid message in interrupt entry #%d (reference: %d).",i+1, the_area.intheader.strrefs[i]);
        break;
      case 2:
        ret|=BAD_STRREF;
        log("Deleted message in interrupt entry #%d (reference: %d).",i+1, the_area.intheader.strrefs[i]);
        break;
      }
    }
    else
    {
      if(the_area.intheader.creatures[i][0])
      {
        ret|=BAD_ATTR;
        log("Useless interrupt entry #%d (counter is %d), message # is %d, creature is %-.8s",
          i+1,cnt,the_area.intheader.strrefs[i], the_area.intheader.creatures[i] );
      }
    }
  }
  return ret;
}

int CChitemDlg::check_area(int swap_weds)
{
  int ret;
  CString tmpref;
  int flg;
  
  ret=0;
  if(the_area.tilecount) log("Tiled object found!");
  if(the_area.exploredsize) log("Contains explored area!");
  RetrieveResref(tmpref,the_area.header.wed);
  if(check_wed_reference(tmpref, 1) )
  {
    ret|=BAD_RESREF;
    log("Wedfile '%s' is missing!", tmpref);
  }
  if(check_bitmap(tmpref+"LM",false) )
  {
    ret|=BAD_RESREF;
    log("Missing lightmap: %sLM", tmpref);
  }
  if(the_area.header.areatype&EXTENDED_NIGHT)
  {
    if(check_bitmap(tmpref+"LN",false) )
    {
      ret|=BAD_RESREF;
      log("Missing night lightmap: %sLN", tmpref);
    }
  }
  if(check_bitmap(tmpref+"SR",false) )
  {
    ret|=BAD_RESREF;
    log("Missing searchmap: %sSR", tmpref);
  }
  if(check_bitmap(tmpref+"HT",false) )
  {
    ret|=BAD_RESREF;
    log("Missing heightmap: %sHT", tmpref);
  }

  tmpref+="N";
  switch(check_wed_reference(tmpref, the_area.header.areatype&EXTENDED_NIGHT) )
  {
  case -1:
    ret|=BAD_RESREF;
    log("Night wedfile '%s' is missing!", tmpref);
    break;
  case -2:
    ret|=BAD_ATTR;
    log("Night wedfile '%s' is present!", tmpref);
    break;
  }
  flg=bg1_compatible_area();
  if(check_area_reference(the_area.header.northref,flg&&the_area.header.northflags))
  {
    ret|=BAD_ATTR;
    if (bg1_compatible_area()) {
      log("Problem with north area reference");
    } else {
      log("Unused north area reference");
    }
  }
  if(check_area_reference(the_area.header.eastref,flg&&the_area.header.eastflags))
  {
    ret|=BAD_ATTR;
    if (bg1_compatible_area()) {
      log("Problem with east area reference");
    } else {
      log("Unused east area reference");
    }
  }
  if(check_area_reference(the_area.header.southref,flg&&the_area.header.southflags))
  {
    ret|=BAD_ATTR;
    if (bg1_compatible_area()) {
      log("Problem with south area reference");
    } else {
      log("Unused south area reference");
    }
  }
  if(check_area_reference(the_area.header.westref,flg&&the_area.header.westflags))
  {
    ret|=BAD_ATTR;
    if (bg1_compatible_area()) {
      log("Problem with west area reference");
    } else {
      log("Unused west area reference");
    }
  }
  if(the_area.header.lastsaved)
  {
    ret|=BAD_ATTR;
    log("Lastsaved field is set."); //may be a problem
  }
  switch(check_scriptref(the_area.header.scriptref,true))
  {
  case -1:
    ret|=BAD_RESREF;
    log("Area script '%-.8s' does not exist.",the_area.header.scriptref);
    break;
  case 0:
    if (!(chkflg&NOSCRIPT))
    {
      if(the_area.header.scriptref[0] && strnicmp(itemname,the_area.header.scriptref,8) )
      {
        log("Scriptname '%-.8s' isn't the same as the area name (possible problem)",the_area.header.scriptref);
      }
    }
  }
  if(!the_area.wedheader.overlaycnt || !the_area.overlaycount || !the_area.m_height || !the_area.m_width)
  {
    ret|=BAD_EXTHEAD;
    log("Area has no dimensions!");
    return ret;
  }
  
  ret|=check_area_actors();
  ret|=check_area_trigger();
  ret|=check_area_spawn();
  ret|=check_area_entrance();
  ret|=check_area_container();
  ret|=check_area_item();
  ret|=check_area_ambient();
  ret|=check_area_anim();
  ret|=check_area_vertex();
  ret|=check_area_variable();
  ret|=check_area_door();
  ret|=check_area_mapnote();
  if(the_area.header.intoffset)
  {
    ret|=check_area_interrupt();
  }
  else
  {
    ret|=BAD_EXTHEAD;
    log("Missing rest interruption header.");
  }
  if(the_area.header.songoffset)
  {
    ret|=check_area_song();
  }
  else
  {
    ret|=BAD_EXTHEAD;
    log("Missing song header.");
  }
  if(the_area.WedAvailable())
  {
    ret|=check_wed(swap_weds);
  }
  else
  {
    ret|=BAD_EXTHEAD;
  }
  return ret;
}

int CChitemDlg::check_wed(int swap_weds)
{
  CString tmpstr;
  CString daytile, nighttile;
  loc_entry dummy;
  short *doortiles;
  POSITION pos;
  int ret;
  int idx;
  int tilecount;
  int i,j;
  int overlaybits;
  Cmos tmptis;
  int bad;
  
  ret=0;
retry:
  if(the_area.wedheader.overlaycnt!=5)
  {
    ret|=BAD_EXTHEAD;
    log("Non-standard wed overlay count: %d", the_area.wedheader.overlaycnt);
  }

  if(the_area.overlaycount<1)
  {
    log("Cannot check without overlay!");
    return ret;
  }

  overlaybits = 1;
  if (the_area.m_night)
  {
    RetrieveResref(nighttile, the_area.overlayheaders[0].tis);
    ret = read_tis_preview(nighttile, &tmptis, 0);
  } else
  {
    RetrieveResref(daytile, the_area.overlayheaders[0].tis);
    ret = read_tis_preview(daytile, &tmptis, 0);
  }
  if (ret<0)
  {
    log("Cannot check wed without tileset!");
    return ret;
  }
  
  i=the_area.overlayheaders[0].height;
  if (i>60 || i<5)
  {
    ret|=BAD_EXTHEAD;
    log("Invalid area height (5-60)");
  }
  j=the_area.overlayheaders[0].width;
  if (j>80 || j<5)
  {
    ret|=BAD_EXTHEAD;
    log("Invalid area width (5-80)");
  }
  tilecount=i*j;
  bad = 0;
  for(i=0;i<tilecount;i++)
  {
    if(the_area.overlaytileheaders[i].counttileprimary-1>12)
    {
      ret|=BAD_EXTHEAD;
      log("Invalid tile in [%d.%d] (animation count)",i%the_area.overlayheaders[0].width,i/the_area.overlayheaders[0].width);
      continue;
    }
    int alt = the_area.overlaytileheaders[i].alternate;
    if (alt!=-1 && bad<=10)
    {
      if ((unsigned long) alt>=tmptis.tisheader.numtiles)
      {
        ret|=BAD_EXTHEAD;
        log("Invalid alt. tile (%d) in [%d.%d]", alt, i%the_area.overlayheaders[0].width,i/the_area.overlayheaders[0].width);
        bad++;
        if (bad>10)
        {
          log("more than 10...");
        }
      }
    }
    overlaybits|=the_area.overlaytileheaders[i].overlayflags;
    idx=the_area.overlaytileheaders[i].firsttileprimary;
    idx=(unsigned short) the_area.overlaytileindices[idx];
    if((unsigned long) idx>=tmptis.tisheader.numtiles)
    {
      ret|=BAD_EXTHEAD;
      log("Invalid tile in [%d.%d]",i%the_area.overlayheaders[0].width,i/the_area.overlayheaders[0].width);
    }
  }
  if (is_this_bgee())
  {
    if (tmptis.tisheader.tilelength!=12)
    {
      ret|=BAD_EXTHEAD;
      log("Tileset (%s) is in old format.", tmptis.m_name);
    }
  }
  else
  {
    if (tmptis.tisheader.tilelength!=5120)
    {
      ret|=BAD_EXTHEAD;
      log("The tileset (%s) is corrupted (or you didn't configure DLTCEP to BGEE).", tmptis.m_name);
    }
  }

  pos=the_area.doortilelist.GetHeadPosition();
  for(i=0;i<the_area.weddoorcount;i++)
  {
    doortiles=(short *) the_area.doortilelist.GetNext(pos);
    for(j=0;j<the_area.weddoorheaders[i].countdoortileidx;j++)
    {
      idx=(unsigned short) doortiles[j];
      if(idx>=the_area.overlaytilecount)
      {
        ret|=BAD_EXTHEAD;
        RetrieveResref(tmpstr, the_area.weddoorheaders[i].doorid);
        log("Invalid %d. door (%s) tile: %d",i+1,tmpstr,j);
      }
    }
  }
  j=1;
  for(i=0;i<5;i++)
  {
    if (overlaybits&j)
    {
      RetrieveResref(tmpstr, the_area.overlayheaders[i].tis);
      if (!tis.Lookup(tmpstr, dummy))
      {
        ret|=BAD_RESREF;
        log("Missing tileset #%d: %s causes crash!", i+1, tmpstr);
      }
    }
    j+=j;
  }

  if(swap_weds && (the_area.header.areatype&EXTENDED_NIGHT))
  {
    if (the_area.m_night!=1)
    {
      the_area.m_night = 1;
      ReadWed(0);
      goto retry;
    }
    if (daytile==nighttile)
    {
      log("The night and day tilesets are the same, though the area was supposed to be extended night.");
    }
  }

  return ret;
}

int CChitemDlg::check_ids()
{
  CIntMapInt used;
  CString *tmppoi;
	POSITION pos;
  int mask, val, ret, line, line2;

  ret = 0;
  if(itemname=="TRIGGER") mask=0x3fff;
  else mask=~0;
  pos=the_ids.data->GetHeadPosition();
  line = 1;
  while(pos)
  {
    tmppoi=(CString *) the_ids.data->GetNext(pos);
    val=strtonum(tmppoi[0])&mask;
    line2 = used[val];
    if (line2)
    {
      log("%d is duplicated in lines %d and %d.", val, line, line2);
      ret = BAD_INDEX;
    }
    else
    {
      used[val]=line;
    }
    line++;
  }
  return ret;
}

int CChitemDlg::check_scriptitems()
{
  int ret;
  CString key, tmp;
  
  the_script.RollBack();
  do
  {
    ret=the_script.get_next_scriptitem(key, 0);
    switch(ret)
    {
    case 1:
      if(storeitems.Lookup(key, tmp) )
      {
        if(!tmp.IsEmpty())
        {
          log("The item %s was already stocked at %s",key,tmp);
        }
        storeitems[key]=itemname+".bcs";
      }
      break;
    case 0: break; //end of file
    default: //some error
      return ret;
      break;
    }
  }
  while(ret==1);
  return 0;
}

inline int check_flag(int a, int b)
{
  if(a!=b)
  {
    if(a) return 1;
    return 2;
  }
  return 0;
}

long resolve_store_flags(int storetype, unsigned long flags)
{
  switch(storetype)
  {
  case STT_STORE:
    return flags|ST_BUY|ST_SELL;
  case STT_TAVERN:
    return flags|ST_DRINK;
  case STT_INN:
    return flags|ST_RENT;
  case STT_TEMPLE:
    return flags|ST_CURE|ST_DONAT;
  case STT_CONT:
  case STT_IWDCONT:
    flags&=~(ST_BUY|ST_SELL);
    return flags;
  }
  return -1; //not a valid store type
}

int CChitemDlg::check_counters(unsigned long flags)
{
  int ret;
  
  if(chkflg&NOATTRCH) return 0;
  ret=0;
  switch(check_flag(!(flags&ST_BUY),!the_store.header.itemcount))
  {
  case 2:
    if((the_store.header.type!=STT_CONT) && (the_store.header.type!=STT_IWDCONT) )
    {
      log("Store selling flag set, but no stock.");
      ret|=BAD_ATTR;
    }
    break;
  case 1:
    log("Store selling flag not set, but there is stock.");
    ret|=BAD_ATTR;
    break;
  }
  switch(check_flag(!(flags&ST_CURE),!the_store.header.curecount))
  {
  case 2:
    log("Store cure flag set, but no cures.");
    ret|=BAD_ATTR;
    break;
  case 1:
    log("Store cure flag not set, but there are cures.");
    ret|=BAD_ATTR;
    break;
  }
  switch(check_flag(!(flags&ST_DRINK),!the_store.header.drinkcount))
  {
  case 2:
    log("Store drink flag set, but no drinks.");
    ret|=BAD_ATTR;
    break;
  case 1:
    log("Store drink flag not set, but there are drinks.");
    ret|=BAD_ATTR;
    break;
  }
  return ret;
}

int CChitemDlg::check_storetype(unsigned int type, int iscont)
{
  unsigned int illeg;
  int ret;
  
  if(chkflg&NOSTATCH) return 0;
  if(iwd2_structures() ) illeg=STT_CONT;
  else illeg=STT_IWDCONT;
  if((type>NUM_STORETYPE) || (type==illeg) )
  {
    log("Invalid store type: %s",format_storetype(type));
    return BAD_STAT;
  }  
  ret=0;
  if(iscont)
  {
    if((type!=STT_CONT) && (type!=STT_IWDCONT) )
    {
      log("Container has %s type.",format_storetype(type));
      ret|=BAD_STAT;
    }
    if(the_store.header.selling || the_store.header.buying)
    {
      log("Container has selling/buying price set.");
      ret|=BAD_STAT;
    }
  }
  else
  {
    if(type==STT_CONT)
    {
      log("Store has container type?");
      ret|=BAD_STAT;
    }
    if(!the_store.header.selling && (the_store.header.flags&ST_BUY) )
    {
      log("Store has no selling price rate set.");
      ret|=BAD_STAT;
    }
    if(!the_store.header.buying && (the_store.header.flags&ST_SELL) )
    {
      log("Store has no buying price rate set.");
      ret|=BAD_STAT;
    }
    
    if(the_store.header.flags&(ST_SELL|ST_BUY))
    {
      if(the_store.header.selling<the_store.header.buying)
      {
        log("Store sells cheaper than it buys.");
        ret|=BAD_STAT;
      }
    }    
  }  
  return ret;
}

int CChitemDlg::check_storespelltype()
{
  int i;
  CString tmpref;
  loc_entry dummy;
  tooltip_data dummy2;
  
  for(i=0;i<the_store.curenum;i++)
  {
    RetrieveResref(tmpref,the_store.cures[i].curename);
    if(!spells.Lookup(tmpref, dummy) )
    {
      log("Invalid spell (%s) in temple.",tmpref);
      return BAD_RESREF;
    }
    if(!store_spell_desc.Lookup(tmpref,dummy2))
    {
      log("Spell (%s) has no entry in speldesc.2da.",tmpref);
      return BAD_STRREF;
    }
  }
  return 0;
}

int CChitemDlg::check_storedrinktype()
{
  int i;
  CString tmpref;
  loc_entry dummy;
  int ret;
  
  ret=0;
  for(i=0;i<the_store.drinknum;i++)
  {
    switch(check_scriptref(the_store.drinks[i].drinkscript,true) )
    {
    case 1:
      ret|=BAD_RESREF;
      log("Invalid drink script #%d (%-.8s) in tavern.",i+1, the_store.drinks[i].drinkscript);
    }
    switch(check_reference(the_store.drinks[i].drinkname))
    {
    case 1:
      ret|=BAD_STRREF;
      log("Invalid drink name #%d (reference: %d).",i+1, the_store.header.strref);
      break;
    case 2:
      ret|=BAD_STRREF;
      log("Deleted drink name #%d (reference: %d).",i+1, the_store.header.strref);
      break;
    }
  }
  return ret;
}

int CChitemDlg::check_storeitemtype()
{
  int i;
  
  for(i=0;i<the_store.itemtypenum;i++)
  {
    if((the_store.itemtypes[i]<0) || (the_store.itemtypes[i]>NUM_ITEMTYPE))
    {
      log("Invalid itemtype (%x) in store.",the_store.itemtypes[i]);
      return BAD_ITEMTYPE;
    }
  }
  return 0;
}

int CChitemDlg::check_storeentries()
{
  int i;
  int ret;
  int stock;
  CString tmpref, key, tmp;
  
  ret=0;
  for(i=0;i<the_store.entrynum;i++)
  {
    RetrieveResref(tmpref, the_store.entries[i].itemname);
    tmpref.MakeUpper();

    if ( check_itemref(the_store.entries[i].itemname, 0) )
    {
      log("Nonexistent item (%s) in store entry #%d", tmpref, i+1);
    }
    
    if(the_store.entries[i].flags&~7)
    {
      log("Unusual stored item flags (%x) in store item #%d.", the_store.entries[i].flags, i+1);
      ret|=BAD_ITEMTYPE;
    }
    stock=the_store.entries[i].count;
    switch(the_store.entries[i].infinite)
    {
    case 0:
      if(!stock)
      {
        log("Zero stock for stored item #%d.",i+1);
        ret|=BAD_ITEMTYPE;
      }
      break;
    case 1:
      //too many items tripped this check
      if(stock>1)
      {
        log("Infinite flag set for %d (%s) item #%d.", stock, tmpref, i+1);
        ret|=BAD_ITEMTYPE;
      }
      stock = 99;
      break;
    default:
      log("Unusual infinite flag (%x) in store item #%d.", the_store.entries[i].infinite, i+1);
      ret|=BAD_ITEMTYPE;
      stock = 99;
    }

    if(storeitems.Lookup(key, tmp) )
    {
      if(stock!=1)
      {
        log("The stock is not exactly 1 for a container (%s).",key);
      }
      if(!tmp.IsEmpty())
      {
        log("The item %s was already stocked at %s",key,tmp);
        ret|=1;
      }
      storeitems[key]=itemname+".sto";
    }  
  }
  return ret;
}

int CChitemDlg::check_store()
{
  CString tmpref;
  int ret, ret2;
  int flags;
  
  ret=0;
  switch(check_reference(the_store.header.strref) )
  {
  case 1:
    ret|=BAD_STRREF;
    log("Invalid store name (reference: %d).",the_store.header.strref);
    break;
  case 2:
    ret|=BAD_STRREF;
    log("Deleted store name (reference: %d).",the_store.header.strref);
    break;
  }
  
  switch(check_dialogres(the_store.header.dlgresref3,false) )
  {
  case -1:
    ret|=BAD_RESREF;
    log("Missing store dialog reference: %-.8s.",the_store.header.dlgresref3);
    break;
  }
  
  switch(check_dialogres(the_store.header.dlgresref2,false) )
  {
  case -1:
    ret|=BAD_RESREF;
    log("Missing store dialog reference: %-.8s.",the_store.header.dlgresref2);
    break;
  }
  
  switch(check_dialogres(the_store.header.dlgresref1,false) )
  {
  case -1:
    ret|=BAD_RESREF;
    log("Missing store dialog reference: %-.8s.",the_store.header.dlgresref1);
    break;
  }
  flags=resolve_store_flags(the_store.header.type, the_store.header.flags);
  if(flags<0)
  {
    ret|=BAD_ATTR;
    log("Invalid store type: %d",the_store.header.type);    
  }
  else
  {
    ret2=check_counters(flags);
    ret|=ret2;
    
    ret2=check_storetype(the_store.header.type,storeitems.Lookup(itemname,tmpref));
    ret|=ret2;
    
    ret2=check_storeitemtype();
    ret|=ret2;
    
    ret2=check_storeentries();
    ret|=ret2;
    
    ret2=check_storespelltype();
    ret|=ret2;
    
    ret2=check_storedrinktype();
    ret|=ret2;
  }
  return ret;
}

int CChitemDlg::check_proj_explode()
{
  int ret;
  
  ret=0;
  switch(check_vvc_reference(the_projectile.extension.vvc,!!(the_projectile.extension.aoe&PROJ_HAS_VVC)) )
  {
  case -1:
    log("VVC is missing from projectile.");
    ret=BAD_RESREF;
    break;
  case 1:
    log("VVC is unused in projectile.");
    ret=BAD_RESREF;
    break;
  }
  return ret;
}

int CChitemDlg::check_proj_header()
{
  int ret;
  
  ret=0;
  //the bigger the faster
  if(the_projectile.header.speed>1000 || (the_projectile.header.speed%5) )
  {
    log("Unusual travel speed: %d",the_projectile.header.speed); //not a bug afaik
  }
  if(the_projectile.header.sparks&1)
  {
    if(the_projectile.header.spkcolour<1 || the_projectile.header.spkcolour>12)
    {
      ret|=BAD_ANIM;
      log("Sparkle colour must be 1-12!");
    }
    if(!(the_projectile.header.sparks&2) )
    {
      log("Sparks are not visible for creeping projectiles.");
    }
  }
  return ret;
}

int CChitemDlg::check_projectile()
{
  CString format;
  int expl;
  int ret;
  
  ret=0;
  switch(the_projectile.header.type)
  {
  case 3:
    if(!the_projectile.hasextension)
    {
      log("Missing explosion extension (causes crash).");
      expl=0;
    }
    else expl=1;
    break;
  case 2:
    expl=0;
    if(the_projectile.hasextension)
    {
      log("Dead explosion extension.");
    }
    break;
  default:
    log("Invalid projectile type:%d. (type must be 2 or 3)",the_projectile.header.type);
    return 1;    
  }
  ret|=check_proj_header();
  
  if(expl) ret|=check_proj_explode();
  return ret;
}

int CChitemDlg::check_bam()
{
  int ret;
  int cycle;
  
  if (chkflg&WARNINGS)
  {
    ret = 0;
  }
  else
  {
    ret=the_bam.DropUnusedFrame(0);
    if(ret)
    {
      log("Could drop %d unused frame(s).",ret);
      ret=BAD_COMPRESS;
    }
  }
  if(the_bam.GetTransparentIndex())
  {
    log("This bam has a nonzero transparent index.");
    ret|=BAD_ATTR;
  }
  
  //compression check
  if(no_compress()) return ret; //don't check bams for lame noncompressing games
  if(the_bam.GetFrameCount()<=4) //most likely item bams
  {
    //apparently animation bams are not compressed, could they be ?
    if(the_bam.CanCompress() )
    {
      ret|=BAD_COMPRESS;
      log("Bam could be compressed to save at least %d bytes.",the_bam.GetDataSize()/2);
    }
  }

  if (!(chkflg&NOSTRUCT))
  {
    cycle = the_bam.CheckFrameSizes();
    if (cycle>0)
    {
      log("This bam has entirely empty frames in %d cycle(s).", cycle);
      ret|=BAD_ATTR;
    }
  }
  return ret;
}

int CChitemDlg::check_variable(const gam_variable *var, CString scope)
{
  int cnt;
  CString varname;
  
  varname = scope+var->variablename;
  if (variables.Lookup(varname, cnt))
  {
    return 0;
  }
  return BAD_VAR;
}

int CChitemDlg::check_game()
{
  CString tmpstr;
  int i;
  int ret;
  
  ret=0;
  for(i=0;i<the_game.npccount;i++)
  {
    if(!the_game.npcs[i].cresize && check_creature_reference(the_game.npcs[i].creresref,false) )
    {
      log("Invalid creature (%-.8s) in %d. npc block",the_game.npcs[i].creresref, i+1);
      ret=BAD_RESREF;
    }
    if(check_area_reference(the_game.npcs[i].curarea,1) )
    {
      log("Invalid area (%-.8s) in %d. npc block", the_game.npcs[i].curarea, i+1);
      ret=BAD_RESREF;
    }
  }
  if(check_area_reference(the_game.header.masterarea,2) )
  {
    log("Invalid master area: %-.8s",the_game.header.masterarea);
    ret=BAD_RESREF;
  }
  if(check_area_reference(the_game.header.currentarea,1) )
  {
    log("Invalid current area: %-.8s",the_game.header.currentarea);
    ret=BAD_RESREF;
  }
  
  for(i=0;i<9;i++) //alignment numbers
  {
    if(check_creature_reference(the_game.familiar.familiars[i],false)==-1)
    {
      log("Invalid familiar: %-.8s",the_game.familiar.familiars[i]);
      ret=BAD_RESREF;
    }
  }
  if (pst_compatible_var()) {
    for(i=0;i<the_game.variablecount;i++) {
      if (check_variable(the_game.variables+i,"GLOBAL")) {
        log("Invalid global variable: %-.32s", the_game.variables[i].variablename);
        ret|=BAD_VAR;
      }
    }
    
    for(i=0;i<the_game.deathvariablecount;i++) {
      if (check_variable(the_game.deathvariables+i,"KAPUTZ")) {
        log("Invalid kaputz variable: %-.32s", the_game.deathvariables[i].variablename);
        ret|=BAD_VAR;
      }
    }
    
  }
  return ret;
}

int CChitemDlg::check_point_string(CString point)
{
  int a, b;
  
  if(point.GetLength()<2 || point[0]!='[' || point[point.GetLength()-1]!=']')
  {
    return -1;
  }
  a=-1;
  b=-1;
  sscanf(point, "[%d.%d]", &a, &b);
  if(a==-1 || b==-1) return -1;
  //return -2 if the area has no such point
  return 0;
}

bool CChitemDlg::check_value(CString section, CString key, bool error)
{
  if(the_ini.GetValue(section, key).IsEmpty())
  {
    if(error)
    {
      log("Missing %s key in section %s", key, section);
    }
    return true;
  }
  return false;
}

void CChitemDlg::check_keys(CString section, int type)
{
  CString key, value;
  POSITION pos;
  int tmp;
  CStringMapString *sect = NULL;
  
  the_ini.Lookup(section, sect);
  
  pos=sect->GetStartPosition();
  while(pos)
  {
    sect->GetNextAssoc(pos, key, value);
    tmp = -1;
    ini_entry.Lookup(key, tmp);
    
    if(tmp!=type)
    {
      log("Invalid key: %s in section: %s", key, section);
    }
  }
}

void CChitemDlg::check_creature_section(CString section, CString referenced)
{
  
  if(section.IsEmpty()) return;
  
  if(!the_ini.HasSection(section))
  {
    log("There is no %s spawn section, but it is referenced in %s", section, referenced);
    return;
  }
  check_keys(section, INI_CREATURE);
  
  check_value(section, "cre_file", true);
  if(check_value(section, "spec", !has_xpvar()))
  {
    if(has_xpvar())
    {
      check_value(section,"spec_var", true);
    }
  }
  CString ps = the_ini.GetValue(section,"point_select");
  if(check_value(section, "spawn_point", false))
  {
    if(check_value(section, "spawn_point_global", false))
    {
      log("Neither spawn_spoint nor spawn_point_global keys exist in section %s", section);
      return;
    }
    if(ps.Compare("e"))
    {
      log("Point_select isn't set to 'e' but it needs for spawn_point_global in section %s", section);
      return;
    }
  }
  else
  {
    int a, b, c;
    CString tmpstr = the_ini.GetValue(section,"spawn_point");
    
    a=-1;
    b=-1;
    c=-1;
    sscanf(tmpstr,"[%d.%d:%d]", &a, &b, &c);
    if(b==-1)
    {
      sscanf(tmpstr,"[%d.%d]", &a, &b);
    }
    if(b==-1 || a==-1)
    {
      log("Invalid spawn_point value: %s in section %s", tmpstr, section);
    }
  }
  
  if(!ps.Compare("r") && !ps.Compare("s") && !ps.Compare("e") )
  {
    log("Point_select should be either [e,r,s] in section %s", section);
  }
  
  int cnt;
  loc_entry dummy;
  CString *lines = explode(the_ini.GetValue(section, "cre_file"),',', cnt);
  if(lines)
  {
    while(cnt--)
    {
      lines[cnt].TrimLeft();
      lines[cnt].TrimRight();
      lines[cnt].MakeUpper();
      if(!creatures.Lookup(lines[cnt], dummy))
      {
        log("Invalid creature reference %s in cre_file of section %s", lines[cnt], section);
      }
    }
    delete [] lines;
  }
}

void CChitemDlg::check_spawn_section(CString section, CString referenced)
{
  if(section.IsEmpty()) return;
  
  if(!the_ini.HasSection(section))
  {
    log("No %s spawn section referenced in %s", section, referenced);
    return;
  }
  check_value(section, "critters", true);
  
  int cnt;
  CString *lines = explode(the_ini.GetValue(section,"critters"), ',', cnt);
  the_ini.AddValid(lines, cnt);
  
  if(lines)
  {
    while(cnt--)
    {
      check_creature_section( lines[cnt], section+":critters");
    }
    delete [] lines;
  }
}

int CChitemDlg::check_section(CString key)
{
  if(!key.Compare("nameless")) return 0;
  if(!key.Compare("namelessvar")) return 0;
  if(!key.Compare("locals")) return 0;
  if(!key.Compare("spawn_main")) return 0;
  
  if(the_ini.IsValid(key)) return 0;
  
  return 1;
}

int CChitemDlg::check_spawnini()
{
  CStringMapString *value = NULL;
  CString key;
  POSITION pos;
  int ret;
  
  ret=0;
  if(pst_compatible_var() && !the_ini.HasSection("nameless"))
  {
    log("No nameless spawn section");
  }
  
  if(the_ini.GetValue("nameless","destare").GetLength())
  {
    CString point = the_ini.GetValue("nameless","point");
    switch(check_point_string(point))
    {
    case -1:
      log("Invalid nameless respawn point:%s", point);
      break;
    case -2:
      break;
    }
    //syntax checking of point
    CString state = the_ini.GetValue("nameless","state");
    int stnum = atoi(state);
    
    if(stnum<0 || stnum>40)
    {
      log("Invalid nameless respawn state:%s", state);
    }
  }
  key = the_ini.GetValue("spawn_main","enter");
  check_spawn_section(key, "spawn_main:enter");
  the_ini.AddValid(key);
  
  int cnt;
  CString *lines = explode(the_ini.GetValue("spawn_main","events"), ',', cnt);
  the_ini.AddValid(lines, cnt);
  
  if(lines)
  {
    while(cnt--)
    {
      check_spawn_section( lines[cnt], "spawn_main:events");
    }
    delete [] lines;
  }
  
  pos=the_ini.GetStartPosition();
  while(pos)
  {
    the_ini.GetNextAssoc(pos, key, value);
    if(check_section(key) && !(chkflg&WARNINGS))
    {
      log("Invalid section %s", key);
    }
  }
  
  return ret;
}

int fill_links(short *links, int from, int count, int area, int max)
{
  int i;
  int ret;
  
  if(!links) return 0; //safeguard
  ret=0;
  if(from<0) return -1; //underflow
  for(i=0;i<count;i++)
  {
    if(from+i>=max) return -1; //overflow
    if(links[from+i]!=-1) ret=1;
    links[from+i]=(short) area;
  }
  return ret;
}

int CChitemDlg::check_map()
{
  int i; //mapcount
  int j; //areacount, arealinkcount
  int k; //linkcount in areas
  int ret;
  int link;
  short *links;
  
  ret=0;
  if(!the_map.mapcount)
  {
    log("No map?");
    return BAD_INDEX;
  }
  for(i=0;i<the_map.mapcount;i++)
  {
    if(the_map.headers[i].width<1 || the_map.headers[i].height<1)
    {
      log("Invalid map dimensions for map #%d",i+1);
      ret|=BAD_INDEX;
    }
    if(check_mos_reference(the_map.headers[i].mos,2) )
    {
      log("Invalid mos resource for map #%d",i+1);
      ret|=BAD_ICONREF;
    }
    if(check_resource(the_map.headers[i].mapicons, false) )
    {
      log("Invalid mapicon resource for map #%d",i+1);
      ret|=BAD_ICONREF;
    }
    k=the_map.headers[i].arealinkcount;
    links=new short[k];
    if(links)
    {
      memset(links,-1,k * sizeof(short) );
    }
    else
    {
      log("Out of memory while allocating links.");
      return -3;
    }
    for(j=0;j<the_map.headers[i].areacount;j++)
    {
      switch(check_reference(the_map.areas[i][j].caption,1,1,100) )
      {
      case 1:
        ret|=BAD_STRREF;
        log("Invalid unidentified name (reference: %d).",the_map.areas[i][j].caption);
        break;
      case 2:
        ret|=BAD_STRREF;
        log("Deleted unidentified name (reference: %d).",the_map.areas[i][j].caption);
        break;
      case 5:
        ret|=BAD_STRREF;
        log("Length constraint violated (reference: %d).",the_map.areas[i][j].caption);
        break;
      }
      switch(check_reference(the_map.areas[i][j].tooltip,1,1,100) )
      {
      case 1:
        ret|=BAD_STRREF;
        log("Invalid unidentified name (reference: %d).",the_map.areas[i][j].tooltip);
        break;
      case 2:
        ret|=BAD_STRREF;
        log("Deleted unidentified name (reference: %d).",the_map.areas[i][j].tooltip);
        break;
      case 5:
        ret|=BAD_STRREF;
        log("Length constraint violated (reference: %d).",the_map.areas[i][j].tooltip);
        break;
      }
      switch(fill_links(links,the_map.areas[i][j].northidx,the_map.areas[i][j].northcnt,j,k))
      {
      case 1:
        log("Duplicate references for the north group in map #%d area #%d.",i+1,j+1);
        ret|=BAD_INDEX;
        break;
      case -1:
        log("Invalid references for the north group in map #%d area #%d.",i+1,j+1);
        ret|=BAD_INDEX;
        break;
      }
      switch(fill_links(links,the_map.areas[i][j].westidx,the_map.areas[i][j].westcnt,j,k))
      {
      case 1:
        log("Duplicate references for the west group in map #%d area #%d.",i+1,j+1);
        ret|=BAD_INDEX;
        break;
      case -1:
        log("Invalid references for the west group in map #%d area #%d.",i+1,j+1);
        ret|=BAD_INDEX;
        break;
      }
      switch(fill_links(links,the_map.areas[i][j].southidx,the_map.areas[i][j].southcnt,j,k))
      {
      case 1:
        log("Duplicate references for the south group in map #%d area #%d.",i+1,j+1);
        ret|=BAD_INDEX;
        break;
      case -1:
        log("Invalid references for the south group in map #%d area #%d.",i+1,j+1);
        ret|=BAD_INDEX;
        break;
      }
      switch(fill_links(links,the_map.areas[i][j].eastidx,the_map.areas[i][j].eastcnt,j,k))
      {
      case 1:
        log("Duplicate references for the east group in map #%d area #%d.",i+1,j+1);
        ret|=BAD_INDEX;
        break;
      case -1:
        log("Invalid references for the east group in map #%d area #%d.",i+1,j+1);
        ret|=BAD_INDEX;
        break;
      }
      
      if(check_area_reference(the_map.areas[i][j].arearesref,1) )
      {
        log("Invalid area reference for map #%d/area entry #%d",i+1,j+1);
        ret|=BAD_RESREF;
      }
    }
    if(links)
    {
      for(j=0;j<the_map.headers[i].arealinkcount;j++)
      {
        if(links[j]==-1)
        {
          log("Unreferenced link #%d in map #%d",j,i+1);
          ret|=BAD_INDEX;
          continue;
        }
        if(!the_map.arealinks[i][j].flags)
        {
          log("Invalid area link flag for map #%d/link #%d used in area entry #%d.",i+1,j,links[j]);
          ret|=BAD_ATTR;
        }
        link=the_map.arealinks[i][j].link;
        if(link<0 || link>=the_map.headers[i].areacount)
        {
          log("Invalid area reference for map #%d/link #%d used in area entry #%d.",i+1,j,links[j]);
          ret|=BAD_INDEX;
        }
      }
      delete [] links;
    }
  }
  return ret;
}

int CChitemDlg::check_songlist()
{
  int ret;
  POSITION pos;
  loc_entry dummy;
  CString2 tmpstr;
  
  if(chkflg&NOMUSCH) return 0;
  changeitemname("SONGLIST or equivalent");
  pos=songlist.GetHeadPosition();
  if(!pos)
  {
    log("No songlist.2da (or equivalent) found.");
    return 2;
  }
  ret=0;
  songlist.GetNext(pos); //skipping reserve
  while(pos)
  {
    tmpstr=songlist.GetNext(pos);
    if(!musiclists.Lookup(tmpstr.b,dummy))
    {
      log("Missing songlist entry: %s/%s",tmpstr.a,tmpstr.b);
      ret=1;
    }
  }
  return ret;
}

int CChitemDlg::check_ascii_strref(CString value, CString row)
{
  if(invalidnumber(value))
  {
    log("Invalid strref #: %s in row %s",value, row);
    return -1;
  }
  switch(check_reference(strtonum(value)) )
  {
  case 1:
    log("Invalid TLK reference %s in row %s", value, row);
    return 1;
  case 2:
    log("Deleted TLK reference %s in row %s", value, row);
    return 2;
  }
  return 0;
}

//features: AP, GA
int CChitemDlg::check_clab(CString table, int features)
{
  loc_entry tmploc;
  C2da da2da;
  int i,j;
  POSITION pos;
  int ret;
  CString tmpstr;
  CString *row;
  //Cspell myspell; //for convenience, don't use this unless needed

  ret = 0;
  if (read_2da(table, da2da)<0)
  {
    log("Missing CLAB file: %s", table);
    return BAD_RESREF;
  }
  int rows = da2da.rows;
  int cols = da2da.cols;

  pos=da2da.data->GetHeadPosition();
  for (i=0;i<rows; i++)
  {
    row = (CString *) da2da.data->GetNext(pos);
    for (j=1;j<cols; j++)
    {
      if (row[j][0]=='*') continue;
      tmpstr = CString(row[j]);
      tmpstr.MakeUpper();
      int type = 0;
      if (tmpstr.Left(3)=="GA_")
      {
        type = 1;
        tmpstr = tmpstr.Mid(3);
      } else if (tmpstr.Left(3)=="AP_")
      {
        type = 2;
        tmpstr = tmpstr.Mid(3);
      } else if (tmpstr.Left(3)=="???")
      {
        if (features)
        {
          type = 3;
          tmpstr = tmpstr.Mid(3);
        }
      }
      if (!type)
      {
        log("Invalid entry: %s in %s", tmpstr, table);
        ret |=BAD_RESREF;
        continue;
      }
      if (!spells.Lookup(tmpstr, tmploc))
      {
        log("Nonexistent spell: %s in %s", tmpstr, table);
        ret |=BAD_RESREF;
        continue;
      }
      changeitemname(tmpstr);
      read_spell(tmpstr, NULL); //using the common spell file, change to local if needed
      ret|=check_spl_extheader();
      switch(type)
      {
      case 1:
        //don't want to check much of this
        break;
      case 2:
        ret|=check_applyspell();
        break;
      case 3:
        //what is this
        break;
      default:;
      }
      changeitemname("KITLIST"); //getting back to the old file
    }
  }
  return ret;
}

int CChitemDlg::check_kits_iwd2()
{
  return 0;
}

int CChitemDlg::check_kits_bg2(bool /*tob*/)
{
  loc_entry tmploc;
  CStringList weaclasses, kitclasses;
  CStringList profs;
  CString rowname,colname, tmpstr;
  POSITION pos, pos2, pos3;
  int i;
  int num, clsnum, cnt;
  int ret;
  
  //checking kitlist
  changeitemname("KITLIST");
  ret=0;
  darefs.Lookup("KITLIST",tmploc);
  if(Read2daColumn(tmploc, kitclasses,1, true)<0) //second column
  {
    log("Cannot read class names.");
    ret=4;
  }
  for(i=2;i<5;i++)
  {
    Read2daColumn(tmploc, profs, i, false);
    if(profs.GetCount()!=kitclasses.GetCount())
    {
      log("Missing entry in column %d",i);
      ret=4;
      break;
    }
    pos=profs.GetHeadPosition();
    pos2=kitclasses.GetHeadPosition();
    profs.GetNext(pos); //skipping RESERVE
    kitclasses.GetNext(pos2);
    while(pos && pos2)
    {
      tmpstr=profs.GetNext(pos);
      rowname=kitclasses.GetNext(pos2);
      if (check_ascii_strref(tmpstr,rowname))
      {
        ret|=BAD_STRREF;
      }
    }
  }
  Read2daColumn(tmploc, profs, 5, true);
  pos=profs.GetHeadPosition();
  pos2=kitclasses.GetHeadPosition();
  profs.GetNext(pos);//skipping RESERVE
  kitclasses.GetNext(pos2);
  while(pos && pos2)
  {
    tmpstr=profs.GetNext(pos);
    rowname=kitclasses.GetNext(pos2);
    if(check_2da_reference(tmpstr,true))
    {
      log("Nonexistent class ability 2da: '%s' for %s",tmpstr, rowname);
      ret=4;
    }
    else
    {
      //deep check
      check_clab(tmpstr, 0);
    }
  }
  Read2daColumn(tmploc, profs, 7, false);
  pos=profs.GetHeadPosition();
  pos2=kitclasses.GetHeadPosition();
  profs.GetNext(pos);
  kitclasses.GetNext(pos2);
  while(pos && pos2)
  {
    tmpstr=profs.GetNext(pos);
    rowname=kitclasses.GetNext(pos2);
    if(invalidnumber(tmpstr))
    {
      log("Invalid unusability number '%s' for %s",tmpstr, rowname);
      ret=BAD_USE;
    }
  }  
  
  Read2daColumn(tmploc, profs, 8, false);
  pos=profs.GetHeadPosition();
  pos2=kitclasses.GetHeadPosition();
  profs.GetNext(pos);
  kitclasses.GetNext(pos2);
  while(pos && pos2)
  {
    tmpstr=profs.GetNext(pos);
    rowname=kitclasses.GetNext(pos2);
    if(invalidnumber(tmpstr))
    {
      log("Invalid class number '%s' for %s",tmpstr, rowname);
      ret=BAD_WEAPROF;
    }
  }  
  
  //last check for kitlist, then switch to weapprof
  Read2daColumn(tmploc, profs, 6, false);
  
  darefs.Lookup("WEAPPROF",tmploc);
  if(Read2daRow(tmploc, weaclasses, 0)<0) //first row
  {
    log("Cannot read class names");
    ret=BAD_WEAPROF;
  }
  else
  {
    pos=profs.GetHeadPosition();
    pos2=kitclasses.GetHeadPosition();
    profs.GetNext(pos);
    kitclasses.GetNext(pos2);
    while(pos && pos2)
    {
      tmpstr=profs.GetNext(pos);
      rowname=kitclasses.GetNext(pos2);
      if(invalidnumber(tmpstr))
      {
        log("Invalid proficiency column number: %s in row %s",tmpstr,rowname);
        ret=BAD_WEAPROF;
        continue;
      }
      num=atoi(tmpstr);
      if(num<0 || num>=weaclasses.GetCount())
      {
        log("Non existent column number: %s in row %s",tmpstr,rowname);
        ret=BAD_WEAPROF;
        continue;
      }
      pos3=weaclasses.FindIndex(num+1);
      if(!pos3 || (tmpstr=weaclasses.GetAt(pos3))!=rowname)
      {
        log("Column name in weapprof.2da doesn't match row name (%s vs. %s)",tmpstr,rowname);
        ret=BAD_WEAPROF;
      }
    }
  }
  
  //checking weapprof
  changeitemname("WEAPPROF");
  darefs.Lookup("WEAPPROF",tmploc);
  Read2daColumn(tmploc, profs, 1, false);
  Read2daColumn(tmploc, kitclasses, 0, true); //recycled list (used for weapon proficiency names)
  pos=profs.GetHeadPosition();
  pos2=kitclasses.GetHeadPosition();
  for(num=0;num<32;num++) 
  {
    tmpstr=profs.GetNext(pos);
    rowname=kitclasses.GetNext(pos2);
    if(!pos || !pos2)
    {
      log("Crippled file");
      break;
    }
    if(num<8) continue; //skipping bg1 residue
    
    if(invalidnumber(tmpstr))
    {
      log("Invalid proficiency ID: %s in row %s",tmpstr,rowname);
      ret=BAD_WEAPROF;
    }
  }
  for(i=2;i<4;i++)
  {
    Read2daColumn(tmploc, profs,i,false);
    pos=profs.GetHeadPosition();
    pos2=kitclasses.GetHeadPosition();
    for(num=0;num<32;num++) 
    {
      tmpstr=profs.GetNext(pos);
      rowname=kitclasses.GetNext(pos2);
      if(!pos || !pos2) break;
      if(num<8) continue; //skipping bg1 residue
      
      if (check_ascii_strref(tmpstr, rowname))
      {
        ret|=BAD_STRREF;
      }
    }
  }
  clsnum=weaclasses.GetCount();
  pos3=weaclasses.FindIndex(4);
  for(i=4;i<clsnum;i++)
  {
    colname=weaclasses.GetNext(pos3);
    Read2daColumn(tmploc,profs,i,false);
    pos=profs.GetHeadPosition();
    pos2=kitclasses.GetHeadPosition();
    for(num=0;num<32;num++) 
    {
      tmpstr=profs.GetNext(pos);
      rowname=kitclasses.GetNext(pos2);
      if(!pos || !pos2)
      {
        log("Crippled file");
        break;
      }
      if(num<8) continue; //skipping bg1 residue
      cnt=strtonum(tmpstr);
      if((!cnt && invalidnumber(tmpstr)) || cnt<0 || cnt>5)
      {
        log("Invalid proficiency count %s for %s/%s",tmpstr, colname,rowname);
        ret=4;
      }
    }    
  }
  
  changeitemname("ALIGNMNT");
  darefs.Lookup("ALIGNMNT",tmploc);
  Read2daColumn(tmploc, profs, 0, true); //file, refs, column, caps
  pos=profs.GetHeadPosition();
  ret|=CompareLabels(profs, weaclasses, "ALIGNMNT","WEAPROFS");
  
  changeitemname("ABCLASRQ");
  darefs.Lookup("ABCLASRQ",tmploc);
  Read2daColumn(tmploc, profs, 0, true);
  pos=profs.GetHeadPosition();
  ret|=CompareLabels(profs, weaclasses, "ABCLASRQ","WEAPROFS");
  
  changeitemname("ABCLSMOD");
  darefs.Lookup("ABCLSMOD",tmploc);
  Read2daColumn(tmploc, profs, 0, true);
  pos=profs.GetHeadPosition();
  ret|=CompareLabels(profs, weaclasses, "ABCLSMOD","WEAPROFS");
  
  changeitemname("ABDCDSRQ");
  darefs.Lookup("ABDCDSRQ",tmploc);
  Read2daColumn(tmploc, profs, 0, true);
  pos=profs.GetHeadPosition();
  ret|=CompareLabels(profs, weaclasses, "ABDCDSRQ","WEAPROFS");
  
  changeitemname("ABDCSCRQ");
  darefs.Lookup("ABDCSCRQ",tmploc);
  Read2daColumn(tmploc, profs, 0, true);
  pos=profs.GetHeadPosition();
  ret|=CompareLabels(profs, weaclasses, "ABDCSCRQ","WEAPROFS");
  
  changeitemname("LUABBR");
  darefs.Lookup("LUABBR",tmploc);
  Read2daColumn(tmploc, profs, 0, true);
  pos=profs.GetHeadPosition();
  ret|=CompareLabels(profs, weaclasses, "LUABBR","WEAPROFS");
  
  changeitemname("DUALCLAS");
  darefs.Lookup("DUALCLAS",tmploc);
  Read2daColumn(tmploc, profs, 0, true);
  pos=profs.GetHeadPosition();
  ret|=CompareLabels(profs, weaclasses, "DUALCLAS","WEAPROFS");
  
  return ret;
}

int CChitemDlg::CompareLabels(CStringList &first, CStringList &second, CString firstname, CString secondname)
{
  CString tmpstr;
  POSITION pos;
  int ret;
  
  ret=0;
  pos=first.GetHeadPosition();
  while(pos)
  {
    tmpstr=first.GetNext(pos);
    if(!second.Find(tmpstr))
    {
      log("Invalid label %s in %s (not listed in %s.2da)",tmpstr,firstname,secondname);
      ret=1;
      continue;
    }
  }
  
  pos=second.FindIndex(4); //weaprofs, start in 4. column
  while(pos)
  {
    tmpstr=second.GetNext(pos);
    if(!first.Find(tmpstr))
    {
      log("Invalid label %s in %s (not listed in %s.2da)",tmpstr,secondname, firstname);
      ret=1;
      continue;
    }
  }
  return ret;
}

int CChitemDlg::check_kits()
{
  if(iwd2_structures()) return check_kits_iwd2();
  
  if(tob_specific()) return check_kits_bg2(1);  //bg2 tob
  
  //check_kits_bg2(0); //soa
  return 0;
}

int CChitemDlg::check_tooltip()
{
  int ret;
  POSITION pos, pos2;
  CStringList resrefs;
  CStringList strrefs[3];
  loc_entry tmploc;
  CString index, value;
  int i;
  
  changeitemname("TOOLTIP");
  darefs.Lookup("TOOLTIP", tmploc);
  Read2daColumn(tmploc,resrefs,0, false);
  for (i=0;i<3;i++)
  {
    Read2daColumn(tmploc,strrefs[i],i+1, false);
  }
  
  pos=resrefs.GetHeadPosition();
  if(!pos)
  {
    log("No statdesc.2da found.");
    return 2;
  }
  
  ret=0;
  while(pos)
  {
    index=resrefs.GetNext(pos);
    if (check_itemref(index,0))
    {
      log("Invalid item reference: '%.8s'",index);
      ret|=BAD_ITEMREF;
    }
  }
  
  for (i=0;i<3;i++)
  {
    if(resrefs.GetCount()!=strrefs[i].GetCount())
    {
      log("File is crippled in column #%d", i+1);
      continue;
    }
    pos2=strrefs[i].GetHeadPosition();
    pos=resrefs.GetHeadPosition();
    while(pos && pos2)
    {
      value=strrefs[i].GetNext(pos2);
      index=resrefs.GetNext(pos);
      if (check_ascii_strref(value,index))
      {
        ret|=BAD_STRREF;
      }
    }
  }
  return ret;
}

int CChitemDlg::check_statdesc() 
{
  int ret;
  POSITION pos, pos2;
  CStringList indices;
  CStringList strrefs;
  loc_entry tmploc;
  CString index, value;
  int expected;
  
  if (chkflg&NOREFCHK) return 0;
  changeitemname("STATDESC");
  darefs.Lookup("STATDESC", tmploc);
  Read2daColumn(tmploc,indices,0, false);
  Read2daColumn(tmploc,strrefs,1, false);
  pos=indices.GetHeadPosition();
  pos2=strrefs.GetHeadPosition();
  if(!pos)
  {
    log("No statdesc.2da found.");
    return 2;
  }
  if(indices.GetCount()!=strrefs.GetCount())
  {
    log("File is crippled");
    return 2;
  }
  ret=0;
  expected=0;
  while(pos && pos2)
  {
    value=strrefs.GetNext(pos2);
    index=indices.GetNext(pos);
    if(atoi(index)!=expected)
    {
      log("Indices are not sequential");
      expected=atoi(index);
    }
    expected++;
    if (check_ascii_strref(value,index))
    {
      ret|=BAD_STRREF;
    }
  }
  return ret;
}

int CChitemDlg::check_spelldesc() 
{
  int ret;
  POSITION pos, pos2;
  loc_entry tmploc;
  CStringList indices;
  CStringList strrefs;
  CString index, value;
  
  changeitemname("SPELDESC");
  darefs.Lookup("SPELDESC", tmploc);
  Read2daColumn(tmploc,indices,0, false);
  Read2daColumn(tmploc,strrefs,1, false);
  pos=indices.GetHeadPosition();
  pos2=strrefs.GetHeadPosition();
  if(!pos)
  {
    log("No speldesc.2da found.");
    return 2;
  }
  if(indices.GetCount()!=strrefs.GetCount())
  {
    log("File is crippled");
    return 2;
  }
  ret=0;
  while(pos && pos2)
  {
    value=strrefs.GetNext(pos2);
    index=indices.GetNext(pos);
    if(check_spell_reference(index))
    {
      log("Index %s is not an existing spell.", index);
      ret|=BAD_RESREF;
    }
    if (check_ascii_strref(value,index))
    {
      ret|=BAD_STRREF;
    }
  }
  return ret;
}

int CChitemDlg::check_moviedesc() 
{
  int ret;
  POSITION pos, pos2;
  loc_entry tmploc;
  CStringList indices;
  CStringList strrefs;
  CString index, value;
  
  changeitemname("MOVIDESC");
  darefs.Lookup("MOVIDESC", tmploc);
  Read2daColumn(tmploc,indices,0, false);
  Read2daColumn(tmploc,strrefs,1, false);
  pos=indices.GetHeadPosition();
  pos2=strrefs.GetHeadPosition();
  if(!pos)
  {
    log("No movidesc.2da found.");
    return 2;
  }
  if(indices.GetCount()!=strrefs.GetCount())
  {
    log("File is crippled");
    return 2;
  }
  ret=0;
  while(pos && pos2)
  {
    value=strrefs.GetNext(pos2);
    index=indices.GetNext(pos);
    index.MakeUpper();
    if(check_movie_reference(index))
    {
      log("Index %s is not an existing movie.", index);
      ret|=BAD_RESREF;
    }
    if (check_ascii_strref(value,index))
    {
      ret|=BAD_STRREF;
    }
  }
  return ret;
}

int CChitemDlg::check_scriptdesc() 
{
  int ret;
  POSITION pos, pos2;
  loc_entry tmploc;
  CStringList indices;
  CStringList strrefs[2];
  CString index, value;
  int i;
  
  changeitemname("SCRPDESC");
  if (!darefs.Lookup("SCRPDESC", tmploc))
  {
    log("No scrpdesc.2da found.");
    return 2;
  }
  Read2daColumn(tmploc,indices,0, false);
  Read2daColumn(tmploc,strrefs[0],1, false);
  Read2daColumn(tmploc,strrefs[1],2, false);
  if(indices.GetCount()!=strrefs[0].GetCount() ||
     indices.GetCount()!=strrefs[1].GetCount())
  {
    log("File is crippled");
    return 2;
  }
  ret=0;
  for (i=0;i<2;i++)
  {
    pos2=strrefs[i].GetHeadPosition();
    pos=indices.GetHeadPosition();
    while(pos && pos2)
    {
      value=strrefs[i].GetNext(pos2);
      index=indices.GetNext(pos);
      index.MakeUpper();
      if(!i && check_scriptref(index, true))
      {
        log("Index %s is not an existing script.", index);
        ret|=BAD_RESREF;
      }
      if (check_ascii_strref(value,index))
      {
        ret|=BAD_STRREF;
      }
    }
  }
  return ret;
}

int CChitemDlg::check_itemexcl()
{
  int ret;
  POSITION pos;
  loc_entry tmploc;
  CStringList indices;
  CString index;

  changeitemname("ITEMEXCL");
  if (!darefs.Lookup("ITEMEXCL", tmploc))
  {
    log("No itemexcl.2da found.");
    return 2;
  }
  Read2daColumn(tmploc,indices,0, false);
  ret=0;
  pos = indices.GetHeadPosition();
  while(pos)
  {
    index=indices.GetNext(pos);
    index.MakeUpper();
    if(check_itemref(index, 0))
    {
      log("Index %s is not an existing item.", index);
      ret|=BAD_RESREF;
    }
  }
  return ret;
}

int CChitemDlg::check_itemanim()
{
  int ret;
  POSITION pos;
  loc_entry tmploc;
  CStringList indices;
  CString index;

  changeitemname("ITEMANIM");
  if (!darefs.Lookup("ITEMANIM", tmploc))
  {
    log("No itemanim.2da found.");
    return 2;
  }
  Read2daColumn(tmploc,indices,0, false);
  ret=0;
  pos = indices.GetHeadPosition();
  while(pos)
  {
    index=indices.GetNext(pos);
    index.MakeUpper();
    if(check_itemref(index, 0))
    {
      log("Index %s is not an existing item.", index);
      ret|=BAD_RESREF;
    }
  }
  return ret;
}

int CChitemDlg::check_itemdial()
{
  int ret;
  POSITION pos, pos2, pos3;
  loc_entry tmploc;
  CStringList indices;
  CStringList strrefs;
  CStringList dialogs;
  CString index, value, dlg;

  changeitemname("ITEMDIAL");
  if (!darefs.Lookup("ITEMDIAL", tmploc))
  {
    log("No itemdial.2da found.");
    return 2;
  }
  Read2daColumn(tmploc,indices,0, false);
  Read2daColumn(tmploc,strrefs,1, false);
  Read2daColumn(tmploc,dialogs,2, false);
  if(indices.GetCount()!=strrefs.GetCount() ||
     indices.GetCount()!=dialogs.GetCount())
  {
    log("File is crippled");
    return 2;
  }
  ret=0;
  pos = indices.GetHeadPosition();
  pos2 = strrefs.GetHeadPosition();
  pos3 = dialogs.GetHeadPosition();
  while(pos)
  {
    index=indices.GetNext(pos);
    index.MakeUpper();
    if(check_itemref(index, 0))
    {
      log("Index %s is not an existing item.", index);
      ret|=BAD_RESREF;
    }
    value=strrefs.GetNext(pos2);
    if (check_ascii_strref(value,index))
    {
      ret|=BAD_STRREF;
    }
    dlg = dialogs.GetNext(pos3);
    dlg.MakeUpper();
    if (check_dialogres(dlg, false) )
    {
      ret|=BAD_RESREF;
      log("Index %s contains an invalid dialog reference (%s).", index, dlg);
    }
  }
  return ret;
}

char *defsummons[]={"MONSUM01","MONSUM02","MONSUM03","MONSUM04",
"ANISUM01","ANISUM02", NULL};

int CChitemDlg::check_summons()
{
  int ret;
  POSITION pos, pos2;
  loc_entry tmploc;
  CStringList indices;
  CStringList summons;
  CString tmpstr;
  int i;
  int logt;
  
  ret = 0;
  i = 0;

  //the blackisle branch has no default 2da's
  if (!has_xpvar())
  {
    while(defsummons[i])
    {
      summons.AddHead(defsummons[i]);
      i++;
    }
  }
  //
  memset(&searchdata,0,sizeof(searchdata));
  searchflags=MF|(MF<<1);
  searchdata.feature=searchdata.feature2=F_SUMMON;  //summon effect with 2da
  logt = logtype;
  logtype = 0;
  process_effects(SCANNING, &summons);
  process_spells(SCANNING, &summons);
  logtype = logt;
  //
  pos2 = summons.GetHeadPosition();
  while(pos2)
  {    
    changeitemname(summons.GetNext(pos2));
    darefs.Lookup(itemname, tmploc);
    Read2daColumn(tmploc, indices, 1, false);
    if (!indices.GetCount())
    {
      log("Nonexistent file.");
      continue;
    }
    pos=indices.GetHeadPosition();
    while(pos)
    {
      tmpstr = indices.GetNext(pos);
      tmpstr.MakeUpper();
      if (!creatures.Lookup(tmpstr, tmploc))
      {
        log("Nonexistent creature entry: %s", tmpstr);
        ret|=BAD_RESREF;
      }
    }
  }
  return ret;
}

int CChitemDlg::check_familiar()
{
  int ret;
  POSITION pos;
  loc_entry tmploc;
  CStringList indices;
  CString tmpstr;
  int i;
  
  ret = 0;
  i = 0;

  changeitemname("FAMILIAR");
  darefs.Lookup(itemname, tmploc);
  Read2daColumn(tmploc, indices, 1, false);
  if (!indices.GetCount())
  {
    //no problem exists only in BGEE
    return 0;
  }
  pos=indices.GetHeadPosition();
  while(pos)
  {
    tmpstr = indices.GetNext(pos);
    tmpstr.MakeUpper();
    if (!creatures.Lookup(tmpstr, tmploc))
    {
      log("Nonexistent creature entry: %s", tmpstr);
      ret|=BAD_RESREF;
    }
  }
  return ret;
}

int CChitemDlg::check_select_spell()
{
  int ret;
  POSITION pos, pos2;
  loc_entry tmploc;
  CStringList indices;
  CStringList books;
  CString tmpstr;
  int i;
  int logt;
  
  ret = 0;
  i = 0;
  memset(&searchdata,0,sizeof(searchdata));
  searchflags=MF|(MF<<1);
  searchdata.feature=searchdata.feature2=F_SELECT;  //select spell effect with 2da
  logt = logtype;
  logtype = 0;
  process_effects(SCANNING, &books);
  process_spells(SCANNING, &books);
  logtype = logt;
  //
  pos2 = books.GetHeadPosition();
  while(pos2)
  {    
    changeitemname(books.GetNext(pos2));
    darefs.Lookup(itemname, tmploc);
    Read2daColumn(tmploc, indices, 1, false);
    if (!indices.GetCount())
    {
      log("Nonexistent file.");
      continue;
    }
    pos=indices.GetHeadPosition();
    while(pos)
    {
      tmpstr = indices.GetNext(pos);
      tmpstr.MakeUpper();
      if (!spells.Lookup(tmpstr, tmploc))
      {
        log("Nonexistent spell entry: %s", tmpstr);
        ret|=BAD_RESREF;
      }
    }
  }
  return ret;
}

int CChitemDlg::check_anisound(CString key)
{
  int ret;
  int i;
  int cnt;
  POSITION pos;
  loc_entry tmploc;
  CStringList snds;
  CString tmpstr;
  CString *refs;

  ret = 0;
  key.MakeUpper();
  changeitemname(key);
  if (!darefs.Lookup(key, tmploc))
  {
    //don't bother with this now
    return 0;
  }
  Read2daColumn(tmploc, snds, 1, true);
  pos=snds.GetHeadPosition();
  while(pos)
  {
    tmpstr = snds.GetNext(pos);
    if (pos) snds.GetNext(pos);   //skip every second line
    if (tmpstr[0]=='*') continue; //skip * resources
    tmpstr.MakeUpper();
    refs = explode(tmpstr,' ', cnt);
    for(i=0;i<cnt;i++)
    {
      if (!sounds.Lookup(refs[i], tmploc))
      {
        log("%s sound resource is missing.", refs[i]);
      }
    }
    if (refs) delete [] refs;
  }  
  return ret;
}

int CChitemDlg::check_anisounds()
{
  return OnAvatars(true);
}

int CChitemDlg::check_treasure()
{
  int ret;
  int i,j;
  int loc;
  POSITION pos;
  loc_entry tmploc;
  C2da da2da;
  CStringList tables;
  CStringList used;
  CString tmpstr;
  CString *row;
  
  ret = 0;
  if (iwd2_structures())
  {
    tables.AddHead("RT_FURY");
    tables.AddHead("RT_NORM");
  }
  else if (has_xpvar())
  {
    tables.AddHead("RNDTRES");
    if (darefs.Lookup("RNDTRES", tmploc))
    {
      Read2daColumn(tmploc, used, 0, true);
    }
    used.AddHead("RNDTRES");
  }
  else
  {
    tables.AddHead("RNDTREAS");
    used.AddHead("RNDTREAS");
  }
  while(tables.GetCount())
  {
    changeitemname(tables.RemoveHead());
    if (read_2da(itemname, da2da)<0)
    {
      log("Missing treasure file");
      ret|=BAD_ITEMREF;
      continue;
    }
    int rows = da2da.rows;
    int cols = da2da.cols;
    pos=da2da.data->GetHeadPosition();
    row = (CString *) da2da.data->GetNext(pos);
    for (i=1;i<rows; i++)
    {
      row = (CString *) da2da.data->GetNext(pos);
      for (j=1;j<cols; j++)
      {
        if (row[j][0]=='*') continue;
        tmpstr = CString(row[j]);
        tmpstr.MakeUpper();
        if (tmpstr[0]=='0')
        {
          //check if all numeric
          continue;
        }
        loc = tmpstr.Find('*');
        if (loc>0) tmpstr=tmpstr.Left(loc);
        if (!items.Lookup(tmpstr, tmploc) )
        {
          CString add = tmpstr.Left(6);
          if (tables.Find(add))
          {
            continue;
          }

          //check if special table
          if (add=="RNDEQU") add = "RNDEQUIP";
          else
          if (add=="RNDMAG") add = "RNDMAGIC";
          else
          if (add=="RNDWEP") add = "RNDWEP";
          else
          if (add=="RNDSCR") add = "RNDSCROL";
          else
          {
          //not a special table
            log("Invalid item '%s' listed in treasure table", tmpstr);
            continue;
          }
          if (used.Find(add))
          {
            //don't add
            continue;
          }
          used.AddHead(add);
          tables.AddHead(add);
        }
      }
    }
  }
  return ret;
}

int CChitemDlg::check_spawngroups() 
{
  int ret;
  int i,j;
  POSITION pos;
  loc_entry tmploc;
  C2da da2da;
  CString tmpstr;
  CString *row;
  
  if (chkflg&NOCRECHK) return 0;
  changeitemname("SPAWNGRP");
  ret = read_2da("SPAWNGRP", da2da);
  int rows = da2da.rows;
  int cols = da2da.cols;
  if (ret<0 || !rows || !cols) return 0;
  
  ret = 0;
  pos=da2da.data->GetHeadPosition();
  row = (CString *) da2da.data->GetNext(pos);
  for (i=1;i<rows; i++)
  {
    row = (CString *) da2da.data->GetNext(pos);
    for (j=1;j<cols; j++)
    {
      if (row[j][0]=='*') continue;
      tmpstr = CString(row[j]);
      tmpstr.MakeUpper();
      if (!creatures.Lookup(tmpstr, tmploc))
      {
        log("Invalid spawngroup entry (no such cre resref): %s in row %s, column %s", row[j], row[0], da2da.collabels[j]);
        ret |= BAD_ITEMREF;
      }
    }
  }
  
  return ret;
}

#define CODE_MIRROR_COUNT 13
CString code_mirror_suffixes[CODE_MIRROR_COUNT]={"a1","a2","a3","a4","a5","a6","a7","a8","a9","sa","sx","ss","ca"};
int cycle_count[CODE_MIRROR_COUNT]={9};
int mandatory_code_mirror_s[CODE_MIRROR_COUNT]={1,1,1,1,1,1,1,1,1,1,1,1,0};

int CChitemDlg::check_ani_code_mirror(CString prefix)
{
  CString resref;
  int i;
  int res;
  int cnt;
  
  cnt=0;
  for(i=0;i<CODE_MIRROR_COUNT;i++)
  {
    resref=prefix+code_mirror_suffixes[i];
    res=check_resource(resref, false);
    if(mandatory_code_mirror_s[i] && res)
    {
      cnt++;
    }
  }
  
  //9 is the number of mandatory animations, update it when mandatory_iwd_ani_s changed
  if(cnt==12) return 1; //completely missing animation
  if(cnt) return -1;    //missing one file
  return 0;
}

int CChitemDlg::check_ani_one_file(CString prefix)
{
  int res, res2;
  
  res=check_resource(prefix, false);
  res2=check_resource(prefix+"e", false);
  if(res) return 1;    //if prefix doesn't exist this is not a single file animation
  if(!res2) return 1;  //nor if an east animation exists
  return 0;
}

#define CODE_SF_COUNT 3
CString code_sf_suffixes[CODE_SF_COUNT]={"g1","g2","g3"};
int need_suffix_two[CODE_SF_COUNT]={1,0,0};
int need_suffix_four[CODE_SF_COUNT]={1,1,0};

int four_files_cycles[CODE_SF_COUNT]={48,24,0};
int four_files_2_cycles[CODE_SF_COUNT]={48,16,0};

int CChitemDlg::check_ani_four_files(CString prefix)
{
  CString resref;
  int i;
  int res, res2;
  Cbam tmp;
  
  for(i=0;i<CODE_SF_COUNT;i++)
  {
    resref=prefix+code_sf_suffixes[i];
    res=check_resource(resref, false);
    res2=check_resource(resref+"e", false);
    if(res!=res2) return -2;
    
    if(!res ^ need_suffix_four[i])
    {
      return 1;
    }
    
    if(four_files_cycles[i])
    {
      resref.MakeUpper();
      if(read_bam_preview(resref,&tmp)) return -2;
      if(tmp.GetCycleCount()!=four_files_cycles[i]) return 1;
    }
  }
  return 0;
}

int CChitemDlg::check_ani_four_files_2(CString prefix)
{
  CString resref;
  int i;
  int res, res2;
  Cbam tmp;
  
  for(i=0;i<CODE_SF_COUNT;i++)
  {
    resref=prefix+code_sf_suffixes[i];
    res=check_resource(resref, false);
    res2=check_resource(resref+"e", false);
    if(res!=res2) return -2;
    
    if(!res ^ need_suffix_four[i])
    {
      return 1;
    }
    if(four_files_2_cycles[i])
    {
      resref.MakeUpper();
      if(read_bam_preview(resref,&tmp)) return -2;
      if(tmp.GetCycleCount()!=four_files_2_cycles[i]) return 1;
    }
  }
  return 0;
}


int CChitemDlg::check_ani_two_files(CString prefix)
{
  int i;
  int res, res2;
  
  for(i=0;i<CODE_SF_COUNT;i++)
  {
    res=check_resource(prefix+code_sf_suffixes[i], false);
    res2=check_resource(prefix+code_sf_suffixes[i]+"e", false);
    if(res!=res2) return -2;
    if(!res ^ need_suffix_two[i])
    {
      return 1;
    }
  }
  return 0;
}

#define CODE_MIRROR2_COUNT 12
CString code_mirror2_suffixes[CODE_MIRROR_COUNT]={"g1","g2","g11","g12","g13","g14","g15","g21","g22","g23","g24","g25","g26"};
int code_mirror_2_cycles[CODE_MIRROR_COUNT]={54, 63, 54, 54, 54, 54, 54, 63, 63, 63, 63, 63, 63};
//int code_mirror_2b_cycles[CODE_MIRROR_COUNT]={54, 55, 54, 54, 54, 54, 54, 55, 55, 55, 55, 55};

int CChitemDlg::check_ani_code_mirror_2(CString prefix)
{
  CString resref;
  int i;
  int res;
  int cnt;
  Cbam tmp;
  
  cnt=0;
  for(i=0;i<CODE_MIRROR2_COUNT;i++)
  {
    resref=prefix+code_mirror2_suffixes[i];
    res=check_resource(resref, false);
    if(res)
    {
      cnt++;
    }
    resref.MakeUpper();
    if(read_bam_preview(resref,&tmp)) return -2;
    if(tmp.GetCycleCount()!=code_mirror_2_cycles[i]) return 1;
  }
  
  if(cnt) return 1;    //missing at least one file
  return 0;
}

int six_files_2_cycles[CODE_SF_COUNT]={24,16,32};

int CChitemDlg::check_ani_six_files_2(CString prefix)
{
  int i;
  int res, res2;
  CString resref;
  Cbam tmp;
  
  for(i=0;i<CODE_SF_COUNT;i++)
  {
    resref=prefix+code_sf_suffixes[i];
    res=check_resource(resref, false);
    res2=check_resource(resref+"e", false);
    if(res) return 1;    //if prefix doesn't exist this is not a single file animation
    if(res2) return 1;   //nor if an east animation is missing
    resref.MakeUpper();
    if(read_bam_preview(resref,&tmp)) return -2;
    if(tmp.GetCycleCount()!=six_files_2_cycles[i]) return 1;
  }
  return 0;
}

#define CODE_TWENTYTWO_COUNT 11
CString code_twentytwo_suffixes[CODE_MIRROR_COUNT]={"a1","a2","a3","a4","a5","a6","ca","g1","w2","sx","sa"};
int mandatory_twentytwo_s[CODE_MIRROR_COUNT]={2,3,2,3,2,3,1,1,1,0,0};

int CChitemDlg::check_ani_twentytwo(CString prefix)
{
  int i;
  int res, res2;
  int cnt;
  bool east;
  bool twohanded; //needs twohanded
  
  cnt=0;
  east=false;
  twohanded=false;
  for(i=0;i<CODE_SF_COUNT;i++)
  {
    res=check_resource(prefix+code_twentytwo_suffixes[i], false);
    res2=check_resource(prefix+code_twentytwo_suffixes[i]+"e", false);
    if (res!=res2) east=true;
    if (mandatory_twentytwo_s[i]==2)
    {
      if(res) twohanded=true;
      else twohanded=false;
    }
    
    if(res)
    {
      switch(mandatory_twentytwo_s[i])
      {
      case 0: case 2:
        break;
      case 1:
        cnt++;
        break;
      case 3:
        if(twohanded)
        {
          cnt++;
        }
        break;
      }
    }
  }
  
  if(cnt) return -1;   //missing only a few files
  if(east) return -2;  //missing an east file
  return 0;
}

int CChitemDlg::check_ani_bird(CString prefix)
{
  int res, res2;
  
  res=check_resource(prefix, false);
  res2=check_resource(prefix+"e", false);
  if(res) return 1;    //if prefix doesn't exist this is not a single file animation
  if(!res2) return 1;  //nor if an east animation exists
  return 0;
}

int six_files_cycles[CODE_SF_COUNT]={16,80,48};

int CChitemDlg::check_ani_six_files(CString prefix)
{
  int i;
  int res, res2;
  CString resref;
  Cbam tmp;
  
  for(i=0;i<CODE_SF_COUNT;i++)
  {
    resref=prefix+code_sf_suffixes[i];
    res=check_resource(resref, false);
    res2=check_resource(resref+"e", false);
    if(res) return 1;    //if prefix doesn't exist this is not a single file animation
    if(res2) return 1;   //nor if an east animation is missing
    resref.MakeUpper();
    if(read_bam_preview(resref,&tmp)) return -2;
    if(tmp.GetCycleCount()!=six_files_cycles[i]) return 1;
  }
  return 0;
}

int CChitemDlg::check_ani_two_files_2(CString prefix)
{
  int i;
  int res, res2;
  
  for(i=0;i<CODE_SF_COUNT;i++)
  {
    res=check_resource(prefix+code_sf_suffixes[i], false);
    res2=check_resource(prefix+code_sf_suffixes[i]+"e", false);
    if(res!=res2) return -2;
    if(!res ^ need_suffix_two[i])
    {
      return 1;
    }
  }
  return 0;
}

#define IWD_ANI_COUNT  13

CString iwd_ani_suffixes[IWD_ANI_COUNT]={"wk","tw","sl","gu","de","gh","sc","sp","ca","sd","a1","a2","a4"};
int mandatory_iwd_ani_s[IWD_ANI_COUNT]={1,1,1,1,1,1,1,0,0,1,1,0,0};

int CChitemDlg::check_ani_two_files_3(CString prefix)
{
  CString resref;
  int i;
  int res, res2;
  int cnt;
  bool east;
  
  cnt=0;
  east=false;
  for(i=0;i<IWD_ANI_COUNT;i++)
  {
    resref=prefix+iwd_ani_suffixes[i];
    res=check_resource(resref, false);
    res2=check_resource(resref+"e", false);
    if(res!=res2)
    {
      east=true;
    }
    if(mandatory_iwd_ani_s[i] && res)
    {
      cnt++;
    }
  }
  
  //9 is the number of mandatory animations, update it when mandatory_iwd_ani_s changed
  //but some animations may contain a1 animation even when they are not iwd animations
  if(cnt>=8) return 1; //completely missing animation or not this type
  if(cnt) return -1;   //missing only a few files
  if(east) return -2;  //missing an east file
  return 0;
}

#define NF_COUNT 27
CString nf_suffixes[NF_COUNT]={"g11","g12","g13","g14","g15","g16","g17","g18","g19",
"g21","g22","g23","g24","g25","g26","g27","g28","g29",
"g31","g32","g33","g34","g35","g36","g37","g38","g39"};

int need_four_frames[NF_COUNT]={1,1,1,1,0,0,0,0,0,1,1,1,1,0,0,0,0,0,1,1,1,1,0,0,0,0,0};

int CChitemDlg::check_ani_four_frames(CString prefix)
{
  CString resref;
  int i;
  int res;
  
  for(i=0;i<NF_COUNT;i++)
  {
    resref=prefix+nf_suffixes[i];
    res=check_resource(resref, false);
    if(!res ^ need_four_frames[i])
    {
      return 1;
    }
  }
  
  return 0;
}

#define NF_STANCE_COUNT 7
int NFStancePrefix[]={1,2,3,4,4,4,5};
int NFCyclePrefix[]= {0,0,0,0,1,2,1}; 

int CChitemDlg::check_ani_nine_frames(CString prefix)
{
  CString resref;
  int i;
  int res;
  int part, cycle;
  
  for(i=0;i<NF_STANCE_COUNT;i++)
  {
    for(part=1;part<10;part++)
    {
      for(cycle=0;cycle<9;cycle++)
      {
        resref.Format("%s%d%d%d%d",prefix, NFStancePrefix[i], part, NFCyclePrefix[i], cycle);
        res=check_resource(resref, false);
        if(res)
        {
          return 1;
        }
      }
    }
  }
  
  return 0;
}

#define PINF_COUNT 21
CString pst_infixes[PINF_COUNT]={"at1","at2","c2s","s2c","cf1","cf2","sf1","sf2","dfb","gup","hit",
"run","sp1","sp2","stc","std","wlk","tk1","tk2","tk3","ms1"};
int pst_ani_1_mandatory_infixes[PINF_COUNT]={0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,1,1,0,0,0,0};
int pst_ani_2_mandatory_infixes[PINF_COUNT]={0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,1,0,0,0,0};
int pst_ani_3_mandatory_infixes[PINF_COUNT]={0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,1,0,0,0,0};
int pst_ani_stand_mandatory_infixes[PINF_COUNT]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0};

int CChitemDlg::check_ani_pst_animation_1(CString prefix)
{
  CString resref;
  int i;
  int res;
  int cnt;
  
  cnt=0;
  for(i=0;i<PINF_COUNT;i++)
  {
    resref=prefix.Left(1)+pst_infixes[i]+prefix.Right(4);
    res=check_resource(resref, false);
    if(pst_ani_1_mandatory_infixes[i] && res)
    {
      cnt++;
    }
  }
  
  if(cnt>2) return 1;
  if(cnt) return -1;
  return 0;
}

int CChitemDlg::check_ani_pst_ghost(CString prefix)
{
  int res, res2;
  
  res=check_resource(prefix, false);
  res2=check_resource(prefix+"e", false);
  if(res) return 1;    //if prefix doesn't exist this is not a single file animation
  if(!res2) return 1;  //nor if an east animation exists
  return 0;
}

int CChitemDlg::check_ani_pst_stand(CString prefix)
{
  CString resref;
  int i;
  int res;
  int cnt;
  
  cnt=0;
  for(i=0;i<PINF_COUNT;i++)
  {
    resref=prefix.Left(1)+pst_infixes[i]+prefix.Right(4);
    res=check_resource(resref, false);
    if(pst_ani_stand_mandatory_infixes[i] && res)
    {
      cnt++;
    }
  }
  
  if(cnt>9) return 1;
  if(cnt) return -1;
  return 0;
}

int CChitemDlg::check_ani_pst_animation_2(CString prefix)
{
  CString resref;
  int i;
  int res;
  int cnt;
  
  cnt=0;
  for(i=0;i<PINF_COUNT;i++)
  {
    resref=prefix.Left(1)+pst_infixes[i]+prefix.Right(4);
    res=check_resource(resref, false);
    if(pst_ani_2_mandatory_infixes[i] && res)
    {
      cnt++;
    }
  }
  
  if(cnt>9) return 1;
  if(cnt) return -1;
  return 0;
}

int CChitemDlg::check_ani_pst_animation_3(CString prefix)
{
  CString resref;
  int i;
  int res;
  int cnt;
  
  cnt=0;
  for(i=0;i<PINF_COUNT;i++)
  {
    resref=prefix.Left(1)+pst_infixes[i]+prefix.Right(4);
    res=check_resource(resref, false);
    if(pst_ani_3_mandatory_infixes[i] && res)
    {
      cnt++;
    }
  }
  
  if(cnt>9) return 1;
  if(cnt) return -1;
  return 0;
}


int CChitemDlg::check_all_types(CString prefix, int except)
{
  int type, res;
  for(type=0;type<21;type++)
  {
    if(type==except) continue;
    switch(type)
    {
    case 0: res=check_ani_code_mirror(prefix); break; //simple bg1 animations
    case 1: res=check_ani_one_file(prefix); break;
    case 2: res=check_ani_four_files(prefix); break;
    case 3: res=check_ani_two_files(prefix); break;
    case 4: res=check_ani_code_mirror_2(prefix); break;
    case 5: res=check_ani_six_files_2(prefix); break;
    case 6: res=check_ani_twentytwo(prefix); break;
    case 7: res=check_ani_bird(prefix); break;
    case 8: res=check_ani_six_files(prefix); break;
    case 9: res=check_ani_two_files_3(prefix); break; //iwd animations
    case 10: res=check_ani_two_files_2(prefix); break;
    case 11: res=check_ani_four_frames(prefix); break; //large animations
    case 12: res=check_ani_nine_frames(prefix); break; //huge animations
    case 14: res=check_ani_four_files_2(prefix); break;
      
    case 56: res=check_ani_pst_animation_1(prefix); break;
    case 57: res=check_ani_pst_ghost(prefix); break;
    case 58: res=check_ani_pst_stand(prefix); break;
    case 59: res=check_ani_pst_animation_2(prefix); break; //std->stc
    case 60: res=check_ani_pst_animation_3(prefix); break; //stc->std
    default: res=-3; break;
    }
    if(!res) return type;
  }
  return -1;
}

int CChitemDlg::check_avatar() 
{
  int ret;
  POSITION pos, pos2, pos3;
  CStringList ids;
  CStringList prefixes;
  CStringList types;
  loc_entry tmploc;
  CString prefix, id;
  int type, res;
  
  changeitemname("AVATARS");
  if(!darefs.Lookup("AVATARS", tmploc))
  {
    MessageBox("There is no avatars.2da (it is a GemRB specific file)!","Warning",MB_ICONEXCLAMATION|MB_OK);
    return 2;
  }
  
  Read2daColumn(tmploc, ids, 0, false);
  Read2daColumn(tmploc, prefixes, 1, false);
  Read2daColumn(tmploc, types, 5, false);
  pos=prefixes.GetHeadPosition();
  pos2=types.GetHeadPosition();
  pos3=ids.GetHeadPosition();
  if(!pos || !pos2 || !pos3 || prefixes.GetCount()!=types.GetCount())
  {
    log("File is crippled");
    return 2;
  }
  ret=0;
  while(pos && pos2 && pos3)
  {
    prefix=prefixes.GetNext(pos);
    type=atoi(types.GetNext(pos2));
    id=ids.GetNext(pos3);
    switch(type)
    {
    case 0: res=check_ani_code_mirror(prefix); break; //simple bg1 animations
    case 1: res=check_ani_one_file(prefix); break;
    case 2: res=check_ani_four_files(prefix); break;
    case 3: res=check_ani_two_files(prefix); break;
    case 4: res=check_ani_code_mirror_2(prefix); break;
    case 5: res=check_ani_six_files_2(prefix); break;
    case 6: res=check_ani_twentytwo(prefix); break;
    case 7: res=check_ani_bird(prefix); break;
    case 8: res=check_ani_six_files(prefix); break;
    case 9: res=check_ani_two_files_3(prefix); break; //iwd animations
    case 10: res=check_ani_two_files_2(prefix); break;
    case 11: res=check_ani_four_frames(prefix); break; //large animations
    case 12: res=check_ani_nine_frames(prefix); break; //huge animations
    case 14: res=check_ani_four_files_2(prefix); break;
      
    case 56: res=check_ani_pst_animation_1(prefix); break;
    case 57: res=check_ani_pst_ghost(prefix); break;
    case 58: res=check_ani_pst_stand(prefix); break;
    case 59: res=check_ani_pst_animation_2(prefix); break; //std->stc
    case 60: res=check_ani_pst_animation_3(prefix); break; //stc->std
    default:
      res=-3; break;
    }
    
    if(res<0) ret=-2;
    switch(res)
    {
    case -3:
      log("Invalid avatar type for %s (%s)",prefix, id);
      break;
    case -2:
      log("Missing east animation file for %s (%s)", prefix, id);
      break;
    case -1:
      log("Missing file for %s (%s)", prefix, id);
      break;
    case 1:
      //this is not an exact error, completely missing animations are fine
      //but if we have a better suggestion, it means avatars.2da is wrong
      log("Invalid animation %s (%s) for type %d.", prefix, id, type);
    }
    if(res)
    {
      type=check_all_types(prefix, type);
      if(type>=0)
      {
        ret=-1; //this means a fixable problem
        log("Suggested type is: %d", type);
      }
    }
  }
  
  return ret;
}

int CChitemDlg::check_feature(long feature, int par1, int par2, const char *resource)
{
  int ret;
  int restype;
  
  if(chkflg&NOFEATCHK) return 0;
  if(feature>=NUM_FEATS)
  {
    log("Effect opcode is larger than allowed, this will crash for sure!");
    return 1;
  }
  switch(feature)
  {
  case F_COLOR: case F_COLORRGB: case F_COLORGLOW:
    if((par2&0xffc0) || ((par2&15)>=7) )
    {
      //iwd has a special case: 255=whole body
      if(has_xpvar() && ((par2&255)==255) ) break;
      log("Color location is invalid: %d",par2&0xffff);
      return 1;
    }
    break;
  case F_NAME:
  case F_DISPLAYSTRING: case F_CANTUSEITEM: case F_NODISPLAYSTR:
    ret=check_reference(par1);
    switch(ret)
    {
    case 1: log("Invalid TLK reference: %d",par1); break;
    case 2: log("Deleted TLK reference: %d",par1); break;
    }
    return ret;
    case F_DEATH:case F_HOLDCREATURE: case F_USEEFFFILE:
      ret=check_ids_targeting(par1, par2);
      if(ret)
      {
        log("Invalid IDS targeting (%d,%d)",par1,par2);
      }
      return ret;
  }
  
  restype=feature_resource(feature);
  switch(restype) {
  case REF_WAV:
    ret = check_soundref(resource, false);
    break;
  case REF_BAM:
    ret = check_resource(resource, false);
    break;
  case REF_ITM:
    ret = check_itemref(resource, 1);
    break;
  case REF_SPL:
    ret = check_spell_reference(resource);
    break;
  case REF_CRE:
    ret = check_creature_reference(resource, true);
    break;
  case REF_VVC:
    //all vvc could be a bam too
    ret = check_vvc_reference(resource, true);
    if (ret) ret = check_resource(resource, false);
    break;
  case REF_MVE: case REF_WBM:
    ret = check_movie_reference(resource);
    break;
  default:
    ret = 0;
  }
  if (ret) {
    log("Missing resource: %s", resource);
  }
  return ret;
}
