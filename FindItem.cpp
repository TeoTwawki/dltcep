// FindItem.cpp : implementation file
//

#include "stdafx.h"
#include "chitem.h"
#include "FindItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFindItem dialog

CFindItem::CFindItem(CWnd* pParent /*=NULL*/)
	: CDialog(CFindItem::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFindItem)
	//}}AFX_DATA_INIT
}

void CFindItem::DoDataExchange(CDataExchange* pDX)
{
  CString tmpstr; //hexadecimal

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFindItem)
	DDX_Control(pDX, IDC_MPROJ, m_proj_control);
	DDX_Control(pDX, IDC_CHANGE, m_change_control);
	DDX_Control(pDX, IDC_MTYPE, m_mtype_control);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_RESOURCE, m_resource);
	DDV_MaxChars(pDX, m_resource, 8);
	DDX_Text(pDX, IDC_ITEMNAME, m_itemname);
	DDV_MaxChars(pDX, m_itemname, 8);
	DDX_Text(pDX, IDC_VARIABLE, m_variable);
	DDV_MaxChars(pDX, m_variable, 32);
	DDX_Text(pDX, IDC_NEWRESOURCE, m_newres);
  DDV_MaxChars(pDX, m_newres, 8);
  if(pDX->m_bSaveAndValidate==UD_RETRIEVE)
  {
    DDX_Text(pDX, IDC_PROJ, tmpstr);
    searchdata.projectile=(unsigned long) strtonum(tmpstr);
    DDX_Text(pDX, IDC_PROJ2, tmpstr);
    searchdata.projectile2=(unsigned long) strtonum(tmpstr);
    DDX_Text(pDX, IDC_ITEMTYPE, tmpstr);
    searchdata.itemtype=(short) strtonum(tmpstr);
    DDX_Text(pDX, IDC_ITEMTYPE2, tmpstr);
    searchdata.itemtype2=(short) strtonum(tmpstr);
    DDX_Text(pDX, IDC_EFFECT, tmpstr);
    searchdata.feature=(short) strtonum(tmpstr);
    DDX_Text(pDX, IDC_EFFECT2, tmpstr);
    searchdata.feature2=(short) strtonum(tmpstr);
    DDX_Text(pDX, IDC_EA, tmpstr);
    searchdata.ea=(unsigned char) strtonum(tmpstr);
    DDX_Text(pDX, IDC_EA2, tmpstr);
    searchdata.ea2=(unsigned char) strtonum(tmpstr);
    DDX_Text(pDX, IDC_GENERAL, tmpstr);
    searchdata.general=(unsigned char) strtonum(tmpstr);
    DDX_Text(pDX, IDC_GENERAL2, tmpstr);
    searchdata.general2=(unsigned char) strtonum(tmpstr);
    DDX_Text(pDX, IDC_RACE, tmpstr);
    searchdata.race=(unsigned char) strtonum(tmpstr);
    DDX_Text(pDX, IDC_RACE2, tmpstr);
    searchdata.race2=(unsigned char) strtonum(tmpstr);
    DDX_Text(pDX, IDC_CLASS, tmpstr);
    searchdata.class1=(unsigned char) strtonum(tmpstr);
    DDX_Text(pDX, IDC_CLASS2, tmpstr);
    searchdata.class2=(unsigned char) strtonum(tmpstr);
    DDX_Text(pDX, IDC_SPECIFIC, tmpstr);
    searchdata.specific=(unsigned char) strtonum(tmpstr);
    DDX_Text(pDX, IDC_SPECIFIC2, tmpstr);
    searchdata.specific2=(unsigned char) strtonum(tmpstr);
    DDX_Text(pDX, IDC_GENDER, tmpstr);
    searchdata.gender=(unsigned char) strtonum(tmpstr);
    DDX_Text(pDX, IDC_GENDER2, tmpstr);
    searchdata.gender2=(unsigned char) strtonum(tmpstr);
    DDX_Text(pDX, IDC_ALIGN, tmpstr);
    searchdata.align=(unsigned char) strtonum(tmpstr);
    DDX_Text(pDX, IDC_ALIGN2, tmpstr);
    searchdata.align2=(unsigned char) strtonum(tmpstr);
    DDX_Text(pDX, IDC_CHANGE, tmpstr);
    searchdata.which=strtonum(tmpstr);
    DDX_Text(pDX, IDC_STRREF, tmpstr);
    searchdata.strref=strtonum(tmpstr);
    DDX_Text(pDX, IDC_NEWVALUE, tmpstr);
    searchdata.newvalue=strtonum(tmpstr);
  }
  else //display
  {
    DDX_Text(pDX, IDC_PROJ, searchdata.projectile);
    DDX_Text(pDX, IDC_PROJ2, searchdata.projectile2);
    DDX_Text(pDX, IDC_ITEMTYPE, searchdata.itemtype);
    DDX_Text(pDX, IDC_ITEMTYPE2, searchdata.itemtype2);
    DDX_Text(pDX, IDC_EFFECT, searchdata.feature);
    DDX_Text(pDX, IDC_EFFECT2, searchdata.feature2);
    DDX_Text(pDX, IDC_EA, searchdata.ea);
    DDX_Text(pDX, IDC_EA2, searchdata.ea2);
    DDX_Text(pDX, IDC_GENERAL, searchdata.general);
    DDX_Text(pDX, IDC_GENERAL2, searchdata.general2);
    DDX_Text(pDX, IDC_RACE, searchdata.race);
    DDX_Text(pDX, IDC_RACE2, searchdata.race2);
    DDX_Text(pDX, IDC_CLASS, searchdata.class1);
    DDX_Text(pDX, IDC_CLASS2, searchdata.class2);
    DDX_Text(pDX, IDC_SPECIFIC, searchdata.specific);
    DDX_Text(pDX, IDC_SPECIFIC2, searchdata.specific2);
    DDX_Text(pDX, IDC_GENDER, searchdata.gender);
    DDX_Text(pDX, IDC_GENDER2, searchdata.gender2);
    DDX_Text(pDX, IDC_ALIGN, searchdata.align);
    DDX_Text(pDX, IDC_ALIGN2, searchdata.align2);
    DDX_Text(pDX, IDC_STRREF, searchdata.strref);
    DDX_Text(pDX, IDC_NEWVALUE, searchdata.newvalue);
  }
}

BEGIN_MESSAGE_MAP(CFindItem, CDialog)
	//{{AFX_MSG_MAP(CFindItem)
	ON_BN_CLICKED(IDC_MPROJ, OnMproj)
	ON_BN_CLICKED(IDC_MTYPE, OnMtype)
	ON_EN_KILLFOCUS(IDC_ITEMTYPE, OnKillfocusItemtype)
	ON_EN_KILLFOCUS(IDC_ITEMTYPE2, OnKillfocusItemtype2)
	ON_EN_KILLFOCUS(IDC_PROJ, OnKillfocusProj)
	ON_EN_KILLFOCUS(IDC_PROJ2, OnKillfocusProj2)
	ON_EN_KILLFOCUS(IDC_EFFECT, OnKillfocusEffect)
	ON_EN_KILLFOCUS(IDC_EFFECT2, OnKillfocusEffect2)
	ON_BN_CLICKED(IDC_MFEAT, OnMfeat)
	ON_BN_CLICKED(IDC_MRES, OnMres)
	ON_EN_KILLFOCUS(IDC_RESOURCE, OnKillfocusResource)
	ON_BN_CLICKED(IDC_MITEM, OnMitem)
	ON_EN_KILLFOCUS(IDC_ITEMNAME, OnKillfocusItemname)
	ON_EN_KILLFOCUS(IDC_EA, OnKillfocusEa)
	ON_EN_KILLFOCUS(IDC_EA2, OnKillfocusEa2)
	ON_EN_KILLFOCUS(IDC_GENERAL, OnKillfocusGeneral)
	ON_EN_KILLFOCUS(IDC_GENERAL2, OnKillfocusGeneral2)
	ON_EN_KILLFOCUS(IDC_RACE, OnKillfocusRace)
	ON_EN_KILLFOCUS(IDC_RACE2, OnKillfocusRace2)
	ON_EN_KILLFOCUS(IDC_CLASS, OnKillfocusClass)
	ON_EN_KILLFOCUS(IDC_CLASS2, OnKillfocusClass2)
	ON_EN_KILLFOCUS(IDC_SPECIFIC, OnKillfocusSpecific)
	ON_EN_KILLFOCUS(IDC_SPECIFIC2, OnKillfocusSpecific2)
	ON_EN_KILLFOCUS(IDC_GENDER, OnKillfocusGender)
	ON_EN_KILLFOCUS(IDC_GENDER2, OnKillfocusGender2)
	ON_EN_KILLFOCUS(IDC_ALIGN, OnKillfocusAlign)
	ON_EN_KILLFOCUS(IDC_ALIGN2, OnKillfocusAlign2)
	ON_BN_CLICKED(IDC_MEA, OnMea)
	ON_BN_CLICKED(IDC_MGENERAL, OnMgeneral)
	ON_BN_CLICKED(IDC_MRACE, OnMrace)
	ON_BN_CLICKED(IDC_MCLASS, OnMclass)
	ON_BN_CLICKED(IDC_MSPECIFIC, OnMspecific)
	ON_BN_CLICKED(IDC_MGENDER, OnMgender)
	ON_BN_CLICKED(IDC_MALIGN, OnMalign)
	ON_EN_KILLFOCUS(IDC_VARIABLE, OnKillfocusVariable)
	ON_BN_CLICKED(IDC_MVAR, OnMvar)
	ON_CBN_KILLFOCUS(IDC_CHANGE, OnKillfocusChange)
	ON_EN_KILLFOCUS(IDC_NEWVALUE, OnKillfocusNewvalue)
	ON_BN_CLICKED(IDC_REPLACE, OnReplace)
	ON_EN_KILLFOCUS(IDC_STRREF, OnKillfocusStrref)
	ON_BN_CLICKED(IDC_MSTRREF, OnMstrref)
	ON_EN_KILLFOCUS(IDC_NEWRESOURCE, OnKillfocusNewresource)
	ON_CBN_SELCHANGE(IDC_CHANGE, OnSelchangeChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static int fieldlist[]={-1, IDC_MTYPE, -1, IDC_MRES,
-1, IDC_MEA, IDC_MGENERAL, IDC_MRACE, IDC_MCLASS, 
IDC_MSPECIFIC, IDC_MGENDER, IDC_MALIGN, -1,-1,
0};

static int DialogRefs[32]=
{
  IDC_MPROJ, IDC_MPROJ, IDC_MTYPE, IDC_MTYPE,
  IDC_MFEAT, IDC_MFEAT, IDC_MRES, IDC_MITEM,
  IDC_MEA, IDC_MEA, IDC_MGENERAL, IDC_MGENERAL,
  IDC_MRACE, IDC_MRACE, IDC_MCLASS, IDC_MCLASS,
  IDC_MSPECIFIC, IDC_MSPECIFIC, IDC_MGENDER, IDC_MGENDER,
  IDC_MALIGN, IDC_MALIGN, IDC_MVAR, IDC_MSTRREF,
  -1,-1,-1,-1,
  -1,-1,IDC_REPLACE, IDC_REPLACE,
};

static int EditRefs[32]=
{
  IDC_PROJ, IDC_PROJ2, IDC_ITEMTYPE, IDC_ITEMTYPE2,
  IDC_EFFECT, IDC_EFFECT2, IDC_RESOURCE, IDC_ITEMNAME,
  IDC_EA, IDC_EA2, IDC_GENERAL, IDC_GENERAL2, 
  IDC_RACE, IDC_RACE2, IDC_CLASS, IDC_CLASS2,
  IDC_SPECIFIC, IDC_SPECIFIC2, IDC_GENDER, IDC_GENDER2,
  IDC_ALIGN, IDC_ALIGN2, IDC_VARIABLE, IDC_STRREF,
  -1,-1,-1,-1,
  -1,-1,IDC_CHANGE, IDC_NEWVALUE,
};

/////////////////////////////////////////////////////////////////////////////
// CFindItem message handlers
void CFindItem::Refresh()
{
  int i;
  unsigned int j;
  CButton *checkbox;
  CEdit *edit;
  int set, show;

  j=1;
	for(i=0;i<32;i++)
  {
    show=!!(mask&j);
    set=!!(flags&j);
    checkbox=(class CButton *) GetDlgItem(DialogRefs[i]);
    if(checkbox)
    {
      if(show)
      {
        checkbox->SetCheck(set);
      }
      else
      {
        checkbox->EnableWindow(show);
      }
    }
    edit=(class CEdit *) GetDlgItem(EditRefs[i]);
    if(edit)
    {
      if(show)
      {
        if(EditRefs[i]==IDC_NEWVALUE)
        {
          edit->EnableWindow(set && (searchdata.which!=FLG_MRES));
          edit->ShowWindow(searchdata.which!=FLG_MRES);
          edit=(class CEdit *) GetDlgItem(IDC_NEWRESOURCE);
          edit->EnableWindow(set && (searchdata.which==FLG_MRES));
          edit->ShowWindow(searchdata.which==FLG_MRES);
        }
        else
        {
          edit->EnableWindow(set);
        }
      }
      else
      {
        edit->EnableWindow(0);
        if(EditRefs[i]==IDC_NEWVALUE)
        {
          edit->ShowWindow(searchdata.which!=FLG_MRES);
          edit=(class CEdit *) GetDlgItem(IDC_NEWRESOURCE);
          edit->EnableWindow(0);
          edit->ShowWindow(searchdata.which==FLG_MRES);
        }
      }
    }
    j<<=1;
  }
}


BOOL CFindItem::OnInitDialog() 
{
  CWnd *cb;
  CString tmpstr,field;
  int i,j;

	CDialog::OnInitDialog();
  Refresh();
  SetWindowText(title);
  m_mtype_control.SetWindowText(mtype_title);
  if(proj_title.GetLength())
  {
    m_proj_control.SetWindowText(proj_title);
  }

  //well this is a weird piece of code,
  //i don't even want to talk about it
  for(i=0;fieldlist[i];i++)
  {
    cb=GetDlgItem(fieldlist[i]);
    if(cb && cb->IsWindowEnabled())
    {
      cb->GetWindowText(field);
      field.Replace("Match ","");
      for(j=0;j<32;j++)
      {
        if(DialogRefs[j]==fieldlist[i])
        {
          tmpstr.Format("%d-%s",j,field);
          m_change_control.AddString(tmpstr);
          break;
        }
      }
    }
  }
  tmpstr.Format("%d",searchdata.which);
  m_change_control.SelectString(0,tmpstr);
  //end of weird code :)
  m_resource=CString(searchdata.resource,8);
  m_itemname=CString(searchdata.itemname,8);
  m_variable=CString(searchdata.variable,32);
  m_newres=CString(searchdata.newresource,8);
  UpdateData(UD_DISPLAY);
	return TRUE;
}

void CFindItem::OnKillfocusItemtype() 
{
  UpdateData(UD_RETRIEVE);
  if(searchdata.itemtype2<searchdata.itemtype)
  {
    searchdata.itemtype2=searchdata.itemtype;
  }
  UpdateData(UD_DISPLAY);
}

void CFindItem::OnKillfocusItemtype2() 
{
  UpdateData(UD_RETRIEVE);
  if(searchdata.itemtype2<searchdata.itemtype)
  {
    searchdata.itemtype=searchdata.itemtype2;
  }
  UpdateData(UD_DISPLAY);
}

void CFindItem::OnKillfocusProj() 
{
  UpdateData(UD_RETRIEVE);
  if(searchdata.projectile2<searchdata.projectile)
  {
    searchdata.projectile2=searchdata.projectile;
  }
  UpdateData(UD_DISPLAY);
}

void CFindItem::OnKillfocusProj2() 
{
  UpdateData(UD_RETRIEVE);
  if(searchdata.projectile2<searchdata.projectile)
  {
    searchdata.projectile=searchdata.projectile2;
  }
  UpdateData(UD_DISPLAY);
}

void CFindItem::OnKillfocusEffect() 
{
  UpdateData(UD_RETRIEVE);
  if(searchdata.feature2<searchdata.feature)
  {
    searchdata.feature2=searchdata.feature;
  }
  UpdateData(UD_DISPLAY);
}

void CFindItem::OnKillfocusEffect2() 
{
  UpdateData(UD_RETRIEVE);
  if(searchdata.feature2<searchdata.feature)
  {
    searchdata.feature=searchdata.feature2;
  }
  UpdateData(UD_DISPLAY);
}

void CFindItem::OnKillfocusResource() 
{
  UpdateData(UD_RETRIEVE);  
  StoreResref(m_resource,searchdata.resource);
  UpdateData(UD_DISPLAY);
}

void CFindItem::OnKillfocusItemname() 
{
  UpdateData(UD_RETRIEVE);  
  StoreResref(m_itemname,searchdata.itemname);
  UpdateData(UD_DISPLAY);
}

void CFindItem::OnKillfocusEa() 
{
  UpdateData(UD_RETRIEVE);
  if(searchdata.ea2<searchdata.ea)
  {
    searchdata.ea2=searchdata.ea;
  }
  UpdateData(UD_DISPLAY);
}

void CFindItem::OnKillfocusEa2() 
{
  UpdateData(UD_RETRIEVE);
  if(searchdata.ea2<searchdata.ea)
  {
    searchdata.ea=searchdata.ea2;
  }
  UpdateData(UD_DISPLAY);
}

void CFindItem::OnKillfocusGeneral() 
{
  UpdateData(UD_RETRIEVE);
  if(searchdata.general2<searchdata.general)
  {
    searchdata.general2=searchdata.general;
  }
  UpdateData(UD_DISPLAY);
}

void CFindItem::OnKillfocusGeneral2() 
{
  UpdateData(UD_RETRIEVE);
  if(searchdata.general2<searchdata.general)
  {
    searchdata.general=searchdata.general2;
  }
  UpdateData(UD_DISPLAY);
}

void CFindItem::OnKillfocusRace() 
{
  UpdateData(UD_RETRIEVE);
  if(searchdata.race2<searchdata.race)
  {
    searchdata.race2=searchdata.race;
  }
  UpdateData(UD_DISPLAY);
}

void CFindItem::OnKillfocusRace2() 
{
  UpdateData(UD_RETRIEVE);
  if(searchdata.race2<searchdata.race)
  {
    searchdata.race=searchdata.race2;
  }
  UpdateData(UD_DISPLAY);
}

void CFindItem::OnKillfocusClass() 
{
  UpdateData(UD_RETRIEVE);
  if(searchdata.class2<searchdata.class1)
  {
    searchdata.class2=searchdata.class1;
  }
  UpdateData(UD_DISPLAY);
}

void CFindItem::OnKillfocusClass2() 
{
  UpdateData(UD_RETRIEVE);
  if(searchdata.class1<searchdata.class2)
  {
    searchdata.class2=searchdata.class1;
  }
  UpdateData(UD_DISPLAY);
}

void CFindItem::OnKillfocusSpecific() 
{
  UpdateData(UD_RETRIEVE);
  if(searchdata.specific2<searchdata.specific)
  {
    searchdata.specific2=searchdata.specific;
  }
  UpdateData(UD_DISPLAY);
}

void CFindItem::OnKillfocusSpecific2() 
{
  UpdateData(UD_RETRIEVE);
  if(searchdata.specific2<searchdata.specific)
  {
    searchdata.specific=searchdata.specific2;
  }
  UpdateData(UD_DISPLAY);
}

void CFindItem::OnKillfocusGender() 
{
  UpdateData(UD_RETRIEVE);
  if(searchdata.gender2<searchdata.gender)
  {
    searchdata.gender2=searchdata.gender;
  }
  UpdateData(UD_DISPLAY);
}

void CFindItem::OnKillfocusGender2() 
{
  UpdateData(UD_RETRIEVE);
  if(searchdata.gender2<searchdata.gender)
  {
    searchdata.gender=searchdata.gender2;
  }
  UpdateData(UD_DISPLAY);
}

void CFindItem::OnKillfocusAlign() 
{
  UpdateData(UD_RETRIEVE);
  if(searchdata.align2<searchdata.align)
  {
    searchdata.align2=searchdata.align;
  }
  UpdateData(UD_DISPLAY);
}

void CFindItem::OnKillfocusAlign2() 
{
  UpdateData(UD_RETRIEVE);
  if(searchdata.align2<searchdata.align)
  {
    searchdata.align=searchdata.align2;
  }
  UpdateData(UD_DISPLAY);
}

void CFindItem::OnKillfocusVariable() 
{
  UpdateData(UD_RETRIEVE);  
  StoreVariable(m_variable,searchdata.variable);
  UpdateData(UD_DISPLAY);
}

void CFindItem::OnMproj() 
{
	flags^=1<<FLG_MPROJ;
	flags^=1<<FLG_MPROJ2;
  Refresh();
}

void CFindItem::OnMtype() 
{
	flags^=1<<FLG_MTYPE;
	flags^=1<<FLG_MTYPE2;
  Refresh();
}

void CFindItem::OnMfeat() 
{
	flags^=1<<FLG_MFEAT;
	flags^=1<<FLG_MFEAT2;
  Refresh();
}

void CFindItem::OnMres() 
{
	flags^=1<<FLG_MRES;
  Refresh();
}

void CFindItem::OnMitem() 
{
	flags^=1<<FLG_MITEM;
  Refresh();
}
void CFindItem::OnMea() 
{
	flags^=1<<FLG_MEA;
	flags^=1<<FLG_MEA2;
  Refresh();
}

void CFindItem::OnMgeneral() 
{
	flags^=1<<FLG_MGENERAL;
	flags^=1<<FLG_MGENERAL2;
  Refresh();
}

void CFindItem::OnMrace() 
{
	flags^=1<<FLG_MRACE;
	flags^=1<<FLG_MRACE2;
  Refresh();
}

void CFindItem::OnMclass() 
{
	flags^=1<<FLG_MCLASS;
	flags^=1<<FLG_MCLASS2;
  Refresh();
}

void CFindItem::OnMspecific() 
{
	flags^=1<<FLG_MSPECIFIC;
	flags^=1<<FLG_MSPECIFIC2;
  Refresh();
}

void CFindItem::OnMgender() 
{
	flags^=1<<FLG_MGENDER;
	flags^=1<<FLG_MGENDER2;
  Refresh();
}

void CFindItem::OnMalign() 
{
	flags^=1<<FLG_MALIGN;
	flags^=1<<FLG_MALIGN2;
  Refresh();
}

void CFindItem::OnMvar() 
{
	flags^=1<<FLG_MVAR;
	Refresh();
}

void CFindItem::OnMstrref() 
{
  flags^=1<<FLG_MSTRREF;
	Refresh();
}

void CFindItem::OnReplace() 
{
  flags^=1<<FLG_CHANGE;
  flags^=1<<FLG_CHANGE2;
  Refresh();
}

void CFindItem::OnKillfocusStrref() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CFindItem::OnKillfocusChange() 
{
  UpdateData(UD_RETRIEVE);
  Refresh();
  UpdateData(UD_DISPLAY);
}

void CFindItem::OnSelchangeChange() 
{
  UpdateData(UD_RETRIEVE);
  Refresh();
  UpdateData(UD_DISPLAY);
}

void CFindItem::OnKillfocusNewvalue() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CFindItem::OnKillfocusNewresource() 
{
  UpdateData(UD_RETRIEVE);
  StoreResref(m_newres, searchdata.newresource);
  UpdateData(UD_DISPLAY);
}
///////////////////////////////////////////////////////////////////////////////////////


