// PaletteEdit.cpp : implementation file
//

#include "stdafx.h"
#include "chitem.h"
#include "PaletteEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPaletteEdit dialog


CPaletteEdit::CPaletteEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CPaletteEdit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPaletteEdit)
	m_green = 0;
	m_blue = 0;
	m_red = 0;
	m_from = 1;
	m_to = 255;
	m_function = -1;
	m_keepgray = FALSE;
	m_xr = 0;
	m_xg = 0;
	m_xb = 0;
	//}}AFX_DATA_INIT
  mypoint=0;
  palette=0;
  m_pSteps=NULL;
}

void CPaletteEdit::DoDataExchange(CDataExchange* pDX)
{
  BYTE myidx;

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPaletteEdit)
	DDX_Control(pDX, IDC_PALETTE, m_palette_control);
	DDX_Text(pDX, IDC_GREEN, m_green);
	DDX_Text(pDX, IDC_BLUE, m_blue);
	DDX_Text(pDX, IDC_RED, m_red);
	DDX_Text(pDX, IDC_FROM, m_from);
	DDX_Text(pDX, IDC_TO, m_to);
	DDX_CBIndex(pDX, IDC_FUNCTION, m_function);
	DDX_Check(pDX, IDC_KEEPGRAY, m_keepgray);
	DDX_Text(pDX, IDC_BLUE2, m_xr);
	DDX_Text(pDX, IDC_GREEN2, m_xg);
	DDX_Text(pDX, IDC_RED2, m_xb);
	//}}AFX_DATA_MAP
  if(pDX->m_bSaveAndValidate==UD_DISPLAY)
  {
    myidx=(BYTE) (mypoint.x+mypoint.y*16);
    DDX_Text(pDX, IDC_IDX, myidx);
  }
  else
  {
    DDX_Text(pDX, IDC_IDX, myidx);
    mypoint.x=myidx&15;
    mypoint.y=myidx>>4;
  }
}

BEGIN_MESSAGE_MAP(CPaletteEdit, CDialog)
	//{{AFX_MSG_MAP(CPaletteEdit)
	ON_STN_CLICKED(IDC_PALETTE, OnPalette2)
	ON_STN_DBLCLK(IDC_PALETTE, OnPalette)
	ON_WM_MOUSEMOVE()
	ON_EN_KILLFOCUS(IDC_BLUE, OnKillfocusBlue)
	ON_EN_KILLFOCUS(IDC_GREEN, OnKillfocusGreen)
	ON_EN_KILLFOCUS(IDC_RED, OnKillfocusRed)
	ON_EN_KILLFOCUS(IDC_IDX, OnKillfocusIdx)
	ON_EN_KILLFOCUS(IDC_FROM, OnKillfocusFrom)
	ON_EN_KILLFOCUS(IDC_TO, OnKillfocusTo)
	ON_BN_CLICKED(IDC_SMOOTH, OnSmooth)
	ON_CBN_KILLFOCUS(IDC_FUNCTION, OnKillfocusFunction)
	ON_BN_CLICKED(IDC_RGB, OnRgb)
	ON_BN_CLICKED(IDC_ALL, OnAll)
	ON_BN_CLICKED(IDC_KEEPGRAY, OnKeepgray)
	ON_EN_KILLFOCUS(IDC_BLUE2, OnKillfocusBlue2)
	ON_EN_KILLFOCUS(IDC_GREEN2, OnKillfocusGreen2)
	ON_EN_KILLFOCUS(IDC_RED2, OnKillfocusRed2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPaletteEdit message handlers
void CPaletteEdit::OnPalette2()
{
  COLORREF mycolor;

  mypoint=mousepoint;
  MapWindowPoints(GetDlgItem(IDC_PALETTE),&mypoint,1);
  mypoint.x/=16;
  mypoint.y/=16;
  if(mypoint.x<0 || mypoint.y<0 || mypoint.x>15 || mypoint.y>15) return;
  mycolor=palette[mypoint.x+mypoint.y*16];
  m_red=GetRValue(mycolor);
  m_green=GetGValue(mycolor);
  m_blue=GetBValue(mycolor);
	InitPaletteBitmap();
  UpdateData(UD_DISPLAY);
}

void CPaletteEdit::OnPalette()
{
  COLORREF mycolor;

  mypoint=mousepoint;
  MapWindowPoints(GetDlgItem(IDC_PALETTE),&mypoint,1);
  mypoint.x/=16;
  mypoint.y/=16;
  if(mypoint.x<0 || mypoint.y<0 || mypoint.x>15 || mypoint.y>15) return;
  if(dlg.DoModal()==IDOK)
  {    
    palette[mypoint.x+mypoint.y*16]=dlg.GetColor();
  }
  mycolor=palette[mypoint.x+mypoint.y*16];
  m_red=GetRValue(mycolor);
  m_green=GetGValue(mycolor);
  m_blue=GetBValue(mycolor);
  InitPaletteBitmap();
  UpdateData(UD_DISPLAY);
}

void CPaletteEdit::InitPaletteBitmap() 
{
  BYTE Red,Green,Blue;
  HBITMAP hb;
  BITMAPINFO bmi;
  int i,j;
  int idx, myidx;

	CDC *pDC = AfxGetMainWnd()->GetDC();
	if (!pDC)
		return;

  myidx=mypoint.x+mypoint.y*16;
  for(j=0;j<256;j++)
  {
    Red=((BYTE *) (palette+j))[2];
    Green=((BYTE *) (palette+j))[1];
    Blue=((BYTE *) (palette+j))[0];
    for(i=0;i<16*16;i++)
    {
      idx=((j&0xf0)<<8)|((i&0xf0)<<4)|((j&15)<<4)|(i&15);
      if((myidx==j) && ((i&3)==((i>>4)&3)) )
      {
        colorbits[idx].rgbRed=(BYTE) (Red^0xaa);
        colorbits[idx].rgbGreen=(BYTE) (Green^0xaa);
        colorbits[idx].rgbBlue=(BYTE) (Blue^0xaa);
      }
      else
      {
        colorbits[idx].rgbRed=Red;
        colorbits[idx].rgbGreen=Green;
        colorbits[idx].rgbBlue=Blue;
      }
      colorbits[idx].rgbReserved=0;
    }
  }

	memset(&bmi.bmiHeader,0,sizeof(BITMAPINFOHEADER));
	
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = 16*16;
	bmi.bmiHeader.biHeight = -16*16;// The graphics are stored as bottom up DIBs.
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	hb = ::CreateDIBitmap(pDC->GetSafeHdc(), &bmi.bmiHeader,CBM_INIT,colorbits,
		&bmi,DIB_RGB_COLORS);
	AfxGetMainWnd()->ReleaseDC(pDC);

  m_palette_control.SetBitmap(hb);
}

BOOL CPaletteEdit::OnInitDialog() 
{
  COLORREF mycolor;

	CDialog::OnInitDialog();
  mypoint=0;
  palettehwnd=GetDlgItem(IDC_PALETTE)->m_hWnd;
  mycolor=palette[0];
  m_red=GetRValue(mycolor);
  m_green=GetGValue(mycolor);
  m_blue=GetBValue(mycolor);
	InitPaletteBitmap();
  UpdateData(UD_DISPLAY);
	return TRUE;
}

void CPaletteEdit::OnMouseMove(UINT nFlags, CPoint point) 
{
	mousepoint=point;
	CDialog::OnMouseMove(nFlags, point);
}

BOOL CPaletteEdit::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->hwnd==palettehwnd) 
  {
    if(pMsg->message==WM_MOUSEMOVE)
    {
      mousepoint=pMsg->pt;
      ScreenToClient(&mousepoint);
    }
  }
	return CDialog::PreTranslateMessage(pMsg);
}

void CPaletteEdit::OnKillfocusBlue() 
{
  int myidx;

	UpdateData(UD_RETRIEVE);
  myidx=mypoint.x+mypoint.y*16;
  *((BYTE *) (palette+myidx)+2)=m_blue;
	InitPaletteBitmap();
	UpdateData(UD_DISPLAY);
}

void CPaletteEdit::OnKillfocusGreen() 
{
  int myidx;

	UpdateData(UD_RETRIEVE);
  myidx=mypoint.x+mypoint.y*16;
  *((BYTE *) (palette+myidx)+1)=m_green;
	InitPaletteBitmap();
	UpdateData(UD_DISPLAY);
}

void CPaletteEdit::OnKillfocusRed() 
{
  int myidx;

	UpdateData(UD_RETRIEVE);
  myidx=mypoint.x+mypoint.y*16;
  *((BYTE *) (palette+myidx))=m_red;
	InitPaletteBitmap();
	UpdateData(UD_DISPLAY);
}

void CPaletteEdit::OnKillfocusIdx() 
{
	UpdateData(UD_RETRIEVE);
	InitPaletteBitmap();
	UpdateData(UD_DISPLAY);
}

void CPaletteEdit::OnKillfocusFrom() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CPaletteEdit::OnKillfocusTo() 
{
	UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CPaletteEdit::OnAll()  //all except transparent color
{
  m_from=1;
  m_to=255;
  UpdateData(UD_DISPLAY);
}

void CPaletteEdit::OnSmooth() 
{
  int from, to, div, pos;
  BYTE fr, fg, fb;
  BYTE tr, tg, tb;
  BYTE val;

  if(m_to<m_from) { from=m_to; to=m_from; }
  else { from=m_from; to=m_to; }
  div=to-from;
  if(div<3) return; //no way
  fr=GetRValue(palette[from]);
  fg=GetGValue(palette[from]);
  fb=GetBValue(palette[from]);
  tr=GetRValue(palette[to]);
  tg=GetGValue(palette[to]);
  tb=GetBValue(palette[to]);
  pos=0;
  while(from++<to)
  {
    pos++;
    val=(BYTE) (0.5+fr+(tr-fr)*pos/div);
    *((BYTE *) (palette+from))=val;
    val=(BYTE) (0.5+fg+(tg-fg)*pos/div);
    *((BYTE *) (palette+from)+1)=val;
    val=(BYTE) (0.5+fb+(tb-fb)*pos/div);
    *((BYTE *) (palette+from)+2)=val;
  }
}

void CPaletteEdit::OnKillfocusFunction() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CPaletteEdit::OnKeepgray() 
{
  UpdateData(UD_RETRIEVE);	
  UpdateData(UD_DISPLAY);
}

void CPaletteEdit::ExecuteStep(int &fr, int &fg, int &fb, int &ftmp, int function)
{
  switch(function)
  {
  case 100: ftmp=fr; break;
  case 101: ftmp=fg; break;
  case 102: ftmp=fb; break;
  case 103: fr=ftmp; break;
  case 104: fg=ftmp; break;
  case 105: fb=ftmp; break;
  case 106: ftmp/=2; break;
  case 107: ftmp*=2; break;

  case 0: fr=fg; break;
  case 1: fr=fb; break;
  case 2: fg=fr; break;
  case 3: fg=fb; break;
  case 4: fb=fr; break;
  case 5: fb=fg; break;
  case 6: ftmp=fr; fr=fg; fg=ftmp; break;
  case 7: ftmp=fg; fg=fb; fb=ftmp; break;
  case 8: ftmp=fb; fb=fr; fr=ftmp; break;
  case 9: fr/=2; break;
  case 10: fg/=2; break;
  case 11: fb/=2; break;
  case 12: fr*=2;  break;
  case 13: fg*=2;  break;
  case 14: fb*=2;  break;
  case 15: //blue red
    if(fr>(fb+fg)*1.111)
    {
      ftmp=fb; fb=fr; fr=ftmp;
    }
    break;
  case 16: //red green
    if(fg>(fr+fb)*1.111)
    {
      ftmp=fr; fr=fg; fg=ftmp;
    }
    break;
  case 17: //green blue
    if(fb>(fg+fr)*1.111)
    {
      ftmp=fg; fg=fb; fb=ftmp;
    }
    break;
  case 18:
    fr+=m_xr; fg+=m_xg; fb+=m_xb;
    break;
  case 19:
    fr-=m_xr; fg-=m_xg; fb-=m_xb;
    break;
  case 20: //redshift
    if(fb+fg>fr*1.35)
    {
      ftmp=(int) ((fb>fg?fg:fb)/1.5);
      fb-=ftmp;
      fg-=ftmp;
    }
    break;
  }
  if(fr>255) fr=255;
  else if(fr<0) fr=0;
  if(fg>255) fg=255;
  else if(fg<0) fg=0;
  if(fb>255) fb=255;
  else if(fb<0) fb=0;
}

#define SCRIPT 21

int CPaletteEdit::ExecuteScript(int &fr, int &fg, int &fb, int &ftmp)
{
  int i;

  if(!m_pSteps)
  {
    MessageBox("Sorry... Not implemented.","Warning",MB_OK);
    return -1;
  }
  for(i=0;i<m_nStep;i++)
  {
    ExecuteStep(fr,fg,fb,ftmp,m_pSteps[i]);
  }
  return 0;
}

void CPaletteEdit::OnRgb() 
{
  int from, to;
  int tmp, isgray;
  int fr, fg, fb, ftmp;

  if(m_to<m_from) { from=m_to; to=m_from; }
  else { from=m_from; to=m_to; }

  for(;from<=to;from++)
  {
    fb=GetRValue(palette[from]);
    fg=GetGValue(palette[from]);
    fr=GetBValue(palette[from]);
    if(m_keepgray)
    {
      tmp=(fb-fg);
      tmp*=tmp;
      isgray=tmp;
      tmp=(fb-fr);
      tmp*=tmp;
      isgray+=tmp;
      if(isgray<GREY_TRESHOLD) continue;
    }
    ftmp=0;
    if(m_function==SCRIPT)
    {
      if(ExecuteScript(fr,fg,fb,ftmp))
      {
        break;
      }
    }
    else
    {
      ExecuteStep(fr,fg,fb,ftmp,m_function);
    }
    *(BYTE *) (palette+from)=(BYTE) fb;
    *((BYTE *) (palette+from)+1)=(BYTE) fg;
    *((BYTE *) (palette+from)+2)=(BYTE) fr;
  }
  InitPaletteBitmap();
  UpdateData(UD_DISPLAY);
}

void CPaletteEdit::OnKillfocusBlue2() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CPaletteEdit::OnKillfocusGreen2() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}

void CPaletteEdit::OnKillfocusRed2() 
{
  UpdateData(UD_RETRIEVE);
  UpdateData(UD_DISPLAY);
}
