#if !defined(AFX_ITEMPICKER_H__B178BA11_12F0_4003_ACA4_6495441F7A38__INCLUDED_)
#define AFX_ITEMPICKER_H__B178BA11_12F0_4003_ACA4_6495441F7A38__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ItemPicker.h : header file
//

#include "creature.h"
#include "item.h"
#include "spell.h"
#include "store.h"
#include "ImageView.h"

/////////////////////////////////////////////////////////////////////////////
// CItemPicker dialog

class CItemPicker : public CDialog
{
// Construction
public:
	CItemPicker(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CItemPicker)
	enum { IDD = IDD_ITEMPICKER };
	CListBox	m_pick_control;
	CString	m_text;
	//}}AFX_DATA
  CString m_picked;
  int m_restype;
  int m_oldrestype;
  Ccreature my_creature;
  Citem my_item;
  Cspell my_spell;
  Cstore my_store;
  Carea my_area;
  Cbam my_bam;
  CImageView m_preview;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CItemPicker)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  void FillList(int restype, CListBox &list);
  void Preview(CString &key, loc_entry &fileloc, int restype);
  void ResolveKey(CString &key, loc_entry &fileloc, int restype);
  void RefreshDialog();
	// Generated message map functions
	//{{AFX_MSG(CItemPicker)
	afx_msg void OnOK();
	afx_msg void OnDblclkPick();
	virtual BOOL OnInitDialog();
	afx_msg void OnSearch();
	afx_msg void OnResolve();
	afx_msg void OnPreview();
	afx_msg void OnSelchangePick();
	afx_msg void OnPlay();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ITEMPICKER_H__B178BA11_12F0_4003_ACA4_6495441F7A38__INCLUDED_)
