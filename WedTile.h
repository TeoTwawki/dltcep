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
	CComboBox	m_indexpicker;
	CListBox	m_tilepicker;
	BOOL	m_open;
	BOOL	m_setoverlay;
	//}}AFX_DATA
  BOOL m_graphics;
  int m_tilenum, m_firstindex;
  wed_tilemap *m_ptileheader;
  short *m_ptileidx;
  short *m_pdooridx;
  DWORD m_tilecountx,m_tilecounty, m_maxtile;
  CString m_tisname;
  int m_overlay;
  int m_doornum;

  inline void SetTile(int baseoverlay, int countx, int county)
  {
    m_overlay=baseoverlay;
    m_ptileheader=the_area.overlaytileheaders+the_area.getotc(baseoverlay);
    m_ptileidx=the_area.overlaytileindices+the_area.getfoti(baseoverlay);
    m_tilecountx=countx;
    m_tilecounty=county;
    m_pdooridx=NULL;
    m_maxtile=countx*county;
  }
  inline void SetDoor(int doornum)
  {
    POSITION pos;

    m_doornum=doornum;
    m_overlay=-1;
    m_ptileheader=the_area.overlaytileheaders;
    m_ptileidx=the_area.overlaytileindices;
    //m_pdooridx=the_area.doortileindices+the_area.weddoorheaders[doornum].firstdoortileidx;
    pos=the_area.doortilelist.FindIndex(doornum);
    m_pdooridx=(short *) the_area.doortilelist.GetAt(pos);
    m_maxtile=the_area.weddoorheaders[doornum].countdoortileidx;
  }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWedTile)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  CToolTipCtrl m_tooltip;
  CImageView m_preview;

  void RefreshTile();
  void RefreshFields();
  void RefreshDialog();
  void ChangeOverlay(int flag);
  void LoadTileSetAt(int firsttile, int random);

	// Generated message map functions
	//{{AFX_MSG(CWedTile)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeBlockpicker();
	afx_msg void OnAddalt();
	afx_msg void OnFixalter();
	afx_msg void OnGreenwater();
	afx_msg void OnOverlay();
	afx_msg void OnSave();
	afx_msg void DefaultKillfocus();
	afx_msg void OnKillfocusFirst();
	afx_msg void OnKillfocusTileidx();
	afx_msg void OnRemove();
	afx_msg void OnLoad();
	afx_msg void OnLoad2();
	afx_msg void OnOpen();
	afx_msg void OnPreview();
	afx_msg void OnDrop();
	//}}AFX_MSG
  void CWedTile::OnTile();
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WEDTILE_H__2ACD9489_AFF6_451C_9DDE_92F2D746D89A__INCLUDED_)
