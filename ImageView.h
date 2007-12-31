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
#define IW_EDITMAP      64        //edit a map (light/search/height)
#define IW_OVERLAY      128       //edit overlay
#define IW_MARKTILE     256
#define IW_ENABLEFILL   512
#define IW_GETPOLYGON   1024      //draws all polygons, you are allowed to select one
#define IW_SHOWALL      2048      //show all button
#define IW_OKBUTTON     4096

typedef CList<CPoint, CPoint &> CPointList;

class CImageView : public CDialog
{
// Construction
public:
	CImageView(CWnd* pParent = NULL);   // standard constructor
  void InitView(int flags, Cmos *my_mos=NULL); //sets the viewer to the current mos dimensions
  void SetMapType(int maptype, LPBYTE map);
  void RedrawContent();
  void RefreshDialog();
  void SetupAnimationPlacement(Cbam *bam, int orgx, int orgy, int frame);
  void DrawGrid();
  CPoint GetPoint(int frame);

// Dialog Data
	//{{AFX_DATA(CImageView)
	enum { IDD = IDD_IMAGEVIEW };
	CButton	m_fill_control;
	CButton	m_showgrid_control;
	CComboBox	m_value_control;
	CButton	m_showall_control;
	CSpinButtonCtrl	m_spiny;
	CSpinButtonCtrl	m_spinx;
	CButton	m_setbutton;
	CScrollBar	m_vertical;
	CScrollBar	m_horizontal;
	CButton	m_button;
	CStatic	m_bitmap;
	int		m_value;
	BOOL	m_showall;
	BOOL	m_showgrid;
	BOOL	m_fill;
	//}}AFX_DATA
  int m_actvertex, m_vertexcount;
  int m_max;
  int m_maptype;
  CPoint m_oladjust;
  CPoint m_mousepoint;
  CPoint m_sourcepoint;
  CPoint m_confirmed;
  int m_maxclipx, m_maxclipy;
  int m_minclipx, m_minclipy;
  int m_clipx, m_clipy;
  int m_enablebutton;
  int m_maxextentx, m_maxextenty;
  Cbam *m_animbam;
  int m_frame;
  LPBYTE m_map;
  area_vertex *m_polygon;
  COLORREF *m_palette;
  HBITMAP m_bm;  //accessed from chui editor
  Cmos *m_mos;
  COLORREF m_gridcolor1;
  COLORREF m_gridcolor2;
  COLORREF m_gridcolor3;

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

  void PutPixel(CPoint point, unsigned char color);
  int GetPixel(CPoint point);
  void FloodFill(CPoint point, unsigned char color);
  void DrawLine(CPoint source, CPoint destination, unsigned char color);
  void DrawActors();
  void DrawIcons();
  void DrawPolyPolygon(CPtrList *polygons);
  void CenterPolygon(CPtrList *polygons, int idx);
  POSITION GetPolygonAt(CPtrList *polygons, int idx);
  int GetCountPolygon();
  CString GetPolygonText(int idx);
  short *GetPolygonBox(int idx);
  int GetPolygonSize(int idx);
  inline int IsPointInPolygon(area_vertex *wedvertex, int idx, CPoint point);
  int FindPolygon(CPtrList *polygons, CPoint point);
  void DrawLines(POINTS *polygon, unsigned int count, CString title, int fill, int actv);
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
	afx_msg void OnKillfocusValue();
	afx_msg void OnShowall();
	afx_msg void OnSelchangeValue();
	afx_msg void OnShowgrid();
	afx_msg void OnFill();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMAGEVIEW_H__5E92EE7C_8629_40B0_AA23_1FA9751BF88A__INCLUDED_)
