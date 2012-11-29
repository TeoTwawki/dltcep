// ItemPicker.cpp : implementation file
//

#include "stdafx.h"
#include "chitem.h"
#include "options.h"
#include "2da.h"
#include "AnimDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CItemPicker dialog

CItemPicker::CItemPicker(CWnd* pParent /*=NULL*/)
	: CDialog(CItemPicker::IDD, pParent)
{
	//{{AFX_DATA_INIT(CItemPicker)
	m_text = _T("");
	//}}AFX_DATA_INIT
  m_restype=-1;
  m_oldrestype=-1;
}

void CItemPicker::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CItemPicker)
	DDX_Control(pDX, IDC_PLAY, m_play_control);
	DDX_Control(pDX, IDC_PREVIEW, m_preview_control);
	DDX_Control(pDX, IDC_RESOLVE, m_resolve_control);
	DDX_Control(pDX, IDC_ANIMLIST, m_animlist_control);
	DDX_Control(pDX, IDC_PICK, m_pick_control);
	DDX_Text(pDX, IDC_TEXT, m_text);
	//}}AFX_DATA_MAP
  DDX_Text(pDX, IDC_PICK, m_picked);
}

BEGIN_MESSAGE_MAP(CItemPicker, CDialog)
	//{{AFX_MSG_MAP(CItemPicker)
	ON_LBN_DBLCLK(IDC_PICK, OnDblclkPick)
	ON_BN_CLICKED(IDC_SEARCH, OnSearch)
	ON_BN_CLICKED(IDC_RESOLVE, OnResolve)
	ON_BN_CLICKED(IDC_PREVIEW, OnPreview)
	ON_LBN_SELCHANGE(IDC_PICK, OnSelchangePick)
	ON_BN_CLICKED(IDC_PLAY, OnPlay)
	ON_BN_CLICKED(IDC_ANIMLIST, OnAnimlist)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CItemPicker message handlers

void CItemPicker::OnOK()
{
  int sel;
  int x;

  UpdateData(UD_RETRIEVE);
  sel=m_pick_control.GetCurSel();
  if(sel==-1) OnSearch();
  sel=m_pick_control.GetCurSel();
  if(sel>=0 && sel<m_pick_control.GetCount())
  {
    m_pick_control.GetText(sel,m_picked);
    x=m_picked.Find(" ");
    if(x>0) m_picked=m_picked.Left(x);
    UpdateData(UD_DISPLAY);
    CDialog::OnOK();
  }
  else OnCancel();
}
void CItemPicker::OnDblclkPick() 
{
  OnOK();
}

void CItemPicker::Preview(CString &key, loc_entry &fileloc, int restype)
{
  CString tmpstr;
  int fhandle;
  int fc;

  switch(restype)
  {
  case REF_BMP:
  case REF_BAM:
  case REF_ITM:
  case REF_SPL:
    m_preview.InitView(IW_NOREDRAW); //don't add the 'back' button
    if(editflg&PREVIEW)
    {
      m_preview.ShowWindow(false);
      return;
    }
    m_preview.SetWindowText(key);
    switch(restype)
    {
    case REF_ITM: case REF_SPL:
      fhandle=locate_file(fileloc, 0);
      if(restype==REF_ITM) tmpstr=my_item.RetrieveResourceRef(fhandle);
      else tmpstr=my_spell.RetrieveResourceRef(fhandle);
      break;
    case REF_BMP: case REF_BAM:
      tmpstr=key;
      break;
    }
    if(restype==REF_BMP) fc=read_bmp(tmpstr, &my_bam);
    else fc=read_bam_preview(tmpstr,&my_bam);
    if(!fc)
    {
      fc=my_bam.GetFrameIndex(0,0);
      if(restype!=REF_SPL) my_bam.MakeBitmap(fc,RGB(32,32,32),m_preview.m_bm,BM_RESIZE,32,32);
      else my_bam.MakeBitmap(fc,RGB(255,198,128),m_preview.m_bm,BM_RESIZE,32,32);
    }
    else
    {
      if(m_preview.m_bm)
      {
        DeleteObject(m_preview.m_bm);
        m_preview.m_bm=0;
      }
    }
    m_preview.RedrawContent();
    m_preview.ShowWindow(true);
    break;
  case REF_ARE:
  case REF_MOS:
  case REF_CHU:
    if(editflg&PREVIEW)
    {
      m_preview.ShowWindow(false);
      return;
    }
    m_preview.SetWindowText(key);
    switch(restype)
    {
    case REF_CHU:
      fhandle=locate_file(fileloc, 0);
      tmpstr=my_chui.RetrieveMosRef(fhandle);
      break;
    case REF_ARE:
      fhandle=locate_file(fileloc, 0);
      tmpstr=my_area.RetrieveMosRef(fhandle);
      break;
    default:
      tmpstr=key;
    }

    if(read_mos(tmpstr,&my_mos,true))
    {
      my_mos.new_mos();
    }
    else
    {
      if(m_preview.m_bm)
      {
        DeleteObject(m_preview.m_bm);
        m_preview.m_bm=0;
      }
    }
    m_preview.m_maxextentx=m_preview.m_maxextenty=8;
    my_mos.m_tileheaders=NULL;
    m_preview.InitView(0, &my_mos);
    m_preview.RedrawContent();
    m_preview.ShowWindow(true);
    break;
  }
}

void CItemPicker::ResolveKey(CString &key, loc_entry &fileloc, int restype)
{
  long nameref;
  CString tmpstr;
  int fhandle;

  if(editflg&RESOLVE) return;
  switch(restype)
  {
  case REF_ARE:
    if(!the_2da.rows) return;    
    nameref=atoi(the_2da.FindValue(key,1));
    break;
  case REF_ITM:
    fhandle=locate_file(fileloc, 0);
    nameref=my_item.RetrieveNameRef(fhandle);
    break;
  case REF_CRE:
    fhandle=locate_file(fileloc, 0);
    nameref=my_creature.RetrieveNameRef(fhandle);
    break;
  case REF_SPL:
    fhandle=locate_file(fileloc, 0);
    nameref=my_spell.RetrieveNameRef(fhandle);
    break;
  case REF_STO:
    fhandle=locate_file(fileloc, 0);
    nameref=my_store.RetrieveNameRef(fhandle);
    break;
  default:
    return;
  }
  if(nameref<=0) tmpstr=" (?)";
  tmpstr=CString(" (")+resolve_tlk_text(nameref)+")";
  key+=tmpstr;
}

void CItemPicker::FillList(int restype, CListBox &list)
{
  POSITION pos;
  CString key;
  loc_entry fileloc;
  int idx;
  
  list.ResetContent();
  idx=determinetype(restype);
  if(!idx)
  {
    MessageBox("Internal error (no method for filling listbox)","Error",MB_OK);
    return;
  }
  else pos=resources[idx]->GetStartPosition();
  while(pos)
  {
    resources[idx]->GetNextAssoc(pos,key,fileloc);
    ResolveKey(key,fileloc,restype);
    list.AddString(key);
  }
}

void CItemPicker::RefreshDialog()
{
  CString tmp;
  CRect rect;
  int flg;

  tmp.LoadString(idstrings[determinetype(m_restype)]);
  tmp="Pick "+tmp;
  if(readonly) tmp+=" (read only)";
  SetWindowText(tmp);
  FillList(m_restype,m_pick_control);
  flg=!(editflg&RESOLVE);
  m_resolve_control.SetCheck(flg);
  flg=!(editflg&PREVIEW);
  m_preview_control.SetCheck(flg);
}

BOOL CItemPicker::OnInitDialog() 
{
  CString tmp;
  CRect rect;

	CDialog::OnInitDialog();
  switch(m_restype)
  {
  case REF_ARE:
    the_2da.new_2da();
    read_2da("MAPNAME",the_2da);
    //falling through
  case REF_BMP:
  case REF_BAM:
  case REF_ITM:
  case REF_SPL:
  case REF_MOS:
  case REF_CHU:
    m_preview_control.ShowWindow(true);
    m_preview.Create(IDD_IMAGEVIEW,this);
    GetWindowRect(rect);
    m_preview.SetWindowPos(0,rect.right,rect.top,0,0,SWP_NOACTIVATE|SWP_NOZORDER|SWP_HIDEWINDOW|SWP_NOSIZE);
    break;
  case REF_WAV:
  case REF_ACM:
    m_play_control.ShowWindow(true);
    break;
  }
  if (m_restype==REF_BAM)
  {
    m_animlist_control.ShowWindow(true);
    m_resolve_control.ShowWindow(false);
  }
  RefreshDialog();
  m_pick_control.SelectString(0,m_picked);
  OnSelchangePick();
  //tooltips
  {
    m_tooltip.Create(this,TTS_NOPREFIX);
    m_tooltip.SetMaxTipWidth(200);
    m_tooltip.SetTipBkColor(RGB(240,224,160));
  }
  UpdateData(UD_DISPLAY);
  return TRUE;
}

void CItemPicker::OnSearch() 
{
  CString tmpstr;
  int cnt;
  int i;

	UpdateData(UD_RETRIEVE);
	if(m_pick_control.SelectString(m_pick_control.GetCurSel(),m_text)==-1)
  {
    cnt=m_pick_control.GetCount();
    i=m_pick_control.GetCurSel()+1;
    if(m_text.GetLength())
    {
      m_text.MakeLower();
      if(m_text.GetAt(0)=='*') m_text=m_text.Mid(1);
      for(i=m_pick_control.GetCurSel()+1;i<cnt;i++)
      {
        m_pick_control.GetText(i,tmpstr);
        tmpstr.MakeLower();
        if(tmpstr.Find(m_text)!=-1)
        {
          break;
        }
      }
    }
    if(i==cnt)
    {
      m_pick_control.SetCurSel(0);
    }
    else
    {
      m_pick_control.SetCurSel(i);
    }
  }
  OnSelchangePick();
}

void CItemPicker::OnResolve() 
{
	editflg^=RESOLVE;
  RefreshDialog();
}

void CItemPicker::OnPreview() 
{
	editflg^=PREVIEW;
  OnSelchangePick();
}

void CItemPicker::OnPlay() 
{
  int sel;
  CString key;

  sel=m_pick_control.GetCurSel();
  if(sel>=0 && sel<m_pick_control.GetCount())
  {
    m_pick_control.GetText(sel,key);
  }
	play_acm(key,m_restype==REF_ACM,0);
}

void CItemPicker::OnSelchangePick() 
{
  int sel;
  int x;
  int idx;
  CString key;
  loc_entry fileloc;

  sel=m_pick_control.GetCurSel();
  if(sel>=0 && sel<m_pick_control.GetCount())
  {
    m_pick_control.GetText(sel,key);
    x=key.Find(" ");
    if(x>0) key=key.Left(x);
  }
  idx=determinetype(m_restype);
  if (!idx) return;
  resources[idx]->Lookup(key,fileloc);
  Preview(key,fileloc,m_restype);	
}

void CItemPicker::PostNcDestroy() 
{
	my_bam.new_bam();
	my_mos.new_mos();
	CDialog::PostNcDestroy();
}

void CItemPicker::OnAnimlist() 
{
	CAnimDialog dlg;
	
  UpdateData(UD_RETRIEVE);
  dlg.m_restype=m_restype;
  dlg.m_picked=m_picked;
  if (dlg.DoModal()==IDOK)
  {
    m_picked=dlg.m_picked;
  }
  m_pick_control.SelectString(0,m_picked);
  OnSelchangePick();
  UpdateData(UD_DISPLAY);
}

BOOL CItemPicker::PreTranslateMessage(MSG* pMsg) 
{
	m_tooltip.RelayEvent(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}
