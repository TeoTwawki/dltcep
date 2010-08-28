
#if !defined(AFX_STRUCTS_H__42E3FDEA_0FCA_4733_8AD1_6E396250B019__INCLUDED_)
#define AFX_STRUCTS_H__42E3FDEA_0FCA_4733_8AD1_6E396250B019__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//ie structs

#include "myafxtempl.h"  //the original stuff was too restricting, trying to overcome that

#pragma pack(1)

#define SLOT_COUNT  38
#define PST_SLOT_COUNT 46
#define IWD2_SLOT_COUNT  50
#define SND_SLOT_COUNT 100
#define SND_SLOT_IWD2 64
#define IWD2_NUM_FEAT 64
#define IWD2_SKILL_COUNT 66

#define BG2_SPSLOTNUM 3
#define IWD2_SPSLOTNUM 7

#define ATTR_CRITICAL 1
#define ATTR_TWOHAND  2
#define ATTR_MOVABLE  4
#define ATTR_DISPLAY  8
#define ATTR_CURSED   16
#define ATTR_COPYABLE 32
#define ATTR_MAGICAL  64
#define ATTR_BOW      128
#define ATTR_SILVER   256
#define ATTR_COLD     512
#define ATTR_STOLEN   1024
#define ATTR_CONVERS  2048
//all the rest is unknown

//bit locations for matched fields
#define FLG_MPROJ     0
#define FLG_MPROJ2    1
#define FLG_MTYPE     2
#define FLG_MTYPE2    3
#define FLG_MFEAT     4
#define FLG_MFEAT2    5
#define FLG_MRES      6
#define FLG_MITEM     7
#define FLG_MEA        8
#define FLG_MEA2       9
#define FLG_MGENERAL   10
#define FLG_MGENERAL2  11
#define FLG_MRACE      12
#define FLG_MRACE2     13
#define FLG_MCLASS     14
#define FLG_MCLASS2    15
#define FLG_MSPECIFIC  16
#define FLG_MSPECIFIC2 17
#define FLG_MGENDER    18
#define FLG_MGENDER2   19
#define FLG_MALIGN     20
#define FLG_MALIGN2    21
#define FLG_MVAR       22
#define FLG_MSTRREF    23

#define FLG_CHANGE     30
#define FLG_CHANGE2    31
//internal
typedef struct {
  CString bifname;//bif or filename
  long index;
  long size;
  unsigned short bifindex;   // used for extraction (marks bif), original index
  unsigned short cdloc;
  CString resref;            // the key itself
} loc_entry;

typedef struct {
  CString ref;
  loc_entry entry;
} stringloc_entry;

//dialog.tlk header
typedef struct {
  char signature[4];
  char version[4];
  short unused;
  long entrynum;
  long start;
} tlk_header;

typedef char resref_t[8];

typedef struct {
  unsigned short flags;
  resref_t soundref;
  unsigned long pitch;
  unsigned long volume;
  long offset;
  long length;
} tlk_reference;

typedef struct {
  tlk_reference reference;
  CString text;
} tlk_entry;

//bif
typedef struct {
  char signature[4];
  char version[4];
  long file_entries;
  long tile_entries;
  long file_offset;
} bif_header;

typedef struct {
  long res_loc;
  long offset;
  long size;
  short restype;
  short unused;
} bif_entry;

typedef struct {
  long res_loc;
  long offset;
  long numtiles;     //extra field compared to bif_entry
  long size;
  short restype;
  short unused;
} tis_entry;

typedef struct {
  CString filename;
  long uncompressed;
  long compressed;
} sav_entry;

//chitin.key
typedef struct {
  char signature[4]; //'KEY '
  char version[4]; //'V1  '
  long numbif;
  unsigned long numref;
  long offbif;
  long offref;
} key_header;

typedef struct {
  long biflen;
  long offbif; //name offset in this file
  short namelen; //name length
  short location; //location of bif (we will always look in the data section ?)
} keybif_entry;

typedef struct {
  long biflen;
  CString bifname;
  short location;
} membif_entry;

typedef struct {
  resref_t filename;
  short restype;
  unsigned long residx;
} key_entry;

//*.chu
typedef struct
{
  char filetype[4];
  char revision[4];
  long windowcnt;
  long controloffs; //this could be checked easily
  long windowoffs;
} chui_header;

typedef struct
{
  short windowid;
  short unknown1;   //unknown
  short xpos, ypos;
  short width, height;
  short flags;      //1 if has background
  short controlcount;
  char mos[8];
  short controlidx; //first control index
  short unknown2;   //may be strref
} chui_window;

typedef struct
{
  short controlid;
  short idflags;
  short xpos, ypos;
  short width, height;
  char controltype;
  unsigned char unknown;
} chui_control_common;

typedef struct//0
{
  char bam[8];
  unsigned char cycle;
  unsigned char flags;
  short unpressed;
  short pressed;
  short selected;
  short disabled;
} chui_button;

typedef struct//1
{
  char mos1[8];
  char mos2[8];
  char bam[8];
  short jumpcount;    //if this is 0 then the bam is a mos, and its width is the jumpcount
  short cycle;        //this is zero if the progressbar is a mos
  short xpos, ypos;   //the position of the progressbar 'bam' if it is a mos
  short xposcap, yposcap; //the position of the cap 'bam' if it is a mos
} chui_progress;

typedef struct//2
{
  char mos[8];
  char bam[8];
  short cycle;
  short released;
  short grabbed;
  short xpos;
  short ypos;
  short jumpwidth;
  short jumpcount;
  short unknown5;
  short unknown6;
  short unknown7;
  short unknown8;
} chui_slider;

typedef struct//3
{
  char mos1[8];
  char mos2[8];
  char mos3[8];
  char cursor[8]; //cursor bam
  short cycle;
  short frame;
  short xpos;
  short ypos;
  long unknown2;
  char font[8]; //font bam
  char unknown3[34];
  short length;
  long uppercase;
} chui_editbox;

typedef struct//5
{
  char bam1[8]; //initials
  char bam2[8]; //text
  unsigned char rgb1r,rgb1g,rgb1b,rgb1a;
  unsigned char rgb2r,rgb2g,rgb2b,rgb2a;
  unsigned char rgb3r,rgb3g,rgb3b,rgb3a;
  long scrollcontrolid;
} chui_textarea;

typedef struct //6
{
  long strref;
  char font[8];
  unsigned char rgb1r,rgb1g,rgb1b,rgb1a;
  unsigned char rgb2r,rgb2g,rgb2b,rgb2a;
  short justify;
} chui_label;

typedef struct //7
{
  char bam[8];
  short cycle;
  short upunpressed;
  short uppressed;
  short downunpressed;
  short downpressed;
  short trough;
  short slider;
  long textcontrolid;
} chui_scrollbar;

//*.spl
typedef struct
{
  char filetype[4];       //'SPL '
  char revision[4];       //'V1.0'
  long spellname;
  long unknown0c;
  char wavc[8];           //completion sound
  unsigned char itmattr;
  unsigned char splattr;
  unsigned short unkattr2;
  short spelltype;
  unsigned short school1;
  unsigned short priesttype; //0, 4000, 8000
  unsigned short castingglow;
  unsigned char unknown24;
  short school3; //spell school from school.ids
  unsigned char sectype; //secondary type from sectype.ids
  long unknown28;
  long unknown2c;
  long unknown30;
  long level;
  short unknown38;
  char icon[8];
  short unknown42;
  long unknown44;
  long unknown48;
  long unknown4c;
  long desc;     //strref
  long unknown54;
  long unknown58;
  long unknown5c;
  long unknown60;
  long extheadoffs;
  short extheadcount;
  long featureoffs;
  short featblkoffs;
  short featblkcount;
} spell_header;

typedef struct
{
  unsigned char spellform;
  unsigned char unknown01;
  unsigned short location; //innate or spell (the high nibble disables the spell)
  char useicon[8];
  unsigned char target_type;
  unsigned char target_num;
  unsigned short range;
  unsigned short level;
  unsigned long speed;
  unsigned long unknown16;
  unsigned long unknown1a;
  unsigned short fbcount;
  unsigned short fboffs;
  short unknown22;
  short unknown24;
  unsigned short proref;   //projectl.ids reference
} spl_ext_header;

//*.itm
typedef struct
{
  char filetype[4];       //'ITM '
  char revision[4];       //'V1.0'
  long unidref;           //dialog.tlk reference
  long idref;             //dialog.tlk reference
  char destname[8];       //filename(.itm)
  unsigned short itmattr; //item attribute bits
  unsigned short splattr; //spell attribute bits
  short itemtype;         //2 byte chars
  char  usability[4];     //bits class/alignment/race
  char  animtype[2];      //animation type
  unsigned char minlevel; //minimum level
  unsigned char unknown1;
  unsigned char minstr;   //minimum strength 
  unsigned char unknown2; 
  unsigned char minstrpl; //minimum strength 18 + x
  unsigned char kits1;    //bits
  unsigned char minint;   //
  unsigned char kits2;    //more kit bits
  unsigned char mindex;
  unsigned char kits3;
  unsigned char minwis;
  unsigned char kits4;
  unsigned char mincon;
  unsigned char weaprof;  //weapon proficiency type
  unsigned char mincha;   //
  unsigned char unknown3;
  unsigned long price;     //item price
  short maxstack;          //maximum stack
  char  invicon[8];        //inventory icon
  short loreid;            //lore to identify
  char  grnicon[8];        //ground icon
  unsigned long weight;
  long uniddesc; //unidentified description reference
  long iddesc;   //identified desc. ref.
  char descicon[8];       //description icon
  unsigned long mplus;    //magic level
  long extheadoffs;
  short extheadcount;
  long featureoffs;
  short featblkoffs;
  short featblkcount;
} item_header;

typedef struct
{
  unsigned char unknown[16];
} iwd2_header;

typedef struct
{
  char dialog[8];
  unsigned long nameref;
  unsigned short colour;    //wielding colour
  unsigned char unknown80[26];
} pst_header;

typedef struct
{
  long capacity;
  unsigned char unknown[80];
} iwd2store_header;

typedef struct
{
  unsigned char attack_type;
  unsigned char force_id;
  unsigned char location;
  unsigned char unknown03;          //0x03
  char useicon[8];
  unsigned char target_type;
  unsigned char target_num;
  unsigned short range;
  unsigned short projectile;
  unsigned short speed;
  short thaco;
  unsigned char damdice;
  unsigned char school;
  unsigned char dammult;
  unsigned char sectype;
  short damplus;
  unsigned short damtype;
  unsigned short fbcount;
  unsigned short fboffs;
  unsigned short charges;
  unsigned short drained;
  unsigned long flags;
  unsigned short proref;       //projectl.ids reference
  unsigned short animation[3];
  unsigned short missile[3];
} ext_header;

typedef union
{
  long parl;
  unsigned short parw[2];
  unsigned char parb[4];
}  parameter;

typedef struct {
  short feature;
  unsigned char target;
  unsigned char power;
  parameter par1;
  parameter par2;
  unsigned char timing;
  unsigned char resist;
  long duration;
  unsigned char prob2;
  unsigned char prob1;
  char vvc[8];
  long count;
  long sides;
  long stype;
  long sbonus;
  long unknown2c;
} feat_block;

typedef struct {
  char filetype[4];       //'STOR'
  char revision[4];       //'V1.0'
  long type; //store type
  long strref; //store name
  long flags; //bit 0 means this shop sells items (important)
  long selling;    //selling markup  1
  long buying;     //buying markup   2
  long depreciation;  //depreciacion 3
  short steal;     //steal success   4
  short capacity;  //capacity
  char dlgresref1[8];
  long pitemoffset; 
  long pitemcount; 
  long offset;    //important
  long itemcount; //important
  long lore;
  long idprice; 
  char dlgresref2[8];
  long drinkoffset;
  long drinkcount;
  char dlgresref3[8];
  long rent;
  long prices[4];
  long cureoffset;
  long curecount;
  unsigned char unknown[36];
} store_header;

typedef struct {
  char drinkscript[8]; //drinkspell/drinkscript resref
  long drinkname;
  long price;
  long strength; //rumour-drink strength
} store_drink;

typedef struct {
  char curename[8]; //spell resref
  long price;       //cure price
} store_cure;

typedef struct {
  char itemname[8];  //resref
  short unknown08;
  unsigned short usages[3];
  long flags;
  long count;        //count must be 1 for bags
  long infinite;
} store_item_entry;

typedef struct {
  char itemname[8];  //resref
  short unknown08;
  unsigned short usages[3];
  long flags;
  long count;        //count must be 1 for bags
  long infinite;
  long trigger;
  char unknown2c[56];
} store_item_entry11; //this is for sto v1.1

typedef struct {
  char filetype[4];       //'PRO '
  char revision[4];       //'V1.0'
  short type;
  short speed;
  short sparks;
  short unknown0e;
  char wavc1[8];
  char wavc2[8];
  char wavc3[8];
  short spkcolour;   //cannot be 0 if there are sparks (must be 1-12)
  short spknumber;
  long extflags;    //0x2c //flags used by gemrb
  long text;        //0x30 //text strref
  long rgb;         //0x34 //rgb single pulse color
  short pulsespeed; //0x36 //pulse speed
  short shake;      //0x38 //screen shake
  char unknown3c[196]; //unknown till next 256 block
  unsigned long attr;
  char bam1[8];
  char bam2[8];
  unsigned char seq1, seq2; //both seq for shadow/projectile
  short lightz;
  short lightx;
  short lighty;
  char bmp[8]; //sttravl.. (a bmp filename)
  unsigned char colours[7]; //more colours
  unsigned char smokepuff;
  unsigned char smokecols[7];
  unsigned char aim; // 1 or 5
  unsigned short smokeanim;  //from animate IDS
  char trailbam[3][8];
  short trailtype[3];
  char unknown154[172];
} proj_header;

typedef struct {
  short aoe; //bitflags
  short flags;
  short tradius;//256=30' 
  short radius; // 256=30'
  char wavc1[8]; //trigger sound
  short delay; 
  unsigned short fragmentanim; // animate.ids
  short proref;  //this is a projectile too, played at setoff
  unsigned char duration;
  unsigned char expgr2; //set off
  short expclr;
  short projectile; //another projectile comes now
  char vvc[8];      //center animation (x)
  short conewidth;
  short unknown26;
  char spread[8];   //spread animation  (a) 0x30
  char second[8];   //recoil animation? (r) 0x38
  char wavc2[8];    //area sound            0x40
  long gemrbflags;  //gemrb specific area flags 0x48
  char unknown44[188];
} proj_extension;

typedef struct
{
  char filetype[4]; //'EFF '
  char revision[4]; //'V2.0 '
  char signature[4]; //'EFF '
  char version[4]; //'V2.0 '
//  unsigned short feature; // hardcoded effect number
//  unsigned short filler;
  unsigned long feature;
  unsigned long target;
  unsigned long power;
  parameter par1;
  parameter par2;
  unsigned short timing;
  unsigned short unknown2; //part of timing
  unsigned long duration;
  unsigned short prob2;
  unsigned short prob1;
  char resource[8];
  unsigned long count;
  unsigned long sides;
  unsigned long stype;
  long sbonus;
  long unknown2c;
  long school;
  long u3,u4,u5;
  long resist;
  long par3;
  long par4;
  unsigned char unused2[8];
  char vvc[8];
  char resource3[8];
  long sx, sy;
  long dx, dy;
  long unused3;
  char source[8];
  long unused4;
  long projectile;//the effect uses this projectile (from spell)
  long slot;      //the equipping slot
  char variable[32];
  long casterlevel;//(in items this is set to 10)
  long unused8;
  long sectype;
  unsigned char unused9[60];
} eff_header;

//+2 is for the equipped item info (dword)
typedef short creature_itemslots[IWD2_SLOT_COUNT+2]; //IWD2 has more slots

typedef struct
{
  char filetype[4]; //'CHR '
  char revision[4]; //'V2.1'-bg2/tob,  v1.0 - iwd
  char name[32];
  long creoffset;
  long cresize;
  char filler[0x34];
} character_header;

typedef struct
{
  char filetype[4]; //'CHR '
  char revision[4]; //'V2.2'-iwd2
  char name[32];
  long creoffset;
  long cresize;
  char filler[0x1f4];
} character_iwd2_header;

typedef struct
{
  char filetype[4]; //'CRE '
  char revision[4]; //'V1.0'
  long longname;
  long shortname;
  unsigned long flags;
  long expval;
  long expcur;
  long gold;
  long state;
  short curhp;
  short maxhp;
  unsigned long animid;
  unsigned char colours[7];
  char effbyte;
  char iconm[8];
  char iconl[8];
  unsigned char reputation;   //unused in team
  unsigned char hideskill;
  short natac;
  short effac;
  short crushac;
  short missac;
  short piercac;
  short slashac;
  char thac0;
  unsigned char attacks;
  unsigned char sdeath;
  unsigned char swands;
  unsigned char spoly;
  unsigned char sbreath;
  unsigned char sspell;
  char resfire;
  char rescold;
  char reselec;
  char resacid;
  char resmagic;
  char resmfire;
  char resmcold;
  char resslash;
  char rescrush;
  char respierc;
  char resmiss;
  unsigned char detillusion;
  unsigned char settraps;
  unsigned char lore;
  unsigned char lockpick;
  unsigned char stealth;
  unsigned char findtrap;
  unsigned char pickpock;
  unsigned char fatigue;
  unsigned char intox;
  char luck;
  unsigned char unused[21];       //old weapon proficiencies
  unsigned char tracking;
  unsigned long unused2[8];
  long strrefs[SND_SLOT_COUNT];
  unsigned char levels[3];
  unsigned char sex;
  unsigned char strstat;
  unsigned char strbon;
  unsigned char intstat;
  unsigned char wisstat;
  unsigned char dexstat;
  unsigned char constat;
  unsigned char chastat;
  unsigned char morale;
  unsigned char moralebreak;
  unsigned char enemy;
  unsigned char moralerecover;
  unsigned char unknown243;
  long kit;
  char scripts[5][8];
  unsigned char idsea;
  unsigned char idsgeneral;
  unsigned char idsrace;
  unsigned char idsclass;
  unsigned char idsspecific;
  unsigned char idsgender;
  unsigned char idsinternal[5];
  unsigned char idsalign;
  short globalID;
  short localID;
  char dvar[32];
  long bookoffs;
  long bookcnt;
  long selectoffs;
  long selectcnt;
  long memoffs;
  long memcnt;
  //////
  long itemslots;
  long itemoffs;
  long itemcnt;
  long effectoffs;
  long effectcnt;
  char dialogresref[8];
} creature_header;

typedef struct
{
  char resref[8];
  long flags;
  short timing;
  unsigned char type;
  unsigned char u1;
  long duration;
  long u2;
  long u3;
  long u4;
  long u5;
} creature_pst_overlay;

typedef struct
{
  char filetype[4]; //'CRE '
  char revision[4]; //'V1.2'
  long longname;
  long shortname;
  unsigned long flags;
  long expval;
  long expcur;
  long gold;
  long state;
  short curhp;
  short maxhp;
  unsigned long animid;
  unsigned char unused[7]; //colors
  char effbyte;
  char iconm[8];
  char iconl[8];
  unsigned char reputation;   //unused in team
  unsigned char hideskill;
  short natac;
  short effac;
  short crushac;
  short missac;
  short piercac;
  short slashac;
  char thac0;
  unsigned char attacks;
  unsigned char sdeath;
  unsigned char swands;
  unsigned char spoly;
  unsigned char sbreath;
  unsigned char sspell;
  char resfire;
  char rescold;
  char reselec;
  char resacid;
  char resmagic;
  char resmfire;
  char resmcold;
  char resslash;
  char rescrush;
  char respierc;
  char resmiss;
  unsigned char detillusion;
  unsigned char settraps;
  unsigned char lore;
  unsigned char lockpick;
  unsigned char stealth;
  unsigned char findtrap;
  unsigned char pickpock;
  unsigned char fatigue;
  unsigned char intox;
  char luck;
  unsigned char profs[21];
  unsigned char tracking;
  unsigned long unused2[8];
  long strrefs[SND_SLOT_COUNT];
  unsigned char levels[3];
  unsigned char sex;
  unsigned char strstat;
  unsigned char strbon;
  unsigned char intstat;
  unsigned char wisstat;
  unsigned char dexstat;
  unsigned char constat;
  unsigned char chastat;
  unsigned char morale;
  unsigned char moralebreak;
  unsigned char enemy;
  unsigned char moralerecover;
  unsigned char unknown243;
  long kit;
  char scripts[5][8];
  char unknown270[36];
  long overlayoffs;
  long overlaysize;
  long magexp;
  long thiefxp;
  char internals[20]; //2a4
  char good, law, lady, murder;
  char bestiary[32]; //2bc
  unsigned char dialogradius;
  unsigned char feetcircle;
  unsigned char unknown2de;
  unsigned char colornum;
  unsigned long killflags;
  short colours[7]; //24e
  unsigned char unknown2f2[3];
  unsigned char colorslots[7];
  unsigned char unknown2fc[21];
  unsigned char idsspecies;
  unsigned char idsteam;
  unsigned char idsfaction;
  unsigned char idsea;
  unsigned char idsgeneral;
  unsigned char idsrace;
  unsigned char idsclass;
  unsigned char idsspecific;
  unsigned char idsgender;
  unsigned char idsinternal[5];
  unsigned char idsalign;
  short globalID;
  short localID;
  char dvar[32];
  long bookoffs;
  long bookcnt;
  long selectoffs;
  long selectcnt;
  long memoffs;
  long memcnt;
  long itemslots;
  long itemoffs;
  long itemcnt;
  long effectoffs;
  long effectcnt;
  char dialogresref[8];
} creature_pst_header;

typedef struct
{
  char filetype[4]; //'CRE '
  char revision[4]; //'V9.0'
  long longname;
  long shortname;
  unsigned long flags;
  long expval;
  long expcur;
  long gold;
  long state;
  short curhp;
  short maxhp;
  unsigned long animid;
  unsigned char colours[7];
  char effbyte;
  char iconm[8];
  char iconl[8];
  unsigned char reputation;   //unused in team
  unsigned char hideskill;
  short natac;
  short effac;
  short crushac;
  short missac;
  short piercac;
  short slashac;
  char thac0;
  unsigned char attacks;
  unsigned char sdeath;
  unsigned char swands;
  unsigned char spoly;
  unsigned char sbreath;
  unsigned char sspell;
  char resfire;
  char rescold;
  char reselec;
  char resacid;
  char resmagic;
  char resmfire;
  char resmcold;
  char resslash;
  char rescrush;
  char respierc;
  char resmiss;
  unsigned char detillusion;
  unsigned char settraps;
  unsigned char lore;
  unsigned char lockpick;
  unsigned char stealth;
  unsigned char findtrap;
  unsigned char pickpock;
  unsigned char fatigue;
  unsigned char intox;
  char luck;
  unsigned char profs[21];
  unsigned char tracking;
  unsigned long unused2[8];
  long strrefs[SND_SLOT_COUNT];
  unsigned char levels[3];
  unsigned char sex;
  unsigned char strstat;
  unsigned char strbon;
  unsigned char intstat;
  unsigned char wisstat;
  unsigned char dexstat;
  unsigned char constat;
  unsigned char chastat;
  unsigned char morale;
  unsigned char moralebreak;
  unsigned char enemy;
  unsigned char moralerecover;
  unsigned char unknown243;
  long kit;
  char scripts[5][8];
  char scriptflags[4];  //3. flag is kill var count
  //long unknown270;
  short internals[5];
  char scriptname[32];
  char scriptname2[32];
  short unknown;
  short savedx, savedy, savedir; //saved location
  char unknown2be[18];
  unsigned char idsea;
  unsigned char idsgeneral;
  unsigned char idsrace;
  unsigned char idsclass;
  unsigned char idsspecific;
  unsigned char idsgender;
  unsigned char idsinternal[5];
  unsigned char idsalign;
  short globalID;
  short localID;
  char dvar[32];
  long bookoffs;
  long bookcnt;
  long selectoffs;
  long selectcnt;
  long memoffs;
  long memcnt;
  long itemslots;
  long itemoffs;
  long itemcnt;
  long effectoffs;
  long effectcnt;
  char dialogresref[8];
} creature_iwd_header;

typedef struct
{
  char filetype[4]; //'CRE '
  char revision[4]; //'V2.2'
  long longname;
  long shortname;
  unsigned long flags;
  long expval;
  long expcur;
  long gold;
  long state;
  short curhp;
  short maxhp;
  unsigned long animid;
  char colours[7];
  char effbyte;
  char iconm[8];
  char iconl[8];
  char reputation;   //unused in team
  char hideskill;
  short natac;
  short crushac;
  short missac;
  short piercac;
  short slashac;
  char thac0;            //unused
  unsigned char attacks; //unused
  unsigned char sfort;
  unsigned char sreflex;
  unsigned char swill;
  char resfire;
  char rescold;
  char reselec;
  char resacid;
  char resmagic;
  char resmfire;
  char resmcold;
  char resslash;
  char rescrush;
  char respierc;
  char resmiss;
  char resmdam;
  unsigned char unknown61[4];
  unsigned char fatigue;
  unsigned char intox;
  char luck;
  unsigned char turnundead;
  unsigned char unknown69[33];
  unsigned char totlevel;
  unsigned char levels[11];
  unsigned char unknown96[22];
  long strrefs[64];
  unsigned char script1[8]; //team script
  unsigned char script2[8]; //special script1
  long dr;                  //damage resistance
  long feat[6];
  unsigned char feats[64];
  unsigned char skills[IWD2_SKILL_COUNT];
  unsigned char cr;
  unsigned char enemy[8];
  unsigned char subrace;
  unsigned char unknown264;
  unsigned char sex;
  unsigned char strstat;
  unsigned char intstat;
  unsigned char wisstat;
  unsigned char dexstat;
  unsigned char constat;
  unsigned char chastat;
  unsigned char morale;
  unsigned char moralebreak;
  unsigned char moralerecover;
  unsigned char unknown26f;
  long kit;
  char scripts[5][8];
  char scriptflags[4];  //3. flag is kill var count
  short internals[5];
  char scriptname[32];
  char scriptname2[32];
  short unknown;
  short savedx, savedy, savedir; //saved location
  char unknown2f2[146];
  unsigned char idsea;
  unsigned char idsgeneral;
  unsigned char idsrace;
  unsigned char idsclass;
  unsigned char idsspecific;
  unsigned char idsgender;
  unsigned char idsinternal[5]; //func spec
  unsigned char idsalign;
  short globalID;
  short localID;
  char dvar[32];
  char unknown3b4[6];  //3 words
  long spelloffs[IWD2_SPSLOTNUM][9]; //7 types: bard, cleric, druid, paladin, ranger, sorc, wizard
  long spellcnt[IWD2_SPSLOTNUM][9];  //7 types
  long domainoffs[9];   //plus domain
  long domaincnt[9];
  long innateoffs;
  long innatecnt;
  long songoffs;
  long songcnt;
  long shapeoffs;
  long shapecnt;
  ////
  long itemslots;
  long itemoffs;
  long itemcnt;
  long effectoffs;
  long effectcnt;
  char dialogresref[8];
} creature_iwd2_header;

typedef struct
{
  unsigned long animid;
  char scripts[5][8];
  char dialogresref[8];
  char iwd2scripts[2][8];
  char scriptname[32];
} creature_data;

typedef struct {
  char resref[8];
  short level;
  short spelltype;
} creature_book;

typedef struct {
  long type;
  long total;
  long remaining;
  long unknown;
} creature_iwd2_spell;

typedef struct {
  short level;
  short num1;
  short num2;
  short spelltype;
  long index;
  long count;
} creature_select;

typedef struct {
  char resref[8];
  long flags;
} creature_memory;

typedef struct {
  char itemname[8];
  short unknown;
  short usages[3];
  long flags;
} creature_item;

typedef struct {
  char signature[4]; //'EFF '
  char version[4]; //'V2.0 '
  unsigned long feature; // hardcoded effect number
  unsigned long target;
  unsigned long power;
  parameter par1;
  parameter par2;
  unsigned char timing;
  unsigned char unknown;
  unsigned short unknown2;
  unsigned long duration;
  unsigned short prob2;
  unsigned short prob1;
  char resource[8];
  unsigned long count;
  unsigned long sides;
  unsigned long stype;
  long sbonus;
  long unknown2c;
  long school;
  long u3,u4,u5;
  long resist;  
  long par3;
  unsigned char unused2[12];
  char vvc[8];
  unsigned char unused3[28];
  char source[8];
  long unused4;
  long projectile;//the effect uses this projectile
  long unused6;  
  char variable[32];
  long unused7;
  long unused8;
  long sectype;
  unsigned char unused9[60];
} creature_effect;

typedef struct
{
  char filetype[4]; //'VVC '
  char revision[4]; //'V1.0 '
  char bam[8];
  long unknown10;
  long unknown14;
  unsigned char transparency;
  unsigned char trans2;
  unsigned char colouring;
  unsigned char col2;
  long unknown1c;
  long sequencing;
  long unknown24;
  long xpos;
  long ypos;
  long unknown30;
  long framerate;
  long unknown38;
  long unknown3c;
  long position;
  long unknown44;
  long unknown48;
  long zpos;
  long unknown50;
  long unknown54;
  long unknown58;
  long duration;
  long unknown60;
  long unknown64;
  long seq1;
  long seq2;
  long unknown70;
  long unknown74;
  char sound1[8];
  char sound2[8];
  long unknown88;
  long unknown8c;
  long unknown90;
  char unknown94[8];
  char unused[336];
} vvc_header;

// *.are

typedef struct
{
  char filetype[4];       //'AREA'
  char revision[4];       //'V1.0'
  char wed[8];
  long lastsaved;
  unsigned long areaflags; //can save, dream, etc
  char northref[8];
  long northflags;
  char westref[8];
  long westflags;
  char southref[8];
  long southflags;
  char eastref[8];
  long eastflags;
  unsigned char areatype; //forest, extended night, etc
  unsigned char unkflags;
  short rain;
  short snow;
  short fog;
  short lightning;
  short unknown52; //weather ?
  long actoroffset;
  short actorcnt;
  short infocnt;
  long infooffset;
  long spawnoffset;
  long spawncnt;
  long entranceoffset;
  long entrancecnt;
  long containeroffset;
  short containercnt;
  short itemcnt;
  long itemoffset;
  long vertexoffset;
  short vertexcnt;
  short ambicnt;
  long ambioffset;
  long variableoffset;
  long variablecnt;
  long unknown90;
  char scriptref[8];
  long exploredsize;
  long exploredoffset;
  long doorcnt;
  long dooroffset;
  long animationcnt;
  long animationoffset;
  long tilecnt;
  long tileoffset;
  long songoffset;
  long intoffset;
  long mapnoteoffset;
  long mapnotecnt;
  long pstmapnotecnt; //also trapoffset
  long trapcnt;
  char sleep1[8];
  char sleep2[8];
  char unused[56];
} area_header;

typedef struct {
  char filetype[4];       //'WED '
  char revision[4];       //'V1.3'
  long overlaycnt;
  long doorcnt;
  long overlayoffset;
  long secheaderoffset;
  long dooroffset;
  long tilecelloffset;
} wed_header;

typedef struct {
  short width;
  short height;
  char tis[8];
  long unknownc;
  long tilemapoffset;
  long tileidxoffset;
} wed_overlay;

typedef struct {
  long wallpolycnt;
  long polygonoffset;
  long verticeoffset;
  long wallgroupoffset;
  long polygonidxoffset;
} wed_secondary_header;

typedef struct {
  char doorid[8]; //matched with area_door.doorid
  short closed;
  short firstdoortileidx;//door tile cell indices are 16 bit values
  short countdoortileidx;
  short countpolygonopen;
  short countpolygonclose;
  long offsetpolygonopen;
  long offsetpolygonclose;
} wed_door;

typedef struct {
  unsigned short firsttileprimary;
  unsigned short counttileprimary; //tile indices are 16 bit values
  short alternate;
  short flags;
  short unknown;
} wed_tilemap;

// polygon indices could be POINTS type, x=index, y=count

typedef struct {
  short index;
  short count;
} wed_polyidx;

typedef struct {
  long firstvertex;
  long countvertex;
  unsigned char flags;
  unsigned char unkflags;
  short minx, maxx, miny, maxy;  
} wed_polygon;

typedef struct {
  char actorname[32];
  short posx;
  short posy;
  short destx;
  short desty;
  long fields; //used fields
  long unknown2c;
  long animation;//actor's animation type
  short face;
  short unknown36;
  long unknown38; //time to appear ?
  long unknown3c;
  unsigned long schedule;
  long talknum;
  char dialog[8];
  char scroverride[8];
  char scrgeneral[8];
  char scrclass[8];
  char scrrace[8];
  char scrdefault[8];
  char scrspecific[8];
  char creresref[8];
  long creoffset;
  long cresize;
  char scrarea[8];
  char notused[120];
} area_actor;

typedef struct {
  char infoname[32];
  short triggertype;
  short p1x, p1y, p2x, p2y;
  short vertexcount;
  long vertindx;
  long unknown30;
  long cursortype;
  char destref[8]; //area resref
  char destname[32]; //destination name in destination area
  long infoflags;
  long strref;
  short trapdetect;
  short trapremove;
  short trapped;
  short detected;
  short launchx,launchy;
  char key[8];
  char scriptref[8]; //script of this trigger  
  short pointx, pointy;
  char unknown[44];
  short pstpointx, pstpointy;
  long dialogstrref; //at least in pst, trigger points can talk
  char dialogref[8];
} area_trigger;

typedef struct {
  char spawnname[32];
  short px, py;
  char creatures[10][8];
  short creaturecnt;
  short difficulty;
  short delay; //seconds
  short method; //usually 1
  long expiry;
  short randomwalk;
  short unknown82;
  short max;
  short min; // ?
  unsigned long schedule;
  short percent1;
  short percent2;
  char unused[56];
} area_spawn;

typedef struct {
  char entrancename[32];
  short px, py;
  short face;
  short unknown26;
  short unknown28;
  short unknown2a;
  char unused[60];
} area_entrance;

typedef struct {
  char containername[32];
  short posx, posy;
  short type;
  short lockdiff;
  long locked;
  short trapdetect;
  short trapremove;
  short trapped;
  short trapvisible;
  short launchx, launchy;
  short p1x, p1y, p2x, p2y; //minimum bounding box
  long firstitem;
  long itemcount;
  char trapscript[8];
  long firstvertex;
  short vertexcount;
  short unknown56;
  char trapname[32];
  char keyitem[8];
  short unknown80;
  short unknown82;
  long strref;             //STRREF!!!
  char unused[56];
} area_container;

typedef struct {
  char itemname[8];
  short unknown8;
  short usages[3];
  long flags;
} area_item;

typedef struct {
  char ambiname[32];
  short posx, posy;
  short radius;
  short height;
  short unknown28;
  short unknown2a;
  short unknown2c;
  short volume;
  char ambients[10][8];
  short ambientnum;
  short ambientnum2; // ????
  long silence;
  long soundnum; // ??
  unsigned long schedule;
  long flags;
  long unknown90;
  char unused[60];
} area_ambient;

typedef struct {
  char variablename[32];
  long unknown20;
  long unknown24;
  long value;
  char unused[40];
} area_variable;

typedef struct {
  char doorname[32];
  char doorid[8];
  long flags;
  long firstvertexopen;
  short countvertexopen;
  short countvertexclose;
  long firstvertexclose;
  short op1x,op1y,op2x,op2y; //minimum bounding box for open
  short cp1x,cp1y,cp2x,cp2y; //minimum bounding box for close
  long firstblockopen;
  short countblockopen;
  short countblockclose;
  long firstblockclose;
  long unknown54; //short,short
  char openres[8];
  char closeres[8];
  long cursortype;
  short trapdetect;
  short trapremoval;
  short trapflags;
  short trapdetflags;
  short launchx, launchy;
  char key[8];
  char openscript[8];
  long locked;
  long lockremoval;
  short locp1x, locp1y;   //open location 1
  short locp2x, locp2y;   //open location 2
  long strref;            //check this for missing strings STRREF!!!
  char regionlink[32];
  long nameref;           //this is unsure
  char dialogref[8];      //also unsure
} area_door;

typedef struct {
  char animname[32];
  short posx, posy;
  unsigned long schedule;
  char bam[8];
  short cyclenum;
  short framenum;
  short flags;
  short unknown36;
  short height;
  short transparency;
  short current; //current frame?
  unsigned char progress;
  unsigned char skipcycle;
  char bmp[8];  //palette bmp
  long unknown48;
} area_anim;

typedef struct {
  short px,py;
  long strref;
  short unknownc;
  short colour;
  long cnt;
  char unused[36];
} area_mapnote;

typedef struct {
  long px, py;
  char text[500];
  long readonly;
  long unused[5];
} pst_area_mapnote;

typedef struct {
  char projectile[8];
  long offset;
  short size;
  short proj;
  long unknown10;
  short posx,posy;
  short unknown18;
  short unknown1a;
} area_trap;

typedef struct {
  char tilename[32];
  char resref[8]; //script ?
  long unknown;
  long prstr;
  long prnum;
  long secstr;
  long secnum;
  char unused[48];
} area_tile;

typedef struct { //10 songrefs, day,night,battle, etc
  long songs[10];
  char dayambi1[8];
  char dayambi2[8];
  long volumed;
  char nightambi1[8];
  char nightambi2[8];
  long volumen;
  long songflag;  //in pst
  char unused[60];
} area_song;

typedef struct {
  char intname[32];
  long strrefs[10];
  char creatures[10][8];
  short creaturecnt; //entry number
  short difficulty; //creature number depends on this
  long unknown9c;  //1000
  short unknowna0; //1000
  short unknowna2; //1000
  short maxnumber; //??
  short minnumber; //??
  short day;
  short night;
  short unknownac;
  short unknownae;
  char unused[52];
} area_int;

typedef struct {
  unsigned short x,y;
} area_vertex;

//this pointer list will free previous elements
//Do we need this for removehead/tail as well ?
class CMyPtrList : public CPtrList
{
public:
  void RemoveAt(POSITION pos)
  {
    void *poi;

    poi=GetAt(pos);
    if(poi) delete [] poi;
    CPtrList::RemoveAt(pos);
  }
  void SetAt(POSITION pos, void *poi)
  {
    void *poi2;

    poi2=GetAt(pos);
    if(poi2) delete [] poi2;
    CPtrList::SetAt(pos,poi);
  }
};

/* we won't use this, we will simply cut 4 bytes
typedef struct {
  char filetype[4]; //'DLG '
  char revision[4]; //'V1.0'
  long numstates;
  long offstates;
  long numtrans;
  long offtrans;
  long offsttrigger;
  long numsttrigger;
  long offtrtrigger;
  long numtrtrigger;
  long offaction;
  long numaction;
} dlg_header_old;
*/

typedef struct {
  char filetype[4]; //'DLG '
  char revision[4]; //'V1.0'
  long numstates;
  long offstates;
  long numtrans;
  long offtrans;
  long offsttrigger;
  long numsttrigger;
  long offtrtrigger;
  long numtrtrigger;
  long offaction;
  long numaction;
  unsigned long flags;
} dlg_header;

typedef struct {
  long actorstr;
  long trindex;   //transition index
  long trnumber;  //transition number
  long stindex;   //state trigger index
} dlg_state;

typedef struct {
  long flags;
  long playerstr;
  long journalstr;
  long trtrindex;   //transition trigger index
  long actionindex; //action index
  char nextdlg[8];  //transition dlg
  long stateindex;  //next state index
} dlg_trans;

typedef struct {
  long textoffset;
  long textlength;
} offset_length;

typedef struct {
  char filetype[4]; //'GAM '
  char revision[4]; //'V1.0' or 'V2.0'
  long gametime; //300 units == 1 hour
  short formation;
  short formations[5];
  long gold;
  short weather1;
  short weather2;
  long pcoffset;
  long pccount;
  long unknown1offset;
  long unknown1count;
  long npcoffset;
  long npccount;
  long variableoffset;
  long variablecount;
  char mainarea[8];
  long unknown48;
  long journalcount;
  long journaloffset;
  long reputation;
  char curarea[8];
  long controls;
  long unknown64;
  long familiaroffset;
  long slocoffset;
  long sloccount;
  long realtime;
  long ppoffset;
  long ppcount;
  char unknown80[52];
} gam_header;

typedef struct {
  char area[8];
  short xpos, ypos;
} gam_sloc;

typedef struct {
  char filetype[4]; //'GAME'
  char revision[4]; //'V1.1'
  long gametime; //300 units == 1 hour
  short formation;
  short formations[5];
  long gold;
  short unknown1c;
  short unknown1e;
  long pcoffset;
  long pccount;
  long unknown1offset;
  long unknown1count;
  long npcoffset;
  long npccount;
  long variableoffset;
  long variablecount;
  char mainarea[8];
  long unknown48;
  long journalcount;
  long journaloffset;
  long mazeoffset;
  long reputation;
  char curarea[8];
  long dvaroffset;
  long dvarcount;
  long familiaroffset;
  char unknownarea[8];
  char unknown74[64];
} gam_pst_header;

typedef struct {
  char familiars[9][8];
  long offset;
} gam_familiar;

typedef struct {
  short selected;
  short slot;
  long creoffset;
  long cresize;
  char creresref[8]; //name of resource (if creature isn't embedded)
  long direction;
  char curarea[8];
  short xpos, ypos;
  short vrectx, vrecty;
  //bardsong = 1
  //detecting traps = 2
  //hide in shadows = 3
  //turning undead = 4
  short action;   
  short happiness;
  char unknown2c[96];
} gam_npc;

typedef struct {
  short weapons[4];
  short wslots[4];
  char quickspells[3][8];
  short quickitems[3];
  short quickslots[3];
  char name[32];
  long talkcount;
  long nameofmpv; //name of most powerful vanquished
  long xpofmpv;   //xp of most powerful vanquished
  long absent;
  long joindate;
  long unknown3;
  long killxp; //this chapter
  long killnum;
  long totalkillxp;
  long totalkillnum;
  char favspell[4][8];
  short favspcnt[4];
  char favweap[4][8];
  short favwpcnt[4];
  char soundset[8];
} gam_bg_npc;

typedef struct {
  short weapons[4];
  short wslots[4];
  char quickspells[3][8];
  short quickitems[5];
  short quickslots[5];
  char name[32];
  long talkcount; //numtimestalkedto
  long nameofmpv; //name of most powerful vanquished
  long xpofmpv;   //xp of most powerful vanquished
  long absent;
  long joindate;
  long unknown3;
  long killxp; //this chapter
  long killnum;
  long totalkillxp;
  long totalkillnum;
  char favspell[4][8];
  short favspcnt[4];
  char favweap[4][8];
  short favwpcnt[4];
  char soundset[8];
} gam_pst_npc;

typedef struct {
  short weapons[4];
  short wslots[4];
  char quickspells[3][8];
  short quickitems[3];
  short quickslots[3];
  char name[32];
  long talkcount;
  long nameofmpv; //name of most powerful vanquished
  long xpofmpv;   //xp of most powerful vanquished
  long absent;
  long joindate;
  long unknown3;
  long killxp; //this chapter
  long killnum;
  long totalkillxp;
  long totalkillnum;
  char favspell[4][8];
  short favspcnt[4];
  char favweap[4][8];
  short favwpcnt[4];
  char soundset[8];
  char voiceset[32];
} gam_iwd_npc;

typedef struct {
  short weapons[8];
  short wslots[8];
  char quickspells[9][8];
  char qsclass[9];
  char unknown;
  short quickitems[3];
  short quickslots[3];
  char unknown00[144];
  long qslots[9];
  char name[32];
  long talkcount;
  long nameofmpv; //name of most powerful vanquished
  long xpofmpv;   //xp of most powerful vanquished
  long absent;
  long joindate;
  long unknown3;
  long killxp; //this chapter
  long killnum;
  long totalkillxp;
  long totalkillnum;
  char favspell[4][8];
  short favspcnt[4];
  char favweap[4][8];
  short favwpcnt[4];
  char soundset[8];
  char voiceset[32];
  char unknown4[194];
} gam_iwd2_npc;

typedef union {
  gam_bg_npc gbn;
  gam_pst_npc gpn;
  gam_iwd_npc gin;
  gam_iwd2_npc gwn;
} gam_npc_extension;

typedef struct {
  char unknown[20];
} gam_unknown1;

typedef struct {
  char unknown[1720]; //not so big hack
} gam_mazedata;

typedef struct {
  char variablename[32];
  char unknown20[8];
  long value;
  char unknown2c[40];
} gam_variable;

typedef struct {
  long strref;
  long time;
  unsigned char chapter;
  unsigned char unknown09;
  unsigned char section;
  unsigned char user; //1f means user entry
} gam_journal;

typedef struct {
  char filetype[4]; //'WMAP'
  char revision[4]; //'V1.0'
  long wmapcount;
  long wmapoffset;
} map_mainheader;

typedef struct {
  char mos[8];
  long width;
  long height;
  long mapnumber;
  long mapname;          //strref!!!
  long unknown18;
  long unknown1c;
  long areacount;
  long areaoffset;
  long arealinkoffset;
  long arealinkcount;
  char mapicons[8];
  char unused38[128];
} map_header;

typedef struct {
  char areaname[8]; //2da resref ?
  char arearesref[8]; //area resref
  char areadesc[32]; //for editors ?
  long flags;
  long bamindex; //bam index in mapicons.bam
  long xpos,ypos;
  long caption; //strref
  long tooltip; //strref
  char loadscreen[8]; //mos
  long northidx;
  long northcnt;
  long westidx;
  long westcnt;
  long southidx;
  long southcnt;
  long eastidx;
  long eastcnt;
  char unused70[128];
} map_area;

typedef struct {
  long link;          //link to another 'map_area'
  char entryname[32]; //an entrypoint (entrance in .are file)
  long distancescale;
  long flags; //entry direction (empty entryname is allowed?)
  char encounters[5][8]; //encounter areas
  long chance; //encounter chance
  char unused58[128];
} map_arealink;

typedef struct {
  char filetype[4];
  char version[4];
  unsigned long numtiles;  //global size/5120
  unsigned long tilelength;//5120
  unsigned long offset;    //20
  unsigned long pixelsize; //64
} tis_header;

typedef struct {
  char signature[2]; //BM
  unsigned long fullsize;     //offset+width*height
  unsigned long unknown1;      //zero, reserved
  unsigned long offset;       //0x36
  unsigned long headersize;   //0x28 (40) (the size of the rest of the header)
  unsigned long width;
  unsigned long height;
  unsigned short planes;      //1 ?
  unsigned short bits;        //24 3*8?
  unsigned long compression;  //0
  unsigned long size;         //width*height*3 (3 bytes for 24 bits)
  unsigned long hres;         //0xb12 ?
  unsigned long vres;         //0xb12 ?
  unsigned long colors;       //0
  unsigned long impcolors;    //important colors
} bmp_header;

typedef struct {
  char signature[2]; //BM
  unsigned long fullsize;     //offset+width*height
  unsigned long unknown1;      //zero, reserved
  unsigned long offset;       //0x36
  unsigned long headersize;   //0x0c (12) (the size of the rest of the header)
  unsigned short width;
  unsigned short height;
  unsigned short planes;      //1 ?
  unsigned short bits;        //24 3*8?
} bmpos2_header;

//tbg & iap

typedef struct
{
  char signature[4]; //TBG3
  long unused;       //null
  char filename[12]; //filename+extension  
  long fileoffset;
  long filelength;
  long genre;          //1-bg1, 2-pst, 3-iwd, 4-bg2, 5-iwd2
  long textentryoffset;//strings start
  long tlkentrycount;  //text header count
  long tlkentryoffset; //text header ofset
} tbg3_header;

typedef struct
{
  char signature[4]; //TBG4
  long unused;       //null
  char filename[12]; //filename+extension  
  long fileoffset;
  long filelength;
  long genre;          //1-bg1, 2-pst, 3-iwd, 4-bg2, 5-iwd2
  long textentryoffset;//strings start

  long tlkentrycount;  //text header count
  long tlkentryoffset; //text header ofset
  long strrefcount;
  long strrefoffset;
  long asciicount;
  long asciioffset;
} tbg4_header;

typedef struct
{
  char signature[4]; //TBG4
  long unused;       //null
  long npccount;
  long npcoffset;
  short genre;     //1-bg1, 2-pst, 3-iwd, 4-bg2, 5-iwd2
} tbgn_header;

typedef struct
{
  char npcname[8];
  char area[8];
  short x;
  short y;
} npc_header;

typedef struct //text header
{
  long offset;
  long length;
  char soundref[8];
  unsigned long volume;
  unsigned long pitch;
  unsigned short flags;
  long strrefoffset;
  long strrefcount;
  long asciioffset;
  long asciicount;
  long asciimodoffset;
  long asciimodcount;
} tbg_tlk_reference;

typedef struct
{
  long offset;
  long length;
  long insert;
  char soundref[8];
} tbg3_tlk_reference;

typedef struct {
  int which;
  unsigned long newvalue;
  char newresource[8];
  long strref;
  long param1, param2; //to display parameters for effects
  short projectile, projectile2;
  short itemtype, itemtype2;
  short feature, feature2;
  char resource[8];
  char itemname[8];
  char variable[32];
  unsigned char ea, ea2;
  unsigned char general, general2;
  unsigned char race, race2;
  unsigned char class1, class2;
  unsigned char specific, specific2;
  unsigned char gender, gender2;
  unsigned char align, align2;
} search_data;

typedef struct {
  CString opcodetext;
  CString opcodedesc;
  CString par1;
  CString par2;
} opcode_struct;

typedef struct
{
  int bytes[14]; //iwd2 12, others 6
  int area[4];   //only in pst, iwd
  char var[8+32+1];
  int bytes2[2]; //only in iwd2
} object;

class action
{
public:
  int opcode;
  object obj[3];
  int bytes[5];
  char var1[8+32+1];
  char var2[8+32+1];
  void Reset()
  {
    int i;

    opcode=0;
    memset(bytes,0,sizeof(bytes));
    memset(var1,0,sizeof(var1));
    memset(var2,0,sizeof(var2));
    for(i=0;i<3;i++) memset(obj+i,0,sizeof(object) );
  }
  action()
  {
    Reset();
  }
};

class response
{
public:
  int weight;
  int actioncount;
  action *actions;
  void Reset()
  {
    int i;

    weight=0;
    for(i=0;i<actioncount;i++) actions[i].Reset();
  }
  response()
  {
    actioncount=0;
    actions=NULL;
    weight=0;
  }
};

class trigger
{
public:
  int opcode;
  bool negate;
  int bytes[6]; //PST triggers got 2 extra bytes (points)
  char var1[6+32+1];
  char var2[6+32+1];
  object trobj;
  void Reset()
  {
    opcode=0;
    negate=false;
    memset(bytes,0,sizeof(bytes));
    memset(var1,0,sizeof(var1));
    memset(var2,0,sizeof(var2));
    memset(&trobj,0,sizeof(object));
  }
  trigger()
  {
    Reset();
  }
};

class block
{
public:
  int triggercount;
  trigger *triggers;
  int responsecount;
  response *responses;
  void Reset()
  {
    int i;

    for(i=0;i<triggercount;i++) triggers[i].Reset();
    for(i=0;i<responsecount;i++) responses[i].Reset();
  }
  block()
  {
    triggercount=0;
    responsecount=0;
    triggers=NULL;
    responses=NULL;
  }
};

typedef struct
{
  short type;
  CString idsfile;
} parameter_data;

class compiler_data//hashed compiler data
{
public:
  CString keyword;
  int opcode;
  int parnum;
  int parnumx; //prototype code (parameter count, but strings add 16)
  parameter_data *parameters;
public:
  compiler_data()
  {
    parameters=NULL;
    parnum=0;
  }
};

typedef struct
{
  int data[3];
} tooltip_data;

typedef struct
{
  CString a,b;
} CString2;
typedef CList<CString2, CString2&> CString2List;
typedef CMap<CString, LPCSTR, int, int&> CStringMapInt;
typedef CMap<int, int, int, int&> CIntMapInt;
typedef CMap<int, int, CString, LPCSTR> CIntMapString;
typedef CMap<CString, LPCSTR, tooltip_data, tooltip_data&> CStringMapTooltip;
typedef CMap<CString, LPCSTR, compiler_data, compiler_data&> CStringMapToCompiler;
typedef CTypedPtrMap<CMapStringToOb, CString, CStringMapInt*> CStringMapToCStringMapInt;
typedef CMap<CString, LPCSTR, CString, LPCSTR> CStringMapString;
typedef CTypedPtrMap<CMapStringToOb, CString, CStringMapString*> CStringMapToCStringMapString;
typedef COLORREF palettetype[256];

typedef struct 
{
  unsigned long checkopt;
  unsigned long editopt;
  unsigned long gameopt;
  CString bgfolder;
  CString descpath;
} gameprops;

typedef CMap<CString, LPCSTR, gameprops, gameprops&> CStringMapGameProps;

#endif AFX_STRUCTS_H__42E3FDEA_0FCA_4733_8AD1_6E396250B019__INCLUDED_
