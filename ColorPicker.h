#if !defined(AFX_COLORPICKER_H__72D3D0B6_8EA0_4FD2_A722_0B8306D05FE4__INCLUDED_)
#define AFX_COLORPICKER_H__72D3D0B6_8EA0_4FD2_A722_0B8306D05FE4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColorPicker.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CColorPicker dialog

class CColorPicker : public CDialog
{
// Construction
public:
	CColorPicker(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CColorPicker)
	enum { IDD = IDD_COLORPICKER };
	CListCtrl	m_colorpicker;
	//}}AFX_DATA
  CImageList colorlist;
  unsigned int m_picked;
  bool m_uninitialised;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorPicker)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CColorPicker)
	virtual BOOL OnInitDialog();
	afx_msg void OnClickColor(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkColor(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORPICKER_H__72D3D0B6_8EA0_4FD2_A722_0B8306D05FE4__INCLUDED_)
 