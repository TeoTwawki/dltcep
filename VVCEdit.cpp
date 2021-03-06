// VVCEdit.cpp : implementation file
//

#include "stdafx.h"

#include <fcntl.h>
#include <sys/stat.h>
#include "chitem.h"
#include "chitemDlg.h"
#include "VVCEdit.h"
#include "MyFileDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVVCEdit dialog

CVVCEdit::CVVCEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CVVCEdit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CVVCEdit)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

static int tranboxid1[]={IDC_TRANSPARENT,IDC_TRANSLUCENT,IDC_SHADOW,IDC_BRIGHTEST,IDC_MIRROR,IDC_MIRROR2,0,0};
static int tranboxid2[]={0,IDC_GLOW,0,IDC_IGNOREGREY,IDC_IGNORERED,0,0,0};
static int tranboxid3[]={IDC_BLEND,IDC_TINT,0,IDC_GREY,0,IDC_LIGHTEFFECT,0,0};
static int tranboxid4[]={0,IDC_RED,0,0,0,0,0,0};

void CVVCEdit::checkflags(int *boxids, int value)
{
  int i,j;
  CButton *cb;

  j=1;
  for(i=0;i<8;i++)
  {
    if(boxids[i])
    {
      cb=(CButton *) GetDlgItem(boxids[i]);
      cb->SetCheck(!!(value&j));
    }
    j<<=1;
  }
}

void CVVCEdit::DoDataExchange(CDataExchange* pDX)
{
  vvc_header tmp;
  CString tmpstr;

  memcpy(&tmp,&the_videocell.header,sizeof(vvc_header));
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVVCEdit)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
  RetrieveResref(tmpstr,the_videocell.header.bam);
	DDX_Text(pDX, IDC_BAM, tmpstr);
	DDV_MaxChars(pDX, tmpstr, 8);
  StoreResref(tmpstr,the_videocell.header.bam);

  tmpstr.Format("0x%x",the_videocell.header.transparency);
  DDX_Text(pDX, IDC_FLAG1, tmpstr);
  DDV_MaxChars(pDX, tmpstr,4);
  the_videocell.header.transparency=(BYTE) strtonum(tmpstr);
  checkflags(tranboxid1,the_videocell.header.transparency);

  tmpstr.Format("0x%x",the_videocell.header.trans2);
  DDX_Text(pDX, IDC_FLAG2, tmpstr);
  DDV_MaxChars(pDX, tmpstr,4);
  the_videocell.header.trans2=(BYTE) strtonum(tmpstr);
  checkflags(tranboxid2,the_videocell.header.trans2);

  tmpstr.Format("0x%x",the_videocell.header.colouring);
  DDX_Text(pDX, IDC_FLAG3, tmpstr);
  DDV_MaxChars(pDX, tmpstr,4);
  the_videocell.header.colouring=(BYTE) strtonum(tmpstr);
  checkflags(tranboxid3,the_videocell.header.colouring);

  tmpstr.Format("0x%x",the_videocell.header.col2);
  DDX_Text(pDX, IDC_FLAG4, tmpstr);
  DDV_MaxChars(pDX, tmpstr,4);
  the_videocell.header.col2=(BYTE) strtonum(tmpstr);
  checkflags(tranboxid4,the_videocell.header.col2);

  DDX_Text(pDX, IDC_SEQ1, the_videocell.header.seq1);
  DDX_Text(pDX, IDC_SEQ2, the_videocell.header.seq2);

  tmpstr.Format("%d",the_videocell.header.framerate);
  DDX_Text(pDX, IDC_FRAMERATE, tmpstr);
  DDV_MaxChars(pDX, tmpstr, 4);
  the_videocell.header.framerate=strtonum(tmpstr);
  if(!the_videocell.header.framerate) the_videocell.header.framerate=15;
  //
  tmpstr.Format("0x%x",the_videocell.header.sequencing);
  DDX_Text(pDX, IDC_SEQUENCING, tmpstr);
 	DDV_MaxChars(pDX, tmpstr, 10);
  the_videocell.header.sequencing=strtonum(tmpstr);
  //
  DDX_Text(pDX, IDC_DURATION, the_videocell.header.duration);

  DDX_Text(pDX, IDC_XCO, the_videocell.header.xpos);
  DDV_MinMaxLong(pDX,the_videocell.header.xpos,-0x7ffffff,0x7ffffff);
  DDX_Text(pDX, IDC_YCO, the_videocell.header.ypos);
  DDV_MinMaxLong(pDX,the_videocell.header.ypos,-0x7ffffff,0x7ffffff);
  DDX_Text(pDX, IDC_ZCO, the_videocell.header.zpos);
  DDV_MinMaxLong(pDX,the_videocell.header.zpos,-0x7ffffff,0x7ffffff);

  RetrieveResref(tmpstr,the_videocell.header.sound1);
	DDX_Text(pDX, IDC_SOUND1, tmpstr);
 	DDV_MaxChars(pDX, tmpstr, 8);
  StoreResref(tmpstr,the_videocell.header.sound1);

  RetrieveResref(tmpstr,the_videocell.header.sound2);
	DDX_Text(pDX, IDC_SOUND2, tmpstr);
 	DDV_MaxChars(pDX, tmpstr, 8);
  StoreResref(tmpstr,the_videocell.header.sound2);

  RetrieveResref(tmpstr,the_videocell.header.shadow);
  DDX_Text(pDX, IDC_UNKNOWN10, tmpstr);
	DDV_MaxChars(pDX, tmpstr, 8);
  StoreResref(tmpstr,the_videocell.header.shadow);

  //supposed to be transparency level
  tmpstr.Format("0x%0x",the_videocell.header.transvalue);
  DDX_Text(pDX, IDC_UNKNOWN1C, tmpstr);
  the_videocell.header.transvalue=strtonum(tmpstr);

  //supposed to be tint color?
  tmpstr.Format("0x%0x",the_videocell.header.colorvalue);
  DDX_Text(pDX, IDC_UNKNOWN24, tmpstr);
  the_videocell.header.colorvalue=strtonum(tmpstr);

  tmpstr.Format("0x%0x",the_videocell.header.hasorient);
  DDX_Text(pDX, IDC_FACE, tmpstr);
  the_videocell.header.hasorient=strtonum(tmpstr);

  tmpstr.Format("0x%0x",the_videocell.header.facecount);
  DDX_Text(pDX, IDC_UNKNOWN38, tmpstr);
  the_videocell.header.facecount=strtonum(tmpstr);

  tmpstr.Format("0x%0x",the_videocell.header.currorient);
  DDX_Text(pDX, IDC_UNKNOWN3C, tmpstr);
  the_videocell.header.currorient=strtonum(tmpstr);

  tmpstr.Format("0x%0x",the_videocell.header.position);
  DDX_Text(pDX, IDC_UNKNOWN40, tmpstr);
  the_videocell.header.position=strtonum(tmpstr);

  RetrieveResref(tmpstr,the_videocell.header.palette);
	DDX_Text(pDX, IDC_UNKNOWN44, tmpstr);
	DDV_MaxChars(pDX, tmpstr, 8);
  StoreResref(tmpstr,the_videocell.header.palette);

  //this 2 appears to be a numeric value (not bitfield)
  tmpstr.Format("%d",the_videocell.header.spotX);
  DDX_Text(pDX, IDC_UNKNOWN50, tmpstr);
  the_videocell.header.spotX=strtonum(tmpstr);

  tmpstr.Format("%d",the_videocell.header.spotY);
  DDX_Text(pDX, IDC_UNKNOWN54, tmpstr);
  the_videocell.header.spotY=strtonum(tmpstr);

  tmpstr.Format("%d",the_videocell.header.spotZ);
  DDX_Text(pDX, IDC_UNKNOWN58, tmpstr);
  the_videocell.header.spotZ=strtonum(tmpstr);

  RetrieveResref(tmpstr,the_videocell.header.name);
	DDX_Text(pDX, IDC_UNKNOWN60, tmpstr);
	DDV_MaxChars(pDX, tmpstr, 8);
  StoreResref(tmpstr,the_videocell.header.name);

  tmpstr.Format("0x%0x",the_videocell.header.currentseq);
  DDX_Text(pDX, IDC_UNKNOWN70, tmpstr);
  the_videocell.header.currentseq=strtonum(tmpstr);

  tmpstr.Format("0x%0x",the_videocell.header.spanseq);
  DDX_Text(pDX, IDC_UNKNOWN74, tmpstr);
  the_videocell.header.spanseq=strtonum(tmpstr);

  RetrieveResref(tmpstr,the_videocell.header.alpha);
	DDX_Text(pDX, IDC_UNKNOWN88, tmpstr);
	DDV_MaxChars(pDX, tmpstr, 8);
  StoreResref(tmpstr,the_videocell.header.alpha);

  DDX_Text(pDX, IDC_UNKNOWN90, the_videocell.header.seq3);

  RetrieveResref(tmpstr,the_videocell.header.sound3);
  DDX_Text(pDX, IDC_UNKNOWN94, tmpstr);
	DDV_MaxChars(pDX, tmpstr, 8);
  StoreResref(tmpstr,the_videocell.header.sound3);

  if(memcmp(&tmp,&the_videocell.header,sizeof(vvc_header)))
  {
    the_videocell.m_changed=true;
  }
}

void CVVCEdit::NewVVC()
{
	itemname="new video cell";
  memset(&the_videocell.header,0,sizeof(vvc_header));
  the_videocell.header.framerate=15; //default value
  the_videocell.header.duration=-1;  //usually set to -1
  the_videocell.header.facecount=1;  //usually set to 1
  the_videocell.header.sequencing=8; //usually set to 8
  the_videocell.m_changed=false;
}

BEGIN_MESSAGE_MAP(CVVCEdit, CDialog)
	//{{AFX_MSG_MAP(CVVCEdit)
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_BN_CLICKED(IDC_LOADEX, OnLoadex)
	ON_BN_CLICKED(IDC_SAVEAS, OnSaveas)
	ON_EN_KILLFOCUS(IDC_SEQUENCING, OnKillfocusSequencing)
	ON_BN_CLICKED(IDC_LOOP, OnLoop)
	ON_BN_CLICKED(IDC_PLAY1, OnPlay1)
	ON_BN_CLICKED(IDC_PLAY2, OnPlay2)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_BROWSE2, OnBrowse2)
	ON_BN_CLICKED(IDC_BROWSE3, OnBrowse3)
	ON_BN_CLICKED(IDC_NEW, OnNew)
	ON_BN_CLICKED(IDC_TRANSPARENT, OnTransparent)
	ON_BN_CLICKED(IDC_TINT, OnTint)
	ON_BN_CLICKED(IDC_GLOW, OnGlow)
	ON_BN_CLICKED(IDC_GREY, OnGrey)
	ON_BN_CLICKED(IDC_UNKNOWN, OnUnknown)
	ON_BN_CLICKED(IDC_BRIGHTEST, OnBrightest)
	ON_BN_CLICKED(IDC_LIGHTEFFECT, OnLighteffect)
	ON_BN_CLICKED(IDC_CHECK, OnCheck)
	ON_COMMAND(ID_FILE_SAVE, OnSave)
	ON_BN_CLICKED(IDC_POS1, OnPos1)
	ON_BN_CLICKED(IDC_POS2, OnPos2)
	ON_BN_CLICKED(IDC_POS4, OnPos4)
	ON_BN_CLICKED(IDC_USEBAM, OnUsebam)
	ON_BN_CLICKED(IDC_LIGHTSPOT, OnLightspot)
	ON_BN_CLICKED(IDC_HEIGHT, OnHeight)
	ON_BN_CLICKED(IDC_MIRROR, OnMirror)
	ON_BN_CLICKED(IDC_MIRROR2, OnMirror2)
	ON_BN_CLICKED(IDC_BLEND, OnBlend)
	ON_BN_CLICKED(IDC_WALL, OnWall)
	ON_BN_CLICKED(IDC_RED, OnRed)
	ON_BN_CLICKED(IDC_PALETTE, OnPalette)
	ON_BN_CLICKED(IDC_BROWSE4, OnBrowse4)
	ON_BN_CLICKED(IDC_PLAY3, OnPlay3)
	ON_BN_CLICKED(IDC_SHADOW, OnShadow)
	ON_BN_CLICKED(IDC_TRANSLUCENT, OnTranslucent)
	ON_BN_CLICKED(IDC_IGNOREGREY, OnIgnoregrey)
	ON_BN_CLICKED(IDC_IGNORERED, OnIgnorered)
	ON_BN_CLICKED(IDC_POS3, OnPos3)
	ON_EN_KILLFOCUS(IDC_UNKNOWN90, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_BAM, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_FLAG1, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_FLAG2, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_FLAG3, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_FLAG4, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_FRAMERATE, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_SEQ1, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_SEQ2, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_SOUND1, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_SOUND2, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_UNKNOWN10, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_UNKNOWN1C, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_UNKNOWN24, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_FACE, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_UNKNOWN38, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_UNKNOWN3C, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_UNKNOWN44, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_UNKNOWN48, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_UNKNOWN50, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_UNKNOWN54, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_DURATION, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_UNKNOWN60, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_UNKNOWN70, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_XCO, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_YCO, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_ZCO, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_UNKNOWN14, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_UNKNOWN74, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_UNKNOWN88, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_UNKNOWN64, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_UNKNOWN58, DefaultKillfocus)
	ON_COMMAND(ID_FILE_NEW, OnNew)
	ON_COMMAND(ID_FILE_LOAD, OnLoad)
	ON_COMMAND(ID_FILE_LOADEXTERNAL, OnLoadex)
	ON_COMMAND(ID_CHECK, OnCheck)
	ON_COMMAND(ID_FILE_SAVEAS, OnSaveas)
	ON_EN_KILLFOCUS(IDC_UNKNOWN40, OnKillfocusSequencing)
	ON_EN_KILLFOCUS(IDC_UNKNOWN94, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_UNKNOWN8C, DefaultKillfocus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVVCEdit message handlers

void CVVCEdit::OnLoad() 
{
	int res;
	
  pickerdlg.m_restype=REF_VVC;
  pickerdlg.m_picked=itemname;
	res=pickerdlg.DoModal();
	if(res==IDOK)
	{
		res=read_videocell(pickerdlg.m_picked);
    switch(res)
    {
    case 0:
  		itemname=pickerdlg.m_picked;
      break;
    default:
      MessageBox("Cannot read video cell!","Error",MB_ICONSTOP|MB_OK);
      NewVVC();
      break;
    }
	}
  RefreshDialog();
  UpdateData(UD_DISPLAY);
}

static char BASED_CODE szFilter[] = "Video Cell files (*.vvc)|*.vvc|All files (*.*)|*.*||";

void CVVCEdit::OnLoadex() 
{
  CString filepath;
  int fhandle;
  int res;
  
  res=OFN_FILEMUSTEXIST|OFN_ENABLESIZING|OFN_EXPLORER;
  if(readonly) res|=OFN_READONLY;  
  CMyFileDialog m_getfiledlg(TRUE, "vvc", makeitemname(".vvc",0), res, szFilter);

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
    res=the_videocell.ReadVVCFromFile(fhandle,-1);
    close(fhandle);
    lastopenedoverride=filepath.Left(filepath.ReverseFind('\\'));
    switch(res)
    {
    case 0:
      itemname=m_getfiledlg.GetFileTitle();
      itemname.MakeUpper();
      break;
    default:
      MessageBox("Cannot read video cell!","Error",MB_ICONSTOP|MB_OK);
      NewVVC();
      break;
    }
  }
  RefreshDialog();
  UpdateData(UD_DISPLAY);
}

void CVVCEdit::OnNew() 
{
	NewVVC();
  RefreshDialog();
  UpdateData(UD_DISPLAY);
}

void CVVCEdit::OnSaveas() 
{
  SaveVVC(0);
}

void CVVCEdit::OnSave() 
{
  SaveVVC(1);
}

void CVVCEdit::SaveVVC(int save) 
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
  CMyFileDialog m_getfiledlg(FALSE, "vvc", makeitemname(".vvc",0), res, szFilter);

  if(save)
  {
    newname=itemname;
    filepath=makeitemname(".vvc",0);
    goto gotname;
  }    
  
restart:  
  if( m_getfiledlg.DoModal() == IDOK )
  {
    filepath=m_getfiledlg.GetPathName();
    filepath.MakeLower();
    if(filepath.Right(4)!=".vvc")
    {
      filepath+=".vvc";
    }
    newname=m_getfiledlg.GetFileName();
    newname.MakeUpper();
    if(newname.Right(4)==".VVC") newname=newname.Left(newname.GetLength()-4);
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
    
    res = write_videocell(newname, filepath);
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
  RefreshDialog();
  UpdateData(UD_DISPLAY);
}

void CVVCEdit::OnCheck() 
{
  int ret;

	ret=((CChitemDlg *) AfxGetMainWnd())->check_videocell();
  if(ret)
  {
    MessageBox(lasterrormsg,"VVC editor",MB_ICONEXCLAMATION|MB_OK);
  }
}

int checkbuttons[]={
  IDC_LOOP, IDC_LIGHTSPOT, IDC_HEIGHT, IDC_USEBAM, IDC_PALETTE, -1, IDC_WALL, 0
};

int posbuttons[]={IDC_POS1, IDC_POS2, IDC_POS3, IDC_POS4, 0
};
  
void CVVCEdit::RefreshDialog()
{
  CButton *chkbutton;
  int i;
  int bit;

  SetWindowText("Edit video cell: "+itemname);
  bit=1;
	for(i=0;i<32 && checkbuttons[i];i++)
  {
    if (checkbuttons[i]!=-1)
    {
      chkbutton=(CButton *) GetDlgItem(checkbuttons[i]);
      chkbutton->SetCheck(!!(the_videocell.header.sequencing&bit));
    }
    bit<<=1;
  }
  bit=1;
  for(i=0;i<32 && posbuttons[i];i++)
  {
    chkbutton=(CButton *) GetDlgItem(posbuttons[i]);
    chkbutton->SetCheck(!!(the_videocell.header.position&bit));
    bit<<=1;
  }
}

BOOL CVVCEdit::OnInitDialog() 
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
    tmpstr2.LoadString(IDS_VVC);
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
    m_tooltip.AddTool(GetDlgItem(IDC_BAM), IDS_BAM);
    m_tooltip.AddTool(GetDlgItem(IDC_BROWSE), IDS_BAM);
    m_tooltip.AddTool(GetDlgItem(IDC_FRAMERATE), IDS_FRAME15);
    m_tooltip.AddTool(GetDlgItem(IDC_FLAG1), IDS_VVCFLAG);
    m_tooltip.AddTool(GetDlgItem(IDC_FLAG2), IDS_VVCFLAG);
    m_tooltip.AddTool(GetDlgItem(IDC_FLAG3), IDS_VVCFLAG);
    m_tooltip.AddTool(GetDlgItem(IDC_FLAG4), IDS_VVCFLAG);
    m_tooltip.AddTool(GetDlgItem(IDC_TRANSLUCENT), IDS_VVCFLAG);
    m_tooltip.AddTool(GetDlgItem(IDC_TRANSPARENT), IDS_VVCFLAG);
    m_tooltip.AddTool(GetDlgItem(IDC_SHADOW), IDS_VVCFLAG);
    m_tooltip.AddTool(GetDlgItem(IDC_BRIGHTEST), IDS_VVCFLAG);
    m_tooltip.AddTool(GetDlgItem(IDC_MIRROR), IDS_VVCFLAG);
    m_tooltip.AddTool(GetDlgItem(IDC_MIRROR2), IDS_VVCFLAG);
    m_tooltip.AddTool(GetDlgItem(IDC_GLOW), IDS_VVCFLAG);
    m_tooltip.AddTool(GetDlgItem(IDC_IGNOREGREY), IDS_VVCFLAG);
    m_tooltip.AddTool(GetDlgItem(IDC_IGNORERED), IDS_VVCFLAG);
    m_tooltip.AddTool(GetDlgItem(IDC_GREY), IDS_VVCFLAG);
    m_tooltip.AddTool(GetDlgItem(IDC_LIGHTEFFECT), IDS_VVCFLAG);
    m_tooltip.AddTool(GetDlgItem(IDC_BLEND), IDS_VVCFLAG);
    m_tooltip.AddTool(GetDlgItem(IDC_TINT), IDS_VVCFLAG);
    m_tooltip.AddTool(GetDlgItem(IDC_RED), IDS_VVCFLAG);
    m_tooltip.AddTool(GetDlgItem(IDC_SEQUENCING), IDS_SEQFLAG);
    m_tooltip.AddTool(GetDlgItem(IDC_LOOP), IDS_SEQFLAG);
    m_tooltip.AddTool(GetDlgItem(IDC_LIGHTSPOT), IDS_SEQFLAG);
    m_tooltip.AddTool(GetDlgItem(IDC_HEIGHT), IDS_SEQFLAG);
    m_tooltip.AddTool(GetDlgItem(IDC_USEBAM), IDS_SEQFLAG);
    m_tooltip.AddTool(GetDlgItem(IDC_PALETTE), IDS_SEQFLAG);
    m_tooltip.AddTool(GetDlgItem(IDC_WALL), IDS_SEQFLAG);
    tmpstr1.LoadString(IDS_FACEHINT);
    m_tooltip.AddTool(GetDlgItem(IDC_FACE), tmpstr1);
    m_tooltip.AddTool(GetDlgItem(IDC_UNKNOWN1C),IDS_UNUSED);
    m_tooltip.AddTool(GetDlgItem(IDC_UNKNOWN24),IDS_UNUSED);
    m_tooltip.AddTool(GetDlgItem(IDC_POS3),IDS_UNUSED);
  }	
	return TRUE;
}

void CVVCEdit::DefaultKillfocus() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CVVCEdit::OnKillfocusSequencing() 
{
  UpdateData(UD_RETRIEVE);
  RefreshDialog();
  UpdateData(UD_DISPLAY);
}

void CVVCEdit::OnLoop() 
{
	the_videocell.header.sequencing^=1;
  UpdateData(UD_DISPLAY);
}

void CVVCEdit::OnLightspot() 
{
	the_videocell.header.sequencing^=2;
  UpdateData(UD_DISPLAY);
}

void CVVCEdit::OnHeight() 
{
	the_videocell.header.sequencing^=4;
  UpdateData(UD_DISPLAY);
}

void CVVCEdit::OnUsebam() 
{
	the_videocell.header.sequencing^=8;
  UpdateData(UD_DISPLAY);
}

void CVVCEdit::OnPalette() 
{
	the_videocell.header.sequencing^=16;
  UpdateData(UD_DISPLAY);
}

void CVVCEdit::OnWall() 
{
	the_videocell.header.sequencing^=64;
  UpdateData(UD_DISPLAY);
}

void CVVCEdit::OnPos1() 
{
	the_videocell.header.position^=1;
  UpdateData(UD_DISPLAY);
}

void CVVCEdit::OnPos2() 
{
	the_videocell.header.position^=2;
  UpdateData(UD_DISPLAY);
}

void CVVCEdit::OnPos3() 
{
	the_videocell.header.position^=4;
  UpdateData(UD_DISPLAY);
}

void CVVCEdit::OnPos4() 
{
	the_videocell.header.position^=8;
  UpdateData(UD_DISPLAY);
}

void CVVCEdit::OnPlay1() 
{
  CString resref;

  RetrieveResref(resref, the_videocell.header.sound1);
  play_acm(resref,false,false);	
}

void CVVCEdit::OnPlay2() 
{
  CString resref;

  RetrieveResref(resref, the_videocell.header.sound2);
  play_acm(resref,false,false);	
}

void CVVCEdit::OnPlay3() 
{
  CString resref;

  RetrieveResref(resref, the_videocell.header.sound3);
  play_acm(resref,false,false);	
}

void CVVCEdit::OnBrowse() 
{
  pickerdlg.m_restype=REF_BAM;
  RetrieveResref(pickerdlg.m_picked,the_videocell.header.bam);
  if(pickerdlg.DoModal()==IDOK)
  {    
    StoreResref(pickerdlg.m_picked,the_videocell.header.bam);
  }
  UpdateData(UD_DISPLAY);	
}

void CVVCEdit::OnBrowse2() 
{
  pickerdlg.m_restype=REF_WAV;
  RetrieveResref(pickerdlg.m_picked,the_videocell.header.sound1);
  if(pickerdlg.DoModal()==IDOK)
  {    
    StoreResref(pickerdlg.m_picked,the_videocell.header.sound1);
  }
  UpdateData(UD_DISPLAY);	
}

void CVVCEdit::OnBrowse3() 
{
  pickerdlg.m_restype=REF_WAV;
  RetrieveResref(pickerdlg.m_picked,the_videocell.header.sound2);
  if(pickerdlg.DoModal()==IDOK)
  {    
    StoreResref(pickerdlg.m_picked,the_videocell.header.sound2);
  }
  UpdateData(UD_DISPLAY);	
}

void CVVCEdit::OnBrowse4() 
{
  pickerdlg.m_restype=REF_WAV;
  RetrieveResref(pickerdlg.m_picked,the_videocell.header.sound3);
  if(pickerdlg.DoModal()==IDOK)
  {    
    StoreResref(pickerdlg.m_picked,the_videocell.header.sound3);
  }
  UpdateData(UD_DISPLAY);	
}

void CVVCEdit::OnTransparent() 
{
	the_videocell.header.transparency^=1;
  UpdateData(UD_DISPLAY);
}

void CVVCEdit::OnTranslucent() 
{
	the_videocell.header.transparency^=2;
  if((the_videocell.header.transparency&0xa)==0xa)
  {
    the_videocell.header.transparency&=~8;
    MessageBox("Turning off brightest flag.","VVC editor",MB_OK);
  }
  UpdateData(UD_DISPLAY);
}

void CVVCEdit::OnShadow() 
{
	the_videocell.header.transparency^=4;
  UpdateData(UD_DISPLAY);
}

void CVVCEdit::OnBrightest() 
{
	the_videocell.header.transparency^=8;
  if((the_videocell.header.transparency&0xa)==0xa)
  {
    the_videocell.header.transparency&=~2;
    MessageBox("Turning off transparency flag.","VVC editor",MB_OK);
  }
  UpdateData(UD_DISPLAY);
}

void CVVCEdit::OnMirror() 
{
	the_videocell.header.transparency^=0x10;
  UpdateData(UD_DISPLAY);
}

void CVVCEdit::OnMirror2() 
{
	the_videocell.header.transparency^=0x20;
  UpdateData(UD_DISPLAY);
}

void CVVCEdit::OnGlow() 
{
	the_videocell.header.trans2^=2;
  UpdateData(UD_DISPLAY);
}

void CVVCEdit::OnIgnoregrey() 
{
	the_videocell.header.trans2^=8;
  UpdateData(UD_DISPLAY);
}

void CVVCEdit::OnIgnorered() 
{
	the_videocell.header.trans2^=0x10;
  UpdateData(UD_DISPLAY);
}

//this isn't blend?
void CVVCEdit::OnBlend() 
{
	the_videocell.header.colouring^=1;
  UpdateData(UD_DISPLAY);
}

void CVVCEdit::OnTint() 
{
	the_videocell.header.colouring^=2;
  UpdateData(UD_DISPLAY);
}

void CVVCEdit::OnGrey() 
{
	the_videocell.header.colouring^=8;
  UpdateData(UD_DISPLAY);
}

void CVVCEdit::OnLighteffect() 
{
	the_videocell.header.colouring^=0x20;
	UpdateData(UD_DISPLAY);
}

void CVVCEdit::OnRed() 
{
	the_videocell.header.col2^=2;
  UpdateData(UD_DISPLAY);
}

void CVVCEdit::OnUnknown() 
{
	MessageBox("Not implemented yet.","Unknown editor",MB_OK);
}

void CVVCEdit::OnCancel() 
{
  CString tmpstr;

  if(the_videocell.m_changed)
  {
    tmpstr.Format("Changes have been made to the videocell.\n"
      "Do you want to quit without save?\n");
    if(MessageBox(tmpstr,"Warning",MB_YESNO)==IDNO)
    {
      return;
    }
  }
	CDialog::OnCancel();
}

BOOL CVVCEdit::PreTranslateMessage(MSG* pMsg) 
{
  m_tooltip.RelayEvent(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}

