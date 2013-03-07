#if !defined(AFX_BAMEDIT_H__B43488E4_4A27_40C4_BD54_991426D94ACA__INCLUDED_)
#define AFX_BAMEDIT_H__B43488E4_4A27_40C4_BD54_991426D94ACA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BamEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBamEdit dialog

class CBamEdit : public CDialog
{
// Construction
public:
  virtual BOOL PreTranslateMessage(MSG* pMsg);
	CBamEdit(CWnd* pParent = NULL);   // standard constructor
  void NewBam();

// Dialog Data
	//{{AFX_DATA(CBamEdit)
	enum { IDD = IDD_BAMEDIT };
	CComboBox	m_saveorder_control;
	CStatic	m_bamframe4_control;
	CStatic	m_bamframe3_control;
	CStatic	m_bamframe2_control;
	CStatic	m_bamframe1_control;
	CComboBox	m_cycleframe_control;
	CComboBox	m_cyclenum_control;
	CComboBox	m_framenum_control;
	int		m_edit;
	int		m_framenum2;
	CString	m_framecount;
	//}}AFX_DATA
  int m_zoom;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBamEdit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
  CString m_folder;             //the folder for multiple bmp frames
  CPoint topleft;
  int background;
  COLORREF bgcolor;
  CToolTipCtrl m_tooltip;
  CString filepath;
  int m_play;
  int playcycle, playmax, playindex;
  HBITMAP hbf[4], hbanim;
  int m_function; //palette function, saved for convenience
  COLORREF m_adjust;
  short *m_cycledata;
  int m_cyclesize;

  void RefreshDialog();
  void RefreshMenu();
  void RefreshFramePicker();
  void Loadpalette(bool force_or_import);
  void Forcepalette(Cbam &palette);
  void Savebam(Cbam &my_bam, int save);
  int PickBmpFile(Cbam &tmpbam);
  void LoadBam(Cbam *resource);
  void LoadBamEx(Cbam *resource);
  void AddonBam(Cbam &addon);
  void AddinBam(Cbam &addin);
  int WriteBackgroundBMP(CString newname, Cbam &my_bam);
  int WriteAllFrames(CString newname, Cbam &my_bam);
  void GetBMPCycle(CString dir, Cbam &bam);
  void Flip5Cycles();
  void Flip8Cycles(int nCount);
  CString LeastFileInDir(CString dir);

	afx_msg void DefaultKillfocus();
	// Generated message map functions
	//{{AFX_MSG(CBamEdit)
	afx_msg void OnCompressed();
	afx_msg void OnLoad();
	afx_msg void OnLoadex();
	afx_msg void OnSaveas();
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusFrame();
	afx_msg void OnSelchangeFrame();
	afx_msg void OnKillfocusCycle();
	afx_msg void OnSelchangeCycle();
	afx_msg void OnSelchangeCycframe();
	afx_msg void OnPlay();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBackground();
	afx_msg void OnSavepalette();
	afx_msg void OnLoadpalette();
	afx_msg void OnEditpalette();
	afx_msg void OnColorscale();
	afx_msg void OnForcepalette();
	afx_msg void OnOrderpalette();
	afx_msg void OnTrimpalette();
	afx_msg void OnPaperdoll();
	afx_msg void OnSelchangeSaveorder();
	afx_msg void OnDropframe();
	afx_msg void OnColorscale2();
	afx_msg void OnReversecycle();
	afx_msg void OnNew();
	afx_msg void OnSave();
	afx_msg void OnCheck();
	afx_msg void OnDropcyc();
	afx_msg void OnAddframe();
	afx_msg void OnDelframe();
	afx_msg void OnNewframe();
	afx_msg void OnNewcycle();
	afx_msg void OnSet();
	afx_msg void OnContinuousplay();
	afx_msg void OnCleanup();
	afx_msg void OnInsertcycle();
	afx_msg void OnCenter();
	afx_msg void OnCenterPos();
	afx_msg void OnImport();
	afx_msg void OnLoadexBmp();
	afx_msg void OnCompressFrames();
	afx_msg void OnDecompress();
	afx_msg void OnAlign();
	afx_msg void OnCopycycle();
	afx_msg void OnPastecycle();
	afx_msg void OnShiftForward();
	afx_msg void OnShiftBackward();
	afx_msg void OnFileLoadbmp();
	afx_msg void OnFileMergebam();
	afx_msg void OnFileMergeexternalbam();
	afx_msg void OnFlipcycle();
	afx_msg void OnFlipframe();
	afx_msg void OnCreatemirror();
	afx_msg void OnDropallbutlast();
	afx_msg void OnReduceorientation();
	afx_msg void OnMergebam();
	afx_msg void OnCycleAlignframes();
	afx_msg void OnCycleDropframe10();
	afx_msg void OnFrameMinimizeframe();
	afx_msg void OnToolsMinimalframe();
	afx_msg void OnZoom();
	afx_msg void OnImportCycles();
	afx_msg void OnCycleDuplicate();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void SplitBAM();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BAMEDIT_H__B43488E4_4A27_40C4_BD54_991426D94ACA__INCLUDED_)
