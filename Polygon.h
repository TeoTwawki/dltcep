#if !defined(AFX_POLYGON_H__A821FBEC_F426_45F4_9AAE_9E0AD85CDA8D__INCLUDED_)
#define AFX_POLYGON_H__A821FBEC_F426_45F4_9AAE_9E0AD85CDA8D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Polygon.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPolygon dialog

class CPolygon : public CDialog
{
// Construction
public:
	CPolygon(CWnd* pParent = NULL);   // standard constructor

  void SetPolygon(int type, int index, short *countvertex, short *bbox);
// Dialog Data
	//{{AFX_DATA(CPolygon)
	enum { IDD = IDD_POLYGON };
	CListBox	m_vertexpicker;
	BOOL	m_open;
	BOOL	m_graphics;
	BOOL	m_insertpoint;
	BOOL	m_movepolygon;
	//}}AFX_DATA
  area_vertex *m_polygon;
  int first;
  int m_polynum;
  int m_vertexnum;
  int m_changed;
  BOOL insertpoint;
  POSITION pos;
  short *countvertex;    //vertex count
  short *bbox;         //bounding box

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPolygon)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  CToolTipCtrl m_tooltip;
  CImageView m_preview;

  void RefreshPolygon(bool repos);
  void RefreshVertex();
  void FixPolygon(CPoint &point);

	// Generated message map functions
	//{{AFX_MSG(CPolygon)
	afx_msg void OnSelchangeVertexpicker();
	afx_msg void OnDblclkVertexpicker();
	afx_msg void OnKillfocusVertexpicker();
	afx_msg void OnPreview();
	afx_msg void OnRecalcbox();
	afx_msg void OnInsert();
	virtual BOOL OnInitDialog();
	afx_msg void OnOrder();
	afx_msg void OnReverse();
	afx_msg void OnShift();
	afx_msg void OnMove();
	afx_msg void OnOpen();
	afx_msg void OnKillfocusPosx();
	afx_msg void OnKillfocusPosy();
	afx_msg void OnSave();
	afx_msg void OnLoad();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POLYGON_H__A821FBEC_F426_45F4_9AAE_9E0AD85CDA8D__INCLUDED_)
