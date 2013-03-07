// chitemDlg.h : header file
//

#if !defined(AFX_CHITEMDLG_H__42E3FDEA_0FCA_4733_8AD1_6E396250B019__INCLUDED_)
#define AFX_CHITEMDLG_H__42E3FDEA_0FCA_4733_8AD1_6E396250B019__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "progressbar.h"
/*

*/

#define BAD_STRREF    1
#define BAD_RESREF    2
#define BAD_ITEMREF   4
#define BAD_ICONREF   8
#define BAD_ATTR      16
#define BAD_MPLUS     32
#define BAD_ITEMTYPE  64
#define BAD_WEAPROF   128
#define BAD_EXTHEAD   256
#define BAD_STAT      512
#define BAD_ANIM      1024
#define BAD_USE       2048
#define BAD_INDEX     4096
#define BAD_COMPRESS  8192
#define BAD_VERTEX    16384
#define BAD_VAR       32768
/////////////////////////////////////////////////////////////////////////////
// CChitemDlg dialog
enum logtype_vals {LOG_NO, LOG_SCREEN, LOG_FILE};

class CChitemDlg : public CDialog
{
// Construction
public:
  virtual BOOL PreTranslateMessage(MSG* pMsg);
	CChitemDlg(CWnd* pParent = NULL);	// standard constructor
  CString GetTlkFileName(int which);
  int write_file_progress(int which);
  void rescan_dialog(bool flg);
  void set_progress(int actual);
  void start_progress(int max, CString title);
  void change_progress_title(CString title);
  void end_progress();
  void start_panic();
  void end_panic();

  int check_videocell();
  int check_store();
  int check_applyspell();
  int check_spell();
  int check_item();
  int check_effect();
  int check_chui();
  int check_creature();
  int check_projectile(); //checking a projectile (contains check_pro_num too)
  int check_area(int swap_weds);
  int check_ids();
  int check_game();
  int check_map();
  int check_bam();
  int check_spawnini();
  int check_dialog(int check_or_scan);
  int check_script(int check_or_scan);

  void Importtbg(int alt);
  void scan_override();
  void log(CString format, ...);

// Dialog Data
	//{{AFX_DATA(CChitemDlg)
	enum { IDD = IDD_CHITEM_DIALOG };
	CEdit	m_event_control;
	CString	m_etitle;
	CString	m_event;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChitemDlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  HICON m_hIcon;
  CToolTipCtrl m_tooltip;
  progressbar *m_progressdlg;
  panicbutton *m_panicbutton;
  CString varname2;  //this is used in subsequent store_variable function calls

  unsigned long searchflags;
  search_data searchdata;
  CMapStringToString storeitems; //valid containers (empty string if store not found yet)
  int minsell, maxbuy;
  CString minsellkey;
  CString maxbuykey;

  bool newitem;

  void changeitemname(CString arg)
  {
    itemname=arg;
    newitem=TRUE;
  }
  int gather_override(CString folder, int where=false);
  void read_cd_locations();
  int scan_2da();
  int scan_chitin();
  void resync();
  int scan_dialog_both(bool refresh=false);
  int scan_dialog(bool refresh=false, int which=0);
  int generate_acm_list();
  int AddAcm(CString key, loc_entry &fileloc, int dont);
  int CollectAcms();
  int rescan_only_storeitems();
  bool SkipOriginal(CString bifname);
  int process_items(bool check_or_search);
  int process_stores(bool check_or_search);
  int process_spells(int check_or_search, CStringList *list); //three states
  int process_pros(bool check_or_search);
  int process_effects(int check_or_search, CStringList *list); //three states
  int process_creatures(int check_or_search);
  int process_chuis(int check_or_search);
  int process_videocells(bool check_or_search);
  int process_scripts(int check_or_search); //three states
  int process_dialogs(int check_or_search);
  int process_areas(bool check_or_search);
  int process_bams(bool check_or_search); // only check/search
  int process_tables(); //only search
  int process_games(); // only check
  int process_maps(); // only check
  int process_spawninis(); // only check
  int check_scriptitems();
  bool match_bam();
  bool match_projectile();
  bool match_store();
  bool match_item();
  bool match_spell();
  bool match_effect();
  bool match_creature();
  bool match_chui();
  bool match_videocell();
  bool match_script();
  bool match_2da();
  bool match_area();
  bool match_dialog();
  int read_next_table(loc_entry fileloc);
  int read_next_videocell(loc_entry fileloc);
  int read_next_item(loc_entry fileloc);
  int read_next_spell(loc_entry fileloc);
  int read_next_projectile(loc_entry fileloc);
  int read_next_store(loc_entry fileloc);
  int read_next_script(loc_entry fileloc);
  int read_next_effect(loc_entry fileloc);
  int read_next_creature(loc_entry fileloc);
  int read_next_chui(loc_entry fileloc);
  int read_next_area(loc_entry fileloc);
  int read_next_dialog(loc_entry fileloc);
  int read_next_bam(loc_entry fileloc, bool onlyheader);
  int read_next_game(loc_entry fileloc);
  int read_next_map(loc_entry fileloc);
  int read_next_spawnini(loc_entry fileloc);

  bool check_value(CString section, CString key, bool error);
  void check_keys(CString section, int type);
  void check_creature_section(CString section, CString referenced);
  int check_section(CString section);
  void check_spawn_section(CString section, CString referenced);
  int check_point_string(CString point);
  int check_ambient_overlap(int exclude, CString name, int x, int y, int radius);
  int check_trigger_overlap(CString name);
  int check_area_actors();
  int check_area_trigger();
  int check_area_spawn();
  int check_area_entrance();
  int check_area_container();
  int check_area_item();
  int check_area_ambient();
  int check_area_anim();
  int check_area_vertex();
  int check_area_variable();
  int check_area_door();
  int check_area_mapnote();
  int check_area_song();
  int check_area_interrupt();
  int check_wed(int swap_weds);
  int check_weaponslots();
  int check_creature_pst();
  int check_creature_spells();
  int check_creature_features();
  int check_feature(long feature, int par1, int par2, const char *resource);
  int check_spell_usability(CString spell, unsigned long school);
  int check_item_effectblock(unsigned long itemtype, unsigned long school);
  int check_spell_effectblock();
  int check_counters(unsigned long flags);
  int check_storespelltype();
  int check_storedrinktype();
  int check_storeitemtype();
  int check_storeentries();
  int check_storetype(unsigned int type, int iscont);
  int check_variable(const gam_variable *var, CString scope);
  int check_proj_explode();
  int check_proj_header();
  int check_weaprofs(int itemtype);
  int check_itemtype(int itemtype);
  int check_usability(unsigned long usability);
  int check_attribute(unsigned long attr, int itemtype);
  int check_mplus(unsigned long mplus);
  int check_anim(const char *attr, int itemtype);
  int check_statr();
  int check_other();
  int check_extheader(int itemtype);
  int check_spl_extheader();
  int check_atype(int atype);
  int check_ttype(int atype, int ttype, int tnum);
  int check_location(int atype, int location);
  int check_spelllocation(int location);
  int check_range(int atype, int range);
  int check_ammo(int atype, int dtype, int ammo);
  int check_charges(int atype, int i);
  int check_pro_num(int pronum); //checking a projectile number (in itm)
  int check_ascii_strref(CString value, CString row);
  int CompareLabels(CStringList &first, CStringList &second, CString firstname, CString secondname);
  int check_clab(CString table, int features);
  int check_kits_iwd2();
  int check_kits_bg2(bool tob);
  int check_kits();
  int check_tooltip();
  int check_statdesc();
  int check_spelldesc();
  int check_moviedesc();
  int check_scriptdesc();
  int check_itemexcl();
  int check_itemanim();
  int check_itemdial();
  int check_summons();
  int check_familiar();
  int check_select_spell();
  int check_anisounds();
  int check_treasure();
  int check_spawngroups();
  int check_songlist();
  int check_avatar();
  int check_anisound(CString key);
  int check_ani_code_mirror(CString prefix);
  int check_ani_code_mirror_2(CString prefix);
  int check_ani_one_file(CString prefix);
  int check_ani_two_files(CString prefix);
  int check_ani_two_files_2(CString prefix);
  int check_ani_two_files_3(CString prefix);
  int check_ani_four_files(CString prefix);
  int check_ani_four_files_2(CString prefix);
  int check_ani_six_files(CString prefix);
  int check_ani_six_files_2(CString prefix);
  int check_ani_twentytwo(CString prefix);
  int check_ani_bird(CString prefix);
  int check_ani_four_frames(CString prefix);
  int check_ani_nine_frames(CString prefix);
  int check_ani_pst_animation_1(CString prefix);
  int check_ani_pst_animation_2(CString prefix);
  int check_ani_pst_animation_3(CString prefix);
  int check_ani_pst_ghost(CString prefix);
  int check_ani_pst_stand(CString prefix);
  int check_all_types(CString prefix, int except);

  int check_or_scan_trigger(const trigger *tpoi, int checkflags, int check_or_scan, int bcnt);
  int check_or_scan_action(const action *apoi, int checkflags, int check_or_scan, int bcnt);
  int check_integers(const int *bytes, int storeflags, int opcode, int trigger, int block);
  int store_variable(CString varname, int storeflags, int opcode, int trigger, int block);
  void RefreshMenu();
  int write_file(CString str, int which);
  void AddAllFiles(int fhandle, CString folder);
  void Compressbif(bool cbf_or_bifc);
  void CompressWav(bool acm_or_wavc);
  void DecompressAcm(bool wavc_or_acm);
  void DecompressAcm2(bool wavc_or_acm);
  void CheckPrefix(CString key, int value);
  void which_transition(int idx);
  int OnAvatars(bool anisound);
  CString GetTitle(CString str);
	// Generated message map functions
	//{{AFX_MSG(CChitemDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnRescan();
	afx_msg void OnRescan2();
	afx_msg void OnCompat();
	afx_msg void OnTooltips();
	afx_msg void OnProgress();
	afx_msg void OnSavesettings();
	afx_msg void OnLoggingNone();
	afx_msg void OnLoggingScreen();
	afx_msg void OnLoggingFile();
	afx_msg void OnFinditem();
	afx_msg void OnFindspell();
	afx_msg void OnFindeff();
	afx_msg void OnFindcre();
	afx_msg void OnFindstore();
	afx_msg void OnFindProjectile();
	afx_msg void OnFindVvc();
	afx_msg void OnFindScript();
	afx_msg void OnFindArea();
	afx_msg void OnCheckItem();
	afx_msg void OnCheckSpell();
	afx_msg void OnCheckStore();
	afx_msg void OnCheckCreature();
	afx_msg void OnCheckVvc();
	afx_msg void OnCheckScript();
	afx_msg void OnCheckArea();
	afx_msg void OnFileExtract();
	afx_msg void OnReadonly();
	afx_msg void OnEditItem();
	afx_msg void OnEditVvc();
  afx_msg void OnEditWfx();
	afx_msg void OnEditCreature();
	afx_msg void OnEditEffect();
	afx_msg void OnEditProjectile();
	afx_msg void OnEditSpell();
	afx_msg void OnEditStore();
	afx_msg void OnEditArea();
	afx_msg void OnAddcfbtospells();
	afx_msg void OnClearcfbfromspells();
	afx_msg void OnScanvariables();
	afx_msg void OnRescan3();
	afx_msg void OnCheckDialog();
	afx_msg void OnFindDialog();
	afx_msg void OnEditDialog();
	afx_msg void OnEditVariables();
	afx_msg void OnCheckEffect();
	afx_msg void OnAboutbox();
	afx_msg void OnEditAnimation();
	afx_msg void OnCheckAnimation();
	afx_msg void OnUncompressbif();
	afx_msg void OnFileSearch();
	afx_msg void OnWavctowav();
	afx_msg void OnRescan4();
	afx_msg void OnEditGames();
	afx_msg void OnCheckGame();
	afx_msg void OnEditWorldmap();
	afx_msg void OnEditGraphics();
	afx_msg void OnCheckWorldmap();
	afx_msg void OnEdit2da();
	afx_msg void OnRescan5();
	afx_msg void OnEditIds();
	afx_msg void OnImporttbg();
	afx_msg void OnCreateIAP();
	afx_msg void OnEditScript();
	afx_msg void OnCheck2da();
	afx_msg void OnLookupstrref();
	afx_msg void OnUncompresssav();
	afx_msg void OnUncompresscbf();
	afx_msg void OnAcmtowav();
	afx_msg void OnEditMusiclist();
	afx_msg void OnWavtoacm();
	afx_msg void OnWavtowavc();
	afx_msg void OnEditTileset();
	afx_msg void OnWavctowav2();
	afx_msg void OnAcmtowav2();
	afx_msg void OnFileImportd();
	afx_msg void OnEditKey();
	afx_msg void OnReorderbif();
	afx_msg void OnCheckProjectile();
	afx_msg void OnFind2da();
	virtual void OnCancel();
	afx_msg void OnSearchBam();
	afx_msg void OnEditChui();
	afx_msg void OnSearchUi();
	afx_msg void OnCheckUi();
	afx_msg void OnEditSRC();
	afx_msg void OnScanjournal();
	afx_msg void OnAvatars();
	afx_msg void OnCompressbif();
	afx_msg void OnCompresscbf();
	afx_msg void OnTispack();
	afx_msg void OnHelpReadme();
	afx_msg void OnSkimsav();
	afx_msg void OnRecompresssav();
	afx_msg void OnUsedialogf();
	afx_msg void OnToolsDecompile();
	afx_msg void OnCheckAvatar2da();
	afx_msg void OnCheckSpawnini();
	afx_msg void OnSearchBmp();
	afx_msg void OnViewDialogTree();
	afx_msg void OnToolsLoadalldialogs();
	afx_msg void OnBg1fx();
	afx_msg void OnPvrPack();
	afx_msg void OnPvrUnpack();
	afx_msg void OnCheckSql();
	afx_msg void OnToolsCreatequestlist();
	//}}AFX_MSG
	afx_msg void OnOk5();
	afx_msg void OnOk6();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHITEMDLG_H__42E3FDEA_0FCA_4733_8AD1_6E396250B019__INCLUDED_)
