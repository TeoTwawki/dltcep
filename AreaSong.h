#if !defined(AFX_AREASONG_H__71920DAC_7C96_4678_84F5_35B853AB6F3C__INCLUDED_)
#define AFX_AREASONG_H__71920DAC_7C96_4678_84F5_35B853AB6F3C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AreaSong.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAreaSong dialog

class CAreaSong : public CDialog
{
// Construction
public:
	CAreaSong(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAreaSong)
	enum { IDD = IDD_AREASONGS };
	CComboBox	m_battlecontrol;
	CComboBox	m_u2control;
	CComboBox	m_u1control;
	CComboBox	m_nightcontrol;
	CComboBox	m_daycontrol;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAreaSong)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  void Musiclist(int idx);

	// Generated message map functions
	//{{AFX_MSG(CAreaSong)
	afx_msg void OnKillfocusAmbi1d();
	afx_msg void OnKillfocusAmbi2d();
	afx_msg void OnKillfocusVolumed();
	afx_msg void OnKillfocusAmbi1n();
	afx_msg void OnKillfocusAmbi2n();
	afx_msg void OnKillfocusVolumen();
	afx_msg void OnKillfocusFlags();
	afx_msg void OnKillfocusDay();
	afx_msg void OnKillfocusNight();
	afx_msg void OnKillfocusUnknown1();
	afx_msg void OnKillfocusBattle();
	afx_msg void OnKillfocusUnknown2();
	virtual BOOL OnInitDialog();
	afx_msg void OnClear();
	afx_msg void OnMusday();
	afx_msg void OnMusnight();
	afx_msg void OnMuswin();
	afx_msg void OnMusbattle();
	afx_msg void OnMuslose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AREASONG_H__71920DAC_7C96_4678_84F5_35B853AB6F3C__INCLUDED_)
