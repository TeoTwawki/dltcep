// ItemProperties.cpp : implementation file
//

#include "stdafx.h"

#include "chitem.h"
#include "2da.h"
#include "ItemProperties.h"
#include "EffEdit.h"
#include "options.h"
#include "compat.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CItemGeneral, CPropertyPage)
IMPLEMENT_DYNCREATE(CItemIcons, CPropertyPage)
IMPLEMENT_DYNCREATE(CItemDescription, CPropertyPage)
IMPLEMENT_DYNCREATE(CItemUsability, CPropertyPage)
IMPLEMENT_DYNCREATE(CItemEquip, CPropertyPage)
IMPLEMENT_DYNCREATE(CItemExtended, CPropertyPage)
IMPLEMENT_DYNCREATE(CItemExtra, CPropertyPage)

//these variables are accessible from CItemEquip and CItemExtended
static feat_block  featcopy;
static ext_header  headcopy;
static feat_block *featblks=NULL; //for headcopy
static int featblkcnt=0;          //for headcopy

static CString ClassUsabilityBits20[64]=
{
  "Barbarian","Bard","Cleric","Druid","Fighter","Monk","Paladin","Ranger",
    "Rogue","Sorcerer","Wizard","","Chaotic","Evil","Good","... Neutral",
    "Lawful","Neutral ...","","","","","","Elf",
    "Dwarf","Half-Elf","Halfling","Human","Gnome","Half-Orc","","",
    "Kit 0x01000000","Kit 0x02000000","Kit 0x04000000","Kit 0x08000000",
    "Kit 0x10000000","Kit 0x20000000","Kit 0x40000000","Kit 0x80000000",
    "C. Lathander","C. Selune","C. Helm","C. Oghma","C. Tempus","C. Bane","C. Mask","C. Talos",
    "Diviner","Enchanter","Evoker","Illusionist","Necromancer","Transmuter","Baseclass","C. Ilmater",
    "P. Ilmater","P. Helm","P. Mystra","Old Order","Broken Ones","Dark Moon","Abjurer","Conjurer"
};

static CString ClassUsabilityBits11[26]=
{
 "Sensates", "Priest", "Godsmen", "Cleric/Thief", "Xaositects",
 "Fighter", "No faction", "Fighter/Mage", "Dustmen",
 "Mercykillers", "Indeps", "Fighter/Thief", "Mage",
 "Mage/Thief", "Dak'kon", "Fall-From-Grace", "Thief",
 "Vhailor", "Ingus", "Morte", "Nordom",
 "22", "Annah", "24", "Nameless One", "26"
};

static CString ClassUsabilityBits[58]=
{
  "Bard","Cleric","Cleric-Mage","Cleric/Thief","Cleric/Ranger",
  "Fighter","Fighter/Druid","Fighter/Mage","Fighter/Cleric",
  "Fighter/Mage/Cleric","Fighter/Mage/Thief","Fighter/Thief","Mage",
  "Mage/Thief","Solamnic Knight","Ranger","Thief",
  "Elf","Dwarf","Half-Elf","Kender",
  "Human","Gnome","Monk","Druid","Minotaur",
  "","","","","","","","",
  "","","","","Handler","","","",
  "Diviner","Enchanter","Evoker","Illusionist","Necromancer","Transmuter","Baseclass","",
  "","","","","","","Abjurer","Conjurer"
};

//gets the starting feature block counter for this extended header
static int GetFBC(int extheader_num)
{
  int fbc;
  int i;

  //equipping feature block count
  fbc=the_item.header.featblkcount;
  //plus extended header feature block counts
  for(i=0;i<extheader_num;i++)
  {
    fbc+=the_item.extheaders[i].fbcount;
  }
  return fbc;
}

static int DeleteFeatures(int start, int count, int newcount) 
{
  int max, current;
  int i;
  feat_block *new_featblocks;

  if(count==newcount) return 0;
  max=GetFBC(the_item.header.extheadcount);
  if(the_item.featblkcount!=max)
  {
    //internal error
    return -2;
  }
  current=max-count+newcount;
  if(current)
  {
    new_featblocks = new feat_block[current];
    if(!new_featblocks)
    {
      return -1; //memory trouble!!!
    }
  }

  for(i=0;i<start;i++)
  {
    new_featblocks[i]=the_item.featblocks[i];
  }
  for(i=start;i<current-newcount;i++)
  {
    new_featblocks[i+newcount]=the_item.featblocks[i+count];
  }
  delete [] the_item.featblocks;
  if(current)
  {
    //new_featblocks gets initialized when current==true 
    //(the warning of uninitialized local is unwarranted)
    the_item.featblocks=new_featblocks;
  }
  else the_item.featblocks=0;
  //this is the cummulative featblockcount
  the_item.featblkcount-=count;
  the_item.featblkcount+=newcount;
  //current is the new featblockcount (was max before cutting)
  if(the_item.featblkcount!=current)
  {
    //internal error
    return -2;
  }
  return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CItemGeneral property page

CItemGeneral::CItemGeneral() : CPropertyPage(CItemGeneral::IDD)
{
	//{{AFX_DATA_INIT(CItemGeneral)
	//}}AFX_DATA_INIT
  hbb=NULL;
}

CItemGeneral::~CItemGeneral()
{
  if(hbb) DeleteObject(hbb);
}

void CItemGeneral::DoDataExchange(CDataExchange* pDX)
{
  CString tmpstr;  

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CItemGeneral)
	DDX_Control(pDX, IDC_PROFICIENCY, m_proficiencypicker_control);
	DDX_Control(pDX, IDC_ANIMATION, m_animationpicker_control);
	DDX_Control(pDX, IDC_ITEMTYPE, m_itemtypepicker_control);
	//}}AFX_DATA_MAP
  tmpstr=format_itemtype(the_item.header.itemtype);
  DDX_Text(pDX, IDC_ITEMTYPE, tmpstr);  
  the_item.header.itemtype=find_itemtype(tmpstr);
  
  tmpstr=format_weaprofs(the_item.header.weaprof);
  DDX_Text(pDX, IDC_PROFICIENCY, tmpstr);
  the_item.header.weaprof=(unsigned char) strtonum(tmpstr);
  
  tmpstr=format_animtype(*(unsigned short *) the_item.header.animtype);
  DDX_Text(pDX, IDC_ANIMATION, tmpstr);
  StoreAnim(tmpstr,the_item.header.animtype);
  
	DDX_Text(pDX, IDC_PRICE, the_item.header.price);
	DDV_MinMaxLong(pDX, the_item.header.price, 0, 1000000);
	DDX_Text(pDX, IDC_LORE, the_item.header.loreid);
	DDV_MinMaxInt(pDX, the_item.header.loreid, 0, 255);
	DDX_Text(pDX, IDC_WEIGHT, the_item.header.weight);
	DDV_MinMaxLong(pDX, the_item.header.weight, 0, 1000000);
	DDX_Text(pDX, IDC_MAGICLEVEL, the_item.header.mplus);
	DDV_MinMaxLong(pDX, the_item.header.mplus, 0, 255);
	DDX_Text(pDX, IDC_MINCHA, the_item.header.mincha);
	DDV_MinMaxInt(pDX, the_item.header.mincha, 0, 255);
	DDX_Text(pDX, IDC_MINCON, the_item.header.mincon);
	DDV_MinMaxInt(pDX, the_item.header.mincon, 0, 255);
	DDX_Text(pDX, IDC_MINDEX, the_item.header.mindex);
	DDV_MinMaxInt(pDX, the_item.header.mindex, 0, 255);
	DDX_Text(pDX, IDC_MININT, the_item.header.minint);
	DDV_MinMaxInt(pDX, the_item.header.minint, 0, 255);
	DDX_Text(pDX, IDC_MINSTR, the_item.header.minstr);
	DDV_MinMaxInt(pDX, the_item.header.minstr, 0, 255);
	DDX_Text(pDX, IDC_MINSTRBONUS, the_item.header.minstrpl);
	DDV_MinMaxInt(pDX, the_item.header.minstrpl, 0, 255);
	DDX_Text(pDX, IDC_MINWIS, the_item.header.minwis);
	DDV_MinMaxInt(pDX, the_item.header.minwis, 0, 255);
	DDX_Text(pDX, IDC_MINLEVEL, the_item.header.minlevel);
	DDV_MinMaxInt(pDX, the_item.header.minlevel, 0, 255);
	DDX_Text(pDX, IDC_X25, the_item.header.unknown1);
	DDV_MinMaxInt(pDX, the_item.header.unknown1, 0, 255);
	DDX_Text(pDX, IDC_X27, the_item.header.unknown2);
	DDV_MinMaxInt(pDX, the_item.header.unknown2, 0, 255);
	DDX_Text(pDX, IDC_X33, the_item.header.unknown3);
	DDV_MinMaxInt(pDX, the_item.header.unknown3, 0, 255);
	DDX_Text(pDX, IDC_STACK, the_item.header.maxstack);

  tmpstr=colortitle(the_item.pstheader.colour);
  DDX_Text(pDX, IDC_COLOR, tmpstr);
  the_item.pstheader.colour=(short) strtonum(tmpstr);
  DDX_Control(pDX, IDC_ITMCOLOR, m_itmcolor);
}

BOOL CItemGeneral::OnInitDialog() 
{
  int x;
  CString tmpstr;

  RefreshGeneral();
	CPropertyPage::OnInitDialog();	
  m_itemtypepicker_control.ResetContent();
  for(x=0;x<NUM_ITEMTYPE+1;x++)
  {
    m_itemtypepicker_control.AddString(itemtypes[x]);
  }

  m_proficiencypicker_control.ResetContent();
  for(x=0;x<act_num_weaprof+1;x++)
  {
    m_proficiencypicker_control.AddString(format_weaprofs(weaprofidx[x]));
  }

  m_animationpicker_control.ResetContent();
  for(x=0;x<NUM_ANIMIDX;x++)
  {
    tmpstr.Format("%2.2s-%s",&animidx[x],itemanimations[x]);
    m_animationpicker_control.AddString(tmpstr);
  }

  UpdateData(UD_DISPLAY);
	return TRUE;
}

static int radioids[]={IDC_V1,IDC_V11,IDC_V20,0};

//move itemdata to local variables
void CItemGeneral::RefreshGeneral()
{
  int i;
  int id;
  CButton *cb;
  CWnd *cw;

  switch(the_item.revision)
  {
  case REV_BG:
    id=0;
    break;
  case REV_PST:
    id=1;
    break;
  case REV_IWD2:
    id=2;
    break;
  }
  for(i=0;radioids[i];i++)
  {
    cb=(CButton *) GetDlgItem(radioids[i]);
    if(i==id) cb->SetCheck(true);
    else cb->SetCheck(false);
  }
  id=(the_item.revision==REV_PST);
  cw=GetDlgItem(IDC_COLOR);
  if(cw) cw->EnableWindow(id);
  cw=GetDlgItem(IDC_ITMCOLOR);
  if(cw) cw->EnableWindow(id);
  MakeGradientBitmap(hbb, the_item.pstheader.colour);
  if(m_itmcolor) m_itmcolor.SetBitmap(hbb);
}

BEGIN_MESSAGE_MAP(CItemGeneral, CPropertyPage)
	//{{AFX_MSG_MAP(CItemGeneral)
	ON_CBN_KILLFOCUS(IDC_ITEMTYPE, OnDefaultKillfocus)
	ON_BN_CLICKED(IDC_V1, OnV1)
	ON_BN_CLICKED(IDC_V11, OnV11)
	ON_BN_CLICKED(IDC_V20, OnV20)
	ON_BN_CLICKED(IDC_ITMCOLOR, OnItmcolor)
	ON_CBN_KILLFOCUS(IDC_PROFICIENCY, OnDefaultKillfocus)
	ON_CBN_KILLFOCUS(IDC_ANIMATION, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_LORE, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_X33, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_X27, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_X25, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_MAGICLEVEL, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_MINCHA, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_MINCON, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_MINDEX, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_MININT, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_MINLEVEL, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_MINSTR, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_MINSTRBONUS, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_MINWIS, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_PRICE, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_STACK, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_WEIGHT, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_COLOR, OnKillfocusColor)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CItemGeneral::OnDefaultKillfocus() 
{
	UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CItemGeneral::OnKillfocusColor() 
{
	UpdateData(UD_RETRIEVE);
  RefreshGeneral();
  UpdateData(UD_DISPLAY);
}

void CItemGeneral::OnItmcolor() 
{
  colordlg.m_picked=the_item.pstheader.colour;
  if(colordlg.DoModal()==IDOK)
  {
    the_item.pstheader.colour=(unsigned char) colordlg.m_picked;
    MakeGradientBitmap(hbb, colordlg.m_picked);
    m_itmcolor.SetBitmap(hbb);
    UpdateData(UD_DISPLAY);
  }
}

void CItemGeneral::OnV1() 
{
  the_item.revision=REV_BG;
  RefreshGeneral();
}

void CItemGeneral::OnV11() 
{
  the_item.revision=REV_PST;
  RefreshGeneral();
}

void CItemGeneral::OnV20() 
{
  the_item.revision=REV_IWD2;
  RefreshGeneral();
}

/////////////////////////////////////////////////////////////////////////////
// CItemIcons property page

CItemIcons::CItemIcons() : CPropertyPage(CItemIcons::IDD)
{
	//{{AFX_DATA_INIT(CItemIcons)
	m_convref = 0;
	m_conv = _T("");
	//}}AFX_DATA_INIT
  topleft=CPoint(220,220);
  hbmi1=NULL;
  hbmi2=NULL;
  hbmg=NULL;
  memset(hbmd,0,sizeof(hbmd));
}

CItemIcons::~CItemIcons()
{
  int i;

  if(hbmi1) DeleteObject(hbmi1);
  if(hbmi2) DeleteObject(hbmi2);
  if(hbmg) DeleteObject(hbmg);
  for(i=0;i<4;i++)
  {
    if(hbmd[i]) DeleteObject(hbmd[i]);
  }
}

static int iconboxids[16]={
  IDC_CRITICAL, IDC_TWOHAND, IDC_MOVABLE, IDC_DISPLAYABLE,
  IDC_CURSED, IDC_COPYABLE, IDC_MAGICAL, IDC_BOW,
  IDC_SILVER, IDC_COLD, IDC_STOLEN, IDC_CONVERSIBLE,
  IDC_FLAG1, IDC_FLAG2, IDC_FLAG3, IDC_FLAG4
};

void CItemIcons::DoDataExchange(CDataExchange* pDX)
{
  CString tmpstr;
  int id, bit;
  CButton *checkbox;
  CPoint newtopleft;
  int fc;
  BOOL itemexcl;

  RefreshIcons(); //doh
  CPropertyPage::DoDataExchange(pDX);
  for(fc=0;fc<4;fc++)
  {
    DDX_Control(pDX, IDC_DESCICON1+fc, m_descicon[fc]);
  }

	//{{AFX_DATA_MAP(CItemIcons)
	DDX_Control(pDX, IDC_GRNDICON, m_grnicon);
	DDX_Control(pDX, IDC_INVICON2, m_invicon2);
	DDX_Control(pDX, IDC_INVICON1, m_invicon1);
	DDX_Text(pDX, IDC_CONVREF, m_convref);
	DDV_MinMaxLong(pDX, m_convref, -1, 1000000);
	DDX_Text(pDX, IDC_CONV, m_conv);
	//}}AFX_DATA_MAP
  tmpstr.Format("0x%0x",the_item.header.splattr);
	DDX_Text(pDX, IDC_FLAGS, tmpstr);
  the_item.header.splattr=(unsigned short) strtonum(tmpstr);
  if(pDX->m_bSaveAndValidate==UD_RETRIEVE)
  {
  	DDX_Text(pDX, IDC_ITEMINVICON, tmpstr);
    StoreResref(tmpstr, the_item.header.invicon);
  	DDX_Text(pDX, IDC_ITEMGRNDICON, tmpstr);
    StoreResref(tmpstr, the_item.header.grnicon);
  	DDX_Text(pDX, IDC_ITEMDESCICON, tmpstr);
    StoreResref(tmpstr, the_item.header.descicon);
  	DDX_Text(pDX, IDC_USEDUP, tmpstr);
    StoreResref(tmpstr, the_item.header.destname);
  	DDX_Text(pDX, IDC_DIALOG, tmpstr);
    StoreResref(tmpstr, the_item.pstheader.dialog);
  }
  else //display
  {
    itemexcl=!!exclude_item.Find(itemname);
  	DDX_Check(pDX, IDC_ITEMEXCL, itemexcl);

    RetrieveResref(tmpstr, the_item.header.destname);
  	DDX_Text(pDX, IDC_USEDUP, tmpstr);
  	DDV_MaxChars(pDX, tmpstr, 8);

    RetrieveResref(tmpstr, the_item.pstheader.dialog);
  	DDX_Text(pDX, IDC_DIALOG, tmpstr);
  	DDV_MaxChars(pDX, tmpstr, 8);

    if(m_convref>0) //conversible bit must be set
    {
      the_item.header.itmattr|=ATTR_CONVERS;
    }
    else
    {
      the_item.header.itmattr&=~ATTR_CONVERS;
    }
    /// displaying icons
    RetrieveResref(tmpstr, the_item.header.invicon);
  	DDX_Text(pDX, IDC_ITEMINVICON, tmpstr);
  	DDV_MaxChars(pDX, tmpstr, 8);  //ddv must be in display!
    if(bmi!=tmpstr)
    {
      bmi=tmpstr;
      if(!read_bam(tmpstr))
      {
        fc=the_bam.GetFrameIndex(1,0);
        the_bam.MakeBitmap(fc,RGB(32,32,32),hbmi1,BM_RESIZE,32,32);
        m_invicon1.SetBitmap(hbmi1);
        fc=the_bam.GetFrameIndex(0,0);
        the_bam.MakeBitmap(fc,RGB(32,32,32),hbmi2,BM_RESIZE,32,32);
        m_invicon2.SetBitmap(hbmi2);
      }
      else
      {
        m_invicon1.SetBitmap(0);
        m_invicon2.SetBitmap(0);
      }
    }

    RetrieveResref(tmpstr, the_item.header.grnicon);
  	DDX_Text(pDX, IDC_ITEMGRNDICON, tmpstr);
  	DDV_MaxChars(pDX, tmpstr, 8);
    if(bmg!=tmpstr)
    {
      bmg=tmpstr;
      if(!read_bam(tmpstr))
      {
        fc=the_bam.GetFrameIndex(0,0);
        the_bam.MakeBitmap(fc,RGB(32,32,32),hbmg,BM_RESIZE,32,32);
        m_grnicon.SetBitmap(hbmg);
      }
      else
      {
        m_grnicon.SetBitmap(0);
      }
    }
    
    RetrieveResref(tmpstr, the_item.header.descicon);
    DDX_Text(pDX, IDC_ITEMDESCICON, tmpstr);
    DDV_MaxChars(pDX, tmpstr, 8);
    if(the_item.revision!=REV_PST)
    {
      if(bmd!=tmpstr)
      {
        bmd=tmpstr;
        if(!read_bam(tmpstr))
        {
          fc=4;        
          while(fc--)
          {
            newtopleft=topleft-the_bam.GetFramePos(fc);
            m_descicon[fc].MoveWindow(CRect(newtopleft,the_bam.GetFrameSize(fc)));
            the_bam.MakeBitmap(fc,RGB(240,224,160),hbmd[fc],BM_RESIZE,1,1);
            m_descicon[fc].SetBitmap(hbmd[fc]);
          }
        }
        else
        {
          for(fc=0;fc<4;fc++) m_descicon[fc].SetBitmap(0);
        }
      }
    }
    else
    {
      for(fc=0;fc<4;fc++) m_descicon[fc].SetBitmap(0);
      bmd="";
    }
    ///
    bit=1;
    for(id=0;id<16;id++)
    {
      checkbox=(class CButton *) GetDlgItem(iconboxids[id]);
      checkbox->SetCheck(!!(the_item.header.itmattr&bit));
      bit=bit<<1;    
    }
  }
}

BOOL CItemIcons::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	RefreshIcons();
  UpdateData(UD_DISPLAY);
	return TRUE;
}

void CItemIcons::RefreshIcons()
{
  CWnd *cb;

  if(the_item.revision==REV_PST) m_convref=the_item.pstheader.nameref;
  else
  {
    m_convref=-1;
    dial_references.Lookup(itemname, m_convref); //sets convref
    StoreResref(itemname, the_item.pstheader.dialog);
  }
  m_conv=resolve_tlk_text(m_convref);
  if(!m_hWnd) return;
  switch(the_item.revision)
  {
  case REV_PST:
    cb=GetDlgItem(IDC_DESCICON);
    if(cb) cb->SetWindowText("Take sound");
    cb=GetDlgItem(IDC_DCENTER);
    if(cb) cb->SetWindowText("Play sound");
    cb=GetDlgItem(IDC_DESTRUCTED);
    if(cb) cb->SetWindowText("Drop sound");
    cb=GetDlgItem(IDC_PLAYSOUND);
    if(cb) cb->ShowWindow(true);
    cb=GetDlgItem(IDC_ITEMEXCL);
    if(cb) cb->ShowWindow(false);
    cb=GetDlgItem(IDC_DIALOG);
    if(cb) cb->EnableWindow(true);
    cb=GetDlgItem(IDC_BROWSE);
    if(cb) cb->EnableWindow(true);
    break;
  default:
    cb=GetDlgItem(IDC_DESCICON);
    if(cb) cb->SetWindowText("Description");
    cb=GetDlgItem(IDC_DCENTER);
    if(cb) cb->SetWindowText("Center BAM");
    cb=GetDlgItem(IDC_DESTRUCTED);
    if(cb) cb->SetWindowText("Destructed item");
    cb=GetDlgItem(IDC_PLAYSOUND);
    if(cb) cb->ShowWindow(false);
    cb=GetDlgItem(IDC_ITEMEXCL);
    if(cb) cb->ShowWindow(true);
    cb=GetDlgItem(IDC_DIALOG);
    if(cb) cb->EnableWindow(false);
    cb=GetDlgItem(IDC_BROWSE);
    if(cb) cb->EnableWindow(false);
    break;
  }
}

BEGIN_MESSAGE_MAP(CItemIcons, CPropertyPage)
	//{{AFX_MSG_MAP(CItemIcons)
	ON_BN_CLICKED(IDC_BOW, OnBow)
	ON_BN_CLICKED(IDC_COLD, OnCold)
	ON_BN_CLICKED(IDC_CONVERSIBLE, OnConversible)
	ON_BN_CLICKED(IDC_COPYABLE, OnCopyable)
	ON_BN_CLICKED(IDC_CRITICAL, OnCritical)
	ON_BN_CLICKED(IDC_CURSED, OnCursed)
	ON_BN_CLICKED(IDC_DISPLAYABLE, OnDisplayable)
	ON_BN_CLICKED(IDC_FLAG1, OnFlag1)
	ON_BN_CLICKED(IDC_FLAG2, OnFlag2)
	ON_BN_CLICKED(IDC_FLAG3, OnFlag3)
	ON_BN_CLICKED(IDC_FLAG4, OnFlag4)
	ON_BN_CLICKED(IDC_MAGICAL, OnMagical)
	ON_BN_CLICKED(IDC_MOVABLE, OnMovable)
	ON_BN_CLICKED(IDC_SILVER, OnSilver)
	ON_BN_CLICKED(IDC_STOLEN, OnStolen)
	ON_BN_CLICKED(IDC_TWOHAND, OnTwohand)
	ON_EN_KILLFOCUS(IDC_CONVREF, OnKillfocusConvref)
	ON_EN_KILLFOCUS(IDC_USEDUP, OnKillfocusUsedup)
	ON_EN_KILLFOCUS(IDC_ITEMINVICON, OnKillfocusIteminvicon)
	ON_EN_KILLFOCUS(IDC_ITEMGRNDICON, OnKillfocusItemgrndicon)
	ON_EN_KILLFOCUS(IDC_ITEMDESCICON, OnKillfocusItemdescicon)
	ON_EN_KILLFOCUS(IDC_FLAGS, OnKillfocusFlags)
	ON_BN_CLICKED(IDC_ICENTER, OnIcenter)
	ON_BN_CLICKED(IDC_GCENTER, OnGcenter)
	ON_BN_CLICKED(IDC_DCENTER, OnDcenter)
	ON_BN_CLICKED(IDC_ITEMEXCL, OnItemexcl)
	ON_BN_CLICKED(IDC_PLAYSOUND, OnPlaysound)
	ON_BN_CLICKED(IDC_INVICON1, OnInvicon)
	ON_BN_CLICKED(IDC_GRNDICON, OnGrndicon)
	ON_BN_CLICKED(IDC_DESCICON1, OnDescicon)
	ON_EN_KILLFOCUS(IDC_DIALOG, OnKillfocusDialog)
	ON_BN_CLICKED(IDC_BROWSE1, OnBrowse1)
	ON_BN_CLICKED(IDC_BROWSE2, OnBrowse2)
	ON_BN_CLICKED(IDC_BROWSE3, OnBrowse3)
	ON_BN_CLICKED(IDC_BROWSE4, OnBrowse4)
	ON_BN_CLICKED(IDC_INVICON2, OnInvicon)
	ON_BN_CLICKED(IDC_DESCICON2, OnDescicon)
	ON_BN_CLICKED(IDC_DESCICON3, OnDescicon)
	ON_BN_CLICKED(IDC_DESCICON4, OnDescicon)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CItemIcons::OnKillfocusIteminvicon() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CItemIcons::OnKillfocusItemgrndicon() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CItemIcons::OnKillfocusItemdescicon() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CItemIcons::OnKillfocusFlags() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CItemIcons::OnBow() 
{
	Togglebit(ATTR_BOW);	
}

void CItemIcons::OnCold() 
{
	Togglebit(ATTR_COLD);	
}

void CItemIcons::OnConversible() 
{
	Togglebit(ATTR_CONVERS);
  if((the_item.header.itmattr&ATTR_CONVERS))
  { //conversible
    if(m_convref<0) m_convref=0;
  }
  else
  {
    if(m_convref>0) m_convref=0;
  }
  the_item.pstheader.nameref=m_convref;
  if(the_item.revision!=REV_PST)
  {
    if(!(editflg&DACHANGE) || (MessageBox("Do you want to update ITEMDIAL.2da?","Item editor",MB_ICONQUESTION|MB_YESNO)==IDYES) )
    {
      dial_references.SetAt(itemname,m_convref);
      Write2daMapToFile("ITEMDIAL",dial_references,"LABEL  FILE");
    }
  }
  UpdateData(UD_DISPLAY);
}

void CItemIcons::OnCopyable() 
{
	Togglebit(ATTR_COPYABLE);	
}

void CItemIcons::OnCritical() 
{
	Togglebit(ATTR_CRITICAL);	
}

void CItemIcons::OnCursed() 
{
	Togglebit(ATTR_CURSED);	
}

void CItemIcons::OnDisplayable() 
{
	Togglebit(ATTR_DISPLAY);	
}

void CItemIcons::OnFlag1() 
{
	Togglebit(0x1000);	
}

void CItemIcons::OnFlag2() 
{
	Togglebit(0x2000);	
}

void CItemIcons::OnFlag3() 
{
	Togglebit(0x4000);	
}

void CItemIcons::OnFlag4() 
{
	Togglebit(0x8000);	
}

void CItemIcons::OnMagical() 
{
  CString tmpstr;

	Togglebit(ATTR_MAGICAL);	
  if((the_item.header.itmattr&ATTR_MAGICAL))
  { //nondesctructable
    tmpstr.Empty();
    StoreResref(tmpstr,the_item.header.destname);
    UpdateData(UD_DISPLAY);
  }
}

void CItemIcons::OnMovable() 
{
	Togglebit(ATTR_MOVABLE);	
}

void CItemIcons::OnSilver() 
{
	Togglebit(ATTR_SILVER);	
}

void CItemIcons::OnStolen() 
{
	Togglebit(ATTR_STOLEN);	
}

void CItemIcons::OnTwohand() 
{
	Togglebit(ATTR_TWOHAND);	
}

void CItemIcons::OnKillfocusConvref() 
{
  UpdateData(UD_RETRIEVE);
  if(m_convref<0) the_item.header.itmattr&=~ATTR_CONVERS;
  else if(m_convref>0) the_item.header.itmattr|=ATTR_CONVERS;
  m_conv=resolve_tlk_text(m_convref);
  if(the_item.revision==REV_PST)
  {
    the_item.pstheader.nameref=m_convref;
  }
  else
  {
    //set itemdial && save it
    if(!(editflg&DACHANGE) || (MessageBox("Do you want to update ITEMDIAL.2da?","Item editor",MB_ICONQUESTION|MB_YESNO)==IDYES) )
    {
      dial_references.SetAt(itemname,m_convref);
      Write2daMapToFile("ITEMDIAL",dial_references,"LABEL  FILE");
    }
  }
  UpdateData(UD_DISPLAY);
}

void CItemIcons::OnKillfocusUsedup() 
{
  UpdateData(UD_RETRIEVE);
	if(the_item.header.destname[0])
  {
    the_item.header.itmattr&=~ATTR_MAGICAL;
  }
  UpdateData(UD_DISPLAY);
}

void CItemIcons::OnBrowse1() 
{
  pickerdlg.m_restype=REF_BAM;
  RetrieveResref(pickerdlg.m_picked,the_item.header.invicon);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_item.header.invicon);
  }
	UpdateData(UD_DISPLAY);	
}

void CItemIcons::OnIcenter() 
{
  CString tmpstr;
  CPoint center;
  int frame;

  UpdateData(UD_RETRIEVE);
  bmi="";
	if(!the_item.header.invicon[0]) return;
  RetrieveResref(tmpstr, the_item.header.invicon);
  if(read_bam(tmpstr))
  {
    MessageBox("Cannot read bam.","Item editor",MB_OK);
    return;
  }
  if(the_bam.GetCycleCount()!=2)
  {
    MessageBox("Inventory bams should have two cycles!","Item editor",MB_OK);
    return;
  }
// inventory bam
  frame=the_bam.GetFrameIndex(1,0);
  if(frame<0)
  {
    MessageBox("Bam is corrupted.","Item editor",MB_OK);
    return;
  }
  center=the_bam.GetFrameSize(frame);
  center.x=(32-center.x)/-2;
  center.y=(32-center.y)/-2;
  if((center.x>0) || (center.y>0))
  {
    MessageBox("The frame is bigger than an inventory icon","Item editor",MB_OK);
    return;
  }
  the_bam.SetFramePos(frame,center.x,center.y);
// transfering bam
  frame=the_bam.GetFrameIndex(0,0);
  if(frame<0)
  {
    MessageBox("Bam is corrupted.","Item editor",MB_OK);
    return;
  }
  center=the_bam.GetFrameSize(frame);
  center.x=center.x/2;
  center.y=center.y/2;
  the_bam.SetFramePos(frame,center.x,center.y);
  write_bam(tmpstr,"",&the_bam);
  UpdateData(UD_DISPLAY);
}

void CItemIcons::OnBrowse2() 
{
  pickerdlg.m_restype=REF_BAM;
  RetrieveResref(pickerdlg.m_picked,the_item.header.grnicon);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_item.header.grnicon);
  }
	UpdateData(UD_DISPLAY);	
}

void CItemIcons::OnGcenter() 
{
  CString tmpstr;
  CPoint center;
  int frame;

  UpdateData(UD_RETRIEVE);
  bmg="";
	if(!the_item.header.grnicon[0]) return;
  RetrieveResref(tmpstr, the_item.header.grnicon);
  if(read_bam(tmpstr))
  {
    MessageBox("Cannot read bam.","Item editor",MB_OK);
    return;
  }
  frame=the_bam.GetFrameIndex(0,0);
  if(frame<0)
  {
    MessageBox("Bam is corrupted.","Item editor",MB_OK);
    return;
  }
  center=the_bam.GetFrameSize(frame);
  center.x=center.x/2;
  center.y=center.y/2;
  the_bam.SetFramePos(frame,center.x,center.y);
  write_bam(tmpstr,"",&the_bam);
  UpdateData(UD_DISPLAY);
}

void CItemIcons::OnPlaysound() 
{
  CString tmpstr;

  UpdateData(UD_RETRIEVE);
	if(!the_item.header.destname[0]) return;
  RetrieveResref(tmpstr, the_item.header.destname);
  play_acm(tmpstr,false,false);
}

void CItemIcons::OnBrowse3() 
{
  if(the_item.revision==REV_PST) pickerdlg.m_restype=REF_WAV;
  else pickerdlg.m_restype=REF_BAM;
  RetrieveResref(pickerdlg.m_picked,the_item.header.descicon);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_item.header.descicon);
  }
	UpdateData(UD_DISPLAY);	
}

void CItemIcons::OnBrowse4() 
{
  if(the_item.revision==REV_PST) pickerdlg.m_restype=REF_WAV;
  else pickerdlg.m_restype=REF_ITM;
  RetrieveResref(pickerdlg.m_picked,the_item.header.destname);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_item.header.destname);
  }
	UpdateData(UD_DISPLAY);	
}

void CItemIcons::OnDcenter() 
{
  CString tmpstr;
  CPoint center;
  int frames;

  UpdateData(UD_RETRIEVE);
  bmd="";
	if(!the_item.header.descicon[0]) return;
  RetrieveResref(tmpstr, the_item.header.descicon);
  //this is a pst file, no description icon, but a wav file is here
  if(the_item.revision==REV_PST) 
  {
    play_acm(tmpstr,false,false);
    return;
  }
  if(read_bam(tmpstr))
  {
    MessageBox("Cannot read bam.","Item editor",MB_OK);
    return;
  }
  frames=the_bam.GetFrameCount();
  switch(frames)
  {
  case 1:
    center=the_bam.GetFrameSize(0);
    the_bam.SetFramePos(0,center.x/2,center.y/2);
    write_bam(tmpstr,"",&the_bam);
    break;
  case 4:
    center=the_bam.GetFrameSize(0);
    the_bam.SetFramePos(0,center.x,center.y);
    center=the_bam.GetFrameSize(1);
    the_bam.SetFramePos(1,0,center.y);
    center=the_bam.GetFrameSize(2);
    the_bam.SetFramePos(2,center.x,0);
    the_bam.SetFramePos(3,0,0);
    write_bam(tmpstr,"",&the_bam);
    break;
  default:
    MessageBox("I can only center one or four pieces.","Item editor",MB_OK);
    break;
  }
  UpdateData(UD_DISPLAY);
}

void CItemIcons::OnItemexcl() 
{
  POSITION pos;

  pos=exclude_item.Find(itemname);
  if(pos) exclude_item.RemoveAt(pos);
  else exclude_item.AddTail(itemname);
  if(!(editflg&DACHANGE) || (MessageBox("Do you want to update ITEMEXCL.2da?","Item editor",MB_ICONQUESTION|MB_YESNO)==IDYES) )
  {
    Write2daListToFile("ITEMEXCL",exclude_item,"VALUE");
  }
  UpdateData(UD_DISPLAY);
}

void CItemIcons::OnInvicon() 
{
  pickerdlg.m_restype=REF_BAM;
  RetrieveResref(pickerdlg.m_picked,the_item.header.invicon);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_item.header.invicon);
  }
	UpdateData(UD_DISPLAY);	
}

void CItemIcons::OnGrndicon() 
{
  pickerdlg.m_restype=REF_BAM;
  RetrieveResref(pickerdlg.m_picked,the_item.header.grnicon);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_item.header.grnicon);
  }
	UpdateData(UD_DISPLAY);	
}

void CItemIcons::OnDescicon() 
{
  pickerdlg.m_restype=REF_BAM;
  RetrieveResref(pickerdlg.m_picked,the_item.header.descicon);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_item.header.descicon);
  }
	UpdateData(UD_DISPLAY);	
}

void CItemIcons::OnKillfocusDialog() 
{
	UpdateData(UD_RETRIEVE);	
	UpdateData(UD_DISPLAY);	
}

void CItemIcons::OnBrowse() 
{
  pickerdlg.m_restype=REF_DLG;
  RetrieveResref(pickerdlg.m_picked,the_item.pstheader.dialog);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_item.pstheader.dialog);
  }
	UpdateData(UD_DISPLAY);	
}

/////////////////////////////////////////////////////////////////////////////
// CItemDescription property page

CItemDescription::CItemDescription() : CPropertyPage(CItemDescription::IDD)
{
	//{{AFX_DATA_INIT(CItemDescription)
	m_longdesc = _T("");
	m_longname = _T("");
	m_shortdesc = _T("");
	m_shortname = _T("");
	m_longdesctag = FALSE;
	m_longnametag = FALSE;
	m_shortdesctag = FALSE;
	m_shortnametag = FALSE;
	//}}AFX_DATA_INIT
}

CItemDescription::~CItemDescription()
{
}

void CItemDescription::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_SHORTREF, the_item.header.unidref);
	DDV_MinMaxLong(pDX, the_item.header.unidref, -1, 1000000);
	DDX_Text(pDX, IDC_LONGREF, the_item.header.idref);
	DDV_MinMaxLong(pDX, the_item.header.idref, -1, 1000000);

	DDX_Text(pDX, IDC_SHORTDESCREF, the_item.header.uniddesc);
	DDV_MinMaxLong(pDX, the_item.header.uniddesc, -1, 1000000);
	DDX_Text(pDX, IDC_LONGDESCREF, the_item.header.iddesc);
	DDV_MinMaxLong(pDX, the_item.header.iddesc, -1, 1000000);
	//{{AFX_DATA_MAP(CItemDescription)
	DDX_Text(pDX, IDC_LONGDESC, m_longdesc);
	DDX_Text(pDX, IDC_LONGNAME, m_longname);
	DDX_Text(pDX, IDC_SHORTDESC, m_shortdesc);
	DDX_Text(pDX, IDC_SHORTNAME, m_shortname);
	DDX_Check(pDX, IDC_LONGDESCTAG, m_longdesctag);
	DDX_Check(pDX, IDC_LONGNAMETAG, m_longnametag);
	DDX_Check(pDX, IDC_SHORTDESCTAG, m_shortdesctag);
	DDX_Check(pDX, IDC_SHORTNAMETAG, m_shortnametag);
	//}}AFX_DATA_MAP
}

BOOL CItemDescription::OnInitDialog() 
{
  RefreshDescription();
	CPropertyPage::OnInitDialog();
  UpdateData(UD_DISPLAY);
	return TRUE;
}

BEGIN_MESSAGE_MAP(CItemDescription, CPropertyPage)
	//{{AFX_MSG_MAP(CItemDescription)
	ON_EN_KILLFOCUS(IDC_LONGDESCREF, OnKillfocusLongdescref)
	ON_EN_KILLFOCUS(IDC_LONGREF, OnKillfocusLongref)
	ON_EN_KILLFOCUS(IDC_SHORTDESCREF, OnKillfocusShortdescref)
	ON_EN_KILLFOCUS(IDC_SHORTREF, OnKillfocusShortref)
	ON_EN_KILLFOCUS(IDC_SHORTDESC, OnKillfocusShortdesc)
	ON_EN_KILLFOCUS(IDC_LONGDESC, OnKillfocusLongdesc)
	ON_EN_KILLFOCUS(IDC_SHORTNAME, OnKillfocusShortname)
	ON_EN_KILLFOCUS(IDC_LONGNAME, OnKillfocusLongname)
	ON_BN_CLICKED(IDC_SHORTNAMETAG, OnShortnametag)
	ON_BN_CLICKED(IDC_SHORTDESCTAG, OnShortdesctag)
	ON_BN_CLICKED(IDC_LONGNAMETAG, OnLongnametag)
	ON_BN_CLICKED(IDC_LONGDESCTAG, OnLongdesctag)
	ON_BN_CLICKED(IDC_NEW1, OnNew1)
	ON_BN_CLICKED(IDC_NEW2, OnNew2)
	ON_BN_CLICKED(IDC_NEW3, OnNew3)
	ON_BN_CLICKED(IDC_NEW4, OnNew4)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//move global data to local
void CItemDescription::RefreshDescription()
{
  m_longname=resolve_tlk_text(the_item.header.idref);
  m_shortname=resolve_tlk_text(the_item.header.unidref);
  m_longdesc=resolve_tlk_text(the_item.header.iddesc);
  m_shortdesc=resolve_tlk_text(the_item.header.uniddesc);
  m_longnametag=resolve_tlk_tag(the_item.header.idref);
  m_shortnametag=resolve_tlk_tag(the_item.header.unidref);
  m_longdesctag=resolve_tlk_tag(the_item.header.iddesc);
  m_shortdesctag=resolve_tlk_tag(the_item.header.uniddesc);    
}

void CItemDescription::OnKillfocusLongdescref() 
{
	UpdateData(UD_RETRIEVE);
 	RefreshDescription();
	UpdateData(UD_DISPLAY);
}

void CItemDescription::OnKillfocusLongref() 
{
	UpdateData(UD_RETRIEVE);
 	RefreshDescription();
	UpdateData(UD_DISPLAY);
}

void CItemDescription::OnKillfocusShortdescref() 
{
	UpdateData(UD_RETRIEVE);
 	RefreshDescription();
	UpdateData(UD_DISPLAY);
}

void CItemDescription::OnKillfocusShortref() 
{
	UpdateData(UD_RETRIEVE);
 	RefreshDescription();
	UpdateData(UD_DISPLAY);
}

void CItemDescription::OnKillfocusLongdesc() 
{
	CString old;

  UpdateData(UD_RETRIEVE);
  old=resolve_tlk_text(the_item.header.iddesc);
  if(old!=m_longdesc)
  {
    if(editflg&TLKCHANGE)
    {
      if(MessageBox("Do you want to update dialog.tlk?","Item editor",MB_YESNO)!=IDYES)
      {
        return;
      }
    }
    the_item.header.iddesc=store_tlk_text(the_item.header.iddesc, m_longdesc);
  }
 	RefreshDescription();
  UpdateData(UD_DISPLAY);
}

void CItemDescription::OnKillfocusShortdesc() 
{
	CString old;

  UpdateData(UD_RETRIEVE);
  old=resolve_tlk_text(the_item.header.uniddesc);
  if(old!=m_shortdesc)
  {
    if(editflg&TLKCHANGE)
    {
      if(MessageBox("Do you want to update dialog.tlk?","Item editor",MB_YESNO)!=IDYES)
      {
        return;
      }
    }
    the_item.header.uniddesc=store_tlk_text(the_item.header.uniddesc, m_shortdesc);
  }
 	RefreshDescription();
  UpdateData(UD_DISPLAY);
}

void CItemDescription::OnKillfocusShortname() 
{
	CString old;

  UpdateData(UD_RETRIEVE);
  old=resolve_tlk_text(the_item.header.unidref);
  if(old!=m_shortname)
  {
    if(editflg&TLKCHANGE)
    {
      if(MessageBox("Do you want to update dialog.tlk?","Item editor",MB_YESNO)!=IDYES)
      {
        goto end;
      }
    }
    the_item.header.unidref=store_tlk_text(the_item.header.unidref, m_shortname);
  }
end:
 	RefreshDescription();
  UpdateData(UD_DISPLAY);
}

void CItemDescription::OnKillfocusLongname() 
{
	CString old;

  UpdateData(UD_RETRIEVE);
  old=resolve_tlk_text(the_item.header.idref);
  if(old!=m_longname)
  {
    if(editflg&TLKCHANGE)
    {
      if(MessageBox("Do you want to update dialog.tlk?","Item editor",MB_YESNO)!=IDYES)
      {
        goto end;
      }
    }
    the_item.header.idref=store_tlk_text(the_item.header.idref, m_longname);
  }
end:
 	RefreshDescription();
  UpdateData(UD_DISPLAY);
}

void CItemDescription::OnShortnametag() 
{
  if((editflg&TLKCHANGE) )
  {
    if(MessageBox("Do you want to update dialog.tlk?","Item editor",MB_YESNO)!=IDYES)
    {
      goto end;
    }
  }
  toggle_tlk_tag(the_item.header.unidref);
end:
 	RefreshDescription();
  UpdateData(UD_DISPLAY);
}

void CItemDescription::OnShortdesctag() 
{
  if((editflg&TLKCHANGE) )
  {
    if(MessageBox("Do you want to update dialog.tlk?","Item editor",MB_YESNO)!=IDYES)
    {
      goto end;
    }
  }
  toggle_tlk_tag(the_item.header.uniddesc);
end:
 	RefreshDescription();
  UpdateData(UD_DISPLAY);
}

void CItemDescription::OnLongnametag() 
{
  if((editflg&TLKCHANGE) )
  {
    if(MessageBox("Do you want to update dialog.tlk?","Item editor",MB_YESNO)!=IDYES)
    {
      goto end;
    }
  }
  toggle_tlk_tag(the_item.header.idref);
end:
 	RefreshDescription();
  UpdateData(UD_DISPLAY);
}

void CItemDescription::OnLongdesctag() 
{
  if((editflg&TLKCHANGE) )
  {
    if(MessageBox("Do you want to update dialog.tlk?","Item editor",MB_YESNO)!=IDYES)
    {
      goto end;
    }
  }
  toggle_tlk_tag(the_item.header.iddesc);
end:
 	RefreshDescription();
  UpdateData(UD_DISPLAY);
}

void CItemDescription::OnNew1() 
{
	the_item.header.unidref=-1;
 	RefreshDescription();
  UpdateData(UD_DISPLAY);
}

void CItemDescription::OnNew2() 
{
	the_item.header.idref=-1;
 	RefreshDescription();
  UpdateData(UD_DISPLAY);
}

void CItemDescription::OnNew3() 
{
	the_item.header.uniddesc=-1;
 	RefreshDescription();
  UpdateData(UD_DISPLAY);
}

void CItemDescription::OnNew4() 
{
	the_item.header.iddesc=-1;
 	RefreshDescription();
  UpdateData(UD_DISPLAY);
}

/////////////////////////////////////////////////////////////////////////////
// CItemUsability property page

CItemUsability::CItemUsability() : CPropertyPage(CItemUsability::IDD)
{
	//{{AFX_DATA_INIT(CItemUsability)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CItemUsability::~CItemUsability()
{
}

static int useboxids[64]={
  IDC_CHAOTIC,IDC_EVIL,IDC_GOOD,IDC_EGNEUTRAL,IDC_LAWFUL,IDC_LCNEUTRAL,IDC_BARD,IDC_CLERIC,
  IDC_CLMAGE,IDC_CLTHIEF,IDC_CLRANGER,IDC_FIGHTER,IDC_FIDRUID,IDC_FIMAGE,IDC_FICLERIC,IDC_FIMACLERIC,
  IDC_FIMATHIEF,IDC_FITHIEF,IDC_MAGE,IDC_MATHIEF,IDC_PALADIN,IDC_RANGER,IDC_THIEF,IDC_ELF,
  IDC_DWARF,IDC_HALF_ELF,IDC_HALFLING,IDC_HUMAN,IDC_GNOME,IDC_MONK,IDC_DRUID,IDC_HALFORC,
  IDC_TALOS,IDC_HELM,IDC_LATHANDER,IDC_TOTEMIC,IDC_SHAPE,IDC_AVENGER,IDC_BARBARIAN,IDC_WILDMAGE,
  IDC_STALKER,IDC_BEAST,IDC_ASSASSIN,IDC_BOUNTY,IDC_SWASH,IDC_BLADE,IDC_JESTER,IDC_SKALD,
  IDC_DIVINER,IDC_ENCHANTER,IDC_ILLUSIONIST,IDC_INVOKER,IDC_NECROMANCER,IDC_TRANSMUTER,IDC_BASECLASS,IDC_FERLAIN,
  IDC_BERSERKER,IDC_WIZARDSLAYER,IDC_KENSAI,IDC_CAVALIER,IDC_INQUISITOR,IDC_UNDEADHUNTER,IDC_ABJURER,IDC_CONJURER
};

void CItemUsability::DoDataExchange(CDataExchange* pDX)
{
  CString tmpstr;
  CButton *checkbox;
  int id;
  int byte;
  int bit;

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CItemUsability)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
  if(pDX->m_bSaveAndValidate==UD_DISPLAY)
  {
    for(id=0;id<64;id++)
    {
      byte=id>>3;
      bit=1<<(id&7);
    
      checkbox=(CButton *) GetDlgItem(useboxids[id]);
      checkbox->SetCheck(!(bytes[byte]&bit));
      switch(the_item.revision)
      {
      case REV_PST:
        if(id>5)
        {
          if(id<32) checkbox->SetWindowText(ClassUsabilityBits11[id-6]);
          else 
          {
            tmpstr.Format("Unknown (%d)",id);
            checkbox->SetWindowText(tmpstr);
          }
        }
        break;
      case REV_IWD2:
        if(ClassUsabilityBits20[id].GetLength())
        {
          checkbox->SetWindowText(ClassUsabilityBits20[id]);
        }
        else
        {
          tmpstr.Format("Unknown (%d)",id);
          checkbox->SetWindowText(tmpstr);
        }
        break;
      default:
        if(optflg&COM_DLTC)
        {
          if(id>5)
          {
            checkbox->SetWindowText(ClassUsabilityBits[id-6]);
          }
        }
      }
    }
  }
}

BOOL CItemUsability::OnInitDialog() 
{
	RefreshUsability();
	CPropertyPage::OnInitDialog();
  UpdateData(UD_DISPLAY);
	return TRUE;
}

BEGIN_MESSAGE_MAP(CItemUsability, CPropertyPage)
	//{{AFX_MSG_MAP(CItemUsability)
	ON_BN_CLICKED(IDC_ABJURER, OnAbjurer)
	ON_BN_CLICKED(IDC_ASSASSIN, OnAssassin)
	ON_BN_CLICKED(IDC_AVENGER, OnAvenger)
	ON_BN_CLICKED(IDC_BARBARIAN, OnBarbarian)
	ON_BN_CLICKED(IDC_BARD, OnBard)
	ON_BN_CLICKED(IDC_BASECLASS, OnBaseclass)
	ON_BN_CLICKED(IDC_BEAST, OnBeast)
	ON_BN_CLICKED(IDC_BERSERKER, OnBerserker)
	ON_BN_CLICKED(IDC_BLADE, OnBlade)
	ON_BN_CLICKED(IDC_BOUNTY, OnBounty)
	ON_BN_CLICKED(IDC_CAVALIER, OnCavalier)
	ON_BN_CLICKED(IDC_CHAOTIC, OnChaotic)
	ON_BN_CLICKED(IDC_CLERIC, OnCleric)
	ON_BN_CLICKED(IDC_CLMAGE, OnClmage)
	ON_BN_CLICKED(IDC_CLRANGER, OnClranger)
	ON_BN_CLICKED(IDC_CLTHIEF, OnClthief)
	ON_BN_CLICKED(IDC_CONJURER, OnConjurer)
	ON_BN_CLICKED(IDC_DIVINER, OnDiviner)
	ON_BN_CLICKED(IDC_DRUID, OnDruid)
	ON_BN_CLICKED(IDC_DWARF, OnDwarf)
	ON_BN_CLICKED(IDC_EGNEUTRAL, OnEgneutral)
	ON_BN_CLICKED(IDC_ELF, OnElf)
	ON_BN_CLICKED(IDC_ENCHANTER, OnEnchanter)
	ON_BN_CLICKED(IDC_EVIL, OnEvil)
	ON_BN_CLICKED(IDC_FERLAIN, OnFerlain)
	ON_BN_CLICKED(IDC_FICLERIC, OnFicleric)
	ON_BN_CLICKED(IDC_FIDRUID, OnFidruid)
	ON_BN_CLICKED(IDC_FIGHTER, OnFighter)
	ON_BN_CLICKED(IDC_FIMACLERIC, OnFimacleric)
	ON_BN_CLICKED(IDC_FIMAGE, OnFimage)
	ON_BN_CLICKED(IDC_FIMATHIEF, OnFimathief)
	ON_BN_CLICKED(IDC_FITHIEF, OnFithief)
	ON_BN_CLICKED(IDC_GNOME, OnGnome)
	ON_BN_CLICKED(IDC_GOOD, OnGood)
	ON_BN_CLICKED(IDC_HALF_ELF, OnHalfElf)
	ON_BN_CLICKED(IDC_HALFLING, OnHalfling)
	ON_BN_CLICKED(IDC_HALFORC, OnHalforc)
	ON_BN_CLICKED(IDC_HELM, OnHelm)
	ON_BN_CLICKED(IDC_HUMAN, OnHuman)
	ON_BN_CLICKED(IDC_ILLUSIONIST, OnIllusionist)
	ON_BN_CLICKED(IDC_INQUISITOR, OnInquisitor)
	ON_BN_CLICKED(IDC_INVOKER, OnInvoker)
	ON_BN_CLICKED(IDC_JESTER, OnJester)
	ON_BN_CLICKED(IDC_KENSAI, OnKensai)
	ON_BN_CLICKED(IDC_LATHANDER, OnLathander)
	ON_BN_CLICKED(IDC_LAWFUL, OnLawful)
	ON_BN_CLICKED(IDC_LCNEUTRAL, OnLcneutral)
	ON_BN_CLICKED(IDC_MAGE, OnMage)
	ON_BN_CLICKED(IDC_MATHIEF, OnMathief)
	ON_BN_CLICKED(IDC_MONK, OnMonk)
	ON_BN_CLICKED(IDC_NECROMANCER, OnNecromancer)
	ON_BN_CLICKED(IDC_PALADIN, OnPaladin)
	ON_BN_CLICKED(IDC_RANGER, OnRanger)
	ON_BN_CLICKED(IDC_SHAPE, OnShape)
	ON_BN_CLICKED(IDC_SKALD, OnSkald)
	ON_BN_CLICKED(IDC_STALKER, OnStalker)
	ON_BN_CLICKED(IDC_SWASH, OnSwash)
	ON_BN_CLICKED(IDC_TALOS, OnTalos)
	ON_BN_CLICKED(IDC_THIEF, OnThief)
	ON_BN_CLICKED(IDC_TOTEMIC, OnTotemic)
	ON_BN_CLICKED(IDC_TRANSMUTER, OnTransmuter)
	ON_BN_CLICKED(IDC_UNDEADHUNTER, OnUndeadhunter)
	ON_BN_CLICKED(IDC_WILDMAGE, OnWildmage)
	ON_BN_CLICKED(IDC_WIZARDSLAYER, OnWizardslayer)
	ON_BN_CLICKED(IDC_ALLSET, OnAllset)
	ON_BN_CLICKED(IDC_ALLUNSET, OnAllunset)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//move item variables to local variables
void CItemUsability::RefreshUsability()
{
  bytes[0]=the_item.header.usability[0];
  bytes[1]=the_item.header.usability[1];
  bytes[2]=the_item.header.usability[2];
  bytes[3]=the_item.header.usability[3];
  bytes[4]=the_item.header.kits1;
  bytes[5]=the_item.header.kits2;
  bytes[6]=the_item.header.kits3;
  bytes[7]=the_item.header.kits4;
}

void CItemUsability::OnAbjurer() 
{
	Togglebit(7,0x40);	
}

void CItemUsability::OnAssassin() 
{
	Togglebit(5,4);	
}

void CItemUsability::OnAvenger() 
{
	Togglebit(4,0x20);	
}

void CItemUsability::OnBarbarian() 
{
	Togglebit(4,0x40);	
}

void CItemUsability::OnBard() 
{
	Togglebit(0,0x40);		
}

void CItemUsability::OnBaseclass() 
{
	Togglebit(6,0x40);	
}

void CItemUsability::OnBeast() 
{
	Togglebit(5,2);	
}

void CItemUsability::OnBerserker() 
{
	Togglebit(7,1);	
}

void CItemUsability::OnBlade() 
{
	Togglebit(5,0x20);	
}

void CItemUsability::OnBounty() 
{
	Togglebit(5,8);	
}

void CItemUsability::OnCavalier() 
{
	Togglebit(7,8);	
}

void CItemUsability::OnChaotic() 
{
	Togglebit(0,1);	
}

void CItemUsability::OnCleric() 
{
	Togglebit(0,0x80);		
}

void CItemUsability::OnClmage() 
{
	Togglebit(1,1);		
}

void CItemUsability::OnClranger() 
{
	Togglebit(1,4);		
}

void CItemUsability::OnClthief() 
{
	Togglebit(1,2);		
}

void CItemUsability::OnConjurer() 
{
	Togglebit(7,0x80);	
}

void CItemUsability::OnDiviner() 
{
	Togglebit(6,1);		
}

void CItemUsability::OnDruid() 
{
	Togglebit(3,0x40);			
}

void CItemUsability::OnDwarf() 
{
	Togglebit(3,1);			
}

void CItemUsability::OnEgneutral() 
{
	Togglebit(0,8);		
}

void CItemUsability::OnElf() 
{
	Togglebit(2,0x80);		
}

void CItemUsability::OnEnchanter() 
{
	Togglebit(6,2);		
}

void CItemUsability::OnEvil() 
{
	Togglebit(0,2);	
}

void CItemUsability::OnFerlain() 
{
	Togglebit(6,0x80);	
}

void CItemUsability::OnFicleric() 
{
	Togglebit(1,0x40);		
}

void CItemUsability::OnFidruid() 
{
	Togglebit(1,16);		
}

void CItemUsability::OnFighter() 
{
	Togglebit(1,8);		
}

void CItemUsability::OnFimacleric() 
{
	Togglebit(1,0x80);		
}

void CItemUsability::OnFimage() 
{
	Togglebit(1,0x20);		
}

void CItemUsability::OnFimathief() 
{
	Togglebit(2,1);		
}

void CItemUsability::OnFithief() 
{
	Togglebit(2,2);		
}

void CItemUsability::OnGnome() 
{
	Togglebit(3,16);			
}

void CItemUsability::OnGood() 
{
	Togglebit(0,4);		
}

void CItemUsability::OnHalfElf() 
{
	Togglebit(3,2);			
}

void CItemUsability::OnHalfling() 
{
	Togglebit(3,4);			
}

void CItemUsability::OnHalforc() 
{
	Togglebit(3,0x80);			
}

void CItemUsability::OnHelm() 
{
	Togglebit(4,2);	
}

void CItemUsability::OnHuman() 
{
	Togglebit(3,8);			
}

void CItemUsability::OnIllusionist() 
{
	Togglebit(6,4);		
}

void CItemUsability::OnInquisitor() 
{
	Togglebit(7,16);	
}

void CItemUsability::OnInvoker() 
{
	Togglebit(6,8);		
}

void CItemUsability::OnJester() 
{
	Togglebit(5,0x40);	
}

void CItemUsability::OnKensai() 
{
	Togglebit(7,4);	
}

void CItemUsability::OnLathander() 
{
	Togglebit(4,4);	
}

void CItemUsability::OnLawful() 
{
	Togglebit(0,16);		
}

void CItemUsability::OnLcneutral() 
{
	Togglebit(0,0x20);		
}

void CItemUsability::OnMage() 
{
	Togglebit(2,4);		
}

void CItemUsability::OnMathief() 
{
	Togglebit(2,8);		
}

void CItemUsability::OnMonk() 
{
	Togglebit(3,0x20);			
}

void CItemUsability::OnNecromancer() 
{
	Togglebit(6,16);		
}

void CItemUsability::OnPaladin() 
{
	Togglebit(2,16);		
}

void CItemUsability::OnRanger() 
{
	Togglebit(2,0x20);		
}

void CItemUsability::OnShape() 
{
	Togglebit(4,16);	
}

void CItemUsability::OnSkald() 
{
	Togglebit(5,0x80);	
}

void CItemUsability::OnStalker() 
{
	Togglebit(5,1);	
}

void CItemUsability::OnSwash() 
{
	Togglebit(5,16);	
}

void CItemUsability::OnTalos() 
{
	Togglebit(4,1);	
}

void CItemUsability::OnThief() 
{
	Togglebit(2,0x40);		
}

void CItemUsability::OnTotemic() 
{
	Togglebit(4,8);	
}

void CItemUsability::OnTransmuter() 
{
	Togglebit(6,0x20);		
}

void CItemUsability::OnUndeadhunter() 
{
	Togglebit(7,0x20);	
}

void CItemUsability::OnWildmage() 
{
	Togglebit(4,0x80);
}

void CItemUsability::OnWizardslayer() 
{
	Togglebit(7,2);	
}

void CItemUsability::OnAllset() 
{
  memset(bytes,0,sizeof(bytes) );
  Togglebit(0,0);
  UpdateData(UD_DISPLAY);
}

void CItemUsability::OnAllunset() 
{
  memset(bytes,-1,sizeof(bytes) );
  Togglebit(0,0);
  UpdateData(UD_DISPLAY);
}

/////////////////////////////////////////////////////////////////////////////
// CItemEquip property page

CItemEquip::CItemEquip() : CPropertyPage(CItemEquip::IDD)
{
	//{{AFX_DATA_INIT(CItemEquip)
	//}}AFX_DATA_INIT
  equipnum=0;
}

CItemEquip::~CItemEquip()
{
}

static int equeffids[]={
//fields
  IDC_EQUIPNUM,
//buttons
  IDC_EQUIPREMOVE, IDC_EQUIPCOPY, IDC_EQUIPPASTE, IDC_EDIT,
0};

void CItemEquip::EnableWindow_Equip(bool value)
{
  CWnd *equeffwnd;
  int id;

  for(id=0;equeffids[id];id++)
  {
    equeffwnd=(class CWnd *) GetDlgItem(equeffids[id]);
    if(equeffwnd) equeffwnd->EnableWindow(value);
  }
}

void CItemEquip::RefreshEquip()
{
  CString tmpstr;
  int i;

  if(m_equipnum_control)
  {
    m_equipnum_control.ResetContent();
    for(i=0;i<the_item.header.featblkcount;i++)
    {
      tmpstr.Format("%d %s (0x%x 0x%x) %.8s",i+1,
        get_opcode_text(the_item.featblocks[i].feature),
        the_item.featblocks[i].par1.parl,
        the_item.featblocks[i].par2.parl,
        the_item.featblocks[i].vvc);      
      m_equipnum_control.AddString(tmpstr);
    }
    if(equipnum<0 || equipnum>=i) equipnum=0;
    equipnum=m_equipnum_control.SetCurSel(equipnum);
  }
}

void CItemEquip::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CItemEquip)
	DDX_Control(pDX, IDC_EQUIPNUM, m_equipnum_control);
	//}}AFX_DATA_MAP
  //this is only the equipping feature count
  if(the_item.header.featblkcount)
  {
    EnableWindow_Equip(true);
  }
  else
  {
    EnableWindow_Equip(false);
  }
}

BOOL CItemEquip::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
  RefreshEquip();
  UpdateData(UD_DISPLAY);
	return TRUE;
}

BEGIN_MESSAGE_MAP(CItemEquip, CPropertyPage)
	//{{AFX_MSG_MAP(CItemEquip)
	ON_CBN_KILLFOCUS(IDC_EQUIPNUM, OnKillfocusEquipnum)
	ON_BN_CLICKED(IDC_EQUIPADD, OnEquipadd)
	ON_BN_CLICKED(IDC_EQUIPCOPY, OnEquipcopy)
	ON_BN_CLICKED(IDC_EQUIPPASTE, OnEquippaste)
	ON_BN_CLICKED(IDC_EQUIPREMOVE, OnEquipremove)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	ON_CBN_DBLCLK(IDC_EQUIPNUM, OnEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CItemEquip::OnKillfocusEquipnum() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CItemEquip::OnEquipremove() 
{
  if(!the_item.header.featblkcount) return;
  equipnum=m_equipnum_control.GetCurSel();
  if(equipnum<0) return;
  if(!DeleteFeatures(equipnum,1,0))
  {
    the_item.header.featblkcount--;  
  }
  if(equipnum==the_item.header.featblkcount) equipnum--;
  RefreshEquip();
  UpdateData(UD_DISPLAY);
}

void CItemEquip::OnEquipadd() 
{
  feat_block *new_featblocks;
  int fbc;
  int allcount;
  int i;

  if(the_item.header.featblkcount>99)
  {
    MessageBox("Cannot add more features.","Warning",MB_ICONEXCLAMATION|MB_OK);
    return;
  }
  //find index for extended effect start
  //exteffnum: actual feature number in actual feature block
  if(the_item.header.featblkcount)
  {
    equipnum=m_equipnum_control.GetCurSel();
    fbc=equipnum+1;
  }
  else fbc=0;
  allcount=GetFBC(the_item.extheadcount); //this is the total number of features
  new_featblocks=new feat_block[allcount+1];
  if(!new_featblocks)
  {
    MessageBox("Not enough memory.","Error",MB_ICONSTOP|MB_OK);
    return;
  }

  for(i=0;i<fbc;i++)
  {
    new_featblocks[i]=the_item.featblocks[i];
  }
  for(;i<allcount;i++)
  {
    new_featblocks[i+1]=the_item.featblocks[i];
  }
  memset(&new_featblocks[fbc],0,sizeof(feat_block) );
  new_featblocks[fbc].target=1; //target is self
  new_featblocks[fbc].timing=2; //while equipped by default
  new_featblocks[fbc].prob2=100; //upper probability 100

  //switching to the new block
  if(the_item.featblocks)
  {
    delete [] the_item.featblocks;
  }
  equipnum=fbc;
  the_item.featblocks=new_featblocks;
  //this is the cummulative feature block count
  the_item.header.featblkcount++;
  the_item.featblkcount++;
  RefreshEquip();
  UpdateData(UD_DISPLAY);
}

void CItemEquip::OnEdit() 
{
  CString tmpname;
  CEffEdit dlg;

  equipnum=m_equipnum_control.GetCurSel();
  if((equipnum<the_item.featblkcount) && (equipnum>=0) )
  {
    ConvertToV20Eff((creature_effect *) the_effect.header.signature,the_item.featblocks+equipnum );
    dlg.SetLimitedEffect(1);
    tmpname=itemname;
    itemname.Format("%d of %s", equipnum+1, tmpname);
    dlg.SetDefaultDuration(0);
    dlg.DoModal();
    itemname=tmpname;
    ConvertToV10Eff((creature_effect *) the_effect.header.signature,the_item.featblocks+equipnum );
  }
  RefreshEquip();
  UpdateData(UD_DISPLAY);
}

void CItemEquip::OnEquipcopy() 
{
  equipnum=m_equipnum_control.GetCurSel();
  if(equipnum<the_item.featblkcount && equipnum>=0)
  {
    memcpy(&featcopy,&the_item.featblocks[equipnum],sizeof(feat_block) );
  }
}

void CItemEquip::OnEquippaste() 
{
  equipnum=m_equipnum_control.GetCurSel();
  if(equipnum<the_item.featblkcount && equipnum>=0)
  {
    memcpy(&the_item.featblocks[equipnum],&featcopy,sizeof(feat_block) );
  }
  RefreshEquip();
  UpdateData(UD_DISPLAY);
}

/////////////////////////////////////////////////////////////////////////////
// CItemExtended property page

CItemExtended::CItemExtended() : CPropertyPage(CItemExtended::IDD)
{
  int i;

  for(i=0;i<3;i++)
  {
	  m_tooltipref.data[i] = -1; //-1 means it is unused data
  }
	//{{AFX_DATA_INIT(CItemExtended)
	//}}AFX_DATA_INIT
	extheadnum = 0;
  exteffnum = 0;
  hbmu=NULL;
}

CItemExtended::~CItemExtended()
{
  if(hbmu) DeleteObject(hbmu);
}

static int forceidboxids[8]={IDC_ID, IDC_NOID,0,0,0,0,0,0};

static int keenboxids[32]={IDC_STRBONUS,0,0,0,0,0,0,0,
0,0,0,IDC_RECHARGES,0,0,0,0,
IDC_FLAG1, IDC_FLAG2,0,0,0,0,0,0,
0,0,0,0,0,0,0,0};

void CItemExtended::DoDataExchangeExtended(CDataExchange* pDX)
{
  CButton *cb;
  int tmp;
  int i,j;
  CString tmpstr;

  if(pDX->m_bSaveAndValidate==UD_DISPLAY)
  {
    //m_extheadnum: displayed actual extended header number
    //extheadnum: actual extended header number
    m_extheadnum=extheadnum+1;
    EnableWindow_Extended(true, m_extheadnum<4);
    DDX_Text(pDX, IDC_EXTHEADNUM, m_extheadnum);    
    DDV_MinMaxLong(pDX, m_extheadnum, 1,the_item.header.extheadcount);

    RetrieveResref(tmpstr, the_item.extheaders[extheadnum].useicon);
  	DDX_Text(pDX, IDC_EXTUSEICON, tmpstr);
  	DDV_MaxChars(pDX, tmpstr, 8);
    if(bmu!=tmpstr)
    {
      bmu=tmpstr;
      
      if(!read_bam(tmpstr))
      {
        the_bam.MakeBitmap(0,RGB(32,32,32),hbmu,BM_RESIZE,32,32);
        m_useicon_control.SetBitmap(hbmu);
      }
      else m_useicon_control.SetBitmap(0);
    }

    tmpstr=get_attack_type(the_item.extheaders[extheadnum].attack_type);
    DDX_Text(pDX, IDC_EXTTYPE, tmpstr);
    
    tmpstr=get_location_type(the_item.extheaders[extheadnum].location);
    DDX_Text(pDX, IDC_LOC, tmpstr);
    
    DDX_Text(pDX, IDC_TARGETNUM, the_item.extheaders[extheadnum].target_num);
    DDV_MinMaxInt(pDX, the_item.extheaders[extheadnum].target_num, 0, 99);
    
    tmpstr=get_target_type(the_item.extheaders[extheadnum].target_type);
    DDX_Text(pDX, IDC_TARGET, tmpstr);

    tmpstr=get_ammo_type(the_item.extheaders[extheadnum].projectile);
    DDX_Text(pDX, IDC_PROJFRAME, tmpstr);

  	DDX_Text(pDX, IDC_RANGE,(short &) the_item.extheaders[extheadnum].range);
  	DDX_Text(pDX, IDC_SPEED,(short &) the_item.extheaders[extheadnum].speed);
  	DDX_Text(pDX, IDC_IDENTIFY, the_item.extheaders[extheadnum].force_id);
    tmp=the_item.extheaders[extheadnum].force_id;
    j=1;
    for(i=0;i<8;i++)
    {
      cb=(CButton *) GetDlgItem(forceidboxids[i]);
      if(cb) cb->SetCheck(!!(tmp&j) );
      j<<=1;
    }

    DDX_Text(pDX, IDC_UNKNOWN1, the_item.extheaders[extheadnum].unknown03);

    DDX_Text(pDX, IDC_THAC0,the_item.extheaders[extheadnum].thaco);
    DDX_Text(pDX, IDC_ROLL,(short &) the_item.extheaders[extheadnum].dammult);
    DDX_Text(pDX, IDC_DIE,(short &) the_item.extheaders[extheadnum].damdice);
    DDX_Text(pDX, IDC_ADD,(short &) the_item.extheaders[extheadnum].damplus);

    tmpstr=get_damage_type(the_item.extheaders[extheadnum].damtype);
    DDX_Text(pDX, IDC_DAMAGETYPE, tmpstr);

    if(extheadnum<3)
    {
  	  DDX_Text(pDX, IDC_TOOLTIPREF, m_tooltipref.data[extheadnum]);
	    DDV_MinMaxLong(pDX, m_tooltipref.data[extheadnum], -1, 1000000);
    	DDX_Text(pDX, IDC_TOOLTIPTEXT, m_tooltip[extheadnum]);
    }
    tmp=the_item.extheaders[extheadnum].missile[0];
  	DDX_Check(pDX, IDC_BOW, tmp);    

    tmp=the_item.extheaders[extheadnum].missile[1];
  	DDX_Check(pDX, IDC_XBOW, tmp);

    tmp=the_item.extheaders[extheadnum].missile[2];
  	DDX_Check(pDX, IDC_MISC, tmp);

  	DDX_Text(pDX, IDC_ANIM1,(short &) the_item.extheaders[extheadnum].animation[0]);
  	DDX_Text(pDX, IDC_ANIM2,(short &) the_item.extheaders[extheadnum].animation[1]);
  	DDX_Text(pDX, IDC_ANIM3,(short &) the_item.extheaders[extheadnum].animation[2]);

   	DDX_Text(pDX, IDC_CHARGES,(short &) the_item.extheaders[extheadnum].charges);

    tmpstr=get_charge_type(the_item.extheaders[extheadnum].drained);
    DDX_Text(pDX, IDC_PERDAY, tmpstr);

    tmpstr.Format("0x%08x",the_item.extheaders[extheadnum].flags);
   	DDX_Text(pDX, IDC_FLAGS, tmpstr);

    j=1;
    for(i=0;i<32;i++)
    {
      cb=(CButton *) GetDlgItem(keenboxids[i]);
      if(cb) cb->SetCheck(!!(the_item.extheaders[extheadnum].flags&j) );
      j<<=1;
    }

    tmpstr=get_projectile_id(the_item.extheaders[extheadnum].proref,0);
   	DDX_Text(pDX, IDC_PROJID,tmpstr);

    tmpstr.Format("%d",the_item.extheaders[extheadnum].fbcount);
    DDX_Text(pDX, IDC_EFFNUM, tmpstr);
  }
  else //retrieve
  {
    DDX_Text(pDX, IDC_EXTHEADNUM, tmpstr);
    m_extheadnum=atoi(tmpstr);  //should stay decimal

  	DDX_Text(pDX, IDC_EXTUSEICON, tmpstr);
    StoreResref(tmpstr,the_item.extheaders[extheadnum].useicon);
    
    DDX_Text(pDX, IDC_EXTTYPE, tmpstr);
    the_item.extheaders[extheadnum].attack_type=(unsigned char) strtonum(tmpstr);
    
    DDX_Text(pDX, IDC_LOC, tmpstr);
    the_item.extheaders[extheadnum].location=(unsigned char) strtonum(tmpstr);
    
    DDX_Text(pDX, IDC_TARGETNUM, the_item.extheaders[extheadnum].target_num);
    
    DDX_Text(pDX, IDC_TARGET, tmpstr);
    the_item.extheaders[extheadnum].target_type=(unsigned char) strtonum(tmpstr);
    
    DDX_Text(pDX, IDC_PROJFRAME, tmpstr);
    the_item.extheaders[extheadnum].projectile=(short) strtonum(tmpstr);

  	DDX_Text(pDX, IDC_RANGE,(short &) the_item.extheaders[extheadnum].range);
  	DDX_Text(pDX, IDC_SPEED,(short &) the_item.extheaders[extheadnum].speed);
  	DDX_Text(pDX, IDC_IDENTIFY, tmpstr);
    the_item.extheaders[extheadnum].force_id=(unsigned char) strtonum(tmpstr);

    DDX_Text(pDX, IDC_UNKNOWN1, the_item.extheaders[extheadnum].unknown03);

  	DDX_Text(pDX, IDC_THAC0,the_item.extheaders[extheadnum].thaco);
    DDX_Text(pDX, IDC_ROLL,(short &) the_item.extheaders[extheadnum].dammult);
    DDX_Text(pDX, IDC_DIE,(short &) the_item.extheaders[extheadnum].damdice);
    DDX_Text(pDX, IDC_ADD,the_item.extheaders[extheadnum].damplus);

    DDX_Text(pDX, IDC_DAMAGETYPE, tmpstr);
    the_item.extheaders[extheadnum].damtype=(unsigned char) strtonum(tmpstr);

//  	DDX_Text(pDX, IDC_STRBONUS, the_item.extheaders[extheadnum].str_bonus);
    if(extheadnum<3)
    {
  	  DDX_Text(pDX, IDC_TOOLTIPREF, m_tooltipref.data[extheadnum]);
    	DDX_Text(pDX, IDC_TOOLTIPTEXT, m_tooltip[extheadnum]);
    }
  	DDX_Check(pDX, IDC_BOW, tmp);
    the_item.extheaders[extheadnum].missile[0]=(unsigned short) tmp;
  	DDX_Check(pDX, IDC_XBOW, tmp);
    the_item.extheaders[extheadnum].missile[1]=(unsigned short) tmp;
  	DDX_Check(pDX, IDC_MISC, tmp);
    the_item.extheaders[extheadnum].missile[2]=(unsigned short) tmp;

  	DDX_Text(pDX, IDC_ANIM1,(short &) the_item.extheaders[extheadnum].animation[0]);
  	DDX_Text(pDX, IDC_ANIM2,(short &) the_item.extheaders[extheadnum].animation[1]);
  	DDX_Text(pDX, IDC_ANIM3,(short &) the_item.extheaders[extheadnum].animation[2]);

   	DDX_Text(pDX, IDC_CHARGES,(short &) the_item.extheaders[extheadnum].charges);

    DDX_Text(pDX, IDC_PERDAY, tmpstr);
    the_item.extheaders[extheadnum].drained=(unsigned short) strtonum(tmpstr);

   	DDX_Text(pDX, IDC_FLAGS,tmpstr);
    the_item.extheaders[extheadnum].flags=strtonum(tmpstr);

   	DDX_Text(pDX, IDC_PROJID,tmpstr);
    the_item.extheaders[extheadnum].proref=(unsigned short) strtonum(tmpstr);

    //logical adjustments
    switch(the_item.extheaders[extheadnum].attack_type)
    {
    case A_MAGIC:
  	  the_item.extheaders[extheadnum].missile[0]=0;
  	  the_item.extheaders[extheadnum].missile[1]=0;
  	  the_item.extheaders[extheadnum].missile[2]=0;
      break;
    case A_MELEE:
      the_item.extheaders[extheadnum].location=1;
      the_item.extheaders[extheadnum].target_num=0;
      the_item.extheaders[extheadnum].target_type=1;
  	  the_item.extheaders[extheadnum].missile[0]=0;
  	  the_item.extheaders[extheadnum].missile[1]=0;
  	  the_item.extheaders[extheadnum].missile[2]=0;
      break;
    case A_PROJ: case A_BOW: //bows crash if animation is set
      the_item.extheaders[extheadnum].location=1;
      the_item.extheaders[extheadnum].target_num=0;
      the_item.extheaders[extheadnum].target_type=1;
      the_item.extheaders[extheadnum].animation[0]=0;
      the_item.extheaders[extheadnum].animation[1]=0;
      the_item.extheaders[extheadnum].animation[2]=0;
    }
    extheadnum=m_extheadnum-1;
  }
}

void CItemExtended::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CItemExtended)
	DDX_Control(pDX, IDC_MELEE, m_melee_control);
  DDX_Control(pDX, IDC_USEICON, m_useicon_control);
	DDX_Control(pDX, IDC_PROJID, m_projid_control);
	DDX_Control(pDX, IDC_PERDAY, m_chargetype_control);
	DDX_Control(pDX, IDC_EXTEFFNUM, m_exteffnum_control);
	DDX_Control(pDX, IDC_DAMAGETYPE, m_damagetype_control);
	DDX_Control(pDX, IDC_EXTHEADNUM, m_extheadnum_control);
	DDX_Control(pDX, IDC_PROJFRAME, m_projframe_control);
	DDX_Control(pDX, IDC_TARGET, m_target_control);
	DDX_Control(pDX, IDC_LOC, m_loc_control);
	DDX_Control(pDX, IDC_EXTTYPE, m_exttype_control);
	DDX_Text(pDX, IDC_HEADNUM, m_headnum);
	DDX_Text(pDX, IDC_EFFNUM, m_effnum);
	//}}AFX_DATA_MAP
  if(the_item.header.extheadcount)
  {
    DoDataExchangeExtended(pDX);
    if(the_item.extheaders[extheadnum].fbcount)
    {
      EnableWindow_ExtEffect(true);
    }
    else
    {
      EnableWindow_ExtEffect(false);
    }
  }
  else
  {
    EnableWindow_Extended(false, false);
    EnableWindow_ExtEffect(false);
  }
}

//we have to check if m_extheadnum_control is a valid window
//m_extheadnum_control doesn't exist all the time
//
void CItemExtended::RefreshExtended()
{
  CString tmp;
  int fbc;
  int i;

  memset(m_tooltipref.data,-1,sizeof(m_tooltipref.data) );
  tooltipnumbers.Lookup(itemname, m_tooltipref);
  m_headnum.Format("%d",the_item.header.extheadcount);
  if(IsWindow(m_extheadnum_control))
  {
    m_extheadnum_control.ResetContent();
    for(i=0;i<the_item.extheadcount;i++)
    {
      tmp.Format("%d (%d effects)",i+1,the_item.extheaders[i].fbcount);
      m_extheadnum_control.AddString(tmp);
    }
    if(extheadnum<0 || extheadnum>=i) extheadnum=0;
    extheadnum=m_extheadnum_control.SetCurSel(extheadnum);
  }
  if(extheadnum<0) return;
  if(extheadnum<=2)
  {
    m_tooltip[extheadnum]=resolve_tlk_text(m_tooltipref.data[extheadnum]);
  }
  if(IsWindow(m_exteffnum_control) )
  {
    if(the_item.header.extheadcount)
    {
      fbc=GetFBC(extheadnum); //this is the starting fbc in cummulative feature block
      m_exteffnum_control.ResetContent();
      for(i=0;i<the_item.extheaders[extheadnum].fbcount;i++)
      {
        tmp.Format("%d %s (0x%x 0x%x) %.8s",i+1,
          get_opcode_text(the_item.featblocks[fbc+i].feature),
          the_item.featblocks[fbc+i].par1.parl,
          the_item.featblocks[fbc+i].par2.parl,
          the_item.featblocks[fbc+i].vvc);
        m_exteffnum_control.AddString(tmp);
      }
      if(exteffnum<0 || exteffnum>=i) exteffnum=0;
      exteffnum=m_exteffnum_control.SetCurSel(exteffnum);
      m_melee_control.SetCurSel(find_animtype(the_item.extheaders[extheadnum].animation));
    }
  }
}

BOOL CItemExtended::OnInitDialog() 
{
  POSITION pos, pos2;
  int i;
  CString tmp, tmp2;

	CPropertyPage::OnInitDialog();
  m_extheadnum_control.SetDroppedWidth(80);

  for(i=0;i<NUM_ATYPE;i++)
  {
    m_exttype_control.AddString(get_attack_type(i));
  }

  for(i=0;i<NUM_LFTYPE;i++)
  {
    m_loc_control.AddString(get_location_type(i));
  }

  for(i=0;i<NUM_TTYPE;i++)
  {
    m_target_control.AddString(get_target_type(i));
  }

  for(i=0;i<NUM_AMTYPE;i++)
  {
    m_projframe_control.AddString(ammo_types[i]);
  }
  
  for(i=0;i<NUM_DTYPE;i++)
  {
    m_damagetype_control.AddString(get_damage_type(i));
  }

  for(i=0;i<NUM_CHTYPE;i++)
  {
    m_chargetype_control.AddString(get_charge_type(i));
  }

  for(i=0;i<NUM_ANIMTYPES;i++)
  {
    m_melee_control.AddString(animnames[i]);
  }

  pos=pro_references.GetHeadPosition();
  pos2=pro_titles.GetHeadPosition();
  i=0;
  while(pos)
  {
    if(pos2) tmp2=pro_titles.GetNext(pos2);
    else tmp2.Format("???");
    tmp.Format("%d-%s (%s)",i,tmp2, pro_references.GetNext(pos) );
    m_projid_control.AddString(tmp);
    i++;
  }
  RefreshExtended();
  UpdateData(UD_DISPLAY);
	return TRUE;
}

static int exteffids[]={
//buttons
  IDC_EXTEFFNUM, IDC_EXTEFFREMOVE, IDC_EXTEFFCOPY, 
  IDC_EXTEFFPASTE, IDC_BROWSE, IDC_EDIT,
0};

void CItemExtended::EnableWindow_ExtEffect(bool value)
{
  CWnd *exteffwnd;
  int id;

  for(id=0;exteffids[id];id++)
  {
    exteffwnd=(class CWnd *) GetDlgItem(exteffids[id]);
    if(exteffwnd) exteffwnd->EnableWindow(value);
  }
}

static int extids2[]={
  IDC_TOOLTIPTEXT, IDC_TOOLTIPREF,0
};
static int extids[]={
  //attribute controls
  IDC_EXTUSEICON, IDC_USEICON,
  IDC_EXTHEADNUM, IDC_EXTTYPE, IDC_LOC, IDC_TARGETNUM, IDC_TARGET,
  IDC_PROJFRAME, IDC_RANGE, IDC_SPEED, IDC_IDENTIFY, IDC_UNKNOWN1,
  IDC_THAC0, IDC_ROLL, IDC_DIE, IDC_ADD, IDC_DAMAGETYPE, 
  IDC_BOW, IDC_XBOW, IDC_MISC, IDC_ANIM1, IDC_ANIM2, IDC_ANIM3,
  IDC_CHARGES, IDC_PERDAY, 
  IDC_FLAGS, IDC_STRBONUS, IDC_RECHARGES, IDC_FLAG1, IDC_FLAG2,
  IDC_PROJID,  IDC_ID, IDC_NOID, IDC_MELEE, 
  //buttons
  //last button really belongs here!
  IDC_EXTREMOVE, IDC_EXTCOPY, IDC_EXTPASTE, IDC_EXTEFFADD,
0};

void CItemExtended::EnableWindow_Extended(bool value, bool second)
{
  CWnd *extwnd;
  int id;

  for(id=0;extids[id];id++)
  {
    extwnd=(class CWnd *) GetDlgItem(extids[id]);
    extwnd->EnableWindow(value);
  }
  for(id=0;extids2[id];id++)
  {
    extwnd=(class CWnd *) GetDlgItem(extids2[id]);
    extwnd->EnableWindow(second);
  }
}

BEGIN_MESSAGE_MAP(CItemExtended, CPropertyPage)
	//{{AFX_MSG_MAP(CItemExtended)
	ON_CBN_KILLFOCUS(IDC_EXTEFFNUM, OnKillfocusExteffnum)
	ON_CBN_KILLFOCUS(IDC_EXTHEADNUM, OnKillfocusExtheadnum)
	ON_BN_CLICKED(IDC_EXTPASTE, OnExtpaste)
	ON_BN_CLICKED(IDC_EXTEFFADD, OnExteffadd)
	ON_BN_CLICKED(IDC_EXTEFFCOPY, OnExteffcopy)
	ON_BN_CLICKED(IDC_EXTEFFPASTE, OnExteffpaste)
	ON_BN_CLICKED(IDC_EXTEFFREMOVE, OnExteffremove)
	ON_BN_CLICKED(IDC_EXTCOPY, OnExtcopy)
	ON_BN_CLICKED(IDC_EXTADD, OnExtadd)
	ON_BN_CLICKED(IDC_EXTREMOVE, OnExtremove)
	ON_CBN_KILLFOCUS(IDC_EXTTYPE, OnDefaultKillfocus)
	ON_CBN_KILLFOCUS(IDC_LOC, OnDefaultKillfocus)
	ON_CBN_KILLFOCUS(IDC_TARGET, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_RANGE, OnDefaultKillfocus)
	ON_CBN_KILLFOCUS(IDC_PROJFRAME, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_SPEED, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_UNKNOWN1, OnKillfocusUnknown1)
	ON_CBN_SELCHANGE(IDC_EXTHEADNUM, OnSelchangeExtheadnum)
	ON_EN_KILLFOCUS(IDC_THAC0, OnKillfocusThac0)
	ON_EN_KILLFOCUS(IDC_ROLL, OnKillfocusRoll)
	ON_CBN_KILLFOCUS(IDC_DAMAGETYPE, OnKillfocusDamagetype)
	ON_EN_KILLFOCUS(IDC_DIE, OnKillfocusDie)
	ON_EN_KILLFOCUS(IDC_ADD, OnKillfocusAdd)
	ON_EN_KILLFOCUS(IDC_TARGETNUM, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_TOOLTIPTEXT, OnKillfocusTooltiptext)
	ON_EN_KILLFOCUS(IDC_TOOLTIPREF, OnKillfocusTooltipref)
	ON_EN_KILLFOCUS(IDC_STRBONUS, OnKillfocusStrbonus)
	ON_CBN_KILLFOCUS(IDC_PERDAY, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_ANIM1, OnKillfocusAnim1)
	ON_EN_KILLFOCUS(IDC_ANIM2, OnKillfocusAnim2)
	ON_EN_KILLFOCUS(IDC_ANIM3, OnKillfocusAnim3)
	ON_EN_KILLFOCUS(IDC_CHARGES, OnDefaultKillfocus)
	ON_BN_CLICKED(IDC_BOW, OnBow)
	ON_BN_CLICKED(IDC_XBOW, OnXbow)
	ON_BN_CLICKED(IDC_MISC, OnMisc)
	ON_EN_KILLFOCUS(IDC_EXTUSEICON, OnKillfocusExtuseicon)
	ON_CBN_KILLFOCUS(IDC_PROJID, OnDefaultKillfocus)
	ON_BN_CLICKED(IDC_USEICON, OnUseicon)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	ON_BN_CLICKED(IDC_ID, OnId)
	ON_BN_CLICKED(IDC_NOID, OnNoid)
	ON_CBN_SELCHANGE(IDC_MELEE, OnSelchangeMelee)
	ON_BN_CLICKED(IDC_FLAG1, OnFlag1)
	ON_BN_CLICKED(IDC_FLAG2, OnFlag2)
	ON_BN_CLICKED(IDC_RECHARGES, OnRecharges)
	ON_BN_CLICKED(IDC_STRBONUS, OnStrbonus)
	ON_EN_KILLFOCUS(IDC_FLAGS, OnDefaultKillfocus)
	ON_CBN_DBLCLK(IDC_EXTEFFNUM, OnEdit)
	ON_EN_KILLFOCUS(IDC_IDENTIFY, OnDefaultKillfocus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CItemExtended::OnKillfocusExtheadnum() 
{
  UpdateData(UD_RETRIEVE);
  exteffnum=0;  
  RefreshExtended();
  UpdateData(UD_DISPLAY);
}

void CItemExtended::OnSelchangeExtheadnum() 
{
  extheadnum=m_extheadnum_control.GetCurSel();
  exteffnum=0;
  RefreshExtended();
  UpdateData(UD_DISPLAY);
}

void CItemExtended::OnKillfocusExteffnum() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CItemExtended::OnKillfocusExtuseicon() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);	
}

void CItemExtended::OnUseicon() 
{
  pickerdlg.m_restype=REF_BAM;
  RetrieveResref(pickerdlg.m_picked,the_item.extheaders[extheadnum].useicon);
  if(pickerdlg.DoModal()==IDOK)
  {    
    StoreResref(pickerdlg.m_picked,the_item.extheaders[extheadnum].useicon);
  }
  UpdateData(UD_DISPLAY);	
}

void CItemExtended::OnDefaultKillfocus() 
{
  UpdateData(UD_RETRIEVE);  
  UpdateData(UD_DISPLAY);
}

void CItemExtended::OnId() 
{
  UpdateData(UD_RETRIEVE);
  the_item.extheaders[extheadnum].force_id^=1;
  UpdateData(UD_DISPLAY);
}

void CItemExtended::OnNoid() 
{
  UpdateData(UD_RETRIEVE);
  the_item.extheaders[extheadnum].force_id^=2;
  UpdateData(UD_DISPLAY);
}

void CItemExtended::OnFlag1() 
{
  UpdateData(UD_RETRIEVE);
  the_item.extheaders[extheadnum].flags^=1<<16;
  UpdateData(UD_DISPLAY);
}

void CItemExtended::OnFlag2() 
{
  UpdateData(UD_RETRIEVE);
  the_item.extheaders[extheadnum].flags^=2<<16;
  UpdateData(UD_DISPLAY);
}

void CItemExtended::OnRecharges() 
{
  UpdateData(UD_RETRIEVE);
  the_item.extheaders[extheadnum].flags^=8<<8;
  UpdateData(UD_DISPLAY);
}

void CItemExtended::OnStrbonus() 
{
  UpdateData(UD_RETRIEVE);
  the_item.extheaders[extheadnum].flags^=1;
  UpdateData(UD_DISPLAY);
}

void CItemExtended::OnKillfocusUnknown1() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CItemExtended::OnKillfocusThac0() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CItemExtended::OnKillfocusRoll() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CItemExtended::OnKillfocusDie() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CItemExtended::OnKillfocusAdd() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CItemExtended::OnKillfocusDamagetype() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CItemExtended::OnKillfocusStrbonus() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

static CString tooltipcolumnnames[]={"1","2","3"};

void CItemExtended::OnKillfocusTooltiptext() 
{
  CString tmpstr;

  if(extheadnum<0 || extheadnum>2) return;
  UpdateData(UD_RETRIEVE);  
  if(itemname!="new item")
  {
    tmpstr=resolve_tlk_text(m_tooltipref.data[extheadnum]);
    if(tmpstr!=m_tooltip[extheadnum])
    {
      if(editflg&TLKCHANGE) 
      {
        if(MessageBox("Do you want to update dialog.tlk?","Item editor",MB_YESNO)!=IDYES)
        {
          return;
        }
      }
      m_tooltipref.data[extheadnum]=store_tlk_text(m_tooltipref.data[extheadnum],m_tooltip[extheadnum]);
      tooltipnumbers.SetAt(itemname,m_tooltipref);
      Write2daArrayToFile("TOOLTIP", tooltipnumbers, tooltipcolumnnames, 3);
    }
  }
  UpdateData(UD_DISPLAY);
}

void CItemExtended::OnKillfocusTooltipref() 
{
  if(extheadnum<0 || extheadnum>2) return;
  UpdateData(UD_RETRIEVE);
  if(m_tooltipref.data[extheadnum]!=tooltipnumbers[itemname].data[extheadnum] && (itemname!="new item") )
  {
    if((editflg&DACHANGE) && (MessageBox("Do you want to update TOOLTIP.2da?","Item editor",MB_YESNO)!=IDYES) )
    {
      return;
    }
    if(m_tooltipref.data[0]==-1 && m_tooltipref.data[1]==-1 && m_tooltipref.data[2]==-1)
    {
      tooltipnumbers.RemoveKey(itemname); //all became -1, we just remove it from the 2da
    }
    else
    { //this will create an entry, if it wasn't there yet, we store ALL entries to create
      //a new entry with -1 fields
      tooltipnumbers.SetAt(itemname,m_tooltipref);
    }
    Write2daArrayToFile("TOOLTIP", tooltipnumbers, tooltipcolumnnames, 3);
  }
  RefreshExtended();      //refreshing tooltip text
  UpdateData(UD_DISPLAY);
}

void CItemExtended::OnBow() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CItemExtended::OnXbow() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CItemExtended::OnMisc() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CItemExtended::OnSelchangeMelee() 
{
  int pick;

  UpdateData(UD_RETRIEVE);
  pick=m_melee_control.GetCurSel();
  if(pick>=0 && pick<NUM_ANIMTYPES-1) //last value is 'unknown'
  {
    memcpy(the_item.extheaders[extheadnum].animation,animtypes[pick],3*sizeof(short));
  }
  UpdateData(UD_DISPLAY);	
}

void CItemExtended::OnKillfocusAnim1() 
{
  UpdateData(UD_RETRIEVE);
  RefreshExtended();
  UpdateData(UD_DISPLAY);	
}

void CItemExtended::OnKillfocusAnim2() 
{
  UpdateData(UD_RETRIEVE);	
  RefreshExtended();
  UpdateData(UD_DISPLAY);	
}

void CItemExtended::OnKillfocusAnim3() 
{
  UpdateData(UD_RETRIEVE);	
  RefreshExtended();
  UpdateData(UD_DISPLAY);	
}

///// feature blocks

void CItemExtended::OnExtremove() 
{
  int deletepos;
  int i;
  int res;
  ext_header *new_extheaders;

  extheadnum=m_extheadnum_control.GetCurSel();
  if(extheadnum<0) return;
  deletepos=extheadnum;
  res=DeleteFeatures(GetFBC(deletepos),the_item.extheaders[deletepos].fbcount, 0);
  if(res)      
  {
    if(res==-2)
    {
      MessageBox("Internal error","Error",MB_ICONSTOP|MB_OK);
    }
    else
    {
      MessageBox("Not enough memory.","Error",MB_ICONSTOP|MB_OK);
    }
    return;
  }
  the_item.header.extheadcount--;
  if(the_item.header.extheadcount)
  {
    new_extheaders=new ext_header[the_item.header.extheadcount];
    if(!new_extheaders)
    {
      MessageBox("Not enough memory.","Error",MB_ICONSTOP|MB_OK);
      return;
    }
    for(i=0;i<deletepos;i++)
    {
      new_extheaders[i]=the_item.extheaders[i];
    }
    //deletes all features of this extended header (#i)
    //it starts at GetFBC
    for(;i<the_item.header.extheadcount;i++)
    {
      new_extheaders[i]=the_item.extheaders[i+1];
    }
  }
  else new_extheaders=0;
  //switching to the new block, delete should work here, because we
  //must have had at least one block
  delete [] the_item.extheaders;
  extheadnum=deletepos-1;
  exteffnum=0;
  the_item.extheadcount=the_item.header.extheadcount;
  the_item.extheaders=new_extheaders;
  RefreshExtended();
  UpdateData(UD_DISPLAY);
}

void CItemExtended::OnExtadd() 
{
  int insertpos;
  int i;
  ext_header *new_extheaders;

  //don't allow more than 9 extended headers
  if(the_item.header.extheadcount>9)
  {
    MessageBox("Cannot add more extended headers.","Warning",MB_ICONEXCLAMATION|MB_OK);
    return;
  }
  if(the_item.header.extheadcount)
  {
    extheadnum=m_extheadnum_control.GetCurSel();
	  insertpos=extheadnum+1;
  }
  else insertpos=0;
  the_item.header.extheadcount++;
  new_extheaders=new ext_header[the_item.header.extheadcount];
  if(!new_extheaders)
  {
    the_item.header.extheadcount--;
    MessageBox("Not enough memory.","Error",MB_ICONSTOP|MB_OK);
    return;
  }
  for(i=0;i<insertpos;i++)
  {
    new_extheaders[i]=the_item.extheaders[i];
  }
  for(;i<the_item.header.extheadcount-1;i++)
  {
    new_extheaders[i+1]=the_item.extheaders[i];
  }
  memset(&new_extheaders[insertpos],0,sizeof(ext_header) );
  new_extheaders[insertpos].target_type=1;   //creature
  if(insertpos)
  {
    new_extheaders[insertpos].attack_type=A_MAGIC; //magical
    new_extheaders[insertpos].location=LF_MAGIC;   //equipment
  }
  else
  {  //first header is most likely a melee/bow header
    new_extheaders[insertpos].location=LF_WEAPON;     //weapon
    new_extheaders[insertpos].thaco=(short) the_item.header.mplus; //thaco bonus
    switch(the_item.header.itemtype)
    {
    case IT_ARROW:
      new_extheaders[insertpos].attack_type=A_PROJ;  //projectile
      new_extheaders[insertpos].projectile=1;
      break;
    case IT_BOLT:
      new_extheaders[insertpos].attack_type=A_PROJ;  //projectile
      new_extheaders[insertpos].projectile=2;
      break;
    case IT_BULLET:
      new_extheaders[insertpos].attack_type=A_PROJ;  //projectile
      new_extheaders[insertpos].projectile=3;
      break;
    case IT_BOW:
      new_extheaders[insertpos].attack_type=A_BOW;  //bow
//      new_extheaders[insertpos].projectile=1;
      new_extheaders[insertpos].missile[M_ARROW]=1;
      break;
    case IT_XBOW:
      new_extheaders[insertpos].attack_type=A_BOW;  //bow
//      new_extheaders[insertpos].projectile=2;
      new_extheaders[insertpos].missile[M_BOLT]=1;
      break;
    case IT_SLING:
      new_extheaders[insertpos].attack_type=A_BOW;  //bow
//      new_extheaders[insertpos].projectile=3; //for some reason this isn't set
      new_extheaders[insertpos].missile[M_BULLET]=1;
      break;
    case IT_DART:
      new_extheaders[insertpos].attack_type=A_PROJ;
      break;
    case IT_DAGGER: case IT_MACE: case IT_SHORT:
    case IT_BASTARD:case IT_HAMMER: case IT_MORNING:
    case IT_FLAIL: case IT_AXE: case IT_SPEAR:
    case IT_HALBERD:case IT_CLUB: case IT_SWORD: 
      new_extheaders[insertpos].attack_type=A_MELEE;  //melee
      new_extheaders[insertpos].flags|=1;
      new_extheaders[insertpos].range=1;
      break;
    default:
      new_extheaders[insertpos].attack_type=A_MAGIC;
      new_extheaders[insertpos].range=1;
    }
  }
  //switching to the new block
  if(the_item.extheaders)
  {
    delete [] the_item.extheaders;
  }
  extheadnum=insertpos;
  exteffnum=0;
  the_item.extheadcount=the_item.header.extheadcount;
  the_item.extheaders=new_extheaders;
  RefreshExtended();
  UpdateData(UD_DISPLAY);
}

void CItemExtended::OnExtcopy() 
{
  int fbc;

  extheadnum=m_extheadnum_control.GetCurSel();
  if(extheadnum<0) return;
  memcpy(&headcopy, &the_item.extheaders[extheadnum],sizeof(ext_header) );
  if(featblkcnt!=the_item.extheaders[extheadnum].fbcount)
  {
    if(featblkcnt) delete [] featblks;
    featblkcnt=the_item.extheaders[extheadnum].fbcount;
    if(featblkcnt)
    {
      featblks=new feat_block[featblkcnt];
      if(!featblks)
      {
        featblkcnt=0;
        MessageBox("Not enough memory.","Error",MB_ICONSTOP|MB_OK);
        return;
      }
    }
  }
  fbc=GetFBC(extheadnum);
  memcpy(featblks, &the_item.featblocks[fbc],featblkcnt*sizeof(feat_block) );
}

void CItemExtended::OnExtpaste() 
{
  int fbc;  

  if(the_item.header.extheadcount)
  {
    fbc=GetFBC(extheadnum);
    if(!DeleteFeatures(fbc, the_item.extheaders[extheadnum].fbcount, featblkcnt))
    {
      memcpy(&the_item.extheaders[extheadnum], &headcopy, sizeof(ext_header) );    
      memcpy(&the_item.featblocks[fbc], featblks, featblkcnt*sizeof(feat_block) );
    }
    RefreshExtended();
    UpdateData(UD_DISPLAY);
  }
}

void CItemExtended::OnExteffremove() 
{
  if(!the_item.header.extheadcount) return;
  if(extheadnum<0) return;
  exteffnum=m_exteffnum_control.GetCurSel();
  if(exteffnum<0 || exteffnum>=the_item.extheaders[extheadnum].fbcount) return;
  if(!DeleteFeatures(GetFBC(extheadnum)+exteffnum,1,0))
  {
    the_item.extheaders[extheadnum].fbcount--;  
  }
  if(exteffnum==the_item.extheaders[extheadnum].fbcount) exteffnum--;
  RefreshExtended();
  UpdateData(UD_DISPLAY);
}

void CItemExtended::OnExteffadd() 
{
  feat_block *new_featblocks;
  int fbc;
  int allcount;
  int i;

  //this is the actual maximum size of the feature block in the
  //actual extended header
  if(the_item.extheaders[extheadnum].fbcount>99)
  {
    MessageBox("Cannot add more features.","Warning",MB_OK);
    return;
  }
  //find index for extended effect start
  //exteffnum: actual feature number in actual feature block
  if(extheadnum<0) return;
  if(the_item.extheaders[extheadnum].fbcount)
  {
    exteffnum=m_exteffnum_control.GetCurSel();
    fbc=GetFBC(extheadnum)+exteffnum+1;///////
  }
  else
  {
    exteffnum=0;
    fbc=GetFBC(extheadnum);
  }
  allcount=GetFBC(the_item.extheadcount);
  new_featblocks=new feat_block[allcount+1];
  if(!new_featblocks)
  {
    MessageBox("Not enough memory.","Error",MB_ICONSTOP|MB_OK);
    return;
  }

  for(i=0;i<fbc;i++)
  {
    new_featblocks[i]=the_item.featblocks[i];
  }
  for(;i<allcount;i++)
  {
    new_featblocks[i+1]=the_item.featblocks[i];
  }
  memset(&new_featblocks[fbc],0,sizeof(feat_block) );
  new_featblocks[fbc].target=2; //target is pre-target
  new_featblocks[fbc].timing=1; //permanent by default
  new_featblocks[fbc].prob2=100; //upper probability 100

  //switching to the new block
  if(the_item.featblocks)
  {
    delete [] the_item.featblocks;
  }
  exteffnum=fbc-GetFBC(extheadnum);
  the_item.extheaders[extheadnum].fbcount++;
  the_item.featblocks=new_featblocks;
  //this is the cummulative feature block count
  the_item.featblkcount++;
  RefreshExtended();
  UpdateData(UD_DISPLAY);
}

void CItemExtended::OnExteffcopy() 
{
  int fbc;

  if(extheadnum<0) return;
  exteffnum=m_exteffnum_control.GetCurSel();
  if(exteffnum<0 || exteffnum>=the_item.extheaders[extheadnum].fbcount) return;
  fbc=GetFBC(extheadnum)+exteffnum;
  memcpy(&featcopy,&the_item.featblocks[fbc],sizeof(feat_block) );
}

void CItemExtended::OnExteffpaste() 
{
  int fbc;

  if(extheadnum<0) return;
  exteffnum=m_exteffnum_control.GetCurSel();
  if(exteffnum<0 || exteffnum>=the_item.extheaders[extheadnum].fbcount) return;
  fbc=GetFBC(extheadnum)+exteffnum;
  memcpy(&the_item.featblocks[fbc],&featcopy, sizeof(feat_block) );
  RefreshExtended();
  UpdateData(UD_DISPLAY);
}

void CItemExtended::OnEdit() 
{
  int fbc;
  CString tmpname;
  CEffEdit dlg;

  if(extheadnum<0) return;
  exteffnum=m_exteffnum_control.GetCurSel();
  if(exteffnum<0 || exteffnum>=the_item.extheaders[extheadnum].fbcount) return;
  fbc=GetFBC(extheadnum)+exteffnum;
  ConvertToV20Eff((creature_effect *) the_effect.header.signature,the_item.featblocks+fbc );
  dlg.SetLimitedEffect(1);
  tmpname=itemname;
  itemname.Format("%d,%d of %s", extheadnum+1, exteffnum+1,tmpname);
  dlg.SetDefaultDuration(the_item.featblocks[GetFBC(extheadnum)].duration);
  dlg.DoModal();
  itemname=tmpname;
  ConvertToV10Eff((creature_effect *) the_effect.header.signature,the_item.featblocks+fbc );
  RefreshExtended();
  UpdateData(UD_DISPLAY);
}

/////////////////////////////////////////////////////////////////////////////
// CItemExtra property page
CItemExtra::CItemExtra() : CPropertyPage(CItemExtra::IDD)
{
	//{{AFX_DATA_INIT(CItemExtra)
	//}}AFX_DATA_INIT
}

CItemExtra::~CItemExtra()
{
}

void CItemExtra::DoDataExchange(CDataExchange* pDX)
{
  int i;

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CItemExtra)
	//}}AFX_DATA_MAP
  for(i=0;i<16;i++)
  {
    DDX_Text(pDX, IDC_U1+i,the_item.iwd2header.unknown[i]);
  	DDV_MinMaxInt(pDX, the_item.iwd2header.unknown[i], 0, 255);
  }
}

void CItemExtra::RefreshExtra()
{
}

BEGIN_MESSAGE_MAP(CItemExtra, CPropertyPage)
	//{{AFX_MSG_MAP(CItemExtra)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CItemExtra::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
  UpdateData(UD_DISPLAY);	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CItemPropertySheet

IMPLEMENT_DYNAMIC(CItemPropertySheet, CPropertySheet)

//ids_aboutbox is a fake ID, the sheet has no caption
CItemPropertySheet::CItemPropertySheet(CWnd* pWndParent)
: CPropertySheet(IDS_ABOUTBOX, pWndParent)
{
  AddPage(&m_PageGeneral);
  AddPage(&m_PageIcons);
  AddPage(&m_PageDescription);
  AddPage(&m_PageUsability);
  AddPage(&m_PageEquip);
  AddPage(&m_PageExtended);
  if(iwd2_structures())
  {
    AddPage(&m_PageExtra);
  }
  memset(&featcopy,0,sizeof(featcopy) );
  memset(&headcopy,0,sizeof(headcopy) );
  if(featblks)
  {
    delete [] featblks;
    featblks=NULL;
  }
  featblkcnt=0;
}

CItemPropertySheet::~CItemPropertySheet()
{
  if(featblks)
  {
    delete [] featblks;
    featblks=NULL;
  }
}

void CItemPropertySheet::RefreshDialog()
{
  CPropertyPage *page;

  m_PageGeneral.RefreshGeneral();
  m_PageIcons.RefreshIcons();
  m_PageDescription.RefreshDescription();
  m_PageUsability.RefreshUsability();
  m_PageEquip.RefreshEquip();
  m_PageExtended.RefreshExtended();
  if(iwd2_structures())
  {
    m_PageExtra.RefreshExtra();
  }
  page=GetActivePage();
  page->UpdateData(UD_DISPLAY);
}

BEGIN_MESSAGE_MAP(CItemPropertySheet, CPropertySheet)
//{{AFX_MSG_MAP(CItemPropertySheet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
