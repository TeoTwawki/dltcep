#if !defined(AFX_CREATURELEVELS_H__A9D27295_914D_4D8D_B376_6176E32F6382__INCLUDED_)
#define AFX_CREATURELEVELS_H__A9D27295_914D_4D8D_B376_6176E32F6382__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CreatureLevels.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCreatureLevels dialog

class CCreatureLevels : public CDialog
{
// Construction
public:
	CCreatureLevels(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCreatureLevels)
	enum { IDD = IDD_CREATURELEVELS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCreatureLevels)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCreatureLevels)
	virtual BOOL OnInitDialog();
	afx_msg void OnDefaultKillfocus();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CREATURELEVELS_H__A9D27295_914D_4D8D_B376_6176E32F6382__INCLUDED_)
