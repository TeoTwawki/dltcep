#if !defined(AFX_PALETTEEDIT_H__0AE18A95_5667_436B_9AE3_D3FE29B22FEA__INCLUDED_)
#define AFX_PALETTEEDIT_H__0AE18A95_5667_436B_9AE3_D3FE29B22FEA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PaletteEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPaletteEdit dialog

class CPaletteEdit : public CDialog
{
// Construction
public:
	CPaletteEdit(CWnd* pParent = NULL);   // standard constructor
  inline void SetPalette(unsigned long *pal, int function, COLORREF adjust)
  {
    palette=pal;
    m_function=function;
    m_xr=GetRValue(adjust);
    m_xg=GetGValue(adjust);
    m_xb=GetBValue(adjust);
  }
  inline COLORREF GetAdjustment()
  {
    return RGB(m_xr, m_xg, m_xb);
  }
// Dialog Data
	//{{AFX_DATA(CPaletteEdit)
	enum { IDD = IDD_PALETTE };
	CStatic	m_palette_control;
	BYTE	m_green;
	BYTE	m_blue;
	BYTE	m_red;
	BYTE	m_from;
	BYTE	m_to;
	int		m_function;
	BOOL	m_keepgray;
	BYTE	m_xr;
	BYTE	m_xg;
	BYTE	m_xb;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPaletteEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CColorDialog dlg;
  RGBQUAD colorbits[16*16*16*16];
  CPoint mousepoint;
  CPoint mypoint;
  HWND palettehwnd;
  unsigned long *palette;
  int m_nStep;
  int *m_pSteps;         //custom script for palette modifications

  void InitPaletteBitmap();
  void ExecuteScript(int &fr, int &fg, int &fb, int &ftmp);
  void ExecuteStep(int &fr, int &fg, int &fb, int &ftmp, int function);
	// Generated message map functions
	//{{AFX_MSG(CPaletteEdit)
	virtual BOOL OnInitDialog();
	afx_msg void OnPalette2();
	afx_msg void OnPalette();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKillfocusBlue();
	afx_msg void OnKillfocusGreen();
	afx_msg void OnKillfocusRed();
	afx_msg void OnKillfocusIdx();
	afx_msg void OnKillfocusFrom();
	afx_msg void OnKillfocusTo();
	afx_msg void OnSmooth();
	afx_msg void OnKillfocusFunction();
	afx_msg void OnRgb();
	afx_msg void OnAll();
	afx_msg void OnKeepgray();
	afx_msg void OnKillfocusBlue2();
	afx_msg void OnKillfocusGreen2();
	afx_msg void OnKillfocusRed2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PALETTEEDIT_H__0AE18A95_5667_436B_9AE3_D3FE29B22FEA__INCLUDED_)
