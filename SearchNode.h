#if !defined(AFX_SEARCHNODE_H__6699463A_4551_406A_838A_4ED41D7DEBAA__INCLUDED_)
#define AFX_SEARCHNODE_H__6699463A_4551_406A_838A_4ED41D7DEBAA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SearchNode.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSearchNode dialog

class CSearchNode : public CDialog
{
// Construction
public:
	CSearchNode(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSearchNode)
	enum { IDD = IDD_SEARCHNODE };
	CString	m_text;
	int		m_nodetype;
	BOOL	m_type;
	int		m_node;
	BOOL	m_ignorecase;
	int		m_part;
	//}}AFX_DATA
  int m_nodepart;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSearchNode)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSearchNode)
	afx_msg void OnKillfocusNode();
	afx_msg void OnTop();
	afx_msg void OnKillfocusText();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SEARCHNODE_H__6699463A_4551_406A_838A_4ED41D7DEBAA__INCLUDED_)
