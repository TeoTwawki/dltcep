#if !defined(AFX_WEDPOLYGON_H__884E66DF_37C6_43DC_8710_74ED1D1236D9__INCLUDED_)
#define AFX_WEDPOLYGON_H__884E66DF_37C6_43DC_8710_74ED1D1236D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WedPolygon.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWedPolygon dialog

class CWedPolygon : public CDialog
{
// Construction
public:
	CWedPolygon(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWedPolygon)
	enum { IDD = IDD_WEDPOLY };
	CListBox	m_vertexpicker;
	//}}AFX_DATA
  wed_polygon *wedpolygon;
  area_vertex *wedvertex;
  int m_vertexnum;

  inline void SetPolygon(wed_polygon *poi1, area_vertex *poi2)
  {
    wedpolygon=poi1;
    wedvertex=poi2;
  }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWedPolygon)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  void RefreshPolygon();

	// Generated message map functions
	//{{AFX_MSG(CWedPolygon)
	afx_msg void OnFlag1();
	afx_msg void OnFlag2();
	afx_msg void OnFlag3();
	afx_msg void OnFlag4();
	afx_msg void OnFlag5();
	afx_msg void OnFlag6();
	afx_msg void OnFlag7();
	afx_msg void OnFlag8();
	afx_msg void OnKillfocusFlags();
	afx_msg void OnKillfocusUnknown();
	afx_msg void OnKillfocusMinx();
	afx_msg void OnKillfocusMiny();
	afx_msg void OnKillfocusMaxx();
	afx_msg void OnKillfocusMaxy();
	afx_msg void OnKillfocusVertexpicker();
	afx_msg void OnSelchangeVertexpicker();
	afx_msg void OnKillfocusPosx();
	afx_msg void OnKillfocusPosy();
	afx_msg void OnRecalcbox();
	afx_msg void OnReverse();
	afx_msg void OnOrder();
	virtual BOOL OnInitDialog();
	afx_msg void OnShift();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WEDPOLYGON_H__884E66DF_37C6_43DC_8710_74ED1D1236D9__INCLUDED_)
