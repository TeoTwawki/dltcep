// VEFEdit.cpp : implementation file
//

#include "stdafx.h"

#include <fcntl.h>
#include <sys/stat.h>
#include "chitem.h"
#include "chitemDlg.h"
#include "VEFEdit.h"
#include "MyFileDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVEFEdit dialog

CVEFEdit::CVEFEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CVEFEdit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CVEFEdit)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	group1num = 0;
  group2num = 0;
}

int group1boxids[] = {IDC_REMOVE, IDC_RESOURCE, IDC_START, 
 IDC_UNUSED, IDC_LENGTH, IDC_TYPE, IDC_CONTINUOUS, 0};
int group2boxids[] = {IDC_REMOVE2, IDC_RESOURCE2, IDC_START2,
 IDC_UNUSED2, IDC_LENGTH2, IDC_TYPE2, IDC_CONTINUOUS2, 0};

void CVEFEdit::DoDataExchange(CDataExchange* pDX)
{
  CWnd *cb;
  CButton *cbox;
  vef_header tmp;
  CString tmpstr, tmpstr2;
  int i;
  int flg;

  memcpy(&tmp,&the_vef.header,sizeof(vef_header));
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVEFEdit)
	DDX_Control(pDX, IDC_TYPE2, m_type2_control);
	DDX_Control(pDX, IDC_TYPE, m_type1_control);
	DDX_Control(pDX, IDC_GROUP2, m_group2_control);
	DDX_Control(pDX, IDC_GROUP1, m_group1_control);
	//}}AFX_DATA_MAP

  RefreshDialog();

  flg = the_vef.comp1count;
  i = 0;
  while(group1boxids[i])
  {  
    cb = GetDlgItem(group1boxids[i++]);
    cb->EnableWindow(flg);
  }

  flg=(flg && group1num>=0);
  if(flg)
  {
    RetrieveResref(tmpstr, the_vef.comp1[group1num].resref);
    tmpstr2=tmpstr;
    DDX_Text(pDX, IDC_RESOURCE, tmpstr);
    DDV_MaxChars(pDX, tmpstr, 8);
    StoreResref(tmpstr, the_vef.comp1[group1num].resref);

    DDX_Text(pDX, IDC_START, the_vef.comp1[group1num].ticks);
    DDX_Text(pDX, IDC_UNUSED, the_vef.comp1[group1num].unknown);
    DDX_Text(pDX, IDC_LENGTH, the_vef.comp1[group1num].length);
    if (pDX->m_bSaveAndValidate==UD_DISPLAY)
    {
      i = the_vef.comp1[group1num].type;
      m_type1_control.SetCurSel(i);
    }
    else
    {
      DDX_Text(pDX, IDC_TYPE, tmpstr);
      the_vef.comp1[group1num].type=atoi(tmpstr);
    }

    cbox = (CButton *) GetDlgItem(IDC_CONTINUOUS);
    cbox->SetCheck(the_vef.comp1[group1num].cycles);
  }

  flg = the_vef.comp2count;
  i = 0;
  while(group2boxids[i])
  {
    cb = GetDlgItem(group2boxids[i++]);
    cb->EnableWindow(flg);
  }

  flg=(flg && group2num>=0);
  if(flg)
  {
    RetrieveResref(tmpstr, the_vef.comp2[group2num].resref);
    tmpstr2=tmpstr;
    DDX_Text(pDX, IDC_RESOURCE2, tmpstr);
    DDV_MaxChars(pDX, tmpstr, 8);
    StoreResref(tmpstr, the_vef.comp2[group2num].resref);

    DDX_Text(pDX, IDC_START2, the_vef.comp2[group2num].ticks);
    DDX_Text(pDX, IDC_UNUSED2, the_vef.comp2[group2num].unknown);
    DDX_Text(pDX, IDC_LENGTH2, the_vef.comp2[group2num].length);
    if (pDX->m_bSaveAndValidate==UD_DISPLAY)
    {
      i = the_vef.comp2[group2num].type;
      m_type2_control.SetCurSel(i);
    }
    else
    {
      DDX_Text(pDX, IDC_TYPE2, tmpstr);
      the_vef.comp2[group2num].type=atoi(tmpstr);
    }

    cbox = (CButton *) GetDlgItem(IDC_CONTINUOUS2);
    cbox->SetCheck(the_vef.comp2[group2num].cycles);
  }
  if(memcmp(&tmp,&the_vef.header,sizeof(vef_header)))
  {
    the_vef.m_changed=true;
  }
}

void CVEFEdit::NewVEF()
{
	itemname="new video effect";
  the_vef.KillComp1Headers();
  the_vef.KillComp2Headers();
  memset(&the_vef.header,0,sizeof(vef_header));
  the_vef.m_changed=false;
}

BEGIN_MESSAGE_MAP(CVEFEdit, CDialog)
	//{{AFX_MSG_MAP(CVEFEdit)
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_BN_CLICKED(IDC_LOADEX, OnLoadex)
	ON_BN_CLICKED(IDC_SAVEAS, OnSaveas)
	ON_BN_CLICKED(IDC_NEW, OnNew)
	ON_BN_CLICKED(IDC_CHECK, OnCheck)
	ON_COMMAND(ID_FILE_SAVE, OnSave)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_REMOVE2, OnRemove2)
	ON_BN_CLICKED(IDC_ADD2, OnAdd2)
	ON_EN_KILLFOCUS(IDC_START, OnDefaultKillfocus)
	ON_CBN_KILLFOCUS(IDC_GROUP1, OnKillfocusGroup1)
	ON_CBN_SELCHANGE(IDC_GROUP1, OnSelchangeGroup1)
	ON_CBN_KILLFOCUS(IDC_GROUP2, OnKillfocusGroup2)
	ON_CBN_SELCHANGE(IDC_GROUP2, OnSelchangeGroup2)
	ON_BN_CLICKED(IDC_CONTINUOUS, OnContinuous)
	ON_COMMAND(ID_FILE_NEW, OnNew)
	ON_COMMAND(ID_FILE_LOAD, OnLoad)
	ON_COMMAND(ID_FILE_LOADEXTERNAL, OnLoadex)
	ON_COMMAND(ID_CHECK, OnCheck)
	ON_COMMAND(ID_FILE_SAVEAS, OnSaveas)
	ON_EN_KILLFOCUS(IDC_UNUSED, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_LENGTH, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_RESOURCE, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_RESOURCE2, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_START2, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_UNUSED2, OnDefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_LENGTH2, OnDefaultKillfocus)
	ON_CBN_KILLFOCUS(IDC_TYPE, OnDefaultKillfocus)
	ON_CBN_KILLFOCUS(IDC_TYPE2, OnDefaultKillfocus)
	ON_BN_CLICKED(IDC_CONTINUOUS2, OnContinuous2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVEFEdit message handlers

void CVEFEdit::OnLoad() 
{
	int res;
	
  pickerdlg.m_restype=REF_VEF;
  pickerdlg.m_picked=itemname;
	res=pickerdlg.DoModal();
	if(res==IDOK)
	{
		res=read_vef(pickerdlg.m_picked);
    switch(res)
    {
    case 0:
  		itemname=pickerdlg.m_picked;
      break;
    default:
      MessageBox("Cannot read video effect!","Error",MB_ICONSTOP|MB_OK);
      NewVEF();
      break;
    }
	}
  UpdateData(UD_DISPLAY);
}

static char BASED_CODE szFilter[] = "Video Effect files (*.vef)|*.vef|All files (*.*)|*.*||";

void CVEFEdit::OnLoadex() 
{
  CString filepath;
  int fhandle;
  int res;
  
  res=OFN_FILEMUSTEXIST|OFN_ENABLESIZING|OFN_EXPLORER;
  if(readonly) res|=OFN_READONLY;  
  CMyFileDialog m_getfiledlg(TRUE, "vef", makeitemname(".vef",0), res, szFilter);

restart:  
  if( m_getfiledlg.DoModal() == IDOK )
  {
    filepath=m_getfiledlg.GetPathName();
    fhandle=open(filepath, O_RDONLY|O_BINARY);
    if(!fhandle)
    {
      MessageBox("Cannot open file!","Error",MB_ICONSTOP|MB_OK);
      goto restart;
    }
    readonly=m_getfiledlg.GetReadOnlyPref();
    res=the_vef.ReadVEFFromFile(fhandle,-1);
    close(fhandle);
    lastopenedoverride=filepath.Left(filepath.ReverseFind('\\'));
    switch(res)
    {
    case 0:
      itemname=m_getfiledlg.GetFileTitle();
      itemname.MakeUpper();
      break;
    default:
      MessageBox("Cannot read video effect!","Error",MB_ICONSTOP|MB_OK);
      NewVEF();
      break;
    }
  }
  UpdateData(UD_DISPLAY);
}

void CVEFEdit::OnNew() 
{
	NewVEF();
  UpdateData(UD_DISPLAY);
}

void CVEFEdit::OnSaveas() 
{
  SaveVEF(0);
}

void CVEFEdit::OnSave() 
{
  SaveVEF(1);
}

void CVEFEdit::SaveVEF(int save) 
{
  CString filepath;
  CString newname;
  CString tmpstr;
  int res;

  if(readonly)
  {
    MessageBox("You opened it read only!","Warning",MB_ICONEXCLAMATION|MB_OK);
    return;
  }
  res=OFN_HIDEREADONLY|OFN_ENABLESIZING|OFN_EXPLORER;
  CMyFileDialog m_getfiledlg(FALSE, "vef", makeitemname(".vef",0), res, szFilter);

  if(save)
  {
    newname=itemname;
    filepath=makeitemname(".vef",0);
    goto gotname;
  }    
  
restart:  
  if( m_getfiledlg.DoModal() == IDOK )
  {
    filepath=m_getfiledlg.GetPathName();
    filepath.MakeLower();
    if(filepath.Right(4)!=".vef")
    {
      filepath+=".vef";
    }
    newname=m_getfiledlg.GetFileName();
    newname.MakeUpper();
    if(newname.Right(4)==".VEF") newname=newname.Left(newname.GetLength()-4);
gotname:
    if(newname.GetLength()>8 || newname.GetLength()<1 || newname.Find(" ",0)!=-1)
    {
      tmpstr.Format("The resource name '%s' is bad, it should be 8 characters long and without spaces.",newname);
      MessageBox(tmpstr,"Warning",MB_ICONEXCLAMATION|MB_OK);
      goto restart;
    }
    if(newname!=itemname && file_exists(filepath) )
    {
      res=MessageBox("Do you want to overwrite "+newname+"?","Warning",MB_ICONQUESTION|MB_YESNO);
      if(res==IDNO) goto restart;
    }
    
    res = write_vef(newname, filepath);
    lastopenedoverride=filepath.Left(filepath.ReverseFind('\\'));
    switch(res)
    {
    case 0:
      itemname=newname;
      break; //saved successfully
    case -2:
      MessageBox("Error while writing file!","Error",MB_ICONSTOP|MB_OK);
      break;
    case -3:
      MessageBox("Internal Error!","Error",MB_ICONSTOP|MB_OK);
      break;
    default:
      MessageBox("Unhandled error!","Error",MB_ICONSTOP|MB_OK);
    }
  }
  UpdateData(UD_DISPLAY);
}

void CVEFEdit::OnCheck() 
{
  int ret;

	ret=((CChitemDlg *) AfxGetMainWnd())->check_vef();
  if(ret)
  {
    MessageBox(lasterrormsg,"VEF editor",MB_ICONEXCLAMATION|MB_OK);
  }
}
  
void CVEFEdit::RefreshDialog()
{
  CString tmpstr;
  int i;

  SetWindowText("Edit video effect: "+itemname);

  if(m_group1_control)
  {
    m_group1_control.ResetContent();
    for(i=0;i<the_vef.comp1count;i++)
    {
      tmpstr.Format("%d %-.8s %d-%d",i+1,the_vef.comp1[i].resref,
        the_vef.comp1[i].ticks, the_vef.comp1[i].length);
      m_group1_control.AddString(tmpstr);
    }
    //m_maxoverlay.Format("/ %d",i);
    if(group1num<0) group1num=0;
    group1num=m_group1_control.SetCurSel(group1num);
  }

  if(m_group2_control)
  {
    m_group2_control.ResetContent();
    for(i=0;i<the_vef.comp2count;i++)
    {
      tmpstr.Format("%d %-.8s %d-%d",i+1,the_vef.comp2[i].resref,
        the_vef.comp2[i].ticks, the_vef.comp2[i].length);
      m_group2_control.AddString(tmpstr);
    }
    //m_maxoverlay.Format("/ %d",i);
    if(group2num<0) group2num=0;
    group2num=m_group2_control.SetCurSel(group2num);
  }

}

BOOL CVEFEdit::OnInitDialog() 
{
  CString tmpstr, tmpstr1, tmpstr2;

	CDialog::OnInitDialog();
  //tooltips
  {
    m_tooltip.Create(this,TTS_NOPREFIX);
    m_tooltip.SetMaxTipWidth(200);
    m_tooltip.SetTipBkColor(RGB(240,224,160));
    
    m_tooltip.AddTool(GetDlgItem(IDCANCEL), IDS_CANCEL);
    tmpstr1.LoadString(IDS_LOAD);
    tmpstr2.LoadString(IDS_VEF);
    tmpstr.Format(tmpstr1, tmpstr2);
    m_tooltip.AddTool(GetDlgItem(IDC_LOAD), tmpstr);
    tmpstr1.LoadString(IDS_LOADEX);
    tmpstr.Format(tmpstr1, tmpstr2);
    m_tooltip.AddTool(GetDlgItem(IDC_LOADEX), tmpstr);
    tmpstr1.LoadString(IDS_SAVE);
    tmpstr.Format(tmpstr1, tmpstr2);
    m_tooltip.AddTool(GetDlgItem(IDC_SAVEAS), tmpstr);
    tmpstr1.LoadString(IDS_NEW);
    tmpstr.Format(tmpstr1, tmpstr2);
    m_tooltip.AddTool(GetDlgItem(IDC_NEW), tmpstr);
    tmpstr1.LoadString(IDS_CHECK);
    tmpstr.Format(tmpstr1, tmpstr2);
    m_tooltip.AddTool(GetDlgItem(IDC_CHECK), tmpstr);
  }	
  UpdateData(UD_DISPLAY);
	return TRUE;
}

void CVEFEdit::OnDefaultKillfocus() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CVEFEdit::OnCancel() 
{
  CString tmpstr;

  if(the_vef.m_changed)
  {
    tmpstr.Format("Changes have been made to the video effect.\n"
      "Do you want to quit without save?\n");
    if(MessageBox(tmpstr,"Warning",MB_YESNO)==IDNO)
    {
      return;
    }
  }
	CDialog::OnCancel();
}

int CVEFEdit::DeleteComponent(vef_component *&oldcomp, int pos, int &max)
{
  vef_component *component = (vef_component *) malloc( (max-1)*sizeof(vef_component) );
  if (!component) return -3;
  memcpy(component, oldcomp, pos * sizeof(vef_component) );
  max--;
  memcpy(component+pos, oldcomp+pos+1, (max-pos)*sizeof(vef_component) );
  delete [] oldcomp;
  oldcomp = component;
  return 0;
}

int CVEFEdit::InsertComponent(vef_component *&oldcomp, int pos, int &max)
{
  vef_component *component = (vef_component *) malloc( (max+1)*sizeof(vef_component) );
  if (!component) return -3;
  memcpy(component, oldcomp, pos * sizeof(vef_component) );
  memset(component+pos, 0, sizeof(vef_component) );
  memcpy(component+pos+1, oldcomp+pos, (max-pos)*sizeof(vef_component) );
  max++;
  delete [] oldcomp;
  oldcomp = component;
  return 0;
}

void CVEFEdit::OnRemove() 
{
  int deletepos;
  int res;

  group1num=m_group1_control.GetCurSel();
  if(group1num<0) return;
  if(group1num>=the_vef.comp1count) return;
  deletepos=group1num;
	res = DeleteComponent(the_vef.comp1, deletepos, the_vef.comp1count);
  if (res<0)
  {
    MessageBox("Not enough memory.","Error",MB_ICONSTOP|MB_OK);
    return;
  }
  if(group1num>=the_vef.comp1count) group1num--;
  UpdateData(UD_DISPLAY);
}

void CVEFEdit::OnAdd() 
{
  int insertpos;
  int res;

  group1num=m_group1_control.GetCurSel()+1;
  insertpos = group1num;
  res = InsertComponent(the_vef.comp1, insertpos, the_vef.comp1count);
  if (res<0)
  {
    MessageBox("Not enough memory.","Error",MB_ICONSTOP|MB_OK);
    return;
  }
  if(group2num>=the_vef.comp2count) group2num--;
  UpdateData(UD_DISPLAY);
}

void CVEFEdit::OnRemove2() 
{
  int deletepos;
  int res;

  group2num=m_group2_control.GetCurSel();
  if(group2num<0) return;
  deletepos=group2num;
  res = DeleteComponent(the_vef.comp2, deletepos, the_vef.comp2count);
  if (res<0)
  {
    MessageBox("Not enough memory.","Error",MB_ICONSTOP|MB_OK);
  }
  UpdateData(UD_DISPLAY);
}

void CVEFEdit::OnAdd2() 
{
  int insertpos;
  int res;

  group2num=m_group2_control.GetCurSel()+1;
  insertpos = group2num;
  res = InsertComponent(the_vef.comp2, insertpos, the_vef.comp2count);
  if (res<0)
  {
    MessageBox("Not enough memory.","Error",MB_ICONSTOP|MB_OK);
  }
  UpdateData(UD_DISPLAY);
}

void CVEFEdit::OnKillfocusGroup1() 
{
  CString tmpstr;
  int x,max;

  m_group1_control.GetWindowText(tmpstr);
  x=strtonum(tmpstr)-1;
  max=the_vef.comp1count;
  if(x>=0 && x<=max)
  {
    group1num=m_group1_control.SetCurSel(x);
  }
	UpdateData(UD_DISPLAY);	
}

void CVEFEdit::OnSelchangeGroup1() 
{
  int x,max;

  x=m_group1_control.GetCurSel();
  max=the_vef.comp1count;
  if(x>=0 && x<=max) group1num=x;
	UpdateData(UD_DISPLAY);	
}

void CVEFEdit::OnKillfocusGroup2() 
{
  CString tmpstr;
  int x,max;

  m_group2_control.GetWindowText(tmpstr);
  x=strtonum(tmpstr)-1;
  max=the_vef.comp2count;
  if(x>=0 && x<=max)
  {
    group2num=m_group2_control.SetCurSel(x);
  }
	UpdateData(UD_DISPLAY);	
}

void CVEFEdit::OnSelchangeGroup2() 
{
  int x,max;

  x=m_group2_control.GetCurSel();
  max=the_vef.comp2count;
  if(x>=0 && x<=max) group2num=x;
	UpdateData(UD_DISPLAY);	
}

void CVEFEdit::OnContinuous() 
{
  group1num=m_group1_control.GetCurSel();
	the_vef.comp1[group1num].cycles=!the_vef.comp1[group1num].cycles;
  UpdateData(UD_DISPLAY);
}

void CVEFEdit::OnContinuous2() 
{
  group2num=m_group1_control.GetCurSel();
	the_vef.comp2[group1num].cycles=!the_vef.comp2[group2num].cycles;
  UpdateData(UD_DISPLAY);
}

BOOL CVEFEdit::PreTranslateMessage(MSG* pMsg) 
{
  m_tooltip.RelayEvent(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}
