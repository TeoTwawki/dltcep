// ChuiEdit.cpp : implementation file
//

#include "stdafx.h"
#include <fcntl.h>
#include <sys/stat.h>

#include "chitem.h"
#include "chitemDlg.h"
#include "options.h"
#include "ChuiEdit.h"
#include "tbg.h"
#include "MyFileDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChuiEdit dialog

CChuiEdit::CChuiEdit(CWnd* pParent /*=NULL*/)	: CDialog(CChuiEdit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChuiEdit)
	m_text = _T("");
	//}}AFX_DATA_INIT
}

static int labelbits[9]={1,2,4,8,16,32,64,128,0};
static int buttonbits[9]={0,0,0,1,2,4,0,8,32};

static int winids[]={IDC_WINDOWPICKER,
0};
static int winboxids[]={IDC_ID, IDC_XPOS, IDC_YPOS, IDC_WIDTH, IDC_HEIGHT,
IDC_MOS, IDC_BACKGROUND, IDC_FLAGS, IDC_U1, IDC_U2, IDC_CTRLCNT, IDC_CONTROLPICKER,

//buttons
IDC_BROWSE, IDC_DELWIN, IDC_ADDCTRL,
0};

static int ctrlboxids[]={IDC_ID2, IDC_XPOS2, IDC_YPOS2, IDC_WIDTH2, IDC_HEIGHT2,
IDC_LENGTH, IDC_TYPE, IDC_U3, IDC_DELCTRL,
0};

static int buttonboxids[]={IDC_BAM, IDC_BROWSE2, IDC_CYCLE,
IDC_BAMFRAME1,IDC_BAMFRAME2, IDC_BAMFRAME3,IDC_BAMFRAME4,
IDC_BUTTONCYCLE, IDC_BUTTONBAM, IDC_BUTTONFLAG, IDC_BAM2,
IDC_FLAG4, IDC_FLAG5, IDC_FLAG6, IDC_FLAG8, IDC_FLAG9,
IDC_BUTTONFRAME1,IDC_BUTTONFRAME2, IDC_BUTTONFRAME3,IDC_BUTTONFRAME4,
0};

static int progressboxids[]={IDC_SLIDERMOS, IDC_BAM, IDC_BROWSE2,
IDC_BAM2, IDC_BROWSE3, IDC_CYCLE, IDC_BUTTONCYCLE, IDC_BAM3, IDC_BROWSE4, 
IDC_BAMFRAME3, IDC_STEPS, IDC_UNKNOWN1, IDC_UNKNOWN2, IDC_UNKNOWN3, IDC_UNKNOWN4,
0};

static int sliderboxids[]={IDC_BAM, IDC_BROWSE2, IDC_BAM2, IDC_BROWSE3,
IDC_SLIDERMOS, IDC_SLIDERBAM, IDC_CYCLE, IDC_SLIDERCYCLE,
IDC_BAMFRAME1, IDC_BAMFRAME2, IDC_UNKNOWN1, IDC_UNKNOWN2,
IDC_UNKNOWN3, IDC_UNKNOWN4,IDC_UNKNOWN5, IDC_UNKNOWN6,IDC_UNKNOWN7, IDC_UNKNOWN8,
IDC_SLIDERFRAME1, IDC_SLIDERFRAME2, IDC_UNKNOWN,
0};

static int editboxids[]={IDC_BAM, IDC_BAM2, IDC_BAM3, IDC_BAM4, IDC_BAM5,
IDC_BROWSE2,IDC_BROWSE3,IDC_BROWSE4,IDC_BROWSE5,IDC_BROWSE6, IDC_BAMFRAME6,
IDC_EDITMOS1,IDC_EDITMOS2,IDC_EDITMOS3,IDC_UNKNOWN4, IDC_UNKNOWN5, IDC_UNKNOWN6,
0};

static int textboxids[]={IDC_BAM, IDC_BAM2,IDC_BROWSE2, IDC_BROWSE3, IDC_TEXTAREA1,
IDC_TEXTAREA2, IDC_TEXTAREA3, IDC_TEXTAREA4, IDC_CYCLE, //cycle used for scrollbar ID
IDC_UNKNOWN1,IDC_UNKNOWN2,IDC_UNKNOWN3,IDC_UNKNOWN4,IDC_UNKNOWN5,IDC_UNKNOWN6,
IDC_UNKNOWN7,IDC_UNKNOWN8,IDC_UNKNOWN9,
0};

static int labelboxids[]={IDC_BAM, IDC_BROWSE2, IDC_CYCLE, IDC_TEXT,
IDC_LABEL1, IDC_LABEL2, IDC_LABEL3, IDC_TEXTAREA2, IDC_BAMFRAME6,
IDC_FLAG1,IDC_FLAG2,IDC_FLAG3,IDC_FLAG4,IDC_FLAG5,IDC_FLAG6,IDC_FLAG7,IDC_FLAG8,
IDC_UNKNOWN1,IDC_UNKNOWN2,IDC_UNKNOWN3,IDC_UNKNOWN4,IDC_UNKNOWN5,IDC_UNKNOWN6,
0};

static int scrollbarids[]={IDC_BAM, IDC_BROWSE2, IDC_CYCLE, IDC_BAM2, //using as textctrlid
IDC_BAMFRAME1,IDC_BAMFRAME2, IDC_BAMFRAME3,IDC_BAMFRAME4,IDC_BAMFRAME5,IDC_BAMFRAME6,
IDC_BUTTONCYCLE,IDC_SCROLL1,IDC_SCROLL2,IDC_SCROLL3,
0};

static int *allctrlids[]={buttonboxids,progressboxids,sliderboxids,editboxids,(int *) -1,
textboxids, labelboxids, scrollbarids,
0};

void CChuiEdit::DisplayControls(int type)
{
  CWnd *cw;
  int i,j;

  for(i=0;allctrlids[i];i++)
  {
    if(allctrlids[i]!=(int *)-1)
    {
      for(j=0;allctrlids[i][j];j++)
      {
        cw=GetDlgItem(allctrlids[i][j]);
        if(cw)
        {
          cw->ShowWindow(false);
        }
      }
    }
  }
  if(type<0) return;
  for(j=0;allctrlids[type][j];j++)
  {
    cw=GetDlgItem(allctrlids[type][j]);
    if(cw)
    {
      cw->ShowWindow(true);
    }
  }
}
void CChuiEdit::RefreshControls(CDataExchange* pDX, int type, int position)
{
  CString tmpstr;

  tmpstr=ChuiControlName(type);
  DDX_Text(pDX, IDC_EXTENSION, tmpstr);
  switch(type)
  {
  case -1: //none selected
    break;
  case CC_BUTTON:
    {
    int i,k;
    chui_button *cc=(chui_button *) the_chui.extensions[position];
    RetrieveResref(tmpstr,cc->bam);
    DDX_Text(pDX, IDC_BAM, tmpstr);
    StoreResref(tmpstr,cc->bam);
    DDX_Text(pDX, IDC_CYCLE, cc->cycle);
    DDX_Text(pDX, IDC_BAM2, cc->flags);
    DDX_Text(pDX, IDC_BAMFRAME1, cc->unpressed);
    DDX_Text(pDX, IDC_BAMFRAME2, cc->pressed);
    DDX_Text(pDX, IDC_BAMFRAME3, cc->disabled);
    DDX_Text(pDX, IDC_BAMFRAME4, cc->selected);
    for(i=0;i<9;i++)
    {
      k=!!(cc->flags&buttonbits[i]);
      DDX_Check(pDX,IDC_FLAG1+i,k);
    }
    }
    break;
  case CC_PROGRESS:
    {
    chui_progress *cc=(chui_progress *) the_chui.extensions[position];
    RetrieveResref(tmpstr,cc->mos1);
    DDX_Text(pDX, IDC_BAM, tmpstr);
    StoreResref(tmpstr,cc->mos1);
    RetrieveResref(tmpstr,cc->mos2);
    DDX_Text(pDX, IDC_BAM2, tmpstr);
    StoreResref(tmpstr,cc->mos2);
    RetrieveResref(tmpstr,cc->bam);
    DDX_Text(pDX, IDC_BAM3, tmpstr);
    StoreResref(tmpstr,cc->bam);
    DDX_Text(pDX, IDC_CYCLE, cc->cycle);
    DDX_Text(pDX, IDC_BAMFRAME3, cc->jumpcount);
    DDX_Text(pDX, IDC_UNKNOWN1, cc->xpos);
    DDX_Text(pDX, IDC_UNKNOWN2, cc->ypos);
    DDX_Text(pDX, IDC_UNKNOWN3, cc->xposcap);
    DDX_Text(pDX, IDC_UNKNOWN4, cc->yposcap);
    }
    break;
  case CC_SLIDER:
    {
    chui_slider *cc=(chui_slider *) the_chui.extensions[position];
    RetrieveResref(tmpstr,cc->mos);
    DDX_Text(pDX, IDC_BAM, tmpstr);
    StoreResref(tmpstr,cc->mos);
    DDX_Text(pDX, IDC_CYCLE, cc->cycle);
    RetrieveResref(tmpstr,cc->bam);
    DDX_Text(pDX, IDC_BAM2, tmpstr);
    StoreResref(tmpstr,cc->bam);
    DDX_Text(pDX,IDC_BAMFRAME1,cc->released);
    DDX_Text(pDX,IDC_BAMFRAME2,cc->grabbed);
    DDX_Text(pDX,IDC_UNKNOWN1,cc->xpos);
    DDX_Text(pDX,IDC_UNKNOWN2,cc->ypos);
    DDX_Text(pDX,IDC_UNKNOWN3,cc->jumpwidth);
    DDX_Text(pDX,IDC_UNKNOWN4,cc->jumpcount);
    DDX_Text(pDX,IDC_UNKNOWN5,cc->unknown5);
    DDX_Text(pDX,IDC_UNKNOWN6,cc->unknown6);
    DDX_Text(pDX,IDC_UNKNOWN7,cc->unknown7);
    DDX_Text(pDX,IDC_UNKNOWN8,cc->unknown8);
    }
    break;
  case CC_EDITBOX:
    {
    chui_editbox *cc=(chui_editbox *) the_chui.extensions[position];
    RetrieveResref(tmpstr,cc->mos1);
    DDX_Text(pDX, IDC_BAM, tmpstr);
    StoreResref(tmpstr,cc->mos1);

    RetrieveResref(tmpstr,cc->mos2);
    DDX_Text(pDX, IDC_BAM2, tmpstr);
    StoreResref(tmpstr,cc->mos2);

    RetrieveResref(tmpstr,cc->mos3);
    DDX_Text(pDX, IDC_BAM3, tmpstr);
    StoreResref(tmpstr,cc->mos3);

    RetrieveResref(tmpstr,cc->cursor);
    DDX_Text(pDX, IDC_BAM4, tmpstr);
    StoreResref(tmpstr,cc->cursor);

    RetrieveResref(tmpstr,cc->font);
    DDX_Text(pDX, IDC_BAM5, tmpstr);
    StoreResref(tmpstr,cc->font);

    DDX_Text(pDX, IDC_UNKNOWN4, cc->length);
    DDX_Text(pDX, IDC_UNKNOWN5, cc->xpos);
    DDX_Text(pDX, IDC_UNKNOWN6, cc->ypos);
    DDX_Text(pDX, IDC_BAMFRAME6, cc->uppercase);
    }
    break;
  case CC_TEXT:
    {
    chui_textarea *cc=(chui_textarea *) the_chui.extensions[position];
    RetrieveResref(tmpstr,cc->bam1);
    DDX_Text(pDX, IDC_BAM, tmpstr);
    StoreResref(tmpstr,cc->bam1);

    RetrieveResref(tmpstr,cc->bam2);
    DDX_Text(pDX, IDC_BAM2, tmpstr);
    StoreResref(tmpstr,cc->bam2);

    DDX_Text(pDX, IDC_CYCLE, cc->scrollcontrolid);
    DDX_Text(pDX, IDC_UNKNOWN1, cc->rgb1r);
    DDX_Text(pDX, IDC_UNKNOWN2, cc->rgb1g);
    DDX_Text(pDX, IDC_UNKNOWN3, cc->rgb1b);
    DDX_Text(pDX, IDC_UNKNOWN4, cc->rgb2r);
    DDX_Text(pDX, IDC_UNKNOWN5, cc->rgb2g);
    DDX_Text(pDX, IDC_UNKNOWN6, cc->rgb2b);
    DDX_Text(pDX, IDC_UNKNOWN7, cc->rgb3r);
    DDX_Text(pDX, IDC_UNKNOWN8, cc->rgb3g);
    DDX_Text(pDX, IDC_UNKNOWN9, cc->rgb3b);
    }
    break;
  case CC_LABEL:
    {
    int i,k;
    chui_label *cc=(chui_label *) the_chui.extensions[position];
    RetrieveResref(tmpstr,cc->font);
    DDX_Text(pDX, IDC_BAM, tmpstr);
    StoreResref(tmpstr,cc->font);

    DDX_Text(pDX, IDC_CYCLE, cc->strref);
    tmpstr=resolve_tlk_text(cc->strref);
    DDX_Text(pDX, IDC_TEXT, tmpstr);

    DDX_Text(pDX, IDC_UNKNOWN1, cc->rgb1r);
    DDX_Text(pDX, IDC_UNKNOWN2, cc->rgb1g);
    DDX_Text(pDX, IDC_UNKNOWN3, cc->rgb1b);
    DDX_Text(pDX, IDC_UNKNOWN4, cc->rgb2r);
    DDX_Text(pDX, IDC_UNKNOWN5, cc->rgb2g);
    DDX_Text(pDX, IDC_UNKNOWN6, cc->rgb2b);

    DDX_Text(pDX,IDC_BAMFRAME6, cc->justify);
    
    for(i=0;i<9;i++)
    {
      k=!!(cc->justify&labelbits[i]);
      DDX_Check(pDX,IDC_FLAG1+i,k);
    }
    }
    break;
  case CC_SCROLLBAR:
    {
    chui_scrollbar *cc=(chui_scrollbar *) the_chui.extensions[position];   
    RetrieveResref(tmpstr,cc->bam);
    DDX_Text(pDX, IDC_BAM, tmpstr);
    StoreResref(tmpstr,cc->bam);
    DDX_Text(pDX, IDC_CYCLE, cc->cycle);
    DDX_Text(pDX, IDC_BAMFRAME1, cc->upunpressed);
    DDX_Text(pDX, IDC_BAMFRAME2, cc->uppressed);
    DDX_Text(pDX, IDC_BAMFRAME3, cc->downunpressed);
    DDX_Text(pDX, IDC_BAMFRAME4, cc->downpressed);
    DDX_Text(pDX, IDC_BAMFRAME5, cc->trough);
    DDX_Text(pDX, IDC_BAMFRAME6, cc->slider);
    DDX_Text(pDX, IDC_BAM2, cc->textcontrolid);
    }
    break;
  case CC_U2:
  default:
    MessageBox("Unknown controltype!","Warning",MB_OK);
    break;
  }
}
void CChuiEdit::DoDataExchange(CDataExchange* pDX)
{
  CString tmpstr;
  CButton *cb;
  int pos, pos2, first;
  int flg, flg2;
  int i;

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChuiEdit)
	DDX_Control(pDX, IDC_CONTROLPICKER, m_controlpicker);
	DDX_Control(pDX, IDC_WINDOWPICKER, m_windowpicker);
	DDX_Text(pDX, IDC_TEXT, m_text);
	//}}AFX_DATA_MAP
  flg=the_chui.windowcnt;
  tmpstr.Format("/ %d",the_chui.windowcnt);
  DDX_Text(pDX, IDC_WINCNT, tmpstr);
  for(i=0;winids[i];i++)
  {
    GetDlgItem(winids[i])->EnableWindow(flg);
  }
  pos=m_windowpicker.GetCurSel();
  flg=flg && (pos>=0) && (pos<the_chui.windowcnt);

  for(i=0;winboxids[i];i++)
  {
    GetDlgItem(winboxids[i])->EnableWindow(flg);
  }

  if(flg)
  {
    DDX_Text(pDX,IDC_ID, the_chui.windows[pos].windowid);
    DDX_Text(pDX,IDC_XPOS, the_chui.windows[pos].xpos);
    DDX_Text(pDX,IDC_YPOS, the_chui.windows[pos].ypos);
    DDX_Text(pDX,IDC_WIDTH, the_chui.windows[pos].width);
    DDX_Text(pDX,IDC_HEIGHT, the_chui.windows[pos].height);

    RetrieveResref(tmpstr,the_chui.windows[pos].mos);
    DDX_Text(pDX,IDC_MOS,tmpstr);
    StoreResref(tmpstr,the_chui.windows[pos].mos);

    DDX_Text(pDX, IDC_FLAGS, the_chui.windows[pos].flags);
    cb=(CButton *) GetDlgItem(IDC_BACKGROUND);
    flg2=the_chui.windows[pos].flags&1;
    cb->SetCheck(flg2);
    GetDlgItem(IDC_MOS)->EnableWindow(flg&flg2);
    GetDlgItem(IDC_BROWSE)->EnableWindow(flg&flg2);

    DDX_Text(pDX, IDC_U1, the_chui.windows[pos].unknown1);
    DDX_Text(pDX, IDC_U2, the_chui.windows[pos].unknown2);

    tmpstr.Format("/ %d",the_chui.windows[pos].controlcount);
    DDX_Text(pDX, IDC_CTRLCNT, tmpstr);
    first=the_chui.windows[pos].controlidx;
  }
  else first=0;

  pos2=m_controlpicker.GetCurSel();
  flg=flg && (pos2>=0) && (pos2+first<the_chui.controlcnt);
  for(i=0;ctrlboxids[i];i++)
  {
    GetDlgItem(ctrlboxids[i])->EnableWindow(flg);
  }

  if(flg)
  {
    pos2+=first;
    DDX_Text(pDX, IDC_ID2, the_chui.controls[pos2].controlid);
    DDX_Text(pDX,IDC_XPOS2, the_chui.controls[pos2].xpos);
    DDX_Text(pDX,IDC_YPOS2, the_chui.controls[pos2].ypos);
    DDX_Text(pDX,IDC_WIDTH2, the_chui.controls[pos2].width);
    DDX_Text(pDX,IDC_HEIGHT2, the_chui.controls[pos2].height);
    DDX_Text(pDX, IDC_LENGTH, the_chui.controls[pos2].idflags);
    DDX_Text(pDX, IDC_U3, the_chui.controls[pos2].unknown);
    first=the_chui.controls[pos2].controltype;
  	DDX_CBIndex(pDX, IDC_TYPE, first);
    the_chui.controls[pos2].controltype=(char) first;
  }
  else first=-1;
  RefreshControls(pDX, first, pos2);
}

BEGIN_MESSAGE_MAP(CChuiEdit, CDialog)
	//{{AFX_MSG_MAP(CChuiEdit)
	ON_COMMAND(ID_FILE_LOAD, OnLoad)
	ON_COMMAND(ID_FILE_LOADEXTERNAL, OnLoadex)
	ON_COMMAND(ID_FILE_SAVEAS, OnSaveas)
	ON_COMMAND(ID_FILE_NEW, OnNew)
	ON_COMMAND(ID_CHECK, OnCheck)
	ON_CBN_SELCHANGE(IDC_WINDOWPICKER, OnSelchangeWindowpicker)
	ON_BN_CLICKED(IDC_ADDWIN, OnAddwin)
	ON_BN_CLICKED(IDC_DELWIN, OnDelwin)
	ON_COMMAND(ID_FILE_SAVE, OnSave)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_BACKGROUND, OnBackground)
	ON_CBN_SELCHANGE(IDC_CONTROLPICKER, OnSelchangeControlpicker)
	ON_CBN_KILLFOCUS(IDC_TYPE, OnKillfocusType)
	ON_BN_CLICKED(IDC_ADDCTRL, OnAddctrl)
	ON_BN_CLICKED(IDC_DELCTRL, OnDelctrl)
	ON_BN_CLICKED(IDC_BROWSE2, OnBrowse2)
	ON_BN_CLICKED(IDC_BROWSE3, OnBrowse3)
	ON_BN_CLICKED(IDC_PREVIEW, OnPreview)
	ON_BN_CLICKED(IDC_BROWSE4, OnBrowse4)
	ON_BN_CLICKED(IDC_BROWSE5, OnBrowse5)
	ON_BN_CLICKED(IDC_BROWSE6, OnBrowse6)
	ON_EN_KILLFOCUS(IDC_TEXT, OnKillfocusText)
	ON_COMMAND(ID_FILE_TBG, OnFileTbg)
	ON_COMMAND(ID_FILE_TP2, OnFileTp2)
	ON_BN_CLICKED(IDC_FLAG1, OnFlag1)
	ON_BN_CLICKED(IDC_FLAG2, OnFlag2)
	ON_BN_CLICKED(IDC_FLAG3, OnFlag3)
	ON_BN_CLICKED(IDC_FLAG4, OnFlag4)
	ON_BN_CLICKED(IDC_FLAG5, OnFlag5)
	ON_BN_CLICKED(IDC_FLAG6, OnFlag6)
	ON_BN_CLICKED(IDC_FLAG7, OnFlag7)
	ON_BN_CLICKED(IDC_FLAG8, OnFlag8)
	ON_BN_CLICKED(IDC_FLAG9, OnFlag9)
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_BN_CLICKED(IDC_LOADEX, OnLoadex)
	ON_BN_CLICKED(IDC_SAVEAS, OnSaveas)
	ON_BN_CLICKED(IDC_NEW, OnNew)
	ON_BN_CLICKED(IDC_CHECK, OnCheck)
	ON_EN_KILLFOCUS(IDC_UNKNOWN1, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_UNKNOWN2, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_UNKNOWN3, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_UNKNOWN4, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_UNKNOWN5, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_UNKNOWN6, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_UNKNOWN7, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_UNKNOWN8, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_UNKNOWN9, DefaultKillfocus)
	ON_CBN_KILLFOCUS(IDC_WINDOWPICKER, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_XPOS, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_YPOS, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_WIDTH, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_HEIGHT, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_ID, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_MOS, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_FLAGS, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U1, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U2, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_XPOS2, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_YPOS2, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_WIDTH2, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_HEIGHT2, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_LENGTH, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_U3, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_BAM, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_CYCLE, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_BAMFRAME1, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_BAMFRAME2, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_BAMFRAME3, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_BAMFRAME4, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_BAM3, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_BAM2, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_BAM4, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_BAM5, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_BAMFRAME5, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_BAMFRAME6, DefaultKillfocus)
	ON_EN_KILLFOCUS(IDC_ID2, OnKillfocusId2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CChuiEdit::NewChui()
{
	itemname="new chui";
  the_chui.KillWindows();
  memset(&the_chui.header,0,sizeof(chui_header));
}

void CChuiEdit::RefreshDialog()
{
  int pos, pos2, first;
  CString tmpstr;
  int i;

  SetWindowText("Edit user interface: "+itemname);
  if(m_windowpicker)
  {
    pos=m_windowpicker.GetCurSel();
    if(pos<0) pos=0;
    m_windowpicker.ResetContent();
    for(i=0;i<the_chui.windowcnt;i++)
    {
      tmpstr.Format("%d %d",i+1, the_chui.windows[i].windowid);
      m_windowpicker.AddString(tmpstr);
    }
    if(pos>=i) pos=i-1;
    pos=m_windowpicker.SetCurSel(pos);
  }
  else pos=-1;
  
  first=-1;
  if(pos>=0)
  {
    pos2=m_controlpicker.GetCurSel();
    if(pos2<0) pos2=0;
    m_controlpicker.ResetContent();
    for(i=0;i<the_chui.windows[pos].controlcount;i++)
    {
      first=the_chui.windows[pos].controlidx;
      tmpstr.Format("%d %d %s",i+1, the_chui.controls[first+i].controlid,
        ChuiControlName(the_chui.controls[first+i].controltype));
      m_controlpicker.AddString(tmpstr);
    }
    if(pos2>=i) pos2=i-1;
    pos2=m_controlpicker.SetCurSel(pos2);
    if(pos2>=0)
    {
      if(first!=-1)
      {
        DisplayControls(the_chui.controls[first+pos2].controltype);
        return;
      }
    }
  }
  DisplayControls(first);
}

/////////////////////////////////////////////////////////////////////////////
// CChuiEdit message handlers

void CChuiEdit::OnLoad() 
{
	int res;
	
  pickerdlg.m_restype=REF_CHU;
  pickerdlg.m_picked=itemname;
	res=pickerdlg.DoModal();
	if(res==IDOK)
	{
		res=read_chui(pickerdlg.m_picked);
    switch(res)
    {
    case -4:
      MessageBox("User interface loaded with errors.","Warning",MB_ICONEXCLAMATION|MB_OK);
  		itemname=pickerdlg.m_picked;
      break;
    case 2:
      MessageBox("User interface has size problems (corrected).","Warning",MB_ICONEXCLAMATION|MB_OK);
  		itemname=pickerdlg.m_picked;
      break;
    case 1:
      MessageBox("User interface will be reordered (harmless inconsistency).","Warning",MB_ICONEXCLAMATION|MB_OK);
  		itemname=pickerdlg.m_picked;
      break;
    case 0:
  		itemname=pickerdlg.m_picked;
      break;
    default:
      MessageBox("Cannot read user interface!","Error",MB_ICONSTOP|MB_OK);
      NewChui();
      break;
    }
    RefreshDialog();
    UpdateData(UD_DISPLAY);
	}
}

static char BASED_CODE szFilter[] = "Chui files (*.chu)|*.chu|All files (*.*)|*.*||";

void CChuiEdit::OnLoadex() 
{
  CString filepath;
  int fhandle;
  int res;
  
  res=OFN_FILEMUSTEXIST|OFN_ENABLESIZING|OFN_EXPLORER;
  if(readonly) res|=OFN_READONLY;  
  CMyFileDialog m_getfiledlg(TRUE, "chu", makeitemname(".chu",0), res, szFilter);

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
    res=the_chui.ReadChuiFromFile(fhandle,-1);
    close(fhandle);
    lastopenedoverride=filepath.Left(filepath.ReverseFind('\\'));
    switch(res)
    {
    case -4:
      MessageBox("User interface loaded with errors.","Warning",MB_ICONEXCLAMATION|MB_OK);
      itemname=m_getfiledlg.GetFileTitle();
      itemname.MakeUpper();
      break;
    case 2: case 3:
      MessageBox("User interface is crippled (size corrected).","Warning",MB_ICONEXCLAMATION|MB_OK);
      itemname=m_getfiledlg.GetFileTitle();
      itemname.MakeUpper();
      break;
    case 1:
      MessageBox("User interface will be reordered (harmless inconsistency).","Warning",MB_ICONEXCLAMATION|MB_OK);
      itemname=m_getfiledlg.GetFileTitle();
      itemname.MakeUpper();
      break;
    case 0:
      itemname=m_getfiledlg.GetFileTitle();
      itemname.MakeUpper();
      break;
    default:
      MessageBox("Cannot read user interface!","Error",MB_ICONSTOP|MB_OK);
      NewChui();
      break;
    }
    RefreshDialog();
    UpdateData(UD_DISPLAY);
  }
}

void CChuiEdit::OnSave() 
{
  SaveChui(1);
}

void CChuiEdit::OnSaveas() 
{
  SaveChui(0);
}

void CChuiEdit::SaveChui(int save) 
{
  CString filepath;
  CString newname;
  CString tmpstr;
  int chrorcre;
  int res;

  if(readonly)
  {
    MessageBox("You opened it read only!","Warning",MB_ICONEXCLAMATION|MB_OK);
    return;
  }
  res=OFN_HIDEREADONLY|OFN_ENABLESIZING|OFN_EXPLORER;
  CMyFileDialog m_getfiledlg(FALSE, "chu", makeitemname(".chu",0), res, szFilter);

  if(save)
  {
    newname=itemname;
    filepath=makeitemname(".chu",0);
    goto gotname;
  }    
restart:
  chrorcre=0;
  if( m_getfiledlg.DoModal() == IDOK )
  {
    filepath=m_getfiledlg.GetPathName();
    filepath.MakeLower();
    if(filepath.Right(4)!=".chu")
    {
      filepath+=".chu";
    }
    newname=m_getfiledlg.GetFileName();
    newname.MakeUpper();
    if(newname.Right(4)==".CHU") newname=newname.Left(newname.GetLength()-4);
gotname:
    if(newname.GetLength()>8 || newname.GetLength()<1 || newname.FindOneOf(" *")!=-1)
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
    
    res=write_chui(newname, filepath);
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

void CChuiEdit::OnNew() 
{
  NewChui();
  RefreshDialog();
  UpdateData(UD_DISPLAY);
}

void CChuiEdit::OnCheck() 
{
  int ret;

	ret=((CChitemDlg *) AfxGetMainWnd())->check_chui();
  if(ret)
  {
    MessageBox(lasterrormsg,"User Interface editor",MB_ICONEXCLAMATION|MB_OK);
  }
}

BOOL CChuiEdit::OnInitDialog() 
{
  CString tmpstr, tmpstr1, tmpstr2;
  CComboBox *cb2;
  int i;

	CDialog::OnInitDialog();	

  cb2=(CComboBox *) GetDlgItem(IDC_TYPE);
  for(i=0;i<CCNUM;i++)
  {
    cb2->AddString(ChuiControlName(i));
  }

  //tooltips
  {
    m_tooltip.Create(this,TTS_NOPREFIX);
    m_tooltip.SetMaxTipWidth(200);
    m_tooltip.SetTipBkColor(RGB(240,224,160));
    
    m_tooltip.AddTool(GetDlgItem(IDCANCEL), IDS_CANCEL);
    tmpstr1.LoadString(IDS_LOAD);
    tmpstr2.LoadString(IDS_CHU);
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
  RefreshDialog();
  return TRUE;
}

void CChuiEdit::OnSelchangeWindowpicker() 
{
  RefreshDialog();
	UpdateData(UD_DISPLAY);
}

void CChuiEdit::OnSelchangeControlpicker() 
{
  RefreshDialog();
	UpdateData(UD_DISPLAY);
}

void CChuiEdit::OnKillfocusId2() 
{
	UpdateData(UD_RETRIEVE);
  RefreshDialog();
	UpdateData(UD_DISPLAY);
}

void CChuiEdit::DefaultKillfocus() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);	
}

void CChuiEdit::OnBackground() 
{
  int pos;

  pos=m_windowpicker.GetCurSel();
  if(pos>=0)
  {
	  the_chui.windows[pos].flags^=1;
  }
  UpdateData(UD_DISPLAY);
}

void CChuiEdit::OnBrowse() 
{
  int pos;
  
  pos=m_windowpicker.GetCurSel();
  if(pos>=0)
  {
    pickerdlg.m_restype=REF_MOS;
    RetrieveResref(pickerdlg.m_picked,the_chui.windows[pos].mos);
    if(pickerdlg.DoModal()==IDOK)
    {
      StoreResref(pickerdlg.m_picked,the_chui.windows[pos].mos);
    }
  }
  UpdateData(UD_DISPLAY);
}

int CChuiEdit::GetCurrentControlType(int &pos2, char *&oldtype)
{
  CComboBox *cb;
  int pos;

  pos=m_windowpicker.GetCurSel();
  if(pos<0) return -1;
  pos2=m_controlpicker.GetCurSel();
  if(pos2<0) return -1;
  pos2+=the_chui.windows[pos].controlidx;
  oldtype=&the_chui.controls[pos2].controltype;
  cb=(CComboBox *) GetDlgItem(IDC_TYPE);
  return cb->GetCurSel();
}

void CChuiEdit::OnKillfocusText() 
{
	char *dummy;
  int pos2;
	CString old;

  GetCurrentControlType(pos2, dummy); //type==*dummy
  chui_label *cc=(chui_label *) the_chui.extensions[pos2];

  UpdateData(UD_RETRIEVE);
  old=resolve_tlk_text(cc->strref);
  if(old!=m_text)
  {
    if(editflg&TLKCHANGE)
    {
      if(MessageBox("Do you want to update dialog.tlk?","Chui editor",MB_YESNO)!=IDYES)
      {
        return;
      }
    }
    cc->strref=store_tlk_text(cc->strref, m_text);
  }
  UpdateData(UD_DISPLAY);
}

void CChuiEdit::OnKillfocusType() 
{
  BYTE *newextension;
  char *oldtype;
  int pos2;
  int newtype;
  int oldsize, newsize;

  newtype=GetCurrentControlType(pos2, oldtype);
  if(*oldtype==-1) return;
  newsize=ChuiControlSize(newtype);
  if(newtype==-1 || newtype==*oldtype) return; //didn't change
  if(newsize>=0)
  {
    newextension=new BYTE[newsize];
    oldsize=ChuiControlSize(*oldtype);
    if(oldsize>newsize) oldsize=newsize;
    memset(newextension,0,newsize);
    memcpy(newextension,the_chui.extensions[pos2],oldsize);
    delete [] the_chui.extensions[pos2];
    the_chui.extensions[pos2]=newextension;
    the_chui.controltable[pos2].y=newsize+sizeof(chui_control_common);
    *oldtype=(char) newtype;
  }
  RefreshDialog();
	UpdateData(UD_DISPLAY);	
}

void CChuiEdit::OnAddwin() 
{
	chui_window *newwindows;
  int first;

	newwindows=new chui_window[the_chui.windowcnt+1];
  if(!newwindows)
  {
    return;
  }
  memcpy(newwindows,the_chui.windows,the_chui.windowcnt*sizeof(chui_window) );
  memset(newwindows+the_chui.windowcnt,0,sizeof(chui_window) );
  if(the_chui.windowcnt)
  {
    first=newwindows[the_chui.windowcnt-1].controlidx+newwindows[the_chui.windowcnt-1].controlcount;
  }
  else
  {
    first=0;
  }
  newwindows[the_chui.windowcnt].controlidx=(short) first;

  delete [] the_chui.windows;
  the_chui.windows=newwindows;

  m_windowpicker.AddString(""); //fake
  m_windowpicker.SetCurSel(the_chui.windowcnt);
  the_chui.header.windowcnt=++the_chui.windowcnt;
  RefreshDialog();
  UpdateData(UD_DISPLAY);
}

int CChuiEdit::DeleteControls(int firstcontrol, int controlcount)
{
  chui_control_common *newcontrols;
  BYTE **newextensions;
  POINT *newtable;
  int i;

  if(firstcontrol<0 || controlcount<0) return -1;
  if(the_chui.controlcnt<firstcontrol+controlcount)
  {
    return -1;
  }
  newcontrols=new chui_control_common[the_chui.controlcnt-controlcount];
  if(!newcontrols) return -3;
  newextensions=new BYTE *[the_chui.controlcnt-controlcount];
  if(!newextensions)
  {
    delete [] newcontrols;
    return -3;
  }
  newtable=new POINT[the_chui.controlcnt-controlcount];
  if(!newtable)
  {
    delete [] newcontrols;
    delete [] newextensions;
    return -3;
  }

  memcpy(newtable, the_chui.controltable, firstcontrol*sizeof(POINT) );
  memcpy(newtable+firstcontrol, the_chui.controltable+firstcontrol+controlcount,(the_chui.controlcnt-firstcontrol-controlcount)*sizeof(POINT));
  delete [] the_chui.controltable;
  the_chui.controltable=newtable;

  for(i=firstcontrol;i<firstcontrol+controlcount;i++)
  {
    delete [] the_chui.extensions[i];
  }
  memcpy(newextensions, the_chui.extensions,firstcontrol*sizeof(BYTE *) );
  memcpy(newextensions+firstcontrol,the_chui.extensions+firstcontrol+controlcount,(the_chui.controlcnt-firstcontrol-controlcount)*sizeof(BYTE *));

  delete [] the_chui.extensions;
  the_chui.extensions=newextensions;

  memcpy(newcontrols,the_chui.controls,firstcontrol*sizeof(chui_control_common));
  memcpy(newcontrols+firstcontrol,the_chui.controls+firstcontrol+controlcount,(the_chui.controlcnt-firstcontrol-controlcount)*sizeof(chui_control_common));

  delete [] the_chui.controls;
  the_chui.controls=newcontrols;

  the_chui.controlcnt-=controlcount;
  return 0;
}

void CChuiEdit::OnDelwin() 
{
	chui_window *newwindows;
  int i,cnt;
  int windowcnt;
	
  windowcnt=m_windowpicker.GetCurSel();
  if(windowcnt<0) return;

  //removal of controls
  cnt=the_chui.windows[windowcnt].controlcount;
  if(DeleteControls(the_chui.windows[windowcnt].controlidx,cnt)) return;
  for(i=windowcnt+1;i<the_chui.windowcnt;i++)
  {
    the_chui.windows[i].controlidx=(short) (the_chui.windows[i].controlidx-cnt);
  }

  newwindows= new chui_window[the_chui.windowcnt-1];
  if(!newwindows)
  {
    return;
  }
  memcpy(newwindows,the_chui.windows,windowcnt*sizeof(chui_window));
  memcpy(newwindows+windowcnt,the_chui.windows+windowcnt+1,(the_chui.windowcnt-windowcnt-1) *sizeof(chui_window) );

  delete [] the_chui.windows;
  the_chui.windows=newwindows;

  the_chui.header.windowcnt=--the_chui.windowcnt;
  RefreshDialog();
  UpdateData(UD_DISPLAY);
}

void CChuiEdit::OnAddctrl() 
{
  chui_control_common *newcontrols;
  BYTE **newextensions;
  POINT *newtable;
  int i;
  int windowcnt, controlcnt, pos;

  windowcnt=m_windowpicker.GetCurSel();
  if(windowcnt<0) return;
  controlcnt=m_controlpicker.GetCurSel()+1;
  if(controlcnt>the_chui.windows[windowcnt].controlcount) return;
  pos=the_chui.windows[windowcnt].controlidx+controlcnt;
  if(pos>the_chui.controlcnt) return;
	
  newcontrols=new chui_control_common[the_chui.controlcnt+1];
  if(!newcontrols) return;
  newextensions=new BYTE *[the_chui.controlcnt+1];
  if(!newextensions)
  {
    delete [] newcontrols;
    return;
  }
  newtable=new POINT[the_chui.controlcnt+1];
  if(!newtable)
  {
    delete [] newcontrols;
    delete [] newextensions;
    return;
  }

  i=ChuiControlSize(0)*sizeof(BYTE);
  //copying the control table (sizes)
  memcpy(newtable, the_chui.controltable, pos * sizeof(POINT) );
  memcpy(newtable+pos+1, the_chui.controltable+pos, (the_chui.controlcnt-pos)*sizeof(POINT) );
  //setting up the new size
  newtable[pos].y=i+sizeof(chui_control_common);
  //copying the extension pointers
  memcpy(newextensions,the_chui.extensions,pos*sizeof(BYTE *) );
  memcpy(newextensions+pos+1,the_chui.extensions+pos,(the_chui.controlcnt-pos)*sizeof(BYTE *) );
  //creating the new extension
  newextensions[pos]=new BYTE[i]; //the zero type control is a button
  //lets hope it is successfully allocated;
  if(!newextensions[pos])
  {
    delete [] newcontrols;
    delete [] newextensions;
    delete [] newtable;
    return;
  }
  memset(newextensions[pos],0,i);

  delete [] the_chui.controltable;
  the_chui.controltable=newtable;

  delete [] the_chui.extensions;
  the_chui.extensions=newextensions;

  memcpy(newcontrols,the_chui.controls, pos*sizeof(chui_control_common) );
  memcpy(newcontrols+pos+1,the_chui.controls+pos,(the_chui.controlcnt-pos) *sizeof(chui_control_common) );
  memset(newcontrols+pos,0,sizeof(chui_control_common)); //setting this control to empty
  newcontrols[pos].controlid=(short) pos;

  delete [] the_chui.controls;
  the_chui.controls=newcontrols;

  m_controlpicker.AddString(""); //fake
  m_controlpicker.SetCurSel(the_chui.windows[windowcnt].controlcount);
  the_chui.windows[windowcnt].controlcount++;
  for(i=windowcnt+1;i<the_chui.windowcnt;i++)
  {
    the_chui.windows[i].controlidx++;
  }
  the_chui.controlcnt++;
  RefreshDialog();
  UpdateData(UD_DISPLAY);
}

void CChuiEdit::OnDelctrl() 
{
  int i;
  int windowcnt, controlcnt;

  windowcnt=m_windowpicker.GetCurSel();
  if(windowcnt<0) return;
  controlcnt=m_controlpicker.GetCurSel();
  if(controlcnt<0 || controlcnt>=the_chui.windows[windowcnt].controlcount) return;
  DeleteControls(the_chui.windows[windowcnt].controlidx+controlcnt,1);
  for(i=windowcnt+1;i<the_chui.windowcnt;i++)
  {
    the_chui.windows[i].controlidx--;
  }
  the_chui.windows[windowcnt].controlcount--;
  RefreshDialog();
  UpdateData(UD_DISPLAY);
}

void CChuiEdit::OnBrowse2()  //browsing into the first resref
{
	char *dummy;
  int pos2;
  int type;
	
  type=GetCurrentControlType(pos2, dummy); //type==*dummy
  switch(type)
  {
  case CC_BUTTON:
    {
    chui_button *cc=(chui_button *) the_chui.extensions[pos2];

    pickerdlg.m_restype=REF_BAM;
    RetrieveResref(pickerdlg.m_picked,cc->bam);
    if(pickerdlg.DoModal()==IDOK)
    {
      StoreResref(pickerdlg.m_picked,cc->bam);
    }
    }
    break;
  case CC_PROGRESS:
    {
    chui_progress *cc=(chui_progress *) the_chui.extensions[pos2];

    pickerdlg.m_restype=REF_MOS;
    RetrieveResref(pickerdlg.m_picked,cc->mos1);
    if(pickerdlg.DoModal()==IDOK)
    {
      StoreResref(pickerdlg.m_picked,cc->mos1);
    }
    }
    break;
  case CC_SLIDER:
    {
    chui_slider *cc=(chui_slider *) the_chui.extensions[pos2];

    pickerdlg.m_restype=REF_MOS;
    RetrieveResref(pickerdlg.m_picked,cc->mos);
    if(pickerdlg.DoModal()==IDOK)
    {
      StoreResref(pickerdlg.m_picked,cc->mos);
    }
    }
    break;
  case CC_EDITBOX:
    {
    chui_editbox *cc=(chui_editbox *) the_chui.extensions[pos2];

    pickerdlg.m_restype=REF_MOS;
    RetrieveResref(pickerdlg.m_picked,cc->mos1);
    if(pickerdlg.DoModal()==IDOK)
    {
      StoreResref(pickerdlg.m_picked,cc->mos1);
    }
    }
    break;
  case CC_TEXT:
    {
    chui_textarea *cc=(chui_textarea *) the_chui.extensions[pos2];

    pickerdlg.m_restype=REF_BAM;
    RetrieveResref(pickerdlg.m_picked,cc->bam1);
    if(pickerdlg.DoModal()==IDOK)
    {
      StoreResref(pickerdlg.m_picked,cc->bam1);
    }
    }
    break;
  case CC_LABEL:
    {
    chui_label *cc=(chui_label *) the_chui.extensions[pos2];

    pickerdlg.m_restype=REF_BAM;
    RetrieveResref(pickerdlg.m_picked,cc->font);
    if(pickerdlg.DoModal()==IDOK)
    {
      StoreResref(pickerdlg.m_picked,cc->font);
    }
    }
    break;
  case CC_SCROLLBAR:
    {
    chui_scrollbar *cc=(chui_scrollbar *) the_chui.extensions[pos2];

    pickerdlg.m_restype=REF_BAM;
    RetrieveResref(pickerdlg.m_picked,cc->bam);
    if(pickerdlg.DoModal()==IDOK)
    {
      StoreResref(pickerdlg.m_picked,cc->bam);
    }
    }
    break;
  }
  UpdateData(UD_DISPLAY);
}

void CChuiEdit::OnBrowse3() //browsing into the second resref
{
	char *dummy;
  int pos2;
  int type;
	
  type=GetCurrentControlType(pos2, dummy); //type==*dummy
  switch(type)
  {
  case CC_PROGRESS:
    {
    chui_progress *cc=(chui_progress *) the_chui.extensions[pos2];

    pickerdlg.m_restype=REF_MOS;
    RetrieveResref(pickerdlg.m_picked,cc->mos2);
    if(pickerdlg.DoModal()==IDOK)
    {
      StoreResref(pickerdlg.m_picked,cc->mos2);
    }
    }
    break;
  case CC_SLIDER:
    {
    chui_slider *cc=(chui_slider *) the_chui.extensions[pos2];

    pickerdlg.m_restype=REF_BAM;
    RetrieveResref(pickerdlg.m_picked,cc->bam);
    if(pickerdlg.DoModal()==IDOK)
    {
      StoreResref(pickerdlg.m_picked,cc->bam);
    }
    }
    break;
  case CC_EDITBOX:
    {
    chui_editbox *cc=(chui_editbox *) the_chui.extensions[pos2];

    pickerdlg.m_restype=REF_MOS;
    RetrieveResref(pickerdlg.m_picked,cc->mos2);
    if(pickerdlg.DoModal()==IDOK)
    {
      StoreResref(pickerdlg.m_picked,cc->mos2);
    }
    }
    break;
  case CC_TEXT:
    {
    chui_textarea *cc=(chui_textarea *) the_chui.extensions[pos2];

    pickerdlg.m_restype=REF_BAM;
    RetrieveResref(pickerdlg.m_picked,cc->bam2);
    if(pickerdlg.DoModal()==IDOK)
    {
      StoreResref(pickerdlg.m_picked,cc->bam2);
    }
    }
    break;
  }
  UpdateData(UD_DISPLAY);
}

void CChuiEdit::OnBrowse4() 
{
	char *dummy;
  int pos2;
  int type;
	
  type=GetCurrentControlType(pos2, dummy); //type==*dummy
  switch(type)
  {
  case CC_PROGRESS:
    {
    chui_progress *cc=(chui_progress *) the_chui.extensions[pos2];

    if(cc->jumpcount)
    {
      pickerdlg.m_restype=REF_BAM;
    }
    else
    {
      pickerdlg.m_restype=REF_MOS;
    }
    RetrieveResref(pickerdlg.m_picked,cc->bam);
    if(pickerdlg.DoModal()==IDOK)
    {
      StoreResref(pickerdlg.m_picked,cc->bam);
    }
    }
    break;
  case CC_EDITBOX:
    {
    chui_editbox *cc=(chui_editbox *) the_chui.extensions[pos2];

    pickerdlg.m_restype=REF_MOS;
    RetrieveResref(pickerdlg.m_picked,cc->mos3);
    if(pickerdlg.DoModal()==IDOK)
    {
      StoreResref(pickerdlg.m_picked,cc->mos3);
    }
    }
    break;
  }
  UpdateData(UD_DISPLAY);
}

void CChuiEdit::OnBrowse5() 
{
	char *dummy;
  int pos2;
  int type;
	
  type=GetCurrentControlType(pos2, dummy); //type==*dummy
  switch(type)
  {
  case CC_EDITBOX:
    {
    chui_editbox *cc=(chui_editbox *) the_chui.extensions[pos2];

    pickerdlg.m_restype=REF_BAM;
    RetrieveResref(pickerdlg.m_picked,cc->cursor);
    if(pickerdlg.DoModal()==IDOK)
    {
      StoreResref(pickerdlg.m_picked,cc->cursor);
    }
    }
    break;
  }
  UpdateData(UD_DISPLAY);
}

void CChuiEdit::OnBrowse6() //third resref
{
	char *dummy;
  int pos2;
  int type;
	
  type=GetCurrentControlType(pos2, dummy); //type==*dummy
  switch(type)
  {
  case CC_EDITBOX:
    {
    chui_editbox *cc=(chui_editbox *) the_chui.extensions[pos2];

    pickerdlg.m_restype=REF_BAM;
    RetrieveResref(pickerdlg.m_picked,cc->font);
    if(pickerdlg.DoModal()==IDOK)
    {
      StoreResref(pickerdlg.m_picked,cc->font);
    }
    }
    break;
  }
  UpdateData(UD_DISPLAY);
}

void CChuiEdit::OnLabelFlag(int flag)
{
	char *dummy;
  int pos2;
  int type;

  type=GetCurrentControlType(pos2, dummy); //type==*dummy
  switch(type)
  {
  case CC_LABEL:
    {
      chui_label *cc=(chui_label *) the_chui.extensions[pos2];
      cc->justify^=labelbits[flag];
    }
    break;
  case CC_BUTTON:
    {
      chui_button *cc=(chui_button *) the_chui.extensions[pos2];
      cc->flags^=buttonbits[flag];
    }
    break;
  }
  UpdateData(UD_DISPLAY);
}

void CChuiEdit::OnFlag1() 
{
	OnLabelFlag(0);	
}

void CChuiEdit::OnFlag2() 
{
	OnLabelFlag(1);	
}

void CChuiEdit::OnFlag3() 
{
	OnLabelFlag(2);	
}

void CChuiEdit::OnFlag4() 
{
	OnLabelFlag(3);		
}

void CChuiEdit::OnFlag5() 
{
	OnLabelFlag(4);	
}

void CChuiEdit::OnFlag6() 
{
	OnLabelFlag(5);	
}

void CChuiEdit::OnFlag7() 
{
	OnLabelFlag(6);	
}

void CChuiEdit::OnFlag8() 
{
	OnLabelFlag(7);	
}

void CChuiEdit::OnFlag9() 
{
	OnLabelFlag(8);
}

void CChuiEdit::OnPreview() 
{
  CImageView preview;
  Cmos tmp_mos;
  Cbam tmp_bam;
  CPoint point;
  CString tmpstr;
	int pos, pos2;
  int frame, length, end;
  int act, mode, rgb;
  int i;
  
	pos=m_windowpicker.GetCurSel();
  if(pos<0) return;
  RetrieveResref(tmpstr,the_chui.windows[pos].mos);
  if(read_mos(tmpstr,&the_mos,true))
  {
    MessageBox("Cannot load the base mos.","User Interface editor",MB_ICONEXCLAMATION|MB_OK);
    return;
  }

  //we prepare the initview by drawing the window
  preview.m_maxextentx=the_mos.mosheader.wColumn;
  preview.m_maxextenty=the_mos.mosheader.wRow;
  preview.InitView(IW_ENABLEBUTTON|IW_NOREDRAW, &the_mos);
  the_mos.MakeBitmapWhole(GREY,preview.m_bm,0,0,0,0,true);

  //then we put the controls on it
  act=m_controlpicker.GetCurSel();
  for(i=0;i<the_chui.windows[pos].controlcount;i++)
  {
    if(act==i) mode=BM_INVERT|BM_OVERLAY;
    else mode=BM_OVERLAY;
    pos2=the_chui.windows[pos].controlidx+i;
    if(pos2<0 || pos2>the_chui.controlcnt)
    {
      MessageBox("The window is crippled.","Error",MB_ICONSTOP|MB_OK);
      break;
    }
    switch(the_chui.controls[pos2].controltype)
    {
    case CC_BUTTON:
      {
        chui_button *cc=(chui_button *) the_chui.extensions[pos2];
        RetrieveResref(tmpstr,cc->bam);
        //display bam cycle/frame with coordinates
        if(!read_bam(tmpstr, &tmp_bam, true))
        {
          frame=tmp_bam.GetFrameIndex(cc->cycle,cc->unpressed);
          tmp_bam.MakeBitmap(frame,GREY,the_mos,mode,
            the_chui.controls[pos2].xpos,the_chui.controls[pos2].ypos);
        }
      }
      break;
    case CC_PROGRESS:
      {
        chui_progress *cc=(chui_progress *) the_chui.extensions[pos2];
        RetrieveResref(tmpstr,cc->mos1);
        if(!read_mos(tmpstr, &tmp_mos, true))
        {
          tmp_mos.MakeBitmap(TRANSPARENT_GREEN,the_mos,mode,the_chui.controls[pos2].xpos,the_chui.controls[pos2].ypos);
        }
        RetrieveResref(tmpstr,cc->mos2);
        if(!read_mos(tmpstr, &tmp_mos, true))
        {
          tmp_mos.MakeBitmap(TRANSPARENT_GREEN,the_mos,mode,the_chui.controls[pos2].xpos+cc->xpos,the_chui.controls[pos2].ypos+cc->ypos);
        }
      }
      break;
    case CC_SLIDER:
      {
        chui_slider *cc=(chui_slider *) the_chui.extensions[pos2];
        
        RetrieveResref(tmpstr,cc->mos);
        if(!read_mos(tmpstr, &tmp_mos, true))
        {
          tmp_mos.MakeBitmap(TRANSPARENT_GREEN,the_mos,mode,the_chui.controls[pos2].xpos,the_chui.controls[pos2].ypos);
        }
        RetrieveResref(tmpstr,cc->bam);
        if(!read_bam(tmpstr, &tmp_bam, true))
        {
          frame=tmp_bam.GetFrameIndex(cc->cycle,cc->released);
          tmp_bam.MakeBitmap(frame,0,the_mos,mode,
            the_chui.controls[pos2].xpos+cc->xpos,the_chui.controls[pos2].ypos+cc->ypos);
        }
      }
      break;
    case CC_EDITBOX:
      {
        chui_editbox *cc=(chui_editbox *) the_chui.extensions[pos2];
        RetrieveResref(tmpstr,cc->mos1);
        if(!read_mos(tmpstr, &tmp_mos, true))
        {
          tmp_mos.MakeBitmap(TRANSPARENT_GREEN,the_mos,mode,the_chui.controls[pos2].xpos,the_chui.controls[pos2].ypos);
        }
        RetrieveResref(tmpstr,cc->font);
        if(!read_bam(tmpstr,&tmp_bam,true))
        {
          tmp_bam.DrawText("EDIT",0,the_mos,mode,
            the_chui.controls[pos2].xpos,the_chui.controls[pos2].ypos,
            the_chui.controls[pos2].width);
        }
      }
      break;
    case CC_TEXT:
      {
        chui_textarea *cc=(chui_textarea *) the_chui.extensions[pos2];
        RetrieveResref(tmpstr,cc->bam1); //font
        if(!read_bam(tmpstr,&tmp_bam, true))
        {
          tmp_bam.DrawText("TEXTAREA",RGB(cc->rgb2r,cc->rgb2g,cc->rgb2b),the_mos,mode,
            the_chui.controls[pos2].xpos,the_chui.controls[pos2].ypos,
            the_chui.controls[pos2].width);
        }
      }
      break;
    case CC_LABEL:
      {
        chui_label *cc=(chui_label *) the_chui.extensions[pos2];
        RetrieveResref(tmpstr,cc->font); //font
        if(!read_bam(tmpstr,&tmp_bam, true))
        {
          tmpstr=resolve_tlk_text(cc->strref);
          if(tmpstr.IsEmpty()) tmpstr="LABEL";
          if(cc->justify&0x10) mode|=BM_RIGHT;
          else if(cc->justify&8) mode|=BM_LEFT;
          else mode|=BM_CENTER;
          
          if(cc->justify&1) rgb=RGB(cc->rgb1r,cc->rgb1g,cc->rgb1b);
          else rgb=0;
          tmp_bam.DrawText(tmpstr,rgb,the_mos,mode,
            the_chui.controls[pos2].xpos,the_chui.controls[pos2].ypos,
            the_chui.controls[pos2].width);
        }
      }
      break;
    case CC_SCROLLBAR:
      {
        chui_scrollbar *cc=(chui_scrollbar *) the_chui.extensions[pos2];
        RetrieveResref(tmpstr,cc->bam);
        if(!read_bam(tmpstr,&tmp_bam, true))
        {
          frame=tmp_bam.GetFrameIndex(cc->cycle,cc->trough);
          tmp_bam.MakeBitmap(frame,0,the_mos,mode,
            the_chui.controls[pos2].xpos,the_chui.controls[pos2].ypos);
          
          frame=tmp_bam.GetFrameIndex(cc->cycle,cc->trough);
          end=the_chui.controls[pos2].height-tmp_bam.GetFrameSize(frame).y;
          for(length=0;length<end;length+=tmp_bam.GetFrameSize(frame).y)
          {
            tmp_bam.MakeBitmap(frame,0,the_mos,mode,
              the_chui.controls[pos2].xpos,the_chui.controls[pos2].ypos+length);
          }
          
          frame=tmp_bam.GetFrameIndex(cc->cycle,cc->upunpressed);
          tmp_bam.MakeBitmap(frame,0,the_mos,mode,
            the_chui.controls[pos2].xpos,the_chui.controls[pos2].ypos);
          
          frame=tmp_bam.GetFrameIndex(cc->cycle,cc->downunpressed);
          tmp_bam.MakeBitmap(frame,0,the_mos,mode,
            the_chui.controls[pos2].xpos,the_chui.controls[pos2].ypos+the_chui.controls[pos2].height-tmp_bam.GetFrameSize(frame).y);
          
          frame=tmp_bam.GetFrameIndex(cc->cycle,cc->slider);
          tmp_bam.MakeBitmap(frame,0,the_mos,mode,
            the_chui.controls[pos2].xpos,the_chui.controls[pos2].ypos+the_chui.controls[pos2].height/2);
        }
      }
      break;
    }
  }
  //we put the prepared bitmap on the screen
  MakeBitmapExternal(the_mos.GetDIB(),the_mos.m_pixelwidth,the_mos.m_pixelheight,preview.m_bm);  
  preview.DoModal();

  //ImageView returns the point clicked on the screen, so we can select
  //the control pointed by the mouse
  point=preview.GetPoint(GP_POINT);
  for(i=0;i<the_chui.windows[pos].controlcount;i++)
  {
    pos2=the_chui.windows[pos].controlidx+i;
    if(point.x>=the_chui.controls[pos2].xpos && 
       point.y>=the_chui.controls[pos2].ypos)
    {
      if(point.x<=the_chui.controls[pos2].xpos+the_chui.controls[pos2].width &&
         point.y<=the_chui.controls[pos2].ypos+the_chui.controls[pos2].height)
      {
        //found control
        m_controlpicker.SetCurSel(i);
      }
    }
  }
  RefreshDialog();
  UpdateData(UD_DISPLAY);
}

void CChuiEdit::OnFileTbg() 
{
 ExportTBG(this, REF_CHU, 0);
}

void CChuiEdit::OnFileTp2() 
{
 ExportTBG(this, REF_CHU, 1);
}

BOOL CChuiEdit::PreTranslateMessage(MSG* pMsg) 
{
  m_tooltip.RelayEvent(pMsg);	
  return CDialog::PreTranslateMessage(pMsg);
}
