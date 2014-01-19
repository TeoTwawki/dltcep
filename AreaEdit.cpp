// AreaEdit.cpp : implementation file
//

#include "stdafx.h"

#include <fcntl.h>
#include <sys/stat.h>
#include "chitem.h"
#include "chitemDlg.h"
#include "AreaEdit.h"
#include "WedEdit.h"
#include "StrRefDlg.h"
#include "tbg.h"
#include "MyFileDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAreaEdit dialog

CAreaEdit::CAreaEdit(CWnd* pParent /*=NULL*/)
: CDialog(CAreaEdit::IDD, pParent)
{
	m_pModelessPropSheet = NULL;
	//{{AFX_DATA_INIT(CAreaEdit)
	//}}AFX_DATA_INIT
}

void CAreaEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAreaEdit)
	//}}AFX_DATA_MAP
}

void CAreaEdit::NewArea()
{
	itemname="new area";
  the_area.new_area();
  the_mos.new_mos();
  memset(&the_area.header,0,sizeof(area_header));
  memset(&the_area.songheader,0,sizeof(area_song));
  memset(&the_area.intheader,0,sizeof(area_int));
  memset(&the_area.wedheader,0,sizeof(wed_header));
  memset(&the_area.secheader,0,sizeof(wed_secondary_header));
  if(iwd2_structures())
  {
    the_area.revision=91;
  }
  else
  {
    the_area.revision=10;
  }
  if(the_area.DefaultAreaOverlays())
  {
    MessageBox("Out of memory","Error",MB_ICONSTOP|MB_OK);
  }
  the_area.m_changed=false;
}

BEGIN_MESSAGE_MAP(CAreaEdit, CDialog)
//{{AFX_MSG_MAP(CAreaEdit)
ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_BN_CLICKED(IDC_LOADEX, OnLoadex)
	ON_BN_CLICKED(IDC_SAVEAS, OnSaveas)
	ON_BN_CLICKED(IDC_NEW, OnNew)
	ON_BN_CLICKED(IDC_CHECK, OnCheck)
	ON_COMMAND(ID_FILE_SAVE, OnSave)
	ON_COMMAND(ID_FILE_TBG, OnFileTbg)
	ON_COMMAND(ID_FILE_TP2, OnFileTp2)
	ON_COMMAND(ID_TOOLS_MIRRORAREAVERTICALLY, OnToolsMirrorareavertically)
	ON_COMMAND(ID_REPAIRWED, OnRepairwed)
	ON_COMMAND(ID_REPAIRWED2, OnRepairwed2)
	ON_COMMAND(ID_TOOLS_LOOKUPSTRREF, OnToolsLookupstrref)
	ON_COMMAND(ID_TOOLS_CREATEMINIMAP, OnToolsCreateminimap)
	ON_COMMAND(ID_CHECK, OnCheck)
	ON_COMMAND(ID_FILE_NEW, OnNew)
	ON_COMMAND(ID_FILE_LOAD, OnLoad)
	ON_COMMAND(ID_FILE_LOADEXTERNALSCRIPT, OnLoadex)
	ON_COMMAND(ID_FILE_SAVEAS, OnSaveas)
	ON_COMMAND(ID_TOOLS_CONVERTNIGHT, OnToolsConvertnight)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAreaEdit message handlers
void CAreaEdit::OnLoad()
{
	int res;

	pickerdlg.m_restype=REF_ARE;
	pickerdlg.m_picked=itemname;
	res=pickerdlg.DoModal();
	if(res==IDOK)
	{
		res=read_area(pickerdlg.m_picked, NULL);
    switch(res)
    {
    case -3:
      MessageBox("Not enough memory.","Error",MB_ICONSTOP|MB_OK);
		itemname=pickerdlg.m_picked;
      break;
    case 3:
      MessageBox("Area loaded without wed (cannot edit doors).","Warning",MB_ICONEXCLAMATION|MB_OK);
		itemname=pickerdlg.m_picked;
      break;
    case 2:
      MessageBox("Area loaded with some errors (minor inconsistency).","Warning",MB_ICONEXCLAMATION|MB_OK);
		itemname=pickerdlg.m_picked;
      break;
    case 0:
		itemname=pickerdlg.m_picked;
      break;
    case 1:
      MessageBox("Area will be reordered to official standard.","Warning",MB_ICONEXCLAMATION|MB_OK);
		itemname=pickerdlg.m_picked;
      break;
    default:
      MessageBox("Cannot read area!","Error",MB_ICONSTOP|MB_OK);
      NewArea();
      break;
    }
	}
  SetWindowText("Edit area: "+itemname);
  m_pModelessPropSheet->RefreshDialog();
  UpdateData(UD_DISPLAY);
}

static char BASED_CODE szFilter[] = "Area files (*.are)|*.are|All files (*.*)|*.*||";

void CAreaEdit::OnLoadex()
{
  int fhandle;
  int res;

  res=OFN_FILEMUSTEXIST|OFN_ENABLESIZING|OFN_EXPLORER;
  if(readonly) res|=OFN_READONLY;
  CMyFileDialog m_getfiledlg(TRUE, "are", makeitemname(".are",0), res, szFilter);

restart:
  //if (filepath.GetLength()) strncpy(m_getfiledlg.m_ofn.lpstrFile,filepath, filepath.GetLength()+1);
  if( m_getfiledlg.DoModal() == IDOK )
  {
    filepath=m_getfiledlg.GetPathName();
    fhandle=open(filepath, O_RDONLY|O_BINARY);
    if(!fhandle)
    {
      MessageBox("Cannot open file!","Error",MB_ICONSTOP|MB_OK);
      goto restart;
    }
    readonly=m_getfiledlg.GetReadOnlyPref();
    the_mos.new_mos();
    res=the_area.ReadAreaFromFile(fhandle,-1);
    close(fhandle);
    lastopenedoverride=filepath.Left(filepath.ReverseFind('\\'));
    if(res>=0)
    {
      res=ReadWed(res);
    }
    switch(res)
    {
    case -3:
      MessageBox("Not enough memory.","Error",MB_ICONSTOP|MB_OK);
      itemname=m_getfiledlg.GetFileTitle();
      itemname.MakeUpper();
      break;
    case 3:
      MessageBox("Area loaded without wed (cannot edit doors).","Warning",MB_ICONEXCLAMATION|MB_OK);
      itemname=m_getfiledlg.GetFileTitle();
      itemname.MakeUpper();
      break;
    case 2:
      MessageBox("Area loaded with some errors (minor inconsistency).","Warning",MB_ICONEXCLAMATION|MB_OK);
      itemname=m_getfiledlg.GetFileTitle();
      itemname.MakeUpper();
      break;
    case 1:
      MessageBox("Area will be reordered to official standard.","Warning",MB_ICONEXCLAMATION|MB_OK);
      itemname=m_getfiledlg.GetFileTitle();
      itemname.MakeUpper();
      break;
    case 0:
      itemname=m_getfiledlg.GetFileTitle();
      itemname.MakeUpper();
      break;
    default:
      MessageBox("Cannot read area!","Error",MB_ICONSTOP|MB_OK);
      NewArea();
      break;
    }
  }
  SetWindowText("Edit area: "+itemname);
  m_pModelessPropSheet->RefreshDialog();
  UpdateData(UD_DISPLAY);
}

void CAreaEdit::OnSave()
{
  SaveArea(1);
}

void CAreaEdit::OnSaveas()
{
  SaveArea(0);
}

void CAreaEdit::SaveArea(int save)
{
  CString tmpath;
  CString newname;
  CString tmpstr;
  int res;

  if(readonly)
  {
    MessageBox("You opened it read only!","Warning",MB_ICONEXCLAMATION|MB_OK);
    return;
  }
  res=OFN_HIDEREADONLY|OFN_ENABLESIZING|OFN_EXPLORER;
  CMyFileDialog m_getfiledlg(FALSE, "are", makeitemname(".are",0), res, szFilter);

  if(save)
  {
    newname=itemname;
    filepath=makeitemname(".are",0);
    goto gotname;
  }
restart:
  //if (filepath.GetLength()) strncpy(m_getfiledlg.m_ofn.lpstrFile,filepath, filepath.GetLength()+1);
  if( m_getfiledlg.DoModal() == IDOK )
  {
    filepath=m_getfiledlg.GetPathName();
    filepath.MakeLower();
    if(filepath.Right(4)!=".are")
    {
      filepath+=".are";
    }
    newname=m_getfiledlg.GetFileName();
    newname.MakeUpper();
    if(newname.Right(4)==".ARE") newname=newname.Left(newname.GetLength()-4);
gotname:
    if(newname.GetLength()!=6 || newname.Find(" ",0)!=-1)
    {
      tmpstr.Format("The resource name '%s' is bad, it should be exactly 6 characters long and without spaces.",newname);
      MessageBox(tmpstr,"Warning",MB_ICONEXCLAMATION|MB_OK);
      goto restart;
    }
    RetrieveResref(tmpstr,the_area.header.wed);
    if(tmpstr=="NEW AREA")
    {
      StoreResref(newname, the_area.header.wed);
      tmpstr=newname;
    }
    if(newname!=tmpstr)
    {
      res=MessageBox("The wed name isn't "+newname+"\nContinue anyway?","Warning",MB_ICONQUESTION|MB_YESNO);
      if(res==IDNO) goto restart;
    }

    if(newname!=itemname && file_exists(filepath) )
    {
      res=MessageBox("Do you want to overwrite "+newname+"?","Warning",MB_ICONQUESTION|MB_YESNO);
      if(res==IDNO) goto restart;
    }

    res = write_area(newname, filepath);
    if(res)
    {
      goto endofquest;
    }
    lastopenedoverride=filepath.Left(filepath.ReverseFind('\\'));

    if(the_area.WedChanged() && the_area.WedAvailable()) //writing the wed file out
    {
      RetrieveResref(tmpstr,the_area.header.wed);
      if(tmpstr.GetLength()!=6 || tmpstr.Find(" ",0)!=-1 )
      {
        MessageBox("The WED name must be 6 characters long, preferably the same as the area name.","Warning",MB_OK);
        res = -2;
        goto endofquest;
      }
      if(the_area.m_night) tmpstr+="N";

      filepath = filepath.Left(filepath.ReverseFind('\\')+1)+tmpstr+".WED";
      res = write_wed(tmpstr, filepath);
    }
    if(!res && the_mos.MosChanged())
    {
      RetrieveResref(tmpstr,the_area.overlayheaders[0].tis);
      filepath = filepath.Left(filepath.ReverseFind('\\')+1)+tmpstr+".TIS";
      res = write_tis(tmpstr, filepath);
    }
endofquest:
    switch(res)
    {
    case 0:
      itemname=newname;
      break; //saved successfully
    case -2:
      MessageBox("Error while writing file!","Error",MB_ICONSTOP|MB_OK);
      break;
    case -1:
      MessageBox("Internal Error!","Error",MB_ICONSTOP|MB_OK);
      break;
    default:
      MessageBox("Unhandled error!","Error",MB_ICONSTOP|MB_OK);
    }
  }
  SetWindowText("Edit area: "+itemname);
  m_pModelessPropSheet->RefreshDialog();
  UpdateData(UD_DISPLAY);
}

void CAreaEdit::OnFileTbg()
{
  ExportTBG(this, REF_ARE, 0);
}

void CAreaEdit::OnFileTp2()
{
  ExportTBG(this, REF_ARE, 1);
}

void CAreaEdit::OnCheck()
{
  int ret;

  //don't check wed swapping
	ret=((CChitemDlg *) AfxGetMainWnd())->check_area(0);
  if(ret)
  {
    MessageBox(lasterrormsg,"Area editor",MB_ICONEXCLAMATION|MB_OK);
  }
}

void CAreaEdit::OnNew()
{
	NewArea();
  SetWindowText("Edit area: "+itemname);
  m_pModelessPropSheet->RefreshDialog();
  UpdateData(UD_DISPLAY);
}

BOOL CAreaEdit::OnInitDialog()
{
  CString tmpstr, tmpstr1, tmpstr2;

	CDialog::OnInitDialog();
  read_bam("CURSORS",&the_bam); //pre-reading the cursors bam
  SetWindowText("Edit area: "+itemname);
	if (m_pModelessPropSheet == NULL)
	{
		m_pModelessPropSheet = new CAreaPropertySheet(this);
    if(!m_pModelessPropSheet) return TRUE; //not enough memory

		if (!m_pModelessPropSheet->Create(this,
    DS_CONTROL | WS_CHILD | WS_VISIBLE ))
		{
			delete m_pModelessPropSheet;
			m_pModelessPropSheet = NULL;
			return TRUE;
		}
		m_pModelessPropSheet->SetWindowPos(0,0,0,0,0,SWP_NOSIZE);
	}

	if (m_pModelessPropSheet != NULL && !m_pModelessPropSheet->IsWindowVisible())
		m_pModelessPropSheet->ShowWindow(SW_SHOW);
  //tooltips
  {
    m_tooltip.Create(this,TTS_NOPREFIX);
    m_tooltip.SetMaxTipWidth(200);
    m_tooltip.SetTipBkColor(RGB(240,224,160));

    m_tooltip.AddTool(GetDlgItem(IDCANCEL), IDS_CANCEL);
    tmpstr1.LoadString(IDS_LOAD);
    tmpstr2.LoadString(IDS_AREA);
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
	return TRUE;
}

void CAreaEdit::OnToolsMirrorareavertically()
{
  POSITION pos;
  wed_tilemap alt;
  DWORD nFrameWanted;
  int i,y;
  int minx, maxx, maxy;

  the_area.wedchanged=true;
	for(i=0;i<the_area.actorcount;i++)
  {
    the_area.actorheaders[i].destx=(short) (the_area.m_width-the_area.actorheaders[i].destx);
    the_area.actorheaders[i].posx =(short) (the_area.m_width-the_area.actorheaders[i].posx);
    the_area.actorheaders[i].face =(short) ((16-the_area.actorheaders[i].face)&15);
  }
  for(i=0;i<the_area.ambientcount;i++)
  {
    the_area.ambientheaders[i].posx=(short) (the_area.m_width-the_area.ambientheaders[i].posx);
  }
  for(i=0;i<the_area.animcount;i++)
  {
    the_area.animheaders[i].posx=(short) (the_area.m_width-the_area.animheaders[i].posx);
    the_area.animheaders[i].flags^=AA_MIRROR;
  }
  for(i=0;i<the_area.triggercount;i++)
  {
    the_area.triggerheaders[i].launchx=(short) (the_area.m_width-the_area.triggerheaders[i].launchx);
    the_area.triggerheaders[i].launchy=(short) (the_area.m_width-the_area.triggerheaders[i].launchy);
    minx=the_area.m_width-the_area.triggerheaders[i].p2x;
    maxx=the_area.m_width-the_area.triggerheaders[i].p1x;
    the_area.triggerheaders[i].p1x=(short) minx;
    the_area.triggerheaders[i].p2x=(short) maxx;
    the_area.triggerheaders[i].pointx=(short) (the_area.m_width-the_area.triggerheaders[i].pointx);
    the_area.FlipVertex(i,the_area.triggerheaders[i].vertexcount,the_area.m_width);
  }
  for(i=0;i<the_area.containercount;i++)
  {
    the_area.containerheaders[i].launchx=(short) (the_area.m_width-the_area.containerheaders[i].launchx);
    the_area.containerheaders[i].posx=(short) (the_area.m_width-the_area.containerheaders[i].posx);
    minx=the_area.m_width-the_area.containerheaders[i].p2x;
    maxx=the_area.m_width-the_area.containerheaders[i].p1x;
    the_area.containerheaders[i].p1x=(short) minx;
    the_area.containerheaders[i].p2x=(short) maxx;
    the_area.FlipVertex(i+the_area.triggercount,the_area.containerheaders[i].vertexcount,the_area.m_width);
  }
  for(i=0;i<the_area.doorcount;i++)
  {
    the_area.doorheaders[i].launchx=(short) (the_area.m_width-the_area.doorheaders[i].launchx);
    minx=the_area.m_width-the_area.doorheaders[i].locp2x;
    maxx=the_area.m_width-the_area.doorheaders[i].locp1x;
    the_area.doorheaders[i].locp1x=(short) minx;
    the_area.doorheaders[i].locp2x=(short) maxx;
    minx=the_area.m_width-the_area.doorheaders[i].cp2x;
    maxx=the_area.m_width-the_area.doorheaders[i].cp1x;
    the_area.doorheaders[i].cp1x=(short) minx;
    the_area.doorheaders[i].cp2x=(short) maxx;
    minx=the_area.m_width-the_area.doorheaders[i].op2x;
    maxx=the_area.m_width-the_area.doorheaders[i].op1x;
    the_area.doorheaders[i].op1x=(short) minx;
    the_area.doorheaders[i].op2x=(short) maxx;
    the_area.FlipVertex(i*4+the_area.containercount+the_area.triggercount,
      the_area.doorheaders[i].countvertexopen,the_area.m_width);
    the_area.FlipVertex(i*4+1+the_area.containercount+the_area.triggercount,
      the_area.doorheaders[i].countvertexclose,the_area.m_width);
    the_area.FlipVertex(i*4+2+the_area.containercount+the_area.triggercount,
      the_area.doorheaders[i].countblockopen,the_area.m_width/GR_WIDTH);
    the_area.FlipVertex(i*4+3+the_area.containercount+the_area.triggercount,
      the_area.doorheaders[i].countblockclose,the_area.m_width/GR_WIDTH);
  }
  for(i=0;i<the_area.entrancecount;i++)
  {
    the_area.entranceheaders[i].px=(short) (the_area.m_width-the_area.entranceheaders[i].px);
    the_area.entranceheaders[i].face=(short) ((16-the_area.entranceheaders[i].face)&15);
  }
  for(i=0;i<the_area.mapnotecount;i++)
  {
    the_area.mapnoteheaders[i].px=(short) (the_area.m_width-the_area.mapnoteheaders[i].px);
  }
  for(i=0;i<the_area.spawncount;i++)
  {
    the_area.spawnheaders[i].px=(short) (the_area.m_width-the_area.spawnheaders[i].px);
  }
  for(i=0;i<the_area.wallpolygoncount;i++)
  {
    the_area.FlipWedVertex(i, the_area.wallpolygonheaders[i].countvertex, the_area.m_width);
  }
  for(i=0;i<the_area.doorpolygoncount;i++)
  {
    the_area.FlipWedVertex(i+the_area.secheader.wallpolycnt, the_area.doorpolygonheaders[i].countvertex, the_area.m_width);
  }

  //make sure the tis is loaded
  if(SetupSelectPoint(0)) return;
  for(nFrameWanted=0;nFrameWanted<the_mos.tisheader.numtiles;nFrameWanted++)
  {
    the_mos.FlipTile(nFrameWanted);
  }

  the_area.MirrorMap(the_area.lightmap);
  the_area.MirrorMap(the_area.heightmap);
  the_area.MirrorMap(the_area.searchmap);
  maxx=the_area.m_width/64;
  maxy=the_area.m_height/64;
  for(y=0;y<maxy;y++)
  {
    for(i=0;i<maxx/2;i++)
    {
      alt=the_area.overlaytileheaders[y*maxx+i];
      the_area.overlaytileheaders[y*maxx+i]=the_area.overlaytileheaders[(y+1)*maxx-i-1];
      the_area.overlaytileheaders[(y+1)*maxx-i-1]=alt;
    }
  }
  pos=the_area.doortilelist.GetHeadPosition();
  for(i=0;i<the_area.weddoorcount;i++)
  {
    the_area.FlipDoorTile(i, the_area.weddoorheaders[i].countdoortileidx, maxx);
  }

  the_area.RecalcBoundingBoxes();
  for(i=0;i<3;i++)
  {
    the_area.changedmap[i]=true;
  }
  the_area.wedchanged=true;
  the_mos.m_changed=true;
  UpdateData(UD_DISPLAY);
}

void CAreaEdit::OnToolsCreateminimap()
{
  CString tmpstr;

  RetrieveResref(tmpstr, the_area.header.wed);
  read_tis(tmpstr,&the_mos,true); //this is loaded into the common mos file
  the_mos.mosheader.wColumn=the_area.overlayheaders[0].width;
  the_mos.mosheader.wRow=the_area.overlayheaders[0].height;
  SetupSelectPoint(0);
  CreateMinimap(GetSafeHwnd());
  the_mos.m_changed=true;
  UpdateData(UD_DISPLAY);
}


void CAreaEdit::OnRepairwed()
{
	CWedEdit dlg;

  if(GetWed(false)==-99) return; //make sure we get the day wed
  the_area.wedavailable=true; //make sure we have access to it
  dlg.m_repair=true;
  dlg.DoModal();
  m_pModelessPropSheet->RefreshDialog();
  UpdateData(UD_DISPLAY);
}

void CAreaEdit::OnRepairwed2()
{
	CWedEdit dlg;

  if(!(the_area.header.areatype&EXTENDED_NIGHT))
  {
    the_area.header.areatype|=EXTENDED_NIGHT;
  }

  if(GetWed(true)==-99) return; //make sure we get the night wed
  the_area.wedavailable=true;
  dlg.m_repair=true;
  dlg.DoModal();
  m_pModelessPropSheet->RefreshDialog();
  UpdateData(UD_DISPLAY);
}

void CAreaEdit::OnToolsLookupstrref()
{
	CStrRefDlg dlg;

  dlg.DoModal();
  m_pModelessPropSheet->RefreshDialog();
  UpdateData(UD_DISPLAY);
}

void CAreaEdit::OnToolsConvertnight()
{
  CString tmpstr;

  if(GetWed(false)==-99) return;
  if(SetupSelectPoint(0)) return; //make sure tileset is loaded
  RetrieveResref(tmpstr, the_area.overlayheaders[0].tis);
  if (tmpstr.Right(1)!="N" && tmpstr.GetLength()<8)
  {
    tmpstr+="N";
  }
  else return;
  StoreResref(tmpstr, the_area.overlayheaders[0].tis);
  the_area.header.areatype|=EXTENDED_NIGHT;
	the_mos.ApplyPaletteRGB(200,100,100,256);
  the_mos.m_name=tmpstr+".tis";
  the_area.m_night=true;
  the_area.wedchanged=true;
  the_area.wedavailable=true;
  m_pModelessPropSheet->RefreshDialog();
  UpdateData(UD_DISPLAY);
}

void CAreaEdit::PostNcDestroy()
{
  if (m_pModelessPropSheet)
  {
    delete m_pModelessPropSheet;
    m_pModelessPropSheet=NULL;
  }
	CDialog::PostNcDestroy();
}

void CAreaEdit::OnCancel()
{
  CString tmpstr;

	if(the_area.wedchanged && (itemname!="new area"))
  {
    tmpstr.Format("Changes have been made to the wed, tileset\n"
      "and/or an auxiliary map (%s).\n"
      "Do you want to quit without save?\n",itemname);
    if(MessageBox(tmpstr,"Warning",MB_YESNO)==IDNO)
    {
      return;
    }
    goto endofquest;
  }
  if(the_area.m_changed)
  {
    tmpstr.Format("Changes have been made to the area.\n"
      "Do you want to quit without save?\n");
    if(MessageBox(tmpstr,"Warning",MB_YESNO)==IDNO)
    {
      return;
    }
    goto endofquest;
  }

  if(the_area.changedmap[0] ||the_area.changedmap[1] ||the_area.changedmap[2] ) {
    tmpstr.Format("Changes have been made to one or more auxiliary maps.\n"
      "Do you want to quit without save?\n");
    if(MessageBox(tmpstr,"Warning",MB_YESNO)==IDNO)
    {
      return;
    }
    goto endofquest;
  }

endofquest:
	CDialog::OnCancel();
}

BOOL CAreaEdit::PreTranslateMessage(MSG* pMsg)
{
  m_tooltip.RelayEvent(pMsg);
  return CDialog::PreTranslateMessage(pMsg);
}

