// MapEdit.cpp : implementation file
//

#include "stdafx.h"

#include <fcntl.h>
#include <sys/stat.h>
#include "chitem.h"
#include "chitemDlg.h"
#include "MapEdit.h"
#include "options.h"
#include "tbg.h"
#include "ImageView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMapEdit dialog

CMapEdit::CMapEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CMapEdit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMapEdit)
	m_maxmap = _T("");
	m_caption = _T("");
	m_tooltiptext = _T("");
	m_maxarea = _T("");
	m_north = _T("");
	m_east = _T("");
	m_south = _T("");
	m_west = _T("");
	m_text = _T("");
	m_maxlink = _T("");
	//}}AFX_DATA_INIT
}

static int mapboxids[]={IDC_MOS, IDC_BROWSE1, IDC_STRREF, IDC_TEXT, IDC_MAPICONS,
IDC_BROWSE2,IDC_WIDTH, IDC_HEIGHT,IDC_MAPNUMBER, IDC_UNKNOWN18, IDC_UNKNOWN1C,
IDC_DELMAP, IDC_ADDAREA, IDC_MAPCOUNT,
0};

static int areaboxids[]={IDC_AREA,IDC_BROWSE3,IDC_BROWSE4,IDC_NAME,IDC_LONGNAME,
IDC_ICONIDX,IDC_ICONRES,IDC_XPOS,IDC_YPOS,IDC_LOADSCREEN,IDC_AREAFLAG,IDC_AREAPICKER,
IDC_CAPTION,IDC_TOOLTIP,IDC_CAPTIONTEXT,IDC_TOOLTIPTEXT,IDC_EDITLINK,IDC_DELAREA,
IDC_FLAG1,IDC_FLAG2,IDC_FLAG3,IDC_FLAG4,IDC_FLAG5,IDC_FLAG6,IDC_FLAG7,IDC_FLAG8,
IDC_SET,
0};

void CMapEdit::DoDataExchange(CDataExchange* pDX)
{
  HBITMAP hb;
  CString tmpstr;
  int pos,pos2;
  int i,j;
  int flags;
  int fc;

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMapEdit)
	DDX_Control(pDX, IDC_ICONRES, m_iconres);
	DDX_Control(pDX, IDC_AREAPICKER, m_areapicker);
	DDX_Control(pDX, IDC_MAPCOUNT, m_mappicker);
	DDX_Text(pDX, IDC_MAXMAP, m_maxmap);
	DDX_Text(pDX, IDC_CAPTIONTEXT, m_caption);
	DDX_Text(pDX, IDC_TOOLTIPTEXT, m_tooltiptext);
	DDX_Text(pDX, IDC_MAXAREA, m_maxarea);
	DDX_Text(pDX, IDC_NORTH, m_north);
	DDX_Text(pDX, IDC_EAST, m_east);
	DDX_Text(pDX, IDC_SOUTH, m_south);
	DDX_Text(pDX, IDC_WEST, m_west);
	DDX_Text(pDX, IDC_TEXT, m_text);
	DDX_Text(pDX, IDC_MAXLINK, m_maxlink);
	//}}AFX_DATA_MAP
  pos=m_mappicker.GetCurSel();
  pos2=-1;
  if(pos>=0)
  {
    RetrieveResref(tmpstr,the_map.headers[pos].mos);
    DDX_Text(pDX,IDC_MOS, tmpstr);
    DDV_MaxChars(pDX, tmpstr, 8);
    StoreResref(tmpstr,the_map.headers[pos].mos);
    
    DDX_Text(pDX, IDC_WIDTH, the_map.headers[pos].width);
    DDX_Text(pDX, IDC_HEIGHT, the_map.headers[pos].height);
    DDX_Text(pDX, IDC_MAPNUMBER, the_map.headers[pos].mapnumber);

    DDX_Text(pDX, IDC_STRREF, the_map.headers[pos].mapname);
    tmpstr=resolve_tlk_text(the_map.headers[pos].mapname);
    DDX_Text(pDX, IDC_TEXT, tmpstr);

    DDX_Text(pDX, IDC_UNKNOWN18, the_map.headers[pos].unknown18);
    DDX_Text(pDX, IDC_UNKNOWN1C, the_map.headers[pos].unknown1c);

    RetrieveResref(tmpstr,the_map.headers[pos].mapicons);
    DDX_Text(pDX,IDC_MAPICONS, tmpstr);
    DDV_MaxChars(pDX, tmpstr, 8);
    StoreResref(tmpstr,the_map.headers[pos].mapicons);

    pos2=m_areapicker.GetCurSel();
    if(pos2>=0)
    {
      RetrieveResref(tmpstr,the_map.areas[pos][pos2].arearesref);
      DDX_Text(pDX,IDC_AREA, tmpstr);
      DDV_MaxChars(pDX, tmpstr, 8);
      StoreResref(tmpstr,the_map.areas[pos][pos2].arearesref);
      if(!the_map.areas[pos][pos2].areadesc[0])
      {
        StoreResref(tmpstr,the_map.areas[pos][pos2].areadesc);
      }
      if(!the_map.areas[pos][pos2].areaname[0])
      {
        StoreResref(tmpstr,the_map.areas[pos][pos2].areaname);
      }

      RetrieveResref(tmpstr,the_map.areas[pos][pos2].areadesc);
      DDX_Text(pDX,IDC_LONGNAME, tmpstr);
      DDV_MaxChars(pDX, tmpstr, 32);
      StoreResref(tmpstr,the_map.areas[pos][pos2].areadesc);
      
      RetrieveResref(tmpstr,the_map.areas[pos][pos2].areaname);
      DDX_Text(pDX,IDC_NAME, tmpstr);
      DDV_MaxChars(pDX, tmpstr, 8);
      StoreResref(tmpstr,the_map.areas[pos][pos2].areaname);
            
      DDX_Text(pDX, IDC_AREAFLAG, the_map.areas[pos][pos2].flags);

      DDX_Text(pDX, IDC_ICONIDX, the_map.areas[pos][pos2].bamindex);
      hb=0;
      fc=the_bam.GetFrameIndex(the_map.areas[pos][pos2].bamindex,0);
      the_bam.MakeBitmap(fc,RGB(32,32,32),hb,BM_RESIZE,32,32);
      m_iconres.SetBitmap(hb);

      DDX_Text(pDX, IDC_XPOS, the_map.areas[pos][pos2].xpos);
      DDX_Text(pDX, IDC_YPOS, the_map.areas[pos][pos2].ypos);

      RetrieveResref(tmpstr,the_map.areas[pos][pos2].loadscreen);
      DDX_Text(pDX,IDC_LOADSCREEN, tmpstr);
      DDV_MaxChars(pDX, tmpstr, 8);
      StoreResref(tmpstr,the_map.areas[pos][pos2].loadscreen);
     
      DDX_Text(pDX, IDC_CAPTION, the_map.areas[pos][pos2].caption);
      DDX_Text(pDX, IDC_TOOLTIP, the_map.areas[pos][pos2].tooltip);

      j=1;
      flags=the_map.areas[pos][pos2].flags;
      for(i=0;i<8;i++)
      {
        ((CButton *) GetDlgItem(IDC_FLAG1+i))->SetCheck(!!(flags&j) );
        j<<=1;
      }

      m_north.Format("North: %d",the_map.areas[pos][pos2].northcnt);
      m_west.Format("West: %d",the_map.areas[pos][pos2].westcnt);
      m_south.Format("South: %d",the_map.areas[pos][pos2].southcnt);
      m_east.Format("East: %d",the_map.areas[pos][pos2].eastcnt);
    }
    else
    {
      m_north="";
      m_east="";
      m_south="";
      m_west="";
    }
  }
  for(i=0;mapboxids[i];i++)
  {
    GetDlgItem(mapboxids[i])->EnableWindow(pos>=0);
  }
  for(i=0;areaboxids[i];i++)
  {
    GetDlgItem(areaboxids[i])->EnableWindow(pos2>=0);
  }
}

void CMapEdit::RefreshDialog()
{
  int i;
  int pos, pos2;
  int cnt, link;
  CString tmpstr, text;

  SetWindowText("Edit Worldmap: "+itemname);

  for(pos=0;pos<the_map.mapcount;pos++)
  {
    cnt=the_map.linkcount[pos];
    for(pos2=0;pos2<cnt;pos2++)
    {
      link=the_map.arealinks[pos][pos2].link;
      if(link>=the_map.areacount[pos])
      {
        link=the_map.arealinks[pos][pos2].link=0; //fixup
      }
    }
  }

  //map picker
  m_maxmap.Format("/ %d",the_map.mapcount);
  pos=m_mappicker.GetCurSel();
  m_mappicker.ResetContent();
  for(i=0;i<the_map.mapcount;i++)
  {
    text=resolve_tlk_text(the_map.headers[i].mapname);
    tmpstr.Format("%d. %s",i+1,text);
    m_mappicker.AddString(tmpstr);
  }
  if(i)
  {
    if(pos>=i || pos<0) pos=0;
    m_mappicker.SetCurSel(pos);
    read_bam(the_map.headers[pos].mapicons);
  }
  
  //area picker
  pos2=m_areapicker.GetCurSel();
  m_areapicker.ResetContent();
  if(pos>=0 && pos<the_map.mapcount)
  {
    m_maxlink.Format("Total links: %d",the_map.headers[pos].arealinkcount);
    m_maxarea.Format("/ %d",the_map.headers[pos].areacount);
    for(i=0;i<the_map.areacount[pos];i++)
    {
      RetrieveResref(text,the_map.areas[pos][i].arearesref);
      tmpstr.Format("%d. %s",i+1, text);
      m_areapicker.AddString(tmpstr);
    }
    if(pos2>=i) pos2=m_areapicker.GetCount()-1;
    else if(pos2<0) pos2=0;
    pos2=m_areapicker.SetCurSel(pos2);
    if(pos2>=0)
    {
      m_caption=resolve_tlk_text(the_map.areas[pos][pos2].caption);
      m_tooltiptext=resolve_tlk_text(the_map.areas[pos][pos2].tooltip);
      m_north.Format("North: %d",the_map.areas[pos][pos2].northcnt);
      m_east.Format("East: %d",the_map.areas[pos][pos2].eastcnt);
      m_south.Format("South: %d",the_map.areas[pos][pos2].southcnt);
      m_west.Format("West: %d",the_map.areas[pos][pos2].westcnt);
    }
    else
    {
      m_caption="";
      m_tooltiptext="";
      m_north="";
      m_east="";
      m_south="";
      m_west="";
    }
  }
  else m_maxarea="";
}

BOOL CMapEdit::OnInitDialog() 
{
  CString tmpstr, tmpstr1, tmpstr2;

  CDialog::OnInitDialog();
  //tooltips
  {
    m_tooltip.Create(this,TTS_NOPREFIX);
    m_tooltip.SetMaxTipWidth(200);
    m_tooltip.SetTipBkColor(RGB(240,224,160));
    
    m_tooltip.AddTool(GetDlgItem(IDCANCEL), IDS_CANCEL);
    tmpstr1.LoadString(IDS_LOAD);
    tmpstr2.LoadString(IDS_WMP);
    tmpstr.Format(tmpstr1, tmpstr2);
    m_tooltip.AddTool(GetDlgItem(IDC_LOAD), tmpstr);
    tmpstr1.LoadString(IDS_LOADEX);
    tmpstr.Format(tmpstr1, tmpstr2);
    m_tooltip.AddTool(GetDlgItem(IDC_LOADEX), tmpstr);
    tmpstr1.LoadString(IDS_SAVE);
    tmpstr.Format(tmpstr1, tmpstr2);
    m_tooltip.AddTool(GetDlgItem(IDC_SAVEAS), tmpstr);
    tmpstr1.LoadString(IDS_NEW);
    tmpstr.Format(tmpstr1, tmpstr2);
    m_tooltip.AddTool(GetDlgItem(IDC_NEW), tmpstr);
    tmpstr1.LoadString(IDS_CHECK);
    tmpstr.Format(tmpstr1, tmpstr2);
    m_tooltip.AddTool(GetDlgItem(IDC_CHECK), tmpstr);
  }	
  RefreshDialog();
  UpdateData(UD_DISPLAY);
  return TRUE;
}

void CMapEdit::NewMap()
{
  itemname="new map";
  the_map.KillMaps();
  
  memset(&the_map.mainheader,0,sizeof(map_mainheader) );
  the_map.mainheader.wmapoffset=sizeof(map_mainheader);
}

BEGIN_MESSAGE_MAP(CMapEdit, CDialog)
	//{{AFX_MSG_MAP(CMapEdit)
	ON_CBN_KILLFOCUS(IDC_MAPCOUNT, OnKillfocusMapcount)
	ON_CBN_SELCHANGE(IDC_MAPCOUNT, OnSelchangeMapcount)
	ON_EN_KILLFOCUS(IDC_MOS, OnKillfocusMos)
	ON_EN_KILLFOCUS(IDC_STRREF, OnKillfocusStrref)
	ON_EN_KILLFOCUS(IDC_TEXT, OnKillfocusText)
	ON_EN_KILLFOCUS(IDC_MAPICONS, OnKillfocusMapicons)
	ON_EN_KILLFOCUS(IDC_WIDTH, OnKillfocusWidth)
	ON_EN_KILLFOCUS(IDC_HEIGHT, OnKillfocusHeight)
	ON_EN_KILLFOCUS(IDC_UNKNOWN18, OnKillfocusUnknown18)
	ON_EN_KILLFOCUS(IDC_UNKNOWN1C, OnKillfocusUnknown1c)
	ON_CBN_KILLFOCUS(IDC_AREAPICKER, OnKillfocusAreapicker)
	ON_CBN_SELCHANGE(IDC_AREAPICKER, OnSelchangeAreapicker)
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_BN_CLICKED(IDC_LOADEX, OnLoadex)
	ON_BN_CLICKED(IDC_SAVEAS, OnSaveas)
	ON_BN_CLICKED(IDC_NEW, OnNew)
	ON_EN_KILLFOCUS(IDC_NAME, OnKillfocusName)
	ON_EN_KILLFOCUS(IDC_AREA, OnKillfocusArea)
	ON_EN_KILLFOCUS(IDC_MAPNUMBER, OnKillfocusMapnumber)
	ON_BN_CLICKED(IDC_BROWSE1, OnBrowse1)
	ON_BN_CLICKED(IDC_BROWSE2, OnBrowse2)
	ON_EN_KILLFOCUS(IDC_AREAFLAG, OnKillfocusAreaflag)
	ON_BN_CLICKED(IDC_FLAG2, OnFlag2)
	ON_BN_CLICKED(IDC_FLAG3, OnFlag3)
	ON_BN_CLICKED(IDC_FLAG4, OnFlag4)
	ON_BN_CLICKED(IDC_FLAG5, OnFlag5)
	ON_BN_CLICKED(IDC_FLAG6, OnFlag6)
	ON_BN_CLICKED(IDC_FLAG7, OnFlag7)
	ON_BN_CLICKED(IDC_FLAG8, OnFlag8)
	ON_EN_KILLFOCUS(IDC_ICONIDX, OnKillfocusIconidx)
	ON_EN_KILLFOCUS(IDC_XPOS, OnKillfocusXpos)
	ON_EN_KILLFOCUS(IDC_YPOS, OnKillfocusYpos)
	ON_EN_KILLFOCUS(IDC_CAPTION, OnKillfocusCaption)
	ON_EN_KILLFOCUS(IDC_TOOLTIP, OnKillfocusTooltip)
	ON_BN_CLICKED(IDC_FLAG1, OnFlag1)
	ON_EN_KILLFOCUS(IDC_LOADSCREEN, OnKillfocusLoadscreen)
	ON_BN_CLICKED(IDC_BROWSE3, OnBrowse3)
	ON_EN_KILLFOCUS(IDC_LONGNAME, OnKillfocusLongname)
	ON_EN_KILLFOCUS(IDC_CAPTIONTEXT, OnKillfocusCaptiontext)
	ON_EN_KILLFOCUS(IDC_TOOLTIPTEXT, OnKillfocusTooltiptext)
	ON_BN_CLICKED(IDC_EDITLINK, OnEditlink)
	ON_BN_CLICKED(IDC_ADDAREA, OnAddarea)
	ON_BN_CLICKED(IDC_DELAREA, OnDelarea)
	ON_BN_CLICKED(IDC_ADDMAP, OnAddmap)
	ON_BN_CLICKED(IDC_DELMAP, OnDelmap)
	ON_BN_CLICKED(IDC_BROWSE4, OnBrowse4)
	ON_BN_CLICKED(IDC_CHECK, OnCheck)
	ON_COMMAND(ID_FILE_SAVE, OnSave)
	ON_COMMAND(ID_FILE_TBG, OnFileTbg)
	ON_COMMAND(ID_FILE_NEW, OnNew)
	ON_COMMAND(ID_FILE_LOAD, OnLoad)
	ON_COMMAND(ID_FILE_LOADEXTERNALSCRIPT, OnLoadex)
	ON_COMMAND(ID_FILE_SAVEAS, OnSaveas)
	ON_COMMAND(ID_CHECK, OnCheck)
	ON_BN_CLICKED(IDC_SET, OnSet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMapEdit message handlers

void CMapEdit::OnLoad() 
{
	int res;
	
  pickerdlg.m_restype=REF_WMP;
  pickerdlg.m_picked=itemname;
	res=pickerdlg.DoModal();
	if(res==IDOK)
	{
		res=read_worldmap(pickerdlg.m_picked);
		itemname=pickerdlg.m_picked;
    switch(res)
    {
    case -1:
      MessageBox("Worldmap loaded with errors (corrected).","Warning",MB_ICONEXCLAMATION|MB_OK);
      break;
    case 3:
      MessageBox("Headers will be resized and reordered.","Warning",MB_ICONEXCLAMATION|MB_OK);
      break;
    case 2:
      MessageBox("Headers will be resized.","Warning",MB_ICONEXCLAMATION|MB_OK);
      break;
    case 1:
      MessageBox("Headers will be reordered.","Warning",MB_ICONEXCLAMATION|MB_OK);
      break;
    case 0:
      break;
    default:
      MessageBox("Cannot read worldmap!","Error",MB_ICONEXCLAMATION|MB_OK);
      NewMap();
      break;
    }
	}
  RefreshDialog();
  UpdateData(UD_DISPLAY);
}

static char BASED_CODE szFilter[] = "Worldmap files (*.wmp)|*.wmp|All files (*.*)|*.*||";

void CMapEdit::OnLoadex() 
{
  CString filepath;
  int fhandle;
  int res;
  
  res=OFN_FILEMUSTEXIST|OFN_ENABLESIZING|OFN_EXPLORER;
  if(readonly) res|=OFN_READONLY;
  CFileDialog m_getfiledlg(TRUE, "wmp", makeitemname(".wmp",0), res, szFilter);

restart:  
  if( m_getfiledlg.DoModal() == IDOK )
  {
    filepath=m_getfiledlg.GetPathName();
    fhandle=open(filepath, O_RDONLY|O_BINARY);
    if(!fhandle)
    {
      MessageBox("Cannot open file!","Error",MB_OK);
      goto restart;
    }
    readonly=m_getfiledlg.GetReadOnlyPref();
    itemname=m_getfiledlg.GetFileTitle(); //itemname moved here because... dunno
    itemname.MakeUpper();
    res=the_map.ReadMapFromFile(fhandle,-1);
    close(fhandle);
    lastopenedoverride=filepath.Left(filepath.ReverseFind('\\'));
    switch(res)
    {
    case -1:
      MessageBox("Worldmap loaded with errors (corrected).","Warning",MB_ICONEXCLAMATION|MB_OK);
      break;
    case 3:
      MessageBox("Headers will be resized and reordered.","Warning",MB_ICONEXCLAMATION|MB_OK);
      break;
    case 2:
      MessageBox("Headers will be resized.","Warning",MB_ICONEXCLAMATION|MB_OK);
      break;
    case 1:
      MessageBox("Headers will be reordered.","Warning",MB_ICONEXCLAMATION|MB_OK);
      break;
    case 0:
      break;
    default:
      MessageBox("Cannot read worldmap!","Error",MB_ICONEXCLAMATION|MB_OK);
      NewMap();
      break;
    }
  }
  RefreshDialog();
  UpdateData(UD_DISPLAY);
}

void CMapEdit::OnSave() 
{
	SaveMap(1);	
}

void CMapEdit::OnSaveas() 
{
	SaveMap(0);	
}

void CMapEdit::SaveMap(int save) 
{
  CString filepath;
  CString newname;
  CString tmpstr;
  int res;
  
  if(readonly)
  {
    MessageBox("You opened it read only!","Warning",MB_ICONEXCLAMATION|MB_OK);
    return;
  }
  res=OFN_HIDEREADONLY|OFN_ENABLESIZING|OFN_EXPLORER;
  CFileDialog m_getfiledlg(FALSE, "wmp", makeitemname(".wmp",0), res, szFilter);

  if(save)
  {
    newname=itemname;
    filepath=makeitemname(".wmp",0);
    goto gotname;
  }
restart:  
  if( m_getfiledlg.DoModal() == IDOK )
  {
    filepath=m_getfiledlg.GetPathName();
    filepath.MakeLower();
    if(filepath.Right(4)!=".wmp")
    {
      filepath+=".wmp";
    }
    newname=m_getfiledlg.GetFileName();
    newname.MakeUpper();
    if(newname.Right(4)==".WMP") newname=newname.Left(newname.GetLength()-4);
gotname:
    if(newname.GetLength()>8 || newname.GetLength()<1 || newname.Find(" ",0)!=-1)
    {
      tmpstr.Format("The resource name '%s' is bad, it should be 8 characters long and without spaces.",newname);
      MessageBox(tmpstr,"Warning",MB_ICONEXCLAMATION|MB_OK);
      goto restart;
    }
    if(newname!=itemname && file_exists(filepath) )
    {
      res=MessageBox("Do you want to overwrite "+newname+"?","Warning",MB_ICONQUESTION|MB_YESNO);
      if(res==IDNO) goto restart;
    }
    
    res = write_map(newname, filepath);
    lastopenedoverride=filepath.Left(filepath.ReverseFind('\\'));
    switch(res)
    {
    case 0:
      itemname=newname;
      break; //saved successfully
    case -2:
      MessageBox("Error while writing file!","Error",MB_ICONSTOP|MB_OK);
      break;
    case -3:
      MessageBox("Internal Error!","Error",MB_ICONSTOP|MB_OK);
      break;
    default:
      MessageBox("Unhandled error!","Error",MB_ICONSTOP|MB_OK);
    }
  }
  RefreshDialog();
  UpdateData(UD_DISPLAY);
}

void CMapEdit::OnFileTbg() 
{
 ExportTBG(this, REF_WMP);
}

void CMapEdit::OnCheck() 
{
  int ret;

	ret=((CChitemDlg *) AfxGetMainWnd())->check_map();
  if(ret)
  {
    MessageBox(lasterrormsg,"Worldmap editor",MB_ICONEXCLAMATION|MB_OK);
  }
}

void CMapEdit::OnNew() 
{
	NewMap();
  RefreshDialog();
	UpdateData(UD_DISPLAY);
}

void CMapEdit::OnKillfocusMapcount() 
{
  RefreshDialog();
	UpdateData(UD_DISPLAY);	
}

void CMapEdit::OnSelchangeMapcount() 
{
  RefreshDialog();
	UpdateData(UD_DISPLAY);	
}

void CMapEdit::OnKillfocusMos() 
{
	UpdateData(UD_RETRIEVE);	
	UpdateData(UD_DISPLAY);	
}

void CMapEdit::OnKillfocusStrref() 
{
	UpdateData(UD_RETRIEVE);	
	UpdateData(UD_DISPLAY);	
}

void CMapEdit::OnKillfocusText() 
{
  int pos;
  CString old;

	UpdateData(UD_RETRIEVE);
  pos=m_mappicker.GetCurSel();
  if(pos<0) return;
  old=resolve_tlk_text(the_map.headers[pos].mapname);
  if(old!=m_text)
  {
    if(editflg&TLKCHANGE)
    {
      if(MessageBox("Do you want to update dialog.tlk?","Worldmap editor",MB_YESNO)!=IDYES)
      {
        return;
      }
    }
    the_map.headers[pos].mapname=store_tlk_text(the_map.headers[pos].mapname, m_text);
    RefreshDialog();
  }
	UpdateData(UD_DISPLAY);	
}

void CMapEdit::OnKillfocusMapicons() 
{
  int pos;

	UpdateData(UD_RETRIEVE);
  pos=m_mappicker.GetCurSel();
  if(pos<0) return;
  read_bam(the_map.headers[pos].mapicons);
	UpdateData(UD_DISPLAY);	
}

void CMapEdit::OnKillfocusWidth() 
{
	UpdateData(UD_RETRIEVE);	
	UpdateData(UD_DISPLAY);	
}

void CMapEdit::OnKillfocusHeight() 
{
	UpdateData(UD_RETRIEVE);	
	UpdateData(UD_DISPLAY);	
}

void CMapEdit::OnKillfocusMapnumber() 
{
	UpdateData(UD_RETRIEVE);	
	UpdateData(UD_DISPLAY);	
}

void CMapEdit::OnKillfocusUnknown18() 
{
	UpdateData(UD_RETRIEVE);	
	UpdateData(UD_DISPLAY);	
}

void CMapEdit::OnKillfocusUnknown1c() 
{
	UpdateData(UD_RETRIEVE);	
	UpdateData(UD_DISPLAY);	
}

void CMapEdit::OnKillfocusAreapicker() 
{
  RefreshDialog();
	UpdateData(UD_DISPLAY);	
}

void CMapEdit::OnSelchangeAreapicker() 
{
  RefreshDialog();
	UpdateData(UD_DISPLAY);	
}

void CMapEdit::OnKillfocusName() 
{
	UpdateData(UD_RETRIEVE);	
	UpdateData(UD_DISPLAY);	
}

void CMapEdit::OnKillfocusArea() 
{
	UpdateData(UD_RETRIEVE);
  RefreshDialog();
  UpdateData(UD_DISPLAY);	
}

void CMapEdit::OnBrowse1() 
{
  int pos;
  
  pos=m_mappicker.GetCurSel();
  if(pos<0) return;
  pickerdlg.m_restype=REF_MOS;
  RetrieveResref(pickerdlg.m_picked,the_map.headers[pos].mos);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_map.headers[pos].mos);
  }
  UpdateData(UD_DISPLAY);	
}

void CMapEdit::OnBrowse2() 
{
  int pos;
  
  pos=m_mappicker.GetCurSel();
  if(pos<0) return;
  pickerdlg.m_restype=REF_BAM;
  RetrieveResref(pickerdlg.m_picked,the_map.headers[pos].mapicons);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_map.headers[pos].mapicons);
    read_bam(the_map.headers[pos].mapicons);
  }
	UpdateData(UD_DISPLAY);	
}

void CMapEdit::OnKillfocusAreaflag() 
{
	UpdateData(UD_RETRIEVE);	
	UpdateData(UD_DISPLAY);	
}

void CMapEdit::OnFlag1() 
{
  int pos, pos2;

  pos=m_mappicker.GetCurSel();
  if(pos<0) return;
  pos2=m_areapicker.GetCurSel();
  if(pos2<0) return;
	the_map.areas[pos][pos2].flags^=1;
	UpdateData(UD_DISPLAY);	
}

void CMapEdit::OnFlag2() 
{
  int pos, pos2;

  pos=m_mappicker.GetCurSel();
  if(pos<0) return;
  pos2=m_areapicker.GetCurSel();
  if(pos2<0) return;
	the_map.areas[pos][pos2].flags^=2;
	UpdateData(UD_DISPLAY);	
}

void CMapEdit::OnFlag3() 
{
  int pos, pos2;

  pos=m_mappicker.GetCurSel();
  if(pos<0) return;
  pos2=m_areapicker.GetCurSel();
  if(pos2<0) return;
	the_map.areas[pos][pos2].flags^=4;
	UpdateData(UD_DISPLAY);	
}

void CMapEdit::OnFlag4() 
{
  int pos, pos2;

  pos=m_mappicker.GetCurSel();
  if(pos<0) return;
  pos2=m_areapicker.GetCurSel();
  if(pos2<0) return;
	the_map.areas[pos][pos2].flags^=8;
	UpdateData(UD_DISPLAY);	
}

void CMapEdit::OnFlag5() 
{
  int pos, pos2;

  pos=m_mappicker.GetCurSel();
  if(pos<0) return;
  pos2=m_areapicker.GetCurSel();
  if(pos2<0) return;
	the_map.areas[pos][pos2].flags^=16;
	UpdateData(UD_DISPLAY);	
}

void CMapEdit::OnFlag6() 
{
  int pos, pos2;

  pos=m_mappicker.GetCurSel();
  if(pos<0) return;
  pos2=m_areapicker.GetCurSel();
  if(pos2<0) return;
	the_map.areas[pos][pos2].flags^=32;
	UpdateData(UD_DISPLAY);	
}

void CMapEdit::OnFlag7() 
{
  int pos, pos2;

  pos=m_mappicker.GetCurSel();
  if(pos<0) return;
  pos2=m_areapicker.GetCurSel();
  if(pos2<0) return;
	the_map.areas[pos][pos2].flags^=64;
	UpdateData(UD_DISPLAY);	
}

void CMapEdit::OnFlag8() 
{
  int pos, pos2;

  pos=m_mappicker.GetCurSel();
  if(pos<0) return;
  pos2=m_areapicker.GetCurSel();
  if(pos2<0) return;
	the_map.areas[pos][pos2].flags^=128;
	UpdateData(UD_DISPLAY);	
}

void CMapEdit::OnKillfocusIconidx() 
{
	UpdateData(UD_RETRIEVE);	
	UpdateData(UD_DISPLAY);	
}

void CMapEdit::OnKillfocusXpos() 
{
	UpdateData(UD_RETRIEVE);	
	UpdateData(UD_DISPLAY);	
}

void CMapEdit::OnKillfocusYpos() 
{
	UpdateData(UD_RETRIEVE);	
	UpdateData(UD_DISPLAY);	
}

void CMapEdit::OnSet() 
{
  CImageView dlg;
  CString tmpstr;
  POINT point;
  int pos, pos2;
  int i;

  pos=m_mappicker.GetCurSel();
  pos2=m_areapicker.GetCurSel();
  if(pos<0 || pos2<0) return;
  RetrieveResref(tmpstr, the_map.headers[pos].mos);
  read_mos(tmpstr, &the_mos, false);
  i=the_map.headers[pos].areacount;
  //if points is null, then it will be ignored, no problem

  dlg.SetMapType(MT_BAM,(LPBYTE) the_map.areas[pos]);
  dlg.InitView(IW_SHOWALL|IW_SHOWGRID|IW_ENABLEBUTTON|IW_PLACEIMAGE, &the_mos);
  //the cursors are loaded in 'the_bam' now
  dlg.SetupAnimationPlacement(&the_bam, the_map.areas[pos][pos2].xpos,
    the_map.areas[pos][pos2].ypos, the_bam.GetFrameIndex(the_map.areas[pos][pos2].bamindex,0));    

  if(dlg.DoModal()==IDOK)
  {
    point=dlg.GetPoint(GP_POINT);
    the_map.areas[pos][pos2].xpos=point.x;
    the_map.areas[pos][pos2].ypos=point.y;
  }
  UpdateData(UD_DISPLAY);
}

void CMapEdit::OnKillfocusCaption() 
{
  int pos,pos2;
  int tmp;

	UpdateData(UD_RETRIEVE);
  pos=m_mappicker.GetCurSel();
  if(pos<0) return;
  pos2=m_areapicker.GetCurSel();
  if(pos2<0) return;
  tmp=the_map.areas[pos][pos2].caption;
  if(!the_map.areas[pos][pos2].tooltip)
  {
    the_map.areas[pos][pos2].tooltip=tmp;
  }
  m_caption=resolve_tlk_text(tmp);
	UpdateData(UD_DISPLAY);	
}

void CMapEdit::OnKillfocusTooltip() 
{
  int pos,pos2;

	UpdateData(UD_RETRIEVE);	
  pos=m_mappicker.GetCurSel();
  if(pos<0) return;
  pos2=m_areapicker.GetCurSel();
  if(pos2<0) return;
  m_tooltiptext=resolve_tlk_text(the_map.areas[pos][pos2].tooltip);
	UpdateData(UD_DISPLAY);	
}

void CMapEdit::OnKillfocusLoadscreen() 
{
	UpdateData(UD_RETRIEVE);	
	UpdateData(UD_DISPLAY);	
}

void CMapEdit::OnBrowse3() 
{
  int pos,pos2;
  
  pos=m_mappicker.GetCurSel();
  if(pos<0) return;
  pos2=m_areapicker.GetCurSel();
  if(pos2<0) return;
  pickerdlg.m_restype=REF_MOS;
  RetrieveResref(pickerdlg.m_picked,the_map.areas[pos][pos2].loadscreen);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_map.areas[pos][pos2].loadscreen);
  }
  UpdateData(UD_DISPLAY);	
}

void CMapEdit::OnBrowse4() 
{
  int pos,pos2;
  CString tmpstr;
  
  pos=m_mappicker.GetCurSel();
  if(pos<0) return;
  pos2=m_areapicker.GetCurSel();
  if(pos2<0) return;
  pickerdlg.m_restype=REF_ARE;
  RetrieveResref(pickerdlg.m_picked,the_map.areas[pos][pos2].arearesref);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_map.areas[pos][pos2].arearesref);
    RetrieveResref(tmpstr,the_map.areas[pos][pos2].areaname);
    if(tmpstr.IsEmpty())
    {
      StoreResref(pickerdlg.m_picked,the_map.areas[pos][pos2].areaname);
    }
  }
  UpdateData(UD_DISPLAY);	
}

void CMapEdit::OnKillfocusLongname() 
{
	UpdateData(UD_RETRIEVE);	
	UpdateData(UD_DISPLAY);	
}

void CMapEdit::OnKillfocusCaptiontext() 
{
  int pos,pos2;
  CString old;

	UpdateData(UD_RETRIEVE);	
  pos=m_mappicker.GetCurSel();
  if(pos<0) return;
  pos2=m_areapicker.GetCurSel();
  if(pos2<0) return;
  old=resolve_tlk_text(the_map.areas[pos][pos2].caption);
  if(old!=m_caption)
  {
    if(editflg&TLKCHANGE)
    {
      if(MessageBox("Do you want to update dialog.tlk?","Worldmap editor",MB_YESNO)!=IDYES)
      {
        return;
      }
    }
    the_map.areas[pos][pos2].caption=store_tlk_text(the_map.areas[pos][pos2].caption, m_caption);
    RefreshDialog();
  }
	UpdateData(UD_DISPLAY);	
}

void CMapEdit::OnKillfocusTooltiptext() 
{
  int pos,pos2;
  CString old;

	UpdateData(UD_RETRIEVE);	
  pos=m_mappicker.GetCurSel();
  if(pos<0) return;
  pos2=m_areapicker.GetCurSel();
  if(pos2<0) return;
  old=resolve_tlk_text(the_map.areas[pos][pos2].tooltip);
  if(old!=m_tooltiptext)
  {
    if(editflg&TLKCHANGE)
    {
      if(MessageBox("Do you want to update dialog.tlk?","Worldmap editor",MB_YESNO)!=IDYES)
      {
        return;
      }
    }
    the_map.areas[pos][pos2].tooltip=store_tlk_text(the_map.areas[pos][pos2].tooltip, m_tooltiptext);
    RefreshDialog();
  }
	UpdateData(UD_DISPLAY);	
}

void CMapEdit::OnEditlink() 
{
  int pos,pos2;
	CMapLink dlg;
	
  pos=m_mappicker.GetCurSel();
  if(pos<0) return;
  pos2=m_areapicker.GetCurSel();
  if(pos2<0) return;
  dlg.SetLink(pos,pos2);
  dlg.DoModal();
  //
  //
  RefreshDialog();
  UpdateData(UD_DISPLAY);
}

void CMapEdit::OnAddarea() 
{
  int pos,pos2;
  map_area *newareas;
  
  pos=m_mappicker.GetCurSel();
  if(pos<0) pos=0;
  pos2=the_map.areacount[pos];
  the_map.areacount[pos]++;
  newareas=new map_area[the_map.areacount[pos]];
  if(!newareas)
  {
    the_map.areacount[pos]--;
    return;
  }
  memcpy(newareas,the_map.areas[pos],pos2*sizeof(map_area) );
  memset(newareas+pos2,0,sizeof(map_area) );
  delete [] the_map.areas[pos];
  the_map.areas[pos]=newareas;
  the_map.headers[pos].areacount=the_map.areacount[pos];

  RefreshDialog();
  UpdateData(UD_DISPLAY);
}

//complex function for removing an area and its links
//both to and from links need to be deleted and remaining link indices recalculated
int RemoveLink(int map, int from, int count)
{
  map_arealink *newlinks;
  int newcount;
  int i;
  int areafrom, areaend;

  if(map<0 || map>=the_map.mapcount) return -1;
  if(from<0) return -1;
  if(count<0) return -1;
  if(from+count>the_map.headers[map].arealinkcount) return -1;
//  if(!count) return 0;
  //removing a range of links
  newcount=the_map.linkcount[map]-count;
  newlinks=new map_arealink[newcount];
  if(!newlinks) return -3;
  memcpy(newlinks,the_map.arealinks[map],from*sizeof(map_arealink) );
  memcpy(newlinks+from,the_map.arealinks[map]+from+count,(newcount-from)*sizeof(map_arealink) );
  delete [] the_map.arealinks[map];
  the_map.arealinks[map]=newlinks;
  the_map.headers[map].arealinkcount=the_map.linkcount[map]=newcount;
  //adjusting areas referencing links greater than the removed links
  for(i=0;i<the_map.areacount[map];i++)
  {
    areafrom=the_map.areas[map][i].northidx;
    areaend=areafrom+the_map.areas[map][i].northcnt;
    if(areafrom>from)
    {
      the_map.areas[map][i].northidx-=count;
    }
    else if((areafrom<=from) && (areaend>from))
    {
      the_map.areas[map][i].northcnt-=count; //decreasing the link count with the removed links' count
    }

    areafrom=the_map.areas[map][i].eastidx;
    areaend=areafrom+the_map.areas[map][i].eastcnt;
    if(areafrom>from) the_map.areas[map][i].eastidx-=count;
    else if((areafrom<=from) && (areaend>from))
    {
      the_map.areas[map][i].eastcnt-=count;
    }

    areafrom=the_map.areas[map][i].southidx;
    areaend=areafrom+the_map.areas[map][i].southcnt;
    if(areafrom>from) the_map.areas[map][i].southidx-=count;
    else if((areafrom<=from) && (areaend>from))
    {
      the_map.areas[map][i].southcnt-=count;
    }

    areafrom=the_map.areas[map][i].westidx;
    areaend=areafrom+the_map.areas[map][i].westcnt;
    if(areafrom>from) the_map.areas[map][i].westidx-=count;
    else if((areafrom<=from) && (areaend>from))
    {
      the_map.areas[map][i].westcnt-=count;
    }
  }
  return 0;
}

void CMapEdit::OnDelarea() 
{
  int pos,pos2;
  map_area *newareas;
  int i,j;
  
  pos=m_mappicker.GetCurSel();
  if(pos<0) return;
  pos2=m_areapicker.GetCurSel();
  if(pos2<0) return;

  //removing links of the deleted area
  if(RemoveLink(pos,the_map.areas[pos][pos2].northidx,the_map.areas[pos][pos2].northcnt))
  {
    return;
  }
  if(RemoveLink(pos,the_map.areas[pos][pos2].eastidx,the_map.areas[pos][pos2].eastcnt))
  {
    return;
  }
  if(RemoveLink(pos,the_map.areas[pos][pos2].southidx,the_map.areas[pos][pos2].southcnt))
  {
    return;
  }
  if(RemoveLink(pos,the_map.areas[pos][pos2].westidx,the_map.areas[pos][pos2].westcnt))
  {
    return;
  }
  //removing links referencing the deleted area
  i=0;
  do
  {
    if(the_map.arealinks[pos][i].link==pos2)
    {
      if(RemoveLink(pos,i,1))
      {
        return;
      }
    }
    else i++;
  }
  while(i<the_map.linkcount[pos]);
  //adjusting links referencing areas greater than the removed area
  for(i=0;i<the_map.linkcount[pos];i++)
  {
    if(the_map.arealinks[pos][i].link>pos2) the_map.arealinks[pos][i].link--;
  }
  //removing the unlinked area
  the_map.areacount[pos]--;
  newareas=new map_area[the_map.areacount[pos]];
  if(!newareas)
  {
    the_map.areacount[pos]++;
    return;
  }
  j=0;
  //copying all areas except the deleted one
  for(i=0;i<the_map.headers[pos].areacount;i++)
  {
    if(i!=pos2)
    {
      //adjusting the link indices
      newareas[j]=the_map.areas[pos][i];
      j++;
    }
    else
    {
      i=pos2;
    }
  }  

  delete [] the_map.areas[pos];
  the_map.areas[pos]=newareas;
  the_map.headers[pos].areacount=the_map.areacount[pos];

  RefreshDialog();
  UpdateData(UD_DISPLAY);
}

void CMapEdit::OnAddmap() 
{
  map_header *newheaders;
	int *newareacount;
  int *newlinkcount;
  map_area **newareas;
  map_arealink **newlinks;
  int pos;
  int i,j;

  pos=m_mappicker.GetCurSel()+1;
  the_map.mainheader.wmapcount++;
  newheaders=new map_header[the_map.mainheader.wmapcount];
  if(!newheaders)
  {
    the_map.mainheader.wmapcount--;
    return;
  }
  newareacount=new int[the_map.mainheader.wmapcount];
  if(!newareacount)
  {
    the_map.mainheader.wmapcount--;
    delete [] newheaders;
    return;
  }
  newlinkcount=new int[the_map.mainheader.wmapcount];
  if(!newlinkcount)
  {
    the_map.mainheader.wmapcount--;
    delete [] newheaders;
    delete [] newareacount;
    return;
  }
  newareas=new map_area*[the_map.mainheader.wmapcount];
  if(!newareas)
  {
    the_map.mainheader.wmapcount--;
    delete [] newheaders;
    delete [] newareacount;
    delete [] newlinkcount;
    return;
  }
  newlinks=new map_arealink*[the_map.mainheader.wmapcount];
  if(!newlinks)
  {
    the_map.mainheader.wmapcount--;
    delete [] newheaders;
    delete [] newareacount;
    delete [] newlinkcount;
    delete [] newareas;
    return;
  }
  j=0;
  for(i=0;i<the_map.mainheader.wmapcount;i++)
  {
    if(i!=pos)
    {
      newheaders[i]=the_map.headers[j];
      newareacount[i]=the_map.areacount[j];
      newlinkcount[i]=the_map.linkcount[j];
      newareas[i]=the_map.areas[j];
      newlinks[i]=the_map.arealinks[j];
      j++;
    }
    else
    {
      memset(newheaders+i,0,sizeof(map_header) );
      newheaders[i].mapname=-1;
      newheaders[i].mapnumber=i;
      newareacount[i]=newlinkcount[i]=0;
      newareas[i]=NULL;
      newlinks[i]=NULL;
    }
  }
  delete [] the_map.headers;
  the_map.headers=newheaders;
  delete [] the_map.areacount;
  the_map.areacount=newareacount;
  delete [] the_map.linkcount;
  the_map.linkcount=newlinkcount;
  delete [] the_map.areas;
  the_map.areas=newareas;
  delete [] the_map.arealinks;
  the_map.arealinks=newlinks;
  the_map.mapcount=the_map.mainheader.wmapcount;
  m_mappicker.AddString(""); //dummy string to set pos
  m_mappicker.SetCurSel(pos);
  RefreshDialog();
  UpdateData(UD_DISPLAY);
}

void CMapEdit::OnDelmap() 
{
  int *newareacount;
  int *newlinkcount;
  map_header *newheaders;
  map_area **newareas;
  map_arealink **newlinks;
  int pos;
  int i,j;

  pos=m_mappicker.GetCurSel();
  the_map.KillAreas(pos);
  the_map.KillAreaLinks(pos);
  the_map.mainheader.wmapcount--;
  newareacount=new int[the_map.mainheader.wmapcount];
  if(!newareacount)
  {
    the_map.mainheader.wmapcount++;
    return;
  }
  newlinkcount=new int[the_map.mainheader.wmapcount];
  if(!newlinkcount)
  {
    the_map.mainheader.wmapcount++;
    delete [] newareacount;
    return;
  }
  newareas=new map_area*[the_map.mainheader.wmapcount];
  if(!newareas)
  {
    the_map.mainheader.wmapcount++;
    delete [] newareacount;
    delete [] newlinkcount;
    return;
  }
  newlinks=new map_arealink*[the_map.mainheader.wmapcount];
  if(!newareas)
  {
    the_map.mainheader.wmapcount++;
    delete [] newareacount;
    delete [] newlinkcount;
    delete [] newareas;
    return;
  }

  j=0;
  for(i=0;i<the_map.mapcount;i++)
  {
    if(i!=pos)
    {
      newareacount[j]=the_map.areacount[i];
      newlinkcount[j]=the_map.linkcount[i];
      newareas[j]=the_map.areas[i];
      newlinks[j]=the_map.arealinks[i];
      j++;
    }
  }
  delete [] the_map.areacount;
  the_map.areacount=newareacount;
  delete [] the_map.linkcount;
  the_map.linkcount=newlinkcount;
  delete [] the_map.areas;
  the_map.areas=newareas;
  delete [] the_map.arealinks;
  the_map.arealinks=newlinks;
  newheaders=new map_header[the_map.mainheader.wmapcount];
  if(!newheaders)
  {
    return;
  }
  memcpy(newheaders,the_map.headers,pos*sizeof(map_header) );
  memcpy(newheaders+pos,the_map.headers+pos+1,(the_map.mainheader.wmapcount-pos)*sizeof(map_header) );
  delete [] the_map.headers;
  the_map.headers=newheaders;

  the_map.mapcount=the_map.mainheader.wmapcount;
  RefreshDialog();
  UpdateData(UD_DISPLAY);
}

/////////////////////////////////////////////////////////////////////////////
// CMapLink dialog

CMapLink::CMapLink(CWnd* pParent /*=NULL*/)
	: CDialog(CMapLink::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMapLink)
	m_direction = 0;
	m_maxdir = _T("");
	m_linkpicker = -1;
	m_arealink = -1;
	//}}AFX_DATA_INIT
  m_first=0;
}

//overall link number: m_first+m_linkpicker
//first link index for this area/direction: m_first
//picked link (for this area/direction): m_linkpicker
//idx from available links: m_areapicker
//area index: m_arealink

void CMapLink::DoDataExchange(CDataExchange* pDX)
{
  CString tmpstr;
  int i;

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMapLink)
	DDX_Text(pDX, IDC_MAXDIR, m_maxdir);
	DDX_Control(pDX, IDC_ENTRANCE, m_entrancenamepicker);
	DDX_Control(pDX, IDC_AREALINK, m_areapicker);
	DDX_CBIndex(pDX, IDC_DIRECTION, m_direction);
	DDX_CBIndex(pDX, IDC_LINKPICKER, m_linkpicker);
	DDX_CBIndex(pDX, IDC_AREALINK, m_arealink);
	//}}AFX_DATA_MAP
  if(m_first+m_linkpicker>=0)
  {
    RetrieveVariable(tmpstr, the_map.arealinks[m_map][m_first+m_linkpicker].entryname);
    DDX_Text(pDX, IDC_ENTRANCE, tmpstr);
    DDV_MaxChars(pDX, tmpstr, 32);
    StoreVariable(tmpstr, the_map.arealinks[m_map][m_first+m_linkpicker].entryname);

    DDX_Text(pDX,IDC_DISTANCE,the_map.arealinks[m_map][m_first+m_linkpicker].distancescale);
    DDX_Text(pDX, IDC_FLAGS, the_map.arealinks[m_map][m_first+m_linkpicker].flags);
    for(i=0;i<5;i++)
    {
      RetrieveResref(tmpstr, the_map.arealinks[m_map][m_first+m_linkpicker].encounters[i]);
      DDX_Text(pDX, IDC_U1+i, tmpstr);
      DDV_MaxChars(pDX, tmpstr, 8);
      StoreResref(tmpstr, the_map.arealinks[m_map][m_first+m_linkpicker].encounters[i]);
    }

    DDX_Text(pDX, IDC_CHANCE, the_map.arealinks[m_map][m_first+m_linkpicker].chance);
  }
}

static int linkboxids[]={IDC_LINKPICKER, IDC_AREALINK, IDC_ENTRANCE, IDC_REMOVE,
IDC_CHANCE,IDC_FLAGS, IDC_DISTANCE,IDC_U1,IDC_U2,IDC_U3,IDC_U4,IDC_U5,
IDC_BROWSE1,IDC_BROWSE2,IDC_BROWSE3,IDC_BROWSE4,IDC_BROWSE5,
0};

void CMapLink::RefreshLink()
{
  CComboBox *cb;
  CString tmpstr, tmp;
  int i;
  int cnt;
  int link;
  int pos;
  int flg;

  cb=(CComboBox *) GetDlgItem(IDC_DIRECTION);
  if(cb)
  {
    cb->ResetContent();
    tmpstr.Format("North (%d)",the_map.areas[m_map][m_area].northcnt);
    cb->AddString(tmpstr);
    tmpstr.Format("West (%d)",the_map.areas[m_map][m_area].westcnt);
    cb->AddString(tmpstr);
    tmpstr.Format("South (%d)",the_map.areas[m_map][m_area].southcnt);
    cb->AddString(tmpstr);
    tmpstr.Format("East (%d)",the_map.areas[m_map][m_area].eastcnt);
    cb->AddString(tmpstr);
  }
  switch(m_direction)
  {
  case 0:
    m_maxdir.Format("North (%d)",cnt=the_map.areas[m_map][m_area].northcnt);
    m_first=the_map.areas[m_map][m_area].northidx;
    break;
  case 1:
    m_maxdir.Format("West (%d)",cnt=the_map.areas[m_map][m_area].westcnt);
    m_first=the_map.areas[m_map][m_area].westidx;
    break;
  case 2:
    m_maxdir.Format("South (%d)",cnt=the_map.areas[m_map][m_area].southcnt);
    m_first=the_map.areas[m_map][m_area].southidx;
    break;
  case 3:
    m_maxdir.Format("East (%d)",cnt=the_map.areas[m_map][m_area].eastcnt);
    m_first=the_map.areas[m_map][m_area].eastidx;
    break;
  }
  
  cb=(CComboBox *) GetDlgItem(IDC_LINKPICKER);
  pos=cb->GetCurSel();
  cb->ResetContent();
  for(i=0;i<cnt;i++)
  {
    link=the_map.arealinks[m_map][m_first+i].link;
    RetrieveResref(tmp, the_map.areas[m_map][link].arearesref);
    tmpstr.Format("%d %s %-.32s",i+1, tmp, the_map.arealinks[m_map][m_first+i].entryname);
    cb->AddString(tmpstr);
  }
  if((pos<0) || (pos>=i)) pos=0;
  m_linkpicker=cb->SetCurSel(pos);
  flg=m_linkpicker>=0;
  if(flg)
  {
    m_arealink=the_map.arealinks[m_map][m_first+m_linkpicker].link;
  }
  else m_arealink=-1;

  for(i=0;linkboxids[i];i++)
  {
    GetDlgItem(linkboxids[i])->EnableWindow(flg);
  }
  if(flg)
  {
    RetrieveResref(tmpstr,the_map.areas[m_map][m_arealink].arearesref);
    fill_destination(tmpstr, &m_entrancenamepicker);
    m_entrancenamepicker.SelectString(0,the_map.arealinks[m_map][m_first+m_linkpicker].entryname);
  }
}

BOOL CMapLink::OnInitDialog() 
{
  CString tmpstr, tmp;
  int i;

	CDialog::OnInitDialog();	
  RetrieveResref(tmpstr,the_map.areas[m_map][m_area].arearesref);
  tmp.Format("Edit links of %s",tmpstr);
  SetWindowText(tmp);
  m_areapicker.ResetContent();
  for(i=0;i<the_map.headers[m_map].areacount;i++)
  {
    RetrieveResref(tmpstr,the_map.areas[m_map][i].arearesref);
    m_areapicker.AddString(tmpstr);
  }
  if(the_map.areas[m_map][0].northcnt)
  {
    m_linkpicker=0;
    i=the_map.areas[m_map][0].northidx;
    m_arealink=the_map.arealinks[m_map][i].link;
  }
  else
  {
    m_linkpicker=-1;
    m_arealink=-1;
  }
	RefreshLink();
	UpdateData(UD_DISPLAY);
	return TRUE;
}

BEGIN_MESSAGE_MAP(CMapLink, CDialog)
	//{{AFX_MSG_MAP(CMapLink)
	ON_CBN_KILLFOCUS(IDC_DIRECTION, OnKillfocusDirection)
	ON_CBN_SELCHANGE(IDC_DIRECTION, OnSelchangeDirection)
	ON_CBN_KILLFOCUS(IDC_AREALINK, OnKillfocusArealink)
	ON_CBN_SELCHANGE(IDC_AREALINK, OnSelchangeArealink)
	ON_CBN_KILLFOCUS(IDC_LINKPICKER, OnKillfocusLinkpicker)
	ON_CBN_SELCHANGE(IDC_LINKPICKER, OnSelchangeLinkpicker)
	ON_CBN_KILLFOCUS(IDC_ENTRANCE, OnKillfocusEntrance)
	ON_EN_KILLFOCUS(IDC_DISTANCE, OnKillfocusDistance)
	ON_EN_KILLFOCUS(IDC_FLAGS, OnKillfocusFlags)
	ON_EN_KILLFOCUS(IDC_U1, OnKillfocusU1)
	ON_EN_KILLFOCUS(IDC_U2, OnKillfocusU2)
	ON_EN_KILLFOCUS(IDC_U3, OnKillfocusU3)
	ON_EN_KILLFOCUS(IDC_U4, OnKillfocusU4)
	ON_EN_KILLFOCUS(IDC_U5, OnKillfocusU5)
	ON_EN_KILLFOCUS(IDC_CHANCE, OnKillfocusChance)
	ON_BN_CLICKED(IDC_BROWSE1, OnBrowse1)
	ON_BN_CLICKED(IDC_BROWSE2, OnBrowse2)
	ON_BN_CLICKED(IDC_BROWSE3, OnBrowse3)
	ON_BN_CLICKED(IDC_BROWSE4, OnBrowse4)
	ON_BN_CLICKED(IDC_BROWSE5, OnBrowse5)
	ON_CBN_SELCHANGE(IDC_ENTRANCE, OnSelchangeEntrance)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMapLink message handlers

void CMapLink::OnKillfocusDirection() 
{
	UpdateData(UD_RETRIEVE);
  RefreshLink();
	UpdateData(UD_DISPLAY);
}

void CMapLink::OnSelchangeDirection() 
{
  UpdateData(UD_RETRIEVE);
  RefreshLink();
	UpdateData(UD_DISPLAY);
}

void CMapLink::OnKillfocusLinkpicker() 
{
  UpdateData(UD_RETRIEVE);
  RefreshLink();
	UpdateData(UD_DISPLAY);
}

void CMapLink::OnSelchangeLinkpicker() 
{
  RefreshLink();
	UpdateData(UD_DISPLAY);
}

void CMapLink::OnKillfocusArealink() 
{
  CString tmpstr;

  UpdateData(UD_RETRIEVE);
  the_map.arealinks[m_map][m_first+m_linkpicker].link=m_arealink;
  RefreshLink();
	UpdateData(UD_DISPLAY);
}

void CMapLink::OnSelchangeArealink() 
{
  CString tmpstr;

  m_arealink=m_areapicker.GetCurSel();
  if(m_arealink>=0)
  {
    the_map.arealinks[m_map][m_first+m_linkpicker].link=m_arealink;
    RefreshLink();
    UpdateData(UD_DISPLAY);
  }
}

void CMapLink::OnKillfocusEntrance() 
{
  UpdateData(UD_RETRIEVE);
  RefreshLink();
	UpdateData(UD_DISPLAY);
}

void CMapLink::OnSelchangeEntrance() 
{
  int x;
  CString tmpstr;

  x=m_entrancenamepicker.GetCurSel();
  if(x<0) return;
  m_entrancenamepicker.GetLBText(x,tmpstr);
  StoreVariable(tmpstr, the_map.arealinks[m_map][m_first+m_linkpicker].entryname);
  RefreshLink();
	UpdateData(UD_DISPLAY);
}

void CMapLink::OnKillfocusDistance() 
{
  UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CMapLink::OnKillfocusFlags() 
{
  UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CMapLink::OnKillfocusU1() 
{
  UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CMapLink::OnKillfocusU2() 
{
  UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CMapLink::OnKillfocusU3() 
{
  UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CMapLink::OnKillfocusU4() 
{
  UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CMapLink::OnKillfocusU5() 
{
  UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CMapLink::OnKillfocusChance() 
{
  UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

void CMapLink::Encounters(int idx)
{
  pickerdlg.m_restype=REF_ARE;
  RetrieveResref(pickerdlg.m_picked,the_map.arealinks[m_map][m_area].encounters[idx]);
  if(pickerdlg.DoModal()==IDOK)
  {
    StoreResref(pickerdlg.m_picked,the_map.arealinks[m_map][m_area].encounters[idx]);
  }
  UpdateData(UD_DISPLAY);	
}

void CMapLink::OnBrowse1() 
{
  Encounters(0);
}

void CMapLink::OnBrowse2() 
{
  Encounters(1);
}

void CMapLink::OnBrowse3() 
{
  Encounters(2);
}

void CMapLink::OnBrowse4() 
{
  Encounters(3);
}

void CMapLink::OnBrowse5() 
{
  Encounters(4);
}

void CMapLink::OnAdd() 
{
  map_arealink *newlinks;
  int pos;
  int i;

  the_map.linkcount[m_map]++;
	newlinks=new map_arealink[the_map.linkcount[m_map]];
  if(!newlinks)
  {
    the_map.linkcount[m_map]--;
    return;
  }
  switch(m_direction)
  {
  case 0:
    pos=the_map.areas[m_map][m_area].northidx+the_map.areas[m_map][m_area].northcnt;
    the_map.areas[m_map][m_area].northcnt++;
    break;
  case 1:
    pos=the_map.areas[m_map][m_area].westidx+the_map.areas[m_map][m_area].westcnt;
    the_map.areas[m_map][m_area].westcnt++;
    break;
  case 2:
    pos=the_map.areas[m_map][m_area].southidx+the_map.areas[m_map][m_area].southcnt;
    the_map.areas[m_map][m_area].southcnt++;
    break;
  case 3:
    pos=the_map.areas[m_map][m_area].eastidx+the_map.areas[m_map][m_area].eastcnt;
    the_map.areas[m_map][m_area].eastcnt++;
    break;
  }
  memcpy(newlinks, the_map.arealinks[m_map],pos*sizeof(map_arealink) );
	memset(newlinks+pos,0,sizeof(map_arealink) );
  memcpy(newlinks+pos+1,the_map.arealinks[m_map]+pos,(the_map.headers[m_map].arealinkcount-pos)*sizeof(map_arealink) );
  delete [] the_map.arealinks[m_map];
  the_map.arealinks[m_map]=newlinks;
  the_map.headers[m_map].arealinkcount++;
  for(i=0;i<the_map.areacount[m_map];i++)
  {
    if((i!=m_area) || (m_direction!=0) )
    {
      if(the_map.areas[m_map][i].northidx>=pos)
      {
        the_map.areas[m_map][i].northidx++;
      }
    }
    if((i!=m_area) || (m_direction!=1) )
    {
      if(the_map.areas[m_map][i].westidx>=pos)
      {
        the_map.areas[m_map][i].westidx++;
      }
    }
    if((i!=m_area) || (m_direction!=2) )
    {
      if(the_map.areas[m_map][i].southidx>=pos)
      {
        the_map.areas[m_map][i].southidx++;
      }
    }
    if((i!=m_area) || (m_direction!=3) )
    {
      if(the_map.areas[m_map][i].eastidx>=pos)
      {
        the_map.areas[m_map][i].eastidx++;
      }
    }
  }
  RefreshLink();
  UpdateData(UD_DISPLAY);	
}

void CMapLink::OnRemove() 
{
  if(m_linkpicker<0) return;
	RemoveLink(m_map, m_first+m_linkpicker, 1);
  RefreshLink();
  UpdateData(UD_DISPLAY);	
}

BOOL CMapEdit::PreTranslateMessage(MSG* pMsg) 
{
  m_tooltip.RelayEvent(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}
