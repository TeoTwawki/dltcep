// 2DAEdit.cpp : implementation file
//

#include "stdafx.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <mmsystem.h>
#include "chitem.h"
#include "2da.h"
#include "2DAEdit.h"
#include "progressbar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static void ResizeLastColumn(CListCtrl &lc, BOOL bSubtractScroll)
{
	CHeaderCtrl *pHeader = lc.GetHeaderCtrl();

	int nCount = pHeader->GetItemCount();
	if (!nCount)
		return;

	int nTotal = 0;
	CRect rHeader;
	pHeader->GetClientRect(&rHeader);
	HDITEM hdi;
	hdi.mask = HDI_WIDTH;

	for(int i=0;i<nCount-1;i++)
	{
		pHeader->GetItem(i,&hdi);
		if (hdi.mask & HDI_WIDTH)
			nTotal += hdi.cxy;
	}

	hdi.cxy = rHeader.Width() - nTotal;
	if (bSubtractScroll)
		hdi.cxy -= ::GetSystemMetrics(SM_CXHSCROLL);

	pHeader->SetItem(nCount-1,&hdi);
}

/////////////////////////////////////////////////////////////////////////////
// C2DAEdit dialog

C2DAEdit::C2DAEdit(CWnd* pParent /*=NULL*/)	: CDialog(C2DAEdit::IDD, pParent)
{
	//{{AFX_DATA_INIT(C2DAEdit)
	m_integer = FALSE;
	//}}AFX_DATA_INIT
  m_inedit=0;
  m_item=0;
  m_subitem=0;
}

void C2DAEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(C2DAEdit)
	DDX_Control(pDX, IDC_2DA, m_2da_control);
	DDX_Check(pDX, IDC_INT, m_integer);
	//}}AFX_DATA_MAP
  DDX_Text(pDX, IDC_DEFAULT, the_2da.defvalue);
  if(the_2da.defvalue.IsEmpty())
  {
    if(m_integer) the_2da.defvalue="-1";
    else the_2da.defvalue="*";
  }
}

BEGIN_MESSAGE_MAP(C2DAEdit, CDialog)
	//{{AFX_MSG_MAP(C2DAEdit)
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_BN_CLICKED(IDC_LOADEX, OnLoadex)
	ON_BN_CLICKED(IDC_SAVEAS, OnSaveas)
	ON_BN_CLICKED(IDC_NEW, OnNew)
	ON_BN_CLICKED(IDC_INT, OnInt)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_2DA, OnEndlabeledit2da)
	ON_NOTIFY(NM_CLICK, IDC_2DA, OnClick2da)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_2DA, OnColumnclick2da)
	ON_BN_CLICKED(IDC_ROW, OnRow)
	ON_BN_CLICKED(IDC_COLUMN, OnColumn)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_REMOVE2, OnRemove2)
	ON_NOTIFY(NM_RCLICK, IDC_2DA, OnRclick2da)
	ON_BN_CLICKED(IDC_COPY2, OnCopy2)
	ON_BN_CLICKED(IDC_COPY, OnCopy)
	ON_BN_CLICKED(IDC_PASTE2, OnPaste2)
	ON_BN_CLICKED(IDC_PASTE, OnPaste)
	ON_BN_CLICKED(IDC_COLUMN2, OnColumn2)
	ON_BN_CLICKED(IDC_ROW2, OnRow2)
	ON_COMMAND(ID_FILE_SAVE, OnSave)
	ON_EN_KILLFOCUS(IDC_DEFAULT, DefaultKillfocus)
	ON_COMMAND(ID_FILE_NEW, OnNew)
	ON_COMMAND(ID_FILE_LOAD, OnLoad)
	ON_COMMAND(ID_FILE_LOADEXTERNALSCRIPT, OnLoadex)
	ON_COMMAND(ID_FILE_SAVEAS, OnSaveas)
	ON_COMMAND(ID_TOOLS_CAPITALIZEENTRIES, OnToolsCapitalize)
	//}}AFX_MSG_MAP
  ON_EN_KILLFOCUS(IDC_EDITLINK,OnKillfocusEditlink)
  ON_NOTIFY(NM_CUSTOMDRAW, IDC_2DA, OnCustomdrawMyList)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// C2DAEdit message handlers

void C2DAEdit::New2DA()
{
  CString *tmppoi;

	the_2da.new_2da();
  tmppoi=new CString[1];
  if(!tmppoi) return;
  tmppoi[0]="";
  the_2da.collabels=tmppoi;
	itemname="new 2da";
}

void C2DAEdit::OnLoad() 
{
	int res;
	
  if(IsWindow(m_edit)) return;
  pickerdlg.m_restype=REF_2DA;
  pickerdlg.m_picked=itemname;
	res=pickerdlg.DoModal();
	if(res==IDOK)
	{
		res=read_2da(pickerdlg.m_picked);
    switch(res)
    {
    case 1:
      MessageBox("2da loaded with some errors (minor inconsistency).","Warning",MB_ICONEXCLAMATION|MB_OK);
  		itemname=pickerdlg.m_picked;
      break;
    case 0:
  		itemname=pickerdlg.m_picked;
      break;
    default:
      MessageBox("Cannot read 2da!","Error",MB_ICONSTOP|MB_OK);
      New2DA();
      break;
    }
	}
  RefreshDialog();
}

static char BASED_CODE szFilter[] = "2DA files (*.2da)|*.2da|All files (*.*)|*.*||";

void C2DAEdit::OnLoadex() 
{
  CString filepath;
  int fhandle;
  int res;
  
  res=OFN_FILEMUSTEXIST|OFN_ENABLESIZING|OFN_EXPLORER;
  if(readonly) res|=OFN_READONLY;  
  CFileDialog m_getfiledlg(TRUE, "2da", makeitemname(".2da",0), res, szFilter);

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
    res=the_2da.Read2DAFromFile(fhandle,-1);
    close(fhandle);
    switch(res)
    {
    case 1:
      MessageBox("2da loaded with some errors (minor inconsistency).","Warning",MB_ICONEXCLAMATION|MB_OK);
      itemname=m_getfiledlg.GetFileTitle();
      itemname.MakeUpper();
      break;
    case 0:
      itemname=m_getfiledlg.GetFileTitle();
      itemname.MakeUpper();
      break;
    default:
      MessageBox("Cannot read 2da!","Error",MB_ICONSTOP|MB_OK);
      New2DA();
      break;
    }
  }
  RefreshDialog();
}

void C2DAEdit::OnSave() 
{
  SaveTable(1);
}

void C2DAEdit::OnSaveas() 
{
  SaveTable(0);
}

void C2DAEdit::SaveTable(int save) 
{
  CString filepath;
  CString newname;
  CString tmpstr;
  int fhandle;
  int res;

  if(readonly)
  {
    MessageBox("You opened it read only!","Warning",MB_ICONEXCLAMATION|MB_OK);
    return;
  }
  res=OFN_HIDEREADONLY|OFN_ENABLESIZING|OFN_EXPLORER;

//  CFileDialog m_getfiledlg(FALSE, "2da", bgfolder+"override\\"+itemname, res, szFilter);
  CFileDialog m_getfiledlg(FALSE, "2da", makeitemname(".2da",0), res, szFilter);

  if(save)
  {
    newname=itemname;
    filepath=makeitemname(".2da",0);
    goto gotname;
  }

restart:  
  if( m_getfiledlg.DoModal() == IDOK )
  {
    filepath=m_getfiledlg.GetPathName();
    filepath.MakeLower();
    if(filepath.Right(4)!=".2da")
    {
      filepath+=".2da";
    }
    newname=m_getfiledlg.GetFileName();
    newname.MakeUpper();
    if(newname.Right(4)==".2DA") newname=newname.Left(newname.GetLength()-4);
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
    fhandle=open(filepath, O_BINARY|O_RDWR|O_CREAT|O_TRUNC,S_IREAD|S_IWRITE);
    if(fhandle<1)
    {
      MessageBox("Can't write file!","Error",MB_ICONSTOP|MB_OK);
      goto restart;
    }
    res=the_2da.Write2DAToFile(fhandle);
    close(fhandle);
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
}

void C2DAEdit::OnNew() 
{
  New2DA();
  RefreshDialog();
}

void C2DAEdit::RefreshDialog()
{
  POSITION pos;
  CString *tmppoi;
  int i,j;
  int width;

  SetWindowText("Edit 2DA: "+itemname);
  m_2da_control.DeleteAllItems();
  while(m_2da_control.DeleteColumn(0));
  m_2da_control.SetItemCount(the_2da.cols);
  for(i=0;i<the_2da.cols;i++)
  {
    width=10*the_2da.collabels[i].GetLength();
    if(width<50) width=50;
    m_2da_control.InsertColumn(i,the_2da.collabels[i],LVCFMT_LEFT,width);
  }
  pos=the_2da.data.GetHeadPosition();
  for(i=0;i<the_2da.rows;i++)
  {
    tmppoi=(CString *) the_2da.data.GetNext(pos);
    m_2da_control.InsertItem(i, tmppoi[0]);
    for(j=1;j<the_2da.cols;j++)
    {
      m_2da_control.SetItemText(i, j, tmppoi[j]);
    }
  }
  m_integer=!invalidnumber(the_2da.defvalue);
  UpdateData(UD_DISPLAY);
}

BOOL C2DAEdit::OnInitDialog() 
{
	CDialog::OnInitDialog();
	RefreshDialog();

  //tooltips
  {
    m_tooltip.Create(this,TTS_NOPREFIX);
    m_tooltip.SetMaxTipWidth(200);
    m_tooltip.SetTipBkColor(RGB(240,224,160));
    
    m_tooltip.AddTool(GetDlgItem(IDCANCEL), IDS_CANCEL);
  }
	return TRUE;  
}

void C2DAEdit::DefaultKillfocus() 
{
	UpdateData(UD_RETRIEVE);	
	UpdateData(UD_DISPLAY);	
}

void C2DAEdit::OnInt() 
{
  m_integer=!m_integer;
	UpdateData(UD_DISPLAY);	
}

void C2DAEdit::OnEndlabeledit2da(NMHDR* pNMHDR, LRESULT* pResult) 
{
  CString tmpstr;
  CString *tmppoi;
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

  if(pDispInfo->item.pszText)
  {
    tmpstr=pDispInfo->item.pszText;
	  m_2da_control.SetItemText(pDispInfo->item.iItem, 0, tmpstr);
    tmppoi=(CString *) the_2da.data.GetAt(the_2da.data.FindIndex(pDispInfo->item.iItem));
    tmppoi[0]=tmpstr;
  }
	*pResult = 0;
}

void C2DAEdit::RClick()
{
  CString tmpstr;
  CRect ref, ref2;

  m_2da_control.GetSubItemRect(m_item,m_subitem,0,ref);
  tmpstr=m_2da_control.GetItemText(m_item,m_subitem);
  tmpstr=resolve_tlk_text(strtonum(tmpstr));
  m_2da_control.GetWindowRect(ref2);
  ScreenToClient(ref2);
  ref.left+=ref2.left;
  ref.top+=ref2.top;
  ref.right+=ref2.left;
  ref.bottom+=ref2.top+2;
  if(ref.Width()/8<tmpstr.GetLength())
  {
    ref.right+=(tmpstr.GetLength()-5)*8;
    if(ref.right>ref2.right) ref.right=ref2.right;
  }
  m_edit.Create(DS_CONTROL | WS_CHILD | WS_VISIBLE |WS_BORDER |ES_MULTILINE|ES_AUTOHSCROLL|ES_AUTOVSCROLL,ref,this,IDC_EDITLINK);  
  m_edit.SetWindowText(tmpstr);
  m_inedit=3;
  m_edit.SetFocus();
}

void C2DAEdit::Click(int flg)
{
  CString tmpstr;
  CRect ref, ref2;
  LVCOLUMN lvColumn;
  char label[200];

  if(!m_subitem)
  {
    m_2da_control.EditLabel(m_item);
  }
  else
  {
    m_2da_control.GetSubItemRect(m_item,m_subitem,0,ref);
    if(m_item==-1)
    {
      if(m_subitem==-1) return;
      lvColumn.mask=LVCF_TEXT;
      lvColumn.pszText=label;
      lvColumn.cchTextMax=200;
      if(m_2da_control.GetColumn(m_subitem,&lvColumn))
      {
        tmpstr=lvColumn.pszText;
      }
    }
    else
    {
      tmpstr=m_2da_control.GetItemText(m_item,m_subitem);
    }
    m_2da_control.GetWindowRect(ref2);
    ScreenToClient(ref2);
    ref.left+=ref2.left;
    ref.top+=ref2.top;
    ref.right+=ref2.left;
    ref.bottom+=ref2.top+2;
    m_edit.Create(DS_CONTROL | WS_CHILD | WS_VISIBLE |WS_BORDER|ES_AUTOHSCROLL,ref,this,IDC_EDITLINK);
    m_edit.SetWindowText(tmpstr);
    m_edit.SetSel(0,-1);
    m_inedit=flg;
    m_edit.SetFocus();
  }
}

void C2DAEdit::OnClick2da(NMHDR* pNMHDR, LRESULT* pResult) 
{
  LVHITTESTINFO htinfo;
  NM_LISTVIEW *pNMListView = (NM_LISTVIEW*)pNMHDR;

  if(IsWindow(m_edit)) m_edit.DestroyWindow();
  htinfo.pt=pNMListView->ptAction;
	m_item=m_2da_control.SubItemHitTest(&htinfo);
  m_subitem=htinfo.iSubItem;
  Click(1);
	*pResult = 0;
}

void C2DAEdit::OnRclick2da(NMHDR* pNMHDR, LRESULT* pResult) 
{
  LVHITTESTINFO htinfo;
  NM_LISTVIEW *pNMListView = (NM_LISTVIEW*)pNMHDR;

  if(IsWindow(m_edit)) m_edit.DestroyWindow();
  htinfo.pt=pNMListView->ptAction;
	m_item=m_2da_control.SubItemHitTest(&htinfo);
  m_subitem=htinfo.iSubItem;
  if((m_subitem>=1) && (m_item>=0))
  {
    RClick();
  }
	*pResult = 0;
}

void C2DAEdit::OnColumnclick2da(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

  OnKillfocusEditlink();
  m_item=-1;
  m_subitem=pNMListView->iSubItem;
  Click(2);
	*pResult = 0;
}

// original source of this piece of code:
// http://www.codeproject.com/listctrl/lvcustomdraw.asp
// Neat Stuff to do in List Controls Using Custom Draw
// By Michael Dunn
void C2DAEdit::OnCustomdrawMyList(NMHDR *pNMHDR, LRESULT *pResult)
{
  NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( pNMHDR );
  
  // Take the default processing unless we set this to something else below.
  *pResult = CDRF_DODEFAULT;
  
  // First thing - check the draw stage. If it's the control's prepaint
  // stage, then tell Windows we want messages for every item.
  
  if ( CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage )
  {
    *pResult = CDRF_NOTIFYITEMDRAW;
  }
  else if ( CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage )
  {
    // This is the prepaint stage for an item. Here's where we set the
    // item's text color. Our return value will tell Windows to draw the
    // item itself, but it will use the new color we set here.
    // We'll cycle the colors through red, green, and light blue.
    
    COLORREF crText;
    
    if(pLVCD->nmcd.dwItemSpec &1)
    {
      crText = RGB(240,240,240);
    }
    else
    {
      crText = RGB(255,255,255);
    }
    
    // Store the color back in the NMLVCUSTOMDRAW struct.
    pLVCD->clrTextBk = crText;
    
    // Tell Windows to paint the control itself.
    *pResult = CDRF_DODEFAULT;
  }
}

void C2DAEdit::OnKillfocusEditlink()
{
  POSITION pos;
  CString tmpstr;
  CString *row;
  LVCOLUMN lvColumn;
  int strref;

	switch(m_inedit)
  {
  case 3:
    m_inedit=0;
    if(m_item>=the_2da.rows || m_subitem>=the_2da.cols)
    {
      goto escape;
    }
    if(m_item<0 || m_subitem<0)
    {
      goto escape;
    }
    m_edit.GetWindowText(tmpstr);
    pos=the_2da.data.FindIndex(m_item);
    if(!pos)
    {
      goto escape;
    }
    row=(CString *)the_2da.data.GetAt(pos);
    if(!row)
    {
      goto escape;
    }
    strref=strtonum(row[m_subitem]);
    if(!strref && !m_integer)
    {
      goto escape;
    }
    if(resolve_tlk_text(strref)==tmpstr)
    {
      goto escape;
    }
    strref=store_tlk_text(strref, tmpstr);
    row[m_subitem].Format("%d",strref);
    m_2da_control.SetItemText(m_item,m_subitem,row[m_subitem]);
escape:
    m_edit.ShowWindow(false);
    break;
  case 0:
    m_edit.DestroyWindow();
    break;
  case 2:
    m_inedit=1;
    m_edit.SetFocus();
    break;
  case 1:
    m_inedit=0;
    if(m_item<the_2da.rows && m_subitem<the_2da.cols)
    {
      m_edit.GetWindowText(tmpstr);
      if(m_item==-1)
      {
        if(m_subitem<0)
        {
          return;
        }
        the_2da.collabels[m_subitem]=tmpstr;
        lvColumn.mask=LVCF_TEXT;
        lvColumn.pszText=tmpstr.GetBuffer(0);
        lvColumn.cchTextMax=tmpstr.GetLength();
        m_2da_control.SetColumn(m_subitem, &lvColumn);
      }
      else
      {
        if(m_integer && invalidnumber(tmpstr) )
        {
          MessageBox("Enter a decimal or hexadecimal value!","2DA editor",MB_ICONEXCLAMATION|MB_OK);
        }
        else
        {
          pos=the_2da.data.FindIndex(m_item);
          if(pos)
          {
            row=(CString *)the_2da.data.GetAt(pos);
            if(row)
            {
              row[m_subitem]=tmpstr;
              m_2da_control.SetItemText(m_item,m_subitem,tmpstr);
            }
          }
        }
      }
    }
    m_edit.DestroyWindow();
  }
}

void C2DAEdit::OnRow2() 
{
  CString tmpstr;

  if(m_item<0) m_item=the_2da.rows;
  tmpstr.Format("Row%d",m_item);
  the_2da.AddRow(m_item,tmpstr, the_2da.defvalue);
  RefreshDialog();
}

void C2DAEdit::OnColumn() 
{
  CString tmpstr;
  int column;

  column=m_subitem+1;
  if(column<1) column=1;
  else if(column>the_2da.cols) column=the_2da.cols;
  tmpstr.Format("Column%d",column);
  the_2da.AddColumn(column,tmpstr,  the_2da.defvalue);
  m_subitem=column;
  RefreshDialog();
}

void C2DAEdit::OnRow() 
{
	m_item++;
	OnRow2();
}

void C2DAEdit::OnColumn2() 
{
  m_subitem--;
  OnColumn();
}

void C2DAEdit::OnRemove() 
{
  if(m_item<0)
  {
    MessageBox("Please select a row!","2DA editor",MB_ICONEXCLAMATION|MB_OK);
    return;
  }
	if(the_2da.RemoveRow(m_item))
  {
    RefreshDialog();
  }
  else MessageBox("Cannot remove row!","2DA editor",MB_ICONEXCLAMATION|MB_OK);
}

void C2DAEdit::OnRemove2() 
{
	if(m_subitem<1)
  {
    MessageBox("Please select a column (can't remove label column).","2DA editor",MB_ICONEXCLAMATION|MB_OK);
    return;
  }
	if(the_2da.RemoveColumn(m_subitem))
  {
    RefreshDialog();
    m_subitem--;
  }
  else MessageBox("Cannot remove column!","2DA editor",MB_ICONEXCLAMATION|MB_OK);
}

void C2DAEdit::OnCopy() 
{
	if(m_item<0)
  {
    MessageBox("Please select a row!","2DA editor",MB_ICONEXCLAMATION|MB_OK);
    return;
  }
	the_2da.CopyRow(m_item);
}

void C2DAEdit::OnCopy2() 
{
	if(m_subitem<0)
  {
    MessageBox("Please select a column!","2DA editor",MB_ICONEXCLAMATION|MB_OK);
    return;
  }
	the_2da.CopyColumn(m_subitem);
}

void C2DAEdit::OnPaste() 
{
	if(m_item<0)
  {
    MessageBox("Please select a row!","2DA editor",MB_ICONEXCLAMATION|MB_OK);
    return;
  }
	the_2da.PasteRow(m_item);
  RefreshDialog();
}

void C2DAEdit::OnPaste2() 
{
	if(m_subitem<0)
  {
    MessageBox("Please select a column!","2DA editor",MB_ICONEXCLAMATION|MB_OK);
    return;
  }
	the_2da.PasteColumn(m_subitem);
  RefreshDialog();
}

void C2DAEdit::OnToolsCapitalize() 
{
  POSITION pos;
  int i,j;
  CString *tmppoi;

  pos=the_2da.data.GetHeadPosition();
  for(j=0;j<the_2da.cols;j++)
  {
    the_2da.collabels[j].MakeUpper();
  }
	for(i=0;i<the_2da.rows;i++)
  {
    tmppoi=(CString *) the_2da.data.GetNext(pos);
    for(j=0;j<the_2da.cols;j++)
    {
      tmppoi[j].MakeUpper();
    }
  }
  RefreshDialog();
}

BOOL C2DAEdit::PreTranslateMessage(MSG* pMsg) 
{
	m_tooltip.RelayEvent(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}

/////////////////////////////////////////////////////////////////////////////
// CIDSEdit dialog

CIDSEdit::CIDSEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CIDSEdit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CIDSEdit)
	m_hex = FALSE;
	m_four = FALSE;
	//}}AFX_DATA_INIT
  m_inedit=false;
  m_item=0;
  m_subitem=0;
}

void CIDSEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIDSEdit)
	DDX_Control(pDX, IDC_IDS, m_ids_control);
	DDX_Check(pDX, IDC_INT, m_hex);
	DDX_Check(pDX, IDC_FOUR, m_four);
	//}}AFX_DATA_MAP
}

void CIDSEdit::NewIDS()
{
	the_ids.new_ids();
	itemname="new identifiers";
  m_hex=false;
  m_four=false;
}

void CIDSEdit::SetHex()
{
  POSITION pos;
  CString *tmppoi;
  int i;

  pos=the_ids.data->GetHeadPosition();
  m_hex=false;
  m_four=true;
  for(i=0;i<the_ids.rows;i++)
  {
    tmppoi=(CString *) the_ids.data->GetNext(pos);
    if(atoi(tmppoi[0])!=strtonum(tmppoi[0]) )
    {
      m_hex=true;
      if(tmppoi[0].GetLength()!=6) m_four=false;
    }
    else
    {
      if(tmppoi[0].GetLength()<4) m_four=false;
    }
  }
}

void CIDSEdit::RefreshDialog()
{
  POSITION pos;
  CString *tmppoi;
  int i;
  int val;

  SetWindowText("Edit IDS: "+itemname);
  m_ids_control.DeleteAllItems();
  while(m_ids_control.DeleteColumn(0));
  m_ids_control.SetItemCount(2);
  m_ids_control.InsertColumn(0,"Value",LVCFMT_LEFT,80);
  m_ids_control.InsertColumn(1,"Identifier",LVCFMT_LEFT,0);
  ResizeLastColumn(m_ids_control,true);
  pos=the_ids.data->GetHeadPosition();
  for(i=0;i<the_ids.rows;i++)
  {
    tmppoi=(CString *) the_ids.data->GetNext(pos);
    val=strtonum(tmppoi[0]);
    if(m_four)
    {
      if(m_hex) tmppoi[0].Format("0x%04x",val);
      else tmppoi[0].Format("%04d",val);
    }
    else
    {
      if(m_hex) tmppoi[0].Format("0x%x",val);
      else tmppoi[0].Format("%d",val);
    }

    m_ids_control.InsertItem(i, tmppoi[0]);
    m_ids_control.SetItemText(i, 1, tmppoi[1]);
  }
  UpdateData(UD_DISPLAY);
}

BOOL CIDSEdit::OnInitDialog() 
{
	CDialog::OnInitDialog();	
	RefreshDialog();	
  //tooltips
  {
    m_tooltip.Create(this,TTS_NOPREFIX);
    m_tooltip.SetMaxTipWidth(200);
    m_tooltip.SetTipBkColor(RGB(240,224,160));
    
    m_tooltip.AddTool(GetDlgItem(IDCANCEL), IDS_CANCEL);
  }
  if(m_readonly)
  {
    GetDlgItem(IDC_SAVEAS)->EnableWindow(false);
  }
	return TRUE;
}

BEGIN_MESSAGE_MAP(CIDSEdit, CDialog)
	//{{AFX_MSG_MAP(CIDSEdit)
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_BN_CLICKED(IDC_LOADEX, OnLoadex)
	ON_BN_CLICKED(IDC_NEW, OnNew)
	ON_BN_CLICKED(IDC_SAVEAS, OnSaveas)
	ON_BN_CLICKED(IDC_ROW, OnRow)
	ON_NOTIFY(NM_CLICK, IDC_IDS, OnClickIds)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_IDS, OnEndlabeleditIds)
	ON_BN_CLICKED(IDC_INT, OnInt)
	ON_BN_CLICKED(IDC_ORDER, OnOrder)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_FOUR, OnFour)
	ON_COMMAND(ID_FILE_SAVE, OnSave)
	ON_COMMAND(ID_FILE_NEW, OnNew)
	ON_COMMAND(ID_FILE_LOAD, OnLoad)
	ON_COMMAND(ID_FILE_LOADEXTERNALSCRIPT, OnLoadex)
	ON_COMMAND(ID_INT, OnInt)
	ON_COMMAND(ID_FOUR, OnFour)
	ON_COMMAND(ID_REMOVE, OnRemove)
	ON_COMMAND(ID_ORDER, OnOrder)
	ON_COMMAND(ID_ADD, OnRow)
	//}}AFX_MSG_MAP
  ON_EN_KILLFOCUS(IDC_EDITLINK,OnKillfocusEditlink)
  ON_NOTIFY(NM_CUSTOMDRAW, IDC_IDS, OnCustomdrawMyList)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIDSEdit message handlers

void CIDSEdit::OnLoad() 
{
	int res;
	
  pickerdlg.m_restype=REF_IDS;
  pickerdlg.m_picked=itemname;
	res=pickerdlg.DoModal();
	if(res==IDOK)
	{
		res=read_ids(pickerdlg.m_picked);
    switch(res)
    {
    case 1:
      MessageBox("Identifiers loaded with some errors.","Warning",MB_ICONEXCLAMATION|MB_OK);
  		itemname=pickerdlg.m_picked;
      break;
    case 0:
  		itemname=pickerdlg.m_picked;
      break;
    default:
      MessageBox("Cannot read identifiers!","Error",MB_ICONSTOP|MB_OK);
      NewIDS();
      break;
    }
	}
  SetHex();
  RefreshDialog();
}

static char BASED_CODE szFilter2[] = "IDS files (*.ids)|*.ids|All files (*.*)|*.*||";

void CIDSEdit::OnLoadex() 
{
  CString filepath;
  int fhandle;
  int res;
  
  res=OFN_FILEMUSTEXIST|OFN_ENABLESIZING|OFN_EXPLORER;
  if(readonly) res|=OFN_READONLY;  
  CFileDialog m_getfiledlg(TRUE, "ids", makeitemname(".ids",0), res, szFilter2);

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
    res=the_ids.ReadIDSFromFile(fhandle,-1);
    close(fhandle);
    switch(res)
    {
    case 1:
      MessageBox("Identifiers loaded with some errors.","Warning",MB_ICONEXCLAMATION|MB_OK);
      itemname=m_getfiledlg.GetFileTitle();
      itemname.MakeUpper();
      break;
    case 0:
      itemname=m_getfiledlg.GetFileTitle();
      itemname.MakeUpper();
      break;
    default:
      MessageBox("Cannot read identifiers!","Error",MB_ICONSTOP|MB_OK);
      NewIDS();
      break;
    }
  }
  SetHex();
  RefreshDialog();
}

void CIDSEdit::OnNew() 
{
  NewIDS();
  RefreshDialog();
}

void CIDSEdit::OnSave() 
{
  SaveIDS(1);
}

void CIDSEdit::OnSaveas() 
{
  SaveIDS(0);
}

void CIDSEdit::SaveIDS(int save) 
{
  CString filepath;
  CString newname;
  CString tmpstr;
  int fhandle;
  int res;

  if(readonly || m_readonly) //m_readonly is the local read only flag
  {
    MessageBox("You opened it read only!","Warning",MB_ICONEXCLAMATION|MB_OK);
    return;
  }
  res=OFN_HIDEREADONLY|OFN_ENABLESIZING|OFN_EXPLORER;
  CFileDialog m_getfiledlg(FALSE, "ids", makeitemname(".ids",0), res, szFilter2);
  if(save)
  {
    newname=itemname;
    filepath=makeitemname(".ids",0);
    goto gotname;
  }

restart:  
  if( m_getfiledlg.DoModal() == IDOK )
  {
    filepath=m_getfiledlg.GetPathName();
    filepath.MakeLower();
    if(filepath.Right(4)!=".ids")
    {
      filepath+=".ids";
    }
    newname=m_getfiledlg.GetFileName();
    newname.MakeUpper();
    if(newname.Right(4)==".IDS") newname=newname.Left(newname.GetLength()-4);
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
    fhandle=open(filepath, O_BINARY|O_RDWR|O_CREAT|O_TRUNC,S_IREAD|S_IWRITE);
    if(fhandle<1)
    {
      MessageBox("Can't write file!","Error",MB_ICONSTOP|MB_OK);
      goto restart;
    }
    res=the_ids.WriteIDSToFile(fhandle);
    close(fhandle);
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
}

void CIDSEdit::OnKillfocusEditlink()
{
  POSITION pos;
  CString tmpstr;
  CString *row;

	if(m_inedit)
  {
    m_inedit=false;
    if(m_item<the_ids.rows && m_subitem<2)
    {
      m_edit.GetWindowText(tmpstr);
      if(m_item==-1)
      {
        //label
      }
      else
      {
        pos=the_ids.data->FindIndex(m_item);
        if(pos)
        {
          row=(CString *)the_ids.data->GetAt(pos);
          if(row)
          {
            row[m_subitem]=tmpstr;
            m_ids_control.SetItemText(m_item,m_subitem,tmpstr);
          }
        }
      }
    }
    m_edit.DestroyWindow();
  }
}

void CIDSEdit::OnRemove() 
{
  if(m_item<0)
  {
    MessageBox("Please select an item to remove.","IDS editor",MB_ICONEXCLAMATION|MB_OK);
    return;
  }
	if(the_ids.RemoveRow(m_item))
  {
    m_ids_control.DeleteItem(m_item);
  }
  else
  {
    MessageBox("Cannot remove item.","IDS editor",MB_ICONEXCLAMATION|MB_OK);
  }
  UpdateData(UD_DISPLAY);
}

void CIDSEdit::OnRow() 
{
  CString tmpstr, def;

  if(m_four)
  {
    if(m_hex) def="0x0000";
    else def="0000";
  }
  else
  {
    if(m_hex) def="0x0";
    else def="0";
  }
  if(m_item<0) m_item=the_ids.rows;
  tmpstr.Format("Identifier%d",m_item);
  the_ids.AddRow(m_item,def,tmpstr);
  m_ids_control.InsertItem(m_item, def);
  m_ids_control.SetItemText(m_item, 1, tmpstr);
//  RefreshDialog();
  UpdateData(UD_DISPLAY);
}

void CIDSEdit::OnClickIds(NMHDR* pNMHDR, LRESULT* pResult) 
{
  CString tmpstr;
  CRect ref, ref2;
  LVHITTESTINFO htinfo;
  NM_LISTVIEW *pNMListView = (NM_LISTVIEW*)pNMHDR;

  if(IsWindow(m_edit)) m_edit.DestroyWindow();
  htinfo.pt=pNMListView->ptAction;
	m_item=m_ids_control.SubItemHitTest(&htinfo);
  if(m_item<0) return;
  m_subitem=htinfo.iSubItem;
  if(!m_subitem)
  {
    m_ids_control.EditLabel(m_item);
  }
  else
  {
    m_ids_control.GetSubItemRect(m_item,1,0,ref);
    tmpstr=m_ids_control.GetItemText(m_item,1);
    m_ids_control.GetWindowRect(ref2);
    ScreenToClient(ref2);
    ref.left+=ref2.left;
    ref.top+=ref2.top;
    ref.right+=ref2.left;
    ref.bottom+=ref2.top+2;
    m_edit.Create(DS_CONTROL | WS_CHILD | WS_VISIBLE |WS_BORDER,ref,this,IDC_EDITLINK);
    m_edit.SetWindowText(tmpstr);
    m_edit.SetFocus();
    m_edit.SetSel(0,-1);
    m_inedit=true;
  }
	*pResult = 0;
}

void CIDSEdit::OnEndlabeleditIds(NMHDR* pNMHDR, LRESULT* pResult) 
{
  int val;
  CString tmpstr;
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
  CString *tmppoi;

  if(pDispInfo->item.pszText)
  {
    val=strtonum(pDispInfo->item.pszText);
    if(m_four)
    {
      if(m_hex) tmpstr.Format("0x%04X",val);
      else tmpstr.Format("%04d",val);
    }
    else
    {
      if(m_hex) tmpstr.Format("0x%x",val);
      else tmpstr.Format("%d",val);
    }
	  m_ids_control.SetItemText(pDispInfo->item.iItem, 0, tmpstr);
    tmppoi=(CString *) the_ids.data->GetAt(the_ids.data->FindIndex(pDispInfo->item.iItem));
    tmppoi[0]=tmpstr;
  }
	*pResult = 0;
}

void CIDSEdit::OnInt() 
{
  UpdateData(UD_RETRIEVE);
  m_hex=!m_hex;
	RefreshDialog();
}

void CIDSEdit::OnFour() 
{
  UpdateData(UD_RETRIEVE);
  m_four=!m_four;
	RefreshDialog();
}

void CIDSEdit::OnOrder() 
{
  CString *tmppoi, *tmppoi2;
  POSITION pos, pos2, pos3;
  CPtrList *data2;
  int val;
  int mask;

  if(itemname=="TRIGGER") mask=0x3fff;
  else mask=~0;
  data2=new CPtrList;
  if(!data2) return; //can't order it due to low memory
  pos=the_ids.data->GetHeadPosition();
  while(pos)
  {
    tmppoi=(CString *) the_ids.data->GetNext(pos);
    val=strtonum(tmppoi[0])&mask;
    pos2=data2->GetTailPosition();
    do
    {
      pos3=pos2;
      if(!pos2) break;
      tmppoi2=(CString *) data2->GetPrev(pos2);
    }
    while((strtonum(tmppoi2[0])&mask)>val);
    if(pos3) data2->InsertAfter(pos3,tmppoi);
    else data2->AddHead(tmppoi);
  }
  delete the_ids.data; //we must not delete referenced data members
	the_ids.data=data2;
	RefreshDialog();
}

void CIDSEdit::OnCustomdrawMyList(NMHDR *pNMHDR, LRESULT *pResult)
{
  NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( pNMHDR );
  
  // Take the default processing unless we set this to something else below.
  *pResult = CDRF_DODEFAULT;
  
  // First thing - check the draw stage. If it's the control's prepaint
  // stage, then tell Windows we want messages for every item.
  
  if ( CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage )
  {
    *pResult = CDRF_NOTIFYITEMDRAW;
  }
  else if ( CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage )
  {
    // This is the prepaint stage for an item. Here's where we set the
    // item's text color. Our return value will tell Windows to draw the
    // item itself, but it will use the new color we set here.
    // We'll cycle the colors through red, green, and light blue.
    
    COLORREF crText;
    
    if(pLVCD->nmcd.dwItemSpec &1)
    {
      crText = RGB(240,240,240);
    }
    else
    {
      crText = RGB(255,255,255);
    }
    
    // Store the color back in the NMLVCUSTOMDRAW struct.
    pLVCD->clrTextBk = crText;
    
    // Tell Windows to paint the control itself.
    *pResult = CDRF_DODEFAULT;
  }
}

BOOL CIDSEdit::PreTranslateMessage(MSG* pMsg) 
{
	m_tooltip.RelayEvent(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}

/////////////////////////////////////////////////////////////////////////////
// CMUSEdit dialog

void CMUSEdit::NewMUS()
{
	the_mus.new_mus();
  the_mus.internal="";
	itemname="new musiclist";
}

CMUSEdit::CMUSEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CMUSEdit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMUSEdit)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
  m_inedit=0;
  m_item=0;
  m_subitem=0;
}

void CMUSEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMUSEdit)
	DDX_Control(pDX, IDC_MUS, m_mus_control);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMUSEdit, CDialog)
	//{{AFX_MSG_MAP(CMUSEdit)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_ROW, OnRow)
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_BN_CLICKED(IDC_LOADEX, OnLoadex)
	ON_BN_CLICKED(IDC_SAVEAS, OnSaveas)
	ON_BN_CLICKED(IDC_NEW, OnNew)
	ON_NOTIFY(NM_CLICK, IDC_MUS, OnClickMus)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_MUS, OnEndlabeleditMus)
	ON_NOTIFY(NM_RCLICK, IDC_MUS, OnRclickMus)
	ON_BN_CLICKED(IDC_PLAYWHOLE, OnPlaywhole)
	ON_BN_CLICKED(IDC_PLAY, RClick)
	ON_COMMAND(ID_PLAY_PLAYWHOLE, OnPlaywhole)
	ON_COMMAND(ID_PLAY_PLAYPART, RClick)
	ON_COMMAND(ID_ADD, OnRow)
	ON_COMMAND(ID_REMOVE, OnRemove)
	//}}AFX_MSG_MAP
  ON_EN_KILLFOCUS(IDC_EDITLINK,OnKillfocusEditlink)
  ON_NOTIFY(NM_CUSTOMDRAW, IDC_MUS, OnCustomdrawMyList)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMUSEdit message handlers
void CMUSEdit::OnLoad() 
{
	int res;
	
  pickerdlg.m_restype=REF_MUS;
  pickerdlg.m_picked=itemname;
	res=pickerdlg.DoModal();
	if(res==IDOK)
	{
		res=read_mus(pickerdlg.m_picked);
    switch(res)
    {
    case 1:
      MessageBox("Musiclist loaded with some errors (minor inconsistency).","Warning",MB_ICONEXCLAMATION|MB_OK);
  		itemname=pickerdlg.m_picked;
      break;
    case 0:
  		itemname=pickerdlg.m_picked;
      break;
    default:
      MessageBox("Cannot read list!","Error",MB_ICONSTOP|MB_OK);
      NewMUS();
      break;
    }
	}
  RefreshDialog();
}

static char BASED_CODE szFilter3[] = "Musiclists (*.mus)|*.mus|All files (*.*)|*.*||";

void CMUSEdit::OnLoadex() 
{
  CString filepath;
  int fhandle;
  int res;
  
  res=OFN_FILEMUSTEXIST|OFN_ENABLESIZING|OFN_EXPLORER;
  if(readonly) res|=OFN_READONLY;  
  CFileDialog m_getfiledlg(TRUE, "mus", makeitemname(".mus",0), res, szFilter3);

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
    res=the_mus.ReadMusFromFile(fhandle,-1);
    close(fhandle);
    switch(res)
    {
    case 1:
      MessageBox("Musiclist loaded with some errors (minor inconsistency).","Warning",MB_ICONEXCLAMATION|MB_OK);
      itemname=m_getfiledlg.GetFileTitle();
      itemname.MakeUpper();
      break;
    case 0:
      itemname=m_getfiledlg.GetFileTitle();
      itemname.MakeUpper();
      break;
    default:
      MessageBox("Cannot read list!","Error",MB_ICONSTOP|MB_OK);
      NewMUS();
      break;
    }
  }
  RefreshDialog();
}

void CMUSEdit::OnSaveas() 
{
  CString filepath;
  CString newname;
  CString tmpstr;
  int fhandle;
  int res;

  if(readonly)
  {
    MessageBox("You opened it read only!","Warning",MB_ICONEXCLAMATION|MB_OK);
    return;
  }
  res=OFN_HIDEREADONLY|OFN_ENABLESIZING|OFN_EXPLORER;
  CFileDialog m_getfiledlg(FALSE, "mus", bgfolder+"music\\"+itemname+".mus", res, szFilter3);

restart:  
  if( m_getfiledlg.DoModal() == IDOK )
  {
    filepath=m_getfiledlg.GetPathName();
    filepath.MakeLower();
    if(filepath.Right(4)!=".mus")
    {
      filepath+=".mus";
    }
    newname=m_getfiledlg.GetFileName();
    newname.MakeUpper();
    if(newname.Right(4)==".MUS") newname=newname.Left(newname.GetLength()-4);
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
    fhandle=open(filepath, O_BINARY|O_RDWR|O_CREAT|O_TRUNC,S_IREAD|S_IWRITE);
    if(fhandle<1)
    {
      MessageBox("Can't write file!","Error",MB_ICONSTOP|MB_OK);
      goto restart;
    }
    if(the_mus.internal.IsEmpty()) the_mus.internal=newname;
    the_mus.internal.MakeUpper();

    res=the_mus.WriteMusToFile(fhandle);
    close(fhandle);
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
}

void CMUSEdit::OnNew() 
{
  NewMUS();
  RefreshDialog();
}

void CMUSEdit::OnKillfocusEditlink()
{
  POSITION pos;
  CString tmpstr;
  CString *row;

	switch(m_inedit)
  {
  case 0:
    m_edit.DestroyWindow();
    break;
  case 1:
    m_inedit=0;
    if(m_item<the_mus.rows && m_subitem<MUSCOLUMN)
    {
      m_edit.GetWindowText(tmpstr);
      tmpstr.MakeUpper();
      pos=the_mus.data->FindIndex(m_item);
      if(pos)
      {
        row=(CString *)the_mus.data->GetAt(pos);
        if(row)
        {
          row[m_subitem]=tmpstr;
          m_mus_control.SetItemText(m_item,m_subitem,tmpstr);
        }
      }
    }
    m_edit.DestroyWindow();
  }
}

void CMUSEdit::OnRemove() 
{
  if(m_item<0)
  {
    MessageBox("Please select a row!","Musiclist editor",MB_ICONEXCLAMATION|MB_OK);
    return;
  }
	if(the_mus.RemoveRow(m_item))
  {
    RefreshDialog();
  }
  else MessageBox("Cannot remove row!","Musiclist editor",MB_ICONEXCLAMATION|MB_OK);
}

void CMUSEdit::OnRow() 
{
  if(m_item<0) m_item=the_2da.rows;
  the_mus.AddRow(m_item,"NEW","","");
  RefreshDialog();
}

static CString muscolumns[MUSCOLUMN]={"Part suffix","Loop suffix","Tag suffix"};

void CMUSEdit::RefreshDialog()
{
  POSITION pos;
  CString *tmppoi;
  int i,j;
  int width;

  SetWindowText("Edit musiclist: "+itemname);
  m_mus_control.DeleteAllItems();
  while(m_mus_control.DeleteColumn(0));
  m_mus_control.SetItemCount(MUSCOLUMN);
  for(i=0;i<MUSCOLUMN;i++) //3 
  {
    width=10*muscolumns[i].GetLength();
    if(width<50) width=50;
    m_mus_control.InsertColumn(i,muscolumns[i],LVCFMT_LEFT,width);
  }
  ResizeLastColumn(m_mus_control,true);
  pos=the_mus.data->GetHeadPosition();
  for(i=0;i<the_mus.rows;i++)
  {
    tmppoi=(CString *) the_mus.data->GetNext(pos);
    m_mus_control.InsertItem(i, tmppoi[0]);
    for(j=1;j<MUSCOLUMN;j++)
    {
      m_mus_control.SetItemText(i, j, tmppoi[j]);
    }
  }
  UpdateData(UD_DISPLAY);
}

BOOL CMUSEdit::OnInitDialog() 
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
    tmpstr2.LoadString(IDS_MUS);
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
  }	
	return TRUE;
}

void CMUSEdit::Click()
{
  CString tmpstr;
  CRect ref, ref2;

  if(!m_subitem)
  {
    m_mus_control.EditLabel(m_item);
  }
  else
  {
    m_mus_control.GetSubItemRect(m_item,m_subitem,0,ref);
    if(m_item==-1) return;
    tmpstr=m_mus_control.GetItemText(m_item,m_subitem);
    m_mus_control.GetWindowRect(ref2);
    ScreenToClient(ref2);
    ref.left+=ref2.left;
    ref.top+=ref2.top;
    ref.right+=ref2.left;
    ref.bottom+=ref2.top+2;
    m_edit.Create(DS_CONTROL | WS_CHILD | WS_VISIBLE |WS_BORDER|ES_AUTOHSCROLL,ref,this,IDC_EDITLINK);
    m_edit.SetWindowText(tmpstr);
    m_edit.SetSel(0,-1);
    m_inedit=1;
    m_edit.SetFocus();
  }
}

void CMUSEdit::OnClickMus(NMHDR* pNMHDR, LRESULT* pResult) 
{
  LVHITTESTINFO htinfo;
  NM_LISTVIEW *pNMListView = (NM_LISTVIEW*)pNMHDR;

  if(IsWindow(m_edit)) m_edit.DestroyWindow();
  htinfo.pt=pNMListView->ptAction;
	m_item=m_mus_control.SubItemHitTest(&htinfo);
  m_subitem=htinfo.iSubItem;
  Click();
	*pResult = 0;
}

int CMUSEdit::RClick()
{
  CString filename;

  filename=m_mus_control.GetItemText(m_item,m_subitem);
  //these hacks are the same as in the engine code
  return play_acm(convert_musiclist(filename, true), true, false);
}

void CMUSEdit::OnPlaywhole() 
{
  panicbutton dlg;
  int poi;
  int tmpitem=m_item;
  int tmpsubitem=m_subitem;
  void *memory1=NULL, *memory2=NULL;
  CString filename;
  int ret;

  m_subitem=0;
  filename=m_mus_control.GetItemText(m_item++,m_subitem);
  poi=play_acm(convert_musiclist(filename, true), true, true);
  if(poi<1 && poi>-10) goto endofquest;
  memory1=(void *) poi;
  
	for(;m_item<the_mus.rows;m_item++)
  {
    if(memory1)
    {
      sndPlaySound((const char *) memory1,SND_MEMORY | SND_ASYNC);
    }
    filename=m_mus_control.GetItemText(m_item,m_subitem);
    poi=play_acm(convert_musiclist(filename, true), true, true);
    if(poi<1 && poi>-10) goto endofquest;
    memory2=(void *) poi;
    dlg.waitsound=true;
    ret=dlg.DoModal();
    sndPlaySound(NULL,SND_NODEFAULT);
    if(memory1) delete memory1;
    memory1=memory2;
    if(ret==IDCANCEL) goto endofquest;
  }
  if(memory1)
  {
    sndPlaySound((const char *) memory1,SND_MEMORY | SND_ASYNC);
  }
  dlg.waitsound=true;
  dlg.DoModal();

endofquest:
  if(memory1)
  {
    delete memory1;
  }
  m_item=tmpitem;
  m_subitem=tmpsubitem;
}

void CMUSEdit::OnRclickMus(NMHDR* pNMHDR, LRESULT* pResult) 
{
  LVHITTESTINFO htinfo;
  NM_LISTVIEW *pNMListView = (NM_LISTVIEW*)pNMHDR;

  if(IsWindow(m_edit)) m_edit.DestroyWindow();
  htinfo.pt=pNMListView->ptAction;
	m_item=m_mus_control.SubItemHitTest(&htinfo);
  m_subitem=htinfo.iSubItem;
  if((m_subitem>=0) && (m_item>=0))
  {
    RClick();
  }
	*pResult = 0;
}

void CMUSEdit::OnEndlabeleditMus(NMHDR* pNMHDR, LRESULT* pResult) 
{
  CString tmpstr;
  CString *tmppoi;
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

  if(pDispInfo->item.pszText)
  {
    tmpstr=pDispInfo->item.pszText;
	  m_mus_control.SetItemText(pDispInfo->item.iItem, 0, tmpstr);
    tmppoi=(CString *) the_mus.data->GetAt(the_mus.data->FindIndex(pDispInfo->item.iItem));
    tmppoi[0]=tmpstr;
  }
	*pResult = 0;
}

void CMUSEdit::OnCustomdrawMyList(NMHDR *pNMHDR, LRESULT *pResult)
{
  NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( pNMHDR );
  
  // Take the default processing unless we set this to something else below.
  *pResult = CDRF_DODEFAULT;
  
  // First thing - check the draw stage. If it's the control's prepaint
  // stage, then tell Windows we want messages for every item.
  
  if ( CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage )
  {
    *pResult = CDRF_NOTIFYITEMDRAW;
  }
  else if ( CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage )
  {
    // This is the prepaint stage for an item. Here's where we set the
    // item's text color. Our return value will tell Windows to draw the
    // item itself, but it will use the new color we set here.
    // We'll cycle the colors through red, green, and light blue.
    
    COLORREF crText;
    
    if(pLVCD->nmcd.dwItemSpec &1)
    {
      crText = RGB(240,240,240);
    }
    else
    {
      crText = RGB(255,255,255);
    }
    
    // Store the color back in the NMLVCUSTOMDRAW struct.
    pLVCD->clrTextBk = crText;
    
    // Tell Windows to paint the control itself.
    *pResult = CDRF_DODEFAULT;
  }
}

BOOL CMUSEdit::PreTranslateMessage(MSG* pMsg) 
{
  m_tooltip.RelayEvent(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}

