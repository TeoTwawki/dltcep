// ProjEdit.cpp : implementation file
//

#include "stdafx.h"

#include <fcntl.h>
#include <sys/stat.h>
#include "chitem.h"
#include "chitemDlg.h"
#include "ProjEdit.h"
#include "ProjgemRB.h"
#include "ColorPicker.h"
#include "tbg.h"
#include "MyFileDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProjEdit dialog


CProjEdit::CProjEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CProjEdit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProjEdit)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
  m_smokecoloridx=m_bamcoloridx=0;
  hbs=hbb=0;
}

static int sparkboxids[]={ IDC_SPARKCOLOUR, IDC_UNKNOWN0E, IDC_UNKNOWN2A, 0};
static int bamcolorids[]={IDC_MPTRAVEL, IDC_U1, IDC_U2, IDC_U3,
IDC_U4, IDC_U5, IDC_U6, IDC_U7, 0};
static int smokeboxids[]={IDC_DELAY, IDC_MPSMOKE, IDC_SMOKETYPE,
IDC_U11, IDC_U12, IDC_U13, IDC_U14, IDC_U15, IDC_U16, IDC_U17, 0};
static int shadowids[]={IDC_BAM2, IDC_BROWSE5, IDC_SEQ2, 0};
static int lightspotids[]={IDC_X, IDC_Y, IDC_Z,0};

int *headercontrols[16]={bamcolorids, smokeboxids,0,0,0,shadowids, lightspotids, 0,
0,0,0,0,0,0,0,0};

void CProjEdit::DoDataExchange(CDataExchange* pDX)
{
  proj_header tmp;
  CButton *cb;
  CWnd *cw;
  CString tmpstr;
  int i,j,k;
  int flg;

  memcpy(&tmp,&the_projectile.header,sizeof(proj_header));
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProjEdit)
	DDX_Control(pDX, IDC_MPSMOKE, m_mpsmoke);
	DDX_Control(pDX, IDC_MPTRAVEL, m_mptravel);
	//}}AFX_DATA_MAP
  switch(the_projectile.header.type)
  {
  case 2: tmpstr="0x2 One target"; break;
  case 3: tmpstr="0x3 Area effect"; break;
  default: tmpstr.Format("0x%x Unknown",the_projectile.header.type); break;
  }
  DDX_Text(pDX, IDC_TYPE, tmpstr);
  the_projectile.header.type=(short) strtonum(tmpstr);

  DDX_Text(pDX, IDC_SPEED, the_projectile.header.speed);
  DDX_Text(pDX, IDC_FLAGS, the_projectile.header.sparks);

  DDX_Text(pDX, IDC_UNKNOWN0E, the_projectile.header.unknown0e);
  DDX_Text(pDX, IDC_UNKNOWN2A, the_projectile.header.spknumber);

  RetrieveResref(tmpstr,the_projectile.header.wavc1);
  DDX_Text(pDX, IDC_SOUND1, tmpstr);
  DDV_MaxChars(pDX, tmpstr,8);
  StoreResref(tmpstr,the_projectile.header.wavc1);

  RetrieveResref(tmpstr,the_projectile.header.wavc2);
  DDX_Text(pDX, IDC_SOUND2, tmpstr);
  DDV_MaxChars(pDX, tmpstr,8);
  StoreResref(tmpstr,the_projectile.header.wavc2);

  RetrieveResref(tmpstr,the_projectile.header.vvc);
  DDX_Text(pDX, IDC_VVC, tmpstr);
  DDV_MaxChars(pDX, tmpstr,8);
  StoreResref(tmpstr,the_projectile.header.vvc);

  tmpstr=get_spark_colour(the_projectile.header.spkcolour);
  DDX_Text(pDX, IDC_SPARKCOLOUR, tmpstr);
  the_projectile.header.spkcolour=(short) strtonum(tmpstr);

  tmpstr.Format("%d", the_projectile.header.attr);
  DDX_Text(pDX, IDC_ATTR, tmpstr);
  the_projectile.header.attr=strtonum(tmpstr);

  RetrieveResref(tmpstr,the_projectile.header.bam1);
  DDX_Text(pDX, IDC_BAM1, tmpstr);
  DDV_MaxChars(pDX, tmpstr,8);
  StoreResref(tmpstr,the_projectile.header.bam1);

  RetrieveResref(tmpstr,the_projectile.header.bam2);
  DDX_Text(pDX, IDC_BAM2, tmpstr);
  DDV_MaxChars(pDX, tmpstr,8);
  StoreResref(tmpstr,the_projectile.header.bam2);

  DDX_Text(pDX, IDC_SEQ1, the_projectile.header.seq1);
  DDX_Text(pDX, IDC_SEQ2, the_projectile.header.seq2);

  DDX_Text(pDX, IDC_Z, the_projectile.header.lightz);
  DDX_Text(pDX, IDC_X, the_projectile.header.lightx);
  DDX_Text(pDX, IDC_Y, the_projectile.header.lighty);

  RetrieveResref(tmpstr,the_projectile.header.bmp);
  DDX_Text(pDX, IDC_PALETTE, tmpstr);
  DDV_MaxChars(pDX, tmpstr,8);
  StoreResref(tmpstr,the_projectile.header.bmp);

  for(i=0;i<7;i++)
  {
    DDX_Text(pDX, IDC_U1+i, (unsigned char &) the_projectile.header.colours[i]);
  }
  DDX_Text(pDX, IDC_DELAY, the_projectile.header.smokepuff);
  for(i=0;i<7;i++)
  {
    DDX_Text(pDX, IDC_U11+i, (unsigned char &) the_projectile.header.smokecols[i]);
  }

  tmpstr=get_face_value(the_projectile.header.aim);
  DDX_Text(pDX, IDC_AIM, tmpstr);
  the_projectile.header.aim=(unsigned char) strtonum(tmpstr);

  tmpstr.Format("0x%04x %s",the_projectile.header.smokeanim,IDSToken("ANIMATE",the_projectile.header.smokeanim, false) );
  DDX_Text(pDX, IDC_SMOKETYPE, tmpstr);
  k=IDSKey2("ANIMATE", tmpstr);
  the_projectile.header.smokeanim=(unsigned short) k;

  for(i=0;i<3;i++)
  {
    RetrieveResref(tmpstr,the_projectile.header.trailbam[i]);
    DDX_Text(pDX, IDC_BAM3+i, tmpstr);
    DDV_MaxChars(pDX, tmpstr,8);
    StoreResref(tmpstr,the_projectile.header.trailbam[i]);

    DDX_Text(pDX, IDC_FREQ1+i, the_projectile.header.trailtype[i]);
  }

  flg=(the_projectile.header.type==3);
  cw=GetDlgItem(IDC_EXTENSION);
  cw->EnableWindow(flg);

  flg=!!(the_projectile.header.sparks&1);
  for(k=0;sparkboxids[k];k++)
  {
    cw=GetDlgItem(sparkboxids[k]);
    cw->EnableWindow(flg);
  }

  j=1;
  for(i=0;i<6;i++)
  {
    cb=(CButton *) GetDlgItem(IDC_FLAG1+i);
    if(cb)
    {
      cb->SetCheck(!!(the_projectile.header.sparks&j));
    }
    j<<=1;
  }

  j=1;
  for(i=0;i<16;i++)
  {
    cb=(CButton *) GetDlgItem(IDC_ATTR1+i);
    flg=!!(the_projectile.header.attr&j);
    cb->SetCheck(flg);
    if(headercontrols[i])
    {
      for(k=0;headercontrols[i][k];k++)
      {
        cw=GetDlgItem(headercontrols[i][k]);
        cw->EnableWindow(flg);
      }
    }
    j<<=1;
  }
  if(memcmp(&tmp,&the_projectile.header,sizeof(proj_header)))
  {
    the_projectile.m_changed=true;
  }
}

BEGIN_MESSAGE_MAP(CProjEdit, CDialog)
	//{{AFX_MSG_MAP(CProjEdit)
	ON_COMMAND(ID_FILE_LOAD, OnLoad)
	ON_COMMAND(ID_FILE_LOADEXTERNALSCRIPT, OnLoadex)
	ON_COMMAND(ID_FILE_SAVEAS, OnSaveas)
	ON_COMMAND(ID_FILE_NEW, OnNew)
	ON_BN_CLICKED(IDC_FLAG1, OnFlag1)
	ON_BN_CLICKED(IDC_FLAG2, OnFlag2)
	ON_BN_CLICKED(IDC_FLAG3, OnFlag3)
	ON_BN_CLICKED(IDC_FLAG4, OnFlag4)
	ON_BN_CLICKED(IDC_FLAG5, OnFlag5)
	ON_BN_CLICKED(IDC_FLAG6, OnFlag6)
	ON_BN_CLICKED(IDC_BROWSE1, OnBrowse1)
	ON_BN_CLICKED(IDC_PLAY1, OnPlay1)
	ON_BN_CLICKED(IDC_PLAY2, OnPlay2)
	ON_BN_CLICKED(IDC_BROWSE2, OnBrowse2)
	ON_BN_CLICKED(IDC_BROWSE3, OnBrowse3)
	ON_BN_CLICKED(IDC_BROWSE4, OnBrowse4)
	ON_BN_CLICKED(IDC_BROWSE5, OnBrowse5)
	ON_BN_CLICKED(IDC_BROWSE6, OnBrowse6)
	ON_BN_CLICKED(IDC_ATTR1, OnAttr1)
	ON_BN_CLICKED(IDC_ATTR2, OnAttr2)
	ON_BN_CLICKED(IDC_ATTR3, OnAttr3)
	ON_BN_CLICKED(IDC_ATTR4, OnAttr4)
	ON_BN_CLICKED(IDC_ATTR5, OnAttr5)
	ON_BN_CLICKED(IDC_ATTR6, OnAttr6)
	ON_BN_CLICKED(IDC_ATTR7, OnAttr7)
	ON_BN_CLICKED(IDC_ATTR8, OnAttr8)
	ON_BN_CLICKED(IDC_ATTR9, OnAttr9)
	ON_BN_CLICKED(IDC_ATTR10, OnAttr10)
	ON_BN_CLICKED(IDC_ATTR11, OnAttr11)
	ON_BN_CLICKED(IDC_ATTR12, OnAttr12)
	ON_BN_CLICKED(IDC_ATTR13, OnAttr13)
	ON_BN_CLICKED(IDC_ATTR14, OnAttr14)
	ON_BN_CLICKED(IDC_ATTR15, OnAttr15)
	ON_BN_CLICKED(IDC_ATTR16, OnAttr16)
	ON_BN_CLICKED(IDC_MPSMOKE, OnMpsmoke)
	ON_BN_CLICKED(IDC_MPTRAVEL, OnMptravel)
	ON_CBN_SELCHANGE(IDC_TYPE, OnSelchangeType)
	ON_BN_CLICKED(IDC_EXTENSION, OnExtension)
	ON_BN_CLICKED(IDC_UNKNOWN154, OnUnknown154)
	ON_COMMAND(ID_CHECK, OnCheck)
	ON_COMMAND(ID_FILE_SAVE, OnSave)
	ON_BN_CLICKED(IDC_UNKNOWN2C, OnUnknown2c)
	ON_EN_SETFOCUS(IDC_U1, OnSetfocusU1)
	ON_EN_SETFOCUS(IDC_U2, OnSetfocusU2)
	ON_EN_SETFOCUS(IDC_U3, OnSetfocusU3)
	ON_EN_SETFOCUS(IDC_U4, OnSetfocusU4)
	ON_EN_SETFOCUS(IDC_U5, OnSetfocusU5)
	ON_EN_SETFOCUS(IDC_U6, OnSetfocusU6)
	ON_EN_SETFOCUS(IDC_U7, OnSetfocusU7)
	ON_EN_SETFOCUS(IDC_U11, OnSetfocusU11)
	ON_EN_SETFOCUS(IDC_U12, OnSetfocusU12)
	ON_EN_SETFOCUS(IDC_U13, OnSetfocusU13)
	ON_EN_SETFOCUS(IDC_U14, OnSetfocusU14)
	ON_EN_SETFOCUS(IDC_U15, OnSetfocusU15)
	ON_EN_SETFOCUS(IDC_U16, OnSetfocusU16)
	ON_EN_SETFOCUS(IDC_U17, OnSetfocusU17)
	ON_BN_CLICKED(IDC_BROWSE7, OnBrowse7)
	ON_BN_CLICKED(IDC_BROWSE8, OnBrowse8)
	ON_BN_CLICKED(IDC_BROWSE13, OnBrowse9)
	ON_COMMAND(ID_FILE_TBG, OnFileTbg)
	ON_COMMAND(ID_FILE_TP2, OnFileTp2)
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_BN_CLICKED(IDC_LOADEX, OnLoadex)
	ON_BN_CLICKED(IDC_SAVEAS, OnSaveas)
	ON_BN_CLICKED(IDC_NEW, OnNew)
	ON_BN_CLICKED(IDC_CHECK, OnCheck)
	ON_CBN_KILLFOCUS(IDC_TYPE, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_SPEED, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_FLAGS, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_UNKNOWN0E, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_SOUND1, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_SOUND2, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_VVC, DefaultKillfocus)
	ON_CBN_KILLFOCUS(IDC_SPARKCOLOUR, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_BAM1, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_BAM2, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_PALETTE, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_ATTR, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_X, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_Y, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_Z, DefaultKillfocus)
	ON_CBN_KILLFOCUS(IDC_AIM, DefaultKillfocus)
	ON_CBN_KILLFOCUS(IDC_SMOKETYPE, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_BAM3, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_DELAY, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_FREQ1, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_FREQ2, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_FREQ3, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U2, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U3, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U1, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U11, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U12, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U13, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U14, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U15, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U16, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U17, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U4, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U5, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U6, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U7, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_SEQ1, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_SEQ2, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_UNKNOWN2A, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_BAM4, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_BAM5, DefaultKillfocus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CProjEdit::NewProjectile()
{
	itemname="new projectile";
  memset(&the_projectile.header,0,sizeof(proj_header) );
  memset(&the_projectile.extension,0,sizeof(proj_extension) );
  the_projectile.header.type=2;
  the_projectile.extension.tradius=256;
  the_projectile.extension.radius=256;
  the_projectile.extension.delay=100;
  the_projectile.extension.duration=1;
  the_projectile.m_changed=false;
}

/////////////////////////////////////////////////////////////////////////////
// CProjEdit message handlers

void CProjEdit::OnLoad() 
{
	int res;
	
  pickerdlg.m_restype=REF_PRO;
  pickerdlg.m_picked=itemname;
	res=pickerdlg.DoModal();
	if(res==IDOK)
	{
		res=read_projectile(pickerdlg.m_picked);
    switch(res)
    {
    case 0:
  		itemname=pickerdlg.m_picked;
      break;
    default:
      MessageBox("Cannot read projectile!","Error",MB_ICONSTOP|MB_OK);
      NewProjectile();
      break;
    }
    SetWindowText("Edit projectile: "+itemname);
    UpdateData(UD_DISPLAY);
	}
}

static char BASED_CODE szFilter[] = "Projectiles (*.pro)|*.pro|All files (*.*)|*.*||";

void CProjEdit::OnLoadex() 
{
  int fhandle;
  int res;
  
  res=OFN_FILEMUSTEXIST|OFN_ENABLESIZING|OFN_EXPLORER;
  if(readonly) res|=OFN_READONLY;  
  CMyFileDialog m_getfiledlg(TRUE, "pro", makeitemname(".pro",0), res, szFilter);

restart:  
  //if (filepath.GetLength()) strncpy(m_getfiledlg.m_ofn.lpstrFile,filepath, filepath.GetLength()+1);
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
    res=the_projectile.ReadProjectileFromFile(fhandle,-1);
    close(fhandle);
    lastopenedoverride=filepath.Left(filepath.ReverseFind('\\'));
    switch(res)
    {
    case 0:
      itemname=m_getfiledlg.GetFileTitle();
      itemname.MakeUpper();
      break;
    default:
      MessageBox("Cannot read projectile!","Error",MB_ICONSTOP|MB_OK);
      NewProjectile();
      break;
    }
    SetWindowText("Edit projectile: "+itemname);
    UpdateData(UD_DISPLAY);
  }
}

void CProjEdit::OnNew() 
{
	NewProjectile();
  SetWindowText("Edit projectile: "+itemname);
  UpdateData(UD_DISPLAY);
}

void CProjEdit::OnSaveas() 
{
  SaveProj(0);
}

void CProjEdit::OnSave() 
{
  SaveProj(1);
}

void CProjEdit::SaveProj(int save)
{
  CString newname;
  CString tmpstr;
  int res;

  if(readonly)
  {
    MessageBox("You opened it read only!","Warning",MB_ICONEXCLAMATION|MB_OK);
    return;
  }
  res=OFN_HIDEREADONLY|OFN_ENABLESIZING|OFN_EXPLORER;
  CMyFileDialog m_getfiledlg(FALSE, "pro", makeitemname(".pro",0), res, szFilter);

  if(save)
  {
    newname=itemname;
    filepath=makeitemname(".pro",0);
    goto gotname;
  }    

restart:  
  //if (filepath.GetLength()) strncpy(m_getfiledlg.m_ofn.lpstrFile,filepath, filepath.GetLength()+1);
  if( m_getfiledlg.DoModal() == IDOK )
  {
    filepath=m_getfiledlg.GetPathName();
    filepath.MakeLower();
    if(filepath.Right(4)!=".pro")
    {
      filepath+=".pro";
    }
    newname=m_getfiledlg.GetFileName();
    newname.MakeUpper();
    if(newname.Right(4)==".PRO") newname=newname.Left(newname.GetLength()-4);
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
    
    res = write_projectile(newname, filepath);
    lastopenedoverride=filepath.Left(filepath.ReverseFind('\\'));
    switch(res)
    {
    case 0:
      itemname=newname;
      break; //saved successfully
    case -2:
      MessageBox("Error while writing file!","Error",MB_ICONSTOP|MB_OK);
      break;
    default:
      MessageBox("Unhandled error!","Error",MB_ICONSTOP|MB_OK);
    }
  }
  RefreshDialog();
  UpdateData(UD_DISPLAY);
}

void CProjEdit::OnCheck() 
{
  int ret;

	ret=((CChitemDlg *) AfxGetMainWnd())->check_projectile();
  if(ret)
  {
    MessageBox(lasterrormsg,"Projectile editor",MB_ICONEXCLAMATION|MB_OK);
  }
}

void CProjEdit::RefreshDialog()
{
  CComboBox *cb;
  int i;

  SetWindowText("Edit projectile: "+itemname);
  cb=(CComboBox *) GetDlgItem(IDC_SPARKCOLOUR);
  cb->ResetContent();
  for(i=0;i<NUM_SPKCOL;i++)
  {
    cb->AddString(get_spark_colour(i) );
  }

  cb=(CComboBox *) GetDlgItem(IDC_AIM);
  cb->ResetContent();
  for(i=0;i<NUM_FVALUE;i++)
  {
    cb->AddString(proj_facing_desc[i]);
  }

  cb=(CComboBox *) GetDlgItem(IDC_SMOKETYPE);
  FillCombo("ANIMATE",cb,4);
  UpdateData(UD_DISPLAY);
}

BOOL CProjEdit::OnInitDialog() 
{
  CString tmpstr, tmpstr1, tmpstr2;

	CDialog::OnInitDialog();
  RefreshDialog();
  //tooltips
  {
    m_tooltip.Create(this,TTS_NOPREFIX);
    m_tooltip.SetMaxTipWidth(200);
    m_tooltip.SetTipBkColor(RGB(240,224,160));
    
    m_tooltip.AddTool(GetDlgItem(IDCANCEL), IDS_CANCEL);
    tmpstr1.LoadString(IDS_LOAD);
    tmpstr2.LoadString(IDS_PRO);
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
	return TRUE;
}

void CProjEdit::DefaultKillfocus() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CProjEdit::OnSelchangeType() 
{
  CString tmpstr;
  int idx;
  CComboBox *ce;

  ce=(CComboBox *) GetDlgItem(IDC_TYPE);
  idx=ce->GetCurSel();
  if(idx>=0)
  {
    ce->GetLBText(idx,tmpstr);
    the_projectile.header.type=(short) strtonum(tmpstr);
	  UpdateData(UD_DISPLAY);
  }
}

void CProjEdit::OnFlag1() 
{
	the_projectile.header.sparks^=1;
	UpdateData(UD_DISPLAY);
}

void CProjEdit::OnFlag2() 
{
	the_projectile.header.sparks^=2;
	UpdateData(UD_DISPLAY);
}

void CProjEdit::OnFlag3() 
{
	the_projectile.header.sparks^=4;
	UpdateData(UD_DISPLAY);
}

void CProjEdit::OnFlag4() 
{
	the_projectile.header.sparks^=8;
	UpdateData(UD_DISPLAY);
}

void CProjEdit::OnFlag5() 
{
	the_projectile.header.sparks^=16;
	UpdateData(UD_DISPLAY);
}

void CProjEdit::OnFlag6() 
{
	the_projectile.header.sparks^=32;
	UpdateData(UD_DISPLAY);
}

void CProjEdit::OnBrowse1() 
{
  pickerdlg.m_restype=REF_WAV;
  RetrieveResref(pickerdlg.m_picked,the_projectile.header.wavc1);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_projectile.header.wavc1);
  }
	UpdateData(UD_DISPLAY);	
}

void CProjEdit::OnPlay1() 
{
  CString tmpstr;
  
  RetrieveResref(tmpstr,the_projectile.header.wavc1);
  play_acm(tmpstr,false, false);
}

void CProjEdit::OnPlay2() 
{
  CString tmpstr;
  
  RetrieveResref(tmpstr,the_projectile.header.wavc2);
  play_acm(tmpstr,false, false);
}

void CProjEdit::OnBrowse2() 
{
  pickerdlg.m_restype=REF_WAV;
  RetrieveResref(pickerdlg.m_picked,the_projectile.header.wavc2);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_projectile.header.wavc2);
  }
	UpdateData(UD_DISPLAY);	
}

void CProjEdit::OnBrowse3() 
{
  pickerdlg.m_restype=REF_VVC;
  RetrieveResref(pickerdlg.m_picked,the_projectile.header.vvc);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_projectile.header.vvc);
  }
	UpdateData(UD_DISPLAY);	
}

void CProjEdit::OnBrowse4() 
{
  pickerdlg.m_restype=REF_BAM;
  RetrieveResref(pickerdlg.m_picked,the_projectile.header.bam1);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_projectile.header.bam1);
  }
	UpdateData(UD_DISPLAY);	
}

void CProjEdit::OnBrowse5() 
{
  pickerdlg.m_restype=REF_BAM;
  RetrieveResref(pickerdlg.m_picked,the_projectile.header.bam2);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_projectile.header.bam2);
  }
	UpdateData(UD_DISPLAY);	
}

void CProjEdit::OnBrowse6() 
{
  pickerdlg.m_restype=REF_BMP;
  RetrieveResref(pickerdlg.m_picked,the_projectile.header.bmp);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_projectile.header.bmp);
  }
	UpdateData(UD_DISPLAY);	
}


void CProjEdit::OnBrowse7() 
{
  pickerdlg.m_restype=REF_BAM;
  RetrieveResref(pickerdlg.m_picked,the_projectile.header.trailbam[0]);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_projectile.header.trailbam[0]);
  }
	UpdateData(UD_DISPLAY);	
}

void CProjEdit::OnBrowse8() 
{
  pickerdlg.m_restype=REF_BAM;
  RetrieveResref(pickerdlg.m_picked,the_projectile.header.trailbam[1]);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_projectile.header.trailbam[1]);
  }
	UpdateData(UD_DISPLAY);	
}

void CProjEdit::OnBrowse9() 
{
  pickerdlg.m_restype=REF_BAM;
  RetrieveResref(pickerdlg.m_picked,the_projectile.header.trailbam[2]);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_projectile.header.trailbam[2]);
  }
	UpdateData(UD_DISPLAY);	
}

void CProjEdit::OnAttr1() 
{
	the_projectile.header.attr^=PROJ_BAMPALETTE;
  UpdateData(UD_DISPLAY);	
}

void CProjEdit::OnAttr2() 
{
	the_projectile.header.attr^=PROJ_SMOKE;
  UpdateData(UD_DISPLAY);	
}

void CProjEdit::OnAttr3() 
{
	the_projectile.header.attr^=4;
  UpdateData(UD_DISPLAY);	
}

void CProjEdit::OnAttr4() 
{
	the_projectile.header.attr^=PROJ_NOLIGHT;
  UpdateData(UD_DISPLAY);	
}

void CProjEdit::OnAttr5() 
{
	the_projectile.header.attr^=16;
  UpdateData(UD_DISPLAY);	
}

void CProjEdit::OnAttr6() 
{
	the_projectile.header.attr^=PROJ_SHADOW;
  UpdateData(UD_DISPLAY);	
}

void CProjEdit::OnAttr7() 
{
	the_projectile.header.attr^=PROJ_LIGHTSPOT;
  UpdateData(UD_DISPLAY);	
}

void CProjEdit::OnAttr8() 
{
	the_projectile.header.attr^=128;
  UpdateData(UD_DISPLAY);	
}

void CProjEdit::OnAttr9() 
{
	the_projectile.header.attr^=256;
  UpdateData(UD_DISPLAY);	
}

void CProjEdit::OnAttr10() 
{
	the_projectile.header.attr^=512;
  UpdateData(UD_DISPLAY);	
}

void CProjEdit::OnAttr11() 
{
	the_projectile.header.attr^=1024;
  UpdateData(UD_DISPLAY);	
}

void CProjEdit::OnAttr12() 
{
	the_projectile.header.attr^=0x800;
  UpdateData(UD_DISPLAY);	
}

void CProjEdit::OnAttr13() 
{
	the_projectile.header.attr^=0x1000;
  UpdateData(UD_DISPLAY);	
}

void CProjEdit::OnAttr14() 
{
	the_projectile.header.attr^=0x2000;
  UpdateData(UD_DISPLAY);	
}

void CProjEdit::OnAttr15() 
{
	the_projectile.header.attr^=0x4000;
  UpdateData(UD_DISPLAY);	
}

void CProjEdit::OnAttr16() 
{
	the_projectile.header.attr^=0x8000;
  UpdateData(UD_DISPLAY);	
}

void CProjEdit::OnMpsmoke() 
{
  colordlg.m_picked=(unsigned char) the_projectile.header.smokecols[m_smokecoloridx];
  if(colordlg.DoModal()==IDOK)
  {
    the_projectile.header.smokecols[m_smokecoloridx]=(unsigned char) colordlg.m_picked;
    MakeGradientBitmap(hbs, colordlg.m_picked);
    m_mpsmoke.SetBitmap(hbs);
    UpdateData(UD_DISPLAY);
  }
}

void CProjEdit::RefreshSmoke(int idx)
{
	m_smokecoloridx=idx;	
  MakeGradientBitmap(hbs, (unsigned char) the_projectile.header.smokecols[m_smokecoloridx]);
  m_mpsmoke.SetBitmap(hbs);
  UpdateData(UD_DISPLAY);
}


void CProjEdit::OnSetfocusU11() 
{
  RefreshSmoke(0);
}

void CProjEdit::OnSetfocusU12() 
{
  RefreshSmoke(1);
}

void CProjEdit::OnSetfocusU13() 
{
  RefreshSmoke(2);
}

void CProjEdit::OnSetfocusU14() 
{
  RefreshSmoke(3);
}

void CProjEdit::OnSetfocusU15() 
{
  RefreshSmoke(4);
}

void CProjEdit::OnSetfocusU16() 
{
  RefreshSmoke(5);
}

void CProjEdit::OnSetfocusU17() 
{
  RefreshSmoke(6);
}

void CProjEdit::OnMptravel() 
{
  colordlg.m_picked=the_projectile.header.colours[m_bamcoloridx];
  if(colordlg.DoModal()==IDOK)
  {
    the_projectile.header.colours[m_bamcoloridx]=(unsigned char) colordlg.m_picked;
    MakeGradientBitmap(hbb, colordlg.m_picked);
    m_mptravel.SetBitmap(hbb);
    UpdateData(UD_DISPLAY);
  }
}

void CProjEdit::RefreshTravel(int idx)
{
	m_bamcoloridx=idx;	
  MakeGradientBitmap(hbb, the_projectile.header.colours[m_bamcoloridx]);
  m_mptravel.SetBitmap(hbb);
  UpdateData(UD_DISPLAY);
}

void CProjEdit::OnSetfocusU1() 
{
  RefreshTravel(0);
}

void CProjEdit::OnSetfocusU2() 
{
  RefreshTravel(1);
}

void CProjEdit::OnSetfocusU3() 
{
  RefreshTravel(2);
}

void CProjEdit::OnSetfocusU4() 
{
  RefreshTravel(3);
}

void CProjEdit::OnSetfocusU5() 
{
  RefreshTravel(4);
}

void CProjEdit::OnSetfocusU6() 
{
  RefreshTravel(5);
}

void CProjEdit::OnSetfocusU7() 
{
  RefreshTravel(6);
}

void CProjEdit::OnExtension() 
{
	CProjExt dlg;
	
  dlg.DoModal();
}

void CProjEdit::OnUnknown2c() 
{
	CProjGemRB dlg;
	
  dlg.DoModal();
}

void CProjEdit::OnUnknown154() 
{
	// TODO: Add your control notification handler code here
	
}

void CProjEdit::PostNcDestroy() 
{
  if(hbs) DeleteObject(hbs);
  if(hbb) DeleteObject(hbb);
	CDialog::PostNcDestroy();
}

void CProjEdit::OnCancel() 
{
  CString tmpstr;

  if(the_projectile.m_changed)
  {
    tmpstr.Format("Changes have been made to the projectile.\n"
      "Do you want to quit without save?\n");
    if(MessageBox(tmpstr,"Warning",MB_YESNO)==IDNO)
    {
      return;
    }
  }
	CDialog::OnCancel();
}

BOOL CProjEdit::PreTranslateMessage(MSG* pMsg) 
{
  m_tooltip.RelayEvent(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}

/////////////////////////////////////////////////////////////////////////////
// CProjExt dialog

CProjExt::CProjExt(CWnd* pParent /*=NULL*/)
	: CDialog(CProjExt::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProjExt)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

static int fragmentids[]={IDC_FRAGMENT,0};

static int vvcids[]={IDC_VVC,0};

static int coneids[]={IDC_CONESIZE,0};

static int *extensioncontrols[16]={0,0,0,0,0, fragmentids, 0,0,
0,0,vvcids,coneids, 0,0,0,0};

void CProjExt::DoDataExchange(CDataExchange* pDX)
{
  proj_extension tmp;
  CComboBox *cb2;
  CButton *cb;
  CWnd *cw;
  CString tmpstr;
  int i,j,k;
  int flg;

  memcpy(&tmp,&the_projectile.extension,sizeof(proj_extension));
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProjExt)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP

  tmpstr.Format("0x%02x",the_projectile.extension.aoe);
  DDX_Text(pDX, IDC_ATTR, tmpstr);
  the_projectile.extension.aoe=(short) strtonum(tmpstr);

  DDX_Text(pDX, IDC_UNKNOWN2, the_projectile.extension.flags);

  tmpstr=convert_radius(the_projectile.extension.tradius);
  DDX_Text(pDX, IDC_TRIGGER, tmpstr);
  the_projectile.extension.tradius=(short) strtonum(tmpstr);

  tmpstr=convert_radius(the_projectile.extension.radius);
  DDX_Text(pDX, IDC_AREA, tmpstr);
  the_projectile.extension.radius=(short) strtonum(tmpstr);

  RetrieveResref(tmpstr, the_projectile.extension.wavc1);
  DDX_Text(pDX, IDC_SOUND, tmpstr);
  DDV_MaxChars(pDX, tmpstr, 8);
  StoreResref(tmpstr, the_projectile.extension.wavc1);

  DDX_Text(pDX, IDC_DELAY, the_projectile.extension.delay);

  tmpstr.Format("0x%04x %s",the_projectile.extension.fragmentanim,IDSToken("ANIMATE",the_projectile.extension.fragmentanim, false) );
  DDX_Text(pDX, IDC_FRAGMENT, tmpstr);
  k=IDSKey2("ANIMATE", tmpstr);
  the_projectile.extension.fragmentanim=(unsigned short) k;

  DDX_Text(pDX, IDC_DURATION, the_projectile.extension.duration);

  if(pDX->m_bSaveAndValidate==UD_DISPLAY)
  {
    cb2=(CComboBox *) GetDlgItem(IDC_EXPLOSION);
    tmpstr.Format("0x%02x",the_projectile.extension.expgr2);
    k=cb2->SelectString(0,tmpstr);
    if(k==-1)
    {
      tmpstr.Format("0x%02x unknown",the_projectile.extension.expgr2);
    }
    else
    {
      cb2->GetLBText(k,tmpstr);
    }
  }
  DDX_Text(pDX, IDC_EXPLOSION, tmpstr); //first projectile
  the_projectile.extension.expgr2=(unsigned char) strtonum(tmpstr);

  tmpstr=colortitle(the_projectile.extension.expclr);
  DDX_Text(pDX, IDC_COLOR, tmpstr);
  the_projectile.extension.expclr=(short) strtonum(tmpstr);

  tmpstr=get_projectile_id(the_projectile.extension.proref+1,1);
  DDX_Text(pDX, IDC_UNKNOWN14, tmpstr);
  the_projectile.extension.proref=(short) strtonum(tmpstr);

  tmpstr=get_projectile_id(the_projectile.extension.projectile,0);
  DDX_Text(pDX, IDC_PROJECTILE, tmpstr);
  the_projectile.extension.projectile=(short) strtonum(tmpstr);

  RetrieveResref(tmpstr, the_projectile.extension.vvc);
  DDX_Text(pDX, IDC_VVC, tmpstr);
  DDV_MaxChars(pDX, tmpstr, 8);
  StoreResref(tmpstr, the_projectile.extension.vvc);

  tmpstr=convert_degree(the_projectile.extension.conewidth);
  DDX_Text(pDX, IDC_CONESIZE, tmpstr);
  the_projectile.extension.conewidth=(short) strtonum(tmpstr);

  j=1;
  for(i=0;i<16;i++)
  {
    cb=(CButton *) GetDlgItem(IDC_ATTR1+i);
    flg=!!(the_projectile.extension.aoe&j);
    cb->SetCheck(flg);
    j<<=1;
    if(extensioncontrols[i])
    {
      for(k=0;extensioncontrols[i][k];k++)
      {
        cw=GetDlgItem(extensioncontrols[i][k]);
        cw->EnableWindow(flg);
      }
    }
  }
  if(memcmp(&tmp,&the_projectile.extension,sizeof(proj_extension)))
  {
    the_projectile.m_changed=true;
  }
}

BOOL CProjExt::OnInitDialog() 
{
  CComboBox *cb1, *cb2;
  POSITION pos, pos2;
  CString tmpstr, tmp2;
  CString tmpstr1, tmpstr2;
  int i;

	CDialog::OnInitDialog();
  
  cb1=(CComboBox *) GetDlgItem(IDC_CONESIZE);
  for(i=0;i<360;i+=30)
  {
    tmpstr=convert_degree(i);
    cb1->AddString(tmpstr);
  }
  cb1=(CComboBox *) GetDlgItem(IDC_TRIGGER);
  cb2=(CComboBox *) GetDlgItem(IDC_AREA);
  cb1->ResetContent();
  cb2->ResetContent();
	for(i=5;i<30;i+=5)
  {
    tmpstr=convert_radius(i,1);
    cb1->AddString(tmpstr);
    cb2->AddString(tmpstr);
  }
  for(i=30;i<100;i+=10)
  {
    tmpstr=convert_radius(i,1);
    cb1->AddString(tmpstr);
    cb2->AddString(tmpstr);
  }
  for(i=100;i<301;i+=50)
  {
    tmpstr=convert_radius(i,1);
    cb1->AddString(tmpstr);
    cb2->AddString(tmpstr);
  }
  cb1=(CComboBox *) GetDlgItem(IDC_FRAGMENT);
  FillCombo("ANIMATE",cb1,4);

  pos=pro_references.GetHeadPosition();
  pos2=pro_titles.GetHeadPosition();
  i=0;
  cb1=(CComboBox *) GetDlgItem(IDC_UNKNOWN14); //some projectile reference
  cb2=(CComboBox *) GetDlgItem(IDC_PROJECTILE);
  cb1->ResetContent();
  cb2->ResetContent();
  while(pos)
  {
    if(pos2) tmp2=pro_titles.GetNext(pos2);
    else tmp2.Format("???");
    tmpstr1.Format("%s (%s)",tmp2,pro_references.GetNext(pos));
    tmpstr.Format("%d-%s",i, tmpstr1);
    cb2->AddString(tmpstr);
    if(i)
    {
      tmpstr.Format("%d-%s",i-1, tmpstr1);
      cb1->AddString(tmpstr);
    }
    i++;
  }
  cb1=(CComboBox *) GetDlgItem(IDC_EXPLOSION);
  FillCombo("FIREBALL",cb1,2);
  //tooltips
  {
    m_tooltip.Create(this,TTS_NOPREFIX);
    m_tooltip.SetMaxTipWidth(200);
    m_tooltip.SetTipBkColor(RGB(240,224,160));
    
    m_tooltip.AddTool(GetDlgItem(IDOK), IDS_CANCEL);
  }

	UpdateData(UD_DISPLAY);
	return TRUE;
}

BEGIN_MESSAGE_MAP(CProjExt, CDialog)
	//{{AFX_MSG_MAP(CProjExt)
	ON_BN_CLICKED(IDC_ATTR1, OnAttr1)
	ON_BN_CLICKED(IDC_ATTR2, OnAttr2)
	ON_BN_CLICKED(IDC_ATTR3, OnAttr3)
	ON_BN_CLICKED(IDC_ATTR4, OnAttr4)
	ON_BN_CLICKED(IDC_ATTR5, OnAttr5)
	ON_BN_CLICKED(IDC_ATTR6, OnAttr6)
	ON_BN_CLICKED(IDC_ATTR7, OnAttr7)
	ON_BN_CLICKED(IDC_ATTR8, OnAttr8)
	ON_BN_CLICKED(IDC_ATTR9, OnAttr9)
	ON_BN_CLICKED(IDC_ATTR10, OnAttr10)
	ON_BN_CLICKED(IDC_ATTR11, OnAttr11)
	ON_BN_CLICKED(IDC_ATTR12, OnAttr12)
	ON_BN_CLICKED(IDC_ATTR13, OnAttr13)
	ON_BN_CLICKED(IDC_ATTR14, OnAttr14)
	ON_BN_CLICKED(IDC_ATTR15, OnAttr15)
	ON_BN_CLICKED(IDC_ATTR16, OnAttr16)
	ON_BN_CLICKED(IDC_BROWSE1, OnBrowse1)
	ON_BN_CLICKED(IDC_PLAY1, OnPlay1)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_BROWSE2, OnBrowse2)
	ON_BN_CLICKED(IDC_UNKNOWN, OnUnknown)
	ON_EN_KILLFOCUS(IDC_ATTR, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_UNKNOWN2, DefaultKillfocus)
	ON_CBN_KILLFOCUS(IDC_TRIGGER, DefaultKillfocus)
	ON_CBN_KILLFOCUS(IDC_AREA, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_SOUND, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_DURATION, DefaultKillfocus)
	ON_CBN_KILLFOCUS(IDC_EXPLOSION, DefaultKillfocus)
	ON_CBN_KILLFOCUS(IDC_CONESIZE, DefaultKillfocus)
	ON_CBN_KILLFOCUS(IDC_FRAGMENT, DefaultKillfocus)
	ON_CBN_KILLFOCUS(IDC_UNKNOWN14, DefaultKillfocus)
	ON_CBN_KILLFOCUS(IDC_PROJECTILE, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_COLOR, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_VVC, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_DELAY, DefaultKillfocus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProjExt message handlers

void CProjExt::DefaultKillfocus() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CProjExt::OnAttr1() 
{
  the_projectile.extension.aoe^=PROJ_VISIBLE;
	UpdateData(UD_DISPLAY);
}

void CProjExt::OnAttr2() 
{
  the_projectile.extension.aoe^=PROJ_INANIMATE;
	UpdateData(UD_DISPLAY);
}

void CProjExt::OnAttr3() 
{
  the_projectile.extension.aoe^=PROJ_TRIGGER;
	UpdateData(UD_DISPLAY);
}

void CProjExt::OnAttr4() 
{
  the_projectile.extension.aoe^=PROJ_CONTROLLED;
	UpdateData(UD_DISPLAY);
}

void CProjExt::OnAttr5() 
{
  the_projectile.extension.aoe^=PROJ_NO_PARTY;
	UpdateData(UD_DISPLAY);
}

void CProjExt::OnAttr6() 
{
  the_projectile.extension.aoe^=PROJ_FRAGMENT;
	UpdateData(UD_DISPLAY);
}

void CProjExt::OnAttr7() 
{
  the_projectile.extension.aoe^=PROJ_NO_SELF;
	UpdateData(UD_DISPLAY);
}

void CProjExt::OnAttr8() 
{
  the_projectile.extension.aoe^=PROJ_NO_ENEMY;  
	UpdateData(UD_DISPLAY);
}

void CProjExt::OnAttr9() 
{
  the_projectile.extension.aoe^=PROJ_LEVEL;
	UpdateData(UD_DISPLAY);
}

void CProjExt::OnAttr10() 
{
  the_projectile.extension.aoe^=512;
	UpdateData(UD_DISPLAY);
}

void CProjExt::OnAttr11() 
{
  the_projectile.extension.aoe^=PROJ_HAS_VVC;
	UpdateData(UD_DISPLAY);
}

void CProjExt::OnAttr12() 
{
  the_projectile.extension.aoe^=PROJ_CONE;
	UpdateData(UD_DISPLAY);
}

void CProjExt::OnAttr13() 
{
  the_projectile.extension.aoe^=0x1000;
	UpdateData(UD_DISPLAY);
}

void CProjExt::OnAttr14() 
{
  the_projectile.extension.aoe^=0x2000;
	UpdateData(UD_DISPLAY);
}

void CProjExt::OnAttr15() 
{
  the_projectile.extension.aoe^=PROJ_DELAYED;
	UpdateData(UD_DISPLAY);
}

void CProjExt::OnAttr16() 
{
  the_projectile.extension.aoe^=PROJ_AFFECT_ONE;
	UpdateData(UD_DISPLAY);
}

void CProjExt::OnBrowse1() 
{
  pickerdlg.m_restype=REF_WAV;
  RetrieveResref(pickerdlg.m_picked,the_projectile.extension.wavc1);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_projectile.extension.wavc1);
  }
	UpdateData(UD_DISPLAY);	
}

void CProjExt::OnPlay1() 
{
  CString tmpstr;

  RetrieveResref(tmpstr, the_projectile.extension.wavc1);
  play_acm(tmpstr,false,false);
}

void CProjExt::OnBrowse() 
{
  pickerdlg.m_restype=REF_VVC;
  RetrieveResref(pickerdlg.m_picked,the_projectile.extension.vvc);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_projectile.extension.vvc);
  }
	UpdateData(UD_DISPLAY);	
}

void CProjExt::OnBrowse2() 
{
  colordlg.m_picked=the_projectile.extension.expclr;
  if(colordlg.DoModal()==IDOK)
  {
    the_projectile.extension.expclr=(unsigned char) colordlg.m_picked;
    UpdateData(UD_DISPLAY);
  }	
}

void CProjExt::OnUnknown() 
{
	CProjAreaGemRB dlg;
	
  dlg.DoModal();
}

void CProjEdit::OnFileTbg() 
{
  ExportTBG(this, REF_PRO, 0);
}

void CProjEdit::OnFileTp2() 
{
  ExportTBG(this, REF_PRO, 1);
}

BOOL CProjExt::PreTranslateMessage(MSG* pMsg) 
{
  m_tooltip.RelayEvent(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}
