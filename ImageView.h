#if !defined(AFX_IMAGEVIEW_H__5E92EE7C_8629_40B0_AA23_1FA9751BF88A__INCLUDED_)
#define AFX_IMAGEVIEW_H__5E92EE7C_8629_40B0_AA23_1FA9751BF88A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ImageView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CImageView dialog

#define GREY  RGB(190,190,190)

//// imageview mode flags

#define IW_ENABLEBUTTON 1         //enables the 'back' button
#define IW_SETVERTEX    2         //doesn't remove the window when clicked on it
#define IW_SHOWGRID     4
#define IW_PLACEIMAGE   8
#define IW_MATCH        16        //match the image
#define IW_NOREDRAW     32

class CImageView : public CDialog
{
// Construction
public:
	CImageView(CWnd* pParent = NULL);   // standard constructor
//  void SetBitMap(HBITMAP *hb); obsolete
  void InitView(int flags); //sets the viewer to the current mos dimensions
  void RedrawContent();
  void SetupAnimationPlacement(Cbam *bam, int orgx, int orgy);
  CPoint GetPoint(int frame);

// Dialog Data
	//{{AFX_DATA(CImageView)
	enum { IDD = IDD_IMAGEVIEW };
	CSpinButtonCtrl	m_spiny;
	CSpinButtonCtrl	m_spinx;
	CButton	m_setbutton;
	CScrollBar	m_vertical;
	CScrollBar	m_horizontal;
	CButton	m_button;
	CStatic	m_bitmap;
	//}}AFX_DATA
  CPoint m_oladjust;
  CPoint m_mousepoint;
  CPoint m_confirmed;
  int m_maxclipx, m_maxclipy;
  int m_minclipx, m_minclipy;
  int m_clipx, m_clipy;
  int m_enablebutton;
  int m_maxextentx, m_maxextenty;
  Cbam *m_animbam;
  Cbam *m_map;
  HBITMAP m_bm;  //accessed from chui editor

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImageView)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
  CBitmap m_lines;

  void RefreshDialog();
  void DrawLines();
  void DrawMap(); //light, height, search maps
	// Generated message map functions
	//{{AFX_MSG(CImageView)
	virtual BOOL OnInitDialog();
	afx_msg void OnBitmap();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnDeltaposSpinx(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpiny(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMAGEVIEW_H__5E92EE7C_8629_40B0_AA23_1FA9751BF88A__INCLUDED_)
