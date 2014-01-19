// AnimDialog.cpp : implementation file
//

#include "stdafx.h"
#include "chitem.h"
#include "AnimDialog.h"
#include "ChitemDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAnimDialog dialog


CAnimDialog::CAnimDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CAnimDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAnimDialog)
	m_filter = _T("");
	//}}AFX_DATA_INIT
}


void CAnimDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAnimDialog)
	DDX_Control(pDX, IDC_ICONRES, m_list_control);
	DDX_Text(pDX, IDC_FILTER, m_filter);
	DDV_MaxChars(pDX, m_filter, 8);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAnimDialog, CDialog)
	//{{AFX_MSG_MAP(CAnimDialog)
	ON_NOTIFY(NM_CLICK, IDC_ICONRES, OnClickIconres)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_ICONRES, OnGetdispinfoIconres)
	ON_EN_KILLFOCUS(IDC_FILTER, OnKillfocusFilter)
	//}}AFX_MSG_MAP
  ON_NOTIFY(NM_CUSTOMDRAW, IDC_ICONRES, OnCustomdrawMyList)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAnimDialog message handlers

void CAnimDialog::GetItemImage(LV_DISPINFO *pDispInfo)
{
  HBITMAP hb;

  LVITEM *Item = &pDispInfo->item;
  //m_list_control.GetItem(&Item);
  read_bam_preview(Item->pszText, &the_bam, true);
  the_bam.MakeBitmap(0,RGB(240,240,240),hb,BM_RESIZE,64,64);
  imagelist.Replace(m_cnt,CBitmap::FromHandle(hb),CBitmap::FromHandle(hb));
  Item->iImage = m_cnt;
  m_cnt = (m_cnt+1)&255;
  //m_list_control.SetItem(Item);
}

void CAnimDialog::RemoveItemImage(unsigned int item)
{
  LVITEM Item;
  char key[10];

  Item.mask = (unsigned int) -1;
  Item.iItem=item;
  Item.iSubItem = 0;
  Item.pszText = key;
  Item.cchTextMax=8;
  m_list_control.GetItem(&Item);
  imagelist.Remove(Item.iImage);
  Item.iImage=0;
  m_list_control.SetItem(&Item);
}

void CAnimDialog::OnCustomdrawMyList(NMHDR *pNMHDR, LRESULT *pResult)
{
  COLORREF crText = RGB(240,240,240);    
  NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( pNMHDR );
  
  *pResult = CDRF_DODEFAULT;
    
  if ( CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage )
  {
    pLVCD->clrTextBk=crText;
    *pResult = CDRF_NOTIFYITEMDRAW;
    return;
  }

  if ( CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage )
  {
    pLVCD->clrTextBk = crText;
    *pResult = CDRF_DODEFAULT;
    return;
  }
}

void CAnimDialog::FillList()
{
  POSITION pos;
  CString key;
  loc_entry fileloc;
  int idx;
  int itemcount;

  m_list_control.DeleteAllItems();
  idx=determinetype(m_restype);
  if(!idx)
  {
    MessageBox("Internal error (no method for filling listbox)","Error",MB_ICONSTOP|MB_OK|MB_TASKMODAL);
    return;
  }
  if (m_filter.IsEmpty()) {
    itemcount = resources[idx]->GetCount();
  } else {
    itemcount = 0;
    pos=resources[idx]->GetStartPosition();
    while(pos) {
      resources[idx]->GetNextAssoc(pos, key, fileloc);
      if (key.Find(m_filter)!=-1) itemcount++;
    }
  }
  m_list_control.SetItemCount(itemcount);
}

void CAnimDialog::OnClickIconres(NMHDR* pNMHDR, LRESULT* pResult) 
{
  LVHITTESTINFO htinfo;
  NM_LISTVIEW *pNMListView = (NM_LISTVIEW*)pNMHDR;

	htinfo.pt=pNMListView->ptAction;
	m_item=m_list_control.SubItemHitTest(&htinfo);
  if(m_item<0) return;
  m_picked=m_list_control.GetItemText(m_item,0);
	*pResult = 0;
  OnOK();
}

BOOL CAnimDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
  m_cnt = 0;
  imagelist.Create(64, 64, ILC_COLORDDB, 256, 0);
  imagelist.SetImageCount(256);
  m_list_control.SetImageList(&imagelist, LVSIL_NORMAL);
	FillList();
	return TRUE;
}

void CAnimDialog::OnGetdispinfoIconres(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here
	
  if(pDispInfo->item.mask&LVIF_TEXT) {
    int idx=determinetype(m_restype);
    POSITION pos=resources[idx]->GetStartPosition();
    CString key;
    loc_entry fileloc;
    int i = pDispInfo->item.iItem;
    do {
      resources[idx]->GetNextAssoc(pos,key,fileloc);
      if (m_filter.IsEmpty() || (key.Find(m_filter)!=-1))
      {
        if (!i) break;
        i--;
      }
    }
    while(pos);

    pDispInfo->item.pszText = 
    strncpy(pDispInfo->item.pszText,key,pDispInfo->item.cchTextMax);
  }
  if(pDispInfo->item.mask&LVIF_IMAGE) {
    GetItemImage(pDispInfo);
  }
	*pResult = 0;
}

void CAnimDialog::OnKillfocusFilter() 
{
  UpdateData(UD_RETRIEVE);
	FillList();
}
