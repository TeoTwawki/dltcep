// chitem.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <direct.h>
#include <process.h>
#include <mmsystem.h>
#include "2da.h"
#include "chitem.h"
#include "chitemDlg.h"
#include "zlib.h"
#include "acmsound.h"
#include "options.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define BLOCKSIZE  8192

UINT WM_FINDREPLACE = ::RegisterWindowMessage(FINDMSGSTRING);

//globally used constants
unsigned char xorblock[64]={
  0x88,0xA8,0x8F,0xBA,0x8A,0xD3,0xB9,0xF5,0xED,0xB1,0xCF,0xEA,0xAA,
    0xE4,0xB5,0xFB,0xEB,0x82,0xF9,0x90,0xCA,0xC9,0xB5,0xE7,0xDC,0x8E,
    0xB7,0xAC,0xEE,0xF7,0xE0,0xCA,0x8E,0xEA,0xCA,0x80,0xCE,0xC5,0xAD,
    0xB7,0xC4,0xD0,0x84,0x93,0xD5,0xF0,0xEB,0xC8,0xB4,0x9D,0xCC,0xAF,
    0xA5,0x95,0xBA,0x99,0x87,0xD2,0x9D,0xE3,0x91,0xBA,0x90,0xCA,
};

unsigned short objrefs[NUM_OBJTYPE+1]={
  0,REF_2DA,REF_ACM,REF_ARE,REF_BAM,REF_BCS,REF_BMP,REF_CHU,REF_CRE,REF_DLG,//10
    REF_EFF,REF_GAM,REF_IDS,REF_INI,REF_ITM,REF_MOS,REF_MUS,REF_MVE,REF_PLT,//19
    REF_PRO,REF_SPL,REF_SRC,REF_STO,REF_TIS,REF_VEF,REF_VVC,REF_WAV,REF_WED,//28
    REF_WFX,REF_WMP,//30
};

CString objexts[NUM_OBJTYPE+1]={
  ".*",".2da",".acm",".are",".bam",".bcs",".bmp",".chu",".cre",".dlg",//10
    ".eff",".gam",".ids",".ini",".itm",".mos",".mus",".mve",".plt",".pro",//20
    ".spl",".src",".sto",".tis",".vef",".vvc",".wav",".wed",".wfx",".wmp",//30
};

char tbgext[NUM_OBJTYPE+1]={
  0,'r',0,'a',0,'b',0,'h','c','d',//10
  'e','n',0,0,'-',0,0,0,0,0,'s',  //21
  'p','t',0,0,0,0,0,0,'w',        //30
};

int idstrings[NUM_OBJTYPE+1]={IDS_UNKNOWN,IDS_2DA,IDS_MUSIC,IDS_AREA,  //4
IDS_ANIMATION, IDS_SCRIPT, IDS_BITMAP,IDS_CHU,IDS_CREATURE, IDS_DIALOG,//10
IDS_EFFECT,IDS_GAME,IDS_IDS,IDS_INI,IDS_ITEM,//15
IDS_MOS,IDS_MUS,IDS_MOVIE,IDS_PLT,IDS_PRO,    //20
IDS_SPELL,IDS_SRC,IDS_STORE,IDS_TILESET,IDS_VEF,IDS_VVC,IDS_WAV,IDS_WED,//28
IDS_WFX,IDS_WMP};//30

int menuids[NUM_OBJTYPE+1]={0,ID_EDIT_2DA,0,ID_EDIT_AREA,//4
ID_EDIT_ANIMATION,ID_EDIT_SCRIPT,ID_EDIT_ANIMATION,ID_EDIT_CHUI,ID_EDIT_CREATURE,//9
ID_EDIT_DIALOG, ID_EDIT_EFFECT, ID_EDIT_GAMES,ID_EDIT_IDS,0,ID_EDIT_ITEM,//15
ID_EDIT_GRAPHICS,ID_EDIT_MUSICLIST,0,ID_EDIT_ANIMATION,ID_EDIT_PROJECTILE,//20
ID_EDIT_SPELL,0,ID_EDIT_STORE, ID_EDIT_TILESET, 0, ID_EDIT_VVC, 0, ID_EDIT_AREA,//28
0, ID_EDIT_WORLDMAP,//30
};

char BASED_CODE szFilterKey[] = "chitin.key|chitin.key|All files (*.*)|*.*||";
char BASED_CODE szFilterTbg[] = "Tbg files (*.tbg)|*.tbg|All files (*.*)|*.*||";
char BASED_CODE szFilterWeidu[] = "Dialog source files (*.d)|*.d|All files (*.*)|*.*||";
char BASED_CODE szFilterWeiduAll[] = "Source files (*.d;*.baf)|*.d;*.baf|Dialog source files (*.d)|*.d|Script source files (*.baf)|*.baf|All files (*.*)|*.*||";
char BASED_CODE szFilterBifc[] = "Bifc files (*.bif)|*.bif|All files (*.*)|*.*||";
char BASED_CODE szFilterBif[] = "Biff files (*.bif)|*.bif|All files (*.*)|*.*||";

//decompiled path (in bgfolder)
CString DECOMPILED="script compiler\\decompiled";
//weidu logfile
CString WEIDU_LOG="dltcep.log";
//this is the dialog's self reference (so one can save the dialog at other names)
CString SELF_REFERENCE="********";
//deleted reference in tlk
CString DELETED_REFERENCE="*!*";

CString bg2_slot_names[SLOT_COUNT]={"0-Helmet","1-Armor","2-Shield","3-Gloves","4-L.Ring","5-R.Ring","6-Amulet",
"7-Belt","8-Boots","9-Weapon 1","10-Weapon 2","11-Weapon 3","12-Weapon 4","13-Quiver 1","14-Quiver 2",
"15-Quiver 3","16-Unknown","17-Cloak","18-Quick item 1","19-Quick item 2","20-Quick item 3",
"21-Inventory 1","22-Inventory 2","23-Inventory 3","24-Inventory 4","25-Inventory 5","26-Inventory 6",
"27-Inventory 7","28-Inventory 8","29-Inventory 9","30-Inventory 10","31-Inventory 11","32-Inventory 12",
"33-Inventory 13","34-Inventory 14","35-Inventory 15","36-Inventory 16","37-Fist"};

CString pst_slot_names[PST_SLOT_COUNT]={"0-Earring","1-Armor","2-L.Tattoo","3-Gloves","4-L.Ring","5-R.Ring","6-Eyeball",
"7-R.Tattoo","8-Boots","9-Weapon 1","10-Weapon 2","11-Weapon 3","12-Weapon 4","13-Quiver 1","14-Quiver 2",
"15-Quiver 3","16-Quiver 4","17-Quiver 5","18-Unknown","19-Cloak","20-Quick item 1","21-Quick item 2","22-Quick item 3",
"23-Quick item 4","24-Quick item 5","25-Inventory 1","26-Inventory 2","27-Inventory 3","28-Inventory 4","29-Inventory 5","30-Inventory 6",
"31-Inventory 7","32-Inventory 8","33-Inventory 9","34-Inventory 10","35-Inventory 11","36-Inventory 12",
"37-Inventory 13","38-Inventory 14","39-Inventory 15","40-Inventory 16","41-Inventory 17","42-Inventory 18",
"43-Inventory 19","44-Inventory 20","45-Unknown"};

CString iwd2_slot_names[IWD2_SLOT_COUNT]={"0-Helmet","1-Armor","2-Shield","3-Gloves","4-L.Ring","5-R.Ring","6-Amulet",
"7-Belt","8-Boots","9-Weapon 1","10-Shield 1","11-Weapon 2","12-Shield 2","13-Weapon 3","14-Shield 3","15-Weapon 4","16-Shield 4",
"17-Cloak","18-Quiver 1","19-Quiver 2","20-Quiver 3","21-Fist","22-Quick item 1","23-Quick item 2","24-Quick item 3",
"25-Inventory 1","26-Inventory 2","27-Inventory 3","28-Inventory 4","29-Inventory 5","30-Inventory 6",
"31-Inventory 7","32-Inventory 8","33-Inventory 9","34-Inventory 10","35-Inventory 11","36-Inventory 12",
"37-Inventory 13","38-Inventory 14","39-Inventory 15","40-Inventory 16",
"41-Inventory 17","42-Inventory 18","43-Inventory 19","44-Inventory 20","45-Inventory 21","46-Inventory 22",
"47-Inventory 23","48-Inventory 24","49-Unknown"};

int itvs2h[NUM_ITEMTYPE]={
  1,1,1,1,1,1,1,1,1,1,//9
  1,1,1,1,1,2,1,1,1,1,3,//14
  3,1,1,1,3,2,3,1,2,2,//1e
  1,1,1,1,1,1,1,1,5,5,//28  
  5,5,5,5,5,5,5,5,5,//31
  5,5,5,5,5,5,5,6,  //39
  5,5,5,5,5,5,5,5,  //41
  5,5,5,5,5,5,5,5   //49
};

//inifile
CString setupname;
CString bgfolder;
CString descpath;
CString weidupath;
CString weiduextra;
CString weidudecompiled;
CString tlkfilename;
int logtype;
int tooltips;
int do_progress;
int readonly;
unsigned long chkflg;
unsigned long editflg;
unsigned long optflg;
unsigned long weiduflg;

/// game objects
CString itemname;
CString lasterrormsg;
Citem the_item;
Cstore the_store;
Cproj the_projectile;
Cspell the_spell;
Ceffect the_effect;
Ccreature the_creature;
CVVC the_videocell;
Cscript the_script;
Cdialog the_dialog;
Carea the_area;
Cbam the_bam;
Cgame the_game;
Cmap the_map;
Cmos the_mos;
C2da the_2da;
Cids the_ids;
Cmus the_mus;
Cchui the_chui;
Csrc the_src;
long creature_strrefs[SND_SLOT_COUNT];

//2da, ids
CStringMapInt dial_references;
CStringList exclude_item;
CStringList xplist;
CStringList pro_references;
CStringList pro_titles;
CStringList race_names;
CStringList spawngroup;
CStringMapInt rnditems;             //this is the mapping for ignored items (random treasure)
CStringMapArray store_spell_desc;
CStringList sectype_names;
CStringList school_names;
CString2List songlist;
CString slot_names[IWD2_SLOT_COUNT]; //IWD2 has more slots
CString snd_slots[SND_SLOT_COUNT];
CString action_defs[MAX_ACTION];
CString trigger_defs[MAX_TRIGGER];
CStringMapCStringMapInt idsmaps; //this is a cstring map to a cstringmapint

CColorPicker colordlg;
CItemPicker pickerdlg;

//tlk/chitin
tlk_header tlk_headerinfo;
tlk_entry *tlk_entries;
bool global_changed; //tlk_entries have changed (need save)
int global_date;
key_header key_headerinfo;
membif_entry *bifs;
CString cds[NUM_CD];

CStringMapLocEntry items;//1
CStringMapLocEntry icons;//2
CStringMapLocEntry bitmaps;//3
CStringMapLocEntry creatures;//4
CStringMapLocEntry spells;//5
CStringMapLocEntry stores;//6
CStringMapLocEntry darefs;//7
CStringMapLocEntry idrefs;//8
CStringMapLocEntry dialogs;//9
CStringMapLocEntry scripts;//10
CStringMapLocEntry projects;//11
CStringMapLocEntry effects;//12
CStringMapLocEntry vvcs;//13
CStringMapLocEntry areas;//14
CStringMapLocEntry chuis;//15
CStringMapLocEntry mos;//16
CStringMapLocEntry weds;//17
CStringMapLocEntry tis;//18
CStringMapLocEntry sounds;//19
CStringMapLocEntry games;//20
CStringMapLocEntry wmaps;//21
CStringMapLocEntry musics;//22
CStringMapLocEntry musiclists;//23
CStringMapLocEntry movies;//24
CStringMapLocEntry wfxs;//25
CStringMapLocEntry strings; //26
CStringMapLocEntry paperdolls; //27
CStringMapLocEntry vefs; //28
CStringMapLocEntry inis; //29

CStringListLocEntry d_items;//1
CStringListLocEntry d_icons;//2
CStringListLocEntry d_bitmaps;//3
CStringListLocEntry d_creatures;//4
CStringListLocEntry d_spells;//5
CStringListLocEntry d_stores;//6
CStringListLocEntry d_darefs;//7
CStringListLocEntry d_idrefs;//8
CStringListLocEntry d_dialogs;//9
CStringListLocEntry d_scripts;//10
CStringListLocEntry d_projects;//11
CStringListLocEntry d_effects;//12
CStringListLocEntry d_vvcs;//13
CStringListLocEntry d_areas;//14
CStringListLocEntry d_chuis;//15
CStringListLocEntry d_mos;//16
CStringListLocEntry d_weds;//17
CStringListLocEntry d_tis;//18
CStringListLocEntry d_sounds;//19
CStringListLocEntry d_games;//20
CStringListLocEntry d_wmaps;//21
CStringListLocEntry d_musics;//22
CStringListLocEntry d_musiclists;//23
CStringListLocEntry d_movies;//24
CStringListLocEntry d_wfxs;//25
CStringListLocEntry d_strings; //26
CStringListLocEntry d_paperdolls; //27
CStringListLocEntry d_vefs; //28
CStringListLocEntry d_inis; //29
  
CStringMapLocEntry *resources[NUM_OBJTYPE+1]={0,&darefs,&musics,
&areas, &icons, &scripts,&bitmaps, &chuis, &creatures,
&dialogs, &effects, &games,&idrefs, &inis,&items, &mos,&musiclists,
&movies,&paperdolls,&projects,&spells,&strings,
&stores,&tis,&vefs,&vvcs,&sounds,&weds,&wfxs,&wmaps,
};
CStringListLocEntry *duplicates[NUM_OBJTYPE+1]={0,&d_darefs,&d_musics,
&d_areas, &d_icons, &d_scripts, &d_bitmaps, &d_chuis, &d_creatures,
&d_dialogs, &d_effects, &d_games, &d_idrefs, &d_inis, &d_items, &d_mos,
&d_musiclists,&d_movies, &d_paperdolls, &d_projects, &d_spells, &d_strings,
&d_stores,&d_tis, &d_vefs, &d_vvcs,&d_sounds, &d_weds, &d_wfxs, &d_wmaps,
};

CStringMapInt variables;
CIntMapString journals;

///
/////////////////////////////////////////////////////////////////////////////
// CChitemApp

BEGIN_MESSAGE_MAP(CChitemApp, CWinApp)
//{{AFX_MSG_MAP(CChitemApp)
//}}AFX_MSG
//ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChitemApp construction

CChitemApp::CChitemApp()
{
  tooltips=1;
  do_progress=1;
  logtype=1;
  readonly=1;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CChitemApp object

CChitemApp theApp;
CStringMapGameProps allgameprops;

/////////////////////////////////////////////////////////////////////////////
// CChitemApp initialization

void CChitemApp::read_game_preferences()
{
  gameprops gp;
  CString *games;
  int count;
  CString tmp;
  char *poi;
  
  poi=tmp.GetBuffer(MAX_PATH);
  GetPrivateProfileString("settings","games","",poi,MAX_PATH,m_pszProfileName);
  tmp.ReleaseBuffer(-1);
  games=explode2(tmp,count);
  allgameprops.RemoveAll();
  allgameprops.InitHashTable(get_hash_size(count));
  while(count--)
  {
    gp.checkopt=GetPrivateProfileInt(games[count],"check options",0,m_pszProfileName);
    gp.editopt=GetPrivateProfileInt(games[count],"edit options",0,m_pszProfileName);
    gp.gameopt=GetPrivateProfileInt(games[count],"game options",0,m_pszProfileName);
    poi=gp.bgfolder.GetBuffer(MAX_PATH);
    GetPrivateProfileString(games[count],"game location","",poi,MAX_PATH,m_pszProfileName);
    gp.bgfolder.ReleaseBuffer(-1);
    poi=gp.descpath.GetBuffer(MAX_PATH);
    GetPrivateProfileString(games[count],"external defs","",poi,MAX_PATH,m_pszProfileName);
    gp.descpath.ReleaseBuffer(-1);
    allgameprops.SetAt(games[count],gp);
  }
  delete [] games;
}

int CChitemApp::select_setup(CString setupname)
{
  gameprops gp;

  if(allgameprops.Lookup(setupname, gp))
  {
    bgfolder=gp.bgfolder;
    chkflg=gp.checkopt;
    editflg=gp.editopt;
    optflg=gp.gameopt;
    descpath=gp.descpath;
    return 1;
  }
  return 0;
}

void CChitemApp::save_settings()
{
  POSITION pos;
  CString gname;
  gameprops gp;
  FILE *fpoi;
  
  fpoi=fopen(m_pszProfileName,"wt");
  if(!fpoi) return;
  fprintf(fpoi,
    "[Settings]\n"
    "Current setup=%s\n" //1
    "Logtype=%d\n"       //2
    "Progressbar=%d\n"   //3
    "Readonly=%d\n"      //4
    "Tooltips=%d\n",     //5
    setupname,logtype,do_progress,readonly,tooltips);
  fprintf(fpoi,
    "Check Options=%d\n" //1
    "Edit Options=%d\n" //2
    "Game Options=%d\n"  //3
    "Game Location=%s\n" //4
    "External Defs=%s\n" //5
    "WeiDU Options=%d\n" //6
    "WeiDU Path=%s\n"    //7
    "WeiDU Extra=%s\n"  //8
    "WeiDU Outpath=%s\n", //9
    chkflg,editflg, optflg,bgfolder,descpath,weiduflg,weidupath,weiduextra, weidudecompiled); //1-9
  pos=allgameprops.GetStartPosition();
  fprintf(fpoi,"Games=");
  while(pos)
  {
    allgameprops.GetNextAssoc(pos, gname, gp);
    if(pos) fprintf(fpoi,"%s,",gname);
    else fprintf(fpoi,"%s\n",gname);
  }
  pos=allgameprops.GetStartPosition();
  while(pos)
  {
    allgameprops.GetNextAssoc(pos, gname, gp);
    fprintf(fpoi,"[%s]\n"
      "Check Options=%d\n"
      "Edit Options=%d\n"
      "Game Options=%d\n"
      "Game Location=%s\n"
      "External Defs=%s\n",
      gname,gp.checkopt,gp.editopt,gp.gameopt,gp.bgfolder,gp.descpath);
  }
  fclose(fpoi);
}

CString CChitemApp::MyParseCommandLine(char param)
{
  int getnow=false;

  for (int i = 1; i < __argc; i++)
	{
		CString pszParam(__targv[i]);
    if(getnow) return pszParam;
    if(pszParam[0]=='-' || pszParam[0]=='/')
    {
      switch(pszParam.GetLength())
      {
      case 1:
        continue;
      case 2:
        if(pszParam[1]==param) getnow=true;
        break;
      default:
        if(pszParam[1]==param) return pszParam.Mid(2);
      }
    }
  }
  return "";
}

BOOL CChitemApp::InitInstance()
{
  char inifile[MAX_PATH];
  
  m_bbmp.LoadBitmap(IDB_GETFILES);
  _getcwd(inifile,MAX_PATH-20);
  m_defpath=inifile;
  strcat(inifile,"\\chitem.ini");
  free((void*)m_pszProfileName);
  m_pszProfileName=_tcsdup(_T(inifile));
  
  read_game_preferences();
  setupname=MyParseCommandLine('s');
  if(!setupname.GetLength() || !select_setup(setupname) )
  {
    GetPrivateProfileString("settings","current setup","",inifile,MAX_PATH,m_pszProfileName);
    inifile[MAX_PATH-1]=0;
    setupname=inifile;
    
    GetPrivateProfileString("settings","game location","",inifile,MAX_PATH,m_pszProfileName);//path
    inifile[MAX_PATH-1]=0;
    bgfolder=inifile;
    
    GetPrivateProfileString("settings","external defs","",inifile,MAX_PATH,m_pszProfileName);//path
    inifile[MAX_PATH-1]=0;
    descpath=inifile;
    
    chkflg=GetPrivateProfileInt("settings","check options",0,m_pszProfileName);
    editflg=GetPrivateProfileInt("settings","edit options",0,m_pszProfileName);
    optflg=GetPrivateProfileInt("settings","game options",0,m_pszProfileName);
  }
  tooltips=!!GetPrivateProfileInt("settings","tooltips",1,m_pszProfileName);
  logtype=GetPrivateProfileInt("settings","logtype",1,m_pszProfileName);
  do_progress=!!GetPrivateProfileInt("settings","progressbar",1,m_pszProfileName);
  readonly=!!GetPrivateProfileInt("settings","readonly",1,m_pszProfileName);
  weiduflg=GetPrivateProfileInt("settings","weidu options",0,m_pszProfileName);
  
  GetPrivateProfileString("settings","weidu path","",inifile,MAX_PATH,m_pszProfileName);//path
  inifile[MAX_PATH-1]=0;
  weidupath=inifile;
  
  GetPrivateProfileString("settings","weidu extra","",inifile,MAX_PATH,m_pszProfileName); //not a path
  inifile[MAX_PATH-1]=0;
  weiduextra=inifile;

  GetPrivateProfileString("settings","weidu outpath","",inifile,MAX_PATH,m_pszProfileName); //partial path
  inifile[MAX_PATH-1]=0;
  weidudecompiled=inifile;
  if(weidudecompiled.IsEmpty()) weidudecompiled=DECOMPILED;
  LoadStdProfileSettings( );  //recent files  

//  SetDialogBkColor(RGB(180,192,192),RGB(32,32,32) ); //sets the background/text color of the forms
#ifdef _AFXDLL
  Enable3dControls();			// Call this when using MFC in a shared DLL
#else
  Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
//  AfxEnableControlContainer();
  AfxInitRichEdit();
//  LoadAccelerators(theApp.m_hInstance,IDR_SHORTCUT);

  CChitemDlg dlg;
  m_pMainWnd = &dlg;
  dlg.DoModal();
  return FALSE;
}

int nop()
{
  return 0;
}

//returns the first prime after num for hash table size
int incrementTable[] =
{10,  2, 10,  2,  4,  2,  4,  6,  2,  6,
4,  2,  4,  6,  6,  2,  6,  4,  2,  6,
4,  6,  8,  4,  2,  4,  2,  4,  8,  6,
4,  6,  2,  4,  6,  2,  6,  6,  4,  2,
4,  6,  2,  6,  4,  2,  4,  2,  4,  2,
2};

unsigned long get_hash_size(unsigned long num)
{
  unsigned long p;
  unsigned long cx;
  long bp;
  unsigned long dx;
  unsigned long axbx;
  
  p=num|1;
  if(p<17) return 17;
  while(p<0xffffffff)
  {
    cx=3;
    bp=51;
    do
    {
      dx=p%cx;
      axbx=p/cx;
      if(!dx) break;
      if(axbx<cx) return p;
      if (--bp < 0)
      {
        bp = 47;
      }
      cx+=incrementTable[bp];
    }
    while(1);
    p+=2;
  }
  return 17;
}

int strtonum(CString str)
{
  int len;
  
  len=str.GetLength()-2;
  if((len>0) && (str[0]=='0') && (str[1]=='x') )
  {
    return (int) strtoul(str.Right(len),0,16);
  }
  else return atoi(str);
}

bool invalidnumber(CString tmpstr)
{
  if(strtonum(tmpstr)!=0) return false;
  if((tmpstr.GetLength()==1) && (tmpstr[0]=='0')) return false;
  if(tmpstr.GetLength()>2)
  {
    if(tmpstr[0]=='0' && tmpstr[1]=='x') return false;
  }
  return true;
}

//this code will break up lines stuck together, if possible
CString consolidate(char *poi, int textlength)
{
  int space, crlf;
  int bracket, neednewline;
  int newp, oldp;
  CString tmpstr;
  char *newpoi;

  newpoi=tmpstr.GetBufferSetLength(textlength*2);
  newp=0;
  crlf=0;   //eat crlf by default
  space=0;  //eat spaces by default
  bracket=0;
  neednewline=0;
  for(oldp=0;oldp<textlength;oldp++)
  {
    switch(poi[oldp])
    {
    case ')':
      newpoi[newp++]=poi[oldp];
      if(bracket)
      {
        bracket--;
        if(!bracket) neednewline=1;
      }
      break;
    case '(':
      newpoi[newp++]=poi[oldp];
      bracket++;
      break;
    case '\"':
      newpoi[newp++]=poi[oldp];
      if(editflg&EATSPACE)
      {
        space=!space; //toggle space, don't eat spaces in strings
      }
      break;
    case ' ':
      if(space)
      {
        newpoi[newp++]=poi[oldp];
      }
      break;
    case '\r': case '\n':
      if(crlf)
      {
        newpoi[newp++]='\n';
        crlf=0;
        space=0; //eat spaces after crlf, despite of a possibly open quote
      }
      bracket=0;
      neednewline=0;
      break;
    default:
      if(neednewline)
      {
        newpoi[newp++]='\n';       
        neednewline=0;
      }
      //don't do tolower here, we might want to edit dialogs, and users like caps
      newpoi[newp++]=poi[oldp];
      crlf=1; //allow one crlf now
      break;
    }
  }
  newpoi[newp]=0;
  tmpstr.ReleaseBuffer(newp);
  return tmpstr;
}

int WriteString(FILE *fpoi, CString mystring)
{
  if(fprintf(fpoi,"%s",mystring) !=mystring.GetLength()) return -1;
  return 0;
}

int member_array(int member, int *array)
{
  int i;
  
  i=0;
  while(array[i]!=-1)
  {
    if(member==array[i]) return i;
    i++;
  }
  return -1;
}

bool checkfile(CString fname, CString magic)
{
  int fhandle;
  bool ret;
  char tmp[4];
  
  if(magic.GetLength()>4) abort();
  fhandle=open(fname,O_BINARY|O_RDONLY);
  if(fhandle<1) return false;
  memset(tmp,0,sizeof(tmp));
  read(fhandle,tmp,sizeof(tmp));
  ret=!memcmp(tmp,magic,magic.GetLength());
  close(fhandle);
  return ret;
}

bool file_exists(CString filename)
{
  struct _stat the_stat;
  int x;
  
  the_stat.st_mode=0;
  x=_stat(filename, &the_stat);
  if(the_stat.st_mode&_S_IFDIR) return false; //it is a directory
  return x!=-1;
}

long file_length(CString filename)
{
  struct _stat the_stat;
  int x;
  
  the_stat.st_mode=0;
  x=_stat(filename, &the_stat);
  if(x==-1) return -1;
  return(the_stat.st_size); //it is a directory
}

long file_date(CString filename)
{
  struct _stat the_stat;
  int x;
  
  the_stat.st_mode=0;
  x=_stat(filename, &the_stat);
  if(x==-1) return -1;
  return(the_stat.st_mtime); //it is a directory
}

// windows is fucked up!
// some versions can't handle executable names in apostrophe, but they let you specify
// spaces in pathnames.
// simulating the system() function, breaking down syscommand into an argv structure

#define MAX_PAR 20

int my_system(CString syscommand)
{
  CString cmd;
  int maxlen;
  char *poi;
  char *argv[MAX_PAR];
  int ret;
  int apo, space;
  int argc;

  maxlen=syscommand.GetLength();
  poi=syscommand.GetBuffer(0);
  apo=0;
  space=1;
  argc=0;
  while(*poi)
  {
    switch(*poi)
    {
    case ' ':
      if(!apo)
      {
        *poi=0;
        space=1;
      }
      break;
    case '"':
      if(!argc || (apo==argc) )
      {
        if(apo)
        {
          *poi=0;
        }
        apo=!apo;
        break;
      }
      apo=!apo;
    default:
      if(space)
      {
        argv[argc++]=poi;
        space=0;
      }
    }
    poi++;
  }
  argv[argc]=NULL;
  //even more hacks for this damn stuff
  cmd=argv[0];
  argv[0]="DLTCEP_enhanced_WeiDU";
  ret=spawnv(_P_WAIT, cmd, argv);
  return ret;
}

bool dir_exists(CString filename)
{
  struct _stat the_stat;
  int x;
  
  the_stat.st_mode=0;
  x=_stat(filename, &the_stat);
  if(x) return false;
  if(the_stat.st_mode&_S_IFDIR) return true;
  return false;
}

//extracts a bif from a cbf
int extract_from_cbf(CString key, CString ext, int finput, int override, int &maxlen)
{
  sav_entry saventry;
  CString filename;
  CString tmpstr;
  char *poi1, *poi2;
  int fhandle;
  long oflg;
  int ret;
  int flg;

  if(finput<1)
  {
    return -1;
  }
  if(!maxlen) return 1; //end
  if(maxlen<sizeof(oflg)) return -2;

  if(read(finput,&oflg,sizeof(oflg) )!=sizeof(oflg) )
  {
    return -2;
  }
  if(oflg>200) return -2;
  poi1=saventry.filename.GetBufferSetLength(oflg+1);
  if(read(finput,poi1,oflg)!=oflg)
  {
    return -2;
  }
  saventry.filename.ReleaseBuffer(-1);

  if(read(finput, &saventry.uncompressed, sizeof(long)*2 )!=sizeof(long)*2 )
  {
    return -2;
  }
  maxlen-=oflg+sizeof(oflg)+2*sizeof(long)+saventry.compressed;

  itemname=saventry.filename.Left(saventry.filename.Find('.'));
  if(!key.IsEmpty() && (itemname.Find(key,0)==-1) )
  {
    lseek(finput,saventry.compressed,SEEK_CUR);
    return 2; //no match
  }
  if(!ext.IsEmpty() && (saventry.filename.Right(4).CompareNoCase(ext)) )
  {
    lseek(finput,saventry.compressed,SEEK_CUR);
    return 2; //no match
  }

  poi1=new char[saventry.compressed];
  poi2=new char[saventry.uncompressed];

  filename=makeitemname(saventry.filename.Right(4),2);
  if(override) oflg=O_BINARY|O_RDWR|O_TRUNC|O_CREAT;
  else oflg=O_BINARY|O_RDWR|O_EXCL|O_CREAT;

  fhandle=open(filename,oflg,S_IWRITE|S_IREAD);
  if(fhandle<1)
  {
    ret=2; //overwrite not allowed
    lseek(finput,saventry.compressed,SEEK_CUR); //we skip the file in this case
    goto endofquest;
  }
  if(!poi1 || !poi2)
  {
    ret=-3;
    goto endofquest;
  }
  if(read(finput,poi1,saventry.compressed)!=saventry.compressed)
  {
    ret=-2;
    goto endofquest;
  }
  oflg=saventry.uncompressed;
  flg=uncompress((BYTE *) poi2,(unsigned long *) &oflg, (BYTE *) poi1, saventry.compressed);
  if(oflg!=saventry.uncompressed)
  {
    flg=1;
  }
  if(write(fhandle,poi2,oflg)!=oflg)
  {
    ret=-4;
    goto endofquest;
  }
  if(flg) ret=3; //some problem while decompressing
  else ret=0;
endofquest:
  if(fhandle>0) close(fhandle);
  if(poi1) delete [] poi1;
  if(poi2) delete [] poi2;
  return ret;
}

int locate_bif(loc_entry &fileloc)
{
  CString cbfname;
  int i;
  int maxlen;
  int fhandle;
  char header[8];

  if(fileloc.cdloc)
  {
    return open(cds[fileloc.cdloc]+fileloc.bifname,O_BINARY|O_RDONLY);
  }
  for(i=0;i<NUM_CD;i++)
  {
    if(cds[i].IsEmpty() ) continue;
    fhandle=open(cds[i]+fileloc.bifname,O_BINARY|O_RDONLY);
    if(fhandle>0)
    {
      fileloc.cdloc=(unsigned short) i;
      return fhandle;
    }
    //decompressing IWD cbf on demand
    cbfname=fileloc.bifname;
    cbfname.Replace(".bif",".cbf");
    if(file_exists(cds[i]+cbfname) )
    {      
      fhandle=open(cds[i]+cbfname,O_RDONLY|O_BINARY);
      if(fhandle<1) continue;
      if(read(fhandle,&header,sizeof(header))!=sizeof(header))
      {
        close(fhandle);
        return 0;
      }
      maxlen=filelength(fhandle)-sizeof(header);
      i=extract_from_cbf("",".bif",fhandle,0,maxlen); //don't overwrite
      close(fhandle);
      return open(bgfolder+fileloc.bifname,O_BINARY|O_RDONLY);
    }
  }
  return 0;
}

#define MAX_BUFFSIZE  65536l

//copying size bytes from input to output, alternatively decrypting
//relaxed memory requirements (copying in 64k blocks)
int copy_file(int finput, int fhandle, int size, int decrypt)
{
  //copies data from finput to fhandle
  char *tmpdata;
  int offset;
  int i;
  int buffsize;

  if(size<1) return -4;
  buffsize=min(MAX_BUFFSIZE,size);
  tmpdata=new char[buffsize];
  if(!tmpdata)
  {
    return -3; //out of memory
  }
  while(size)
  {
    if(read(finput,tmpdata,buffsize)!=buffsize)
    {
      delete tmpdata;
      return -2;
    }
    //
    //here we could check on compression and encryption
    //
    offset=0;
    switch(decrypt)
    {
    case 1: //decrypt first block conditionally
      if(*((unsigned short *) tmpdata)==0xffff)
      {
        size-=2;
        buffsize-=2;
        offset=2;
        decrypt=2; //flipping this switch off for the following blocks
      }
      else break; //no need of decryption
    case 2:
      //decryption with 64 byte key
      for(i=0;i<buffsize;i++)
      {
        tmpdata[offset+i]^=xorblock[i&63];
      }
    }
    if(write(fhandle,tmpdata+offset,buffsize)!=buffsize)
    {
      delete tmpdata;
      return -1;
    }
    size-=buffsize;
    buffsize=min(MAX_BUFFSIZE,size); //next buffsize
  }
  delete tmpdata;
  return 0;
}

#define UNCOMPRESSED 0
#define COMPRESSED   1

int get_next_block(int fhandle, BYTE *&decompressed, int &actoffset, int offset)
{
  DWORD cheader[2]; 
  BYTE *compressed;
  int ret;

  if(decompressed)
  {
    delete [] decompressed;
    decompressed=NULL;
  }
  if(read(fhandle,cheader,8)!=8)
  {
    return -2;
  }
  actoffset+=cheader[UNCOMPRESSED];
  if(actoffset<offset)
  {
    lseek(fhandle, cheader[COMPRESSED], SEEK_CUR);
    return cheader[UNCOMPRESSED];
  }
  compressed=new BYTE [cheader[COMPRESSED]];
  decompressed=new BYTE [cheader[UNCOMPRESSED]];
  if(!compressed || !decompressed)
  {
    ret=-3;
    goto end_of_quest;
  }
  if(read(fhandle, compressed, cheader[COMPRESSED])!=(long) cheader[COMPRESSED])
  {
    ret=-2;
    goto end_of_quest;
  }
  ret=uncompress(decompressed, cheader+UNCOMPRESSED, compressed, cheader[COMPRESSED]);
  if(ret!=Z_OK)
  {
    ret=-2;
    goto end_of_quest;
  }
  delete [] compressed;
  return cheader[UNCOMPRESSED];
end_of_quest:
  if(compressed) delete [] compressed;
  if(decompressed) delete [] decompressed;
  return ret;
}

struct BIFC_HEADER
{
  char signature[4];
  char version[4];
  long origlen;
};

int decompress_bif(CString bifname, CString cdpath) //this must be a cd bifname
{
  BIFC_HEADER header;
  BYTE *decompressed=NULL;
  int fhandle1, fhandle2;
  int ret;
  int actoffset, maxlen, bufflen;

  if(cdpath==bgfolder) return -99;
  fhandle1=open(cdpath+bifname,O_RDONLY|O_BINARY);
  if(fhandle1<1) return -2;
  fhandle2=open(bgfolder+bifname,O_RDWR|O_CREAT|O_EXCL,S_IREAD|S_IWRITE);
  if(fhandle2<1)
  {
    close(fhandle1);
    return -2;
  }
  read(fhandle1,&header,sizeof(BIFC_HEADER));
  if(memcmp(header.signature,"BIFC",4) )
  {
    ret=-1;
    goto end_of_quest;
  }
  if(header.version[0]!='V')
  {
    ret=-1;
    goto end_of_quest;
  }
  maxlen=filelength(fhandle1);
  actoffset=0;
  ret=0;
  do
  {
    bufflen=get_next_block(fhandle1, decompressed,actoffset, 0);
    if(bufflen<0)
    {
      ret=-1;
      goto end_of_quest;
    }
    header.origlen-=bufflen;
    if(write(fhandle2,decompressed,bufflen)!=bufflen)
    {
      ret=-3;
      goto end_of_quest;
    }
  }
  while(header.origlen>0);
  if(header.origlen)
  {
    ret=-3;
  }
end_of_quest:
  close(fhandle1);
  close(fhandle2);
  if(decompressed) delete [] decompressed;
  return ret;
}

int write_tis_header(int fhandle, loc_entry fileloc)
{
  tis_header tisheader;

  memcpy(tisheader.filetype,"TIS V1  ",8);
  tisheader.offset=sizeof(tisheader);
  tisheader.pixelsize=64;
  //palette + tiledata
  tisheader.tilelength=256*sizeof(COLORREF)+tisheader.pixelsize*tisheader.pixelsize;
  tisheader.numtiles=fileloc.size/tisheader.tilelength;
  return write(fhandle,&tisheader,sizeof(tisheader))==sizeof(tisheader);
}

int locate_file_compressed(loc_entry &fileloc, int fhandle, long origlen)
{
  bif_header headerinfo;
  bif_entry entryinfo;
  tis_entry tileinfo;
  BYTE *decompressed=NULL;
  //current uncompressed offset/needed uncompressed offset
  int bufflen;
  int startoffset, actoffset, offset;
  //minbuff shows the useful part of the uncompressed buffer, mylen is its length
  int minbuff, mylen, remaining;
  CString filename;
  int fhandle2;
  //compressed/uncompressed sizes and buffers
  int ret;
  int index;
  int needed;

  fhandle2=0;
  lseek(fhandle,12,SEEK_SET);
  startoffset=actoffset=0;
  bufflen=get_next_block(fhandle, decompressed,actoffset, 0);
  if(bufflen<0)
  {
    ret=-1;
    goto end_of_quest;
  }
  memcpy(&headerinfo,decompressed,sizeof(bif_header));
  if(headerinfo.version[0]!='V')
  {
    ret=-1;
    goto end_of_quest;
  }
  if(memcmp(headerinfo.signature,"BIFF",4) )
  {
    ret=-1;
    goto end_of_quest;
  }
  if(fileloc.index&TIS_IDX_MASK) //tileset!!!
  {
    index=((fileloc.index&TIS_IDX_MASK)>>14); //always non-zero with a tileset
    if(!index || (index>headerinfo.tile_entries)) //not >=, it starts from 1
    {
      ret=-1;
      goto end_of_quest;
    }
    //offset is the beginning of the tiledata in the uncompressed stream
    //no need to subtract 1 from index (dunno why)
    //now i had to subtract 1 (for some iwd2/iwd files it worked at least)
    //headerinfo.file_offset == sizeof(tileinfo)
    offset=headerinfo.file_offset+headerinfo.file_entries*sizeof(entryinfo)+(index-1)*sizeof(tileinfo);
  }
  else
  {
    if(fileloc.index>=headerinfo.file_entries)
    {
      ret=-1;
      goto end_of_quest;
    }
    //offset is the beginning of the file in the uncompressed stream
    
    index=0; //always zero with a normal file
    offset=headerinfo.file_offset+fileloc.index*sizeof(entryinfo);
  }
  if(offset>origlen)
  {
    ret=-1;
    goto end_of_quest;
  }
  //this part seeks the beginning of the file
  while(actoffset<offset)
  {
    startoffset=actoffset;
    bufflen=get_next_block(fhandle,decompressed, actoffset, offset);
    if(bufflen<0)
    {
      ret=-1;
      goto end_of_quest;
    }
  }
  //this part reads the entry info
  minbuff=offset-startoffset;
  if(index) needed=sizeof(tileinfo); //tileset
  else needed=sizeof(entryinfo);     //normal file
  mylen=min(needed, bufflen-minbuff);
  if(index) memcpy(&tileinfo, decompressed+minbuff, mylen);
  else memcpy(&entryinfo,decompressed+minbuff, mylen);
  if(mylen!=needed )
  {
    startoffset=actoffset;
    bufflen=get_next_block(fhandle,decompressed, actoffset, offset);
    if(bufflen<0)
    {
      ret=-1;
      goto end_of_quest;
    }
    remaining=needed-mylen;
    if(index) memcpy(((BYTE *) &tileinfo)+mylen,decompressed,remaining);
    else memcpy(((BYTE *) &entryinfo)+mylen,decompressed,remaining);
  }
  if(index) //tileset
  {
    entryinfo.offset=tileinfo.offset;
    entryinfo.res_loc=tileinfo.res_loc;
    entryinfo.restype=tileinfo.restype;
    entryinfo.size=tileinfo.numtiles*tileinfo.size;
    entryinfo.unused=(short) tileinfo.size;          //used to be 5120, always
  }
  offset=entryinfo.offset;
  if(offset>origlen)
  {
    ret=-1;
    goto end_of_quest;
  }
  while(actoffset<offset)
  {
    startoffset=actoffset;
    bufflen=get_next_block(fhandle, decompressed, actoffset, offset);
    if(bufflen<0)
    {
      ret=-1;
      goto end_of_quest;
    }
  }
  //begin extraction of file from compressed bif
  //
  minbuff=offset-startoffset;
  remaining=fileloc.size=entryinfo.size;
  mylen=min(remaining, bufflen-minbuff);
  filename="override\\"+fileloc.resref; //khm
  fhandle2=open(bgfolder+filename,O_RDWR|O_EXCL|O_CREAT,S_IREAD|S_IWRITE);
  if(fhandle2<1)
  {
    ret=-2;
    goto end_of_quest;
  }
  fileloc.bifname=filename;
  fileloc.bifindex=0xffff;
  if(index)
  {
    write_tis_header(fhandle2, fileloc);
  }
  do
  {
    write(fhandle2,decompressed+minbuff, mylen);
    remaining-=mylen;
    if(remaining)
    {
      bufflen=get_next_block(fhandle, decompressed, actoffset, 0);
      minbuff=0;
      mylen=min(remaining, bufflen);
      if(bufflen<0)
      {
        ret=-1;
        goto end_of_quest;
      }
    }
  }
  while(remaining);
  //close bif, return temporary file (uncompressed to override)
  if(decompressed) delete [] decompressed;
  close(fhandle);
  lseek(fhandle2,0,SEEK_SET);
  return fhandle2;
end_of_quest:
  if(fhandle2) close(fhandle2);
  if(decompressed) delete [] decompressed;
  close(fhandle); 
  return ret;
}

int locate_file(loc_entry &fileloc, int ignoreoverride)
{
  bif_header headerinfo;
  bif_entry entryinfo;
  tis_entry tileinfo;
  long offset;
  int index;
  int fhandle;
  
  if(ignoreoverride)
  {
    if(fileloc.bifindex==0xffff) return 0;
    fhandle=open(bgfolder+bifs[fileloc.bifindex].bifname,O_BINARY|O_RDONLY);
  }
  else fhandle=open(bgfolder+fileloc.bifname,O_BINARY|O_RDONLY);
  if(fhandle<1)
  {
    fhandle=locate_bif(fileloc);
    if(fhandle<1) return fhandle;
  }
  if((fileloc.index>=0) && ((ignoreoverride&LF_IGNOREOVERRIDE) || ( (fileloc.bifindex!=0xffff) && (!bifs[fileloc.bifindex].bifname.CompareNoCase(fileloc.bifname) ) ) ) ) //seeking in bif
  {
    if(read(fhandle,&headerinfo,sizeof(headerinfo))!=sizeof(headerinfo))
    {
      return -1;
    }
    if(headerinfo.version[0]!='V')
    {
      return -1;
    }
    if(memcmp(headerinfo.signature,"BIFF",4) )
    {
      if(memcmp(headerinfo.signature,"BIFC",4) ) return -1;
      if(ignoreoverride&LF_IGNORECBF) return -1;
      //file_entries holds the uncompressed length
      return locate_file_compressed(fileloc,fhandle,headerinfo.file_entries);
    }
    if(fileloc.index&TIS_IDX_MASK) //tileset!!!
    {
      if(fileloc.index&FILE_IDX_MASK) return -1;
      index=((fileloc.index&TIS_IDX_MASK)>>14);
      if(index>headerinfo.tile_entries) //not >= !!!
      {
        return -1;
      }
      //no need to subtract 1 from index (dunno why)
      //heh, we need to decrease it
      offset=headerinfo.file_offset+headerinfo.file_entries*sizeof(entryinfo)+(index-1)*sizeof(tileinfo);
      if(lseek(fhandle,offset,SEEK_SET )!=offset)
      {
        return -1; //bif is corrupt
      }
      if(read(fhandle,&tileinfo,sizeof(tileinfo))!=sizeof(tileinfo) )
      {
        return -1;
      }
      entryinfo.offset=tileinfo.offset;
      entryinfo.res_loc=tileinfo.res_loc;
      entryinfo.restype=tileinfo.restype;
      if(tileinfo.restype!=REF_TIS) return -1;       //only tilesets
      if(!(editflg&RESLOC) ) //don't be so pickish in case of weidu
      {
        if((entryinfo.res_loc&TIS_IDX_MASK)!=index<<14)
        {
          return -1;
        }
      }
      entryinfo.size=tileinfo.numtiles*tileinfo.size;
      entryinfo.unused=(short) tileinfo.size;    //this is a hack (used to be 5120)
    }
    else
    {
      if(fileloc.index>=headerinfo.file_entries)
      {
        return -1; //index is greater
      }
      offset=headerinfo.file_offset+fileloc.index*sizeof(entryinfo);
      if(lseek(fhandle,offset,SEEK_SET )!=offset)
      {
        return -1; //bif is corrupt
      }
      if(read(fhandle,&entryinfo,sizeof(entryinfo))!=sizeof(entryinfo) )
      {
        return -1;
      }
      if(entryinfo.restype==REF_TIS) return -1;       //no tilesets!
      if(!(editflg&RESLOC) ) //don't be so pickish in case of weidu
      {
        if((entryinfo.res_loc&FILE_IDX_MASK)!=fileloc.index)
        {
          return -1;
        }
      }
    }
    //resource locator, lowest 14 bits
    //
    if(lseek(fhandle,entryinfo.offset,SEEK_SET)!=entryinfo.offset)
    {
      return -1;
    }
    fileloc.size=entryinfo.size;
  }
  else fileloc.size=-1;
  return fhandle;
}

int resolve_tbg_entry(long reference, tbg_tlk_reference &ref)
{
  if(reference<0) return -1;
  if(reference>=tlk_headerinfo.entrynum) return -2;
  memset(&ref,0,sizeof(tbg_tlk_reference) );
  ref.flags=tlk_entries[reference].reference.flags;
  ref.length=tlk_entries[reference].text.GetLength();
  ref.pitch=tlk_entries[reference].reference.pitch;
  memcpy(ref.soundref,tlk_entries[reference].reference.soundref,8);
  ref.volume=tlk_entries[reference].reference.volume;
  ref.strrefcount=1;
  ref.offset=reference; //fake number
  return 0;
}

CString resolve_tlk_text(long reference)
{
  CString tmp;
  
  if(reference<0) return "<Not Available>";
  if(reference>=tlk_headerinfo.entrynum) return "<Invalid Reference>";
  tmp=tlk_entries[reference].text;
  tmp.Replace("\n","\r\n");
  return tmp;
}

BOOL match_tlk_text(long reference, CString text, int ignorecase)
{
  CString tmp;

  tmp=resolve_tlk_text(reference);
  if(ignorecase)
  {
    tmp.MakeLower();
    text.MakeLower();
  }
  return tmp.Find(text)!=-1;
}

int store_tlk_data(long reference, CString text, CString sound)
{
  CString tmpsound;
  int i;
  tlk_entry *newtlkentries;
  bool newentry=false;
  
  text.Replace("\r\n","\n");
  if(reference<=0) newentry=true;
  if(reference>=tlk_headerinfo.entrynum) newentry=true;
  if(newentry)
  {
    //since we want to avoid to include a new entry at all costs
    //there is a shortcut to reuse existing entries
    for(i=0;i<tlk_headerinfo.entrynum;i++) 
    {
      RetrieveResref(tmpsound, tlk_entries[i].reference.soundref);
      if(tlk_entries[i].text==text)
      { //feature: always update sound for matching strings
        if(tmpsound.IsEmpty())
        {
          store_tlk_soundref(i,sound);
        }
        return i;
      }
    }
    if(!(editflg&RECYCLE)) //if we recycle string references
    {
      for(i=0;i<tlk_headerinfo.entrynum;i++)
      {
        if(tlk_entries[i].text==DELETED_REFERENCE)
        {
          newentry=false;
          reference=i;
          store_tlk_soundref(i,sound);
          break;
        }
      }
    }
  }
  if(newentry)
  {
    reference=tlk_headerinfo.entrynum++;
    newtlkentries=new tlk_entry[tlk_headerinfo.entrynum];
    if(!newtlkentries)
    {
      tlk_headerinfo.entrynum--;
      return -3; //failed due to memory 
    }
    for(i=0;i<tlk_headerinfo.entrynum-1;i++)
    {
      newtlkentries[i]=tlk_entries[i];
    }

    delete [] tlk_entries;
    tlk_entries=newtlkentries;
    tlk_entries[reference].text="";
    memset(&tlk_entries[reference].reference,0,sizeof(tlk_reference) );
  }
  if(text.GetLength())
  {
    tlk_entries[reference].reference.flags|=1;
    if(text.Find('<')>=0) tlk_entries[reference].reference.flags|=4; //tagged text
    else tlk_entries[reference].reference.flags&=~4; //non-tagged text
  }
  else tlk_entries[reference].reference.flags&=~5;
  store_tlk_soundref(reference,sound);
  if(tlk_entries[reference].text!=text)
  {
    tlk_entries[reference].text=text;
    global_changed=true;
  }
  return reference;
}

int store_tlk_text(long reference, CString text)
{
  int i;
  tlk_entry *newtlkentries;
  bool newentry=false;
  
  text.Replace("\r\n","\n");
  if(reference<=0) newentry=true;
  if(reference>=tlk_headerinfo.entrynum) newentry=true;
  if(newentry)
  {
    //since we want to avoid to include a new entry at all costs
    //there is a shortcut to reuse existing entries
    for(i=0;i<tlk_headerinfo.entrynum;i++) 
    {
      if(tlk_entries[i].text==text) return i;
    }    
    if(!(editflg&RECYCLE)) //if we recycle string references
    {
      for(i=0;i<tlk_headerinfo.entrynum;i++) 
      {
        if(tlk_entries[i].text==DELETED_REFERENCE)
        {
          newentry=false;
          reference=i;
          break;
        }
      }
    }
  }
  if(newentry)
  {
    reference=tlk_headerinfo.entrynum++;
    newtlkentries=new tlk_entry[tlk_headerinfo.entrynum];
    if(!newtlkentries)
    {
      tlk_headerinfo.entrynum--;
      return -3; //failed due to memory 
    }
    for(i=0;i<tlk_headerinfo.entrynum-1;i++)
    {
      newtlkentries[i]=tlk_entries[i];
    }

    delete [] tlk_entries;
    tlk_entries=newtlkentries;
    tlk_entries[reference].text="";
    memset(&tlk_entries[reference].reference,0,sizeof(tlk_reference) );
  }
  if(text.GetLength())
  {
    tlk_entries[reference].reference.flags|=1;
    if(text.Find('<')>=0) tlk_entries[reference].reference.flags|=4; //tagged text
    else tlk_entries[reference].reference.flags&=~4; //non-tagged text
  }
  else tlk_entries[reference].reference.flags&=~5;
  if(tlk_entries[reference].reference.soundref[0])
  {
    tlk_entries[reference].reference.flags|=2;
  }
  else
  {
    tlk_entries[reference].reference.flags&=~2;
  }
  if(tlk_entries[reference].text!=text)
  {
    tlk_entries[reference].text=text;
    global_changed=true;
  }
  return reference;
}

bool resolve_tlk_tag(long reference)
{
  if(reference<=0) return false;
  if(reference>=tlk_headerinfo.entrynum) return false;
  return !!(tlk_entries[reference].reference.flags&4);
}

bool toggle_tlk_tag(long reference)
{
  if(reference<=0) return false;
  if(reference>=tlk_headerinfo.entrynum) return false;
  tlk_entries[reference].reference.flags^=4;
  global_changed=true;
  return !!(tlk_entries[reference].reference.flags&4);
}

CString resolve_tlk_soundref(long reference)
{
  CString retval;

  if(reference<0) return "";
  if(reference>=tlk_headerinfo.entrynum) return "";
  RetrieveResref(retval, tlk_entries[reference].reference.soundref);
  return retval;
}

int store_tlk_soundref(long reference, CString sound)
{
  CString tmpsound;
  int i;
  tlk_entry *newtlkentries;
  bool newentry=false;

  if(reference<=0) newentry=true;
  if(reference>=tlk_headerinfo.entrynum) newentry=true;
  if(newentry)
  {
    reference=tlk_headerinfo.entrynum++;
    newtlkentries=new tlk_entry[tlk_headerinfo.entrynum];
    if(!newtlkentries)
    {
      tlk_headerinfo.entrynum--;
      return -3; //failed due to memory 
    }
    for(i=0;i<tlk_headerinfo.entrynum-1;i++)
    {
      newtlkentries[i]=tlk_entries[i];
    }
    delete [] tlk_entries;
    tlk_entries=newtlkentries;
    tlk_entries[reference].text="";
    memset(&tlk_entries[reference].reference,0,sizeof(tlk_reference) );
  }
  //don't report a change if there is no change
  RetrieveResref(tmpsound, tlk_entries[reference].reference.soundref);
  if(sound==tmpsound) return reference;
  if(sound.IsEmpty()) tlk_entries[reference].reference.flags&=~2;
  else tlk_entries[reference].reference.flags|=2;
  StoreResref(sound, tlk_entries[reference].reference.soundref);
  global_changed=true;
  return reference;
}

//moves a fixed char storage area to a CString (always capitalized)
void RetrieveResref(CString &dlgcontrol, const char *itempoi, int dot)
{
  char ref[9];
  int i;
  
  for(i=0;itempoi[i] && i<8;i++)
  {
    if(dot && itempoi[i]=='.') break;
    ref[i]=(char) toupper(itempoi[i]);
  }
  ref[i]=0;
  dlgcontrol=ref;
}

//moves a cstring resref into a fixed char storage area (always capitalized)
void StoreResref(CString &dlgcontrol, char *itempoi)
{
  char *poi;
  int i;
  
  poi=dlgcontrol.GetBuffer(8);
  for(i=0;poi[i] && i<8;i++) itempoi[i]=(char) toupper(poi[i]);
  for(;i<8;i++) itempoi[i]=0;
  dlgcontrol.ReleaseBuffer();
}

//moves a fixed char storage area to a CString (always capitalized)
void RetrieveAnim(CString &dlgcontrol, const char *itempoi)
{
  char ref[3];
  int i;
  
  for(i=0;itempoi[i] && i<2;i++) ref[i]=(char) toupper(itempoi[i]);
  ref[i]=0;
  dlgcontrol=ref;
}

void StoreAnim(CString &dlgcontrol, char *itempoi)
{
  char *poi;
  int i;
  
  poi=dlgcontrol.GetBuffer(2);
  for(i=0;poi[i] && i<2;i++)
  {
    if(poi[i]==' ') itempoi[i]=0;
    else itempoi[i]=(char) toupper(poi[i]);
  }
  for(;i<2;i++) itempoi[i]=0;
  dlgcontrol.ReleaseBuffer();
}

void RetrieveVariable(CString &dlgcontrol, const char *itempoi)
{
  char ref[33];
  int i;
  
  for(i=0;itempoi[i] && i<32;i++) ref[i]=(char) toupper(itempoi[i]); //areas rely on toupper
  ref[i]=0;
  dlgcontrol=ref;
}

void StoreName(CString &dlgcontrol, char *itempoi)
{
  char *poi;
  int i;
  int cap;
  
  poi=dlgcontrol.GetBuffer(32);
  cap=1;
  for(i=0;poi[i] && i<32;i++)
  {
    if(cap)
    {
      itempoi[i]=(char) toupper(poi[i]);
      if(itempoi[i]!=' ') cap=0;
    }
    else
    {
      itempoi[i]=(char) tolower(poi[i]);
      if(itempoi[i]==' ') cap=1;
    }
  }
  for(;i<32;i++) itempoi[i]=0;
  dlgcontrol.ReleaseBuffer();
}

void StoreVariable(CString &dlgcontrol, char *itempoi)
{
  char *poi;
  int i;
  
  poi=dlgcontrol.GetBuffer(32);
  for(i=0;poi[i] && i<32;i++) itempoi[i]=(char) toupper(poi[i]);
  for(;i<32;i++) itempoi[i]=0;
  dlgcontrol.ReleaseBuffer();
}

CString format_label(CString label)
{
  CString tmp;
  int i;
  int cap;
  int ch;

  tmp=label;
  cap=1;
  for(i=0;i<tmp.GetLength();i++)
  {
    ch=tmp[i];
    if(ch=='_') ch=' ';
    if(cap)
    {
      tmp.SetAt(i,(char) toupper(ch));
      cap=0;
    }
    else
    {
      tmp.SetAt(i,(char) tolower(ch));
    }
    if(ch==' ') cap=1;
  }
  return tmp;
}

CString scripttypes[5]={"override","class","race","general","default"};

CString get_script_type(int i)
{
  if(i<5) return scripttypes[i];
  return "Unknown";
}

CString storetypes[NUM_STORETYPE+1]={"00-Store","01-Tavern","02-Inn","03-Temple","04-Container (IWD2)","05-Container (BG2)","\?\?-Unknown"};

CString format_storetype(unsigned int storetype)
{
  CString tmp;
  
  if(storetype>=NUM_STORETYPE)
  {
    tmp.Format("0x%02x-Unknown",storetype);
    return tmp;
  }
  return storetypes[storetype];
}

CString bg2_spellslots[BG2_SPSLOTNUM]={"Priest","Wizard","Innate"};
CString iwd2_spellslots[IWD2_SPSLOTNUM+4]={"Bard","Cleric","Druid",
"Paladin","Ranger","Sorceror","Wizard","Domain","Innates","Songs","Shapes"};

#define IWD2_DOMAIN IWD2_SPSLOTNUM
#define IWD2_INNATE (IWD2_SPSLOTNUM+1)
#define IWD2_SONG   (IWD2_SPSLOTNUM+2)
#define IWD2_SHAPE  (IWD2_SPSLOTNUM+3)

CString format_spellslot(unsigned int spellslot)
{
  CString tmp;

  if(iwd2_structures())
  {
    if(spellslot<IWD2_SPELLCOUNT)
    {
      if(spellslot<7*9) return iwd2_spellslots[spellslot/9]; //type
      spellslot-=7*9;
      if(spellslot<9) return iwd2_spellslots[IWD2_DOMAIN];
      return iwd2_spellslots[spellslot-9+IWD2_INNATE];
    }
    return "Internal error";
  }
  else
  {
    if(spellslot<BG2_SPSLOTNUM)
    {
      return bg2_spellslots[spellslot];
    }
  }
  tmp.Format("0x%02x-Unknown",spellslot);
  return tmp;
}

CString spelltypes[NUM_SPELLTYPE+1]={"0-Special","1-Wizard","2-Cleric","3-Unknown","4-Innate"};

CString format_spelltype(unsigned int spelltype)
{
  CString tmp;
  
  if(spelltype>NUM_SPELLTYPE)
  {
    tmp.Format("0x%02x-Unknown",spelltype);
    return tmp;
  }
  return spelltypes[spelltype];
}

CString format_schooltype(unsigned int schooltype)
{
  POSITION pos;
  CString tmp, tmpstr;

  pos=school_names.FindIndex(schooltype);
  if(pos) tmp=school_names.GetAt(pos);
  if(tmp.IsEmpty()) tmpstr.Format("0x%02x-Unknown",schooltype);
  else tmpstr.Format("0x%02x-%s",schooltype,tmp);
  return tmpstr;
}

CString format_sectype(unsigned int sectype)
{
  POSITION pos;
  CString tmp, tmpstr;

  pos=sectype_names.FindIndex(sectype);
  if(pos) tmp=sectype_names.GetAt(pos);
  if(tmp.IsEmpty()) tmpstr.Format("0x%02x-Unknown",sectype);
  else tmpstr.Format("0x%02x-%s",sectype,tmp);
  return tmpstr;
}

CString itemtypes[NUM_ITEMTYPE+1]={"0-Misc/Book","1-Amulet","2-Armor","3-Belt",
"4-Boots","5-Arrow","6-Bracer/Pet","7-Helmet","8-Key","9-Potion",
"0xA-Ring","0xB-Scroll","0xC-Shield","0xD-Food","0xE-Bullet","0xF-Bow",
"0x10-Dagger","0x11-Mace","0x12-Sling","0x13-Small sword","0x14-Big sword",
"0x15-Hammer","0x16-Morning Star","0x17-Flail","0x18-Dart","0x19-Axe",
"0x1A-Staff","0x1B-Crossbow","0x1C-Hand-to-Hand","0x1D-Spear","0x1E-Polearm",
"0x1F-Bolt","0x20-Cloak","0x21-Coin","0x22-Gem","0x23-Wand","0x24-Bag/Eye/Broken armor",
"0x25-Broken shield/Bracelet","0x26-Broken sword/Earring","0x27-Tattoo","0x28-Lens",
"0x29-Buckler/Teeth","0x2A-Candle","0x2B-Unknown","0x2C-Club","0x2D-Unknown",
"0x2E-Unknown","0x2F-Large shield","0x30-Unknown","0x31-Medium shield",
"0x32-Notes","0x33-Unknown","0x34-Unknown","0x35-Small shield",
"0x36-Unknown","0x37-Telescope","0x38-Drink","0x39-Great sword",
"0x3a-Container (IWD)","0x3b-Fur/pelt","0x3c-Leather armor","0x3d-Studded leather",
"0x3e-Chain mail","0x3f-Splint mail","0x40-Half plate","0x41-Full plate",
"0x42-Hide armor","0x43-Robe","0x44-Unknown","0x45-Bastard sword",
"0x46-Scarf","0x47-Food (IWD2)","0x48-Hat","0x49-Gauntlet",
"\?\?-Unknown"};

CString format_itemtype(unsigned int itemtype)
{
  CString tmp;
  
  if(itemtype>=NUM_ITEMTYPE)
  {
    tmp.Format("0x%02x-Unknown",itemtype);
    return tmp;
  }
  return itemtypes[itemtype];
}

short find_itemtype(CString itemtype)
{
  int i;
  CString tmp;

  for(i=0;i<NUM_ITEMTYPE;i++)
  {
    tmp=itemtypes[i].Mid(itemtypes[i].Find('-')+1);
    tmp.MakeLower();
    if(tmp.Left(itemtype.GetLength())==itemtype) return (short) i;
  }
  return (short) strtonum(itemtype);
}
void Pad(CString &str, int len)
{
  CString tmp, format;
  
  format.Format("%%%d.%ds",len,len);
  tmp.Format(format,str);
  str=tmp;
}

long weaprofidx[NUM_WEAPROF+1]={
  0,1,2,3,4,5,6,7,
  0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,0x60,0x61,0x62,
  0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,
  0x6d,0x6e,0x6f,0x70,0x71,0x72,0x73,0x74,0x75,0x76,
  0x77,0x78,0x79,0x7a,0x7b,0x7c,0x7d,0x7e,0x7f,0x80,
  0x81,0x82,0x83,0x84,0x85,0x86,0xff
};

CString weaprofs[NUM_WEAPROF+1]={
  "None/BG1 Large sword","BG1 Small sword","BG1 Bow","BG1 Spear",
  "BG1 Blunt","BG1 Spiked","BG1 Axe","BG1 Missile",
  "Bastard Sword","Long Sword","Short Sword","Axe",
  "Two-Handed Sword","Katana","Scimitar/Wakizashi/Ninja-To",
  "Dagger","War Hammer","Spear","Halberd","Flail/Morningstar",
  "Mace","Quarterstaff", "Crossbow","Long Bow","Short Bow","Darts",
  "Sling","Blackjack","Gun","Martial Arts","Two-Handed Weapon Skill",
  "Sword and Shield Skill","Single Weapon Skill","Two Weapon skill",
  "Club","Extra Proficiency 2","Extra Proficiency 3",
  "Extra Proficiency 4","Extra Proficiency 5","Extra Proficiency 6",
  "Extra Proficiency 7","Extra Proficiency 8","Extra Proficiency 9",
  "Extra Proficiency 10","Extra Proficiency 11","Extra Proficiency 12",
  "Extra Proficiency 13","Extra Proficiency 14","Extra Proficiency 15",
  "Extra Proficiency 16","Extra Proficiency 17","Extra Proficiency 18",
  "Extra Proficiency 19","Extra Proficiency 20","\?\?-Unknown"
};

int act_num_weaprof=NUM_WEAPROF;

int getproficiencyidx(int profnum)
{
  int i;
  
  for(i=0;i<act_num_weaprof;i++)
  {
    if(weaprofidx[i]==profnum)
      return i;
  }
  return NUM_WEAPROF;
}

CString format_weaprofs(int profnum)
{
  int i;
  CString tmpstr;

  for(i=0;i<act_num_weaprof;i++)
  {
    if(weaprofidx[i]==profnum)
    {
      tmpstr.Format("0x%02x-%s",profnum,weaprofs[i]);
      return tmpstr;
    }
  }
  tmpstr.Format("0x%02x-Unknown",profnum);
  return tmpstr;
}

unsigned short animidx[NUM_ANIMIDX]={
  0,'A2','A3','A4','W2','W3','W4','XA','WB','BC','LC','1D','2D','3D',
    '4D','DD','LF','SF','0H','1H','2H','3H','4H','5H','6H','BH','CM','SM',
    'SQ','1S','2S','LS','PS','SS','HW','3S','CS'
};

CString itemanimations[NUM_ANIMIDX]={
  "None","Leather Armour","Chainmail","Plate Mail","Small Robe",
    "Medium Robe","Large Robe","Axe","Bow","Crossbow","Club",
    "Buckler","Shield (Small)","Shield (Medium)","Shield (Large)",
    "Dagger","Flail","Flame Sword","Helm #1 (Standard)","Helm #2",
    "Helm #3 (Bronze Winged)","(Gold Winged)","Helm #5",
    "Helm #6 (Feathers)","Helm #7","Halberd","Mace","Morning Star",
    "Quarter Staff (Metal)","Sword 1-Handed","Sword 2-Handed","Sling",
    "Spear","Short Sword","War Hammer","Katana","Scimitar"
};

int getanimationidx(int animtype)
{
  int i;
  
  for(i=0;i<NUM_ANIMIDX;i++)
  {
    if(animidx[i]==animtype)
      return i;
  }
  return 0;
}

unsigned short animtypes[NUM_ANIMTYPES][3]={
  {0,20,80}, {10,25,65}, {60,40,0},
  {0,35,65}, {0,0,100}, {10,70,20},
  {50,50,0}, {34,33,33},{0,0,0,},{0,0,0},
};

int find_animtype(unsigned short *percents)
{
  int i;

  for(i=0;i<NUM_ANIMTYPES-1;i++)
  {
    if(!memcmp(percents,animtypes[i],3*sizeof(short)) ) return i;
  }
  return i;
}

CString animnames[NUM_ANIMTYPES]={"Dagger","Short Sword","Two-Handed Sword","Halberd",
"Spear","Staff","Other Melee","Not weapon","Unused","Unknown"};

CString format_animtype(int animtype)
{
  int i;
  CString tmpstr;

  for(i=0;i<NUM_ANIMIDX;i++)
  {
    if(animidx[i]==animtype)
    {
      tmpstr.Format("%2.2s-%s",&animidx[i],itemanimations[i]);
      return tmpstr;
    }
  }
  tmpstr.Format("%2.2s-Unknown",&animtype);
  return tmpstr;
}

CString convert_degree(int value)
{
  CString tmpstr;
  int degree;

  degree=value*360/256;
  tmpstr.Format("%d (%d degree)",value,degree);
  return tmpstr;
}

CString convert_radius(int value, int direction)
{
  CString tmpstr;
  int radius;

  if(direction)
  {
    radius=value;
    value=radius*256/30;
  }
  else
  {
    radius=value*30/256;
  }
  tmpstr.Format("%d (%d')",value,radius);
  return tmpstr;
}

CString proj_facing_desc[NUM_FVALUE]={"0x1 Doesn't face target","0x5-Faces target","0x9-Dragon breath"};
int proj_facing_value[NUM_FVALUE]={1,5,9};

CString get_face_value(int fvalue)
{
  CString tmpstr;
  int i;

  for(i=0;i<NUM_FVALUE;i++)
  {
    if(proj_facing_value[i]==fvalue) return proj_facing_desc[i];
  }
  tmpstr.Format("0x%x Unknown",fvalue);
  return tmpstr;
}

CString sparkcolour[NUM_SPKCOL]={"0-Crash","1-Black","2-Blue","3-Purple & amber","4-Amber","5-Pale green",
"6-Purple","7-Pink","8-Grey","9-Aqua","0xa-White","0xb-Pale pink","0xc-Golden"};

CString get_spark_colour(int spktype)
{
  CString tmp;

  if(spktype<0 || spktype>=NUM_SPKCOL)
  {
    tmp.Format("0x%x-Crash",spktype);
    return tmp;
  }
  return sparkcolour[spktype];
}

CString area_flags[NUM_ARFLAGS]={"-No save/rest","-Tutorial",
"-Antimagic area","-Dream"};
CString area_flags_iwd2[NUM_ARFLAGS]={"-No save","-No rest",
"-Lock battle music","-Unknown"};

CString get_areaflag(unsigned long arflags)
{
  int i,j;
  CString tmp;

  if(!arflags) return "0-Normal";
  j=1;
  if(arflags<10 ) tmp.Format("%d",arflags);
  else tmp.Format("0x%x",arflags);
  for(i=0;i<NUM_ARFLAGS;i++)
  {
    if(arflags&j)
    {
      if(has_xpvar()||pst_compatible_var())
      {
        tmp+=area_flags_iwd2[i];
      }
      else
      {
        tmp+=area_flags[i];
      }
    }
    j<<=1;
  }
  return tmp;
}

CString attack_types[NUM_ATYPE]={
  "0-None","1-Melee","2-Projectile","3-Magic","4-Bow"
};

CString get_attack_type(int atype)
{
  CString tmp;

  if(atype==-1) return "*-Spell";
  if(atype<0||atype>=NUM_ATYPE)
  {
    tmp.Format("%d-Unknown",atype);
    return tmp;
  }
  return attack_types[atype];
}

CString get_attacknum(int anum)
{
  CString tmpstr;

  if(anum<6) tmpstr.Format("%d", anum);
  else tmpstr.Format("%d/2", (anum-6)*2+1);
  return tmpstr;
}

int find_attacknum(CString anum)
{
  if(anum.Right(2)=="/2") return (strtonum(anum)-1)/2+6;
  return strtonum(anum);
}

int slottypenums[NUM_SLOTTYPE]={0,1,2,3,1000,65512,65513,65514};
CString slottypes[NUM_SLOTTYPE]={"0-Weapon 1","1-Weapon 2","2-Weapon 3","3-Weapon 4",
"1000-No slot","65512-Quiver 1","65513-Quiver 2","65514-Quiver 3"};

CString get_slottype(int slottype)
{
  CString tmp;
  int i;
  
  for(i=0;i<NUM_SLOTTYPE;i++)
  {
    if(slottypenums[i]==slottype) return slottypes[i];
  }
  tmp.Format("%d-Unknown",slottype);
  return tmp;
}

CString spell_forms[NUM_SFTYPE]={ "0-Unknown","1-Normal","2-Projectile"
};

CString get_spell_form(int sftype)
{
  CString tmp;
  
  if(sftype<0||sftype>=NUM_SFTYPE)
  {
    tmp.Format("%d-Unknown",sftype);
    return tmp;
  }
  return spell_forms[sftype];
}

CString location_types[NUM_LFTYPE]={
  "0-None","1-Weapon","2-Spell","3-Equipment","4-Innate"
};

CString get_location_type(int lftype)
{
  CString tmp;
  
  if(lftype<0 || lftype>=NUM_LFTYPE)
  {
    tmp.Format("%d-Unknown",lftype);
    return tmp;
  }
  return location_types[lftype];
}

CString target_types[NUM_TTYPE]={
  "0-Invalid","1-Creature","2-Inventory/Crash", "3-Dead character","4-Area","5-Self","6-Unknown/Crash",
  "7-None",
};

CString get_target_type(int ttype)
{
  CString tmp;
  
  if(ttype<0 || ttype>=NUM_TTYPE)
  {
    tmp.Format("%d-Unknown",ttype);
    return tmp;
  }
  return target_types[ttype];
}

CString ammo_types[NUM_AMTYPE]={"0-None","1-Bow","2-XBow","3-Sling","40-Spear","100-Axe"};
int ammos[NUM_AMTYPE]={0,1,2,3,0x28,0x64};

CString get_ammo_type(int amtype)
{
  int i;
  CString tmp;
  
  for(i=0;i<NUM_AMTYPE;i++)
  {
    if(ammos[i]==amtype)
      return ammo_types[i];
  }
  tmp.Format("%d-Unknown",amtype);
  return tmp;
}

CString damage_types[NUM_DTYPE]={
  "0-None","1-Piercing/Magic","2-Blunt","3-Slash","4-Ranged","5-Fists",
};

CString get_damage_type(int dtype)
{
  CString tmp;
  
  if(dtype<0 || dtype>=NUM_DTYPE)
  {
    tmp.Format("%d-Unknown",dtype);
    return tmp;
  }
  return damage_types[dtype];
}

CString charge_types[NUM_CHTYPE]={
  "0-Don't vanish","1-Expended","2-Exp. w/o sound","3-Per day",
};

CString get_charge_type(int chtype)
{
  CString tmp;
  
  if(chtype<0 || chtype>=NUM_CHTYPE)
  {
    tmp.Format("%d-Unknown",chtype);
    return tmp;
  }
  return charge_types[chtype];
}

CString missile_types[NUM_MTYPE]={
  "Arrow","Bolt","Bullet"
};

CString get_missile_type(int mtype)
{
  CString tmp;
  
  if(mtype<0 || mtype>=NUM_MTYPE)
  {
    tmp.Format("%d-Unknown",mtype);
    return tmp;
  }
  return missile_types[mtype];
}

opcode_struct opcodes[NUM_FEATS];
static int strref_opcodes_tob[]={103,139,180,181,206,253,254,290,-1};
static int strref_opcodes_iwd[]={103,139,180,181,283,-1};
static int opcode_opcodes_tob[]={101,198,-1};
static int opcode_opcodes_iwd[]={101,198,261,276,-1};
static int state_opcodes_tob[]={-1};
static int state_opcodes_iwd[]={288,-1};

int feature_resource(int feature)
{
  switch(feature)
  {
  case 67: case 135: case 151: case 410: case 411:
    return REF_CRE;
  case 82:
    return REF_BCS;
  case 111: case 112: case 122: case 123: case 143: case 180: case 255:
    return REF_ITM;
    /* this is the default
  case 146: case 147: case 148: case 171: case 172: case 206: case 207:
  case 232: case 251: case 252:
    return REF_SPL;
    */
  case 152:
    return REF_MVE;
    break;
  case 174:
    return REF_WAV;
  case 177: case 248: case 249: case 272: case 283:
    return REF_EFF;
  case 214: case 298:
    return REF_2DA;
  case 187:  //in PST this used to be some BAM resource
  case 191:  //PST
  case 201:  //PST
  case 215:
    return REF_BAM;
  case 291: case 296:
    return REF_VVC;
  }
  return REF_SPL;
}

int *get_state_opcodes()
{
  if(has_xpvar()) return state_opcodes_iwd;
  return state_opcodes_tob;
}

int *get_opcode_opcodes()
{
  if(has_xpvar()) return opcode_opcodes_iwd;
  return opcode_opcodes_tob;
}
int *get_strref_opcodes()
{
  if(has_xpvar()) return strref_opcodes_iwd;
  return strref_opcodes_tob;
}

CString get_state_text(int state)
{
  CString tmp, tmpstr;

  get_idsfile("SPLSTATE",false);
  tmp=IDSToken("SPLSTATE",state);
  if(tmp.GetLength()) tmpstr.Format("%d %s",state,tmp);
  else tmpstr.Format("%d Unknown",state);
  return tmpstr;
}

CString get_opcode_text(int opcode)
{
  CString tmp;
  
  if(opcode<0 || opcode>=NUM_FEATS)
  {
    tmp.Format("Unknown(%d)",opcode);
    return tmp;
  }
  if(opcodes[opcode].opcodetext.IsEmpty())
  {
    tmp.Format("Unknown(%d)",opcode);
    return tmp;
  }
  if(editflg&NUMEFF)
  {
    tmp.Format("%s(%d)",opcodes[opcode].opcodetext,opcode);
    return tmp;
  }
  else
  {
    return opcodes[opcode].opcodetext;
  }
}

CString get_opcode_desc(int opcode, int par2)
{
  CString tmp;
  int *opcodelist;
  
  if(opcode<0 || opcode>=NUM_FEATS)
  {
    tmp.Format("This feature (%d) is unknown.",opcode);
    return tmp;
  }
  if(opcodes[opcode].opcodedesc.IsEmpty())
  {
    tmp.Format("This feature (%d) is unknown.",opcode);
    return tmp;
  }
  opcodelist=get_opcode_opcodes();
  if(member_array(opcode,opcodelist)!=-1)
  {
    tmp=get_opcode_text(par2);
  }
  else
  {
    opcodelist=get_state_opcodes();
    if(member_array(opcode,opcodelist)!=-1)
    {
      tmp=get_state_text(par2);
    }
  }
  return opcodes[opcode].opcodedesc+tmp;
}

CString get_par1_label(int opcode)
{
  CString tmp;
  
  if(opcode<0 || opcode>=NUM_FEATS)
  {
    tmp.Format("Unknown #1",opcode);
    return tmp;
  }
  return opcodes[opcode].par1;
}

CString get_par2_label(int opcode)
{
  CString tmp;
  
  if(opcode<0 || opcode>=NUM_FEATS)
  {
    tmp.Format("Unknown #2",opcode);
    return tmp;
  }
  return opcodes[opcode].par2;
}
//                       0 button, 1 unknown,2 slider, 3 editbox,4 unknown, 5 scrolltext,
static char chuicontrolsizes[CCNUM]={18,       -1,       38,       92,       -1,        32,
//6 label, 7 scrollbar
22,      26};
static CString chuicontrolnames[CCNUM]={"Button","Unknown 1","Slider","EditBox","Unknown 4",
"Textarea","Label","Scrollbar"};

int ChuiControlSize(int controltype)
{
  if(controltype<0 || controltype>=CCNUM) return -1;
  return chuicontrolsizes[controltype];
}

CString ChuiControlName(int controltype)
{
  CString tmpstr;

  if(controltype<0) return "";
  if(controltype>=CCNUM)
  {
    tmpstr.Format("Unknown %d",controltype);
    return tmpstr;
  }
  return chuicontrolnames[controltype];
}

CString kitfile;

char *idstype[NUM_IDS]={
  "ea","general","race","class",
    "specific","gender","alignment"
};

char *idsname[NUM_IDS]={"EA","GENERAL","RACE","CLASS","SPECIFIC","GENDER","ALIGN"};
char *idsname_iwd[NUM_IDS]={"EA","GENERAL","RACE","CLASS","SPECIFIC","GENDER","ALIGNMEN"};
char *idsname_iwd2[NUM_IDS]={"EA","GENERAL","RACE","CLASS","SPECIFIC","GENDER","ALIGNMNT"};

CString IDSType(int ids, bool addtwo)
{
  if(addtwo) ids-=2;
  if(ids<0 || ids>=NUM_IDS) return "unknown";
  return idstype[ids];
}

CString IDSName(int ids, bool addtwo)
{
  if(addtwo) ids-=2;
  if(ids<0 || ids>6) return "unknown";
  if(iwd2_structures()) return idsname_iwd2[ids];
  if(has_xpvar()) return idsname_iwd[ids];
  return idsname[ids];
}

int IDSKey(CString filename, CString key)
{
  CStringMapInt *idsfile;
  int value;

  key.MakeLower();
  if(!idsmaps.Lookup(filename,idsfile)) return -1;
  if(!idsfile)
  {    
    return -1;   //internal error (no file loaded)
  }
  value=-1;
  idsfile->Lookup(key, value);
  return value;
}

//don't append number so this function could be used for decompile
CString IDSToken(CString filename, int value)
{
  CString key;
  int tmp;
  CStringMapInt *idsfile;
  POSITION pos;
  
  if(!idsmaps.Lookup(filename,idsfile)) return "";
  if(!idsfile)
  {    
    return "";   //internal error (no file loaded)
  }
  pos=idsfile->GetStartPosition();
  while(pos)
  {
    idsfile->GetNextAssoc(pos, key, tmp);
    if(tmp==value)
    {
      return key;
    }
  }
  if((value==0) || (value==255)) return "unused";
  return "";
}

int FillCombo(CString idsname, CComboBox *cb, int len)
{
  CStringMapInt *idsfile;
  int value;
  POSITION pos;
  CString tmpstr, format;

  cb->ResetContent();
  if(!idsmaps.Lookup(idsname, idsfile))
  {
    return -1; //internal error
  }
  pos=idsfile->GetStartPosition();
  while(pos)
  {
    idsfile->GetNextAssoc(pos, idsname, value);
    if(len<0) format.Format("%%0%dd %%s",-len);
    else format.Format("0x%%0%dx %%s",len);
    tmpstr.Format(format,(unsigned long) value,idsname);
    cb->AddString(tmpstr);
  }
  return 0;
}

CString DamageType(int hex)
{
  switch(hex)
  {
  case 0: return "crushing"; break;
  case 0x00010000: return "acid"; break;
  case 0x00020000: return "cold"; break;
  case 0x00040000: return "electricity"; break;
  case 0x00080000: return "fire"; break;
  case 0x00100000: return "piercing"; break;
  case 0x00200000: return "poison";break;
  case 0x00400000: return "magic"; break;
  case 0x00800000: return "projectile"; break;
  case 0x01000000: return "slashing"; break;
  case 0x02000000: return "magic fire"; break;
  case 0x04000000: return "magic cold"; break;
  case 0x08000000: return "stunning"; break;
  case 2: return "lower to percentile"; break;
  case 1: return "lower to fixed"; break;
  }
  return "unknown";
}

CString save_types[NUM_STYPE]={
  "0-None","1-Spells","2-Breath Weapon","4-Death/Poison","8-Wands",
    "16-Polymorph"
};

CString save_types2[NUM_STYPE2]={
  "0-None","1-Fortitude","2-Reflex","4-Will"
};

CString get_save_type(int stype)
{
  CString tmp;
  
  if(iwd2_structures())
  {
    switch(stype)
    {
    case 0: return save_types2[0]; break;
    case 1: return save_types2[1]; break;
    case 2: return save_types2[2]; break;
    case 4: return save_types2[3]; break;
    }
  }
  else
  {
    switch(stype)
    {
    case 0: return save_types[0]; break;
    case 1: return save_types[1]; break;
    case 2: return save_types[2]; break;
    case 4: return save_types[3]; break;
    case 8: return save_types[4]; break;
    case 16:return save_types[5]; break;
    }
  }
  tmp.Format("%d-Unknown",stype);
  return tmp;
}

CString timing_types[NUM_TMTYPE]={
  "0-Duration","1-Permanent","2-While equiped","3-Delayed duration",
    "4-Delayed","5-Delayed?","6-Duration?","7-Permanent?","8-Permanent?",
    "9-Permanent After Death","10-Trigger"
};

int has_duration[]={TIMING_DURATION,TIMING_DELDDUR,TIMING_DELAYED,
TIMING_DEL2,TIMING_DUR2,-1};

CString duration_labels[NUM_TMTYPE]={
  "Duration","N/A","N/A","Delayed/Duration",
    "Delay","Delay","Duration","N/A","N/A","N/A","N/A"
};

CString get_timing_type(int tmtype)
{
  CString tmp;
  
  if(tmtype<0 || tmtype>=NUM_TMTYPE)
  {
    tmp.Format("0x%x-Unknown",tmtype);
    return tmp;
  }
  return timing_types[tmtype];
}

CString get_duration_label(int tmtype)
{
  if(tmtype<0 || tmtype>=NUM_TMTYPE)
  {    
    return "N/A";
  }
  return duration_labels[tmtype];
}

CString efftarget_types[NUM_ETTYPE]={
  "0 None","1 Self","2 Pre-Target","3 Party","4 Global","5 Not in Party",
  "6 Not Evil","7 Unknown","8 Except self","9 Original caster",
  "10 Unknown","11 Unknown","12 Unknown",
  "13 Unknown",
};

CString get_efftarget_type(int ettype)
{
  if(ettype<0 || ettype>=NUM_ETTYPE)
  {    
    return "";
  }
  return efftarget_types[ettype];
}
CString resist_types[NUM_RTYPE]={
  "0-Nonmagical", "1-Dispel/Not bypass resistance",
  "2-Not dispel/Not bypass resistance","3-Dispel/Bypass resistance",
};

CString get_resist_type(int rtype)
{
  CString tmp;

  if(rtype<0 || rtype>=NUM_RTYPE)
  {
    tmp.Format("0x%x-Unknown",rtype);
    return tmp;
  }
  return resist_types[rtype];
}

CString get_projectile_id(int pronum, int offset)
{
  CString ret, tmp1, tmp2;
  POSITION pos;
  
  pos=pro_titles.FindIndex(pronum);
  if(pos)
  {
    tmp1=pro_titles.GetAt(pos);
    if(!tmp1.GetLength())
    {
      tmp1="Unknown";
    }
  }
  else
  {
    tmp1="Unknown";
  }

  pos=pro_references.FindIndex(pronum);
  if(pos)
  {
    tmp2=pro_references.GetAt(pos);
    if(!tmp2.GetLength())
    {
      tmp2="Unknown";
    }
  }
  else
  {
    tmp2="Unknown";
  }

  ret.Format("%d-%s (%s)",pronum-offset, tmp1, tmp2);
  return ret;
}

CString format_priesttype(int ptype)
{
  CString tmp;

  if(ptype==0x4000) return "0x4000-Druid/Ranger";
  if(ptype==0x8000) return "0x8000-Cleric/Paladin";
  if(ptype==0xe07f) return "0xe07f-PST Mage";
  if(ptype==0) return "0x0000-General";
  tmp.Format("0x%04x-Unknown",ptype);
  return tmp;
}

CString region_types[NUM_RTTYPE]={"Trap","Info","Travel"};

CString get_region_type(int rttype)
{
  CString tmp;
  
  if(rttype<0 || rttype>=NUM_RTTYPE)
  {
    tmp.Format("%d Unknown",rttype);
  }
  else
  {
    tmp.Format("%d %s",rttype,region_types[rttype]);
  }
  return tmp;
}

CString face_values[NUM_FVALUES]={"South","South-SW","Southwest","West-SW","West",
    "West-NW","Northwest","North-NW","North","North-NE","Northeast","East-NE",
    "East","East-SE","Southeast","South-SE"};

CString format_direction(int dir)
{
  CString tmp;

  if(dir<0 || dir>=NUM_FVALUES) 
  {
    tmp.Format("%d Unknown",dir);
  }
  else
  {
    tmp.Format("%d %s",dir,face_values[dir]);
  }
  return tmp;
}

CString get_songname(int songidx)
{
  POSITION pos;
  CString tmp;

  pos=songlist.FindIndex(songidx);
  if(pos)
  {
    tmp.Format("%d %s",songidx,songlist.GetAt(pos).a);
  }
  else
  {
    tmp.Format("%d Unknown",songidx);
  }
  return tmp;
}

CString get_songfile(int songidx)
{
  POSITION pos;
  CString tmp;

  pos=songlist.FindIndex(songidx);
  if(pos)
  {
    return songlist.GetAt(pos).b;
  }
  return tmp; //empty string
}

CString spawn_types[NUM_SPTYPE]={"0 Unknown","1 Need rest","2 When revealed"};

CString get_spawntype(int sptype)
{
  CString tmp;
  
  if(sptype<0 || sptype>=NUM_SPTYPE)
  {
    tmp.Format("%d Unknown",sptype);
    return tmp;
  }
  return spawn_types[sptype];
}

CString container_types[NUM_CITYPE]={"0 <n/a>","1 BAG","2 CHEST","3 DRAWER",
"4 PILE","5 TABLE","6 SHELF","7 ALTAR","8 NONVISIBLE","9 SPELLBOOK",
"10 BODY","11 BARREL","12 CRATE"};

CString get_container_icon(int citype)
{
  CString tmp;
  
  if(citype<0 || citype>=NUM_CITYPE)
  {
    tmp.Format("%d Unknown",citype);
    return tmp;
  }
  return container_types[citype];
}

CString resolve_scriptelement(int opcode, int trigger, int block)
{
  CString tmp;
  CString blocknum;
  
  switch(trigger&0xff00)
  {
  case OBJECT|TRIGGER:
    tmp.Format("Trigger object");
    break;
  case OBJECT|ACTION:
    tmp.Format("Action object #%d",opcode);
    break;
  case TRIGGER:
    opcode&=~0x4000;
    if(opcode<MAX_TRIGGER)
    {
      tmp=trigger_defs[opcode];
    }
    if(tmp.IsEmpty() ) tmp="Unknown trigger";
    break;
  case ACTION:
    if(opcode<MAX_ACTION)
    {
      tmp=action_defs[opcode];
    }
    if(tmp.IsEmpty() ) tmp="Unknown action";
    break;
  default:
    return "Internal Error";
  }
  blocknum.Format("block #%d:%s(%d)",block, tmp, opcode);
  return blocknum;
}

CString get_compile_error(int ret)
{
  switch(ret)
  {
    //didn't pass the raw base level parsing
  case -1:
    return "Missing opening bracket or empty function name";
  case -2:
    return "No closing bracket";
  case -3:
    return "Non existent function";
  case -4:
    return "Argument number doesn't match definition";
  case -5:
    return "Argument type doesn't match definition";
  case -6:
    return "Missing quotes or invalid string value";
  case -7:
    return "Too many closing brackets";
  case -8:
    return "Too many arguments";
    //didn't pass argument type matching
  case -9:
    return "Function is not properly terminated";
  case -10:
    return "Not an object";    //complex :)
  case -11:
    return "Not a point";     //two numbers enclosed in square brackets and separated by comma
  case -12:
    return "Not a valid IDS value"; //listed in IDS
  case -13:
    return "Not a string";    //enclosed in double quotes
  case -14:
    return "Not an area";     //not equal with 6
  case -15:
    return "Not a resource";  //longer than 8
  case -16:
    return "Not a variable";  //longer than 32
  case -17:
    return "Not an integer";  //may be missing IDS table in action/trigger
  case -18:
    return "No such IDS file"; //may be a missing IDS file
  case -19:
    return "Too many string arguments"; //1 area+2 variables or 1 resource, or 2 resources
    //didn't pass range checks
  case -20:
    return "Invalid tlk reference"; //strref >= tlk_entrynum
    //didn't pass recursive matching of nested actions or objects
  case -22:
    return "Not a valid object.ids value"; //listed in IDS
  case -30:
    return "Maximum function nesting level exceeded";
  case -31:
    return "Maximum object nesting level exceeded";
    //action or trigger sequencing is incorrect
  case -40:
    return "Triggers after False()";
  case -41:
    return "No need of this True() trigger";
  case -42:
    return "Not enough triggers after Or(x)";
  case -43:
    return "Actions after Continue()";
  case -44:
    return "Empty top level condition, use True()";

  case -100:
    return "Bad IF";
  case -110:
    return "Bad RESPONSE";
  case -120:
    return "Bad THEN";
  case -130:
    return "Bad END";
  default:
    return "Unknown error";
  }
}

#define ST_QUOTE '"'
#define ST_TILDE '~'

//clever explode, can recognise brackets
CString *explode2(const CString &array, int &count)
{
  CStringList internal;
  CString *retval;
  POSITION pos;
  int bracketlevel;  // ()
  int bracketlevel2; // []
  int quotes;
  int p1, p2;
  int maxlen;
  
  bracketlevel=0;
  bracketlevel2=0;
  quotes=0;
  count=0;
  if(array.IsEmpty()) 
  {
    return 0; //nothing to do
  }
  p1=0;
  maxlen=array.GetLength();
  for(p2=0;p2<maxlen;p2++)
  {
    switch(array.GetAt(p2))
    {
    case ST_TILDE:
      if(quotes==ST_QUOTE) break;
      if(editflg&WEIDUSTRING) break; //don't do anything if weidu isn't allowed
      if(quotes==ST_TILDE) quotes=0;
      else quotes=ST_TILDE;
      break;
    case ST_QUOTE:
      if(quotes==ST_TILDE) break;
      if(quotes==ST_QUOTE) quotes=0;
      else quotes=ST_QUOTE;
      break;
    case ',':
      if(quotes==ST_TILDE) break;
      if(quotes)
      {
        count=-6;  //comma in quotes ???
        return 0;
      }
      if(!bracketlevel && !bracketlevel2)
      {//store internally
        internal.AddTail(array.Mid(p1, p2-p1));
        p1=p2+1;
      }
      break;
    case ']':
      if(quotes==ST_TILDE) break;
      if(quotes)
      {
        count=-6;  //bracket in quotes ???
        return 0;
      }
      if(bracketlevel2) bracketlevel2--;
      else
      {
        count=-7; //error with bracket level2
        return 0;
      }
      break;
    case '[':
      if(quotes==ST_TILDE) break;
      if(quotes)
      {
        count=-6;  //bracket in quotes ???
        return 0;
      }
      if(bracketlevel2)
      {
        count=-2; //no nested square brackets are allowed
        return 0;
      }
      bracketlevel2++;
      break;
    case ')': 
      if(quotes==ST_TILDE) break;
      if(quotes)
      {
        count=-6;  //bracket in quotes ???
        return 0;
      }
      if(bracketlevel) bracketlevel--;
      else
      {
        count=-7; //error with bracket level
        return 0;
      }
      break;
    case '(':
      if(quotes==ST_TILDE) break;
      if(quotes)
      {
        count=-6;  //bracket in quotes ???
        return 0;
      }
      if(bracketlevel2)
      {
        count=-2; //no brackets are allowed in square brackets
        return 0;
      }
      bracketlevel++;
      break;
    }
  }
  if(bracketlevel || bracketlevel2)
  {
    count=-2;
    return 0;
  }
  if(quotes)
  {
    count=-6;
    return 0;
  }
  if(array.GetAt(p2-1)!=',')
  {
    internal.AddTail(array.Mid(p1)); //add last element
  }
  else
  {
    count=-3;  //, at the end ???
    return 0;
  }
  
  count=internal.GetCount();
  retval=new CString[count];
  if(retval)
  {
    pos=internal.GetHeadPosition();
    for(p1=0;p1<count;p1++)
    {
      retval[p1]=internal.GetNext(pos);
      retval[p1].TrimLeft();
      retval[p1].TrimRight();
    }
  }
  else
  {
    count=-99; //internal error
  }
  return retval;
}

CString implode(CString *array, int count)
{
  CString ret;
  int i;

  ret.Empty();
  if(count)
  {
    for(i=0;i<count-1;i++)
    {
      ret+=array[i]+"\r\n";
    }
    ret+=array[i];
  }
  return ret;
}

//skips spaces
CString *explode(const CString &array, char separator, int &count)
{
  CString *retval;
  int pos, pos2;
  int i;
  int flg;
  
  if(array.IsEmpty()) 
  {
    count=0;
    return 0;
  }
  if(array.Right(1)!=separator) 
  {
    count=1;
  }
  else count=0;
  flg=0;
  for(i=0;i<array.GetLength();i++)
  {
    if(array.GetAt(i)==separator)
    {
      count++;
      while(i<array.GetLength() && array.GetAt(i)==' ') i++;
    }
  }
  retval=new CString[count];
  if(retval)
  {
    pos=0;
    i=0;
    do
    {
      pos2=array.Find(separator,pos);
      if(pos2==-1) pos2=array.GetLength()+1;
      retval[i]=array.Mid(pos,pos2-pos);
      retval[i].TrimLeft(); //iwd2 actions/triggers parameterlist contained space
      retval[i].TrimRight();//OUTRAGEOUS!
      i++;
      pos=pos2+1;
      while(pos<array.GetLength() && array.GetAt(pos)==' ') pos++;
    }
    while(i<count);
  }
  else
  {
    count=-1;
  }
  return retval;
}

#define setflags(a,b)  return ((b<<8)|a)
#define sf(a,b)  ((b<<8)|a)

#define AO sf(CHECK_AREA, IS_VAR)
#define AA sf(CHECK_AREA, CHECK_AREA)
#define aO sf(CHECK_AREA2, IS_VAR)
#define Am sf(CHECK_AREA, CHECK_MOS2)
#define Av sf(CHECK_AREA, CHECK_VVC2)
#define AE sf(CHECK_AREA, ENTRY_POINT)
#define bO sf(CHECK_BCS2,IS_VAR)
#define BO sf(CHECK_BCS,IS_VAR)
#define by CHECK_BYTE
#define CO sf(CHECK_CRE,IS_VAR)
#define Cv sf(CHECK_CRE,CHECK_VVC2)
#define CD sf(CHECK_CRE,ADD_DEAD)
#define Cd sf(CHECK_CRE,ADD_DEAD2)
#define DO sf(CHECK_DLG, IS_VAR)
#define dO sf(CHECK_DLG2, IS_VAR)
#define EO sf(CHECK_NUM, IS_VAR)
#define DV sf(ADD_DEAD, IS_VAR)
#define IO sf(CHECK_ITEM, IS_VAR)
#define iO sf(CHECK_ITEM2, IS_VAR)
#define io sf(CHECK_ITEM, IS_VAR) | CHECK_STRREF
#define II sf(CHECK_ITEM, CHECK_ITEM)
#define IV sf(CHECK_ITEM, ADD_VAR)
#define iv sf(CHECK_ITEM, ADD_VAR) |CHECK_STRREF
#define GO sf(ADD_GLOBAL, IS_VAR)
#define GG sf(ADD_GLOBAL,ADD_GLOBAL)
#define LO sf(ADD_LOCAL, IS_VAR)
#define LL sf(ADD_LOCAL,ADD_LOCAL)
#define NC sf(NO_CHECK, IS_VAR)
#define MO sf(CHECK_MOVIE, IS_VAR)
#define MV sf(0,MERGE_VARS)
#define mv sf(1,MERGE_VARS)
#define OO sf(IS_VAR,IS_VAR)
#define PO sf(CHECK_SPL, IS_VAR)
#define pO sf(CHECK_SPL2, IS_VAR)
#define sD sf(CHECK_SOUND2, CHECK_DSOUND)
#define SO sf(CHECK_SOUND, IS_VAR)
#define sO sf(CHECK_SOUND2, IS_VAR)
#define sr sf(CHECK_SRC, IS_VAR)
#define st CHECK_STRREF
#define jr CHECK_JOURNAL
#define TO sf(CHECK_STORE, IS_VAR)
#define VO sf(ADD_VAR, IS_VAR)
#define TV sf(VALID_TAG,IS_VAR)
#define tv sf(VALID_TAG,IS_VAR)|CHECK_STRREF
#define VC sf(ADD_VAR, CHECK_CRE)
#define VI sf(ADD_VAR, CHECK_ITEM)
#define vO sf(ADD_VAR2, IS_VAR)
#define VT sf(ADD_VAR, VALID_TAG)
#define VV sf(ADD_VAR, ADD_VAR)
#define vv sf(ADD_VAR2, ADD_VAR2)
#define WO sf(CHECK_2DA, IS_VAR)
#define wO sf(CHECK_2DA2, IS_VAR)
#define XO sf(CHECK_VVC, IS_VAR)
#define xO sf(CHECK_VVC2, IS_VAR)
#define XP sf(CHECK_XPLIST, IS_VAR)
#define xp sf(CHECK_XPLIST, IS_VAR)|CHECK_STRREF
#define YO sf(CHECK_GAME,IS_VAR)

int tob_trigger_flags[MAX_TRIGGER]={
//0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 
    OO,   IO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   IO,  
//0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 
    OO,   OO,   OO,   OO,   OO,   VO,   OO,   OO,   OO,   OO,  
//0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
//0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
//0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   pO,  
//0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 
    OO,   OO,   VO,   VO,   OO,   OO,   OO,   OO,   OO,   OO,  
//0x3c, 0x3d, 0x3e, 0x3f, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 
    OO,   OO,   OO,   mv,   mv,   mv,   IO,   OO,   OO,   OO,  
//0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
//0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 
    OO,   DV,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
//0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 0x60, 0x61, 0x62, 0x63, 
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   IO,   OO,   OO,  
//0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
//0x6e, 0x6f, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 
    OO,   OO,   OO,   DV,   DV,   DV,   OO,   IO,   OO,   IO,  
//0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f, 0x80, 0x81, 
    IO,   IO,   IO,   IO,   IO,   OO,   AO,   IO,   OO,   OO,  
//0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
//0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
//0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f, 
    OO,   OO,   GG,   GG,   GG,   LL,   LL,   LL,   OO,   OO,  
//0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 
    OO,   OO,   OO,   OO,   OO,   DV,   OO,   OO,   OO,   IO,  
//0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf, 0xb0, 0xb1, 0xb2, 0xb3, 
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
//0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 
    OO,   mv,   mv,   mv,   OO,   OO,   OO,   OO,   OO,   OO,  
//0xbe, 0xbf, 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 
    DV,   OO,   OO,   OO,   IO,   OO,   OO,   OO,   GO,   GO,  
//0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0xd0, 0xd1, 
    GO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
//0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 
    OO,   OO,   AO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
//0xdc, 0xdd, 0xde, 0xdf, 0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
//0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef, 
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
//0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
//0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff, 
    OO,   OO,   OO,   OO,   OO,   OO, 
};

int tob_action_flags[MAX_ACTION]={
//  0,    1,    2,    3,    4,    5,    6,    7,    8,    9,  
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   CO,   OO,   IO,  
// 10,   11,   12,   13,   14,   15,   16,   17,   18,   19,  
    OO,   IO,   OO,   OO,   IO,   IO,   OO,   OO,   OO,   OO,  
// 20,   21,   22,   23,   24,   25,   26,   27,   28,   29,  
    OO,   OO,   OO,   OO,   OO,   OO,   SO,   OO,   OO,   OO,  
// 30,   31,   32,   33,   34,   35,   36,   37,   38,   39,  
    VO,   pO,   OO,   OO,   iO,   OO,   OO,   OO,   OO,   OO,  
// 40,   41,   42,   43,   44,   45,   46,   47,   48,   49,  
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
// 50,   51,   52,   53,   54,   55,   56,   57,   58,   59,  
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
// 60,   61,   62,   63,   64,   65,   66,   67,   68,   69,  
    bO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
// 70,   71,   72,   73,   74,   75,   76,   77,   78,   79,  
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
// 80,   81,   82,   83,   84,   85,   86,   87,   88,   89,  
    OO,   OO,   IO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
// 90,   91,   92,   93,   94,   95,   96,   97,   98,   99,  
    OO,   AO,   OO,   OO,   OO,   pO,   OO,   IO,   OO,   OO,  
//100,  101,  102,  103,  104,  105,  106,  107,  108,  109,  
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   aO,   VO,  
//110,  111,  112,  113,  114,  115,  116,  117,  118,  119,  
    Am,   OO,   OO,   pO,   pO,   VO,   IO,   OO,   OO,   OO,  
//120,  121,  122,  123,  124,  125,  126,  127,  128,  129,  
    BO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
//130,  131,  132,  133,  134,  135,  136,  137,  138,  139,  
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   DO,   dO,   OO,  
//140,  141,  142,  143,  144,  145,  146,  147,  148,  149,  
    IO,   MV,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
//150,  151,  152,  153,  154,  155,  156,  157,  158,  159,  
    TO,   st,   OO,   by,   by,   by,   by,   by,   by,   by,  
//160,  161,  162,  163,  164,  165,  166,  167,  168,  169,  
    pO,   wO,   OO,   OO,   OO,   mv,   OO,   MO,   OO,   IO,  //165 VO->mv
//170,  171,  172,  173,  174,  175,  176,  177,  178,  179,  
    AO,   OO,   OO,   jr,   OO,   OO,   OO,   OO,   OO,   OO,  
//180,  181,  182,  183,  184,  185,  186,  187,  188,  189,  
    OO,   pO,   OO,   OO,   OO,   AO,   OO,   OO,   IO,   Am,  
//190,  191,  192,  193,  194,  195,  196,  197,  198,  199,  
    OO,   pO,   pO,   IO,   CO,   OO,   OO,   AO,   OO,   WO,  //190 st->OO
//200,  201,  202,  203,  204,  205,  206,  207,  208,  209,  
    OO,   OO,   OO,   OO,   IO,   OO,   OO,   OO,   OO,   OO,  
//210,  211,  212,  213,  214,  215,  216,  217,  218,  219,  
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   CO,  
//220,  221,  222,  223,  224,  225,  226,  227,  228,  229,  
    IO,   OO,   OO,   OO,   OO,   Av,   WO,   Cv,   CO,   OO,  
//230,  231,  232,  233,  234,  235,  236,  237,  238,  239,  
    OO,   CO,   CO,   CO,   OO,   jr,   OO,   OO,   CO,   OO,  
//240,  241,  242,  243,  244,  245,  246,  247,  248,  249,  
    pO,   OO,   OO,   OO,   VO,   MV,   VC,   tv,   TV,   OO,  
//250,  251,  252,  253,  254,  255,  256,  257,  258,  259,  
    Cv,   AO,   CO,   OO,   OO,   GG,   VI,   IO,   OO,   OO,  
//260,  261,  262,  263,  264,  265,  266,  267,  268,  269,  
    OO,   OO,   st,   jr,   AO,   OO,   DO,   OO,   VO,   st,  
//270,  271,  272,  273,  274,  275,  276,  277,  278,  279,  
    OO,   OO,   xO,   xO,   OO,   OO,   OO,   aO,   II,   PO,  
//280,  281,  282,  283,  284,  285,  286,  287,  288,  289,  
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   st,   OO,  
//290,  291,  292,  293,  294,  295,  296,  297,  298,  299,  
    OO,   OO,   io,   DO,   DO,   CO,   OO,   VO,   OO,   OO,  
//300,  301,  302,  303,  304,  305,  306,  307,  308,  309,  
    OO,   OO,   OO,   GO,   mv,   LO,   OO,   GO,   st,   OO,  
//310,  311,  312,  313,  314,  315,  316,  317,  318,  319,  
    AA,   st,   OO,   OO,   OO,   OO,   OO,   OO,   pO,   pO,  
//320,  321,  322,  323,  324,  325,  326,  327,  328,  329,  
    st,   OO,   OO,   CO,   OO,   OO,   OO,   XP,   st,   OO,  
//330,  331,  332,  333,  334,  335,  336,  337,  338,  339,  
    OO,   OO,   OO,   OO,   DO,   VT,   OO,   pO,   OO,   OO,  
//340,  341,  342,  343,  344,  345,  346,  347,  348,  349,  
    OO,   SO,   st,   OO,   OO,   OO,   st,   AA,   OO,   OO,  
//350,  351,  352,  353,  354,  355,  356,  357,  358,  359,  
    AE,   AE,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
};

int iwd2_trigger_flags[MAX_TRIGGER]={
//0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 
    OO,   IO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   IO,  
//0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 
    OO,   OO,   OO,   OO,   OO,   VO,   OO,   OO,   OO,   OO,  
//0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
//0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
//0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   pO,  
//0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 
    OO,   OO,   VO,   VO,   OO,   OO,   OO,   OO,   OO,   OO,  
//0x3c, 0x3d, 0x3e, 0x3f, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 
    OO,   OO,   OO,   mv,   mv,   mv,   IO,   OO,   OO,   OO,  
//0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
//0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
//0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 0x60, 0x61, 0x62, 0x63, 
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   IO,   OO,   OO,  
//0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
//0x6e, 0x6f, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 
    OO,   OO,   OO,   DV,   DV,   DV,   OO,   IO,   OO,   IO,  
//0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f, 0x80, 0x81, 
    IO,   IO,   IO,   IO,   IO,   OO,   AO,   IO,   OO,   OO,  
//0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
//0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
//0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f, 
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
//0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 
    OO,   OO,   OO,   OO,   OO,   VO,   vv,   OO,   OO,   OO,  
//0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf, 0xb0, 0xb1, 0xb2, 0xb3, 
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
//0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
//0xbe, 0xbf, 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 
    OO,   OO,   OO,   IO,   IO,   IO,   OO,   IO,   DO,   OO,  
//0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0xd0, 0xd1, 
    PO,   VO,   OO,   OO,   OO,   OO,   OO,   OO,   DV,   OO,  
//0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 
    OO,   OO,   OO,   OO,   IO,   OO,   OO,   OO,   OO,   OO,  
//0xdc, 0xdd, 0xde, 0xdf, 0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   VO,   OO,   OO,  
//0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef, 
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
//0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 
    IO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
//0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff, 
    OO,   OO,   OO,   OO,   OO,   OO, 
};

int iwd2_action_flags[MAX_ACTION]={
//  0,    1,    2,    3,    4,    5,    6,    7,    8,    9,  
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   Cd,   OO,   IO,  
// 10,   11,   12,   13,   14,   15,   16,   17,   18,   19,  
    OO,   IO,   OO,   OO,   IO,   IO,   OO,   OO,   OO,   OO,  
// 20,   21,   22,   23,   24,   25,   26,   27,   28,   29,  
    OO,   OO,   OO,   OO,   OO,   OO,   SO,   OO,   OO,   OO,  
// 30,   31,   32,   33,   34,   35,   36,   37,   38,   39,  
    VO,   pO,   OO,   OO,   iO,   OO,   OO,   OO,   OO,   OO,  
// 40,   41,   42,   43,   44,   45,   46,   47,   48,   49,  
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
// 50,   51,   52,   53,   54,   55,   56,   57,   58,   59,  
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
// 60,   61,   62,   63,   64,   65,   66,   67,   68,   69,  
    bO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
// 70,   71,   72,   73,   74,   75,   76,   77,   78,   79,  
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
// 80,   81,   82,   83,   84,   85,   86,   87,   88,   89,  
    OO,   OO,   IO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
// 90,   91,   92,   93,   94,   95,   96,   97,   98,   99,  
    OO,   AO,   OO,   OO,   OO,   pO,   OO,   IO,   OO,   OO,  
//100,  101,  102,  103,  104,  105,  106,  107,  108,  109,  
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   VO,  
//110,  111,  112,  113,  114,  115,  116,  117,  118,  119,  
    Am,   OO,   OO,   pO,   pO,   VO,   IO,   OO,   OO,   OO,  
//120,  121,  122,  123,  124,  125,  126,  127,  128,  129,  
    BO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
//130,  131,  132,  133,  134,  135,  136,  137,  138,  139,  
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   DO,   dO,   OO,  
//140,  141,  142,  143,  144,  145,  146,  147,  148,  149,  
    IO,   MV,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
//150,  151,  152,  153,  154,  155,  156,  157,  158,  159,  
    TO,   st,   OO,   by,   by,   by,   by,   by,   by,   by,  
//160,  161,  162,  163,  164,  165,  166,  167,  168,  169,  
    pO,   wO,   OO,   OO,   OO,   VO,   OO,   MO,   OO,   IO,  
//170,  171,  172,  173,  174,  175,  176,  177,  178,  179,  
    AO,   OO,   OO,   jr,   OO,   OO,   OO,   OO,   OO,   OO,  
//180,  181,  182,  183,  184,  185,  186,  187,  188,  189,  
    OO,   pO,   OO,   OO,   OO,   AO,   OO,   OO,   IO,   Am,  
//190,  191,  192,  193,  194,  195,  196,  197,  198,  199,  
    OO,   pO,   pO,   IO,   CO,   OO,   OO,   AO,   OO,   WO,  
//200,  201,  202,  203,  204,  205,  206,  207,  208,  209,  
    OO,   OO,   OO,   OO,   IO,   OO,   OO,   OO,   OO,   OO,  
//210,  211,  212,  213,  214,  215,  216,  217,  218,  219,  
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   CO,  
//220,  221,  222,  223,  224,  225,  226,  227,  228,  229,  
    OO,   OO,   OO,   OO,   OO,   Av,   WO,   Cv,   CO,   OO,  
//230,  231,  232,  233,  234,  235,  236,  237,  238,  239,  
    OO,   OO,   OO,   OO,   NC,   OO,   OO,   OO,   xp,   OO,  
//240,  241,  242,  243,  244,  245,  246,  247,  248,  249,  
    OO,   st,   OO,   vv,   jr,   jr,   OO,   vO,   vv,   OO,  
//250,  251,  252,  253,  254,  255,  256,  257,  258,  259,  
    OO,   OO,   bO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
//260,  261,  262,  263,  264,  265,  266,  267,  268,  269,  
    OO,   OO,   OO,   OO,   OO,   OO,   st,   YO,   OO,   OO,  
//270,  271,  272,  273,  274,  275,  276,  277,  278,  279,  
    OO,   OO,   OO,   OO,   OO,   st,   OO,   OO,   IO,   OO,  
//280,  281,  282,  283,  284,  285,  286,  287,  288,  289,  
    OO,   OO,   AO,   OO,   BO,   OO,   OO,   OO,   OO,   sD,  
//290,  291,  292,  293,  294,  295,  296,  297,  298,  299,  
    OO,   OO,   OO,   OO,   IO,   OO,   TO,   OO,   OO,   OO,  
//300,  301,  302,  303,  304,  305,  306,  307,  308,  309,  
    OO,   OO,   OO,   OO,   OO,   OO,   VO,   VO,   VO,   OO,  
//310,  311,  312,  313,  314,  315,  316,  317,  318,  319,  
    OO,   OO,   st,   st,   st,   OO,   OO,   OO,   OO,   OO,  
//320,  321,  322,  323,  324,  325,  326,  327,  328,  329,  
    SO,   EO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
//330,  331,  332,  333,  334,  335,  336,  337,  338,  339,  
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
//340,  341,  342,  343,  344,  345,  346,  347,  348,  349,  
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
//350,  351,  352,  353,  354,  355,  356,  357,  358,  359,  
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
};

int pst_trigger_flags[MAX_TRIGGER]={
//0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 
    OO,   IO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   IO,  
//0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 
    OO,   OO,   OO,   OO,   OO,   VO,   OO,   OO,   OO,   OO,  
//0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
//0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
//0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
//0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 
    OO,   OO,   VO,   VO,   OO,   OO,   OO,   OO,   OO,   OO,  
//0x3c, 0x3d, 0x3e, 0x3f, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 
    OO,   OO,   OO,   mv,   mv,   mv,   IO,   OO,   OO,   OO,  
//0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
//0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 
    OO,   DV,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
//0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 0x60, 0x61, 0x62, 0x63, 
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   IO,   OO,   OO,  
//0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
//0x6e, 0x6f, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   IO,   OO,   IO,  
//0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f, 0x80, 0x81, 
    IO,   IO,   IO,   IO,   IO,   OO,   AO,   VO,   VO,   VO,  
//0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 
    VV,   VV,   VV,   VV,   VV,   VV,   VV,   OO,   OO,   OO,  
//0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 
    IO,   IO,   IO,   IO,   IO,   IO,   OO,   DO,   OO,   VO,  
//0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f, 
    OO,   OO,   OO,   OO,   OO,   OO,   VO,   OO,   OO,   OO,  
//0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 
    BO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
//0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf, 0xb0, 0xb1, 0xb2, 0xb3, 
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
//0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
//0xbe, 0xbf, 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
//0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0xd0, 0xd1, 
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
//0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
//0xdc, 0xdd, 0xde, 0xdf, 0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
//0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef, 
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
//0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
//0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff, 
    OO,   OO,   OO,   OO,   OO,   OO,   
};

int pst_action_flags[MAX_ACTION]={
//  0,    1,    2,    3,    4,    5,    6,    7,    8,    9,  
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   CO,   OO,   OO,  
// 10,   11,   12,   13,   14,   15,   16,   17,   18,   19,  
    OO,   IO,   OO,   OO,   IO,   IO,   OO,   OO,   OO,   OO,  
// 20,   21,   22,   23,   24,   25,   26,   27,   28,   29,  
    OO,   OO,   OO,   OO,   OO,   OO,   SO,   OO,   OO,   OO,  
// 30,   31,   32,   33,   34,   35,   36,   37,   38,   39,  
    VO,   OO,   OO,   OO,   IO,   OO,   OO,   OO,   OO,   OO,  
// 40,   41,   42,   43,   44,   45,   46,   47,   48,   49,  
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
// 50,   51,   52,   53,   54,   55,   56,   57,   58,   59,  
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
// 60,   61,   62,   63,   64,   65,   66,   67,   68,   69,  
    bO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
// 70,   71,   72,   73,   74,   75,   76,   77,   78,   79,  
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
// 80,   81,   82,   83,   84,   85,   86,   87,   88,   89,  
    OO,   OO,   IO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
// 90,   91,   92,   93,   94,   95,   96,   97,   98,   99,  
    OO,   AO,   OO,   OO,   OO,   OO,   OO,   IO,   OO,   OO,  
//100,  101,  102,  103,  104,  105,  106,  107,  108,  109,  
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   VO,  
//110,  111,  112,  113,  114,  115,  116,  117,  118,  119,  
    AO,   OO,   OO,   OO,   OO,   VO,   IO,   OO,   OO,   OO,  
//120,  121,  122,  123,  124,  125,  126,  127,  128,  129,  
    BO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
//130,  131,  132,  133,  134,  135,  136,  137,  138,  139,  
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   DO,   dO,   OO,  
//140,  141,  142,  143,  144,  145,  146,  147,  148,  149,  
    IO,   MV,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
//150,  151,  152,  153,  154,  155,  156,  157,  158,  159,  
    TO,   OO,   OO,   by,   by,   by,   by,   by,   by,   by,  
//160,  161,  162,  163,  164,  165,  166,  167,  168,  169,  
    OO,   wO,   OO,   OO,   OO,   VO,   OO,   MO,   OO,   IO,  
//170,  171,  172,  173,  174,  175,  176,  177,  178,  179,  
    OO,   OO,   OO,   jr,   OO,   OO,   OO,   OO,   OO,   OO,  
//180,  181,  182,  183,  184,  185,  186,  187,  188,  189,  
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
//190,  191,  192,  193,  194,  195,  196,  197,  198,  199,  
    OO,   OO,   OO,   IO,   OO,   OO,   OO,   OO,   OO,   OO,  
//200,  201,  202,  203,  204,  205,  206,  207,  208,  209,  
    OO,   OO,   VV,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
//210,  211,  212,  213,  214,  215,  216,  217,  218,  219,  
    OO,   OO,   sr,   sr,   OO,   OO,   OO,   OO,   OO,   OO,  
//220,  221,  222,  223,  224,  225,  226,  227,  228,  229,  
    OO,   OO,   IO,   II,   II,   OO,   OO,   VO,   VO,   VO,  
//230,  231,  232,  233,  234,  235,  236,  237,  238,  239,  
    VO,   VO,   VO,   VV,   VV,   VV,   VV,   VV,   VV,   VV,  
//240,  241,  242,  243,  244,  245,  246,  247,  248,  249,  
    VV,   VV,   VV,   VV,   VO,   VO,   BO,   OO,   OO,   OO,  
//250,  251,  252,  253,  254,  255,  256,  257,  258,  259,  
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
//260,  261,  262,  263,  264,  265,  266,  267,  268,  269,  
    VO,   VV,   OO,   OO,   IO,   OO,   OO,   OO,   OO,   OO,  
//270,  271,  272,  273,  274,  275,  276,  277,  278,  279,  
    OO,   OO,   DO,   OO,   OO,   VO,   OO,   OO,   OO,   OO,  
//280,  281,  282,  283,  284,  285,  286,  287,  288,  289,  
    OO,   AO,   OO,   OO,   OO,   OO,   NC,   NC,   OO,   OO,  
//290,  291,  292,  293,  294,  295,  296,  297,  298,  299,  
    OO,   XO,   XO,   OO,   AO,   CO,   OO,   OO,   OO,   OO,  
//300,  301,  302,  303,  304,  305,  306,  307,  308,  309,  
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   II,   II,   OO,  
//310,  311,  312,  313,  314,  315,  316,  317,  318,  319,  
    BO,   OO,   SO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
//320,  321,  322,  323,  324,  325,  326,  327,  328,  329,  
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
//330,  331,  332,  333,  334,  335,  336,  337,  338,  339,  
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
//340,  341,  342,  343,  344,  345,  346,  347,  348,  349,  
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,  
//350,  351,  352,  353,  354,  355,  356,  357,  358,  359,  
    OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,   OO,
};
/*
void outputtable()
{
  CString tmpstr;
  FILE *fpoi;
  int i,j,flags;

  fpoi=fopen("outputtable","wt");
  for(i=0;i<MAX_TRIGGER;i+=10)
  {
    fprintf(fpoi,"//");
    for(j=i;j<i+10;j++)
    {
      fprintf(fpoi,"0x%02x, ",j);
    }
    fprintf(fpoi,"\n   ");
    for(j=i;j<i+10;j++)
    {
      flags=handle_trigger(j);
      if(tob_action_flags[j]!=flags)
      {
        nop();
      }
      switch(flags)
      {
      case OO: tmpstr="OO"; break;
      case DV: tmpstr="DV"; break;
      case LO: tmpstr="LO"; break;
      case GO: tmpstr="GO"; break;
      case GG: tmpstr="GG"; break;
      case LL: tmpstr="LL"; break;
      case VO: tmpstr="VO"; break;
      case VC: tmpstr="VC"; break;
      case VI: tmpstr="VI"; break;
      case VT: tmpstr="VT"; break;
      case TV: tmpstr="TV"; break;
      case tv: tmpstr="tv"; break;
      case vO: tmpstr="vO"; break;
      case VV: tmpstr="VV"; break;
      case vv: tmpstr="vv"; break;
      case CO: tmpstr="CO"; break;
      case Cv: tmpstr="Cv"; break;
      case CD: tmpstr="CD"; break;
      case Cd: tmpstr="Cd"; break;
      case II: tmpstr="II"; break;
      case IO: tmpstr="IO"; break;
      case iO: tmpstr="iO"; break;
      case io: tmpstr="io"; break;
      case MO: tmpstr="MO"; break;
      case SO: tmpstr="SO"; break;
      case sO: tmpstr="sO"; break;
      case sD: tmpstr="sD"; break;
      case PO: tmpstr="PO"; break;
      case pO: tmpstr="pO"; break;
      case TO: tmpstr="TO"; break;
      case AO: tmpstr="AO"; break;
      case AA: tmpstr="AA"; break;
      case AE: tmpstr="AE"; break;
      case aO: tmpstr="aO"; break;
      case Am: tmpstr="Am"; break;
      case Av: tmpstr="Av"; break;
      case BO: tmpstr="BO"; break;
      case bO: tmpstr="bO"; break;
      case DO: tmpstr="DO"; break;
      case dO: tmpstr="dO"; break;
      case WO: tmpstr="WO"; break;
      case wO: tmpstr="wO"; break;
      case XO: tmpstr="XO"; break;
      case xO: tmpstr="xO"; break;
      case XP: tmpstr="XP"; break;
      case xp: tmpstr="xp"; break;
      case NC: tmpstr="NC"; break;
      case MV: tmpstr="MV"; break;
      case mv: tmpstr="mv"; break;
      case st: tmpstr="st"; break;
      case by: tmpstr="by"; break;
      default: tmpstr="**"; break;
      }
      fprintf(fpoi," %s,  ",tmpstr);
    }
    fprintf(fpoi,"\n");
  }
  fclose(fpoi);
}
*/
int handle_action(int opcode)
{
  if(opcode==165)
  {
    nop();
  }
  if(opcode>=MAX_ACTION || opcode<0) return OO;
  if(pst_compatible_var()) return pst_action_flags[opcode];
  if(has_xpvar() ) return iwd2_action_flags[opcode];
  return tob_action_flags[opcode];
}
int handle_trigger(int opcode)
{
  opcode&=~0x4000;
  if(opcode>=MAX_TRIGGER || opcode<0) return OO;
  if(pst_compatible_var()) return pst_trigger_flags[opcode];
  if(has_xpvar() ) return iwd2_trigger_flags[opcode];
  return tob_trigger_flags[opcode];
}
//validity checkers
int check_probability(int p1, int p2)
{
  return(p1>=p2 || p1>101 || p2>101);
}

inline int check_align_half(int x) { return (x<1) || (x>3); }

int check_alignment(long alignment)
{
  if(alignment<0 || alignment>0xff) return 1;
  if(check_align_half(alignment&15) ) return 1;
  return check_align_half(alignment>>4);
}

int check_ids_targeting(int par1, int par2)
{
  CString idsname;
  CStringMapInt *idsfile;
  POSITION pos;
  int ret;
  int value;

  if(par2<0 || par2>8) return 1; //invalid IDS file
  if(!par1) return 0;            //0 is always accepted
  if(par2<2) return 0;           //<2 is always accepted, check later
  idsname=IDSName(par2,true);
  ret=get_idsfile(idsname, false); //not forced
  if(ret) return ret;
  if(!idsmaps.Lookup(idsname, idsfile))
  {
    return -1; //internal error
  }
  pos=idsfile->GetStartPosition();
  while(pos)
  {
    idsfile->GetNextAssoc(pos, idsname, value);
    if(value==par1) return 0; //found it
  }
  return 1; //not found
}

CString lastopenedscript;
CString lastopenedsave;
CString lastopeneddata;

CString getitemname(CString filepath)
{
  filepath=filepath.Mid(filepath.ReverseFind('\\')+1 );
  filepath=filepath.Mid(filepath.ReverseFind('/')+1 );
  filepath=filepath.Left(filepath.Find(".") );
  return filepath.Left(8); //cutting it into max 8
}

CString makeitemname(CString ext, int remember)
{
  CString tmpstr;

  switch(remember)
  {
  case 1:
    if(!lastopenedscript.IsEmpty() && (editflg&LASTOPENED) )
    {
      if(itemname.Left(4)=="new ") return lastopenedscript+"\\*"+ext;
      return lastopenedscript+"\\"+itemname+ext;
    }
    break;
  case 2:
    if(!lastopenedsave.IsEmpty() && (editflg&DECOMPRESS) )
    {
      if(itemname.Left(4)=="new ") return lastopenedsave+"\\*"+ext;
      return lastopenedsave+"\\"+itemname+ext;
    }
    break;
  case 3:
    if(!lastopeneddata.IsEmpty() && (editflg&DATAFOLDER) )
    {
      if(itemname.Left(4)=="new ") return lastopeneddata+"\\*"+ext;
      return lastopeneddata+"\\"+itemname+ext;
    }
    break;
  }
  if(ext==".sav")
  {
    if(has_xpvar()) tmpstr="mpsave\\"; //iwd1/iwd2 
    else tmpstr="save\\";
  }
  else if((ext==".cbf") || (ext==".bif") ) tmpstr="data\\";
  else if(ext==".baf" || ext==".d") tmpstr=weidudecompiled+"\\";
  else if(ext==".mus" || ext==".acm") tmpstr="music\\";
  else if(ext==".iap" || ext==".tbg") tmpstr="";
  else tmpstr="override\\";
  if(itemname.Left(4)=="new " && ext.GetLength()) return bgfolder+tmpstr+"*"+ext;
  return bgfolder+tmpstr+itemname+ext;
}

//after write, this should be called
void UpdateIEResource(CString key, int restype, CString filepath, int size)
{
  loc_entry fileloc;
  int ext;
  
  ext=determinetype(restype);
  if(!ext)
  {
    return;
  }

  if(resources[ext]->Lookup(key,fileloc))
  {
    fileloc.bifindex=0xffff;
    fileloc.bifname=filepath;
    fileloc.size=size;
  }
  else
  {
    fileloc.bifindex=0xffff;
    fileloc.bifname=filepath;
    fileloc.index=-2;
    fileloc.size=size;
  }
  resources[ext]->SetAt(key,fileloc);
}

///////// Readers & writers/////////////////////

int write_area(CString key)
{
  loc_entry fileloc;
  int ret;
  CString filepath, tmpath;
  int size;
  int fhandle;

  filepath=bgfolder+"override\\"+key+".ARE";
  tmpath=bgfolder+"override\\"+key+".TMP";

  fhandle=open(tmpath, O_BINARY|O_RDWR|O_CREAT|O_TRUNC,S_IREAD|S_IWRITE);
  if(fhandle<1)
  {
    return -2;
  }
  ret=the_area.WriteAreaToFile(fhandle,0);
  size=filelength(fhandle);
  close(fhandle);
  if(!ret && (size>0) )
  {
    unlink(filepath);
    rename(tmpath,filepath);
    filepath="override\\"+key+".ARE";
    UpdateIEResource(key,REF_ARE,filepath,size);
  }
  return ret;
}

int write_tis(CString key)
{
  loc_entry fileloc;
  int ret;
  CString filepath, tmpath;
  int size;
  int fhandle;

  filepath=bgfolder+"override\\"+key+".TIS";
  tmpath=bgfolder+"override\\"+key+".TMP";

  fhandle=open(tmpath, O_BINARY|O_RDWR|O_CREAT|O_TRUNC,S_IREAD|S_IWRITE);
  if(fhandle<1)
  {
    return -2;
  }
  ret=the_mos.WriteTisToFile(fhandle,0);
  size=filelength(fhandle);
  close(fhandle);
  if(!ret && (size>0) )
  {
    unlink(filepath);
    rename(tmpath,filepath);
    filepath="override\\"+key+".TIS";
    UpdateIEResource(key,REF_TIS,filepath,size);
  }
  return ret;
}

int read_item(CString key)
{
  loc_entry fileloc;
  int ret;
  int fhandle;
  CString tmp;
  
  if(items.Lookup(key,fileloc))
  {
    fhandle=locate_file(fileloc, 0);
    if(fhandle<1) return -2;
    items.SetAt(key,fileloc);
    ret=the_item.ReadItemFromFile(fhandle, fileloc.size);
    close(fhandle);
  }
  else return -1;
  return ret;
}

int write_item(CString key)
{
  loc_entry fileloc;
  int ret;
  CString filepath, tmpath;
  int size;
  int fhandle;

  filepath=bgfolder+"override\\"+key+".ITM";
  tmpath=bgfolder+"override\\"+key+".TMP";

  fhandle=open(tmpath, O_BINARY|O_RDWR|O_CREAT|O_TRUNC,S_IREAD|S_IWRITE);
  if(fhandle<1)
  {
    return -2;
  }
  ret=the_item.WriteItemToFile(fhandle,0);
  size=filelength(fhandle);
  close(fhandle);
  if(!ret && (size>0) )
  {
    unlink(filepath);
    rename(tmpath,filepath);
    filepath="override\\"+key+".ITM";
    UpdateIEResource(key,REF_ITM,filepath,size);
  }
  return ret;
}

int read_spell(CString key)
{
  loc_entry fileloc;
  int ret;
  int fhandle;
  CString tmp;
  
  if(spells.Lookup(key,fileloc))
  {
    fhandle=locate_file(fileloc, 0);
    if(fhandle<1) return -2;
    spells.SetAt(key,fileloc);
    ret=the_spell.ReadSpellFromFile(fhandle, fileloc.size);
    close(fhandle);
  }
  else return -1;
  return ret;
}

int write_spell(CString key)
{
  loc_entry fileloc;
  int ret;
  CString filepath, tmpath;
  int size;
  int fhandle;

  filepath=bgfolder+"override\\"+key+".SPL";
  tmpath=bgfolder+"override\\"+key+".TMP";

  fhandle=open(tmpath, O_BINARY|O_RDWR|O_CREAT|O_TRUNC,S_IREAD|S_IWRITE);
  if(fhandle<1)
  {
    return -2;
  }
  ret=the_spell.WriteSpellToFile(fhandle,0);
  size=filelength(fhandle);
  close(fhandle);
  if(!ret && size>0)
  {
    unlink(filepath);
    rename(tmpath,filepath);
    filepath="override\\"+key+".SPL";
    UpdateIEResource(key,REF_SPL,filepath,size);
  }
  return ret;
}

int read_store(CString key)
{
  loc_entry fileloc;
  int ret;
  int fhandle;
  CString tmp;
  
  if(stores.Lookup(key,fileloc))
  {
    fhandle=locate_file(fileloc, 0);
    if(fhandle<1) return -2;
    items.SetAt(key,fileloc);
    ret=the_store.ReadStoreFromFile(fhandle, fileloc.size);
    close(fhandle);
  }
  else return -1;
  return ret;
}

int write_store(CString key)
{
  loc_entry fileloc;
  int ret;
  CString filepath, tmpath;
  int size;
  int fhandle;

  filepath=bgfolder+"override\\"+key+".STO";
  tmpath=bgfolder+"override\\"+key+".TMP";

  fhandle=open(tmpath, O_BINARY|O_RDWR|O_CREAT|O_TRUNC,S_IREAD|S_IWRITE);
  if(fhandle<1)
  {
    return -2;
  }
  ret=the_store.WriteStoreToFile(fhandle,0);
  size=filelength(fhandle);
  close(fhandle);
  if(!ret && size>0)
  {
    unlink(filepath);
    rename(tmpath,filepath);
    filepath="override\\"+key+".STO";
    UpdateIEResource(key,REF_SPL,filepath,size);
  }
  return ret;
}

int GetWed(bool night)
{
  if(night==the_area.m_night) return 0;
  if(the_area.WedChanged() )
  {
    if(MessageBox(0,"Do you want to discard the changes made on the other WED?","Area editor",MB_YESNO)==IDNO)
    {
      return -99;
    }
  }
  the_area.m_night=night;
  return ReadWed(0);
}

int ReadWed(int res)
{
  CString wed;
  loc_entry wedfileloc;
  int fhandle;

  RetrieveResref(wed,the_area.header.wed);
  if(the_area.m_night) wed+="N";
  if(weds.Lookup(wed,wedfileloc))
  {
    fhandle=locate_file(wedfileloc, 0);
    if(fhandle<1)
    {        
      res=3; //wedfile not available
    }
    else
    {
      switch(the_area.ReadWedFromFile(fhandle, wedfileloc.size) )
      {
      case 0: case 1: case 2: case 3: break;
      default:
        res=3;
      }
      close(fhandle);
    }
  }
  else res=3;
  return res;
}

int read_area(CString key)
{
  loc_entry fileloc, wedfileloc;
  int ret;
  int fhandle;
  CString tmp;
  
  if(areas.Lookup(key,fileloc))
  {
    fhandle=locate_file(fileloc, 0);
    if(fhandle<1) return -2;
    areas.SetAt(key,fileloc);
    ret=the_area.ReadAreaFromFile(fhandle, fileloc.size);
    close(fhandle);
    if(ret>=0)
    {
      ret=ReadWed(ret);
    }
  }
  else return -1;
  return ret;
}

int read_projectile(CString key)
{
  loc_entry fileloc;
  int ret;
  int fhandle;
  CString tmp;
  
  if(projects.Lookup(key,fileloc))
  {
    fhandle=locate_file(fileloc, 0);
    if(fhandle<1) return -2;
    projects.SetAt(key,fileloc);
    ret=the_projectile.ReadProjectileFromFile(fhandle, fileloc.size);
    close(fhandle);
  }
  else return -1;
  return ret;
}

int write_projectile(CString key)
{
  loc_entry fileloc;
  int ret;
  CString filepath, tmpath;
  int size;
  int fhandle;

  filepath=bgfolder+"override\\"+key+".PRO";
  tmpath=bgfolder+"override\\"+key+".TMP";

  fhandle=open(tmpath, O_BINARY|O_RDWR|O_CREAT|O_TRUNC,S_IREAD|S_IWRITE);
  if(fhandle<1)
  {
    return -2;
  }
  ret=the_projectile.WriteProjectileToFile(fhandle,0);
  size=filelength(fhandle);
  close(fhandle);
  if(!ret && size>0)
  {
    unlink(filepath);
    rename(tmpath,filepath);
    filepath="override\\"+key+".PRO";
    UpdateIEResource(key,REF_PRO,filepath,size);
  }
  return ret;
}

int read_chui(CString key)
{
  loc_entry fileloc;
  int ret;
  int fhandle;
  CString tmp;
  
  if(chuis.Lookup(key,fileloc))
  {
    fhandle=locate_file(fileloc, 0);
    if(fhandle<1) return -2;
    chuis.SetAt(key,fileloc);
    ret=the_chui.ReadChuiFromFile(fhandle, fileloc.size);
    close(fhandle);
  }
  else return -1;
  return ret;
}

int read_src(CString key)
{
  loc_entry fileloc;
  int ret;
  int fhandle;
  CString tmp;
  
  if(strings.Lookup(key,fileloc))
  {
    fhandle=locate_file(fileloc, 0);
    if(fhandle<1) return -2;
    strings.SetAt(key,fileloc);
    ret=the_src.ReadStringFromFile(fhandle, fileloc.size);
    close(fhandle);
  }
  else return -1;
  return ret;
}

int read_creature(CString key)
{
  loc_entry fileloc;
  int ret;
  int fhandle;
  CString tmp;
  
  if(creatures.Lookup(key,fileloc))
  {
    fhandle=locate_file(fileloc, 0);
    if(fhandle<1) return -2;
    creatures.SetAt(key,fileloc);
    ret=the_creature.ReadCreatureFromFile(fhandle, fileloc.size);
    close(fhandle);
  }
  else return -1;
  return ret;
}

int write_creature(CString key)
{
  loc_entry fileloc;
  int ret;
  CString filepath, tmpath;
  int size;
  int fhandle;

  filepath=bgfolder+"override\\"+key+".CRE";
  tmpath=bgfolder+"override\\"+key+".TMP";

  fhandle=open(tmpath, O_BINARY|O_RDWR|O_CREAT|O_TRUNC,S_IREAD|S_IWRITE);
  if(fhandle<1)
  {
    return -2;
  }
  ret=the_creature.WriteCreatureToFile(fhandle,0);
  size=filelength(fhandle);
  close(fhandle);
  if(!ret && size>0)
  {
    unlink(filepath);
    rename(tmpath,filepath);
    filepath="override\\"+key+".CRE";
    UpdateIEResource(key,REF_CRE,filepath,size);
  }
  return ret;
}

int read_dialog(CString key)
{
  loc_entry fileloc;
  int ret;
  int fhandle;
  CString tmp;
  
  if(dialogs.Lookup(key,fileloc))
  {
    fhandle=locate_file(fileloc, 0);
    if(fhandle<1) return -2;
    dialogs.SetAt(key,fileloc);
    itemname=key;
    ret=the_dialog.ReadDialogFromFile(fhandle, fileloc.size);
    close(fhandle);
  }
  else return -3;
  return ret;
}

int read_videocell(CString key)
{
  loc_entry fileloc;
  int ret;
  int fhandle;
  CString tmp;
  
  if(vvcs.Lookup(key,fileloc))
  {
    fhandle=locate_file(fileloc, 0);
    if(fhandle<1) return -2;
    vvcs.SetAt(key,fileloc);
    ret=the_videocell.ReadVVCFromFile(fhandle, fileloc.size);
    close(fhandle);
  }
  else return -1;
  return ret;
}

int write_videocell(CString key)
{
  loc_entry fileloc;
  int ret;
  CString filepath, tmpath;
  int size;
  int fhandle;

  filepath=bgfolder+"override\\"+key+".VVC";
  tmpath=bgfolder+"override\\"+key+".TMP";

  fhandle=open(tmpath, O_BINARY|O_RDWR|O_CREAT|O_TRUNC,S_IREAD|S_IWRITE);
  if(fhandle<1)
  {
    return -2;
  }
  ret=the_videocell.WriteVVCToFile(fhandle);
  size=filelength(fhandle);
  close(fhandle);
  if(!ret && size>0)
  {
    unlink(filepath);
    rename(tmpath,filepath);
    filepath="override\\"+key+".VVC";
    UpdateIEResource(key,REF_VVC,filepath,size);
  }
  return ret;
}

int read_2da(CString key)
{
  loc_entry fileloc;
  int ret;
  int fhandle;
  CString tmp;
  
  if(darefs.Lookup(key,fileloc))
  {
    fhandle=locate_file(fileloc, 0);
    if(fhandle<1) return -2;
    darefs.SetAt(key,fileloc);
    ret=the_2da.Read2DAFromFile(fhandle, fileloc.size);
    close(fhandle);
  }
  else return -1;
  return ret;
}

int read_ids(CString key)
{
  loc_entry fileloc;
  int ret;
  int fhandle;
  CString tmp;
  
  if(idrefs.Lookup(key,fileloc))
  {
    fhandle=locate_file(fileloc, 0);
    if(fhandle<1) return -2;
    idrefs.SetAt(key,fileloc);
    ret=the_ids.ReadIDSFromFile(fhandle, fileloc.size);
    close(fhandle);
  }
  else return -1;
  return ret;
}

int read_mus(CString key)
{
  loc_entry fileloc;
  int ret;
  int fhandle;
  CString tmp;
  
  if(musiclists.Lookup(key,fileloc))
  {
    fhandle=locate_file(fileloc, 0);
    if(fhandle<1) return -2;
    musiclists.SetAt(key,fileloc);
    ret=the_mus.ReadMusFromFile(fhandle, fileloc.size);
    close(fhandle);
  }
  else return -1;
  return ret;
}

int read_worldmap(CString key)
{
  loc_entry fileloc;
  int ret;
  int fhandle;
  CString tmp;
  
  if(wmaps.Lookup(key,fileloc))
  {
    fhandle=locate_file(fileloc, 0);
    if(fhandle<1) return -2;
    ret=the_map.ReadMapFromFile(fhandle, fileloc.size);
    close(fhandle);
  }
  else return -1;
  return ret;
}

int read_effect(CString key)
{
  loc_entry fileloc;
  int ret;
  int fhandle;
  CString tmp;
  
  if(effects.Lookup(key,fileloc))
  {
    fhandle=locate_file(fileloc, 0);
    if(fhandle<1) return -2;
    effects.SetAt(key,fileloc);
    ret=the_effect.ReadEffectFromFile(fhandle, fileloc.size);
    close(fhandle);
  }
  else return -1;
  return ret;
}

int write_effect(CString key)
{
  loc_entry fileloc;
  int ret;
  CString filepath, tmpath;
  int size;
  int fhandle;

  filepath=bgfolder+"override\\"+key+".EFF";
  tmpath=bgfolder+"override\\"+key+".TMP";

  fhandle=open(tmpath, O_BINARY|O_RDWR|O_CREAT|O_TRUNC,S_IREAD|S_IWRITE);
  if(fhandle<1)
  {
    return -2;
  }
  ret=the_effect.WriteEffectToFile(fhandle,0);
  size=filelength(fhandle);
  close(fhandle);
  if(!ret && size>0)
  {
    unlink(filepath);
    rename(tmpath,filepath);
    filepath="override\\"+key+".EFF";
    UpdateIEResource(key,REF_EFF,filepath,size);
  }
  return ret;
}

int read_game(CString key)
{
  loc_entry fileloc;
  int ret;
  int fhandle;
  CString tmp;
  
  if(games.Lookup(key,fileloc))
  {
    fhandle=locate_file(fileloc, 0);
    if(fhandle<1) return -2;
    games.SetAt(key,fileloc);
    itemname=key;
    ret=the_game.ReadGameFromFile(fhandle, fileloc.size);
    close(fhandle);
  }
  else return -1;
  return ret;
}

int write_game(CString key)
{
  loc_entry fileloc;
  int ret;
  CString filepath, tmpath;
  int size;
  int fhandle;

  filepath=bgfolder+"override\\"+key+".GAM";
  tmpath=bgfolder+"override\\"+key+".TMP";

  fhandle=open(tmpath, O_BINARY|O_RDWR|O_CREAT|O_TRUNC,S_IREAD|S_IWRITE);
  if(fhandle<1)
  {
    return -2;
  }
  ret=the_game.WriteGameToFile(fhandle,0);
  size=filelength(fhandle);
  close(fhandle);
  if(!ret && size>0)
  {
    unlink(filepath);
    rename(tmpath,filepath);
    filepath="override\\"+key+".GAM";
    UpdateIEResource(key,REF_GAM,filepath,size);
  }
  return ret;
}

int read_bmp(CString key, Cbam *cb, int lazy)
{
  loc_entry fileloc;
  int ret;
  int fhandle;
  CString tmp;
  
  if(!cb) cb=&the_bam;
  if(lazy && key==cb->m_name+".bmp") return 0;
  if(bitmaps.Lookup(key,fileloc))
  {
    fhandle=locate_file(fileloc, 0);
    if(fhandle<1)
    {
      ret=-2;
      goto endofquest;
    }
    bitmaps.SetAt(key,fileloc);
    ret=cb->ReadBmpFromFile(fhandle, fileloc.size);
    close(fhandle);
  }
  else ret=-2;
endofquest:
  if(ret) cb->m_name.Empty();
  else cb->m_name=key+".bmp";
  return ret;
}

int read_bam(CString key, Cbam *cb, int lazy)
{
  loc_entry fileloc;
  int ret;
  int fhandle;
  CString tmp;
  
  if(!cb) cb=&the_bam;
  if(lazy && key+".bam"==cb->m_name) return 0;
  if(icons.Lookup(key,fileloc))
  {
    fhandle=locate_file(fileloc, 0);
    if(fhandle<1)
    {
      ret=-2;
      goto endofquest;
    }
    icons.SetAt(key,fileloc);
    ret=cb->ReadBamFromFile(fhandle, fileloc.size,0);
    close(fhandle);
  }
  else ret=-2;
endofquest:
  if(ret) cb->m_name.Empty();
  else cb->m_name=key+".bam";
  return ret;
}

int read_bmp(CString key,HBITMAP &hb)
{
  loc_entry fileloc;
  int fhandle;
  CString tmp;
  char *memory;
  int size;
  
  if(bitmaps.Lookup(key,fileloc))
  {
    fhandle=locate_file(fileloc, 0);
    if(fhandle<1) return -2;
    bitmaps.SetAt(key,fileloc);
    if(fileloc.size<0) size=filelength(fhandle);
    else size=fileloc.size;
    memory=new char[size];
    if(!memory) return -1;
    if(read(fhandle,memory,size)!=size) return -2;
    CDC *pDC = AfxGetMainWnd()->GetDC();
    hb = ::CreateDIBitmap(pDC->GetSafeHdc(),(BITMAPINFOHEADER*)(memory+sizeof(BITMAPFILEHEADER)),
      CBM_INIT, memory+((BITMAPFILEHEADER*)memory)->bfOffBits,
      (BITMAPINFO*)(memory+sizeof(BITMAPFILEHEADER)), DIB_RGB_COLORS);
    AfxGetMainWnd()->ReleaseDC(pDC);
    
    delete [] memory;
    close(fhandle);
  }
  else return -2;
  return 0;
}

int read_bmp(CString key, Cmos *cb, int lazy)
{
  loc_entry fileloc;
  int ret;
  int fhandle;

  if(!cb) cb=&the_mos;
  if(lazy && (key+".bmp"==cb->m_name) ) return 0;
  if(bitmaps.Lookup(key,fileloc))
  {
    fhandle=locate_file(fileloc, 0);
    if(fhandle<1)
    {
      ret=-2;
      goto endofquest;
    }
    bitmaps.SetAt(key,fileloc);
    ret=cb->ReadBmpFromFile(fhandle,-1); //read bmp
    close(fhandle);
  }
  else ret=-2;
endofquest:
  if(ret) cb->m_name.Empty();
  else cb->m_name=key+".bmp";
  return ret;
}

int read_mos(CString key, Cmos *cb, int lazy)
{
  loc_entry fileloc;
  int ret;
  int fhandle;
  CString tmp;
  
  if(!cb) cb=&the_mos;
  if(lazy && (key+".mos"==cb->m_name) ) return 0;
  if(mos.Lookup(key,fileloc))
  {
    fhandle=locate_file(fileloc, 0);
    if(fhandle<1)
    {
      ret=-2;
      goto endofquest;
    }
    mos.SetAt(key,fileloc);
    ret=cb->ReadMosFromFile(fhandle, fileloc.size);
    close(fhandle);
  }
  else ret=-2;
endofquest:
  if(ret) cb->m_name.Empty();
  else cb->m_name=key+".mos";
  return ret;
}

int read_tis(CString key, Cmos *cb, int lazy)
{
  loc_entry fileloc;
  int header;
  int ret;
  int fhandle;
  CString tmp;

  if(!cb) cb=&the_mos;
  if(lazy && (key+".tis"==cb->m_name) ) return 0;
  if(tis.Lookup(key,fileloc))
  {
    fhandle=locate_file(fileloc, 0);
    if(fhandle<1)
    {
      ret=-2;
      goto endofquest;
    }
    tis.SetAt(key,fileloc);
    //if not in bif, then it has a header
    if(fileloc.bifname.Left(8)=="override") header=true;
    else header=false;
    ret=cb->ReadTisFromFile(fhandle, fileloc.size, header);
    close(fhandle);
  }
  else ret=-2;
endofquest:
  if(ret) cb->m_name.Empty();
  else cb->m_name=key+".tis";
  return ret;
}

int write_bam(CString key)
{
  loc_entry fileloc;
  int ret;
  CString filepath, tmpath;
  int size;
  int fhandle;

  filepath=bgfolder+"override\\"+key+".BAM";
  tmpath=bgfolder+"override\\"+key+".TMP";

  fhandle=open(tmpath, O_BINARY|O_RDWR|O_CREAT|O_TRUNC,S_IREAD|S_IWRITE);
  if(fhandle<1)
  {
    return -2;
  }
  ret=the_bam.WriteBamToFile(fhandle);
  size=filelength(fhandle);
  close(fhandle);
  if(!ret && size>0)
  {
    unlink(filepath);
    rename(tmpath,filepath);
    filepath="override\\"+key+".BAM";
    UpdateIEResource(key,REF_BAM,filepath,size);
  }
  return ret;
}

int play_acm(CString key, bool acm_or_wavc, int justload)
{
  panicbutton dlg;
  loc_entry fileloc;
  int fhandle;
  int ret;
  CString tmp;
  unsigned char *memory;
  long samples_written;
  
  if(acm_or_wavc) ret=musics.Lookup(key,fileloc);
  else ret=sounds.Lookup(key,fileloc);
  if(ret)
  {
    fhandle=locate_file(fileloc, 0);
    if(fhandle<1) return -2;
    ConvertAcmWav(fhandle,fileloc.size, memory, samples_written, acm_or_wavc?(editflg&FORCESTEREO):0);
    close(fhandle);
    if(memory)
    {
      if(acm_or_wavc) musics.SetAt(key,fileloc);
      else sounds.SetAt(key,fileloc);
      if(justload) return (int) memory; //very risky

      sndPlaySound((const char *) memory,SND_MEMORY | SND_ASYNC);
      dlg.waitsound=true;
      ret=dlg.DoModal();
      sndPlaySound(NULL,SND_NODEFAULT);
      delete memory;
      return ret;
    }    
    return -1;
  }
  return -2;
}

int write_wav(CString key, void *memory, long samples_written, int wav_or_acm)
{
  loc_entry fileloc;
  int ret;
  CString filepath, tmpath;
  int size;
  int fhandle;

  if(wav_or_acm)
  {
    filepath=bgfolder+"override\\"+key+".WAV";
    tmpath=bgfolder+"override\\"+key+".TMP";
  }
  else
  {
    filepath=bgfolder+"music\\"+key+".ACM";
    tmpath=bgfolder+"music\\"+key+".TMP";
  }

  fhandle=open(tmpath, O_BINARY|O_RDWR|O_CREAT|O_TRUNC,S_IREAD|S_IWRITE);
  if(fhandle<1)
  {
    return -2;
  }
  ret=(write(fhandle,memory,samples_written)!=samples_written);
  size=filelength(fhandle);
  close(fhandle);
  if(!ret && (size==samples_written) )
  {
    unlink(filepath);
    rename(tmpath,filepath);
    if(wav_or_acm)
    {
      filepath="override\\"+key+".WAV";
      UpdateIEResource(key,REF_WAV,filepath,size);
    }
    else
    {
      filepath="music\\"+key+".ACM";
      UpdateIEResource(key,REF_ACM,filepath,size);
    }
  }
  return ret;
}

int longsort(const void *a,const void *b)
{
  if(*(long *) a>*(long *) b) return -1;
  if(*(long *) a<*(long *) b) return 1;
  return 0;
}

int longsortb(const void *a,const void *b)
{
  if(*(long *) a>*(long *) b) return 1;
  if(*(long *) a<*(long *) b) return -1;
  return 0;
}

//this part was taken from DL1
unsigned short r_offset[256], g_offset[256], b_offset[256];
static char dith_max_tbl[512];
char *dith_max;
static BYTE range_tbl[3*256];
BYTE *range;
static unsigned long square_tbl[511];

//all DL1 specific initialisations are kept here so we spare on memory blocks
unsigned long *init_squares() 
{
  int i;

  for (i = 0; i < 256; i++)
  {
    range_tbl[i] = 0;
    range_tbl[i + 256] = (BYTE) i;
    range_tbl[i + 512] = 255;

    dith_max_tbl[i] = -DITHER_MAX;
    dith_max_tbl[i + 256] = DITHER_MAX;
  }
  for (i = -DITHER_MAX; i <= DITHER_MAX; i++)
  {
    dith_max_tbl[i + 256] = (char)i;
  }
  dith_max=dith_max_tbl+256;
  range = range_tbl + 256;

  for (i = 0; i < 256; i++)
  {
    r_offset[i] = (unsigned short) ((i & 128) << 7 | (i & 64) << 5 | (i & 32) << 3 |
				  (i & 16)  << 1 | (i & 8)  >> 1);
    g_offset[i] = (unsigned short) ((i & 128) << 6 | (i & 64) << 4 | (i & 32) << 2 |
				  (i & 16)  << 0 | (i & 8)  >> 2);
    b_offset[i] = (unsigned short) ((i & 128) << 5 | (i & 64) << 3 | (i & 32) << 1 |
				  (i & 16)  >> 1 | (i & 8)  >> 3);
  }
  for(i=-255;i<255;i++) square_tbl[i+255]=i*i;
  return square_tbl+255;
}
unsigned long *squares=init_squares();

// some speed optimisation (using pre-computed squares)
int ChiSquare(BYTE *a,BYTE *b)
{
  int ret;

  ret=squares[*(a)-*(b)];
  ret+=squares[*(a+1)-*(b+1)];
  ret+=squares[*(a+2)-*(b+2)];
  return ret;
}

int determinemenu(CString commandline)
{
  CString tmp;
  int i;

  tmp=commandline.Right(4);
  tmp.MakeLower();
  for(i=0;i<=NUM_OBJTYPE;i++)
  {
    if(tmp==objexts[i])
    {
      return i;
    }
  }
  return 0;
}

int determinetype(int reftype)
{
  int i;

  for(i=0;i<=NUM_OBJTYPE;i++)
  {
    if(reftype==objrefs[i])
    {
      return i;
    }
  }
  return 0;
}

// code derived from ShadowKeeper (c) 2000 Aaron O'Neil
bool MakeBitmapExternal(const COLORREF *pDIBits, int nWidth, int nHeight, HBITMAP &hBitmap)
{
  if(!pDIBits) return false;

	CDC *pDC = AfxGetMainWnd()->GetDC();
	if (!pDC)
		return false;

	BITMAPINFOHEADER bih;
	memset(&bih,0,sizeof(BITMAPINFOHEADER));
	
	bih.biSize = sizeof(BITMAPINFOHEADER);
	bih.biWidth = nWidth;
	bih.biHeight = nHeight * -1;// The graphics are stored as bottom up DIBs.
	bih.biPlanes = 1;
	bih.biBitCount = 32;
	bih.biCompression = BI_RGB;

	hBitmap = ::CreateDIBitmap(pDC->GetSafeHdc(), &bih,CBM_INIT,pDIBits,
		(BITMAPINFO*)&bih,DIB_RGB_COLORS);

	AfxGetMainWnd()->ReleaseDC(pDC);

	return(hBitmap != NULL);
}

bool MakeBitmapExternal(const LPBYTE pPixels, const COLORREF *pPalette, int nWidth, int nHeight, HBITMAP &hBitmap)
{
  int i;
	CDC *pDC = AfxGetMainWnd()->GetDC();
	if (!pDC)
		return false;

  COLORREF *pDIBits=new COLORREF[nWidth*nHeight];
  if(!pDIBits)
  {
    AfxGetMainWnd()->ReleaseDC(pDC);
    return false;
  }
  i=nWidth*nHeight;
  while(i--)
  {
    pDIBits[i]=pPalette[pPixels[i]];
  }

	BITMAPINFOHEADER bih;
	memset(&bih,0,sizeof(BITMAPINFOHEADER));
	
	bih.biSize = sizeof(BITMAPINFOHEADER);
	bih.biWidth = nWidth;
	bih.biHeight = nHeight * -1;// The graphics are stored as bottom up DIBs.
	bih.biPlanes = 1;
	bih.biBitCount = 32;
	bih.biCompression = BI_RGB;

	hBitmap = ::CreateDIBitmap(pDC->GetSafeHdc(), &bih,CBM_INIT,pDIBits,
		(BITMAPINFO*)&bih,DIB_RGB_COLORS);

  delete [] pDIBits;
	AfxGetMainWnd()->ReleaseDC(pDC);

	return(hBitmap != NULL);
}

void SwapPalette(COLORREF *pal, int idx1, int idx2)
{
  COLORREF tmp;

  if(idx1>idx2)
  {
    tmp=pal[idx1];
    memmove(pal+idx2+1, pal+idx2, (idx1-idx2)*sizeof(COLORREF) );
    pal[idx2]=tmp;
  }
  else
  {
    tmp=pal[idx2];
    memmove(pal+idx1+1, pal+idx1, (idx2-idx1)*sizeof(COLORREF) );
    pal[idx1]=tmp;
  }
}

int fill_destination(CString key, CComboBox *cb)
{
  loc_entry fileloc;
  int fhandle;
  Carea tmparea;
  int ret;
  
  if(areas.Lookup(key,fileloc))
  {
    fhandle=locate_file(fileloc, 0);
    if(fhandle<1) return -2;
    ret=tmparea.FillDestination(fhandle,fileloc.size,cb);
    close(fhandle);
    return ret;
  }
  return -1;
}

char *weiduflags[]={"--noheader ","--nofrom ","--nocom ","--text "};

CString AssembleWeiduCommandLine(CString filename, CString outpath)
{
  CString syscommand;
  CString flags;
  int i,j;

  flags=weiduextra+" ";
  j=1;
  for(i=0;i<4;i++)
  {
    if(weiduflg&j) flags+=weiduflags[i];
    j<<=1;
  }
  syscommand.Format("\"%s\" %s--out \"%s\" --tlkout dialog.tlk \"%s\"", weidupath, flags, outpath, filename);
  if(weiduflg&WEI_LOGGING) syscommand+=" --log "+WEIDU_LOG;
  return syscommand;
} 

colortype colors[COLORNUM]={
  colortype(0x50443C,"Red Tinted Black"),
  colortype(0x957140,"Dark Bronze"),
  colortype(0x794C18,"Dark Gold"),
  colortype(0xBAAA71,"Light Gold"),
  colortype(0xA55020,"Auburn"),
  colortype(0x999D8D,"Light Silver"),
  colortype(0x999D8D,"Dark Silver"),
  colortype(0x716D61,"Light Metallic Green"),
  colortype(0x8D6140,"Dark Muddish Brown"),
  colortype(0xA17955,"Light Muddish Brown"),
  colortype(0xB2956D,"Light Pure Gold"),
  colortype(0xC28171,"Light Rose Red"),
  colortype(0xD2A18D,"Light Carnation Pink"),
  colortype(0xC6AA99,"Light Pinkish Silver"),
  colortype(0xAEA1A1,"Dark Pure Silver"),
  colortype(0x85A18D,"Pastel Green"),
  colortype(0xB2A569,"Silverish Gold"),
  colortype(0x799DAE,"Light Blue"),
  colortype(0x3C507D,"Dark Blue"),
  colortype(0xB24828,"Dark Rose Red"),
  colortype(0x2C5924,"Dark Moldy Green"),
  colortype(0x595959,"Dark Iron Gray"),
  colortype(0x996D50,"Dark Brown"),
  colortype(0x795D4C,"Light Copper"),
  colortype(0x9D6524,"Dark Gold"),
  colortype(0xCAA15D,"Dark Pure Gold"),
  colortype(0xD2894C,"Wood"),
  colortype(0x858589,"Gray"),
  colortype(0x7D8185,"Bluish Silver"),
  colortype(0x4C3C34,"Red Tinted Black"),
  colortype(0x656565,"Light Iron Gray"),
  colortype(0x48858D,"Light Sea Blue"),
  colortype(0x108544,"Dark Sea Green"),
  colortype(0xC23461,"Magenta"),
  colortype(0x71954C,"Light:Green/Dark:Blue"),
  colortype(0xDA8150,"Light:Yellow/Dark:Purple"),
  colortype(0x757520,"Light Dirty Yellow"),
  colortype(0x595518,"Dark Dirty Yellow"),
  colortype(0x8D7161,"Light Dirt Brown"),
  colortype(0x715040,"Dark Dirt Brown"),
  colortype(0x815D28,"Dark Dirty Copper"),
  colortype(0x754424,"Dark Brown"),
  colortype(0x7D7561,"Beige"),
  colortype(0x655548,"Dark Beige"),
  colortype(0x954C6D,"Purple"),
  colortype(0x7D3055,"Dark Purple"),
  colortype(0xAE2C1C,"Red"),
  colortype(0x750000,"Pure Dark Red"),
  colortype(0xBA7934,"Light Bronze"),
  colortype(0x955018,"Dark Bronze"),
  colortype(0x9D8D00,"Dark Yellow"),
  colortype(0x756900,"Dark Olive Green"),
  colortype(0x5D8538,"Light Green"),
  colortype(0x406D28,"Green"),
  colortype(0x1C5910,"Dark Green"),
  colortype(0x148D6D,"Turquoise"),
  colortype(0x0C6148,"Dark Turquoise"),
  colortype(0x407595,"Light Blue"),
  colortype(0x1C487D,"Dark Blue"),
  colortype(0x797189,"Light Purple"),
  colortype(0x504C6D,"Dark Purple"),
  colortype(0x75616D,"Lavender"),
  colortype(0x615555,"Dark Lavender"),
  colortype(0x8D8D8D,"Light Gray"),
  colortype(0x656565,"Dark Gray"),
  colortype(0x4C504C,"Really Dark Gray"),
  colortype(0x343438,"Faded Black"),
  colortype(0xF6C650,"Shiny Gold"),
  colortype(0x69D6FF,"Shiny Blue"),
  colortype(0xA5E26D,"Shiny Green"),
  colortype(0xC6615D,"Light Metallic Red"),
  colortype(0xC2E2FF,"Frost Blue"),
  colortype(0x959195,"Steel Gray"),
  colortype(0x14A14C,"Christmas Green"),
  colortype(0xFFFFFF,"Solid White (no gradient)"),
  colortype(0x000000,"Solid Black (no gradient)"),
  colortype(0xFF0000,"Solid Red (no gradient)"),
  colortype(0x00FF00,"Solid Green (no gradient)"),
  colortype(0x0000FF,"Solid Blue (no gradient)"),
  colortype(0xBAB6BA,"Light Silver"),
  colortype(0xDA9100,"Saturated Bronze"),
  colortype(0xC69D30,"Faded Gold"),
  colortype(0x34A1CE,"Dark Metallic Blue"),
  colortype(0x55555D,"Dark Bluish Gray"),
  colortype(0xC68559,"Peach"),
  colortype(0xB6814C,"21 Carat Gold"),
  colortype(0x75950C,"Chrome Green"),
  colortype(0x714C24,"Dark Poopy Brown"),
  colortype(0xBE5975,"Chrome Pink"),
  colortype(0xDE8185,"Metallic Pink"),
  colortype(0x917D10,"Moldy Gold"),
  colortype(0x755028,"Dark Chocolate"),
  colortype(0xA17544,"Light Chocolate"),
  colortype(0x69594C,"Dark Cement Gray"),
  colortype(0x8D5944,"Rhubarb"),
  colortype(0xAA8155,"Light Dirtish Brown"),
  colortype(0x485D91,"Chrome Blue"),
  colortype(0x7599AE,"Light Minty Blue"),
  colortype(0x998D69,"Leaf Green"),
  colortype(0x956524,"Dark Gold"),
  colortype(0x615548,"Dark Iron Gray"),
  colortype(0xB25040,"Light Metallic Red"),
  colortype(0x79898D,"Sea Blue"),
  colortype(0x406150,"Pine Green"),
  colortype(0x794C81,"Dark Chrome Purple"),
  colortype(0xBEC6CE,"Off White"),
  colortype(0xAA8D8D,"Pale Pink"),
  colortype(0x756961,"Khaki"),
  colortype(0xCA9999,"Pinkish Gray"),
  colortype(0xC67965,"Terra Cotta"),
  colortype(0x9D9DAE,"Ash"),
  colortype(0xD28969,"Burnt Sienna"),
  colortype(0x717D65,"Sea Foam"),
  colortype(0xAE9D95,"Fog"),
  colortype(0x855530,"Yet Another Brown"),
  colortype(0xFFEE8D,"Sunkissed"),
  colortype(0xDADADA,"Solid Gray (no gradient)"),
  colortype(0xDADADA,"Solid Gray (no gradient)"),
  colortype(0xDADADA,"Solid Gray (no gradient)"),
  colortype(0xDADADA,"Solid Gray (no gradient)"),
};

void MakeGradientBitmap(HBITMAP &hb, int GradientIndex)
{
  COLORREF bits[16*16];
  int j;

  for(j=0;j<16*16;j++)
  {
    bits[j]=colors[GradientIndex].rgb[(j&15)*12/16];
  }
  MakeBitmapExternal(bits,16,16,hb); 
}

CString searchmap[16]={
  "0 - Solid obstacle", "1 - Sand","2 - Wood 2", "3 - Wood 3", "4 - Stone 1",
  "5 - Soft surface","6 - Dirt","7 - Stone 2","8 - Light obstacle","9 - Wood 1",
  "10 - Side wall",  "11- Snow","12- Water","13- Roof","14- Exit", "15- Grass"
};

CString GetMapTypeValue(int maptype, int value)
{
  CString tmpstr;

  switch(maptype)
  {
  case MT_SEARCH:
    return searchmap[value];
  case MT_BLOCK:
    if(value) return "1 - Blocked";
    else return "0 - Unblocked";
  }
  tmpstr.Format("%d",value);
  return tmpstr;
}

int ReadPaletteFromFile(int fhandle, int ml)
{
  bmp_header sHeader;          //BMP header
  unsigned char *pcBuffer;     //buffer for a scanline
  int scanline;                //size of a scanline in bytes (ncols * bytesize of a pixel)
  int palettesize;             //size of the palette in file (ncolor * bytesize of color)
  long original;
  int x,y;
  unsigned int nSourceOff;     //the source offset in the original scanline (which we cut up)
  int ret;

  pcBuffer=NULL;
  if(ml<0) ml=filelength(fhandle);
  if(ml<0) return -2;
  original=tell(fhandle);
  if(read(fhandle,&sHeader,sizeof(bmp_header))!=sizeof(bmp_header) )
  {
    return -2;
  }
  if((*(unsigned short *) sHeader.signature)!='MB') return -2; //BM
  if(sHeader.bits!=24) return -1;                   //read palette from
  if(sHeader.fullsize>(unsigned int) ml) return -2;
  if(sHeader.headersize!=0x28) return -2;
  if(sHeader.planes!=1) return -1;                  //unsupported
  if(sHeader.compression!=BI_RGB) return -1;        //unsupported
  if(sHeader.height<0 || sHeader.width<0) return -1; //unsupported
  if(sHeader.height>256 || sHeader.width>256) return -1; //unsupported  
  palettesize=sHeader.height;
  if(palettesize>256) return -1;
  //check if the file fits in the claimed length
  if(sHeader.offset!=sHeader.headersize+((unsigned char *) &sHeader.headersize-(unsigned char *) &sHeader) ) return -2;
  scanline=sHeader.width*(sHeader.bits>>3);
  if(scanline&3) scanline+=4-(scanline&3);
  if(scanline*sHeader.height+sHeader.offset>sHeader.fullsize) return -2;
  //check if we are on the position we wanted to be
  nSourceOff=tell(fhandle);
  if(sHeader.offset!=nSourceOff-original ) return -2;

  pcBuffer=new BYTE[scanline];
  if(!pcBuffer)
  {
    ret=-3;
    goto endofquest;
  }
  ret=0; //unsupported mode

  for(y=0;y<COLORNUM;y++) memset(colors[y].rgb,0,sizeof(colors[y].rgb) );
  y=sHeader.height;
  while(y--)
  {
    if(read(fhandle, pcBuffer, scanline)!=scanline)
    {
      ret=-2;
      goto endofquest;
    }
    for(x=0;x<12;x++)
    {
      nSourceOff=sHeader.width*x/12*3;
      colors[y].rgb[x]=RGB(pcBuffer[nSourceOff],pcBuffer[nSourceOff+1],pcBuffer[nSourceOff+2]);
    }
  }
endofquest:
  close(fhandle);
  if(pcBuffer) delete [] pcBuffer;

  return ret;
}

//using colors
int read_bmp_palette(CString key)
{
  loc_entry fileloc;
  int ret;
  int fhandle;
  
  if(bitmaps.Lookup(key,fileloc))
  {
    fhandle=locate_file(fileloc, 0);
    if(fhandle<1) return -2;
    bitmaps.SetAt(key,fileloc);
    ret=ReadPaletteFromFile(fhandle, fileloc.size);
    close(fhandle);
  }
  else return -2;
  return ret;
}

void init_colors()
{
  if(read_bmp_palette("MPALETTE"))
  {
    read_bmp_palette("MPAL256");
  }
}

CString colortitle(unsigned int value)
{
  CString tmpstr, tmp;
  
  tmp=IDSToken("CLOWNCLR", value);
  if(tmp.IsEmpty())
  {
    if(value>=COLORNUM) tmpstr.Format("%d-Unknown",value);
    else tmpstr.Format("%d-%s",value,colors[value].colorname);
  }
  else tmpstr.Format("%d-%s",value,tmp);
  return tmpstr;
}

CString convert_musiclist(CString tmpstr, bool onlyinternal)
{
  CString prefix;

  if(tmpstr.Left(the_mus.internal.GetLength()+1)==the_mus.internal+" ")
  {
    tmpstr=tmpstr.Mid(the_mus.internal.GetLength()+1);
    tmpstr.TrimLeft();
  }
  if(tmpstr=="MX0000A" && iwd2_structures())
  {
    prefix="MX0000\\";
  }
  else if(tmpstr=="MX9000A" && has_xpvar())
  {
    prefix="MX9000\\";
  }
  else if(tmpstr.Left(3)=="SPC") return tmpstr; //pst, bg hack
  else
  {
    prefix=the_mus.internal+"\\";
    tmpstr=the_mus.internal+tmpstr;
  }
  if(onlyinternal) return tmpstr;
  return prefix+tmpstr;
}

char BASED_CODE cfbFilter[] = "CFB Files|*.bin|All files (*.*)|*.*||";

int ModifyCFB(int mode, int featnum, feat_block *cfb)
{
  feat_block *new_featblock;
  int featblkcount;
  int i;
  CString key;

  if(!(mode&1))
  {
    if(mode&2) featnum=-the_item.header.featblkcount;
    else featnum=-the_spell.header.featblkcount;
    if(!featnum)
    {
      return 1;
    }
  }
  //featnum gets initialized once if mode&1==true 
  //or in the while cycle if mode&1==false 
  //(the warning of uninitialized local is unwarranted)
  if(mode&2)
  {
    featblkcount=the_item.featblkcount+featnum;
    the_item.header.featblkcount=(short) (the_item.header.featblkcount+featnum);
    for(i=0;i<the_item.extheadcount;i++)
    {
      the_item.extheaders[i].fboffs=(unsigned short) (the_item.extheaders[i].fboffs+featnum);
    }
  }
  else
  {
    featblkcount=the_spell.featblkcount+featnum;
    the_spell.header.featblkcount=(short) (the_spell.header.featblkcount+featnum);
    for(i=0;i<the_spell.extheadcount;i++)
    {
      the_spell.extheaders[i].fboffs=(unsigned short) (the_spell.extheaders[i].fboffs+featnum);
    }
  }
  
  new_featblock=new feat_block[featblkcount];
  if(mode&1)
  {
    //replacing * with the filename
    for(i=0;i<featnum;i++)
    {
      new_featblock[i]=cfb[i];
      if(!memcmp(new_featblock[i].vvc,SELF_REFERENCE,8) )
      {
        StoreResref(key,new_featblock[i].vvc);
      }
      if(mode&2)
      {
        //do nothing, yet
      }
      else
      {
        if(!memcmp(&new_featblock[i].par1.parb,"++++",4) )
        {
          new_featblock[i].par1.parl=the_spell.header.level;
        }
        else if(!memcmp(&new_featblock[i].par1.parb,"----",4) )
        {
          new_featblock[i].par1.parl=-the_spell.header.level;
        }
      }
    }
  }
  else
  {
    i=0;
  }
  if(mode&2)
  {
    for(;i<featblkcount;i++)
    {
      new_featblock[i]=the_item.featblocks[i-featnum];
    }
    delete [] the_item.featblocks;
    the_item.featblocks=new_featblock;
    the_item.featblkcount=featblkcount;
  }
  else
  {
    for(;i<featblkcount;i++)
    {
      new_featblock[i]=the_spell.featblocks[i-featnum];
    }
    delete [] the_spell.featblocks;
    the_spell.featblocks=new_featblock;
    the_spell.featblkcount=featblkcount;
  }
  return 0;
}

void ConvertToV10Eff(const creature_effect *v20effect, feat_block *v10effect)
{
  memset(v10effect,0,sizeof(feat_block));
  v10effect->feature=(WORD) v20effect->feature;
  v10effect->target=(BYTE) v20effect->target;
  v10effect->power=(BYTE) v20effect->power;
  v10effect->par1=v20effect->par1;
  v10effect->par2=v20effect->par2;
  v10effect->timing=(BYTE) v20effect->timing;
  v10effect->resist=(BYTE) v20effect->resist;
//
  v10effect->duration=v20effect->duration;
  v10effect->prob1=(BYTE) v20effect->prob1;
  v10effect->prob2=(BYTE) v20effect->prob2;
  memcpy(v10effect->vvc,v20effect->resource,8);
  v10effect->count=v20effect->count;
  v10effect->sides=v20effect->sides;
  v10effect->stype=v20effect->stype;
  v10effect->sbonus=v20effect->sbonus;
  v10effect->unknown2c=v20effect->unknown2c;
}

void ConvertToV20Eff(creature_effect *v20effect,const feat_block *v10effect)
{
  memset(v20effect,0,sizeof(creature_effect) );
  v20effect->feature=v10effect->feature;
  v20effect->target=v10effect->target;
  v20effect->power=v10effect->power;
  v20effect->par1=v10effect->par1;
  v20effect->par2=v10effect->par2;
  v20effect->timing=v10effect->timing;
  v20effect->resist=v10effect->resist;
//
  v20effect->duration=v10effect->duration;
  v20effect->prob1=v10effect->prob1;
  v20effect->prob2=v10effect->prob2;
  memcpy(v20effect->resource,v10effect->vvc,8);
  v20effect->count=v10effect->count;
  v20effect->sides=v10effect->sides;
  v20effect->stype=v10effect->stype;
  v20effect->sbonus=v10effect->sbonus;
  v20effect->unknown2c=v10effect->unknown2c;
}

int CheckDestination(CString area, CString entrance)
{
  loc_entry dummyloc;
  int fhandle;
  int ret;

  if(!areas.Lookup(area,dummyloc) ) return -1;
  fhandle=locate_file(dummyloc, 0);
  if(fhandle<1) return -2;
  ret=the_area.CheckDestination(fhandle,dummyloc.size,entrance);
  close(fhandle);
  return ret;
}

static int CALLBACK BFFCallback(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
  TCHAR szDir[MAX_PATH] = {NULL};

  switch(uMsg)
  {
  case BFFM_INITIALIZED:
    SendMessage(hwnd,BFFM_SETSELECTION,TRUE,(LPARAM) (const char *) ((folderbrowse_t *) lpData)->initial);
    SetWindowText(hwnd,((folderbrowse_t *) lpData)->title);
    break;
  case BFFM_SELCHANGED:
    if ( ! SHGetPathFromIDList((LPITEMIDLIST) lParam ,szDir))
    {
      EnableWindow(GetDlgItem(hwnd, IDOK), FALSE); // disable the OK button
    }
    // Set the status window to the currently selected path (or NULL).
    SendMessage(hwnd,BFFM_SETSTATUSTEXT,0,(LPARAM)szDir);
    
    break;
  }
  return BFFM_ENABLEOK;
}

int BrowseForFolder(folderbrowse_t *pfb, HWND hwnd) 
{
  TCHAR path[MAX_PATH];
  LPITEMIDLIST pidl;
  BROWSEINFO bi={0};

  bi.hwndOwner=hwnd;
  bi.pszDisplayName=NULL;
  bi.ulFlags=BIF_RETURNONLYFSDIRS|BIF_STATUSTEXT|BIF_DONTGOBELOWDOMAIN;
  bi.lpfn=&BFFCallback;
//  strncpy(path,m_output,MAX_PATH);
  bi.lParam=(LPARAM) pfb;
	pidl=SHBrowseForFolder(&bi);
  if(pidl)
  {
    if ( SHGetPathFromIDList ( pidl, path) )
    {
      pfb->initial=path;
    }
    // free memory used
    IMalloc * imalloc = 0;
    if ( SUCCEEDED( SHGetMalloc ( &imalloc )) )
    {
      imalloc->Free ( pidl );
      imalloc->Release ( );
    }
    return IDOK;
  }
  return IDCANCEL;
}

//this is a special fuckup (err hack), to get this file selection box
//working on multi selection correctly
void HackForLargeList(CFileDialog &m_getfiledlg)
{
  strncpy(external,m_getfiledlg.m_ofn.lpstrFile,MAXBUFFSIZE);
  m_getfiledlg.m_ofn.lpstrFile=external;
  m_getfiledlg.m_ofn.nMaxFile=MAXBUFFSIZE;
}

int WriteTempCreature(char *creature, long esize)
{
  int fhandle;
  int ret;

  fhandle=open("dltcep.tmp",O_RDWR|O_TRUNC|O_CREAT|O_TEMPORARY,S_IREAD|S_IWRITE);
  if(fhandle<1) return -2;
  if(write(fhandle,creature,esize)!=esize)
  {
    close(fhandle);
    return -2;
  }
  lseek(fhandle,0,SEEK_SET);
  ret=the_creature.ReadCreatureFromFile(fhandle,esize);
  close(fhandle);
  unlink("dltcep.tmp");
  return ret;
}

CString imagefilters[8]={
  "All files (*.*)|*.*||",
  "Image files (*.mos)|*.mos|",
  "Tilesets (*.tis)|*.tis|",
  "Bitmaps (*.bmp)|*.bmp|",
/*  "Compressed tilesets (*.tiz)|*.tiz|",*/ "",
  "Bam files (*.bam)|*.bam|",
  "Paperdoll files (*.plt)|*.plt|",
  "",
};

CString ImageFilter(int mostisbmp)
{
  CString ret;
  int i;

  for(i=0;i<7;i++)
  {
    ret+=imagefilters[mostisbmp&15];
    if(!mostisbmp) break;
    mostisbmp>>=4;
  }
  return ret;
}

void CreateMinimap(HWND hwnd) 
{
  COLORREF *truecolor=NULL;
  Cmos minimap;
  CString tmpstr;
  CString tmpath, filepath;
  int fhandle;
  int x,y;
  int ret;
  unsigned int i;
  int esize;
  int factor;

  tmpath=bgfolder+"override\\"+itemname+".TMP";
  fhandle=open(tmpath, O_BINARY|O_RDWR|O_CREAT|O_TRUNC,S_IREAD|S_IWRITE);
  if(fhandle<1)
  {
    MessageBox(hwnd,"Can't write file!","Error",MB_ICONSTOP|MB_OK);
    return;
  }
  minimap.new_mos();
	if(has_xpvar())  //iwd and iwd2  ( /8 ) 8 pixels in one
  {
    factor=8;
    tmpstr="Creating IWD minimap...";
  }
	else             //pst, bg (/32*3)      32 pixels in three
  {
    factor=6;
    tmpstr="Creating BG minimap...";
  }
  //bg2 and iwd2 has compressed mos
  if(no_compress())
  {
    minimap.m_bCompressed=false;
  }
  else
  {
    minimap.m_bCompressed=true;
  }
  x=the_mos.mosheader.wColumn*factor;
  y=the_mos.mosheader.wRow*factor;
  if(minimap.Allocate(x,y))
  {
    MessageBox(hwnd,"Out of memory","Error",MB_ICONSTOP|MB_OK);
    goto endofquest;
  }
  esize=minimap.mosheader.dwBlockSize*minimap.mosheader.dwBlockSize;
  truecolor=new COLORREF[esize];
  if(!truecolor)
  {
    MessageBox(hwnd,"Out of memory","Error",MB_ICONSTOP|MB_OK);
    goto endofquest;
  }
  ((CChitemDlg *) AfxGetMainWnd())->start_progress(minimap.tisheader.numtiles,tmpstr);
  for(i=0;i<minimap.tisheader.numtiles;i++)
  {
    ((CChitemDlg *) AfxGetMainWnd())->set_progress(i);
    memset(truecolor,0,sizeof(COLORREF)*esize);
    //we need: frame index, output buffer, resize factor
    //input is always in 'the_mos'
    minimap.TakeOneFrame(i,truecolor,factor);
  }
  ((CChitemDlg *) AfxGetMainWnd())->end_progress();
  ret=minimap.WriteMosToFile(fhandle);
  if(ret)
  {
    MessageBox(hwnd,"Can't write file!","Error",MB_ICONSTOP|MB_OK);
  }
endofquest:
  if(truecolor) delete [] truecolor;
  filepath="override\\"+itemname+".MOS";
  UpdateIEResource(itemname,REF_MOS,filepath,filelength(fhandle));
  close(fhandle);
  filepath=bgfolder+filepath;
  unlink(filepath);
  rename(tmpath,filepath);
}

//beware bitspercolor is really in bits, not bytes!!!
int GetScanLineLength(int width, int bitspercolor)
{
  int paddedwidth;

  paddedwidth=(width*bitspercolor+7)/8;
  if(paddedwidth&3) paddedwidth+=4-(paddedwidth&3); // rounding it up to 4 bytes boundary
  return paddedwidth;
}

int CreateBmpHeader(int fhandle, DWORD width, DWORD height, DWORD bytes)
{
  int fullsize;
  int psize;

  if(bytes==1) psize=256;
  else psize=0;
  fullsize=GetScanLineLength(width,8*bytes)*height;
  bmp_header header={'B','M',fullsize+sizeof(header), 0,
  sizeof(header)+psize*4, 40, width, height, (short) 1, (short) (8*bytes), 0, fullsize,
  0xb12, 0xb12,psize,0};

  return write(fhandle,&header,sizeof(header))==sizeof(header);
}

unsigned long getfreememory()
{
  MEMORYSTATUS memstat;

  GlobalMemoryStatus(&memstat);
  return memstat.dwAvailPhys/1024;
}

struct 
{
  char signature[4];
  char version[4];
  unsigned long len;
} bifc_header = {'B','I','F','C','V','1','.','0'};

struct
{
  char signature[4];
  char version[4];
  unsigned long namelen;
} cbf_header = {'B','I','F',' ','V','1','.','0'};

int CompressBIFC(CString infilename)
{
  DWORD cheader[2]; 
  BYTE *compressed, *decompressed;
  CString outfilename;
  int fhandle, infile;
  unsigned long len, chunk;
  int ret;
  
  cheader[UNCOMPRESSED]=BLOCKSIZE;
  infile=open(infilename,O_RDONLY|O_BINARY);
  if(infile<1)
  {
    return -1;
  }
  len=filelength(infile);
  if(len<1)
  {
    close(infile);
    return -1;
  }
  bifc_header.len=len;
  outfilename=infilename;
  fhandle=open(outfilename+".tmp",O_BINARY|O_RDWR,S_IREAD|S_IWRITE);
  if(fhandle<1)
  {
    close(infile);
    return -2;
  }
  compressed=new BYTE[BLOCKSIZE+100];
  decompressed=new BYTE [cheader[UNCOMPRESSED]];
  if(!compressed || !decompressed)
  {
    ret=-3;
    goto endofquest;
  }
  if(write(fhandle, &bifc_header,sizeof(bifc_header))!=sizeof(bifc_header) )
  {
    ret=-2;
    goto endofquest;
  }
  do
  {
    chunk=len>BLOCKSIZE?BLOCKSIZE:len;
    len-=chunk;
    cheader[COMPRESSED]=BLOCKSIZE+100;
    ret=compress(compressed,cheader+COMPRESSED,decompressed,chunk);
    if(ret!=Z_OK)
    {
      ret=-2;
      goto endofquest;
    }
    if(write(fhandle,cheader,sizeof(cheader))!=sizeof(cheader))
    {
      ret=-2;
      goto endofquest;
    }
    if(write(fhandle,compressed,cheader[COMPRESSED])!=(int) cheader[COMPRESSED])
    {
      ret=-2;
      goto endofquest;
    }
  }
  while(len);
endofquest:
  if(compressed) delete [] compressed;
  if(decompressed) delete [] decompressed;
  close(infile);
  close(fhandle);
  if(ret)
  {
    unlink(outfilename+".tmp");
  }
  else
  {
    unlink(outfilename);
    rename(outfilename+".tmp",outfilename);
  }
  return ret;
}

int CompressCBF(CString infilename)
{
  DWORD cheader[2]; 
  BYTE *compressed, *decompressed;
  CString outfilename;
  unsigned long len;
  int fhandle, infile;
  int ret;
  
  infile=open(infilename,O_RDONLY|O_BINARY);
  if(infile<1)
  {
    return -1;
  }
  len=filelength(infile);
  if(len<1)
  {
    close(infile);
    return -1;
  }
  outfilename=infilename;
  cbf_header.namelen=infilename.GetLength();
  outfilename.MakeLower();
  outfilename.Replace(".bif",".cbf");
  fhandle=open(outfilename+".tmp",O_BINARY|O_RDWR,S_IREAD|S_IWRITE);
  if(fhandle<1)
  {
    close(infile);
    return -2;
  }
  cheader[COMPRESSED]=len+100;
  compressed=new BYTE [len+100];
  cheader[UNCOMPRESSED]=len;
  decompressed=new BYTE[len];
  if(!compressed || !decompressed)
  {
    ret=-3;
    goto endofquest;
  }
  if(write(fhandle,&cbf_header,sizeof(cbf_header) ) !=sizeof(cbf_header) )
  {
    ret=-2;
    goto endofquest;
  }
  ret = compress(compressed, cheader+COMPRESSED, decompressed, len);
  if(ret!=Z_OK)
  {
    ret=-1;
    goto endofquest;
  }
  if(write(fhandle,cheader,sizeof(cheader) )!=sizeof(cheader) )
  {
    ret=-2;
    goto endofquest;
  }
  if(write(fhandle,compressed,cheader[COMPRESSED])!=(int) cheader[COMPRESSED])
  {
    ret=-2;
    goto endofquest;
  }
  ret=0;

endofquest:
  if(compressed) delete [] compressed;
  if(decompressed) delete [] decompressed;
  close(infile);
  close(fhandle);
  if(ret)
  {
    unlink(outfilename+".tmp");
  }
  else
  {
    unlink(outfilename);
    rename(outfilename+".tmp",outfilename);
  }
  return ret;
}

int SetupSelectPoint()
{
  CString tmpstr;

  if(the_area.overlayheaders)
  {
    RetrieveResref(tmpstr, the_area.overlayheaders[0].tis);
  }
  else return -1;
  if(read_tis(tmpstr,&the_mos,true))
  {
    MessageBox(0,"Cannot load TIS.","Warning",MB_ICONEXCLAMATION|MB_OK);
    return -1;
  }
  the_mos.SetOverlay(0, the_area.overlaytileheaders, the_area.overlaytileindices);
  the_mos.TisToMos(the_area.overlayheaders[0].width,the_area.overlayheaders[0].height);
  
  return 0;
}

//////////vertex & polygon functions

int PolygonInBox(area_vertex *wedvertex, int count, CRect rect)
{
  int i;

  for(i=0;i<count;i++)
  {
    if(wedvertex[i].point.x<rect.left || wedvertex[i].point.x>rect.right)
    {
      continue;
    }
    if(wedvertex[i].point.y<rect.top || wedvertex[i].point.y>rect.bottom)
    {
      continue;
    }
    return true;
  }
  if(PointInPolygon(wedvertex, count,rect.BottomRight() ) ) return true;
  if(PointInPolygon(wedvertex, count,rect.TopLeft() ) ) return true;
  if(PointInPolygon(wedvertex, count,CPoint(rect.right, rect.top) ) ) return true;
  if(PointInPolygon(wedvertex, count,CPoint(rect.left, rect.bottom) ) ) return true;
  return false;
}

int PointInPolygon(area_vertex *wedvertex, int count, CPoint point)
{
  register int   j, yflag0, yflag1, xflag0 , index;
  register long ty, tx;
  bool inside_flag = false;
  area_vertex * vtx0, * vtx1;
  
  index = 0;
  
  tx = point.x;
  ty = point.y;
  
  if(count<3)
  {
    return false;
  }

  vtx0 = &wedvertex[count - 1];
  yflag0 = ( vtx0->point.y >= ty );
  vtx1 = &wedvertex[index];
  
  for (j = count ; j-- ;)
  {
    yflag1 = ( vtx1->point.y >= ty );
    if (yflag0 != yflag1)
    {
      xflag0 = ( vtx0->point.x >= tx );
      if (xflag0 == ( vtx1->point.x >= tx ))
      {
        if (xflag0)
        {
          inside_flag = !inside_flag;
        }
      }
      else
      {
        if (( vtx1->point.x -
          ( vtx1->point.y - ty ) * ( vtx0->point.x - vtx1->point.x ) /
          ( vtx0->point.y - vtx1->point.y ) ) >= tx)
        {
          inside_flag = !inside_flag;
        }
      }
    }
    yflag0 = yflag1;
    vtx0 = vtx1;
    vtx1 = &wedvertex[++index];
  }
  return inside_flag;
}

//This function orders the vertices of a polygon (without ruining it)
int VertexOrder(area_vertex *wedvertex, int count)
//int Carea::VertexOrder(area_vertex *wedvertex, int count)
{
  int i;
  int shift;
  area_vertex *tmp;
  area_vertex max;

  max=wedvertex[0];
  shift=0;
  for(i=1;i<count;i++)
  {
    if(wedvertex[i].point.y>max.point.y)
    {
      shift=i;
      max=wedvertex[i];
    }
    else
    {
      if(wedvertex[i].point.y==max.point.y)
      {
        if(wedvertex[i].point.x>max.point.x)
        {
          shift=i;
          max=wedvertex[i];
        }
      }
    }
  }
  if(shift)
  {
    tmp=new area_vertex[count];
    if(!tmp)
    {
      return -3;
    }
    for(i=0;i<count;i++)
    {
      tmp[i]=wedvertex[(i+shift)%count];
    }
    memcpy(wedvertex,tmp,count*sizeof(area_vertex) );
    delete [] tmp;
    return 1;
  }
  return 0;
}


//cstringmap functions
CStringMapCStringMapInt::~CStringMapCStringMapInt()
{
  CString key;
  CStringMapInt *value;
  POSITION pos;
  
  pos=GetStartPosition();
  while(pos)
  {
    GetNextAssoc(pos, key, value);
    if(value)
    {
      delete value;
      value=NULL;
    }
  }
}

int CStringMapArray::query_count(CString key)
{
  tooltip_data dummy;
  
  if(Lookup(key,dummy) )
  {
    if(dummy.data[0]==-1) return 0;
    if(dummy.data[1]==-1) return 1;
    if(dummy.data[2]==-1) return 2;
    return 3;
  }
  else return 0;
}

void CStringListLocEntry::Fixup(int index, int value)
{
  POSITION pos, oldpos;
  stringloc_entry stringfileloc;
  
  pos=GetHeadPosition();
  while(pos)
  {
    oldpos=pos;
    stringfileloc=GetNext(pos);
    if(stringfileloc.entry.bifindex>index)
    {
      stringfileloc.entry.bifindex=(unsigned short) (stringfileloc.entry.bifindex+value);
      CList<stringloc_entry, stringloc_entry&>::SetAt(oldpos,stringfileloc);
    }
  }
}

bool CStringListLocEntry::RemoveKey(CString ref)
{
  POSITION pos, oldpos;
  stringloc_entry stringfileloc;
  
  pos=GetHeadPosition();
  while(pos)
  {
    oldpos=pos;
    stringfileloc=GetNext(pos);
    if(stringfileloc.ref==ref)
    {
      RemoveAt(oldpos);
      return true;
    }
  }
  return false;
}

bool CStringListLocEntry::Lookup(CString ref, loc_entry &fileloc)
{
  POSITION pos;
  stringloc_entry stringfileloc;
  
  pos=GetHeadPosition();
  while(pos)
  {
    stringfileloc=GetNext(pos);
    if(stringfileloc.ref==ref)
    {
      fileloc=stringfileloc.entry;
      return true;
    }
  }
  return false;
}
  
bool CStringListLocEntry::SetAt(CString ref, loc_entry &fileloc)
{
  POSITION pos;
  stringloc_entry stringfileloc;
  
  pos=GetHeadPosition();
  while(pos)
  {
    stringfileloc=GetAt(pos);
    if(stringfileloc.ref==ref)
    {
      stringfileloc.entry=fileloc;
      CList<stringloc_entry, stringloc_entry&>::SetAt(pos,stringfileloc);
      return true;
    }
    GetNext(pos);
  }
  return false;
}

void CStringListLocEntry::RemoveAll()
{
  while(GetCount())
  {
    RemoveTail();
  }
}

POSITION CStringListLocEntry::AddList(CString ref, loc_entry entry)
{
  stringloc_entry tmp;
  
  tmp.ref=ref;
  tmp.entry=entry;
  return AddTail(tmp);
}

void CStringMapLocEntry::Fixup(int index, int value)
{
  POSITION pos;
  CString ref;
  loc_entry loc;
  
  pos=GetStartPosition();
  while(pos)
  {
    GetNextAssoc(pos,ref,loc);
    if(loc.bifindex>index)
    {
      loc.bifindex=(unsigned short) (loc.bifindex+value);
      SetAt(ref,loc);
    }
  }
}

int CStringMapLocEntry::Lookup(CString key, loc_entry &loc)
{
  if(key=="STATDESC")
  {
    nop();
  }
  loc.bifname.Empty();
  loc.resref.Empty();
  loc.cdloc=0;
  loc.index=0;
  loc.size=0;
  loc.bifindex=0;
  return CMap<CString, LPCSTR, loc_entry, loc_entry&>::Lookup(key,loc);
}
