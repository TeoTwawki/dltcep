// KeyEdit.cpp : implementation file
//

#include "stdafx.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <direct.h>

#include "chitem.h"
#include "chitemDlg.h"
#include "options.h"
#include "KeyEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define LP_DUPLO   0x80000000   //duplicate entry
#define LP_MARKED  0x40000000   //marked for delete
#define LP_MASK    0x3fffffff   //index

/////////////////////////////////////////////////////////////////////////////
// CKeyEdit dialog

CKeyEdit::CKeyEdit(CWnd* pParent /*=NULL*/)	: CDialog(CKeyEdit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CKeyEdit)
	m_bifpos = _T("");
	m_filepos = _T("");
	//}}AFX_DATA_INIT
  m_bifindex=0xffff;
  m_biflocation=-1;
  m_bifsize=-1;
}

void CKeyEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CKeyEdit)
	DDX_Control(pDX, IDC_FILELIST, m_filelist_control);
	DDX_Control(pDX, IDC_BIFLIST, m_biflist_control);
	DDX_Text(pDX, IDC_BIFPOS, m_bifpos);
	DDX_Text(pDX, IDC_FILEPOS, m_filepos);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CKeyEdit, CDialog)
	//{{AFX_MSG_MAP(CKeyEdit)
	ON_LBN_SELCHANGE(IDC_BIFLIST, OnSelchangeBiflist)
	ON_NOTIFY(NM_CLICK, IDC_FILELIST, OnClickFilelist)
	ON_BN_CLICKED(IDC_DELETEALL, OnDeleteall)
	ON_BN_CLICKED(IDC_REMOVEBIF, OnRemovebif)
	ON_BN_CLICKED(IDC_MARK, OnMark)
	ON_BN_CLICKED(IDC_FLAG1, OnFlag1)
	ON_BN_CLICKED(IDC_FLAG2, OnFlag2)
	ON_BN_CLICKED(IDC_FLAG3, OnFlag3)
	ON_BN_CLICKED(IDC_FLAG4, OnFlag4)
	ON_BN_CLICKED(IDC_FLAG5, OnFlag5)
	ON_BN_CLICKED(IDC_FLAG6, OnFlag6)
	ON_BN_CLICKED(IDC_FLAG7, OnFlag7)
	ON_BN_CLICKED(IDC_FLAG8, OnFlag8)
	ON_BN_CLICKED(IDC_READD, OnReadd)
	ON_COMMAND(ID_RELOAD, OnReload)
	ON_COMMAND(ID_TOOLS_EXPLODE, OnToolsExplode)
	ON_COMMAND(ID_TOOLS_IMPLODE, OnToolsImplode)
	ON_COMMAND(ID_CHECK, OnCheck)
	//}}AFX_MSG_MAP
  ON_NOTIFY(NM_CUSTOMDRAW, IDC_FILELIST, OnCustomdrawFilelist)  
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKeyEdit message handlers

void CKeyEdit::OnCustomdrawFilelist(NMHDR *pNMHDR, LRESULT *pResult)
{
  NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( pNMHDR );
  
  // Take the default processing unless we set this to something else below.
  *pResult = CDRF_DODEFAULT;
  
  // First thing - check the draw stage. If it's the control's prepaint
  // stage, then tell Windows we want messages for every item.
  
  if ( CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage )
  {
    *pResult = CDRF_NOTIFYITEMDRAW;
  }
  else if ( CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage )
  {
    // This is the prepaint stage for an item. Here's where we set the
    // item's text color. Our return value will tell Windows to draw the
    // item itself, but it will use the new color we set here.
    // We'll cycle the colors through red, green, and light blue.
    
    COLORREF crText;
    
    if(pLVCD->nmcd.lItemlParam&LP_DUPLO)
    {
      crText = RGB(240,120,120);
    }
    else
    {
      crText = RGB(255,255,255);
    }
    
    // Store the color back in the NMLVCUSTOMDRAW struct.
    pLVCD->clrTextBk = crText;
    
    // Tell Windows to paint the control itself.
    *pResult = CDRF_DODEFAULT;
  }
}

void CKeyEdit::OnSelchangeBiflist() 
{
  int pos;

  pos=m_biflist_control.GetCurSel();
	m_bifindex=m_biflist_control.GetItemData(pos);
  if(m_bifindex!=0xffff)
  {
    m_biflocation=bifs[m_bifindex].location;
    m_bifsize=bifs[m_bifindex].biflen;
  }
  else
  {
    m_biflocation=-1;
    m_bifsize=-1;
  }
	RefreshDialog();
}
/*
void CKeyEdit::OnSelchangeFilelist() 
{
  POSITION pos;
  int idx;

  pos=m_filelist_control.GetFirstSelectedItemPosition();
  idx=m_filelist_control.GetNextSelectedItem(pos);
  m_filepos.Format("%d/%d (0x%0x)",m_filelist_control.GetItemCount(),idx,m_filelist_control.GetItemData(idx) );
}
*/
int CKeyEdit::GetCurSel()
{
  POSITION pos;

  pos=m_filelist_control.GetFirstSelectedItemPosition();
  return m_filelist_control.GetNextSelectedItem(pos);
}

void CKeyEdit::refresh_list(int list_deleted)
{
  int tmpfhandle;
  CString ref, ext;
  CString tmpstr;
  loc_entry fileloc;
  stringloc_entry stringfileloc;
  int filepos;
  POSITION pos;
  int i, tisres;

  m_filelist_control.DeleteAllItems();
  tisres=determinetype(REF_TIS);
  for(i=1;i<=NUM_OBJTYPE;i++)
  {
    ext=objexts[i];
    pos=resources[i]->GetStartPosition();
    while(pos)
    {
      resources[i]->GetNextAssoc(pos,ref,fileloc);
      if(fileloc.index<0)
      {
        continue;
      }
      if(fileloc.bifindex==m_bifindex)
      {
        if(fileloc.bifname.IsEmpty() && !list_deleted)
        {
          continue;
        }
        filepos=m_filelist_control.InsertItem(m_filelist_control.GetItemCount(),ref+ext);
        if(list_deleted)
        {
          if(fileloc.size==-1)
          {
            tmpfhandle=locate_file(fileloc,LF_IGNOREOVERRIDE);
            if(tmpfhandle) close(tmpfhandle);
          }        
          tmpstr.Format("%7d ",fileloc.size);
          m_filelist_control.SetItemText(filepos,1,tmpstr);
          if(fileloc.bifname.IsEmpty() )
          {
            m_filelist_control.SetItemText(filepos,2,"X");
            m_filelist_control.SetItemData(filepos,LP_MARKED|fileloc.index);
          }
          else
          {
            m_filelist_control.SetItemText(filepos,2," ");
            m_filelist_control.SetItemData(filepos,fileloc.index);
          }
        }
        else
        {
          m_filelist_control.SetItemData(filepos,fileloc.index);
        }
      }
    }
    pos=duplicates[i]->GetHeadPosition();
    while(pos)
    {
      stringfileloc=duplicates[i]->GetNext(pos);
      if(stringfileloc.entry.index<0)
      {
        continue;
      }
      if(stringfileloc.entry.bifindex==m_bifindex)
      {
        if(fileloc.bifname.IsEmpty() && !list_deleted)
        {
          continue;
        }
        filepos=m_filelist_control.InsertItem(m_filelist_control.GetItemCount(),stringfileloc.ref+ext);
        if(list_deleted)
        {
          if(stringfileloc.entry.size==-1)
          {
            tmpfhandle=locate_file(stringfileloc.entry,LF_IGNOREOVERRIDE);
            if(tmpfhandle) close(tmpfhandle);
          }
          tmpstr.Format("%7d ",stringfileloc.entry.size);
          m_filelist_control.SetItemText(filepos,1,tmpstr);
          if(fileloc.bifname.IsEmpty() )
          {
            //marked for removal
            m_filelist_control.SetItemText(filepos,2,"X");
            m_filelist_control.SetItemData(filepos,LP_DUPLO|LP_MARKED|stringfileloc.entry.index);
          }
          else
          {
            m_filelist_control.SetItemText(filepos,2," ");
            m_filelist_control.SetItemData(filepos,LP_DUPLO|stringfileloc.entry.index); 
          }
        }
        else
        {
          m_filelist_control.SetItemData(filepos,stringfileloc.entry.index); 
        }
      }
    }
  }
}

void CKeyEdit::RefreshDialog()
{
  int i,j;
  int tmppos;
  CButton *cb;  

  tmppos=m_biflocation!=-1;
  j=1;
  for(i=0;i<8;i++)
  {
    cb=(CButton *) GetDlgItem(IDC_FLAG1+i);
    if(tmppos)
    {
      cb->EnableWindow(true);
      cb->SetCheck(m_biflocation&j);
    }
    else
    {
      cb->EnableWindow(false);
      cb->SetCheck(0);
    }
    j<<=1;
  }
  tmppos=GetCurSel();
  refresh_list(1); //list deleted entries
  if(tmppos<0) tmppos=0;
  m_filelist_control.SetSelectionMark(tmppos);
//  m_filelist_control.SetCurSel(tmppos);
  if(m_bifindex==0xffff) m_bifpos.Format("-/%d",key_headerinfo.numbif);
  else m_bifpos.Format("%d/%d",m_bifindex,key_headerinfo.numbif);
  UpdateData(UD_DISPLAY);
}

void CKeyEdit::RefreshBiflist()
{
  int pos;
  int i;

  m_biflist_control.ResetContent();
  pos=m_biflist_control.AddString("<unlisted>");
  m_biflist_control.SetItemData(pos,0xffff);
	for(i=0;i<key_headerinfo.numbif;i++)
  {
    pos=m_biflist_control.AddString(bifs[i].bifname);
    m_biflist_control.SetItemData(pos,i);
  }
  m_biflist_control.SetCurSel(0);
  OnSelchangeBiflist();
}

BOOL CKeyEdit::OnInitDialog() 
{
	CDialog::OnInitDialog();

  m_filelist_control.SetExtendedStyle(LVS_EX_FULLROWSELECT);
  m_filelist_control.InsertColumn(0,"Filename",LVCFMT_LEFT,12*10);
  m_filelist_control.InsertColumn(1,"Size   ",LVCFMT_RIGHT,7*8);
  m_filelist_control.InsertColumn(2,"Remove",LVCFMT_LEFT,6*9);
  //tooltips
  {
    m_tooltip.Create(this,TTS_NOPREFIX);
    m_tooltip.SetMaxTipWidth(200);
    m_tooltip.SetTipBkColor(RGB(240,224,160));
    
    m_tooltip.AddTool(GetDlgItem(IDCANCEL), IDS_CANCEL);
    m_tooltip.AddTool(GetDlgItem(IDOK), IDS_SAVEBACK2);
    m_tooltip.AddTool(GetDlgItem(IDC_BIFLIST), IDS_BIFLIST);
    m_tooltip.AddTool(GetDlgItem(IDC_FILELIST), IDS_FILELIST);
  }
  RefreshBiflist();
	return TRUE; 
}

BOOL CKeyEdit::PreTranslateMessage(MSG* pMsg) 
{
  m_tooltip.RelayEvent(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}

int CKeyEdit::WriteChitin(int fhandle)
{
  CString tmpstr;
  int numref;
  int numbif;
  int i;
  int resloc;
  int bifnamesize;
  int ret;
  int esize;
  unsigned long offset;
  key_entry keyentry;
  keybif_entry *bifentry=NULL;
  char *bifnamedump=NULL;

  ret=0;
  numref=0;
  offset=sizeof(key_headerinfo);
  key_headerinfo.offbif=offset;
  offset+=key_headerinfo.numbif*sizeof(keybif_entry);
  bifentry=new keybif_entry[key_headerinfo.numbif];
  if(!bifentry)
  {
    ret=-3;
    goto endofquest;
  }
  //calculating bif name entries
  bifnamesize=0;
  for(numbif=0;numbif<key_headerinfo.numbif;numbif++)
  {
    bifentry[numbif].biflen=file_length(bgfolder+bifs[numbif].bifname);
    if(bifentry[numbif].biflen==-1)
    {
      tmpstr.Format("%s bif doesn't exist, do you want to continue?", bifs[numbif].bifname);
      if(MessageBox(tmpstr, "Key editor",MB_YESNO)==IDNO)
      {
        ret=-1;
        goto endofquest;
      }
      bifentry[numbif].biflen=bifs[numbif].biflen;
    }
    bifentry[numbif].location=bifs[numbif].location;
    bifentry[numbif].namelen=(short) (bifs[numbif].bifname.GetLength()+1);//zero terminating
    bifentry[numbif].offbif=offset;
    offset+=bifentry[numbif].namelen; 
    bifnamesize+=bifentry[numbif].namelen;
  }  
  key_headerinfo.offref=offset;
  bifnamedump=new char [bifnamesize];
  if(!bifnamedump)
  {
    ret=-3;
    goto endofquest;
  }
  esize=0;
  for(numbif=0;numbif<key_headerinfo.numbif;numbif++)
  {
    strcpy(bifnamedump+esize,bifs[numbif].bifname);
    esize+=bifentry[numbif].namelen;
  }

  if(write(fhandle,&key_headerinfo,sizeof(key_headerinfo))!=sizeof(key_headerinfo))
  {
    ret=-2;
    goto endofquest;
  }
  esize=numbif*sizeof(keybif_entry);
  if(write(fhandle,bifentry,esize)!=esize)
  {
    ret=-2;
    goto endofquest;
  }
  if(write(fhandle,bifnamedump,bifnamesize)!=bifnamesize)
  {
    ret=-2;
    goto endofquest;
  }
  delete [] bifentry;
  bifentry=NULL;
  delete [] bifnamedump;
  bifnamedump=NULL;

  //write out bif entries
  //the entries are ordered by bif index
  offset=0;
  for(numbif=0;numbif<key_headerinfo.numbif;numbif++)
  {
    m_bifindex=numbif; //small hack, refresh_list uses m_bifindex
    refresh_list(0);   //gathering items needed to be put in this bif
    
    esize=m_filelist_control.GetItemCount();
    offset+=esize;
    for(i=0;i<esize;i++)
    {
      tmpstr=m_filelist_control.GetItemText(i,0);
      resloc=m_filelist_control.GetItemData(i)&LP_MASK;
      keyentry.restype=objrefs[determinemenu(tmpstr)];
      if(!keyentry.restype)
      {
        MessageBox("Invalid resource type","Key editor",MB_ICONWARNING|MB_OK);
      }
      tmpstr=tmpstr.Left(tmpstr.GetLength()-4); //remove extension
      StoreResref(tmpstr,keyentry.filename);
      keyentry.residx=(numbif<<20)|resloc;
      if(write(fhandle,&keyentry,sizeof(keyentry))!=sizeof(keyentry) )
      {
        ret=-2;
        goto endofquest;
      }
    }
  }
  if(key_headerinfo.numref!=offset)
  {
    key_headerinfo.numref=offset;
    lseek(fhandle,0,SEEK_SET);
    if(write(fhandle,&key_headerinfo,sizeof(key_headerinfo))!=sizeof(key_headerinfo))
    {
      ret=-2;
      goto endofquest;
    }
  }
endofquest:
  if(bifentry) delete [] bifentry;
  if(bifnamedump) delete [] bifnamedump;
  return ret;
}

void CKeyEdit::AddResource(int type, int bifidx, int locidx, CString ref)
{
  CString ext;
  loc_entry fileloc;

  ext=objexts[type];
  fileloc.bifname=bifs[bifidx].bifname;
  fileloc.index=locidx;
  fileloc.size=-1;
  fileloc.bifindex=(unsigned short) bifidx;
  fileloc.cdloc=0;
  fileloc.resref=ref+ext;
  resources[type]->SetAt(ref,fileloc);
  key_headerinfo.numref++;
}

int CKeyEdit::write_biff(int fhandle, int bifidx, CString folder)
{
  _finddata_t fdata;
  CString key, tmp, bifname;
  bif_header header;
  bif_entry bifentry;
  tis_entry tisentry;
  tis_header tisheader;
  loc_entry loc;
  int i;
  int finput, dirhandle;
  int size;

  bifentry.unused=0;
  tisentry.unused=0;

  bifname=bifs[bifidx].bifname+"@";
  bifname.Replace("\\","_");
  bifname.Replace("/","_");
  bifname.Replace(".bif@","");
  folder += "\\" + bifname;
  
  memcpy(header.signature,"BIFF",4);
  memcpy(header.version,"V1  ",4);
  header.file_offset = sizeof(header);  
  header.file_entries = 0;
  header.tile_entries = 0;

  //getting the entry counts
  dirhandle=_findfirst(folder+"\\*.*",&fdata);
  if(dirhandle<1)
  {
    return -1;
  }
  do
  {
    if(fdata.attrib&_A_SUBDIR) continue;
    tmp=fdata.name;
    i=tmp.Find('.');
    if(i>8) continue;   //skipping invalid resrefs
    key=tmp.Left(i);
    i=determinemenu(tmp);
    if(!i) continue;
    if(objrefs[i]==REF_TIS) header.tile_entries++;
    else header.file_entries++;
  }
  while(!_findnext(dirhandle, &fdata));
  _findclose(dirhandle);
  write(fhandle,&header,sizeof(header));
  bifentry.offset=header.file_offset + header.file_entries * sizeof(bif_entry) + 
    header.tile_entries * sizeof(tis_entry);
  header.file_entries=0;
  header.tile_entries=0;

  //getting the bif entries
  dirhandle=_findfirst(folder+"\\*.*",&fdata);
  if(dirhandle<1)
  {
    return -1;
  }
  do
  {
    if(fdata.attrib&_A_SUBDIR) continue;
    tmp=fdata.name;
    i=tmp.Find('.');
    if(i>8) continue;   //skipping invalid resrefs
    key=tmp.Left(i);
    i=determinemenu(tmp);
    if(!i) continue;
    if(objrefs[i]==REF_TIS) continue;

    bifentry.restype=objrefs[i];
    bifentry.res_loc=header.file_entries++;
    if(header.file_entries&TIS_IDX_MASK)
    {
      tmp.Format("Too many files in biff: %s", bifname);
      MessageBox(tmp,"Key editor",MB_OK);
      _findclose(fhandle);
      return -1;
    }
    finput=open(folder+"\\"+tmp,O_RDONLY|O_BINARY);
    if(finput>0)
    {
      AddResource(i, bifidx, bifentry.res_loc, key);
      bifentry.size=filelength(finput);
      close(finput);
      write(fhandle, &bifentry, sizeof(bif_entry));
      bifentry.offset+=bifentry.size;
    }
  }
  while(!_findnext(dirhandle, &fdata));
  _findclose(dirhandle);

  tisentry.offset=bifentry.offset;
  //getting the tis entries
  dirhandle=_findfirst(folder+"\\*.*",&fdata);
  if(dirhandle<1)
  {
    return -1;
  }
  do
  {
    if(fdata.attrib&_A_SUBDIR) continue;
    tmp=fdata.name;
    i=tmp.Find('.');
    if(i>8) continue;   //skipping invalid resrefs
    key=tmp.Left(i);
    i=determinemenu(tmp);
    if(!i) continue;
    if(objrefs[i]!=REF_TIS) continue;

    tisentry.restype=objrefs[i];
    header.tile_entries++; //we need to increase it first, then shift left by 14
    if(header.tile_entries>63)
    {
      tmp.Format("Too many tilesets in biff: %s", bifname);
      MessageBox(tmp,"Key editor",MB_OK);
      _findclose(fhandle);
      return -1;
    }
    tisentry.res_loc=header.tile_entries<<14;
    finput=open(folder+"\\"+tmp,O_RDONLY|O_BINARY);
    if(finput>0)
    {
      AddResource(i, bifidx, tisentry.res_loc, key);
      read(finput,&tisheader, sizeof(tisheader));
      tisentry.numtiles=tisheader.numtiles;
      tisentry.size=filelength(finput);
      close(finput);
      write(fhandle, &tisentry, sizeof(tis_entry));
      tisentry.offset+=tisentry.size;
    }
  }
  while(!_findnext(dirhandle, &fdata));
  _findclose(dirhandle);

  //write the normal files
  dirhandle=_findfirst(folder+"\\*.*",&fdata);
  if(dirhandle<1)
  {
    return -1;
  }
  do
  {
    if(fdata.attrib&_A_SUBDIR) continue;
    tmp=fdata.name;
    i=tmp.Find('.');
    if(i>8) continue;   //skipping invalid resrefs
    key=tmp.Left(i);
    i=determinemenu(tmp);
    if(!i) continue;
    if(objrefs[i]==REF_TIS) continue;
    finput=open(folder+"\\"+tmp,O_RDONLY|O_BINARY);
    if(finput>0)
    {
      size=filelength(finput);
      copy_file(finput,fhandle,size,0);
      close(finput);
    }
  }
  while(!_findnext(dirhandle, &fdata));
  _findclose(dirhandle);

  //write the tileset files
  dirhandle=_findfirst(folder+"\\*.*",&fdata);
  if(dirhandle<1)
  {
    return -1;
  }
  do
  {
    if(fdata.attrib&_A_SUBDIR) continue;
    tmp=fdata.name;
    i=tmp.Find('.');
    if(i>8) continue;   //skipping invalid resrefs
    key=tmp.Left(i);
    i=determinemenu(tmp);
    if(!i) continue;
    if(objrefs[i]!=REF_TIS) continue;
    finput=open(folder+"\\"+tmp,O_RDONLY|O_BINARY);
    if(finput>0)
    {
      lseek(finput,sizeof(tis_header),SEEK_SET);
      size=filelength(finput)-sizeof(tis_header);
      copy_file(finput,fhandle,size,0);
      close(finput);
    }
  }
  while(!_findnext(dirhandle, &fdata));
  _findclose(dirhandle);

  return 0;
}

int CKeyEdit::has_change(int /*bifidx*/)
{
  return 0;
}

void CKeyEdit::OnOK() 
{
  CString oldname, newname, tmpstr;
  int i;
  int fhandle;
  int ret;

	// here we make the scheduled changes on bifs
  ret=0;
	for(i=0;i<key_headerinfo.numbif;i++)
  {
    if(has_change(i))
    {
      newname=bgfolder+bifs[i].bifname;
      oldname=bgfolder+bifs[i].bifname+".bak";
      unlink(oldname);
      rename(newname,oldname);
      fhandle=open(newname,O_RDWR|O_BINARY|O_CREAT|O_TRUNC,S_IREAD|S_IWRITE);
      if(fhandle>0)
      {
//        ret=WriteBiff(fhandle,i);
        close(fhandle);
      }
      else ret=-2;
    }
    
    //handle return value
    if(ret)
    {

      ret=0;
    }
  }

  //here we dump the final chitin.key
  //must be after changing bifs because the keyfile contains the bif length
  oldname=bgfolder+"chitin.bak";
  newname=bgfolder+"chitin.key";
  unlink(oldname);
  rename(newname,oldname);
  fhandle=open(newname,O_RDWR|O_BINARY|O_CREAT|O_TRUNC,S_IREAD|S_IWRITE);
  if(fhandle>0)
  {
    ret=WriteChitin(fhandle);
    close(fhandle);
  }
  else ret=-2;
  //handle return value
  if(ret)
  {
    switch(ret)
    {
    case -1:
      tmpstr="Aborted...";
      break;
    case -2:
      tmpstr="Cannot write file";
      break;
    case -3:
      tmpstr="Out of memory";
      break;
    }
    MessageBox(tmpstr,"Key editor",MB_ICONSTOP|MB_OK);
    unlink(newname);
    rename(oldname,newname);
  }

	CDialog::OnOK();
}

void CKeyEdit::OnClickFilelist(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
{
  POSITION pos;
  int idx;

  *pResult = 0;
  pos=m_filelist_control.GetFirstSelectedItemPosition();
  if(pos)
  {
    idx=m_filelist_control.GetNextSelectedItem(pos);
    m_filepos.Format("%d/%d (0x%0x)",idx,m_filelist_control.GetItemCount(),m_filelist_control.GetItemData(idx) );
    UpdateData(UD_DISPLAY);
  }
}

void CKeyEdit::OnDeleteall() 
{
  int i;

	for(i=1;i<=NUM_OBJTYPE;i++)
  {
    duplicates[i]->RemoveAll();
  }	
  RefreshDialog();
}

void CKeyEdit::AdjustBifindex(int index, int value)
{
  int i;

  if(index==0xffff) return;
  for(i=1;i<=NUM_OBJTYPE;i++)
  {
    resources[i]->Fixup(index,value);
    duplicates[i]->Fixup(index,value);
  }
}

void CKeyEdit::OnRemovebif() 
{
  CString itemname;
  unsigned long itemdata;
  int residx;
  int i,j;
  membif_entry *newbifs;
  loc_entry fileloc;

  for(i=0;i<m_filelist_control.GetItemCount();i++)
  {
    itemdata=m_filelist_control.GetItemData(i);
    itemname=m_filelist_control.GetItemText(i,0);
    residx=determinemenu(itemname);
    itemname=itemname.Left(itemname.GetLength()-4); //remove extension
    if(residx<0)
    {
      //this couldn't happen at the moment
      //it could happen only when adding arbitrary files from override
      MessageBox("Invalid resource type","Key editor",MB_ICONWARNING|MB_OK);
      continue;
    }
    key_headerinfo.numref--;
    if(itemdata&LP_DUPLO)
    {
      duplicates[residx]->RemoveKey(itemname);
    }
    else
    {
      //there was a duplicate which now becomes primary entry
      if(duplicates[residx]->Lookup(itemname,fileloc))
      {
        resources[residx]->SetAt(itemname,fileloc);
        duplicates[residx]->RemoveKey(itemname); //removing the first duplicate from the list
      }
      else //there was no duplicate, this entry now becomes extinct
      {
        resources[residx]->RemoveKey(itemname);
      }      
    }
  }  
  if((m_bifindex!=0xffff) && (m_bifindex<key_headerinfo.numbif) )
  {
    newbifs=new membif_entry[key_headerinfo.numbif];
    if(!newbifs)
    {
      MessageBox("Not enough memory.","Error",MB_OK);
      return;
    }
    
    j=0;
    for(i=0;i<key_headerinfo.numbif;i++)
    {
      if(i!=m_bifindex)
      {
        newbifs[j]=bifs[i];
        j++;
      }
    }
    key_headerinfo.numbif--;
    delete [] bifs;
    bifs=newbifs;
    AdjustBifindex(m_bifindex,-1);
  }
  RefreshBiflist();
}

void CKeyEdit::OnMark() 
{
  CString itemname;
  POSITION pos;
  int itempos;
  int residx;
  unsigned long itemdata;
  loc_entry fileloc;
  int ret;
  
  pos=m_filelist_control.GetFirstSelectedItemPosition();
  while(pos)
  {
    itempos=m_filelist_control.GetNextSelectedItem(pos);
    itemdata=m_filelist_control.GetItemData(itempos);
    //determine the resource type
    itemname=m_filelist_control.GetItemText(itempos,0);
    residx=determinemenu(itemname);
    itemname=itemname.Left(itemname.GetLength()-4); //remove extension
    if(residx<0)
    {
      //this couldn't happen at the moment
      //it could happen only when adding arbitrary files from override
      MessageBox("Invalid resource type","Key editor",MB_ICONWARNING|MB_OK);
      return;
    }

    if(itemdata&LP_DUPLO)
    {
      ret=duplicates[residx]->Lookup(itemname, fileloc);
    }
    else
    {
      ret=resources[residx]->Lookup(itemname,fileloc);
    }
    if(!ret)
    {
      MessageBox("Internal error","Key editor",MB_ICONSTOP|MB_OK);
      return;
    }
    if(fileloc.bifname.IsEmpty())
    {
      if(m_bifindex>=0)
      {
        fileloc.bifname=bifs[m_bifindex].bifname;
        m_filelist_control.SetItemData(itempos,itemdata&~LP_MARKED);
      }
      else
      {
        MessageBox("This entry didn't belong to any bif, cannot bring it back.","Key editor",MB_ICONWARNING|MB_OK);
      }
    }
    else
    {
      fileloc.bifname.Empty();
      m_filelist_control.SetItemData(itempos,itemdata|LP_MARKED);
    }
    if(itemdata&LP_DUPLO)
    {
      duplicates[residx]->SetAt(itemname, fileloc);
    }
    else
    {
      resources[residx]->SetAt(itemname,fileloc);
    }
  }
  RefreshDialog();
}

void CKeyEdit::Clicked(int mask)
{
  if(m_bifindex>=0)
  {
    m_biflocation=bifs[m_bifindex].location^=mask;
  }
}

void CKeyEdit::OnFlag1() 
{
	Clicked(1);	
}

void CKeyEdit::OnFlag2() 
{
	Clicked(2);
}

void CKeyEdit::OnFlag3() 
{
	Clicked(4);
}

void CKeyEdit::OnFlag4() 
{
	Clicked(8);
}

void CKeyEdit::OnFlag5() 
{
	Clicked(16);
}

void CKeyEdit::OnFlag6() 
{
	Clicked(32);
}

void CKeyEdit::OnFlag7() 
{
	Clicked(64);
}

void CKeyEdit::OnFlag8() 
{
	Clicked(128);
}

void CKeyEdit::OnReadd() 
{
  membif_entry mybif;
  membif_entry *newbifs;
  key_header mykey_headerinfo;
  keybif_entry bifentry;
  key_entry resentry;
  loc_entry fileloc;
  CString keypath, bifpath, ref, ext;
  char *poi;
  int flg;
  int fhandle;
  long bifidx, mybifidx, locidx;
  int tmpofs;
  int numref;
  int i;
  int type;
  int duplo;

  duplo=!(chkflg&NODUPLO);
  MessageBox("To readd a BIF you must have an old chitin.key and the BIF file","Key editor",MB_OK|MB_ICONINFORMATION);
  flg=OFN_FILEMUSTEXIST|OFN_ENABLESIZING|OFN_EXPLORER|OFN_READONLY;
  {
    CFileDialog m_getfiledlg(TRUE, NULL, bgfolder+"chitin.key", flg, szFilterKey);
    if( m_getfiledlg.DoModal() != IDOK )
    {
      return;
    }
    keypath=m_getfiledlg.GetPathName();
  }

  {
    CFileDialog m_getfiledlg(TRUE, NULL, bgfolder+"*.bif", flg, szFilterBif);
    if( m_getfiledlg.DoModal() != IDOK )
    {
      return;
    }
    bifpath=m_getfiledlg.GetPathName();
  }
  if(bgfolder.CompareNoCase(bifpath.Left(bgfolder.GetLength())) )
  {
    MessageBox(bifpath+ " must be in the current game folder.","Key editor",MB_ICONSTOP|MB_OK);
    return;
  }
  bifpath=bifpath.Mid(bgfolder.GetLength());
  for(i=0;i<key_headerinfo.numbif;i++)
  {
    if(!bifpath.CompareNoCase(bifs[i].bifname) )
    {
      MessageBox(bifpath +" is already included in the current chitin.key","Key editor",MB_ICONSTOP|MB_OK);
      return;
    }
  }

  fhandle=open(keypath, O_RDONLY|O_BINARY);
  if(fhandle<1)
  {
    MessageBox("No "+keypath+" found.","Key editor",MB_ICONEXCLAMATION|MB_OK);
    return;
  }
  if(read(fhandle,&mykey_headerinfo,sizeof(mykey_headerinfo) )!=sizeof(mykey_headerinfo) )
  {
    MessageBox(keypath+" is corrupt, checking only the override folder","Key editor",MB_ICONEXCLAMATION|MB_OK);
    close(fhandle);
    return;
  }
  if(memcmp(mykey_headerinfo.signature,"KEY ",4) )
  {
    MessageBox(keypath+" is corrupt, checking only the override folder","Key editor",MB_ICONEXCLAMATION|MB_OK);
    close(fhandle);
    return;
  }
  //reading bifnames
  if(lseek(fhandle,mykey_headerinfo.offbif,SEEK_SET)!=mykey_headerinfo.offbif)
  {
    MessageBox(keypath+" is corrupt, checking only the override folder","Key editor",MB_ICONEXCLAMATION|MB_OK);
    close(fhandle);
    return;
  }
  
  for(mybifidx=0;mybifidx<mykey_headerinfo.numbif;mybifidx++)
  {
    read(fhandle,&bifentry,sizeof(bifentry) );
    tmpofs=tell(fhandle); //jumping to bif string
    lseek(fhandle,bifentry.offbif,SEEK_SET);
    mybif.bifname.Empty();
    poi=mybif.bifname.GetBufferSetLength(bifentry.namelen);
    read(fhandle,poi,bifentry.namelen);
    mybif.bifname.ReleaseBuffer();
    mybif.biflen=bifentry.biflen;
    mybif.location=bifentry.location;
    lseek(fhandle,tmpofs,SEEK_SET);
    if(!mybif.bifname.CompareNoCase(bifpath)) break;
  }
  if(mybifidx==mykey_headerinfo.numbif)
  {
    MessageBox("Couldn't find the BIF in the old chitin.key, cannot readd it.","Key editor",MB_ICONSTOP|MB_OK);
    close(fhandle);
    return; //can't find bif in old chitin.key
  }

  //reading itm's
  if(lseek(fhandle,mykey_headerinfo.offref,SEEK_SET)!=mykey_headerinfo.offref)
  {
    MessageBox(keypath+" is corrupt, checking only the override folder.","Key editor",MB_ICONEXCLAMATION|MB_OK);
    close(fhandle);
    return;
  }
  i=0;
  //
  numref=mykey_headerinfo.numref;
  while(numref--)
  {
    read(fhandle,&resentry,sizeof(resentry) );
    RetrieveResref(ref,resentry.filename);
    
    bifidx=(long) (resentry.residx>>20);    //highest 12 bits
    if(bifidx!=mybifidx)
    {
      continue; //not the bif we are looking for
    }
    locidx=(long) (resentry.residx&0xfffff); //lowest 20 bits
    type=determinetype(resentry.restype);
    if(!type)
    {
      continue;
    }
    ext=objexts[type];
    keypath.LoadString(idstrings[type]);
    if(duplo && resources[type]->Lookup(ref,fileloc) )
    {
      duplicates[type]->AddList(ref,fileloc); //old duplicated entries
    }
    if(bifidx>=mykey_headerinfo.numbif)
    {
      continue;
    }
    fileloc.bifname=mybif.bifname;
    fileloc.index=locidx;
    fileloc.size=-1;
    fileloc.bifindex=(unsigned short) bifidx;
    fileloc.cdloc=0;
    fileloc.resref=ref+ext;
    //another cheesy, fishy hack for the different path of sounds
    if(!fileloc.bifname.CompareNoCase("data\\desound.bif") )
    {
      if(file_exists(bgfolder+"\\sounds\\"+ref+".wav") )
      {
        fileloc.index=-1;
        fileloc.bifname="\\sounds\\"+ref+".wav";
      }
    }
    resources[type]->SetAt(ref,fileloc);
  }
  close(fhandle);
  key_headerinfo.numbif++;
  newbifs=new membif_entry[key_headerinfo.numbif];
  if(!bifs) return;
  for(i=0;i<key_headerinfo.numbif-1;i++)
  {
    newbifs[i]=bifs[i];
  }
  newbifs[i]=mybif;
  delete [] bifs;
  bifs=newbifs;
  RefreshDialog();
}


void CKeyEdit::OnReload() 
{
  EndDialog(ID_RELOAD);
}

void CKeyEdit::do_copy_file(CString dest, CString key, CString ext, loc_entry fileloc)
{
  CString filename, tmp;
  int fhandle, finput;
  int decrypt;
  int oflg;
  long size;

  if(editflg&IGNOREOVERRIDE) oflg=LF_IGNOREOVERRIDE;
  else oflg=0;
  finput=locate_file(fileloc, oflg); //don't ignore override
  if(finput<1)
  {
    return;
  }
  if(fileloc.bifindex==0xffff)
  {
    if(editflg&IGNOREOVERRIDE)
    {
      close(finput);
      return;
    }
    tmp="override";
  }
  else
  {
    tmp=bifs[fileloc.bifindex].bifname+"@";
    tmp.Replace("\\","_");
    tmp.Replace("/","_");
    tmp.Replace(".bif@","");
  }

  filename=dest+"\\"+tmp+"\\"+key+ext;
  oflg=O_BINARY|O_RDWR|O_TRUNC|O_CREAT|O_SEQUENTIAL;
  errno=0;
  fhandle=open(filename,oflg,S_IWRITE|S_IREAD);
  if(fhandle<1)
  {
    MessageBox("Can't create file: "+filename,"Error",MB_ICONSTOP|MB_OK);
    close(finput);
    return; //error
  }
  if(ext==".tis")
  {
    if(!write_tis_header(fhandle, fileloc))
    {
      MessageBox("Write error.","Error",MB_ICONSTOP|MB_OK);
      return;
    }
  }
  if(ext==".2da" || ext==".ids") decrypt=1;
  else decrypt=0;
  size=fileloc.size;
  if(size==-1)
  {
    size=filelength(finput);
  }
  switch(copy_file(finput, fhandle, size, decrypt))
  {
  case 0:
    break;
  case -3:
    MessageBox("Out of memory","Error",MB_ICONSTOP|MB_OK);
    break;
  case -2:
    MessageBox("Read error.","Error",MB_ICONSTOP|MB_OK);
    break;
  case -1:
    MessageBox("Write error.","Error",MB_ICONSTOP|MB_OK);
    break;
  case -4:
    MessageBox("Incorrect file size.","Error",MB_ICONSTOP|MB_OK);
    break;
  default:
    MessageBox("Internal error.","Error",MB_ICONSTOP|MB_OK);
    break;
  }
  close(finput);
  close(fhandle);
}

int CKeyEdit::extract_files(CString dest, CString extension, CStringMapLocEntry &refs)
{
  loc_entry fileloc;
	POSITION pos;
  CString key;
  int i=0;

  pos=refs.GetStartPosition();
  while(pos)
  {
    ((CChitemDlg *) AfxGetMainWnd())->set_progress(i++); 
    refs.GetNextAssoc(pos,key,fileloc);
    do_copy_file(dest, key, extension, fileloc);
  }
  return 0;
}

void CKeyEdit::OnToolsExplode() 
{
  CString tmp;
  folderbrowse_t fb;
  int i;
  int res;

  fb.initial=bgfolder;
  int len = fb.initial.GetLength()-1;
  if((len>0) && (fb.initial.GetAt(len)=='\\') )
  {
    fb.initial=fb.initial.Left(len);
  }
  fb.title="Select folder for the unpacked game data";
  if(BrowseForFolder(&fb,m_hWnd)!=IDOK) return;
  fb.initial+="\\"+setupname;
  if(mkdir(fb.initial) )
  {
    tmp.Format("Make sure the folder %s doesn't exist!", fb.initial);
    MessageBox(tmp,"Key editor",MB_ICONEXCLAMATION|MB_OK);
    return;
  }
  for(i=0;i<key_headerinfo.numbif;i++)
  {
    tmp=bifs[i].bifname+"@";
    tmp.Replace("\\","_");
    tmp.Replace("/","_");
    tmp.Replace(".bif@","");
    mkdir(fb.initial+"\\"+tmp);
  }
  if(!(editflg&IGNOREOVERRIDE))
  {
    mkdir(fb.initial+"\\override");
  }
  for(i=1;i<=NUM_OBJTYPE;i++)
  {    
    tmp.Format("Extracting files... (%s)", objexts[i]);
    ((CChitemDlg *) AfxGetMainWnd())->start_progress(resources[i]->GetCount(), tmp);
    res=extract_files(fb.initial, objexts[i], *resources[i])|res;  //1
    if(res==-1) break;
  }
  ((CChitemDlg *) AfxGetMainWnd())->end_progress();
}

void CKeyEdit::cleanup()
{
  int i;

  for(i=1;i<=NUM_OBJTYPE;i++)
  {
    resources[i]->RemoveAll(true);  //keep hash value
    duplicates[i]->RemoveAll();
  }

  key_headerinfo.numbif=0;
  key_headerinfo.numref=0;
  
  if(bifs)
  {
    delete [] bifs;
    bifs=NULL;
  }

}

void CKeyEdit::get_bifs(CString folder)
{
  _finddata_t fdata;
  CString filename;
  int fhandle;

  filename=folder+"\\*.*";
  //scanning override dir for loose items
  fhandle=_findfirst(filename,&fdata);
  if(fhandle<1)
  {
    return;
  }
  do
  {
    if(!(fdata.attrib&_A_SUBDIR)) continue;
    if(fdata.name[0]=='.') continue;   //skipping special dirs
    if(stricmp(fdata.name,"override")) //counting not override
    {
      key_headerinfo.numbif++;
    }
  }
  while(!_findnext(fhandle, &fdata));
  _findclose(fhandle);

  fhandle=_findfirst(filename,&fdata);
  if(fhandle<1)
  {//this shouldn't happen
    key_headerinfo.numbif=0;
    return;
  }
  bifs = new membif_entry[key_headerinfo.numbif];
  int count = 0;
  do
  {
    if(!(fdata.attrib&_A_SUBDIR)) continue;
    if(fdata.name[0]=='.') continue;   
    if(stricmp(fdata.name,"override")) 
    {
      if(count >= key_headerinfo.numbif)
      {
        key_headerinfo.numbif=0;
        delete [] bifs;
        bifs = NULL;
        break; //end with error
      }
      filename = fdata.name;
      filename.Replace("_","\\");
      bifs[count].location=1;
      bifs[count].biflen=0;  //will grow with data stuffed in
      bifs[count].bifname=filename+".bif";
      count++;
    }
  }
  while(!_findnext(fhandle, &fdata));
  _findclose(fhandle);  
}

void CKeyEdit::write_chitin(int fhandle)
{
  int i;
  keybif_entry tmp;
  key_entry tmp2;
  loc_entry loc;
  POSITION pos;
  CString key;

  key_headerinfo.offbif=sizeof(key_headerinfo);
  tmp.offbif = key_headerinfo.offbif + key_headerinfo.numbif * sizeof(keybif_entry);
  write(fhandle, &key_headerinfo, sizeof(key_headerinfo));
  for(i=0;i<key_headerinfo.numbif;i++)
  {
    tmp.biflen=bifs[i].biflen;
    tmp.location=1;
    tmp.namelen=(short) (bifs[i].bifname.GetLength()+1); //string length + zero terminator
    write(fhandle, &tmp, sizeof(keybif_entry));
    tmp.offbif+=tmp.namelen;
  }
  key_headerinfo.offref=tmp.offbif;
  for(i=0;i<key_headerinfo.numbif;i++)
  {
    write(fhandle, bifs[i].bifname, bifs[i].bifname.GetLength()+1); //zero terminator
  }
  //fix the offset
  lseek(fhandle,0,SEEK_SET);
  write(fhandle, &key_headerinfo, sizeof(key_headerinfo));
  lseek(fhandle,key_headerinfo.offref,SEEK_SET);

  for(i=1;i<=NUM_OBJTYPE;i++)
  {
    pos=resources[i]->GetStartPosition();
    while(pos)
    {
      resources[i]->GetNextAssoc(pos, key, loc);      
      strncpy(tmp2.filename,key,8);
      tmp2.restype = objrefs[i];      
      tmp2.residx = (loc.index  &0xfffff) | (loc.bifindex<<20);
      write(fhandle, &tmp2, sizeof(tmp2) );
    }
  }
}

void CKeyEdit::OnToolsImplode() 
{
  folderbrowse_t fb;
  int fhandle, bifhandle;
  CString filename;
  int i;

  fb.initial=bgfolder;
  int len = fb.initial.GetLength()-1;
  if((len>0) && (fb.initial.GetAt(len)=='\\') )
  {
    fb.initial=fb.initial.Left(len);
  }
  fb.title="Select the folder of game data to pack";
  if(BrowseForFolder(&fb,m_hWnd)!=IDOK) return;
  if(!dir_exists(fb.initial+"\\override"))
  {
    MessageBox("At least an empty override folder must exist in the selected folder.","Key editor", MB_OK);
    return;
  }
  fhandle = open(fb.initial+"\\chitin.key", O_RDWR|O_BINARY|O_EXCL|O_CREAT, S_IREAD|S_IWRITE);
  if(fhandle>0)
  {
    cleanup();
    get_bifs(fb.initial);
    if(key_headerinfo.numbif<1)
    {
      MessageBox("No directories of bifs!","Key editor",MB_OK);
      OnReload(); //reloading all crap
      return;
    }
    ((CChitemDlg *) AfxGetMainWnd())->start_progress(key_headerinfo.numbif, "Packing files...");

    for(i=0;i<key_headerinfo.numbif;i++)
    {
      ((CChitemDlg *) AfxGetMainWnd())->set_progress(i);
      filename=fb.initial+"\\"+bifs[i].bifname;
      assure_dir_exists(filename.Left(filename.ReverseFind('\\')) );
      bifhandle=open(filename,O_RDWR|O_BINARY|O_CREAT|O_TRUNC,S_IREAD|S_IWRITE);
      if(write_biff(bifhandle, i,fb.initial))
      {
        MessageBox("Can't create biff!","Key editor",MB_OK);
        close(bifhandle);
        close(fhandle);
        OnCancel();
        return;
      }
      bifs[i].biflen=filelength(bifhandle);
      close(bifhandle);
    }
    ((CChitemDlg *) AfxGetMainWnd())->end_progress();
    write_chitin(fhandle);
    close(fhandle);
    MessageBox("Chitin and biffs are created!","Key editor",MB_OK);
    OnCancel(); //we just now screwed the chitin internal structure, better to quit
    return;
  }
  else
  {
    MessageBox("The target file already exists, cannot create!","Key editor",MB_OK);
  }
}

void CKeyEdit::OnCheck() 
{
	// TODO: Add your command handler code here
	
}
