// ColorPicker.cpp : implementation file
//

#include "stdafx.h"
#include "chitem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorPicker dialog

CColorPicker::CColorPicker(CWnd* pParent /*=NULL*/)
	: CDialog(CColorPicker::IDD, pParent)
{
	//{{AFX_DATA_INIT(CColorPicker)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
  m_uninitialised=true;
}

void CColorPicker::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CColorPicker)
	DDX_Control(pDX, IDC_COLOR, m_colorpicker);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CColorPicker, CDialog)
	//{{AFX_MSG_MAP(CColorPicker)
	ON_NOTIFY(NM_CLICK, IDC_COLOR, OnClickColor)
	ON_NOTIFY(NM_DBLCLK, IDC_COLOR, OnDblclkColor)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorPicker message handlers

BOOL CColorPicker::OnInitDialog() 
{
  COLORREF bits[16*16];
  HBITMAP hb;
  int i,j;

	CDialog::OnInitDialog();	
  if(m_uninitialised)
  {
    m_uninitialised=false;
    colorlist.Create(16,16,ILC_COLORDDB,COLORNUM,0);
    
    for(i=0;i<COLORNUM;i++)
    {
      for(j=0;j<16*16;j++)
      {
        bits[j]=colors[i].rgb[(j&15)*12/16];
      }
      MakeBitmapExternal(bits,16,16,hb);
      colorlist.Add(CBitmap::FromHandle(hb),RGB(0,0,0) );
    }
  }
  m_colorpicker.DeleteAllItems();  
  m_colorpicker.SetImageList(&colorlist,LVSIL_SMALL);
  for(i=0;i<COLORNUM;i++)
  {    
    m_colorpicker.InsertItem(i,colortitle(i),i);
  }
  m_colorpicker.SetItemState(m_picked,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);
  m_colorpicker.EnsureVisible(m_picked,false);
  UpdateData(UD_DISPLAY);
	return TRUE;
}

void CColorPicker::OnClickColor(NMHDR* pNMHDR, LRESULT* pResult) 
{
  NM_LISTVIEW *pNMListView = (NM_LISTVIEW*)pNMHDR;
	m_picked=pNMListView->iItem;
	*pResult = 0;
}

void CColorPicker::OnDblclkColor(NMHDR* pNMHDR, LRESULT* pResult) 
{
  NM_LISTVIEW *pNMListView = (NM_LISTVIEW*)pNMHDR;
	m_picked=pNMListView->iItem;
	*pResult = 0;
  OnOK();
}
