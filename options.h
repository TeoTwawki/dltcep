#if !defined(AFX_OPTIONS_H__64B3DF66_8C1A_4F69_B7EE_DE98E3A99F55__INCLUDED_)
#define AFX_OPTIONS_H__64B3DF66_8C1A_4F69_B7EE_DE98E3A99F55__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// options.h : header file
//

#define WEI_NOHEADER 1
#define WEI_NOFROM   2
#define WEI_NOCOM    4
#define WEI_TEXT     8
#define WEI_LOGGING 0x8000

#define NOREFCHK 1    //string ref
#define NOITEMCH 2    //broken item reference
#define NORESCHK 4    //icon resource
#define NOATTRCH 8    //item attribute, alignment, class, race, kit
#define NOUSECHK 16   //usability
#define NOTYPECH 32   //item type
#define NOMUSCH  64   //music reference
#define NOEXTCHK 128  //extended header
#define NOWPROCH 256  //weapon proficiency
#define IGNORECHITIN 512 //skip chitin.key files, check only override
#define NOSTATCH 1024    //alignment, stats, level
#define NOOTHERCH 2048   //other less important attributes in main header
#define NOANIMCHK 4096   //animation id
#define WARNINGS  8192   //warns about empty references
#define DROPPABLE 16384  //skip undroppable items
#define SKIPEXT   32768  //skip dead extended headers
#define SKIPSOA   65536  //skip original soa items (from items.bif)
#define NODLGCHK  0x20000   //skip dialog checks
#define NODUPLO   0x40000   //duplicate items in chitin.key
#define DOSCRIPTS 0x80000   //don't scan scripts for duplicate store items
#define NOMISS    0x100000  //don't scream about missing files in chitin.key
#define NOSCRIPT  0x200000  //don't check script/dialog references
#define NOFEATCHK 0x400000  //check opcodes/feature blocks
#define NOVARCHK  0x800000  //no variable checks (death variable, global, local)
#define NOSPLCHK  0x1000000 //no spell reference checks
#define NOCRECHK  0x2000000 //no creature reference checks
#define NOSTORCH  0x4000000 //no store reference checks
#define NOCUT     0x8000000 //no cutscene check
#define NOSTRUCT  0x10000000//no structure error listing
#define NODVARCH  0x20000000//no death variable checks
#define NOSLOTCH  0x40000000//no weapon slot checks
#define NODUR     0x80000000//no duration/timing checks

#define RECYCLE     1       //reuse deleted tlk entries
#define TLKCHANGE   2       //change tlk references
#define RESOLVE     4       //resolve resource names (lookup tlk)
#define DACHANGE    8       //change 2da/ids references
#define INDENT      16      //large indent
#define SELECTION   32      //highlight first bad line
#define REMBAF      64      //remove temporary scripts
#define LASTOPENED  128
#define DECOMPRESS  256     //decompress sav files in place
#define DATAFOLDER  512     //decompress cbf files in place
#define NOCHECK     1024
#define FORCESTEREO 2048    //force stereo sound
#define SORTEFF     4096    //sort effects
#define NUMEFF      8192    //number effects
#define EATSPACE    16384   //ignore spaces
#define WEIDUSTRING 32768   //weidu style strings
#define IDUSTRING   65536   //idu style strings
#define PREVIEW     0x20000 //preview selection
#define OCTREE      0x40000 //octree or dl1 color reduction
#define DITHER      0x80000 //dithering (or no), only in dl1
#define W98         0x100000//w98 compatible mode (sound)

/////////////////////////////////////////////////////////////////////////////
// Coptions dialog

class Coptions : public CDialog
{
// Construction
public:
	Coptions(CWnd* pParent = NULL);   // standard constructor

protected:
  CToolTipCtrl m_tooltip;

// Dialog Data
	//{{AFX_DATA(Coptions)
	enum { IDD = IDD_CHECK };
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Coptions)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  void Refresh();
	// Generated message map functions
	//{{AFX_MSG(Coptions)
	virtual BOOL OnInitDialog();
	afx_msg void OnItemref();
	afx_msg void OnAttr();
	afx_msg void OnStrref();
	afx_msg void OnIconchk();
	afx_msg void OnUsability();
	afx_msg void OnItemtype();
	afx_msg void OnMplus();
	afx_msg void OnExthead();
	afx_msg void OnAllset();
	afx_msg void OnAllunset();
	afx_msg void OnNochitin();
	afx_msg void OnWprof();
	afx_msg void OnStatr();
	afx_msg void OnOther();
	afx_msg void OnAnim();
	afx_msg void OnWarn();
	afx_msg void OnNospec();
	afx_msg void OnSkipext();
	afx_msg void OnSoa();
	afx_msg void OnDlgchk();
	afx_msg void OnDuplo();
	afx_msg void OnScript();
	afx_msg void OnMissing();
	afx_msg void OnFeature();
	afx_msg void OnScanvariables();
	afx_msg void OnVariables();
	afx_msg void OnSpellref();
	afx_msg void OnCreref();
	afx_msg void OnStoreref();
	afx_msg void OnSkipdvar();
	afx_msg void OnStruct();
	afx_msg void OnCheckscript();
	afx_msg void OnNocut();
	afx_msg void OnSlot();
	afx_msg void OnDuration();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CEditOpt dialog

class CEditOpt : public CDialog
{
// Construction
public:
	CEditOpt(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEditOpt)
	enum { IDD = IDD_EDIT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditOpt)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  CToolTipCtrl m_tooltip;

  void Refresh();
	// Generated message map functions
	//{{AFX_MSG(CEditOpt)
	virtual BOOL OnInitDialog();
	afx_msg void OnTlkchange();
	afx_msg void On2dachange();
	afx_msg void OnRecycle();
	afx_msg void OnResolve();
	afx_msg void OnAllset();
	afx_msg void OnAllunset();
	afx_msg void OnIndent();
	afx_msg void OnSelection();
	afx_msg void OnRembaf();
	afx_msg void OnCwdbaf();
	afx_msg void OnDecompress();
	afx_msg void OnNocheck();
	afx_msg void OnDatafolder();
	afx_msg void OnStereo();
	afx_msg void OnSorteffect();
	afx_msg void OnEffnumber();
	afx_msg void OnEatspace();
	afx_msg void OnWeidustr();
	afx_msg void OnIdustr();
	afx_msg void OnPreview();
	afx_msg void OnOctree();
	afx_msg void OnDither();
	afx_msg void OnW98();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


///////////////////////////
// WeiDUOpt.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWeiDUOpt dialog

class CWeiDUOpt : public CDialog
{
// Construction
public:
	CWeiDUOpt(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWeiDUOpt)
	enum { IDD = IDD_WEIDU };
	CButton	m_openfile;
	CString	m_command;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWeiDUOpt)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  CToolTipCtrl m_tooltip;

  void Refresh();

	// Generated message map functions
	//{{AFX_MSG(CWeiDUOpt)
	afx_msg void OnKillfocusParam();
	afx_msg void OnFlag1();
	afx_msg void OnFlag2();
	afx_msg void OnFlag3();
	afx_msg void OnFlag4();
	afx_msg void OnOpenfile();
	virtual BOOL OnInitDialog();
	afx_msg void OnLog();
	afx_msg void OnKillfocusDecompiled();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONS_H__64B3DF66_8C1A_4F69_B7EE_DE98E3A99F55__INCLUDED_)
