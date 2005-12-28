// DialogEdit.cpp : implementation file
//

#include "stdafx.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <direct.h>
#include "chitem.h"
#include "chitemDlg.h"
#include "DialogEdit.h"
#include "DialogLink.h"
#include "options.h"
#include "compat.h"
#include "WeiduLog.h"
#include "tbg.h"
#include "2da.h"
#include "2daEdit.h"
#include ".\dialogedit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define FL_TEXT 1
#define FL_OPTION 2
#define FL_CONDITION 4
#define FL_ACTION 8
#define FL_JOURNAL 16

#define STATIC_FLAGS (LEAF_NODE|HAS_TRIGGER|HAS_ACTION|HAS_TEXT)

#define FILEMENU  0
#define TREEMENU  1
/////////////////////////////////////////////////////////////////////////////
// CDialogEdit dialog

CDialogEdit::CDialogEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogEdit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogEdit)
	m_label = _T("");
	m_text = _T("");
	m_strref = 0;
	m_sound = _T("");
	m_dialog = _T("");
	m_value = 0;
	//}}AFX_DATA_INIT
}

void CDialogEdit::NewDialog()
{
	itemname="new dialog";
  the_dialog.new_dialog();
  memset(&the_dialog.header,0,sizeof(dlg_header));
  the_dialog.header.offstates=sizeof(dlg_header);
  the_dialog.changed=0;
}

int add_action()
{
  CString *newactions;
  int acindex;
  int i;
  
  acindex=the_dialog.actioncount++;
  newactions = new CString [the_dialog.actioncount];
  if(!newactions)
  {
    the_dialog.actioncount--;
    return -1;
  }
  for(i=0;i<the_dialog.header.numaction;i++)
  {
    newactions[i]=the_dialog.actions[i];
  }
  delete [] the_dialog.actions;
  the_dialog.actions=newactions;
  the_dialog.header.numaction=the_dialog.actioncount;
  return acindex;
}

int add_transition_trigger()
{
  CString *newtriggers;
  int trindex;
  int i;
  
  trindex=the_dialog.trtriggercount++;
  newtriggers = new CString [the_dialog.trtriggercount];
  if(!newtriggers)
  {
    the_dialog.trtriggercount--;
    return -1;
  }
  for(i=0;i<the_dialog.header.numtrtrigger;i++)
  {
    newtriggers[i]=the_dialog.trtriggers[i];
  }
  delete [] the_dialog.trtriggers;
  the_dialog.trtriggers=newtriggers;
  the_dialog.header.numtrtrigger=the_dialog.trtriggercount;
  return trindex;
}

int clone_transitions(int from, int count)
{
  int i;
  int old,tmp;
  dlg_trans *newtransitions;

  the_dialog.changed=1;
  newtransitions=new dlg_trans[the_dialog.transcount+count];
  if(!newtransitions) return -3;
  memcpy(newtransitions, the_dialog.dlgtrans,the_dialog.transcount*sizeof(dlg_trans));
  memcpy(newtransitions+the_dialog.transcount, the_dialog.dlgtrans+from,count*sizeof(dlg_trans));
  delete [] the_dialog.dlgtrans;
  the_dialog.dlgtrans=newtransitions;
  from=the_dialog.transcount;
  the_dialog.transcount+=count;
  the_dialog.header.numtrans=the_dialog.transcount;
  for(i=from;i<from+count;i++)
  {
    //cloning actions
    old=the_dialog.dlgtrans[i].actionindex;
    if(old!=-1)
    {
      tmp=add_action();
      the_dialog.dlgtrans[i].actionindex=tmp;
      the_dialog.actions[tmp]=the_dialog.actions[old];
    }

    //cloning triggers
    old=the_dialog.dlgtrans[i].trtrindex;
    if(old!=-1)
    {
      tmp=add_transition_trigger();
      the_dialog.dlgtrans[i].trtrindex=tmp;
      the_dialog.trtriggers[tmp]=the_dialog.trtriggers[old];
    }
  }
  return from;
}

int CDialogEdit::AddTransBranch(HTREEITEM ht, int firsttransition, int transitioncount, int level)
{
  CString external, tmpstr, text;
  int strref;
  int i;
  HTREEITEM ht2;
  int stateidx;
  int transitionidx;
  int from, count;

  if(!transitioncount) return 0;
  if(firsttransition+transitioncount>the_dialog.transcount)
  {
    return -1;
  }
  if(level<0) level++;
  else level=1;

  for(i=0;i<transitioncount;i++)
  {
    transitionidx=firsttransition+i;
    if((the_dialog.dlgtrans[transitionidx].flags&HAS_ACTION) &&
       (the_dialog.dlgtrans[transitionidx].actionindex>=the_dialog.actioncount))
    {
      tmpstr.Format("Removing invalid action from transition %d",transitionidx);
      MessageBox(tmpstr,"Dialog editor",MB_OK);
      the_dialog.dlgtrans[transitionidx].actionindex=0;
      the_dialog.dlgtrans[transitionidx].flags&=~HAS_ACTION;
    }
    if((the_dialog.dlgtrans[transitionidx].flags&HAS_TRIGGER) &&
       (the_dialog.dlgtrans[transitionidx].trtrindex>=the_dialog.trtriggercount))
    {
      tmpstr.Format("Removing invalid trigger from transition %d",transitionidx);
      MessageBox(tmpstr,"Dialog editor",MB_OK);
      the_dialog.dlgtrans[transitionidx].trtrindex=0;
      the_dialog.dlgtrans[transitionidx].flags&=~HAS_TRIGGER;
    }

    ht2=m_transstates.FindElement(transitionidx);
    if(ht2)
    {
      stateidx=-1;
      m_treestates.Lookup(ht,stateidx);
      if(stateidx==-1)
      {
        MessageBox("Internal error!","Dialog editor",MB_OK);
        return -1;
      }
      tmpstr.Format("Mixed something up at state %d! (transition %d was already accessed)\nCloning duplicated transitions.\n",stateidx,transitionidx);
      MessageBox(tmpstr,"Dialog editor",MB_OK);
      firsttransition=clone_transitions(firsttransition, transitioncount);
      if(firsttransition<0) return firsttransition;
      the_dialog.dlgstates[stateidx].trindex=firsttransition;
      break;
    }
  }
  for(i=0;i<transitioncount;i++)
  {
    transitionidx=firsttransition+i;
    strref=the_dialog.dlgtrans[transitionidx].playerstr;
    if(strref || (the_dialog.dlgtrans[transitionidx].flags&HAS_TEXT) )
    {
      text=resolve_tlk_text(strref);
    }
    else
    {
      text="<NO TEXT>";
    }
    stateidx=the_dialog.dlgtrans[transitionidx].stateindex;

    if(the_dialog.dlgtrans[transitionidx].flags&LEAF_NODE)
    {
      if(stateidx || the_dialog.dlgtrans[transitionidx].nextdlg[0])
      {
        tmpstr.Format("Transition node #%d terminates abruptly, clearing link completely.",transitionidx);
        MessageBox(tmpstr,"Dialog editor",MB_OK);
        the_dialog.dlgtrans[transitionidx].stateindex=0;
        memset(the_dialog.dlgtrans[transitionidx].nextdlg,0,8);
      }
      tmpstr.Format("End node#%d: %s",transitionidx,text);
      ht2=m_dialogtree.InsertItem(LVIF_TEXT|LVIF_PARAM,tmpstr,0,0,0,0,TRANSSTATE|transitionidx,ht,TVI_LAST);
      m_transstates[ht2]=transitionidx;
      continue; //a dialog path ends here
    }
    tmpstr.Format("Reply#%d: %s",transitionidx,text);
    ht2=m_dialogtree.InsertItem(LVIF_TEXT|LVIF_PARAM,tmpstr,0,0,0,0,TRANSSTATE|transitionidx,ht,TVI_LAST);
    m_transstates[ht2]=transitionidx;
    RetrieveResref(external, the_dialog.dlgtrans[transitionidx].nextdlg);
//    if(!external.IsEmpty() && external!=itemname)
    if(!external.IsEmpty() && external!=SELF_REFERENCE)
    {
      tmpstr.Format("External link: %s.dlg (block:%d)",external,stateidx);
      ht2=m_dialogtree.InsertItem(LVIF_TEXT|LVIF_PARAM,tmpstr,0,0,0,0,EXTERNAL|stateidx,ht2,TVI_LAST);
      continue; //a dialog path continues in another dialog
    }
    if((unsigned int) stateidx>=(unsigned int) the_dialog.statecount)
    {
      the_dialog.dlgtrans[transitionidx].stateindex=0;
      the_dialog.dlgtrans[transitionidx].flags|=LEAF_NODE;
      memset(the_dialog.dlgtrans[transitionidx].nextdlg,0,8);
      stateidx=0;
      tmpstr.Format("State index for transition %d is out of range, clearing it.",transitionidx);
      MessageBox(tmpstr,"Dialog editor",MB_OK);
      continue;
    }
    strref=the_dialog.dlgstates[stateidx].actorstr;
    text=resolve_tlk_text(strref);
    if(m_treestates.FindElement(stateidx) )
    {
      tmpstr.Format("Internal link:%s (block:%d)",text,stateidx);
      ht2=m_dialogtree.InsertItem(LVIF_TEXT|LVIF_PARAM,tmpstr,0,0,0,0,LINKTO|stateidx, ht2,TVI_LAST);
      continue;
    }
    tmpstr.Format("Text#%d: %s",stateidx,text);
    ht2=m_dialogtree.InsertItem(LVIF_TEXT|LVIF_PARAM,tmpstr,0,0,0,0,TREESTATE|stateidx, ht2,TVI_LAST);
    m_treestates[ht2]=stateidx;
    if(the_dialog.dlgstates[stateidx].stindex>=the_dialog.sttriggercount)
    {
      tmpstr.Format("Removing invalid trigger from state %d",stateidx);
      MessageBox(tmpstr,"Dialog editor",MB_OK);
      the_dialog.dlgstates[stateidx].stindex=-1;
    }
    from=the_dialog.dlgstates[stateidx].trindex;
    count=the_dialog.dlgstates[stateidx].trnumber;
    if(AddTransBranch(ht2,from, count,level))
    {
      tmpstr.Format("Transition indices (%d,%d) are out of range for state %d.",from, count, stateidx);
      MessageBox(tmpstr,"Dialog editor",MB_OK);
      the_dialog.dlgstates[stateidx].trindex=the_dialog.transcount;
      the_dialog.dlgstates[stateidx].trnumber=0;
    }
  }
  return 0;
}

void CDialogEdit::RefreshDialog(int stateidx, int expandselection)
{
  CString tmpstr, text;
  int strref;
  int i,j;
  HTREEITEM ht;
  int from, count;

  if(the_dialog.header.offstates==0x30) //quasi file version
  {
    m_freeze_control.EnableWindow(false);
    m_version_control.SetCheck(false);
  }
  else
  {
    m_freeze_control.EnableWindow(true);
    m_version_control.SetCheck(true);
  }
  SetWindowText("Edit dialog: "+itemname);
  if(stateidx==-1)
  {
    return;
  }
  m_currentselection=m_currentselection_new=0;
  m_dialogtree.DeleteAllItems();
  m_treestates.RemoveAll();
  m_transstates.RemoveAll();
  
  i=0;
  j=0;
  //first we gather top level states (states with triggers)
  for(i=0;i<the_dialog.header.numstates;i++)
  {
    if(the_dialog.dlgstates[i].stindex<0) continue;
    j++;
    strref=the_dialog.dlgstates[i].actorstr;
    text=resolve_tlk_text(strref);
    tmpstr.Format("Top level#%d (Order:%d): %s",i,the_dialog.dlgstates[i].stindex,text);
    ht=m_dialogtree.InsertItem(LVIF_TEXT|LVIF_PARAM, tmpstr, 0, 0, 0, 0, TREESTATE | i, TVI_ROOT,TVI_LAST);
    m_treestates[ht]=i;
  }
  if(j!=the_dialog.header.numsttrigger)
  {
    MessageBox("The number of top level states (states with triggers) was incorrect.","Dialog editor",MB_OK);
  }
  //then we gather states that are accessible from top level states
  for(i=0;i<the_dialog.header.numstates;i++)
  {
    if(the_dialog.dlgstates[i].stindex<0) continue;
    ht=m_treestates.FindElement(i);
    if(ht)
    {
      from=the_dialog.dlgstates[i].trindex;
      count=the_dialog.dlgstates[i].trnumber;
      if(AddTransBranch(ht,from,count,0))
      {
        tmpstr.Format("Transition indices (%d,%d) are out of range for state %d.",from, count, stateidx);
        MessageBox(tmpstr,"Dialog editor",MB_OK);
      }
    }
  }
  //externally referenced states (marked as external as well!)
  for(i=0;i<the_dialog.header.numstates;i++)
  {
    if(m_treestates.FindElement(i)) continue; //already reached state
    if(the_dialog.dlgstates[i].stindex>=0)
    {
      tmpstr.Format("Mixed something up: block (%d)!",i);
      MessageBox(tmpstr,"Dialog editor",MB_OK);
      continue;
    }
    strref=the_dialog.dlgstates[i].actorstr;
    text=resolve_tlk_text(strref);
    tmpstr.Format("Ext. ref.#%d: %s",i,text);
    ht=m_dialogtree.InsertItem(LVIF_TEXT|LVIF_PARAM, tmpstr,0, 0, 0, 0,EXTERNAL | i,TVI_ROOT,TVI_LAST);
    m_treestates[ht]=i;
    from=the_dialog.dlgstates[i].trindex;
    count=the_dialog.dlgstates[i].trnumber;
    if(AddTransBranch(m_treestates.FindElement(i),from, count,-1) )
    {
      tmpstr.Format("Transition indices (%d,%d) are out of range for state %d.",from, count, stateidx);
      MessageBox(tmpstr,"Dialog editor",MB_OK);
    }
  }
  if(the_dialog.header.numstates)
  {
    m_currentselection_new=m_treestates.FindElement(stateidx);
    if(m_currentselection_new)
    {
      m_dialogtree.Select(m_currentselection_new,TVGN_CARET);
      if(expandselection) m_dialogtree.Expand(m_currentselection_new,TVE_EXPAND);
    }
    else
    {
      tmpstr.Format("State %d not found.",stateidx);
      MessageBox(tmpstr,"Dialog editor",MB_OK);
    }
  }
  else
  {
    m_currentselection_new=(HTREEITEM) 0;
    m_activesection=FL_TEXT|FL_OPTION;
  }
  m_currentselection=m_currentselection_new;
  RefreshMenu();
}

BOOL CDialogEdit::OnInitDialog() 
{
  CString tmpstr, tmpstr1, tmpstr2;

  the_ids.new_ids();
  m_idsname="";
	CDialog::OnInitDialog();
  m_activesection=FL_TEXT;
	RefreshDialog(0);
  //tooltips
  {
    m_tooltip.Create(this,TTS_NOPREFIX);
    m_tooltip.SetMaxTipWidth(200);
    m_tooltip.SetTipBkColor(RGB(240,224,160));
    
    m_tooltip.AddTool(GetDlgItem(IDCANCEL), IDS_CANCEL);
    tmpstr1.LoadString(IDS_LOAD);
    tmpstr2.LoadString(IDS_DIALOG);
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

	int i;
	CWnd *cntrl;

	CRect rect;
	for(i=0;i<40;i++)
	{
		cntrl=GetDlgItem(IDC_U1+i);
		cntrl->GetWindowRect(&rect);
		ScreenToClient(&rect);
		cntrl->SetWindowPos(NULL, 11, 314+13*i,0,0,SWP_NOSIZE|SWP_NOZORDER|SWP_HIDEWINDOW);
	}
	locked=false;
  UpdateData(UD_DISPLAY);
  return TRUE; 
}

static int dialogboxids[]={IDC_STRREF, IDC_TAGGED, IDC_BROWSE, IDC_PLAY, IDC_SOUND,
IDC_NEWVALUE,IDC_TEXT, IDC_HASOPTION,IDC_HASCONDITION,IDC_HASACTION, IDC_HASJOURNAL, 
IDC_OPTION, IDC_CONDITION,IDC_ACTION,IDC_JOURNAL, IDC_JOURTYPE,IDC_VALUE,IDC_DIALOG,
0};
static int stringboxids[]={IDC_STRREF, IDC_TAGGED, IDC_BROWSE, IDC_PLAY, IDC_SOUND,
IDC_NEWVALUE,0};

void CDialogEdit::EnableWindows(int enableflags, int actualflags)
{
  int i,j;
  int flag, actflag;
  CButton *cb;
  CString tmpstr;
  int tmpsection;
  int actflags;

  actflags=0;
  j=1;
  for(i=0;i<5;i++)
  {
    flag=!!(enableflags&j);
    switch(j)
    {
    case FL_TEXT:
      cb=(CButton *) GetDlgItem(IDC_HASOPTION);
      cb->EnableWindow(true);
      if(flag)
      {
        cb->SetWindowText("Has text");
        cb->SetCheck(true);
        actflag=!!(actualflags&HAS_TEXT);
      }
      else actflag=0;
      cb=(CButton *) GetDlgItem(IDC_OPTION);
      cb->EnableWindow(true);
      if(flag) cb->SetWindowText("Actor text");
      if(actflag) actflags|=FL_TEXT;
      break;
    case FL_OPTION:
      cb=(CButton *) GetDlgItem(IDC_HASOPTION);
      if(flag)
      {
        cb->SetWindowText("Has reply");
        actflag=!!(actualflags&HAS_TEXT);
        cb->SetCheck(actflag);
      }
      else actflag=0;
      cb=(CButton *) GetDlgItem(IDC_OPTION);
      if(flag) cb->SetWindowText("Option");
      if(actflag) actflags|=FL_OPTION;
      break;
    case FL_CONDITION:
      cb=(CButton *) GetDlgItem(IDC_HASCONDITION);
      cb->EnableWindow(flag);
      if(flag)
      {
        actflag=!!(actualflags&HAS_TRIGGER);
      }
      else actflag=0;
      cb->SetCheck(actflag);
      cb=(CButton *) GetDlgItem(IDC_CONDITION);
      cb->EnableWindow(actflag);
      if(actflag) actflags|=FL_CONDITION;
      break;
    case FL_ACTION:
      cb=(CButton *) GetDlgItem(IDC_HASACTION);
      cb->EnableWindow(flag);
      if(flag)
      {
        actflag=!!(actualflags&HAS_ACTION);
      }
      else actflag=0;
      cb->SetCheck(actflag);
      cb=(CButton *) GetDlgItem(IDC_ACTION);
      cb->EnableWindow(actflag);
      if(actflag) actflags|=FL_ACTION;
      break;
    case FL_JOURNAL:
      cb=(CButton *)GetDlgItem(IDC_HASJOURNAL);
      cb->EnableWindow(flag);
      if(flag)
      {
        actflag=!!(actualflags&j);
        cb->SetCheck(actflag);
      }
      else actflag=0;
      cb=(CButton *)GetDlgItem(IDC_JOURNAL);
      cb->EnableWindow(actflag);
      cb=(CButton *)GetDlgItem(IDC_JOURTYPE);
      cb->EnableWindow(actflag);
      cb=(CButton *)GetDlgItem(IDC_VALUE);
      cb->EnableWindow(flag);
      cb=(CButton *)GetDlgItem(IDC_DIALOG);
      cb->EnableWindow(false);
      if(actflag) actflags|=FL_JOURNAL;
      break;
    }
    j<<=1;
  }
  j=1;
  tmpsection=m_activesection&actflags;
  if(!tmpsection) tmpsection=actflags;
  m_activesection=0;
  for(i=0;i<8;i++)
  {
    if(tmpsection&j)
    {
      m_activesection=j;
      break;
    }
    j<<=1;
  }
  flag=!!(m_activesection&(FL_TEXT|FL_OPTION|FL_JOURNAL));
  for(i=0;;i++)
  {
    j=stringboxids[i];
    if(!j) break;
    cb=(CButton *) GetDlgItem(j);
    cb->EnableWindow(flag);
  }

  cb=(CButton *) GetDlgItem(IDC_TEXT);
  cb->EnableWindow(!!m_activesection);
}

void CDialogEdit::RetrieveText()
{
  int key;
  int value;

  if(m_transstates.Lookup(m_currentselection,key) )
  {
    value=(m_value&~STATIC_FLAGS)|(the_dialog.dlgtrans[key].flags&STATIC_FLAGS);
    the_dialog.dlgtrans[key].flags=value;
  }
}

void CDialogEdit::DisplayText()
{
  int key;
  int flag;
  int tmpidx;
  int i,id;

  if(!m_tagcontrol) return;
  if(m_treestates.Lookup(m_currentselection,key) )
  {
    //what can you see
    tmpidx=the_dialog.dlgstates[key].stindex;
    if(tmpidx>=0) flag=HAS_TEXT|HAS_TRIGGER;
    else flag=HAS_TEXT;
    EnableWindows(FL_TEXT|FL_CONDITION,flag);
    //what do you want to see
    switch(m_activesection)
    {
    case FL_TEXT:
      m_label="Actor text";
      m_strref=the_dialog.dlgstates[key].actorstr; 
      m_text=resolve_tlk_text(m_strref);
      m_sound=resolve_tlk_soundref(m_strref);
      m_tagcontrol.SetCheck(resolve_tlk_tag(m_strref));
      break;
    case FL_CONDITION:
      m_label="Trigger condition";
      if(tmpidx>=the_dialog.sttriggercount)
      {
        the_dialog.dlgstates[key].stindex=-1;
        m_text="<invalid reference>";
        break;
      }
      m_text=the_dialog.sttriggers[tmpidx];
      m_text.Replace("\r","");
      m_text.Replace("\n","\r\n");
      break;
    default:
      m_label="";
      m_text="";
    }
    return;
  }
  if(m_transstates.Lookup(m_currentselection,key) )
  {//what do you want to see
    m_value=the_dialog.dlgtrans[key].flags;
    RetrieveResref(m_dialog,the_dialog.dlgtrans[key].nextdlg);
    EnableWindows(FL_OPTION|FL_JOURNAL|FL_ACTION|FL_CONDITION, the_dialog.dlgtrans[key].flags);
    switch(m_activesection)
    {
    case FL_OPTION:
      m_label="Player text";
      m_strref=the_dialog.dlgtrans[key].playerstr;
      m_text=resolve_tlk_text(m_strref);
      m_sound=resolve_tlk_soundref(m_strref);
      m_tagcontrol.SetCheck(resolve_tlk_tag(m_strref));
      break;
    case FL_CONDITION:
      m_label="Enable condition";
      tmpidx=the_dialog.dlgtrans[key].trtrindex;
      if(tmpidx>=the_dialog.trtriggercount)
      {
        the_dialog.dlgtrans[key].trtrindex=-1;
        m_text="<invalid reference>";
        break;
      }
      m_text=the_dialog.trtriggers[tmpidx];
      m_text.Replace("\r","");
      m_text.Replace("\n","\r\n");
      break;
    case FL_ACTION:
      m_label="Action";
      tmpidx=the_dialog.dlgtrans[key].actionindex;
      if(tmpidx>=the_dialog.actioncount)
      {
        m_text="<invalid reference>";
        the_dialog.dlgtrans[key].actionindex=-1;
        break;
      }
      m_text=the_dialog.actions[tmpidx];
      m_text.Replace("\r","");
      m_text.Replace("\n","\r\n");
      break;
    case FL_JOURNAL:
      m_label="Journal entry";
      m_strref=the_dialog.dlgtrans[key].journalstr;
      m_text=resolve_tlk_text(m_strref);
      m_sound=resolve_tlk_soundref(m_strref);
      m_tagcontrol.SetCheck(resolve_tlk_tag(m_strref));
      break;
    default:
      m_text="";
      m_label="";
    }
    return;
  }
  //this is not modifiable leaf
  for(i=0;;i++)
  {
    id=dialogboxids[i];
    if(!id) break;
    GetDlgItem(id)->EnableWindow(false);
  }
}

void CDialogEdit::DoDataExchange(CDataExchange* pDX)
{
  unsigned int tmp;

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogEdit)
	DDX_Control(pDX, IDC_TEXT, m_text_control);
	DDX_Control(pDX, IDC_VERSION, m_version_control);
	DDX_Control(pDX, IDC_FLAGS, m_freeze_control);
	DDX_Control(pDX, IDC_JOURNAL, m_journal_control);
	DDX_Control(pDX, IDC_CONDITION, m_condition_control);
	DDX_Control(pDX, IDC_ACTION, m_action_control);
	DDX_Control(pDX, IDC_HASJOURNAL, m_hasjournal);
	DDX_Control(pDX, IDC_HASCONDITION, m_hascondition);
	DDX_Control(pDX, IDC_HASACTION, m_hasaction);
	DDX_Control(pDX, IDC_HASOPTION, m_hasoption);
	DDX_Control(pDX, IDC_JOURTYPE, m_jourtype_control);
	DDX_Control(pDX, IDC_TAGGED, m_tagcontrol);
	DDX_Control(pDX, IDC_DIALOGTREE, m_dialogtree);
	DDX_Text(pDX, IDC_LABEL, m_label);
	DDX_Text(pDX, IDC_TEXT, m_text);
	DDX_Text(pDX, IDC_STRREF, m_strref);
	DDX_Text(pDX, IDC_SOUND, m_sound);
	DDV_MaxChars(pDX, m_sound, 8);
	DDX_Text(pDX, IDC_DIALOG, m_dialog);
	DDV_MaxChars(pDX, m_dialog, 8);
	DDX_Text(pDX, IDC_VALUE, m_value);
	//}}AFX_DATA_MAP
  CheckTriggerOrAction(false);
      //not all flags have been discerned yet
  switch(m_value&(HAS_QUEST|HAS_SOLVED) )
  {
  case 0: m_jourtype_control.SetCurSel(0); break;
  case HAS_QUEST:m_jourtype_control.SetCurSel(1); break;
  case HAS_SOLVED:m_jourtype_control.SetCurSel(2); break;
    //this will be updated as soon as i find more flags
  default:m_jourtype_control.SetCurSel(3); break;
  }
  tmp=the_dialog.header.flags;
  DDX_Text(pDX, IDC_FLAGS, the_dialog.header.flags);
  if(tmp!=the_dialog.header.flags)
  {
    the_dialog.changed=1;
  }
  if(pDX->m_bSaveAndValidate==UD_RETRIEVE)
  {
    RetrieveText();
  }
}

BEGIN_MESSAGE_MAP(CDialogEdit, CDialog)
	//{{AFX_MSG_MAP(CDialogEdit)
	ON_NOTIFY(TVN_SELCHANGED, IDC_DIALOGTREE, OnSelchangedDialogtree)
	ON_BN_CLICKED(IDC_TAGGED, OnTagged)
	ON_CBN_KILLFOCUS(IDC_JOURTYPE, OnKillfocusJourtype)
	ON_BN_CLICKED(IDC_HASOPTION, OnHasoption)
	ON_BN_CLICKED(IDC_HASCONDITION, OnHascondition)
	ON_BN_CLICKED(IDC_HASACTION, OnHasaction)
	ON_BN_CLICKED(IDC_HASJOURNAL, OnHasjournal)
	ON_EN_KILLFOCUS(IDC_FLAGS, OnKillfocusFlags)
	ON_BN_CLICKED(IDC_OPTION, OnOption)
	ON_BN_CLICKED(IDC_CONDITION, OnCondition)
	ON_BN_CLICKED(IDC_ACTION, OnAction)
	ON_BN_CLICKED(IDC_JOURNAL, OnJournal)
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_BN_CLICKED(IDC_LOADEX, OnLoadex)
	ON_BN_CLICKED(IDC_SAVEAS, OnSaveas)
	ON_BN_CLICKED(IDC_VERSION, OnVersion)
	ON_EN_KILLFOCUS(IDC_TEXT, OnKillfocusText)
	ON_EN_KILLFOCUS(IDC_STRREF, OnKillfocusStrref)
	ON_EN_KILLFOCUS(IDC_VALUE, OnKillfocusValue)
	ON_EN_VSCROLL(IDC_TEXT, OnVscrollText)
	ON_EN_KILLFOCUS(IDC_SOUND, OnKillfocusSound)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_PLAY, OnPlay)
	ON_EN_KILLFOCUS(IDC_DIALOG, OnKillfocusDialog)
	ON_EN_SETFOCUS(IDC_TEXT, OnSetfocusText)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_FOLLOWLINK, OnFollowlink)
	ON_NOTIFY(NM_DBLCLK, IDC_DIALOGTREE, OnDblclkDialogtree)
	ON_NOTIFY(TVN_GETINFOTIP, IDC_DIALOGTREE, OnGetinfotipDialogtree)
	ON_COMMAND(ID_REFRESH, OnRefresh)
	ON_COMMAND(ID_EXPAND, OnExpand)
	ON_COMMAND(ID_COLLAPSE, OnCollapse)
	ON_COMMAND(ID_SEARCHNODE, OnSearchnode)
	ON_COMMAND(ID_REMOVE, OnRemove)
	ON_COMMAND(ID_ADDNODE, OnAddnode)
	ON_COMMAND(ID_CONVERT, OnConvert)
	ON_BN_CLICKED(IDC_NEW, OnNew)
	ON_COMMAND(ID_SWAPNODES, OnSwapnodes)
	ON_COMMAND(ID_INCREASEWEIGHT, OnIncreaseweight)
	ON_COMMAND(ID_TOOLSORDER, OnToolsorder)
	ON_COMMAND(ID_TOOLSORDER2, OnToolsorder2)
	ON_COMMAND(ID_ADDTOP, OnAddtop)
	ON_COMMAND(ID_EXPORT_WEIDU, OnExportWeidu)
	ON_COMMAND(ID_IMPORT_WEIDU, OnImportWeidu)
	ON_COMMAND(ID_EXPORT_TBG, OnExportTbg)
	ON_COMMAND(ID_FILE_SAVE, OnSave)
	ON_BN_CLICKED(IDC_CHECK, OnCheck)
	ON_COMMAND(ID_TOOLSPST, OnToolspst)
	ON_BN_CLICKED(IDC_NEWVALUE, OnNewvalue)
	ON_COMMAND(ID_FILE_LOAD, OnLoad)
	ON_COMMAND(ID_FILE_SAVEAS, OnSaveas)
	ON_COMMAND(ID_FILE_NEW, OnNew)
	ON_COMMAND(ID_TOOLS_IDSBROWSER, OnToolsIdsbrowser)
	//}}AFX_MSG_MAP
//	ON_WM_SIZE()
//ON_WM_SIZING()
ON_WM_SIZE()
//ON_WM_SIZING()
ON_WM_GETMINMAXINFO()
ON_EN_CHANGE(IDC_TEXT, OnEnChangeText)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogEdit message handlers

void CDialogEdit::OnLoad() 
{
	int res;

  pickerdlg.m_restype=REF_DLG;
  pickerdlg.m_picked=itemname;
	res=pickerdlg.DoModal();
	if(res==IDOK)
	{
		res=read_dialog(pickerdlg.m_picked);
    switch(res)
    {
    case -1:
      MessageBox("Dialog loaded with errors.","Warning",MB_ICONEXCLAMATION|MB_OK);
      break;
    case 1:
      if(old_version_dlg())
      {
        MessageBox("Old dialog version (freeze flags were unavailable)","Warning",MB_ICONEXCLAMATION|MB_OK);     
      }
    case 0:
      break;
    default:
      MessageBox("Cannot read dialog!","Error",MB_ICONEXCLAMATION|MB_OK);
      NewDialog();
      break;
    }
	}
  m_activesection=FL_TEXT;
  RefreshDialog(0);
  UpdateData(UD_DISPLAY);
}

static char BASED_CODE szFilter[] = "Dialog files (*.dlg)|*.dlg|All files (*.*)|*.*||";

void CDialogEdit::OnLoadex() 
{
  CString filepath;
  int fhandle;
  int res;
  
  res=OFN_FILEMUSTEXIST|OFN_ENABLESIZING|OFN_EXPLORER;
  if(readonly) res|=OFN_READONLY;
  CFileDialog m_getfiledlg(TRUE, "dlg", makeitemname(".dlg",0), res, szFilter);

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
    itemname=m_getfiledlg.GetFileTitle(); //itemname moved here because the loader relies on it
    itemname.MakeUpper();
    res=the_dialog.ReadDialogFromFile(fhandle,-1);
    close(fhandle);
    lastopenedoverride=filepath.Left(filepath.ReverseFind('\\'));
    switch(res)
    {
    case -1:
      MessageBox("Dialog loaded with errors.","Warning",MB_ICONEXCLAMATION|MB_OK);
      break;
    case 1:
      if(old_version_dlg())
      {
        MessageBox("Old dialog version (freeze flags were unavailable)","Warning",MB_ICONEXCLAMATION|MB_OK);
      }
    case 0:
      break;
    default:
      MessageBox("Cannot read dialog!","Error",MB_ICONEXCLAMATION|MB_OK);
      NewDialog();
      break;
    }
  }
  m_activesection=FL_TEXT;
  RefreshDialog(0);
  UpdateData(UD_DISPLAY);
}

void CDialogEdit::OnNew() 
{
  NewDialog();
  m_activesection=FL_TEXT;
  RefreshDialog(0);
  UpdateData(UD_DISPLAY);
}

int CDialogEdit::SaveDialog(CString filepath, CString newname)
{
  CString tmpstr;
  int fhandle;
  int res;

  if(newname.GetLength()>8 || newname.GetLength()<1 || newname.Find(" ",0)!=-1)
  {
    tmpstr.Format("The resource name '%s' is bad, it should be 8 characters long and without spaces.",newname);
    MessageBox(tmpstr,"Warning",MB_ICONEXCLAMATION|MB_OK);
    return -99;
  }
  if(newname!=itemname && file_exists(filepath) )
  {
    res=MessageBox("Do you want to overwrite "+newname+"?","Warning",MB_ICONQUESTION|MB_YESNO);
    if(res==IDNO) return -99;
  }
  fhandle=open(filepath, O_BINARY|O_RDWR|O_CREAT|O_TRUNC,S_IREAD|S_IWRITE);
  if(fhandle<1)
  {
    MessageBox("Can't write file!","Error",MB_ICONEXCLAMATION|MB_OK);
    return -99;
  }
  //WritedialogToFile needs itemname to resolve self-references !!!
  itemname=newname;
  res=the_dialog.WriteDialogToFile(fhandle, 0);
  close(fhandle);
  lastopenedoverride=filepath.Left(filepath.ReverseFind('\\'));
  switch(res)
  {
  case 0:
    theApp.AddToRecentFileList(filepath);
    break; //saved successfully
  case -2:
    MessageBox("Error while writing file!","Error",MB_ICONEXCLAMATION|MB_OK);
    break;
  case -3:
    MessageBox("Internal Error!","Error",MB_ICONEXCLAMATION|MB_OK);
    break;
  default:
    MessageBox("Unhandled error!","Error",MB_ICONEXCLAMATION|MB_OK);
  }
  return res;
}

void CDialogEdit::OnSave() 
{
	SaveDialog(makeitemname(".dlg",0),itemname);  
}

void CDialogEdit::OnSaveas() 
{
  CString filepath;
  CString newname;
  int res;

  if(readonly)
  {
    MessageBox("You opened it read only!","Warning",MB_ICONEXCLAMATION|MB_OK);
    return;
  }
  res=OFN_HIDEREADONLY|OFN_ENABLESIZING|OFN_EXPLORER;
  CFileDialog m_getfiledlg(FALSE, "dlg", makeitemname(".dlg",0), res, szFilter);

restart:  
  if( m_getfiledlg.DoModal() == IDOK )
  {
    filepath=m_getfiledlg.GetPathName();
    filepath.MakeLower();
    if(filepath.Right(4)!=".dlg")
    {
      filepath+=".dlg";
    }
    newname=m_getfiledlg.GetFileName();
    newname.MakeUpper();
    if(newname.Right(4)==".DLG") newname=newname.Left(newname.GetLength()-4);
    res=SaveDialog(filepath, newname);
    if(res==-99) goto restart;
  }
  SetWindowText("Edit dialog: "+itemname);
  UpdateData(UD_DISPLAY);
}

void CDialogEdit::OnCheck() 
{
  int ret;
  int node;

	ret=((CChitemDlg *) AfxGetMainWnd())->check_dialog(CHECKING);
  if(ret)
  {
    node=ret&NODEMASK;
    MessageBox(lasterrormsg,"Dialog editor",MB_ICONEXCLAMATION|MB_OK);
    switch(ret&FLAGMASK)
    {
    case STATETR:
      m_currentselection_new=m_treestates.FindStTrigger(node);
      m_dialogtree.Select(m_currentselection_new,TVGN_CARET);
      m_activesection = FL_OPTION;
      break;
    case TRANSTR:
      m_currentselection_new=m_transstates.FindTrTrigger(node);
      m_dialogtree.Select(m_currentselection_new,TVGN_CARET);
      m_activesection = FL_CONDITION;
      break;
    case ACTIONBL:
      m_currentselection_new=m_transstates.FindAction(node);
      m_dialogtree.Select(m_currentselection_new,TVGN_CARET);
      m_activesection = FL_ACTION;
      break;
    case EXTERNAL: 
      m_currentselection_new=m_transstates.FindAction(node);
      m_dialogtree.Select(m_currentselection_new,TVGN_CARET);
      m_activesection = FL_TEXT;
      break;
    case TREESTATE:
      m_currentselection_new=m_treestates.FindElement(node);
      m_dialogtree.Select(m_currentselection_new,TVGN_CARET);
      m_activesection = FL_TEXT;
      break;
    case TRANSSTATE:
      m_currentselection_new=m_transstates.FindElement(node);
      m_dialogtree.Select(m_currentselection_new,TVGN_CARET);
      m_activesection = FL_TEXT;
      break;
    default:
      break;
    }
  }
  UpdateData(UD_DISPLAY);
}

void CDialogEdit::WalkOnTree(int command)
{
  POSITION pos;
  HTREEITEM ht;
  int dummy;

  pos=m_treestates.GetStartPosition();
  while(pos)
  {
    m_treestates.GetNextAssoc(pos, ht, dummy);
    m_dialogtree.Expand(ht,command);
  }

  pos=m_transstates.GetStartPosition();
  while(pos)
  {
    m_transstates.GetNextAssoc(pos, ht, dummy);
    m_dialogtree.Expand(ht,command);
  }
}

//////// not completed yet, have to skip links!!!
#define NODE_TREE TRUE
#define NODE_TRANS FALSE

#define P_TEXT 1
#define P_SCRIPT 2
#define P_BOTH 3

int match_script_text(CString text, CString substr)
{
  text.MakeLower();
  substr.MakeLower();
  return text.Find(substr)!=-1;
}

int CDialogEdit::SeekTo(int startnode, int type)
{
  int i;
  int from,to;
  CString tmpstr;

  if(type==NODE_TREE)
  {
    if(the_dialog.header.numstates<=startnode) return -1;
    if(m_treeflags[startnode]) return -1;
    m_treeflags[startnode]=1;
    if(m_searchdlg.m_nodetype!=NODE_TRANS)//if it is only NODE_TRANS then don't look into a state
    {
      if(m_searchdlg.m_nodepart&P_TEXT)
      {
        if(match_tlk_text(the_dialog.dlgstates[startnode].actorstr,m_searchdlg.m_text, m_searchdlg.m_ignorecase) )
        {
          m_activesection=FL_TEXT;
          return startnode|TREESTATE;
        }
      }
      if(m_searchdlg.m_nodepart&P_SCRIPT)
      {
        i=the_dialog.dlgstates[startnode].stindex;
        if(i>=0 && i<the_dialog.header.numsttrigger)
        {// in scripts case is always ignored
          if(match_script_text(the_dialog.sttriggers[i],m_searchdlg.m_text))
          {
            m_activesection=FL_CONDITION;
            return startnode|TREESTATE;
          }
        }
      }
    }
    type=NODE_TRANS;
    from=the_dialog.dlgstates[startnode].trindex;
    if(from<0) return -1;
    to=from+the_dialog.dlgstates[startnode].trnumber;
    if(to>=the_dialog.header.numtrans) return -1;
    for(i=from;i<to;i++)
    {
      startnode=SeekTo(i, type);
      if(startnode!=-1) return startnode;
    }
    return -1;
  }
  if(the_dialog.header.numtrans<=startnode) return -1;
  if(m_transflags[startnode]) return -1;
  m_transflags[startnode]=1;
  if(m_searchdlg.m_nodetype!=NODE_TREE) //if it is only NODE_TREE then don't look into a transition
  {
    if(m_searchdlg.m_nodepart&P_TEXT)
    {
      if(match_tlk_text(the_dialog.dlgtrans[startnode].playerstr,m_searchdlg.m_text, m_searchdlg.m_ignorecase) )
      {
        m_activesection=FL_OPTION;
        return startnode|TRANSSTATE;
      }

      if(match_tlk_text(the_dialog.dlgtrans[startnode].journalstr,m_searchdlg.m_text, m_searchdlg.m_ignorecase) )
      {
        m_activesection=FL_JOURNAL;
        return startnode|TRANSSTATE;
      }
    }
    if(m_searchdlg.m_nodepart&P_SCRIPT)
    {
      i=the_dialog.dlgtrans[startnode].trtrindex;
      if((the_dialog.dlgtrans[startnode].flags&HAS_TRIGGER) && i>=0 && i<the_dialog.header.numtrtrigger)
      {
        if(match_script_text(the_dialog.trtriggers[i],m_searchdlg.m_text))
        {
          m_activesection=FL_CONDITION;
          return startnode|TRANSSTATE;
        }
      }
      i=the_dialog.dlgtrans[startnode].actionindex;      
      if((the_dialog.dlgtrans[startnode].flags&HAS_ACTION) && i>=0 && i<the_dialog.header.numaction)
      {
        if(match_script_text(the_dialog.actions[i],m_searchdlg.m_text))
        {
          m_activesection=FL_ACTION;
          return startnode|TRANSSTATE;
        }
      }
    }
  }
  if(the_dialog.dlgtrans[startnode].flags&LEAF_NODE) return -1; //terminates node
  RetrieveResref(tmpstr,the_dialog.dlgtrans[startnode].nextdlg);
  if(tmpstr!=SELF_REFERENCE) return -1; //external reference
  type=NODE_TREE;
  startnode=the_dialog.dlgtrans[startnode].stateindex;
  if(startnode<0) return -1;
  return SeekTo(startnode, type);
}

void CDialogEdit::SearchNode()
{
  int key;
  int i;
  
  if(m_transstates.Lookup(m_currentselection,key) )
  {
    m_searchdlg.m_node=key;
    m_searchdlg.m_type=NODE_TRANS;
  }
  else
  {
    if(m_treestates.Lookup(m_currentselection,key) )
    {
      m_searchdlg.m_node=key;
    }
    else
    {
      m_searchdlg.m_node=0;
    }
    m_searchdlg.m_type=NODE_TREE;
  }
  if(m_searchdlg.DoModal()==IDOK)
  {
    m_transflags=new char[the_dialog.header.numtrans];
    m_treeflags=new char[the_dialog.header.numstates];
    memset(m_transflags,0,the_dialog.header.numtrans);
    memset(m_treeflags,0,the_dialog.header.numstates);
    if(m_searchdlg.m_node)
    {
      key=SeekTo(m_searchdlg.m_node,m_searchdlg.m_type);
    }
    else
    {
      for(i=0;i<the_dialog.header.numstates;i++)
      {
        key=SeekTo(i,m_searchdlg.m_type);
        if(key!=-1) break;
      }
    }
    delete [] m_transflags;
    delete [] m_treeflags;
    if(key==-1)
    {
      MessageBox("Node not found.","Dialog editor",MB_OK);
      return;
    }
    switch(key&FLAGMASK)
    {
    case TREESTATE:
      m_dialogtree.SelectItem(m_treestates.FindElement(key&NODEMASK));
      break;
    case TRANSSTATE:
      m_dialogtree.SelectItem(m_transstates.FindElement(key&NODEMASK));
      break;
    default:
      break;
    }
  }
}

void CDialogEdit::RefreshMenu()
{
  int key;
  int enable, enable2, enable3, enable4;
  CMenu *menu;
  CMenu *popupmenu;

  DisplayText();
  enable4=enable3=enable2=enable=MF_GRAYED;
  if(m_currentselection)
  {
    switch(m_dialogtree.GetItemData(m_currentselection)&FLAGMASK)
    {
    case LINKTO:
      enable=MF_ENABLED;
      enable2=MF_ENABLED;
      break;
    case EXTERNAL:
      if(m_treestates.Lookup(m_currentselection,key) )
      {
        enable3=MF_ENABLED;
      }
      else
      {
        enable=MF_ENABLED;
        enable2=MF_ENABLED;
      }
      break;
    default:
      if(m_treestates.Lookup(m_currentselection,key) )
      {
        if( m_dialogtree.GetParentItem(m_currentselection) )
        {
          enable2=MF_ENABLED;
        }
        else
        {
          enable4=MF_ENABLED;
        }
      }
      else if(m_transstates.Lookup(m_currentselection,key) )
      {
        enable2=MF_ENABLED;
      }
      enable3=MF_ENABLED;
      break;
    }
  }
  menu=GetMenu();
  popupmenu=menu->GetSubMenu(TREEMENU);
  popupmenu->EnableMenuItem(ID_FOLLOWLINK,enable);
  popupmenu->EnableMenuItem(ID_CONVERT,enable2);
  popupmenu->EnableMenuItem(ID_SWAPNODES,enable3);
  popupmenu->EnableMenuItem(ID_INCREASEWEIGHT,enable4);
}

void CDialogEdit::OnRefresh() 
{
  if(m_currentselection==m_currentselection_new) return;
  m_currentselection=m_currentselection_new;
  if(!m_currentselection) return;
  RefreshMenu();
  UpdateData(UD_DISPLAY);	
}

void CDialogEdit::OnExpand() 
{
  WalkOnTree(TVE_EXPAND);
  m_dialogtree.Select(m_currentselection, TVGN_FIRSTVISIBLE);
  PostMessage(WM_COMMAND,ID_REFRESH,0);
}

void CDialogEdit::OnCollapse() 
{
  WalkOnTree(TVE_COLLAPSE);
  m_currentselection_new=m_dialogtree.GetSelectedItem();
  PostMessage(WM_COMMAND,ID_REFRESH,0);
}

void CDialogEdit::OnSearchnode() 
{
  SearchNode();
  m_dialogtree.Select(m_dialogtree.GetSelectedItem(), TVGN_FIRSTVISIBLE);
}

void CDialogEdit::OnFollowlink() 
{
  CString dialogref;
  int key;
  int stateidx;
  HTREEITEM ht;
  int res;

  ht=m_dialogtree.GetParentItem(m_currentselection);
  if(m_transstates.Lookup(ht, key) )
  {
  	stateidx=the_dialog.dlgtrans[key].stateindex;
    RetrieveResref(dialogref,the_dialog.dlgtrans[key].nextdlg);
    if(dialogref!=SELF_REFERENCE)
    {
      if(MessageBox("Do you want to skip to the other dialog?","Dialog editor",MB_YESNO)!=IDYES) return;
      if(the_dialog.changed && (MessageBox("This dialog wasn't saved, do you want to abandon it?","Dialog editor",MB_YESNO)!=IDYES))
      {
        return;
      }
      //
      res=read_dialog(dialogref);
      switch(res)
      {
      case -3:
        MessageBox("Dialog loaded with errors.","Warning",MB_ICONEXCLAMATION|MB_OK);
        break;
      case 1:
        if(!pst_compatible_var() && !has_xpvar())
        {          
          MessageBox("Old dialog version (freeze flags were unavailable)","Warning",MB_ICONEXCLAMATION|MB_OK);     
        }
      case 0:
        break;
      default:
        MessageBox("Cannot read dialog!","Error",MB_ICONEXCLAMATION|MB_OK);
        NewDialog();
        break;
      }
      m_activesection=FL_TEXT;
      RefreshDialog(stateidx);
    }
    else
    {
      m_currentselection=m_treestates.FindElement(stateidx);
      m_dialogtree.Select(m_currentselection, TVGN_CARET);
      PostMessage(WM_COMMAND,ID_REFRESH,0);
    }
  }
}

void CDialogEdit::OnDblclkDialogtree(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
{
	*pResult = 0;
  PostMessage(WM_COMMAND,ID_FOLLOWLINK,0);
}

void CDialogEdit::OnSelchangedDialogtree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

  *pResult = 0;
  m_currentselection_new=pNMTreeView->itemNew.hItem;
  PostMessage(WM_COMMAND,ID_REFRESH,0);
}

void CDialogEdit::OnTagged() 
{
  if(editflg&TLKCHANGE)
  {
    if(MessageBox("Do you want to update dialog.tlk?","Dialog editor",MB_YESNO)!=IDYES)
    {
      return;
    }
  }
  toggle_tlk_tag(m_strref);
	UpdateData(UD_DISPLAY);	
}

void CDialogEdit::OnKillfocusJourtype() 
{
	int jourtype;
  int flag;
  int key;
	
  the_dialog.changed=1;
  jourtype=m_jourtype_control.GetCurSel();
  if(!m_transstates.Lookup(m_currentselection,key)) return;
  flag=the_dialog.dlgtrans[key].flags&~(HAS_QUEST|HAS_SOLVED);
  //not all journal type flags have been found
  switch(jourtype)
  {
  case 0: //journal
    break;
  case 1: //quest
    flag|=HAS_QUEST;
    break;
  case 2: //solved quest
    flag|=HAS_SOLVED;
    break;
  case 3: //user
    flag|=HAS_QUEST|HAS_SOLVED;
    break;
  }
  the_dialog.dlgtrans[key].flags=flag;
  DisplayText();
	UpdateData(UD_DISPLAY);	
}

void CDialogEdit::OnHasoption() 
{
  int key;

	if(m_transstates.Lookup(m_currentselection,key))
  {
    m_activesection=FL_OPTION;
    the_dialog.dlgtrans[key].flags^=HAS_TEXT;
    if(the_dialog.dlgtrans[key].flags&HAS_TEXT)
    {//create text entry ?
    }
    else //remove text entry
    {
      m_strref=0;
      m_text=resolve_tlk_text(m_strref);
      RefreshLeaf();
    }
    RefreshMenu();
    m_text_control.SetFocus();
    UpdateData(UD_DISPLAY);	
    return;
  }
  if(m_treestates.Lookup(m_currentselection,key))
  {
    m_activesection=FL_TEXT;
    /*
    if(the_dialog.dlgstates[key].actorstr)
    {
      the_dialog.dlgstates[key].actorstr=0;
    }
    */
  	UpdateData(UD_DISPLAY);	
    return; //can't look it up
  }
  //is this an error ?
  m_activesection=0;
  MessageBox("Can't find transition","Internal error",MB_OK);
  UpdateData(UD_DISPLAY);	
  return; //can't look it up
}

int remove_state_trigger(int stindex)
{
  CString *newtriggers;
  int i,j;
  
  the_dialog.sttriggercount--;
  newtriggers = new CString [the_dialog.sttriggercount];
  if(!newtriggers)
  {
    the_dialog.sttriggercount++;
    return -3;
  }
  for(i=0;i<the_dialog.statecount;i++)
  {
    if(the_dialog.dlgstates[i].stindex>stindex)
    {
      the_dialog.dlgstates[i].stindex--;
    }
  }
  j=0;
  for(i=0;i<the_dialog.header.numsttrigger;i++) //the original number of actions
  {
    if(i!=stindex)
    {
      newtriggers[j++]=the_dialog.sttriggers[i];
    }
  }
  delete [] the_dialog.sttriggers;
  the_dialog.sttriggers=newtriggers;
  the_dialog.header.numsttrigger=the_dialog.sttriggercount;
  return 0;
}

int add_state_trigger()
{
  CString *newtriggers;
  int trindex;
  int i;
  
  trindex=the_dialog.sttriggercount++;
  newtriggers = new CString [the_dialog.sttriggercount];
  if(!newtriggers)
  {
    the_dialog.sttriggercount--;
    return -1;
  }
  for(i=0;i<the_dialog.header.numsttrigger;i++)
  {
    newtriggers[i]=the_dialog.sttriggers[i];
  }
  delete [] the_dialog.sttriggers;
  the_dialog.sttriggers=newtriggers;
  the_dialog.header.numsttrigger=the_dialog.sttriggercount;
  return trindex;
}

int remove_transition_trigger(int trindex)
{
  CString *newtriggers;
  int i,j;
  
  the_dialog.trtriggercount--;
  newtriggers = new CString [the_dialog.trtriggercount];
  if(!newtriggers)
  {
    the_dialog.trtriggercount++;
    return -3;
  }
  for(i=0;i<the_dialog.transcount;i++)
  {
    if(the_dialog.dlgtrans[i].trtrindex>trindex)
    {
      the_dialog.dlgtrans[i].trtrindex--;
    }
  }
  j=0;
  for(i=0;i<the_dialog.header.numtrtrigger;i++) //the original number of actions
  {
    if(i!=trindex)
    {
      newtriggers[j++]=the_dialog.trtriggers[i];
    }
  }
  delete [] the_dialog.trtriggers;
  the_dialog.trtriggers=newtriggers;
  the_dialog.header.numtrtrigger=the_dialog.trtriggercount;
  return 0;
}

void CDialogEdit::OnHascondition() 
{
  int key;
  int tmp;
  int refreshflag;

  refreshflag=FALSE;
  the_dialog.changed=1;
  if(m_transstates.Lookup(m_currentselection,key))
  {
    m_activesection=FL_CONDITION;
    if(the_dialog.dlgtrans[key].flags&HAS_TRIGGER)
    {//remove transition trigger entry
      if(!remove_transition_trigger(the_dialog.dlgtrans[key].trtrindex))
      {
        the_dialog.dlgtrans[key].flags&=~HAS_TRIGGER;
        the_dialog.dlgtrans[key].trtrindex=0;
      }
      else
      {
        MessageBox("Cannot remove entry!","Dialog editor",MB_OK);
      }
    }
    else //add transition trigger entry
    {
      tmp=add_transition_trigger();
      if(tmp==-1)
      {
        MessageBox("Cannot add entry!","Dialog editor",MB_OK);
      }
      else
      {
        the_dialog.dlgtrans[key].trtrindex=tmp;
        the_dialog.dlgtrans[key].flags|=HAS_TRIGGER;
      }
    }
    RefreshMenu();
    m_text_control.SetFocus();
    UpdateData(UD_DISPLAY);	
    return;
  }
  if(m_treestates.Lookup(m_currentselection,key))
  {
    m_activesection=FL_CONDITION;
    if(the_dialog.dlgstates[key].stindex==-1)
    {
      tmp=add_state_trigger();
      if(tmp==-1)
      {
        MessageBox("Cannot add entry!","Dialog editor",MB_OK);
      }
      else
      { //adding a state trigger to an external referenced or linked node will make it root node
        //(ext. ref or link becomes Top level)
        the_dialog.dlgstates[key].stindex=tmp;
        refreshflag=TRUE;
      }
    }
    else
    { //removing a state trigger from a top level node will make it ext. ref or internally
      //linked
      if(the_dialog.sttriggercount<2)
      {//some warning comes here, dialogs need at least one state trigger defined 
        if(MessageBox("Are you sure you want to remove the last state trigger?","Dialog editor",MB_YESNO)!=IDYES)
        {
          return;
        }
      }
      if(!remove_state_trigger(the_dialog.dlgstates[key].stindex))
      {
        the_dialog.dlgstates[key].stindex=-1;
        refreshflag=TRUE;
      }
      else
      {
        MessageBox("Cannot remove entry!","Dialog editor",MB_OK);
      }
    }
    if(refreshflag) RefreshDialog(key);
    RefreshMenu();
    UpdateData(UD_DISPLAY);	
    return;
  }
  MessageBox("Can't find trigger","Internal error",MB_OK);
}

int remove_action(int acindex)
{
  CString *newactions;
  int i,j;
  
  the_dialog.actioncount--;
  newactions = new CString [the_dialog.actioncount];
  if(!newactions)
  {
    the_dialog.actioncount++;
    return -3;
  }
  for(i=0;i<the_dialog.transcount;i++)
  {
    if(the_dialog.dlgtrans[i].actionindex>acindex)
    {
      the_dialog.dlgtrans[i].actionindex--;
    }
  }
  j=0;
  for(i=0;i<the_dialog.header.numaction;i++) //the original number of actions
  {
    if(i!=acindex)
    {
      newactions[j++]=the_dialog.actions[i];
    }
  }
  delete [] the_dialog.actions;
  the_dialog.actions=newactions;
  the_dialog.header.numaction=the_dialog.actioncount;
  return 0;
}

void CDialogEdit::OnHasaction() 
{
  int key;
  int tmp;

  the_dialog.changed=1;
	if(!m_transstates.Lookup(m_currentselection,key))
  {
    //is this an error ?
    MessageBox("Can't find action","Internal error",MB_OK);
    return; //can't look it up
  }
  m_activesection=FL_ACTION;
  if(the_dialog.dlgtrans[key].flags&HAS_ACTION)
  {//remove action entry
    if(!remove_action(the_dialog.dlgtrans[key].actionindex))
    {
      the_dialog.dlgtrans[key].flags&=~HAS_ACTION;
      the_dialog.dlgtrans[key].actionindex=0;
    }
    else
    {
      MessageBox("Cannot remove entry!","Dialog editor",MB_OK);
    }
  }
  else //add action entry
  {
    tmp=add_action();
    if(tmp==-1)
    {
      MessageBox("Cannot add entry!","Dialog editor",MB_OK);
    }
    else
    {
      the_dialog.dlgtrans[key].actionindex=tmp;
      the_dialog.dlgtrans[key].flags|=HAS_ACTION;
    }
  }
  //the common part
  RefreshMenu();
  m_text_control.SetFocus();
	UpdateData(UD_DISPLAY);	
}

void CDialogEdit::OnHasjournal() 
{
  int key;

  the_dialog.changed=1;
	if(!m_transstates.Lookup(m_currentselection,key))
  {
    //is this an error ?
    MessageBox("Can't find transition","Internal error",MB_OK);
    return; //can't look it up
  }
  m_activesection=FL_JOURNAL;
  the_dialog.dlgtrans[key].flags^=HAS_JOURNAL;
  if(the_dialog.dlgtrans[key].flags&HAS_JOURNAL)
  {//create journal entry
    the_dialog.dlgtrans[key].journalstr=-1;  //creates empty journal entry
  }
  else //remove journal entry
  {
    the_dialog.dlgtrans[key].journalstr=0;
    the_dialog.dlgtrans[key].flags&=~(HAS_QUEST|HAS_SOLVED);
  }
  RefreshMenu();
  m_text_control.SetFocus();
	UpdateData(UD_DISPLAY);	
}

void CDialogEdit::OnKillfocusFlags() 
{
  UpdateData(UD_RETRIEVE);
	UpdateData(UD_DISPLAY);
}

int CDialogEdit::GetRowCount()
{
	CWnd *cntrl = GetDlgItem(IDC_TEXT);
	CRect rect;
	cntrl->GetWindowRect(&rect);
	return rect.Height()/13;
}

void CDialogEdit::CheckTriggerOrAction(int messages)
{
  CString *lines;
  int *errors;
  CButton *cb;
  CString tmpstr;
  trigger trigger;
  action action;
  int rows, count;
  int i;
  int topline,firsterror;
  int triggeroraction;

	if(locked) return;
	locked=true;
	rows=GetRowCount();
  if((m_activesection!=FL_ACTION) && (m_activesection!=FL_CONDITION))
  {
    for(i=0;i<40;i++)
    {
      cb=(CButton *) GetDlgItem(IDC_U1+i);
      cb->ShowWindow(false);
    }
		locked=false;
    return;
  }
  
  triggeroraction=m_activesection!=FL_ACTION;
	GetDlgItem(IDC_TEXT)->GetWindowText(m_text);
  lines=explode(m_text, '\n', count);
  if(count<0)
  {
    errors=new int[1];
    errors[0]=count;
    firsterror=0;
    count=1;
  }
  else
  {
    firsterror=-1;
    errors=new int[count];
    for(i=0;i<count;i++)
    {
      if(triggeroraction) errors[i]=compile_trigger(lines[i], trigger);
      else errors[i]=compile_action(lines[i], action, 0);
      if(errors[i])
      {
        if(firsterror==-1) firsterror=i;
      }
    }
  }
  if(firsterror>=0)
  {
		if(messages)
		{
			m_text_control.LineScroll(firsterror);
		}
    topline=m_text_control.GetFirstVisibleLine();
    for(i=0;i<40;i++)
    {
      cb=(CButton *) GetDlgItem(IDC_U1+i);
      if(topline+i>=count || i>=rows)
      {
        cb->ShowWindow(false);
      }
      else
      {
        cb->ShowWindow(true);
        tmpstr.Format("%d",topline+i+1);
        cb->SetWindowText(tmpstr);
        if(errors[topline+i]) cb->SetCheck(false);
        else cb->SetCheck(true);
      }
    }
    if(messages)
    {
      tmpstr.Format("Error in script line #%d: %s",firsterror+1,get_compile_error(errors[firsterror]) );
      m_currentselection_new=m_currentselection; //stepping back on the faulty leaf
      MessageBox(tmpstr,"Dialog editor",MB_ICONEXCLAMATION);
    }
  }
  else
  {
    topline=m_text_control.GetFirstVisibleLine();
    for(i=0;i<40;i++)
    {
      cb=(CButton *) GetDlgItem(IDC_U1+i);
			if(!cb) continue;
      if(topline+i>=count || i>=rows)
      {
        cb->ShowWindow(false);
      }
      else
      {
        cb->ShowWindow(true);
        tmpstr.Format("%d",topline+i+1);
        cb->SetWindowText(tmpstr);
        cb->SetCheck(true);
      }
    }
  }
  if(lines) delete [] lines;
  delete [] errors;
	locked=false;
}

void CDialogEdit::OnSetfocusText() 
{
  CheckTriggerOrAction(false);
}

//removing empty lines
CString Consolidate(CString old)
{
  CString *lines;
  int idx;
  int i;

  old.Replace("\r\n","\n");
  lines=explode(old,'\n',idx);
  old.Empty();
  for(i=0;i<idx;i++)
  {
    if(!lines[i].IsEmpty())
    {
      old+=lines[i];
      if(i<idx-1) old+="\n";
    }
  }
  if(lines) delete [] lines;
  return old;
}

void CDialogEdit::OnKillfocusText() 
{
  CString old;
  int key;
  int idx;

  UpdateData(UD_RETRIEVE);
  switch(m_activesection)
  {
  case FL_TEXT: case FL_OPTION: case FL_JOURNAL:
    old=resolve_tlk_text(m_strref);
    if(old!=m_text)
    {
      if(editflg&TLKCHANGE)
      {
        if(MessageBox("Do you want to update dialog.tlk?","Dialog editor",MB_YESNO)!=IDYES)
        {
          return;
        }
      }
      m_strref=store_tlk_text(m_strref,m_text);
      RefreshLeaf();
      UpdateData(UD_DISPLAY);	
    }
    break;
  case FL_ACTION:
    CheckTriggerOrAction(false);
    if(!m_transstates.Lookup(m_currentselection,key) ) break;
    idx=the_dialog.dlgtrans[key].actionindex;
    if(idx<0)
    {
      idx=idx+1;
      break;
    }
    old=Consolidate(m_text);
    if(old.IsEmpty())
    {
      remove_action(idx);
      the_dialog.dlgtrans[key].flags&=~HAS_ACTION;
      break;
    }
    the_dialog.actions[idx]=old;
    break;
  case FL_CONDITION:
    CheckTriggerOrAction(false);
    if(m_transstates.Lookup(m_currentselection,key) )
    {
      idx=the_dialog.dlgtrans[key].trtrindex;
      if(idx<0)
      {
        idx=idx+1;
        break;
      }
      old=Consolidate(m_text);
      if(old.IsEmpty())
      {
        remove_transition_trigger(idx);
        the_dialog.dlgtrans[key].flags&=~HAS_TRIGGER;
        break;
      }
      the_dialog.trtriggers[idx]=old;
      break;
    }
    if(m_treestates.Lookup(m_currentselection,key) )
    {
      idx=the_dialog.dlgstates[key].stindex;
      if(idx<0)
      {
        idx=idx+1;
        break;
      }
      old=Consolidate(m_text);
      if(old.IsEmpty())
      {
        remove_state_trigger(idx);
        the_dialog.dlgstates[key].stindex=-1;
        break;
      }
      the_dialog.sttriggers[idx]=old;
    }
    break;  
  }
  DisplayText();
  RefreshLeaf();
  UpdateData(UD_DISPLAY);	
}

//updates one leaf on the tree & in the dialog structure
void CDialogEdit::RefreshLeaf()
{
  int key,idx;
  CString old;

  if(!m_currentselection) return;
  key=-1;
  idx=m_dialogtree.GetItemData(m_currentselection);
  switch(m_activesection)
  {
  case FL_TEXT:
    if(m_treestates.Lookup(m_currentselection,key) )
    {
      the_dialog.dlgstates[key].actorstr=m_strref;
    }
    if(key!=(idx&NODEMASK) )
    {
      RefreshDialog(0);
      MessageBox("Please try to reproduce this","Internal error",MB_ICONSTOP|MB_OK);
      return;
    }
    switch(idx&FLAGMASK)
    {
    case TREESTATE:
      if(the_dialog.dlgstates[key].stindex==-1)
      {
        old.Format("Text#%d: %s",key,m_text);
      }
      else
      {
        old.Format("Top level#%d (Order:%d):  %s",key, the_dialog.dlgstates[key].stindex,m_text);
      }
      break;
    case EXTERNAL:
      old.Format("Ext. ref.#%d: %s",key,m_text);
      break;
    default:
      abort();
    }
    m_dialogtree.SetItemText(m_currentselection, old);
    break;
  case FL_OPTION:
    if(m_transstates.Lookup(m_currentselection,key) )
    {
      the_dialog.dlgtrans[key].playerstr=m_strref;
      if(m_strref) the_dialog.dlgtrans[key].flags|=HAS_TEXT;
      else the_dialog.dlgtrans[key].flags&=~HAS_TEXT;
    }
    if(key!=(idx&NODEMASK) )
    {
      abort();
    }
    if((idx&FLAGMASK)!=TRANSSTATE)
    {
      abort();
    }
    if(the_dialog.dlgtrans[key].flags&LEAF_NODE)
    {
      old.Format("End node#%d: %s",key,m_text);
    }
    else
    {      
      old.Format("Reply#%d: %s",key,m_text);
    }
    m_dialogtree.SetItemText(m_currentselection, old);
    break;
  case FL_JOURNAL:
    if(m_transstates.Lookup(m_currentselection,key) )
    {
      the_dialog.dlgtrans[key].journalstr=m_strref;
      if(m_strref) the_dialog.dlgtrans[key].flags|=HAS_JOURNAL;
      else the_dialog.dlgtrans[key].flags&=~HAS_JOURNAL;
    }
  }
  m_dialogtree.Select(m_currentselection, TVGN_CARET);
}

void CDialogEdit::OnKillfocusStrref() 
{
  UpdateData(UD_RETRIEVE);
  m_text=resolve_tlk_text(m_strref);
  RefreshLeaf();
  UpdateData(UD_DISPLAY);	
}

//do not change the static flags
void CDialogEdit::OnKillfocusValue() 
{
  UpdateData(UD_RETRIEVE);
  DisplayText();
  UpdateData(UD_DISPLAY);	
}

void CDialogEdit::OnKillfocusDialog() 
{
  int key;

  UpdateData(UD_RETRIEVE);	
  if(m_transstates.Lookup(m_currentselection,key) )
  {
    StoreResref(m_dialog,the_dialog.dlgtrans[key].nextdlg);
  }
  UpdateData(UD_DISPLAY);	
}

void CDialogEdit::OnOption() 
{
	m_activesection=FL_TEXT|FL_OPTION;
  UpdateData(UD_RETRIEVE);
  RefreshMenu();
  UpdateData(UD_DISPLAY);
}

void CDialogEdit::OnCondition() 
{
	m_activesection=FL_CONDITION;	
  UpdateData(UD_RETRIEVE);
  RefreshMenu();
	UpdateData(UD_DISPLAY);	
}

void CDialogEdit::OnAction() 
{
	m_activesection=FL_ACTION;	
  UpdateData(UD_RETRIEVE);
  RefreshMenu();
	UpdateData(UD_DISPLAY);	
}

void CDialogEdit::OnJournal() 
{
	m_activesection=FL_JOURNAL;	
  UpdateData(UD_RETRIEVE);
  RefreshMenu();
	UpdateData(UD_DISPLAY);	
}

void CDialogEdit::OnVersion() 
{
  UpdateData(UD_RETRIEVE);
  if(the_dialog.header.offstates==0x30) the_dialog.header.offstates=0x34;
  else the_dialog.header.offstates=0x30;
  RefreshDialog(-1);
  UpdateData(UD_DISPLAY);
}

void CDialogEdit::OnVscrollText() 
{
  CheckTriggerOrAction(false);
}

void CDialogEdit::OnEnChangeText()
{
  CheckTriggerOrAction(false);
}

void CDialogEdit::OnKillfocusSound() 
{
  CString old;

  UpdateData(UD_RETRIEVE);
  old=resolve_tlk_soundref(m_strref);
  if(old!=m_sound)
  {
    if(editflg&TLKCHANGE)
    {
      if(MessageBox("Do you want to update dialog.tlk?","Dialog editor",MB_YESNO)!=IDYES)
      {
        return;
      }
    }
    store_tlk_soundref(m_strref,m_sound);
  }
  UpdateData(UD_DISPLAY);	
}

void CDialogEdit::OnBrowse() 
{
  pickerdlg.m_restype=REF_WAV;
  pickerdlg.m_picked=m_sound;
  if(pickerdlg.DoModal()==IDOK)
  {
    if(editflg&TLKCHANGE)
    {
      if(MessageBox("Do you want to update dialog.tlk?","Dialog editor",MB_YESNO)!=IDYES)
      {
        return;
      }
    }
    m_sound=pickerdlg.m_picked;
    store_tlk_soundref(m_strref,m_sound);
  }
	UpdateData(UD_DISPLAY);	
}

void CDialogEdit::OnPlay() 
{
  play_acm(m_sound,false,false);	
}

void CDialogEdit::OnContextMenu(CWnd* pWnd, CPoint point) 
{
  if(pWnd && (pWnd->GetDlgCtrlID()==IDC_DIALOGTREE) )
  {
    CMenu *menu;
    CMenu *popupmenu;
    
    menu=GetMenu();    
    popupmenu=menu->GetSubMenu(TREEMENU); //this should be better defined
    popupmenu->TrackPopupMenu(TPM_CENTERALIGN,point.x,point.y,this);
  }
}

void CDialogEdit::OnGetinfotipDialogtree(NMHDR* pNMHDR, LRESULT* pResult) 
{
  CString tmpstr;
  int key;

	NMTVGETINFOTIP* pTVGetInfotip = (NMTVGETINFOTIP*)pNMHDR;
  
	*pResult = 0;
  if(m_transstates.Lookup(pTVGetInfotip->hItem, key) )
  {
    return;
  }
  if(m_treestates.Lookup(pTVGetInfotip->hItem, key) )
  {
    return;
  }
  tmpstr.LoadString(IDS_DBLCLICKLEAF);
  strncpy(pTVGetInfotip->pszText,tmpstr,pTVGetInfotip->cchTextMax);
}

int remove_transitions(int first, int count)
{
  int i;
  dlg_trans *newtrans;
  int idx;
  int ret;

  if(count>the_dialog.transcount) return -1; //how could this be ???
  //removing the triggers/actions of the removed transitions first
  for(i=0;i<count;i++)
  {
    if(the_dialog.dlgtrans[first+i].flags&HAS_TRIGGER)
    {
      idx=the_dialog.dlgtrans[first+i].trtrindex;
      ret=remove_transition_trigger(idx);
      if(ret) return ret;
    }
    if(the_dialog.dlgtrans[first+i].flags&HAS_ACTION)
    {
      idx=the_dialog.dlgtrans[first+i].actionindex;
      ret=remove_action(idx);
      if(ret) return ret;
    }
  }
  the_dialog.transcount-=count;
  newtrans= new dlg_trans[the_dialog.transcount];
  if(!newtrans)
  {
    the_dialog.transcount+=count;
    return -3; //no memory
  }
  memcpy(newtrans,the_dialog.dlgtrans,first*sizeof(dlg_trans) ); //copying the first segment
  memcpy(newtrans+first,the_dialog.dlgtrans+first+count, (the_dialog.transcount-first)*sizeof(dlg_trans) );
  delete [] the_dialog.dlgtrans;
  the_dialog.dlgtrans=newtrans;
  the_dialog.header.numtrans=the_dialog.transcount;
  return 0;
}

int swap_states(int stateidxa, int stateidxb)
{
  CString tmpstr;
  dlg_state tmp;
  int i;

  if(the_dialog.statecount<=stateidxa) return -1;
  if(the_dialog.statecount<=stateidxb) return -1;
  tmp=the_dialog.dlgstates[stateidxa];
  the_dialog.dlgstates[stateidxa]=the_dialog.dlgstates[stateidxb];
  the_dialog.dlgstates[stateidxb]=tmp;
  for(i=0;i<the_dialog.header.numtrans;i++)
  {
    RetrieveResref(tmpstr,the_dialog.dlgtrans[i].nextdlg);
    if(tmpstr==SELF_REFERENCE)
    {
      if(the_dialog.dlgtrans[i].stateindex==stateidxa) the_dialog.dlgtrans[i].stateindex=stateidxb;
      else if(the_dialog.dlgtrans[i].stateindex==stateidxb) the_dialog.dlgtrans[i].stateindex=stateidxa;
    }
  }
  return 0;
}

int remove_state(int stateidx)
{
  CString tmpstr;
  dlg_state *newstates;
  int triggeridx;
  int first,count;
  int i,j;
  int ret;

  //removing the state trigger associated with this state
  triggeridx=the_dialog.dlgstates[stateidx].stindex;
  if(triggeridx>=0)
  {
    remove_state_trigger(triggeridx);
  }
  //allocating states now (if it is allocated later, the transitions are
  //already modified
  the_dialog.statecount--;
  newstates=new dlg_state[the_dialog.statecount];
  if(!newstates)
  {
    the_dialog.statecount++;
    return -3;
  }
  //removing the transitions of this state
  first=the_dialog.dlgstates[stateidx].trindex;
  count=the_dialog.dlgstates[stateidx].trnumber;
  ret=remove_transitions(first,count);
  if(ret)
  {
    //bailing out after a failed transition removal
    delete [] newstates;
    the_dialog.statecount++;
    return ret;
  }
  //making all referencing transitions 'end node'
  //and decreasing references to higher states
  for(i=0;i<the_dialog.transcount;i++)
  {
    RetrieveResref(tmpstr,the_dialog.dlgtrans[i].nextdlg);
    if(tmpstr!=SELF_REFERENCE) continue;
    if(the_dialog.dlgtrans[i].stateindex==stateidx)
    {
      the_dialog.dlgtrans[i].stateindex=0;
      the_dialog.dlgtrans[i].flags|=LEAF_NODE;
      memset(the_dialog.dlgtrans[i].nextdlg,0,8);
    }
    else if(the_dialog.dlgtrans[i].stateindex>stateidx)
    {
      the_dialog.dlgtrans[i].stateindex--;
    }
  }
  //removing the state by copying all other states
  j=0;
  for(i=0;i<the_dialog.header.numstates;i++)
  {
    if(i!=stateidx)
    {
      newstates[j]=the_dialog.dlgstates[i];
      //adjusting the transition references
      if(newstates[j].trindex>=first)
      {
        if(newstates[j].trindex>=first+count)
        {
          newstates[j].trindex-=count; 
        }
        else
        {
          return -1;
        }
      }
      j++;
    }
  }
  delete [] the_dialog.dlgstates;
  the_dialog.dlgstates=newstates;
  the_dialog.header.numstates=the_dialog.statecount;
  return 0;
}

int remove_transition(int stateidx, int transidx)
{
  int i;
  
  //decrease the parent state's transition count, remove transition
  if(the_dialog.dlgstates[stateidx].trnumber) 
  {
    the_dialog.dlgstates[stateidx].trnumber--;
    remove_transitions(transidx,1);
  }
  for(i=0;i<the_dialog.header.numstates;i++)
  {
    if(the_dialog.dlgstates[i].trindex>transidx)
    {
      the_dialog.dlgstates[i].trindex--;
    }
  }
  return 0;
}

void CDialogEdit::OnRemove()
{
  CString tmpstr, text;
  int key, transkey;
  HTREEITEM ht; //papa node
  int numtrans;
  int ret;

  the_dialog.changed=1;
  key=0;
  if(m_treestates.Lookup(m_currentselection,key) )
  {
    numtrans=the_dialog.dlgstates[key].trnumber;
    if(numtrans>1) tmpstr.Format("Do you want to remove state %d and its %d replies?\n(Any orphaned children state will become externally referenced)",key,numtrans);
    else tmpstr.Format("Do you want to remove state %d and its reply?\n(Any orphaned child state will become externally referenced)",key);
    if(MessageBox(tmpstr,"Dialog editor",MB_YESNO)!=IDYES) return; //bailing out
    ret=remove_state(key);//this will remove the state's transitions too
    switch(ret)
    {
    case -3:
      MessageBox("Failed to remove state due to memory shortage!","Dialog editor",MB_OK);
      break;
    case 0:
      break;
    default:
      MessageBox("Failed to remove state due to data inconsistency!","Dialog editor",MB_OK);
      break;
    }
    key=0;
  }
  else
  {
    ht=m_dialogtree.GetParentItem(m_currentselection); //get parent state
    if(m_treestates.Lookup(ht,key))
    {
      if(the_dialog.dlgstates[key].trnumber<2)
      {
        tmpstr.Format("This is the last reply, you have to remove the parent state (#%d).",key);
        MessageBox(tmpstr,"Dialog editor",MB_OK);
        return;
      }
      if(m_transstates.Lookup(m_currentselection,transkey)) //get transition
      {
        ret=remove_transition(key, transkey);
        switch(ret)
        {
        case -3:
          MessageBox("Failed to remove transition due to memory shortage!","Dialog editor",MB_OK);
          break;
        case 0:
          break;
        default:
          MessageBox("Failed to remove transition due to data inconsistency!","Dialog editor",MB_OK);
          break;
        }
      }
    }
    else
    {
      if(m_transstates.Lookup(ht,key))
      {
        the_dialog.dlgtrans[key].flags|=LEAF_NODE;
        memset(the_dialog.dlgtrans[key].nextdlg,0,8);
        the_dialog.dlgtrans[key].stateindex=0;
        ht=m_dialogtree.GetParentItem(ht); //get parent state
        key=0;
        m_treestates.Lookup(ht,key);
      }
    }
  }
  m_activesection=FL_TEXT;
  RefreshDialog(key,1);
  UpdateData(UD_DISPLAY);
}

int swap_transitions(int transidx)
{
  dlg_trans tmp;

  if(the_dialog.transcount<=transidx+1) return -1;
  tmp=the_dialog.dlgtrans[transidx];
  the_dialog.dlgtrans[transidx]=the_dialog.dlgtrans[transidx+1];
  the_dialog.dlgtrans[transidx+1]=tmp;
  return 0;
}

int add_transition(int stateidx, int transidx)
{
  dlg_trans *newtrans;
  int i;

  the_dialog.transcount++;
  newtrans=new dlg_trans[the_dialog.transcount];
  if(!newtrans)
  {
    the_dialog.transcount--;
    return -3;
  }
  memcpy(newtrans,the_dialog.dlgtrans,transidx*sizeof(dlg_trans) );
  memcpy(newtrans+transidx+1,the_dialog.dlgtrans+transidx,(the_dialog.transcount-transidx-1)*sizeof(dlg_trans) );
  memset(newtrans+transidx,0,sizeof(dlg_trans) );
  newtrans[transidx].flags=LEAF_NODE;
  newtrans[transidx].actionindex=-1;
  newtrans[transidx].trtrindex=-1;
  delete [] the_dialog.dlgtrans;
  the_dialog.dlgtrans=newtrans;
  for(i=0;i<the_dialog.header.numstates;i++)
  {
    if(i==stateidx)
    {
      the_dialog.dlgstates[i].trnumber++;
    }
    else
    {
      if(the_dialog.dlgstates[i].trindex>=transidx) the_dialog.dlgstates[i].trindex++;
    }
  }
  the_dialog.header.numtrans=the_dialog.transcount;
  return 0;
}

int add_state()
{
  dlg_state *newstates;
  int stateidx;
  int ret;

  stateidx=the_dialog.statecount++;
  newstates=new dlg_state[the_dialog.statecount];
  if(!newstates)
  {
    the_dialog.statecount--;
    return -3;
  }
  memcpy(newstates,the_dialog.dlgstates,stateidx*sizeof(dlg_state));
  memset(newstates+stateidx,0,sizeof(dlg_state) );
  newstates[stateidx].actorstr=-1;
  newstates[stateidx].stindex=-1;
  newstates[stateidx].trindex=the_dialog.transcount;
  newstates[stateidx].trnumber=1;
  ret=add_transition(-1, the_dialog.transcount);
  if(ret)
  {
    the_dialog.statecount--;
    return ret;
  }
  delete [] the_dialog.dlgstates;
  the_dialog.dlgstates=newstates;
  the_dialog.header.numstates=the_dialog.statecount;
  return 0;
}

void CDialogEdit::OnAddtop() 
{
  int ret;

  the_dialog.changed=1;
  ret=add_state();
  switch(ret)
  {
  case -3:
    MessageBox("Failed to add state due to memory shortage!","Dialog editor",MB_OK);
    break;
  case 0:
    RefreshDialog(0);
    break;
  default:
    MessageBox("Failed to add state due to data inconsistency!","Dialog editor",MB_OK);
    break;
  }
}

void CDialogEdit::OnAddnode() //adds a new transition 
{
  int key;
  int ret;

  the_dialog.changed=1;
  m_strref=-1;
  UpdateData(UD_DISPLAY);
  //adding a transition to the selected state
  if(m_treestates.Lookup(m_currentselection,key) )
  {
    if(!the_dialog.dlgstates[key].trnumber)
    {
      MessageBox("Invalid state node, cannot add transition.","Dialog editor",MB_OK);
      return;
    }
    ret=add_transition(key, the_dialog.dlgstates[key].trindex+the_dialog.dlgstates[key].trnumber);
    switch(ret)
    {
    case -3:
      MessageBox("Failed to add transition due to memory shortage!","Dialog editor",MB_OK);
      break;
    case 0:
      RefreshDialog(key);
      break;
    default:
      MessageBox("Failed to add transition due to data inconsistency!","Dialog editor",MB_ICONEXCLAMATION|MB_OK);
      break;
    }
    return;
  }
  //adding a state (with one transition) to the selected transition (or a root state)
  key=-1;
  m_transstates.Lookup(m_currentselection,key);
  if((key>=0) && !(the_dialog.dlgtrans[key].flags&LEAF_NODE))
  {
    MessageBox("The current response already has a transition!","Dialog editor",MB_ICONINFORMATION|MB_OK);
    RefreshDialog(the_dialog.dlgtrans[key].stateindex);
    return;
  }   
  ret=add_state();
  switch(ret)
  {
  case -3:
    MessageBox("Failed to add state due to memory shortage!","Dialog editor",MB_ICONSTOP|MB_OK);
    break;
  case 0:
    if(key!=-1)
    {
      //linking the state to the transition
      the_dialog.dlgtrans[key].flags&=~LEAF_NODE; //it is not a leaf node anymore
      the_dialog.dlgtrans[key].stateindex=the_dialog.statecount-1; //the linked state is the last state
      StoreResref(SELF_REFERENCE,the_dialog.dlgtrans[key].nextdlg);//internal link by default
      RefreshDialog(the_dialog.dlgtrans[key].stateindex); //positioning on the new state
    }
    else
    {
      RefreshDialog(0);
    }
    break;
  default:
    MessageBox("Failed to add state due to data inconsistency!","Dialog editor",MB_OK);
    break;
  }
}

void CDialogEdit::OnConvert() //changes a transition link
{
  CDialogLink dlg;
  HTREEITEM parent;
  int transkey, statekey;

  parent=m_dialogtree.GetParentItem(m_currentselection);
  if(!m_treestates.Lookup(m_currentselection, statekey) &&
     !m_treestates.Lookup(parent, statekey) )
  {
    statekey=0;
  }
  if(!m_transstates.Lookup(parent,transkey) && 
     !m_transstates.Lookup(m_currentselection, transkey) )
  {
    return;
  }
  dlg.m_leafnode=!!(the_dialog.dlgtrans[transkey].flags&LEAF_NODE);
  dlg.m_state=the_dialog.dlgtrans[transkey].stateindex;
  RetrieveResref(dlg.m_dialogres,the_dialog.dlgtrans[transkey].nextdlg);
  if(dlg.DoModal()!=IDOK) return;
  if(dlg.m_leafnode)
  {
    dlg.m_state=0;
    dlg.m_dialogres="";
    the_dialog.dlgtrans[transkey].flags|=LEAF_NODE;
  }
  else
  {
    the_dialog.dlgtrans[transkey].flags&=~LEAF_NODE;
  }
  the_dialog.dlgtrans[transkey].stateindex=dlg.m_state;
  StoreResref(dlg.m_dialogres,the_dialog.dlgtrans[transkey].nextdlg);
  the_dialog.changed=1;
  RefreshDialog(statekey,1);
}

void CDialogEdit::OnSwapnodes() 
{
  int statekey, statekeyb, transkey;
  HTREEITEM parent;
  int ret;

  if(m_treestates.Lookup(m_currentselection,statekey) )
  {
    //swapping 2 states (non-trivial)
    if(the_dialog.dlgstates[statekey].stindex==-1)
    {      
      MessageBox("This is not a top level state, no need to swap it!","Dialog editor",MB_OK);
      return;
    }
    else
    {
      if(the_dialog.header.numsttrigger<2)
      {
        MessageBox("This is the only top level state, cannot swap it!","Dialog editor",MB_OK);
        return;
      }
      //find statekeyb (the next state)
      ret=-2;
      for(statekeyb=statekey+1;statekeyb<the_dialog.header.numstates;statekeyb++)
      {
        if(the_dialog.dlgstates[statekeyb].stindex!=-1)
        {
          ret=swap_states(statekey, statekeyb);
          break;
        }
      }
      if(ret==-2)
      {
        for(statekeyb=0;statekeyb<statekey;statekeyb++)
        {
          if(the_dialog.dlgstates[statekeyb].stindex!=-1)
          {
            ret=swap_states(statekey, statekeyb);
            break;
          }
        }
      }
    }
    switch(ret)
    {
    case -3:
      MessageBox("Failed to swap states due to memory shortage!","Dialog editor",MB_OK);
      break;
    case 0:
      break;
    default:
      MessageBox("Failed to swap states due to data inconsistency!","Dialog editor",MB_OK);
      break;
    }
    the_dialog.changed=1;
    RefreshDialog(statekey, 0);
    return;
  }
  if(m_transstates.Lookup(m_currentselection,transkey) )
  {
    parent=m_dialogtree.GetParentItem(m_currentselection);
    if(!parent) return; //heh, this isn't possible
    if(!m_treestates.Lookup(parent, statekey) )
    {
      return; //this is impossible too
    }
    if(the_dialog.dlgstates[statekey].trnumber<2)
    {
      MessageBox("This is the only transition on this branch, cannot swap it!","Dialog editor",MB_OK);
      return;
    }
    if(transkey+1==the_dialog.dlgstates[statekey].trindex+the_dialog.dlgstates[statekey].trnumber)
    {
      transkey--;
    }
    swap_transitions(transkey);
    the_dialog.changed=1;
    RefreshDialog(statekey, 1);
  }
}

void swap_state_triggers(int a, int b)
{
  int i;
  CString tmp;

  for(i=0;i<the_dialog.statecount;i++)
  {
    if(the_dialog.dlgstates[i].stindex==a) the_dialog.dlgstates[i].stindex=b;
    else if(the_dialog.dlgstates[i].stindex==b) the_dialog.dlgstates[i].stindex=a;
  }
  tmp=the_dialog.sttriggers[a];
  the_dialog.sttriggers[a]=the_dialog.sttriggers[b];
  the_dialog.sttriggers[b]=tmp;
}

void CDialogEdit::OnIncreaseweight() 
{
  int statekey;
  int sttrigger;

  if(m_treestates.Lookup(m_currentselection,statekey) )
  {
    sttrigger=the_dialog.dlgstates[statekey].stindex;
    if(sttrigger<1)
    {
      MessageBox("Can't increase the weight (it is non-top level or already the first top-level state)","Dialog editor",MB_OK);
      return;
    }
    swap_state_triggers(sttrigger-1,sttrigger);
    the_dialog.changed=1;
    RefreshDialog(statekey);
  }  	
}

int findtrigger(int sttrigger)
{
  int i;

  for(i=0;i<the_dialog.statecount;i++)
  {
    if(sttrigger==the_dialog.dlgstates[i].stindex) return i;
  }
  return -1;
}

void CDialogEdit::OnToolsorder() 
{
  int i,j;
  int trigger1, trigger2;
  int changed;

  changed=0;
	for(i=0;i<the_dialog.sttriggercount-1;i++)
  {
    trigger1=findtrigger(i);
    if(trigger1<0) abort();
    for(j=i+1;j<the_dialog.sttriggercount;j++)
    {
      trigger2=findtrigger(j);
      if(trigger2<0) abort();
      if(trigger1>trigger2)
      {
        swap_state_triggers(i,j);
        trigger1=trigger2;
        changed=1;
      }
    }
  }
  if(changed)
  {
    RefreshDialog(0);
    the_dialog.changed=1;
  }
  else
  {
    MessageBox("The top level states were already ordered by weight.","Dialog editor",MB_OK);
  }
}

void CDialogEdit::OnToolsorder2() 
{
  int i,j;
  unsigned int trigger1, trigger2;
  int changed;

	changed=0;
  for(i=0;i<the_dialog.statecount-1;i++)
  {
    //this hack will make -1 the biggest :)
    //so external refs will go to back
    trigger1=(unsigned int) the_dialog.dlgstates[i].stindex;
    if(trigger1==0xffffffff)
    {//not an external reference
      if(!m_treestates.FindElement(i)) continue;
    }
    for(j=i+1;j<the_dialog.statecount;j++)
    {
      trigger2=(unsigned int) the_dialog.dlgstates[j].stindex;
      if(trigger1>trigger2)
      {
        swap_states(i,j);
        trigger1=trigger2;
        changed=1;
      }
    }
  }	
  if(changed)
  {
    RefreshDialog(0);
  }
  else
  {
    MessageBox("The top level states were already ordered by weight.","Dialog editor",MB_OK);
  }
}

int CDialogEdit::pstize(int reference) 
{
  CString text;
  
  if(reference<1) return 0;
  text=resolve_tlk_text(reference);
  if(text.IsEmpty()) return 0;
  if(text.Find("\"")==-1)
  {
    text="\""+text+"\"";
    store_tlk_text(reference,text);
    return 1;
  }
  return 0;
}

void CDialogEdit::OnToolspst() 
{
  int i;
  int changed;

  changed=0;
	for(i=0;i<the_dialog.statecount;i++)
  {
    changed=pstize(the_dialog.dlgstates[i].actorstr)|changed;
  }
  if(changed)
  {
    RefreshDialog(0);
  }
  else
  {
    MessageBox("The dialog was already formatted.","Dialog editor",MB_OK);
  }
}

void CDialogEdit::OnToolsIdsbrowser() 
{
	CIDSEdit dlg;
  CString tmpstr;

  dlg.SetReadOnly(true);
  tmpstr=itemname;
  itemname=m_idsname;
  dlg.DoModal();
  m_idsname=itemname;
  itemname=tmpstr;
}

void CDialogEdit::OnExportWeidu() 
{
	CString syscommand;
  CString tmpstr;
  int ret;

  if(weidupath.IsEmpty())
  {
    MessageBox("Please set up WeiDU before use.","Dialog editor",MB_OK|MB_ICONSTOP);
    return;
  }
  if(!file_exists(weidupath) )
  {
    MessageBox("WeiDU executable not found.","Dialog editor",MB_OK|MB_ICONSTOP);
    return;
  }
  chdir(bgfolder);
  mkdir(weidudecompiled);
  if(!dir_exists(weidudecompiled) )
  {
    tmpstr.Format("%s cannot be created as output path.",weidudecompiled);
      MessageBox(tmpstr,"Dialog editor",MB_OK|MB_ICONSTOP);
    return;
  }
  syscommand=AssembleWeiduCommandLine(itemname+".dlg", weidudecompiled); //export
	ret=RunWeidu(syscommand);
}

int CDialogEdit::RunWeidu(CString syscommand)
{
  CString tmpstr;
  int res;

  //flushing dialog.tlk
  if(global_changed[0]==true)
  {
    ((CChitemDlg *) AfxGetMainWnd())->write_file_progress(0); 
  }
  if(global_changed[1]==true)
  {
    ((CChitemDlg *) AfxGetMainWnd())->write_file_progress(1); 
  }
  unlink(WEIDU_LOG);
  res=my_system(syscommand);
  if(weiduflg&WEI_LOGGING)
  {
    if(file_exists(WEIDU_LOG) )
    {
      CWeiduLog dlg;
      dlg.DoModal();
    }
    else MessageBox("No logfile has been created by WeiDU.","Dialog editor",MB_OK);
  }
  if(res)
  {
    tmpstr.Format("'%s' returned: %d",syscommand, res);
    MessageBox(tmpstr,"Dialog Editor",MB_OK|MB_ICONSTOP);
  }
  return res;
}

void CDialogEdit::OnImportWeidu() 
{
  CString tmpstr;
	CString syscommand;
  CString filepath;
  int fhandle;
  int res;
  
  res=OFN_FILEMUSTEXIST|OFN_ENABLESIZING|OFN_EXPLORER;
  if(readonly) res|=OFN_READONLY;
  CFileDialog m_getfiledlg(TRUE, "d", makeitemname(".d",0), res, szFilterWeidu);

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
    itemname=m_getfiledlg.GetFileTitle(); //itemname moved here because the loader relies on it
    itemname.MakeUpper();
    if(weidupath.IsEmpty())
    {
      MessageBox("Please set up WeiDU before use.","Dialog editor",MB_OK|MB_ICONSTOP);
      return;
    }
    if(!file_exists(weidupath) )
    {
      MessageBox("WeiDU executable not found.","Dialog editor",MB_OK|MB_ICONSTOP);
      return;
    }
    chdir(bgfolder);
    //this one writes back the tlk file so the import will be consistent
    syscommand=AssembleWeiduCommandLine(filepath,"override");
    close(fhandle); //close file before calling weidu
  	res=RunWeidu(syscommand);

    ((CChitemDlg *) AfxGetMainWnd())->rescan_dialog(true);
    ((CChitemDlg *) AfxGetMainWnd())->scan_override();
    filepath=bgfolder+"override\\"+itemname+".dlg";
    fhandle=open(filepath, O_RDONLY|O_BINARY);
    if(!fhandle)
    {
      MessageBox("Cannot open file!","Error",MB_OK);
      goto restart;
    }
    res=the_dialog.ReadDialogFromFile(fhandle,-1);
    close(fhandle);
    switch(res)
    {
    case -3:
      MessageBox("Dialog loaded with errors.","Warning",MB_ICONEXCLAMATION|MB_OK);
      break;
    case 1:
      if(!pst_compatible_var() && !has_xpvar())
      {
        MessageBox("Old dialog version (freeze flags were unavailable)","Warning",MB_ICONEXCLAMATION|MB_OK);     
      }
    case 0:
      break;
    default:
      MessageBox("Cannot read dialog!","Error",MB_ICONEXCLAMATION|MB_OK);
      NewDialog();
      break;
    }
  }
  m_activesection=FL_TEXT;
  RefreshDialog(0);
  UpdateData(UD_DISPLAY);
}

void CDialogEdit::OnNewvalue() 
{
	m_strref=-1;
  m_text=resolve_tlk_text(m_strref);
  RefreshLeaf();
  UpdateData(UD_DISPLAY);	
}

void CDialogEdit::OnExportTbg() 
{
	ExportTBG(this, REF_DLG);
}

void CDialogEdit::OnCancel() 
{
	if(the_dialog.changed)
  {
    if(MessageBox("Do you want to leave the editor without save?","Dialog editor",MB_YESNO)!=IDYES)
    {
      return;
    }
  }
	CDialog::OnCancel();
}

static int rightbuttons[]={IDC_VERSION, IDC_HASOPTION, IDC_HASCONDITION, IDC_HASACTION, 
IDC_HASJOURNAL, IDC_JOURTYPE, IDC_STATIC2, IDC_STATIC6, IDC_STATIC4, IDC_STATIC5, IDC_SOUND, 0};

static int right2buttons[]={IDC_FLAGS, IDC_OPTION, IDC_CONDITION, IDC_ACTION, 
IDC_JOURNAL, IDC_DIALOG, IDC_VALUE, IDC_STATIC3, IDC_STRREF, 0};

static int bottombuttons[]={IDC_LOAD, IDC_LOADEX, IDC_SAVEAS, IDC_NEW, IDC_CHECK, IDCANCEL,0};

void CDialogEdit::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	int i;
	CRect rect;
	CWnd *cntrl;

	//dialog tree widens
	cntrl = GetDlgItem(IDC_DIALOGTREE);
	if(cntrl && IsWindow(cntrl->m_hWnd))
	{
		cntrl->GetWindowRect(rect);
		ScreenToClient(rect);
		cntrl->SetWindowPos(NULL,0,0,cx-200,rect.Height(),SWP_NOZORDER|SWP_NOMOVE|SWP_NOACTIVATE);
	}

	//text window enlarges
	cntrl = GetDlgItem(IDC_TEXT);
	if(cntrl && IsWindow(cntrl->m_hWnd))
	{
		cntrl->SetWindowPos(NULL,0,0,cx-64,cy-352,SWP_NOZORDER|SWP_NOMOVE|SWP_NOACTIVATE);
		CheckTriggerOrAction(false);
	}

	//side buttons move right
	i=0;
	while(rightbuttons[i])
	{
		cntrl = GetDlgItem(rightbuttons[i]);
		if(cntrl && IsWindow(cntrl->m_hWnd))
		{
			cntrl->GetWindowRect(rect);
			ScreenToClient(rect);
			cntrl->SetWindowPos(NULL,cx-180,rect.top,0,0,SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE);
		}
		i++;
	}

	i=0;
	while(right2buttons[i])
	{
		cntrl = GetDlgItem(right2buttons[i]);
		if(cntrl && IsWindow(cntrl->m_hWnd))
		{
			cntrl->GetWindowRect(rect);
			ScreenToClient(rect);
			cntrl->SetWindowPos(NULL,cx-85,rect.top,0,0,SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE);
		}
		i++;
	}

	//menu buttons move down
	i=0;
	while(bottombuttons[i])
	{
		cntrl = GetDlgItem(bottombuttons[i]);
		if(cntrl && IsWindow(cntrl->m_hWnd))
		{
			cntrl->GetWindowRect(rect);
			ScreenToClient(rect);
			cntrl->SetWindowPos(NULL,rect.left,cy-32,0,0,SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE);
		}
		i++;
	}
}

void CDialogEdit::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	CDialog::OnGetMinMaxInfo(lpMMI);
	lpMMI->ptMinTrackSize.x=700;
	lpMMI->ptMinTrackSize.y=500;
	lpMMI->ptMaxTrackSize.y=900;
}

BOOL CDialogEdit::PreTranslateMessage(MSG* pMsg) 
{
  m_tooltip.RelayEvent(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}

