#if !defined(AFX_IWD2CREATURE_H__AFF81B30_4566_4ED2_9FA5_1B1EA340570D__INCLUDED_)
#define AFX_IWD2CREATURE_H__AFF81B30_4566_4ED2_9FA5_1B1EA340570D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IWD2Creature.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// IWD2Creature dialog

class IWD2Creature : public CDialog
{
// Construction
public:
	IWD2Creature(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(IWD2Creature)
	enum { IDD = IDD_CREATUREIWD2 };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(IWD2Creature)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(IWD2Creature)
	afx_msg void DefaultKillfocus();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IWD2CREATURE_H__AFF81B30_4566_4ED2_9FA5_1B1EA340570D__INCLUDED_)
