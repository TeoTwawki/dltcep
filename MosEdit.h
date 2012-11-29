#if !defined(AFX_MOSEDIT_H__017E07EA_7419_4CC7_84CC_B359922040CA__INCLUDED_)
#define AFX_MOSEDIT_H__017E07EA_7419_4CC7_84CC_B359922040CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MosEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMosEdit dialog

#define TM_MOS     0
#define TM_TIS     1
#define TM_CONFIRM 2

class CMosEdit : public CDialog
{
// Construction
public:
	CMosEdit(CWnd* pParent = NULL);   // standard constructor
  void NewMos(int torm);
  void setrange(int x, int y);

// Dialog Data
	//{{AFX_DATA(CMosEdit)
	enum { IDD = IDD_MOSEDIT };
	CComboBox	m_framenum_control;
	BYTE	m_red;
	BYTE	m_green;
	BYTE	m_blue;
	int		m_limit;
	int		m_xsize;
	int		m_ysize;
	//}}AFX_DATA
  int m_function; //palette function, saved for convenience
  COLORREF m_adjust;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMosEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  CToolTipCtrl m_tooltip;
  CString filepath;
  int tis_or_mos;
  CImageView m_preview;

  int Savemos(Cmos &my_mos, int save);
  void RefreshDialog();
  unsigned int TileDiff(unsigned int upper, unsigned int lower);
  unsigned int RowDiff(unsigned int increment);

	afx_msg void DefaultKillfocus();
	// Generated message map functions
	//{{AFX_MSG(CMosEdit)
	virtual BOOL OnInitDialog();
	afx_msg void OnLoad();
	afx_msg void OnLoadex();
	afx_msg void OnSaveas();
	afx_msg void OnNew();
	afx_msg void OnKillfocusFrame();
	afx_msg void OnSelchangeFrame();
	afx_msg void OnCompressed();
	afx_msg void OnTransparent();
	afx_msg void OnPalette();
	afx_msg void OnView();
	afx_msg void OnDrop();
	afx_msg void OnKillfocusWidth();
	afx_msg void OnKillfocusHeight();
	afx_msg void OnExtract();
	afx_msg void OnSave();
	virtual void OnOK();
	afx_msg void OnMinimap();
	afx_msg void OnLoadBmp();
	afx_msg void OnToolsGuessdimensions();
	afx_msg void OnToolsConvertnight();
	//}}AFX_MSG
  afx_msg void OnTile();
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MOSEDIT_H__017E07EA_7419_4CC7_84CC_B359922040CA__INCLUDED_)
