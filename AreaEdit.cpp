// AreaEdit.cpp : implementation file
//

#include "stdafx.h"

#include <fcntl.h>
#include <sys/stat.h>
#include "chitem.h"
#include "chitemDlg.h"
#include "AreaEdit.h"
#include "tbg.h"

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
	ON_COMMAND(ID_CHECK, OnCheck)
	ON_COMMAND(ID_FILE_NEW, OnNew)
	ON_COMMAND(ID_FILE_LOAD, OnLoad)
	ON_COMMAND(ID_FILE_LOADEXTERNALSCRIPT, OnLoadex)
	ON_COMMAND(ID_FILE_SAVEAS, OnSaveas)
	ON_COMMAND(ID_TOOLS_MIRRORAREAVERTICALLY, OnToolsMirrorareavertically)
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
		res=read_area(pickerdlg.m_picked);
    switch(res)
    {
    case -3:
      MessageBox("Not enough memory.","Error",MB_ICONEXCLAMATION|MB_OK);
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
    case 0:case 1:
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
  CString wed;
  CString filepath;
  int fhandle;
  int res;
  loc_entry wedfileloc;
  
  res=OFN_FILEMUSTEXIST|OFN_ENABLESIZING|OFN_EXPLORER;
  if(readonly) res|=OFN_READONLY;  
  CFileDialog m_getfiledlg(TRUE, "are", makeitemname(".are",0), res, szFilter);

restart:  
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
    res=the_area.ReadAreaFromFile(fhandle,-1);
    close(fhandle);
    if(res>=0)
    {
      RetrieveResref(wed,the_area.header.wed);
      if(weds.Lookup(wed,wedfileloc))
      {
        fhandle=locate_file(wedfileloc, 0);
        if(fhandle<1)
        {        
          res=3; //wedfile not available
        }
        else
        {
          switch(the_area.ReadWedFromFile(fhandle, wedfileloc.size) )
          {
          case 0: case 1: case 2: break;
          default:
            res=3;
          }
          close(fhandle);
        }
      }
      else res=3;
    }
    switch(res)
    {
    case -3:
      MessageBox("Not enough memory.","Error",MB_ICONEXCLAMATION|MB_OK);
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
  CString filepath;
  CString newname;
  CString tmpstr;
  int fhandle;
  int res;

  if(readonly)
  {
    MessageBox("You opened it read only!","Warning",MB_ICONEXCLAMATION|MB_OK);
    return;
  }
  res=OFN_HIDEREADONLY|OFN_ENABLESIZING|OFN_EXPLORER;

  CFileDialog m_getfiledlg(FALSE, "are", makeitemname(".are",0), res, szFilter);

  if(save)
  {
    newname=itemname;
    filepath=makeitemname(".are",0);
    goto gotname;
  }
restart:  
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
    fhandle=open(filepath, O_BINARY|O_RDWR|O_CREAT|O_TRUNC,S_IREAD|S_IWRITE);
    if(fhandle<1)
    {
      MessageBox("Can't write file!","Error",MB_ICONSTOP|MB_OK);
      goto restart;
    }
    res=the_area.WriteAreaToFile(fhandle, 0);
    close(fhandle);
    if(!res && the_area.WedChanged() && the_area.WedAvailable()) //writing the wed file out
    {
      filepath=bgfolder+"override\\"+the_area.header.wed+".wed";
      fhandle=open(filepath, O_BINARY|O_RDWR|O_CREAT|O_TRUNC,S_IREAD|S_IWRITE);
      if(fhandle<1)
      {
        MessageBox("Can't write wed!","Error",MB_ICONSTOP|MB_OK);
      }
      else
      {
        res=the_area.WriteWedToFile(fhandle, false);
        close(fhandle);
      }
      //night wed
      if(the_area.header.areaflags&EXTENDED_NIGHT)
      {
        filepath=bgfolder+"override\\"+the_area.header.wed+"N.wed";
        fhandle=open(filepath, O_BINARY|O_RDWR|O_CREAT|O_TRUNC,S_IREAD|S_IWRITE);
        if(fhandle<1)
        {
          MessageBox("Can't write wed!","Error",MB_ICONSTOP|MB_OK);
        }
        else
        {
          res=the_area.WriteWedToFile(fhandle, true); //night wed
          close(fhandle);
        }
      }
    }
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
 ExportTBG(this, REF_ARE);
}

void CAreaEdit::OnCheck() 
{
  int ret;

	ret=((CChitemDlg *) AfxGetMainWnd())->check_area();
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
  SetWindowText("Edit area: "+itemname);
	if (m_pModelessPropSheet == NULL)
	{
		m_pModelessPropSheet = new CAreaPropertySheet(this);
    if(!m_pModelessPropSheet) return TRUE; //not enough memory

    //WOW DS_CONTROL
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

void CAreaEdit::PostNcDestroy() 
{
  if (m_pModelessPropSheet)
  {
    delete m_pModelessPropSheet;
    m_pModelessPropSheet=NULL;
  }	
	CDialog::PostNcDestroy();
}

BOOL CAreaEdit::PreTranslateMessage(MSG* pMsg)
{
  m_tooltip.RelayEvent(pMsg);
  return CDialog::PreTranslateMessage(pMsg);
}

void CAreaEdit::OnToolsMirrorareavertically() 
{
  int i;
  int width;
  int minx, maxx;

  the_area.wedchanged=true;
  width=the_area.overlayheaders[0].width*64;
	for(i=0;i<the_area.actorcount;i++)
  {
    the_area.actorheaders[i].destx=(short) (width-the_area.actorheaders[i].destx);
    the_area.actorheaders[i].posx=(short) (width-the_area.actorheaders[i].posx);
  }
  for(i=0;i<the_area.ambientcount;i++)
  {
    the_area.ambientheaders[i].posx=(short) (width-the_area.ambientheaders[i].posx);
  }
  for(i=0;i<the_area.animcount;i++)
  {
    the_area.animheaders[i].posx=(short) (width-the_area.animheaders[i].posx);
    the_area.animheaders[i].flags^=AA_MIRROR;
  }
  for(i=0;i<the_area.containercount;i++)
  {
    the_area.containerheaders[i].launchx=(short) (width-the_area.containerheaders[i].launchx);
    the_area.containerheaders[i].posx=(short) (width-the_area.containerheaders[i].posx);
    minx=width-the_area.containerheaders[i].p2x;
    maxx=width-the_area.containerheaders[i].p1x;
    the_area.containerheaders[i].p1x=(short) minx;
    the_area.containerheaders[i].p2x=(short) maxx;
    the_area.FlipVertex(i,the_area.containerheaders[i].vertexcount,width);
  }
  for(i=0;i<the_area.doorcount;i++)
  {
    the_area.doorheaders[i].launchx=(short) (width-the_area.doorheaders[i].launchx);
    minx=width-the_area.doorheaders[i].locp2x;
    maxx=width-the_area.doorheaders[i].locp1x;
    the_area.doorheaders[i].locp1x=(short) minx;
    the_area.doorheaders[i].locp2x=(short) maxx;
    minx=width-the_area.doorheaders[i].cp2x;
    maxx=width-the_area.doorheaders[i].cp1x;
    the_area.doorheaders[i].cp1x=(short) minx;
    the_area.doorheaders[i].cp2x=(short) maxx;
    minx=width-the_area.doorheaders[i].op2x;
    maxx=width-the_area.doorheaders[i].op1x;
    the_area.doorheaders[i].op1x=(short) minx;
    the_area.doorheaders[i].op2x=(short) maxx;
    the_area.FlipVertex(i*4+the_area.containercount+the_area.triggercount,
      the_area.triggerheaders[i].vertexcount,width);
    the_area.FlipVertex(i*4+1+the_area.containercount+the_area.triggercount,
      the_area.triggerheaders[i].vertexcount,width);
  }
  for(i=0;i<the_area.entrancecount;i++)
  {
    the_area.entranceheaders[i].px=(short) (width-the_area.entranceheaders[i].px);
    the_area.entranceheaders[i].face=(short) ((16-the_area.entranceheaders[i].face)&15);
  }
  for(i=0;i<the_area.mapnotecount;i++)
  {
    the_area.mapnoteheaders[i].px=(short) (width-the_area.mapnoteheaders[i].px);
  }
  for(i=0;i<the_area.spawncount;i++)
  {
    the_area.spawnheaders[i].px=(short) (width-the_area.spawnheaders[i].px);
  }
  for(i=0;i<the_area.triggercount;i++)
  {
    the_area.triggerheaders[i].launchx=(short) (width-the_area.triggerheaders[i].launchx);
    the_area.triggerheaders[i].launchy=(short) (width-the_area.triggerheaders[i].launchy);
    minx=width-the_area.triggerheaders[i].p2x;
    maxx=width-the_area.triggerheaders[i].p1x;
    the_area.triggerheaders[i].p1x=(short) minx;
    the_area.triggerheaders[i].p2x=(short) maxx;
    //this is rather a point not a box
    if(the_area.triggerheaders[i].infoflags&1024)
    {
      the_area.triggerheaders[i].ovrp1x=(short) (width-the_area.triggerheaders[i].ovrp1x);
    }
    the_area.FlipVertex(i+the_area.containercount,the_area.triggerheaders[i].vertexcount,width);
  }
  for(i=0;i<the_area.wedvertexcount;i++)
  {
    the_area.wedvertices[i].point.x=(short) (width-the_area.wedvertices[i].point.x);
  }
  UpdateData(UD_DISPLAY);
}
