#if !defined(AFX_WEDTILE_H__2ACD9489_AFF6_451C_9DDE_92F2D746D89A__INCLUDED_)
#define AFX_WEDTILE_H__2ACD9489_AFF6_451C_9DDE_92F2D746D89A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WedTile.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWedTile dialog

class CWedTile : public CDialog
{
// Construction
public:
	CWedTile(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWedTile)
	enum { IDD = IDD_WEDTILE };
	CListBox	m_tilepicker;
	//}}AFX_DATA
  int m_tilenum;
  wed_tilemap *m_ptileheader;
  short *m_ptileidx;
  int m_tilecountx,m_tilecounty;

  inline void SetTile(wed_tilemap *poi1, short *poi2, int countx, int county)
  {
    m_ptileheader=poi1;
    m_ptileidx=poi2;
    m_tilecountx=countx;
    m_tilecounty=county;
  }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWedTile)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  void RefreshTile();

	// Generated message map functions
	//{{AFX_MSG(CWedTile)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeBlockpicker();
	afx_msg void DefaultKillfocus();
	afx_msg void OnFlag1();
	afx_msg void OnFlag2();
	afx_msg void OnFlag3();
	afx_msg void OnFlag4();
	afx_msg void OnFlag5();
	afx_msg void OnFlag6();
	afx_msg void OnFlag7();
	afx_msg void OnFlag8();
	afx_msg void OnTile();
	afx_msg void OnAddalt();
	afx_msg void OnRemall();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WEDTILE_H__2ACD9489_AFF6_451C_9DDE_92F2D746D89A__INCLUDED_)
