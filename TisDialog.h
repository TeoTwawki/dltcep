#if !defined(AFX_TISDIALOG_H__EFBEC71A_AE13_4917_87C2_8B1A7AC142A5__INCLUDED_)
#define AFX_TISDIALOG_H__EFBEC71A_AE13_4917_87C2_8B1A7AC142A5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TisDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTisDialog dialog

class CTisDialog : public CDialog
{
// Construction
public:
	CTisDialog(CWnd* pParent = NULL);   // standard constructor
  void setrange(CString m_tisname, int x, int y, int readnow);

// Dialog Data
	//{{AFX_DATA(CTisDialog)
	enum { IDD = IDD_TISDIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
  CString m_tisname;
  int m_posx, m_posy;
  int m_minx, m_miny;
  int m_maxx, m_maxy;
  int m_tis_or_mos;
  int m_overlaynum;
  wed_tilemap *m_tileheaders;
  short *m_tileindices;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTisDialog)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  CToolTipCtrl m_tooltip;

  void SaveTis(int how);
  void RefreshDialog();
	// Generated message map functions
	//{{AFX_MSG(CTisDialog)
	afx_msg void OnKillfocusPosx();
	afx_msg void OnKillfocusPosy();
	afx_msg void OnKillfocusMaxx();
	afx_msg void OnKillfocusMaxy();
	afx_msg void OnKillfocusMinx();
	afx_msg void OnKillfocusMiny();
	afx_msg void OnBmp();
	afx_msg void OnSavetis();
	afx_msg void OnTop();
	afx_msg void OnBottom();
	afx_msg void OnPreview();
	virtual BOOL OnInitDialog();
	afx_msg void OnMinimap();
	afx_msg void OnOpen();
	afx_msg void OnClearall();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TISDIALOG_H__EFBEC71A_AE13_4917_87C2_8B1A7AC142A5__INCLUDED_)
