// 2da.cpp: implementation of the C2da class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <errno.h>
#include "2da.h"
#include "options.h"

//
//#include "ChitemDlg.h"
//
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CStringMapArray tooltipnumbers;
CStringMapCompiler action_data;
CStringMapCompiler trigger_data;

static int position=0;
static int xorflag=0;
static int maxlength=-1;

char external[MAXBUFFSIZE];

int mygetc(FILE *fpoi)
{
  int ch;

  ch=fgetc(fpoi);
  if((ch==EOF) && feof(fpoi))
  {
    return EOF;
  }
  if((ch==255) && !position)
  {      
    xorflag=1;
    fgetc(fpoi);
    ch=fgetc(fpoi);
    position=0;
    if(maxlength>=2) maxlength-=2;
  }
  if(xorflag)
  {
    ch^=xorblock[position&63];
  }
  position++;
  if(maxlength>=0)
  {
    if(position>maxlength) return EOF;
  }
  if(ch=='\t') ch=' '; //hack for allowing tabs
  return ch&255;
}

int skip_string(FILE *fpoi, char chr)
{
  int ch;

  do
  {
    ch=mygetc(fpoi);
    if(ch==EOF) return -1;
    if((char) ch!=chr)
    {
      position--;
      if(xorflag)
      {
        ungetc(ch^xorblock[position&63],fpoi); //stuffing it back into the can
      }
      else
      {
        ungetc(ch,fpoi); //stuffing it back into the can
      }
      break;
    }      
  }
  while(1);
  return 0;
}

void SetupReader(int max)
{
  xorflag=0;
  position=0;
  maxlength=max;
}

int read_string(FILE *fpoi, char *pattern, char *tmpbuff=NULL, int length=0)
{
  int i,j,k;
  char ch;

  i=0;
  k=0;
  while(pattern[i])
  {
    ch=(char) mygetc(fpoi);
    if(ch==EOF)
    {
      if(tmpbuff) tmpbuff[k]=0;
      if(k)
      { //incomplete last line
        return 2;
      }
      else return 0;
    }
    if(ch!=pattern[i])
    {
      for(j=0;j<i;j++)
      {
        if(tmpbuff)
        {
          tmpbuff[k++]=pattern[j];
          if(k>=length)
          {
            return 3; //buffer overflow
          }
        }
      }
      i=0;
      if(tmpbuff && ch!='\r')
      {
        tmpbuff[k++]=ch; //don't add \r we would remove it later anyway
        if(k>=length)
        {
          return 3; //buffer overflow
        }
      }
    }
    else i++;
  }
  if(tmpbuff) tmpbuff[k]=0;
  return 1;
}


int read_line_until(FILE *fpoi, CString line)
{
  int ret;

  do
  {
    ret = read_string(fpoi, "\n", external,sizeof(external)); //header
    //((CChitemDlg *) AfxGetMainWnd())->log("%s",external);
  }
  while((external!=line) && (ret==1));

  return ret;
}

//// reads an array of integers from ids (ordered)
int ReadIdsFromFile(int fhandle, CStringList &refs, int add, int length)
{
  char tmpref[MAXIDSIZE];
  CString ref;
  int cnt, prev;
  int ret;
  FILE *fpoi;

  if(length<0) maxlength=filelength(fhandle);
  else maxlength=length;
  fpoi=fdopen(fhandle,"rb");
  if(!fpoi)
  {
    close(fhandle);
    return -1;
  }
  switch(add)
  {
  case -1:
    break;
  case 0:
    read_string(fpoi, "\n"); //skipping crap
    break;
  case 1:
   if(!refs.GetCount())
   {
     refs.AddTail("Default");//0
   }
   read_string(fpoi, "\n"); //skipping crap
   break;
  case 2:
    if(!refs.GetCount())
    {
      refs.AddTail("Default");//0
      refs.AddTail("None");   //1
    }
    read_string(fpoi, "\n"); //skipping crap
    break;
  default:
    fclose(fpoi);
    return -2; //internal error
  }
  prev=refs.GetCount()-1;
  do
  {
    ret=read_string(fpoi, " ",tmpref,sizeof(tmpref));
    if(ret!=1) break;
    tmpref[MAXIDSIZE-1]=0;
    cnt=strtonum(tmpref);
    if (add==2) cnt++;
    ret=read_string(fpoi,"\n",tmpref,sizeof(tmpref));
    if(ret!=1 && ret!=2) break;
    tmpref[MAXIDSIZE-1]=0;
    if (cnt>prev)
    {
      while(cnt>prev)
      {
        prev++;
        if(cnt==prev)
        {
          ref=tmpref;
          ref.TrimRight();
          refs.AddTail(ref);
        }
        else
        {
          refs.AddTail("");
        }
      }
    }
    else
    {
      POSITION pos;

      ref=tmpref;
      ref.TrimRight();
      pos = refs.FindIndex(cnt);
      refs.SetAt(pos, ref);
    }
    if(cnt>5000) break; //cut it out at 5000!
  }
  while(1);
  fclose(fpoi);
  return 0;
}

int ReadSongIdsFromFile(int fhandle, CString2List &refs, int length)
{
  char tmpref[MAXIDSIZE];
  char musfile[MAXIDSIZE];
  CString2 ref;
  int i;
  int cnt, prev;
  int ret;
  FILE *fpoi;
  FILE *fpoi2;

  if(length<0) maxlength=filelength(fhandle);
  else maxlength=length;
  fpoi2=fopen(bgfolder+"/music/songlist.txt","rb"); //pst style songlist
  fpoi=fdopen(fhandle,"rb");
  if(!fpoi)
  {
    close(fhandle);
    return -1;
  }
  read_string(fpoi, "\n"); //skipping crap
  prev=-1;
  do
  {
    ret=read_string(fpoi, " ",tmpref,sizeof(tmpref));
    if(ret!=1) break;
    tmpref[MAXIDSIZE-1]=0;
    cnt=strtonum(tmpref);
    ret=read_string(fpoi,"\n",tmpref,sizeof(tmpref));
    if(ret!=1 && ret!=2) break;
    tmpref[MAXIDSIZE-1]=0;
    while(cnt>prev)
    {
      fgets(musfile,MAXIDSIZE,fpoi2);
      for(i=0;i<MAXIDSIZE;i++) if(musfile[i]==' ') musfile[i]=0;
      ref.b=musfile;
      ref.b.MakeUpper();
      if(ref.b.Right(4)==".MUS") ref.b=ref.b.Left(ref.b.GetLength()-4);
      prev++;      
      if(cnt==prev)
      {
        ref.a=tmpref;
        ref.a.TrimRight();
      }
      else
      {
        ref.a.Empty();
      }
      refs.AddTail(ref);
    }
    if(cnt>2000) break; //cut it out at 2000!
  }
  while(1);
  fclose(fpoi);
  return 0;
}

//pst song ids are separated from filenames
int ReadSongIds(loc_entry entry, CString2List &refs)
{
  int fhandle;

  refs.RemoveAll();
  xorflag=position=0;
  fhandle=locate_file(entry, 0);
  if(fhandle<1) return -1;
  return ReadSongIdsFromFile(fhandle, refs, entry.size);
}

int ReadIds(loc_entry entry, CStringList &refs, int add, bool clear)
{
  int fhandle;

  if (clear)
  {
    refs.RemoveAll();
  }
  xorflag=position=0;
  fhandle=locate_file(entry, 0);
  if(fhandle<1) return -1;
  return ReadIdsFromFile(fhandle, refs, add, entry.size);
}

//// reads an array of integers from ids (ordered), fix number
int ReadIdsFromFile2(int fhandle, CString *refs, int maxindex, int mask, int length)
{
  char tmpref[MAXIDSIZE];
  CString ref;
  int cnt;
  int ret;
  FILE *fpoi;

  for(cnt=0;cnt<maxindex;cnt++) refs[cnt].Format("Unknown (%d)",cnt);
  if(length<0) maxlength=filelength(fhandle);
  else maxlength=length;
  fpoi=fdopen(fhandle,"rb");
  if(!fpoi)
  {
    close(fhandle);
    return -1;
  }
  if(mask)
  { //skipping crap, don't skip for slots.ids
    read_string(fpoi, "\n");
  }
  do
  {
    ret=read_string(fpoi, " ",tmpref,sizeof(tmpref));
    if(ret!=1) break;
    tmpref[MAXIDSIZE-1]=0;
    cnt=strtonum(tmpref);
    if(mask) cnt&=mask;
    ret=read_string(fpoi, "\n",tmpref,sizeof(tmpref));
    if(!ret) break;
    tmpref[MAXIDSIZE-1]=0;
    if(cnt>=maxindex) break;
    ref=tmpref;
    ref.TrimLeft();
    ref.TrimRight();
    refs[cnt]=ref;
  }
  while(1);
  fclose(fpoi);
  return 0;
}

int ReadIds2(loc_entry entry, CString *refs, int maxindex)
{
  int fhandle;

  xorflag=position=0;
  fhandle=locate_file(entry, 0);
  if(fhandle<1) return -1;
  return ReadIdsFromFile2(fhandle, refs, maxindex, 0, entry.size);
}

//reads an array of compiler definitions (trigger or action)
int ReadIdsFromFile3(int fhandle, CString *refs, int maxindex, int length, CStringMapCompiler &atdata, int trigger_or_action)
{
  POSITION pos;
  char tmpref[MAXIDSIZE];
  CString ref;
  compiler_data compiler_data;
  int cnt;
  int ret;
  FILE *fpoi;

  pos=atdata.GetStartPosition();
  while(pos)
  {
    atdata.GetNextAssoc(pos,ref,compiler_data);
    delete [] compiler_data.parameters;
  }
  atdata.RemoveAll();
  atdata.InitHashTable(get_hash_size(maxindex));
  for(cnt=0;cnt<maxindex;cnt++) refs[cnt].Format("Unknown (%d)",cnt);
  if(length<0) maxlength=filelength(fhandle);
  else maxlength=length;
  fpoi=fdopen(fhandle,"rb");
  if(!fpoi)
  {
    close(fhandle);
    return -1;
  }
  read_string(fpoi, "\n");
  ret=0;
  do
  {
    ret=read_string(fpoi, " ",tmpref,sizeof(tmpref));
    if(ret!=1)
    {
      break;
    }
    tmpref[MAXIDSIZE-1]=0;
    cnt=strtonum(tmpref);
    ret=read_string(fpoi, "\n",tmpref,sizeof(tmpref));
    if(ret!=1 && ret!=2)
    {
      break;
    }
    tmpref[MAXIDSIZE-1]=0;
    ref=tmpref;
    ref.TrimLeft();
    ref.TrimRight();
    //ref.MakeUpper();
    ret=add_compiler_data(ref, cnt, atdata, trigger_or_action);
    if(ret)
    {
      break;
    }
    refs[cnt%maxindex]=tmpref;
  }
  while(1);
  fclose(fpoi);
  return ret;
}

int ReadIds3(loc_entry entry, CString *refs, int maxindex, CStringMapCompiler &atdata, int trigger_or_action)
{
  int fhandle;

  xorflag=position=0;
  fhandle=locate_file(entry, 0);
  if(fhandle<1) return -1;
  return ReadIdsFromFile3(fhandle, refs, maxindex, entry.size, atdata, trigger_or_action);
}
//reads string mapped to int ids style
int ReadIdsFromFile4(int fhandle, CStringMapInt &refs, int length)
{
  char tmpref[MAXIDSIZE];
  CString ref;
  int cnt;
  int ret;
  FILE *fpoi;
  int p1;
  int maxindex;

  if(length<0) maxlength=filelength(fhandle);
  else maxlength=length;
  maxindex=maxlength/20;
  refs.RemoveAll();
  refs.InitHashTable(get_hash_size(maxindex));
  fpoi=fdopen(fhandle,"rb");
  if(!fpoi)
  {
    close(fhandle);
    return -1;
  }
  do
  {
    ret=read_string(fpoi, "\n",tmpref,sizeof(tmpref));
    if(ret<1) break;
    //skip invalid line
    if (memcmp(tmpref,"IDS",3)==0) continue;
    tmpref[MAXIDSIZE-1]=0;
    cnt=strtonum(tmpref);
    for(p1=0;(p1<MAXIDSIZE) && (tmpref[p1]!=' ') && tmpref[p1];p1++)
    {
      tmpref[p1]=' '; //hacking off number
    }
    tmpref[MAXIDSIZE-1]=0;
    ref=tmpref;
    ref.TrimLeft();
    ref.TrimRight();
    ref.MakeLower();
    if(!ref.IsEmpty())
    {
      refs[ref]=cnt;
    }
  }
  while(1);
  fclose(fpoi);
  return 0;
}

//another bloody hack, this function is called from inside another
//readids function, therefore it must save the ugly globals
//a wiser programmer would have created these functions as a class
int ReadIds4(loc_entry entry, CStringMapInt &refs)
{
  int fhandle;
  int ret;
  int sxor, spos, smax;

  fhandle=locate_file(entry, 0);
  if(fhandle<1) return -1;
  sxor=xorflag;
  spos=position;
  smax=maxlength;
  xorflag=position=0;
  ret=ReadIdsFromFile4(fhandle, refs, entry.size);
  xorflag=sxor;
  position=spos;
  maxlength=smax;
  return ret;
}

///////////////////////2da files
int Read2daFromFile(int fhandle, CStringList &refs, int length)
{
  char tmpref[MAXIDSIZE];
  CString ref;
  FILE *fpoi;

  refs.RemoveAll();
  if(length<0) maxlength=filelength(fhandle);
  else maxlength=length;
  fpoi=fdopen(fhandle,"rb");
  if(!fpoi)
  {
//    close(fhandle);
    return -1;
  }
  read_string(fpoi, "\n"); //skipping crap
  read_string(fpoi, "\n"); //skipping crap
  read_string(fpoi, "\n"); //skipping crap
  do
  {
    if(read_string(fpoi, " ",tmpref,sizeof(tmpref)) )
    {
      tmpref[MAXIDSIZE-1]=0;
      ref=tmpref;
      ref.TrimRight();
      ref.MakeUpper();
      if(!ref.IsEmpty())
      {
        refs.AddTail(ref);
      }
    }
  }
  while(read_string(fpoi,"\n")==1 );
  fclose(fpoi);
  return 0;
}

int Read2daIntFromFile(int fhandle, CStringMapInt &refs, int length)
{
  char tmpref[MAXIDSIZE];
  CString ref;
  FILE *fpoi;
  int ret, val;

  if(length<0) maxlength=filelength(fhandle);
  else maxlength=length;
  fpoi=fdopen(fhandle,"rb");
  if(!fpoi)
  {
    close(fhandle);
    return -1;
  }
  read_string(fpoi, "\n"); //skipping crap
  read_string(fpoi, "\n"); //skipping crap
  read_string(fpoi, "\n"); //skipping crap
  do
  {
    if(read_string(fpoi, " ",tmpref,sizeof(tmpref)) )
    {
      tmpref[MAXIDSIZE-1]=0;
      ref=tmpref;
      ret=read_string(fpoi,"\n", tmpref,sizeof(tmpref));
      if(!ret) break;
      tmpref[MAXIDSIZE-1]=0;
      val=atoi(tmpref);
      ref.MakeUpper();
      refs[ref]=val;
    }
    else break;
  }
  while(1);
  fclose(fpoi);
  return 0;
}

int Read2daInt(loc_entry entry, CStringMapInt &refs, int removeall)
{
  int fhandle;

  if(removeall) refs.RemoveAll();
  xorflag=position=0;
  fhandle=locate_file(entry, 0);
  if(fhandle<1) return -1;
  return Read2daIntFromFile(fhandle, refs, entry.size);
}

int Read2daResRefFromFile(int fhandle, CIntMapString &refs, int length, int column)
{
  char tmpref[MAXIDSIZE];
  CString ref;
  FILE *fpoi;
  int ret, val;

  if(length<0) maxlength=filelength(fhandle);
  else maxlength=length;
  fpoi=fdopen(fhandle,"rb");
  if(!fpoi)
  {
    close(fhandle);
    return -1;
  }
  read_string(fpoi, "\n"); //skipping crap
  read_string(fpoi, "\n"); //skipping crap
  read_string(fpoi, "\n"); //skipping crap
  do
  {
    if(read_string(fpoi, " ",tmpref,sizeof(tmpref)) )
    {
      tmpref[MAXIDSIZE-1]=0;
      val=atoi(tmpref);
      int c = column;
      while(c--)
      {
        if(!read_string(fpoi, " ",tmpref,sizeof(tmpref)))
        {
          fclose(fpoi);
          return -1;
        }
      }
      ret=read_string(fpoi,"\n", tmpref,sizeof(tmpref));
      if(!ret) break;
      tmpref[MAXIDSIZE-1]=0;
      ref=tmpref;
      ref.MakeUpper();
      refs[val]=ref;
    }
    else break;
  }
  while(1);
  fclose(fpoi);
  return 0;
}

int Read2daResRef(loc_entry entry, CIntMapString &refs, int removeall, int skipcolumn)
{
  int fhandle;

  if(removeall) refs.RemoveAll();
  xorflag=position=0;
  fhandle=locate_file(entry, 0);
  if(fhandle<1) return -1;
  return Read2daResRefFromFile(fhandle, refs, entry.size, skipcolumn);
}

int Read2daField(CString daname, int line, int column)
{
  char tmpref[MAXIDSIZE];
  loc_entry entry;
  CString *columns;
  int fhandle;
  FILE *fpoi;
  int defval;
  int count1, count2;
  
  if(line<0 || column<0) return -1;
  xorflag=position=0;
  if(!darefs.Lookup(daname,entry)) return -1;
  fhandle=locate_file(entry, 0);
  if(fhandle<1) return -1;
  if(entry.size<0) maxlength=filelength(fhandle);
  else maxlength=entry.size;
  fpoi=fdopen(fhandle,"rb");
  if(!fpoi)
  {
    close(fhandle);
    return -1;
  }
  read_string(fpoi, "\n"); //skipping crap
  read_string(fpoi, "\n", tmpref,sizeof(tmpref)); //default value
  defval=strtonum(tmpref);
  read_string(fpoi, "\n", tmpref,sizeof(tmpref)); //columns
  columns=explode(tmpref,' ',count1);
  if(columns)
  {
    delete [] columns;
    columns=0;
  }
  line++; //just to make it easier
  while(line--)
  {
    if(!read_string(fpoi, "\n",tmpref,sizeof(tmpref))) return defval; //line not found
  }
  columns=explode(tmpref,' ',count2);
  ///if((count1==count2) && (count1>column))
  if(count2>column)
  {
    defval=strtonum(columns[column]);
  }
  if(columns) delete [] columns;
  return defval;
}

int ReadWeapProfs(loc_entry entry)
{
  CString tmpweaprofs[NUM_WEAPROF];
  CStringMapArray tmpmap;
  CString label;
  tooltip_data value;
  POSITION pos;
  int i;

  Read2daArray(entry,tmpmap,1);
  pos=tmpmap.GetStartPosition();
  i=0;
  while(pos && (i<NUM_WEAPROF))
  {
    tmpmap.GetNextAssoc(pos, label, value);
    tmpweaprofs[i]=format_label(label);
    weaprofidx[i]=(value.data[0]<<16)|i;
    i++;
  }
  weaprofs[i]="Unknown";
  weaprofidx[i]=255;
  act_num_weaprof=i;
  ///order weaprofs in a very obscure way
  qsort(weaprofidx,act_num_weaprof,4,longsortb);
  for(i=0;i<act_num_weaprof;i++)
  {
    weaprofs[i]=tmpweaprofs[weaprofidx[i]&65535];
    weaprofidx[i]>>=16;
  }
  return false;
}

int Read2da(loc_entry entry, CStringList &refs)
{
  int fhandle;

  xorflag=position=0;
  fhandle=locate_file(entry, 0);
  if(fhandle<1) return -1;
  return Read2daFromFile(fhandle, refs, entry.size);
}

//2 strings (for songlist)
int Read2daStringFromFile(int fhandle, CString2List &refs, int length, int column)
{
  CString *tmparray;
  int count;
  CString2 ref, ref2;
  int cnt, prev;
  FILE *fpoi;
  int res, ret;

  ref2.a="";
  ref2.b="";
  refs.RemoveAll();
  if(length<0) maxlength=filelength(fhandle);
  else maxlength=length;
  fpoi=fdopen(fhandle,"rb");
  if(!fpoi)
  {
    close(fhandle);
    return -1;
  }// sprintf
  read_string(fpoi, "\n"); //skipping crap
  read_string(fpoi, "\n"); //skipping crap
  read_string(fpoi, "\n"); //skipping crap
  cnt=prev=-1;
  res=0;
  tmparray=NULL;
  do
  {
    ret=read_string(fpoi, "\n",external,sizeof(external));   
    //allow reading of last broken line
    if(ret==1 || ret==2)
    {
      if(column) cnt=strtonum(external);
      else cnt++;
      if(tmparray) delete [] tmparray;
      tmparray=explode(external,' ',count);
      if(!tmparray)
      {
        ret=2;
        break;
      }
      ref.a.Empty();
      ref.b.Empty();
      if(column<count)
      {
        ref.a=tmparray[column];
        ref.a.MakeUpper();
      }
      if(column+1<count)
      {
        ref.b=tmparray[column+1];
        ref.b.MakeUpper();
        if(ref.b.Right(4)==".MUS") ref.b=ref.b.Left(ref.b.GetLength()-4);
      }
      prev++;
      if(prev!=cnt) res=1;
      refs.AddTail(ref);
    }
  }
  while(ret==1);
  if(tmparray) delete [] tmparray;
  fclose(fpoi);
  return res;
}

int Read2daString(loc_entry entry, CString2List &refs, int startcolumn)
{
  int fhandle;

  xorflag=position=0;
  fhandle=locate_file(entry, 0);
  if(fhandle<1) return -1;
  return Read2daStringFromFile(fhandle, refs, entry.size, startcolumn);
}

int Read2daFromFile4(int fhandle, CStringList &refs, int length, int first)
{
  CString *args;
  CString ref, item;
  FILE *fpoi;
  int count;
  int i;

  refs.RemoveAll();
  if(length<0) maxlength=filelength(fhandle);
  else maxlength=length;
  fpoi=fdopen(fhandle,"rb");
  if(!fpoi)
  {
//    close(fhandle);
    return -1;
  }
  read_string(fpoi, "\n"); //skipping crap
  read_string(fpoi, "\n"); //skipping crap
  read_string(fpoi, "\n", external,sizeof(external)); //reading columns
  args=explode(external,' ',count);
  if(args)
  {
    for(i=first;i<count;i++)
    {
      item=args[i];
      item.MakeUpper();
      refs.AddTail(item);
    }
    delete [] args;
  }
  fclose(fpoi);
  return 0;
}

int Read2daRow(loc_entry entry, CStringList &refs, int first)
{
  int fhandle;

  xorflag=position=0;
  fhandle=locate_file(entry, 0);
  if(fhandle<1) return -1;
  return Read2daFromFile4(fhandle, refs, entry.size, first);
}

int Read2daColumnFromFile(int fhandle, CStringList &refs, int length, int column, bool caps)
{
  CString *args;
  CString ref, item;
  FILE *fpoi;
  int count;
  int ret;

  refs.RemoveAll();
  if(length<0) maxlength=filelength(fhandle);
  else maxlength=length;
  fpoi=fdopen(fhandle,"rb");
  if(!fpoi)
  {
//    close(fhandle);
    return -1;
  }
  read_string(fpoi, "\n"); //skipping crap
  read_string(fpoi, "\n"); //skipping crap
  read_string(fpoi, "\n"); //skipping crap
  do
  {
    ret=read_string(fpoi, "\n", external,sizeof(external)); //reading columns
    args=explode(external,' ',count);
    if(args)
    {
      if(count>column)
      {
        item=args[column];
        if(caps) item.MakeUpper();
        refs.AddTail(item);
        delete [] args;
      }
    }
  }
  while(ret==1);
  fclose(fpoi);
  return 0;
}

int Read2daColumn(loc_entry entry, CStringList &refs, int column, bool caps)
{
  int fhandle;

  xorflag=position=0;
  fhandle=locate_file(entry, 0);
  if(fhandle<1) return -1;
  return Read2daColumnFromFile(fhandle, refs, entry.size, column, caps);
}

//this is mostly for itemdial.2da (watch for the repeated key)
//CString defaultvalue ?
int Write2daMapToFile(CString daname, CStringMapInt &refs, CString columname)
{
  CString filename;
  CString key;
  int value;
  CString tmpstr;
  FILE *fpoi;
  int ret;
  POSITION pos;

  filename=bgfolder+CString("override\\")+daname+".2da";
  fpoi=fopen(filename,"wb");
  if(!fpoi) return -2;
  ret=WriteString(fpoi,"2DA V1.0\r\n-1\r\n");
  if(ret) return ret;
  pos=refs.GetStartPosition();
  tmpstr.Format("%-10s %s\r\n"," ",columname);
  ret=WriteString(fpoi, tmpstr);

  while(pos)
  {
    refs.GetNextAssoc(pos,key,value);
    tmpstr.Format("%-10s %d  %s\r\n",key,value,key);
    //don't write -1 back
    if(value!=-1) ret=WriteString(fpoi,tmpstr);
    if(ret) return ret;
  }
  fclose(fpoi);
  return 0;
}

//CString defaultvalue ?
int Write2daListToFile(CString daname, CStringList &refs, CString columname)
{
  CString filename;
  CString key;
  CString tmpstr;
  FILE *fpoi;
  int ret;
  POSITION pos;

  filename=bgfolder+CString("override\\")+daname+".2da";
  fpoi=fopen(filename,"wb");
  if(!fpoi) return -2;
  ret=WriteString(fpoi,"2DA V1.0\r\n0\r\n");
  if(ret) return ret;
  pos=refs.GetHeadPosition();
  tmpstr.Format("%-10s %s\r\n"," ",columname);
  ret=WriteString(fpoi, tmpstr);

  while(pos)
  {
    key=refs.GetNext(pos);
    tmpstr.Format("%-10s 1\r\n",key);
    ret=WriteString(fpoi,tmpstr);
    if(ret) return ret;
  }
  fclose(fpoi);
  return 0;
}

//reads a 2da with first column (index) and another numeric column (strref)
int Read2daFromFile2(int fhandle, CIntMapInt &refs, int length, int column)
{
  char tmpref[MAXIDSIZE];
  CString *row;
  CString ref;
  int item;
  int ret;
  int mycols;
  FILE *fpoi;
  int num;

  if(length<0) maxlength=filelength(fhandle);
  else maxlength=length;
  fpoi=fdopen(fhandle,"rb");
  if(!fpoi)
  {
    close(fhandle);
    return -1;
  }
  read_string(fpoi, "\n"); //skipping crap
  read_string(fpoi, "\n"); //skipping crap
  read_string(fpoi, "\n"); //skipping crap
  do
  {
    num=0;
    ret=read_string(fpoi, " ", tmpref,sizeof(tmpref));
    if(ret!=1) break;
    tmpref[MAXIDSIZE-1]=0;
    item=strtonum(tmpref);
    skip_string(fpoi,' ');
    ret=read_string(fpoi, "\n", tmpref,sizeof(tmpref));
    if(!ret) break;
    tmpref[MAXIDSIZE-1]=0;

    row=explode(tmpref,' ',mycols);
    if(column>=mycols)
    {
      ret|=1;
    }
    else
    {
      refs[item]=strtonum(row[column]);
    }
    if(row) delete [] row;
  }
  while(1);
  fclose(fpoi);
  return 0;
}

int Read2daStrref(loc_entry entry, CIntMapInt &refs, int column)
{
  int fhandle;

  refs.RemoveAll();
  xorflag=position=0;
  fhandle=locate_file(entry, 0);
  if(fhandle<1) return -1;
  return Read2daFromFile2(fhandle, refs, entry.size, column);
}

//////////////////
int Read2daFromFile3(int fhandle, CStringMapArray &refs, int length, int columns)
{
  char tmpref[MAXIDSIZE];
  CString ref, item;
  int cnt;
  int ret;
  FILE *fpoi;
  int num;
  int i;

  refs.RemoveAll();
  if(length<0) maxlength=filelength(fhandle);
  else maxlength=length;
  fpoi=fdopen(fhandle,"rb");
  if(!fpoi)
  {
    close(fhandle);
    return -1;
  }
  read_string(fpoi, "\n"); //skipping crap
  read_string(fpoi, "\n"); //skipping crap
  read_string(fpoi, "\n"); //skipping crap
  do
  {
    num=0;
    ret=read_string(fpoi, " ", tmpref,sizeof(tmpref));
    if(ret!=1) break;
    tmpref[MAXIDSIZE-1]=0;
    item=tmpref;
    item.MakeUpper();
    for(i=0;i<columns;i++)
    {
      skip_string(fpoi,' ');
      if(i==columns-1)
      {
        ret=read_string(fpoi, "\n", tmpref,sizeof(tmpref));
        if(ret==3)
        {
          read_string(fpoi,"\n");
          ret=1; //overflow        
        }
      }
      else ret=read_string(fpoi, " ", tmpref,sizeof(tmpref));
      if(ret!=1 && ret!=2) break;
      tmpref[MAXIDSIZE-1]=0;
      cnt=strtonum(tmpref);
      refs[item].data[i]=cnt;
    }
  }
  while(1);
  fclose(fpoi);
  return 0;
}

int Read2daArray(loc_entry entry, CStringMapArray &refs, int columns)
{
  int fhandle;

  xorflag=position=0;
  fhandle=locate_file(entry, 0);
  if(fhandle<1) return -1;
  return Read2daFromFile3(fhandle, refs, entry.size, columns);
}

//int defaultvalue
int Write2daArrayToFile(CString daname, CStringMapArray &refs, CString *columnames, int columns)
{
  CString filename;
  CString key;
  CString tmpstr;
  FILE *fpoi;
  int ret;
  POSITION pos;
  tooltip_data value;
  int i;
  int namelen;

  namelen=10;
  pos=refs.GetStartPosition();
  while(pos)
  {
    refs.GetNextAssoc(pos, key, value);
    if(namelen<key.GetLength() )
    {
      namelen=key.GetLength();
      if(namelen>30)
      {
        namelen=30;
        break;
      }
    }
  }
  filename=bgfolder+CString("override\\")+daname+".2da";
  fpoi=fopen(filename,"wb");
  if(!fpoi) return -2;
  ret=WriteString(fpoi,"2DA V1.0\r\n-1\r\n");
  if(ret) return ret;
  tmpstr.Format("%-*s",namelen," ");
  ret=WriteString(fpoi, tmpstr);

  for(i=0;i<columns;i++)
  {
    tmpstr.Format(" %-10s",columnames[i]);
    ret=WriteString(fpoi, tmpstr);
  }
  tmpstr="\r\n";
  ret=WriteString(fpoi, tmpstr);

  pos=refs.GetStartPosition();
  while(pos)
  {
    refs.GetNextAssoc(pos, key, value);
    tmpstr.Format("%-*s",namelen,key);
    ret=WriteString(fpoi,tmpstr);
    if(ret) return ret;
    for(i=0;i<columns;i++)
    {
      tmpstr.Format(" %-10d",value.data[i]);
      ret=WriteString(fpoi,tmpstr);
      if(ret) return ret;
    }
    tmpstr="\r\n";
    ret=WriteString(fpoi, tmpstr);
  }
  fclose(fpoi);
  return 0;
}

void ReadBeastIni(CStringList &beastnames, CStringList &beastkillvars)
{
  CString tmpstr;
  FILE *fpoi;
  int i;

  tmpstr.Format(bgfolder+"beast.ini");
	fpoi=fopen(tmpstr,"rt");
	if(!fpoi) return;
  beastnames.RemoveAll();
  beastkillvars.RemoveAll();
  memset(external,0,sizeof(external));
  while(!feof(fpoi))
  {
    external[0]=0;
    fgets(external,MAXBUFFSIZE-1,fpoi);
    for(i=0;i<MAXBUFFSIZE;i++)
    {
      if(isspace(external[i]) )
      {
        external[i]=0;
        break;
      }
    }

    if(!memicmp(external,"killvar=kill_",13) )
    {
      tmpstr=CString(external+13);
      tmpstr.MakeLower();
      if(!beastkillvars.Find(tmpstr) )
      {
        beastkillvars.AddTail(tmpstr);
      }
    }

    if(!memicmp(external,"name=",5))
    {      
      tmpstr=resolve_tlk_text(atoi(external+5),0);
      beastnames.AddTail(tmpstr);
    }
  }
  fclose(fpoi);
}

int read_until(char c, FILE *fpoi, CString &ret, int length=65535)
{
  int stop, act;
  char *poi;
  int chr;
  bool flg;

  stop=0;
  poi=ret.GetBuffer(act=MAXIDSIZE);
  flg=true;
  do
  {
    chr=mygetc(fpoi);
    if(chr==EOF) break;
    if(chr==c) break;
    if(flg && (chr=='\n'))
    {
      ungetc('\n',fpoi);
      chr='\r';
      flg=false;
    }
    else flg=true;
    if(stop<length)
    {
      poi[stop++]=(char) chr;
    }
    if(stop>act) {
      return -1;
    }
    if(stop==act )
    {
      ret.ReleaseBuffer(act);
      poi=ret.GetBuffer(act=act+MAXIDSIZE);
    }
  }
  while( stop<=length );
  ret.ReleaseBuffer(stop);
  return stop;
}

static CString validtypes="AIOPS0";

int add_compiler_data(CString prototype, int cnt, CStringMapCompiler &at_data, int trigger_or_action)
{
  CString name;
  CString parlist;
  CString subtype;
  compiler_data compiler_data;
  CString *params;
  parameter_data *parpoi;
  int ret;
  int j;
  int p1,p2;
  int objectcnt;
  int pointcnt;
  int intcnt;
  int actioncnt;
  int stringcnt;
  int chkflag, flg;

  p1=prototype.Find('(');
  if(p1<1)
  {
    return -1; //no name
  }
  p2=prototype.Find(')',p1);
  if(p2<0)
  {
    return -1;
  }
  name=prototype.Left(p1);
  prototype.MakeUpper();
  CString keyword = name;
  name.MakeLower();

  if(at_data.Lookup(name,compiler_data))
  {
    delete [] compiler_data.parameters;
  }
  compiler_data.keyword=keyword;
  compiler_data.parameters=NULL;
  compiler_data.parnum=0;
  compiler_data.opcode=cnt;
  
  //hardcoded information
  if(trigger_or_action) flg=chkflag=handle_trigger(cnt);
  else flg=chkflag=handle_action(cnt);
  //special cases
  //if((flg&0xffff)==0xff00) flg=0x105; 
  //if((flg&0xffff)==0xff01) flg=0x104; 

  compiler_data.parnumx=0;
  parlist=prototype.Mid(p1+1, prototype.GetLength()-p1-2);
  params=explode(parlist, ',', compiler_data.parnum);
  if(!params)
  {
    if(compiler_data.parnum)
    {
      return -1;
    }
  }
  else
  {
    compiler_data.parameters=new parameter_data[compiler_data.parnum];
    if(!compiler_data.parameters)
    {
      delete [] params;
      return -3;
    }
    objectcnt=0;
    pointcnt=0;
    intcnt=0;
    actioncnt=0;
    stringcnt=0;

    for(j=0;j<compiler_data.parnum;j++)
    {
      switch(params[j].GetAt(0))
      {
      case 'A': actioncnt++; break;
      case 'I': intcnt++; break;
      case 'P': pointcnt++; break;
      case 'O': objectcnt++; break;
      case 'S': stringcnt++; break;
      }
    }
    //calculate prototype code
    compiler_data.parnumx=actioncnt+intcnt+pointcnt+objectcnt+stringcnt*16;
    if(actioncnt>1 || intcnt>3 || pointcnt>1 || objectcnt>2 || stringcnt>4)
    {
      goto endofquest;
    }

    objectcnt=0;
    pointcnt=0;
    intcnt=0;
    actioncnt=0;
    stringcnt=0;

    for(j=0;j<compiler_data.parnum;j++)
    {
      parpoi=&compiler_data.parameters[j];
      p1=params[j].Find(':');
      if(p1!=1) //type should be 1 character
      {
        goto endofquest;
      }
      p2=params[j].Find('*',p1);
      if(p2==-1)
      {
        p2=params[j].GetLength(); //missing * hack        
      }
      else
      {
        parpoi->idsfile=params[j].Mid(p2+1);
        parpoi->idsfile.MakeUpper();
      }
      if(p2<=p1)
      {
        goto endofquest;
      }
      subtype=params[j].Mid(p1+1,p2-p1-1);
      subtype.MakeLower();
      parpoi->type=params[j].GetAt(0);
      if(validtypes.Find((char) parpoi->type)<0) //invalid type
      {
        goto endofquest;
      }
      switch(parpoi->type)
      {
      case 'P': //only one P
        if(pointcnt)
        {
          goto endofquest;
        }
        if(parpoi->idsfile.GetLength() )
        {
          goto endofquest;
        }
        pointcnt++;
        break;
      case '0': //zero object
        break;
      case 'O': //one or two objects (2./3. object position)
        if(parpoi->idsfile.GetLength() )
        {
          goto endofquest;
        }
        switch(objectcnt)
        {
        case 0: break;
        case 1:
          parpoi->type=SPT_SECONDOB; //second object (3. place)
          break;
        default:
          goto endofquest;
        }
        objectcnt++;
        break;
      case 'A': //one action (slips in 1. object)
        if(actioncnt)
        {
          goto endofquest;
        }
        if(parpoi->idsfile.GetLength() )
        {
          goto endofquest;
        }
        actioncnt++;
        if(compiler_data.parameters[0].type=='O')
        {//the only way of slipping in the first object
         //the parsing of 'A' will resolve the 2/3. objects
          compiler_data.parameters[0].type=SPT_OVERRIDE;
        }
        break;
      case 'S': //if args > 2 then first 2 strings are merged
        if(stringcnt>3) //we allow only 3 strings
        {
          goto endofquest;
        }
        if(parpoi->idsfile.GetLength() )
        {
          goto endofquest;
        }
        //special strings
        if(stringcnt)
        {
          switch(flg&0xff)
          {
          case IS_VAR:
            parpoi->type=SPT_STRING2;
            break;
					case ADD_VAR3:
          case ADD_GLOBAL:
					case ADD_LOCAL:
						parpoi->type=SPT_VAR2;
						break;
          case ADD_VAR:
            switch(stringcnt)
            {
            case 1:            
              parpoi->type=SPT_AREA1;
              break;
            case 3:            
              parpoi->type=SPT_AREA2;
              break;
            default:
              parpoi->type=SPT_VAR2; flg<<=8;
            }
            break;
          case ADD_VAR2:
            switch(stringcnt)
            {
            case 1:
              parpoi->type=SPT_AREA3;
              break;
            case 3:            
              parpoi->type=SPT_AREA4;
              break;
            default:
              parpoi->type=SPT_VAR4; flg<<=8;
            }            
            break;
          case CHECK_DSOUND:
            switch(stringcnt)
            {
            case 1:
              parpoi->type=SPT_RES3;  flg<<=8;
              break;
            default:
              parpoi->type=SPT_RES4;
            }            
            break;
					case CHECK_SCOPE: parpoi->type=SPT_2AREA; break;
          case CHECK_DEAD: parpoi->type=SPT_DEAD2; break;
          case CHECK_NUM: parpoi->type=SPT_NUMLIST2; break;
          case VALID_TAG:
            parpoi->type=SPT_TOKEN2; break; //only tokens are allowed
          case CHECK_XPLIST: parpoi->type=SPT_COLUMN2; break; //column in xplist
          default: parpoi->type=SPT_RESREF2; break;
          }
        }
        else
        {
          switch(flg&0xff)
          {
          case IS_VAR:
            parpoi->type=SPT_STRING1; break;
            break;
          case ADD_GLOBAL: case ADD_LOCAL:case ADD_VAR3:
						parpoi->type=SPT_VAR1;
						break;
          case CHECK_DSOUND: parpoi->type=SPT_RES1; flg<<=8; break;
          case ADD_VAR: parpoi->type=SPT_VAR1; flg<<=8; break;
          case ADD_VAR2: parpoi->type=SPT_VAR3; flg<<=8; break;
          case CHECK_SCOPE: parpoi->type=SPT_1AREA; break;
          case ADD_DEAD: case CHECK_DEAD: parpoi->type=SPT_DEAD1; break;
          case CHECK_NUM: parpoi->type=SPT_NUMLIST1; break;
          case VALID_TAG:
            parpoi->type=SPT_TOKEN1; break; //only tokens are allowed
          case CHECK_XPLIST: parpoi->type=SPT_COLUMN1; break; //column in xplist
          default: parpoi->type=SPT_RESREF1; break;
          }
        }

        flg>>=8;
        stringcnt++;
        break;
      case 'I':
        if(intcnt>2)
        {
          goto endofquest;
        }
        if(parpoi->idsfile.GetLength() )
        {
          ret=get_idsfile(parpoi->idsfile, false); //not forced
          switch(ret)
          {
          case -3:
            MessageBox(0,"Out of memory","Error",MB_ICONSTOP|MB_OK);
            break;
          case 0:
            break;
          default:
            MessageBox(0,"Can't load "+parpoi->idsfile+".ids!","Warning",MB_ICONWARNING|MB_OK);
            break;
          }
        }
        switch(intcnt)
        {
        case 0:
          //additional check/parsing an embedded string reference
          if(chkflag&0x10000)
          {
            parpoi->type=SPT_STRREF;
            break;
          }
          if(subtype=="0")
          {
            parpoi->type=SPT_ZERO;
            break;
          }
          parpoi->type=SPT_INTEGER;
          break;
        case 1:
          if(subtype=="0")
          {
            parpoi->type=SPT_ZERO;
            break;
          }
          if(subtype=="strref")
          {
            parpoi->type=SPT_STRREF2;
            break;
          }
          parpoi->type=SPT_INTEGER2;
          break;
        case 2:
          if(subtype=="0")
          {
            parpoi->type=SPT_ZERO;
            break;
          }
          if(subtype=="strref")
          {
            parpoi->type=SPT_STRREF3;
            break;
          }
          parpoi->type=SPT_INTEGER3;
          break;
        }
        intcnt++;
        break;
      default: //no other argument types are allowed
        goto endofquest;
      }
    }
    delete [] params;
  }
  at_data[name]=compiler_data;   //store it
  return 0;
endofquest:
  delete [] params;
  return -1;
}

////////////////////
int lookup_id(CString filename, CString token, int &value)
{
  CStringMapInt *idsfile;
  int x, len, hex;
  int ch;

  if(token.IsEmpty()) return CE_INVALID_IDS_SYMBOL;
  if( isdigit(token[0]) ||
    ( (token.GetLength()>1) && (token[0]=='-') && (isdigit(token[1]) ) )
    )//accept negative values
  {
    len=token.GetLength();
    hex=0;
    for(x=1;x<len;x++)
    {
      ch=token[x];
      if(ch>='0' && ch<='9') continue;
      if(ch>='a' && ch<='f' && hex) continue;
      if(ch!='x' || x!=1 || hex || token[0]!='0') return CE_INVALID_IDS_SYMBOL;
      hex=1;
    }
    value=strtonum(token);
    return 0; //no resolve needed
  }
  if(filename.IsEmpty()) return CE_INVALID_INTEGER; //no ids file allowed
  if(!idsmaps.Lookup(filename,idsfile)) return CE_MISSING_IDS_FILE; //no such ids file
  if(!idsfile) return -99;   //internal error (no file loaded)
  if(idsfile->Lookup(token, value)) return 1; //resolved successfully
  return CE_INVALID_IDS_SYMBOL; //not an IDS lookup value
}

int check_spelllist(CString tmp)
{
  CString tmpstr;

  int cnt=tmp.GetLength();
  if (cnt%4)
  {
    return CE_INVALID_SPELL_LIST;
  }
  cnt/=4;
  while(cnt--)
  {
    tmpstr = tmp.Mid(cnt*4,4);
    tmpstr = IDSToken("SPELL", atoi(tmpstr), false);
    if (tmpstr.IsEmpty())
    {
      return CE_INVALID_SPELL_NUMBER;
    }
  }
  return 0;
}

int convert_strref(CString string)
{
  if(!(editflg&WEIDUSTRING))
  {
    if(string.Left(1)=="~" && string.Right(1) =="~") return 0; //it's a string
  }
  if(!(editflg&IDUSTRING))
  {
    if(string.Left(1)=="\"" && string.Right(1) =="\"") return 0; //it's a string
  }
  return CE_INVALID_STRING;
}

int convert_string(CString string, char *poi, int maxlength, int longerr)
{
  int i;
  char ch;

  if(string.GetLength()>maxlength+2) return longerr; //string too long
  if(string.Left(1)!="\"") return CE_INVALID_STRING; //not a string
  if(string.Right(1)!="\"") return CE_INVALID_STRING; //not a string
  for(i=0;i<=maxlength;i++)
  {
    ch=string[i+1];
    if(ch=='"')
    {
      poi[i]=0;
      break;
    }
    poi[i]=(char) toupper(ch);
  }
  return 0;
}

int convert_point(CString token, int &value1, int &value2)
{
  int p1;
  CString tmp;

  if(token.Left(1)!="[") return CE_INVALID_POINT; //not a point
  if(token.Right(1)!="]") return CE_INVALID_POINT; //not a point
  p1=token.Find('.');
  if(p1<1) return CE_INVALID_POINT;
  tmp=token.Mid(1,p1-1);
  value1=strtonum(tmp);
  if(!value1)
  {
    if((tmp.GetLength()!=1) || (tmp[0]!='0')) return CE_INVALID_POINT;
  }
  tmp=token.Mid(p1+1,token.GetLength()-p1-2);
  value2=strtonum(tmp);
  if(!value2)
  {
    if((tmp.GetLength()!=1) || (tmp[0]!='0')) return CE_INVALID_POINT;
  }
  return 0;
}

int convert_object(CString line, object &ob, int recursion_level)
{
  CString *values;
  CString name;
  int p1,p2;
  int ret;

  if(line.IsEmpty() )
  {
    if(recursion_level) return 0;
    else return CE_INVALID_OBJECT;
  }
  if(recursion_level>5) return CE_MAXIMUM_OBJECT_NESTING;
  ob.bytes[0]=recursion_level+1;

  switch(line[0])
  {
  case '"': //death variable (string);
    /* sadly we do not know if this is a creature
    if(pst_compatible_var()) return convert_string(line, ob.var, 32, -10);
    else return convert_string(line, ob.var, 18, -10);
    */
    return convert_string(line, ob.var, 32, CE_INVALID_OBJECT);
    break;
  case '[': //IDS descriptor (ea,general,race,class,gender,alignment,specific)
    p2=line.ReverseFind(']');
    if(p2<0) return CE_MISSING_CLOSING_BRACKET;
    values=explode(line.Mid(1,p2-1),'.',p1);
    if(!p1 || !values) return CE_INVALID_OBJECT;
    for(p2=0;p2<p1;p2++)
    {
      ret=lookup_id(IDSName(p2,false),values[p2],ob.bytes[p2]);
      if(ret<0)
      {
        delete [] values;
        return ret;
      }
    }
    delete [] values;
    break;
  default:  //object (using object.ids)
    p1=line.Find('(');
    if(p1<1)
    {
      //accept Myself without brackets for now
      ret=lookup_id("OBJECT", line, ob.bytes[recursion_level+1]);
      if(ret==CE_INVALID_IDS_SYMBOL) ret=CE_INVALID_OBJECT;
      return ret;
    }
    p2=line.ReverseFind(')');
    if(p2<p1)
    {
      return CE_MISSING_CLOSING_PARENTHESES; //no closing bracket
    }
    name=line.Left(p1);
    ret=lookup_id("OBJECT", name, ob.bytes[recursion_level]);
    if(ret==0 || ret==CE_INVALID_IDS_SYMBOL) return CE_INVALID_OBJECT;//don't accept numbers
    if(ret<0) return ret;
    return convert_object(line.Mid(p1+1,p2-p1-1), ob, recursion_level+1);
    break;
  }
  return 0;
}

int compile_trigger(CString line, trigger &trigger)
{
  CString name;
  CString tmpstr;
  CString *args;
  int argnum;
  int j;
  int p1,p2;
  compiler_data compiler_data;
  char tmparea[10];
  int ret;

  trigger.Reset();
  p1=line.Find("//");              //it may contain remarks but we simply cut them
  if(p1>=0) line=line.Left(p1);
  line.TrimRight();                //cut any remaining spaces too
  if(line.IsEmpty()) return 0;     //not important
  line.MakeLower();
  p1=line.Find('(');
  if(p1<1)
  {
    return CE_MISSING_FUNCTION_NAME; 
  }
  p2=line.ReverseFind(')');
  if(p2<p1)
  {
    return CE_MISSING_CLOSING_PARENTHESES; 
  }
  if(p2+1!=line.GetLength() )
  {
    return CE_FUNCTION_NOT_TERMINATED; //crap after closing bracket
  }
  name=line.Left(p1);
  if(name.GetAt(0)=='!')
  {
    trigger.negate=true;
    name=name.Mid(1);
  }
  if(!trigger_data.Lookup(name, compiler_data) )
  {
    return CE_NON_EXISTENT_FUNCTION; //invalid trigger
  }
  trigger.opcode=compiler_data.opcode;

//  chkflag=handle_trigger(trigger.opcode); //information for variable grouping (apparently needed)
  tmpstr=line.Mid(p1+1, p2-p1-1);
  args=explode2(tmpstr, argnum);
  if(argnum<0) return argnum; //direct error from the argument explosion level
  if(argnum>5)
  {
    delete [] args;
    return CE_TOO_MANY_ARGUMENTS; //too many arguments
  }
  if(argnum!=compiler_data.parnum)
  {
    delete [] args;
    return CE_WRONG_ARGUMENT_NUMBER;
  }
  for(j=0;j<argnum;j++)
  {
    switch(compiler_data.parameters[j].type)
    {
    case SPT_ACTION:   //actionoverride in trigger, no way!
    case SPT_OVERRIDE: //actionoverrided object
    case SPT_SECONDOB: //second object in trigger
    case SPT_STRREF:   //no strref in triggers
    case SPT_STRREF2:
    case SPT_STRREF3:
      delete [] args;
      return CE_INVALID_ARGUMENT_TYPE;
    case '0':  //null argument, simply skip it (not in the original compiler)
      break;
    default:
      delete [] args;
      return -99;
    case SPT_POINT:    
      ret=convert_point(args[j], trigger.bytes[4], trigger.bytes[5]);
      if(ret<0)
      {
        delete [] args;
        return ret;
      }      
      break;
    case SPT_INTEGER:
      //lookup IDS, or simple number
      ret=lookup_id(compiler_data.parameters[j].idsfile, args[j], trigger.bytes[0]);
      if(ret<0)
      {
        delete [] args;
        return ret;
      }
      break;
    case SPT_INTEGER2:
      ret=lookup_id(compiler_data.parameters[j].idsfile, args[j], trigger.bytes[2]);
      if(ret<0)
      {
        delete [] args;
        return ret;
      }
      break;
    case SPT_INTEGER3:
      ret=lookup_id(compiler_data.parameters[j].idsfile, args[j], trigger.bytes[3]);
      if(ret<0)
      {
        delete [] args;
        return ret;
      }
      break;
    case SPT_DEAD1:
    case SPT_VAR1:
    case SPT_VAR3:
      ret=convert_string(args[j],trigger.var1,32,CE_INVALID_VARIABLE);
      if(ret<0)
      {
        delete [] args;
        return ret;
      }
//check variable
      break;
    case SPT_STRING1: //free string format
      ret=convert_string(args[j],trigger.var1,32,CE_INVALID_STRING);
      if(ret<0)
      {
        delete [] args;
        return ret;
      }
      break;
    case SPT_COLUMN1:
      ret=convert_string(args[j],trigger.var1,32,CE_INVALID_RESOURCE);
      if(ret<0)
      {
        delete [] args;
        return ret;
      }
//check column ???
      break;
    case SPT_TOKEN1:
      ret=convert_string(args[j],trigger.var1,32,CE_INVALID_RESOURCE);
      if(ret<0)
      {
        delete [] args;
        return ret;
      }
//check tokens ???
      break;
    case SPT_RESREF1: //resource
      ret=convert_string(args[j],trigger.var1,8,CE_INVALID_RESOURCE);
      if(ret<0)
      {
        delete [] args;
        return ret;
      }
      break;
    case SPT_1AREA:
      ret=convert_string(args[j],trigger.var1,6,CE_INVALID_SCOPE);
      if(ret<0)
      {
        delete [] args;
        return ret;
      }
      break;
    case SPT_2AREA:
      ret=convert_string(args[j],trigger.var2,6,CE_INVALID_SCOPE);
      if(ret<0)
      {
        delete [] args;
        return ret;
      }
      break;
    case SPT_AREA1: //area (only for var1)
      ret=convert_string(args[j],tmparea,6,CE_INVALID_SCOPE);
      if(ret<0)
      {
        delete [] args;
        return ret;
      }
      if(strlen(tmparea)!=6)
      {
        delete [] args;
        return CE_INVALID_SCOPE;
      }
      memmove(trigger.var1+6,trigger.var1,32);
      memcpy(trigger.var1,tmparea,6);
      break;
    case SPT_AREA3:
      ret=convert_string(args[j],tmparea,6,CE_INVALID_SCOPE);
      if(ret<0)
      {
        delete [] args;
        return ret;
      }
      if(strlen(tmparea)!=6)
      {
        delete [] args;
        return CE_INVALID_SCOPE;
      }
      memmove(trigger.var1+7,trigger.var1,32);
      memcpy(trigger.var1,tmparea,6);
      trigger.var1[6]=':';
      break;
    case SPT_VAR2:
    case SPT_VAR4:
      ret=convert_string(args[j],trigger.var2,32,CE_INVALID_VARIABLE);
      if(ret<0)
      {
        delete [] args;
        return ret;
      }
//check variable
      break;
    case SPT_STRING2: //free string format
      ret=convert_string(args[j],trigger.var2,32,CE_INVALID_STRING);
      if(ret<0)
      {
        delete [] args;
        return ret;
      }
      break;
    case SPT_COLUMN2:
      ret=convert_string(args[j],trigger.var2,32,CE_INVALID_RESOURCE);
      if(ret<0)
      {
        delete [] args;
        return ret;
      }
//check column ???
      break;
    case SPT_TOKEN2:
      ret=convert_string(args[j],trigger.var2,32,CE_INVALID_RESOURCE);
      if(ret<0)
      {
        delete [] args;
        return ret;
      }
//check tokens ???
      break;
    case SPT_RESREF2: //resource
      ret=convert_string(args[j],trigger.var2,8,CE_INVALID_RESOURCE);
      if(ret<0)
      {
        delete [] args;
        return ret;
      }
      break;
    case SPT_AREA2: //area (only for var2)
      ret=convert_string(args[j],tmparea,6,CE_INVALID_SCOPE);
      if(ret<0)
      {
        delete [] args;
        return ret;
      }
      if(strlen(tmparea)!=6)
      {
        delete [] args;
        return CE_INVALID_SCOPE;
      }
      memmove(trigger.var2+6,trigger.var2,32);
      memcpy(trigger.var2,tmparea,6);
      break;
    case SPT_OBJECT: //only O exists in triggers
      //object (may be string)
      ret=convert_object(args[j],trigger.trobj,0);
      if(ret<0)
      {
        delete [] args;
        return ret;
      }
      break;
    }
  }
  delete [] args;
  return 0;
}

int compile_action(CString line, action &action, bool inoverride)
{
  CString name;
  CString tmpstr;
  CString *args;
  int argnum;
  int j;
  int p1,p2;
  compiler_data compiler_data;
  int ret;
  char tmparea[8];

  if(!inoverride) action.Reset();
  p1=line.Find("//");              //it may contain remarks but we simply cut them
  if(p1>=0) line=line.Left(p1);
  line.TrimRight();                //cut any remaining spaces too
  if(line.IsEmpty()) return 0;
  line.MakeLower();
  p1=line.Find('(');
  if(p1<1)
  {
    return CE_MISSING_FUNCTION_NAME; //no name
  }
  p2=line.ReverseFind(')');
  if(p2<p1)
  {
    return CE_MISSING_CLOSING_PARENTHESES; //no closing bracket
  }
  if(p2+1!=line.GetLength() )
  {
    return CE_FUNCTION_NOT_TERMINATED; //crap after closing bracket
  }
  name=line.Left(p1);
  //no ! needed in actions
  if(!action_data.Lookup(name, compiler_data) )
  {
    return CE_NON_EXISTENT_FUNCTION; //invalid action
  }
  action.opcode=compiler_data.opcode;
//  chkflag=handle_action(action.opcode); //information for variable grouping (apparently needed)
  tmpstr=line.Mid(p1+1, p2-p1-1);
  args=explode2(tmpstr, argnum);
  if(argnum<0) return argnum; //direct error from the argument explosion level
  if(argnum>7)
  {
    delete [] args;
    return CE_TOO_MANY_ARGUMENTS; //too many arguments
  }
  if(argnum!=compiler_data.parnum)
  {
    delete [] args;
    return CE_WRONG_ARGUMENT_NUMBER;
  }
  for(j=0;j<argnum;j++)
  {
    switch(compiler_data.parameters[j].type)
    {
    case 0:  //null argument
      break;
    default:
      delete [] args;
      return -99;
    case SPT_ACTION:
      //actionoverride, we have to recompile this, only one level of recursion is allowed
      if(inoverride)
      {
        delete [] args;
        return CE_MAXIMUM_FUNCTION_NESTING;
      }
      ret=compile_action(args[j],action,true);
      if(ret<0)
      {
        delete [] args;
        return ret;
      }
      break;
    case SPT_INTEGER:
      //lookup IDS
      ret=lookup_id(compiler_data.parameters[j].idsfile, args[j], action.bytes[0]);
      if(ret<0)
      {
        delete [] args;
        return ret;
      }
      break;
    case SPT_STRREF:
      if(!(editflg&WEIDUSTRING))
      {
        ret=convert_strref(args[j]);
        if(!ret) break;
      }
      //lookup IDS
      ret=lookup_id(compiler_data.parameters[j].idsfile, args[j], action.bytes[0]);
      if(ret<0)
      {
        delete [] args;
        return ret;
      }
      if(action.bytes[0]>=tlk_headerinfo[0].entrynum)
      {
        delete [] args;
        return CE_INVALID_TLK_REFERENCE;
      }
      break;
    case SPT_INTEGER2:
      //2. integer
      ret=lookup_id(compiler_data.parameters[j].idsfile, args[j], action.bytes[3]);
      if(ret<0)
      {
        delete [] args;
        return ret;
      }
      break;
    case SPT_STRREF2:
      //2. integer
      ret=lookup_id(compiler_data.parameters[j].idsfile, args[j], action.bytes[3]);
      if(ret<0)
      {
        delete [] args;
        return ret;
      }
      if(action.bytes[3]>=tlk_headerinfo[0].entrynum)
      {
        delete [] args;
        return CE_INVALID_TLK_REFERENCE;
      }
      break;
    case SPT_INTEGER3:
      //3. integer
      ret=lookup_id(compiler_data.parameters[j].idsfile, args[j], action.bytes[4]);
      if(ret<0)
      {
        delete [] args;
        return ret;
      }
      break;
    case SPT_STRREF3:
      //3. integer
      ret=lookup_id(compiler_data.parameters[j].idsfile, args[j], action.bytes[4]);
      if(ret<0)
      {
        delete [] args;
        return ret;
      }
      if(action.bytes[4]>=tlk_headerinfo[0].entrynum)
      {
        delete [] args;
        return CE_INVALID_TLK_REFERENCE;
      }
      break;
    case SPT_VAR1:
    case SPT_VAR3:
      ret=convert_string(args[j],action.var1,32,CE_INVALID_VARIABLE);
      if(ret<0)
      {
        delete [] args;
        return ret;
      }
//check variable
      break;
    case SPT_NUMLIST1:
      ret=convert_string(args[j],action.var1,32,CE_INVALID_RESOURCE);
      if(ret<0)
      {
        delete [] args;
        return ret;
      }
      ret=check_spelllist(action.var1);
      if(ret<0)
      {
        delete [] args;
        return ret;
      }
      break;
    case SPT_COLUMN1:
      ret=convert_string(args[j],action.var1,32,CE_INVALID_RESOURCE);
      if(ret<0)
      {
        delete [] args;
        return ret;
      }
//check column ???
      break;
    case SPT_TOKEN1:
      ret=convert_string(args[j],action.var1,32,CE_INVALID_RESOURCE);
      if(ret<0)
      {
        delete [] args;
        return ret;
      }
//check tokens ???
      break;
    case SPT_RES1:
    case SPT_RESREF1: //resource
      ret=convert_string(args[j],action.var1,8,CE_INVALID_RESOURCE);
      if(ret<0)
      {
        delete [] args;
        return ret;
      }
      break;
    case SPT_RES2:
      ret=convert_string(args[j],tmparea,8,CE_INVALID_RESOURCE);
      if(ret<0)
      {
        delete [] args;
        return ret;
      }
      memmove(action.var1+strlen(tmparea)+1,action.var1,32);
      memcpy(action.var1,tmparea,8);
      action.var1[8]=':';
      break;
    case SPT_AREA3: //area (only for var1)
      ret=convert_string(args[j],tmparea,6,CE_INVALID_SCOPE);
      if(ret<0)
      {
        delete [] args;
        return ret;
      }
      if(strlen(tmparea)!=6)
      {
        delete [] args;
        return CE_INVALID_SCOPE;
      }
      memmove(action.var1+7,action.var1,32);
      memcpy(action.var1,tmparea,6);
      action.var1[6]=':';
      break;
    case SPT_1AREA:
    case SPT_AREA1: //area (only for var1)
      ret=convert_string(args[j],tmparea,6,CE_INVALID_SCOPE);
      if(ret<0)
      {
        delete [] args;
        return ret;
      }
      if(strlen(tmparea)!=6)
      {
        delete [] args;
        return CE_INVALID_SCOPE;
      }
      memmove(action.var1+6,action.var1,32);
      memcpy(action.var1,tmparea,6);
      break;
    case SPT_VAR2:
    case SPT_VAR4:
      ret=convert_string(args[j],action.var2,32,CE_INVALID_VARIABLE);
      if(ret<0)
      {
        delete [] args;
        return ret;
      }
//check variable
      break;
    case SPT_COLUMN2:
      ret=convert_string(args[j],action.var2,32,CE_INVALID_STRING);
      if(ret<0)
      {
        delete [] args;
        return ret;
      }
//check column ???
      break;
    case SPT_TOKEN2:
      ret=convert_string(args[j],action.var2,32,CE_INVALID_STRING);
      if(ret<0)
      {
        delete [] args;
        return ret;
      }
//check tokens ???
      break;
    case SPT_RES3:
    case SPT_RESREF2: //resource
      ret=convert_string(args[j],action.var2,8,CE_INVALID_RESOURCE);
      if(ret<0)
      {
        delete [] args;
        return ret;
      }
      break;
    case SPT_RES4:
      ret=convert_string(args[j],tmparea,8,CE_INVALID_RESOURCE);
      if(ret<0)
      {
        delete [] args;
        return ret;
      }
      if(strlen(tmparea)!=8)
      {
        delete [] args;
        return CE_INVALID_SCOPE;
      }
      memmove(action.var2+9,action.var2,32);
      memcpy(action.var2,tmparea,8);
      action.var2[8]=':';
      break;
    case SPT_AREA4:
      ret=convert_string(args[j],tmparea,6,CE_INVALID_SCOPE);
      if(ret<0)
      {
        delete [] args;
        return ret;
      }
      if(strlen(tmparea)!=6)
      {
        delete [] args;
        return CE_INVALID_SCOPE;
      }
      memmove(action.var2+7,action.var2,32);
      memcpy(action.var2,tmparea,6);
      action.var2[6]=':';
      break;
    case SPT_2AREA:
    case SPT_AREA2: //area (only for var1)
      ret=convert_string(args[j],tmparea,6,CE_INVALID_SCOPE);
      if(ret<0)
      {
        delete [] args;
        return ret;
      }
      if(strlen(tmparea)!=6)
      {
        delete [] args;
        return CE_INVALID_SCOPE;
      }
      memmove(action.var2+6,action.var2,32);
      memcpy(action.var2,tmparea,6);
      break;
    case SPT_POINT: //only one point allowed
      ret=convert_point(args[j], action.bytes[1], action.bytes[2]);
      if(ret<0)
      {
        delete [] args;
        return ret;
      }
      //point
      break;
    case SPT_SECONDOB: //param2 object (3. place)
      ret=convert_object(args[j],action.obj[2],0);
      if(ret<0)
      {
        delete [] args;
        return ret;
      }
      break;
    case SPT_OBJECT: //regular param1 object (2. place)
      ret=convert_object(args[j],action.obj[1],0);
      if(ret<0)
      {
        delete [] args;
        return ret;
      }
      break;
    case SPT_OVERRIDE: //actionoverride object (1. place)
      ret=convert_object(args[j],action.obj[0],0);
      if(ret<0)
      {
        delete [] args;
        return ret;
      }
      break;
    }
  }
  delete [] args;
  return 0;
}

static int check_val;

int read_next_desc(FILE *fpoi)
{
  CString tmpstr;
  CString tmp;
  int len;
  int index;  

  len=read_until(',',fpoi,tmp);
  if(!len)
  {
    return 2;
  }
  index=atoi(tmp); //only dec
  if(index<0 || index>=NUM_FEATS)
  {
    return -1;
  }

  len=read_until(',',fpoi,tmp);
  if(!len)
  {
    return -1;
  }
  fgetc(fpoi); //skip
  len=read_until('"',fpoi,tmp);
  if(!len)
  {
    return -1;
  }
  if(!opcodes[index].opcodetext.IsEmpty())
  {
    tmpstr.Format("Problem reading effect list at %d previous(%d)!\n",index, check_val);
    MessageBox(0,tmpstr,"Warning",MB_ICONSTOP);
  }
  check_val = index;

  opcodes[index].opcodetext=tmp;

  read_until('"',fpoi,tmp);
//  skip_string(fpoi,',');
//  skip_string(fpoi,'"');
  len=read_until('"',fpoi,tmp);
  opcodes[index].par1=tmp;

  read_until('"',fpoi,tmp);
//  skip_string(fpoi,',');
//  skip_string(fpoi,'"');
  len=read_until('"',fpoi,tmp);
  if(!len)
  {
    return -1;
  }
  opcodes[index].par2=tmp;

  read_until('"',fpoi,tmp);
//  skip_string(fpoi,',');
//  skip_string(fpoi,'"');
  len=read_until('"',fpoi,tmp);
  if(!len)
  {
    return -1;
  }
  opcodes[index].opcodedesc=tmp;  

  skip_string(fpoi,'\n');
  return 0;
}

int read_effect_descs()
{
  FILE *fpoi;
  int i;
  int ret;

  for(i=0;i<NUM_FEATS;i++)
  {
    opcodes[i].opcodetext.Empty();
    opcodes[i].opcodedesc.Empty();
    opcodes[i].par1.Empty();
    opcodes[i].par2.Empty();
  }
  if(descpath.IsEmpty())
  {
    return 1;
  }
  
  maxlength=-1;
  xorflag=0;
  position=0;
  fpoi=fopen(descpath,"rt");
  if(fpoi<=0)
  {
    return -2;
  }
  ret=0;
  check_val=0;
  do
  {
    ret=read_next_desc(fpoi);
    if(ret)
    {
      if(ret==2)
      {
        ret=0;
      }
      break;
    }
  }
  while(1);
  fclose(fpoi);
  return ret;  
}

int get_idsfile(CString idsname, bool forced)
{
  loc_entry tmploc;
  CStringMapInt *idsfile;
  int val;

  if(idsname.GetLength()>8) idsname=idsname.Left(8);
  if(forced)
  {
    if(idsmaps.Lookup(idsname, idsfile))
    {
      delete idsfile;
      idsmaps.RemoveKey(idsname);
    }
  }
  if(!idsmaps.Lookup(idsname, idsfile) ) //reading idsfile if it hasn't been read yet
  {
    if(!idrefs.Lookup(idsname,tmploc) ) return -1;
    idsfile=new CStringMapInt;
    if(!idsfile) return -3;              //this may have crashed something
    val=ReadIds4(tmploc, *idsfile);
    if(val<0)
    {
      delete idsfile;
      return val;
    }
    idsmaps[idsname]=idsfile;
    return val;
  }
  return 0;
}

///////////////////////////////////////////////////////////////////////////
//2da class (editor)

void C2da::new_2da()
{
  POSITION pos;
  CString tmpstr;
  CString *tmppoi;

  if(!data) return;
  header="2DA V1.0";
  defvalue="*";
  if(collabels)
  {
    delete [] collabels;
    collabels=NULL;
  }
  pos=data->GetHeadPosition();
  while(pos)
  {
    tmppoi=(CString *) data->GetNext(pos);
    delete [] tmppoi;
  }
  data->RemoveAll();
  rows=0;
  cols=1;
  killcopyrow();
  killcopycol();
}

C2da::C2da()
{
  data=new CPtrList;
  collabels = NULL;
  copycol = NULL;
  copyrow = NULL;
}

C2da::~C2da()
{
  new_2da();
  if(data)
  {
    delete data;
    data=NULL;
  }
}

int C2da::RemoveColumn(int col)
{
  POSITION pos;
  CString *add, *row;
  int i,j;

  if(cols<2) return 0;  //don't remove the row label column
  if(col>=cols) return 0;
  add=new CString[cols-1];
  if(!add) return 0;
  j=0;
  for(i=0;i<cols;i++)
  {
    if(i!=col) add[j++]=collabels[i];
  }
  delete [] collabels;
  collabels=add;
  pos=data->GetHeadPosition();
  while(pos)
  {
    add=new CString[cols-1];
    if(!add) return 0;
    row=(CString *) data->GetAt(pos);
    j=0;
    for(i=0;i<cols;i++)
    {
      if(i!=col) add[j++]=row[i];
    }
    delete [] row;
    data->SetAt(pos, add);
    data->GetNext(pos);
  }
  cols--;
  return 1;
}

int C2da::CopyColumn(int col)
{
  POSITION pos;
  CString *tmppoi;
  int row, i;

  if(col>=cols) return 0;
  killcopycol();
  copycol=new CString [rows+1];
  if(!copycol) return 0;
  crows=rows+1;
  copycol[0]=collabels[col];
  i=1;
  for(row=0;row<rows;row++)
  {
    pos=data->FindIndex(row);
    if(pos)
    {
      tmppoi=(CString *) data->GetAt(pos);
      copycol[i++]=tmppoi[col];
    }
  }
  return 1;
}

int C2da::PasteColumn(int col)
{
  POSITION pos;
  CString *tmppoi;
  int row, i;

  if(col>=cols) return 0;
  if(!copycol) return 0;
  crows=rows+1;
  collabels[col]=copycol[0];
  i=1;
  for(row=0;row<rows && row<crows;row++)
  {
    pos=data->FindIndex(row);
    if(pos)
    {
      tmppoi=(CString *) data->GetAt(pos);
      tmppoi[col]=copycol[i++];
    }
  }
  for(;row<rows;row++)
  {
    pos=data->FindIndex(row);
    if(pos)
    {
      tmppoi=(CString *) data->GetAt(pos);
      tmppoi[col]=defvalue;
    }
  }
  return 1;
}

int C2da::CopyRow(int row)
{
  POSITION pos;
  CString *tmppoi;
  int i;

  if(row>=rows) return 0;
  killcopyrow();
  copyrow=new CString [cols];
  if(!copyrow) return 0;
  ccols=cols;
  pos=data->FindIndex(row);
  if(pos)
  {
    tmppoi=(CString *) data->GetAt(pos);
    for(i=0;i<cols;i++)
    {
      copyrow[i]=tmppoi[i];
    }
  }
  return 1;
}

int C2da::PasteRow(int row)
{
  POSITION pos;
  CString *tmppoi;
  int i;

  if(row>=rows) return 0;
  if(!copyrow) return 0;
  pos=data->FindIndex(row);
  if(pos)
  {
    tmppoi=(CString *) data->GetAt(pos);
    for(i=0;i<cols && i<ccols;i++)
    {
      tmppoi[i]=copyrow[i];
    }
    for(;i<cols;i++)
    {
      tmppoi[i]=defvalue;
    }
  }
  return 1;
}

int C2da::RemoveRow(int row)
{
  POSITION pos;
  CString *tmppoi;

  pos=data->FindIndex(row);
  if(pos)
  {
    tmppoi=(CString *) data->GetAt(pos);
    delete [] tmppoi;
    data->RemoveAt(pos);
    rows--;
    return 1;
  }
  return 0;
}

//adds a column
int C2da::AddColumn(int col, CString label, CString def)
{
  POSITION pos;
  CString *add, *row;
  int i,j;

  if(col>cols) return 0;
  add=new CString[cols+1];
  if(!add) return 0;
  j=0;
  for(i=0;i<=cols;i++)
  {
    if(i==col) add[j++]=label;
    if(i<cols) add[j++]=collabels[i];
  }
  delete [] collabels;
  collabels=add;
  pos=data->GetHeadPosition();
  while(pos)
  {
    add=new CString[cols+1];
    if(!add) return 0;
    row=(CString *) data->GetAt(pos);
    j=0;
    for(i=0;i<=cols;i++)
    {
      if(i==col) add[j++]=def;
      if(i<cols) add[j++]=row[i];
    }
    delete [] row;
    data->SetAt(pos, add);
    data->GetNext(pos);
  }
  cols++;
  return 1;
}

void C2da::AddRow(int row, CString label, CString def)
{
  POSITION pos;
  CString *add;
  int i;

  if(!cols) return;
  add=new CString[cols];
  if(!add) return;
  add[0]=label;
  for(i=1;i<cols;i++)
  {
    add[i]=def;
  }
  pos=data->FindIndex(row);
  if(pos)
  {
    data->InsertBefore(pos,add);
  }
  else
  {
    data->AddTail(add);
  }
  rows++;
}

void C2da::OrderByColumn(int column, int numeric)
{
  CString *tmppoi, *tmppoi2;
  POSITION pos, pos2, pos3;
  CPtrList *data2;
  int val;
  CString val2;

  data2=new CPtrList;
  if(!data2) return; //can't order it due to low memory
  pos=data->GetHeadPosition();
  while(pos)
  {
    tmppoi=(CString *) data->GetNext(pos);
    if(numeric)
    {
      val=strtonum(tmppoi[column]);
    }
    else
    {
      val2=tmppoi[column];
    }
    pos2=data2->GetTailPosition();
    do
    {
      pos3=pos2;
      if(!pos2) break;
      tmppoi2=(CString *) data2->GetPrev(pos2);
      if(numeric)
      {
        //val is used only when numeric == 1
        if(strtonum(tmppoi2[column])>val) break;
      }
      else
      {
        if(val2.CompareNoCase(tmppoi2[column])>0) break;
      }
    }
    while(1);
    if(pos3) data2->InsertAfter(pos3,tmppoi);
    else data2->AddHead(tmppoi);
  }
  delete data; //we must not delete referenced data members
	data=data2;
}

CString C2da::GetValue(int row, int col)
{
  CString *tmppoi;

  if(!data) return defvalue;
  if(rows<=row) return defvalue;
  if(cols<=col) return defvalue;
  tmppoi=(CString *) data->GetAt(data->FindIndex(row));
  return tmppoi[col];
}

CString C2da::FindValue(CString key, int col)
{
  CString *tmppoi;
  POSITION pos;
  int i;

  if(!data) return defvalue;
  if(cols<=col) return defvalue;
  pos=data->GetHeadPosition();
  for(i=0;i<rows;i++)
  {
    tmppoi=(CString *) data->GetNext(pos);
    if(tmppoi[0]==key) return tmppoi[col];
  }
  return defvalue;
}

int C2da::Write2DAToFile(int fhandle)
{
  POSITION pos;
  CString *tmppoi;
  FILE *fpoi;
  CString tmpstr;
  CString format;
  int i,j;
  int width;
  //int namelen;
  int *collen;
  
  collen = (int *) malloc(cols * sizeof(int) ); 
  for (i=0;i<cols;i++)
  {
    collen[i]=10;
  }
  pos=data->GetHeadPosition();
  while(pos)
  {
    tmppoi=(CString *) data->GetNext(pos);
    for (i = 0;i<cols; i++)
    {
      int len = tmppoi[i].GetLength();
      if (len>30) len=30;
      if(collen[i]<tmppoi[i].GetLength() )
      {
        collen[i]=tmppoi[i].GetLength();
      }
    }
  }
  fpoi=fdopen(fhandle,"wb");
  if(!fpoi)
  {
    close(fhandle);
    free(collen);
    return -1;
  }
  tmpstr.Format("%s\r\n%s\r\n",header,defvalue);
  WriteString(fpoi,tmpstr);
  
  for(i=0;i<cols;i++)
  {
    tmpstr.Format("%-*s ",collen[i],collabels[i]);
    WriteString(fpoi,tmpstr);
  }
  WriteString(fpoi,"\r\n");
  
  pos=data->GetHeadPosition();
  for(i=0;i<rows;i++)
  {
    tmppoi=(CString *) data->GetNext(pos);
    for(j=0;j<cols;j++)
    {
      width=collen[j];      
      format.Format("%%-%ds ",width);
      tmpstr.Format(format,tmppoi[j]);
      WriteString(fpoi,tmpstr);
    }
    WriteString(fpoi,"\r\n");
  }
  fclose(fpoi);
  free(collen);
  return 0;
}

int C2da::Read2DAFromFile(int fhandle, int length)
{
  CString ref;
  CString *row;
  CString *add;
  FILE *fpoi;
  int mycols;
  int ret;
  int res;
  int i;

  xorflag=position=0;
  new_2da();
  if(length<0) maxlength=filelength(fhandle);
  else maxlength=length;
  fpoi=fdopen(fhandle,"rb");
  if(!fpoi)
  {
    close(fhandle);
    return -1;
  }
  read_string(fpoi, "\n", external,sizeof(external)); //header
  header=external;
  read_string(fpoi, "\n", external,sizeof(external)); //default
  defvalue=external;
  read_string(fpoi, "\n", external,sizeof(external)); //columns
  if (external[0]!=' ') {
    memmove(external+1,external,MAXBUFFSIZE-1);
    external[0]=' ';
  }
  external[MAXBUFFSIZE-1]=0;
  collabels=explode(external,' ',cols);
  if(cols<1)
  {
    fclose(fpoi);
    return -1;
  }
  ret=0;
  do
  {
    external[0]=0;
    res=read_string(fpoi, "\n",external,sizeof(external));
    external[MAXBUFFSIZE-1]=0;
    ref=external;
    ref.TrimRight();
    ref.MakeUpper();
    if(!ref.GetLength()) continue; //skipping empty rows
    row=explode(external,' ',mycols);
    if(mycols!=cols)
    {
      ret|=1;
    }
    add=new CString [cols];
    if(!add)
    {
      if(row) delete [] row;
      return -3;
    }
    for(i=0;i<mycols && i<cols;i++)
    {
      add[i]=row[i];
    }
    while(i<cols)
    {
      add[i]=defvalue;
      i++;
    }
    if(row) delete [] row;
    data->AddTail(add);
  }
  while(res==1 );
  fclose(fpoi);
  rows=data->GetCount();
  return ret;
}

int C2da::Extract2DAFromSQLFile(int fhandle, int length, CString filename, int defcols)
{
  CString *row;
  CString *add;
  FILE *fpoi;
  int mycols;
  int res;
  int i;
  int ret;

  xorflag=position=0;
  new_2da();
  cols = defcols;
  if(length<0) maxlength=filelength(fhandle);
  else maxlength=length;
  fpoi=fdopen(fhandle,"rb");
  if(!fpoi)
  {
    close(fhandle);
    return -1;
  }

  read_line_until( fpoi, "INSERT INTO "+filename+" ROWS");
  read_string( fpoi, "\n", external, sizeof(external) );
  if (memcmp(external,"(",2) )
  {
    fclose(fpoi);
    return -1;
  }

  ret=0;
  do
  {
    res = read_string( fpoi, "\n", external, sizeof(external) );
    row=explode(external,',',mycols);
    
    if (!memcmp(external,");",3) )
    {
      break;
    }

    if(mycols!=cols)
    {
      ret|=1;
    }
    add=new CString [cols];
    if(!add)
    {
      if(row) delete [] row;
      return -3;
    }
    for(i=0;i<mycols && i<cols;i++)
    {
      add[i]=row[i];
    }
    while(i<cols)
    {
      add[i]=defvalue;
      i++;
    }
    if(row) delete [] row;
    data->AddTail(add);
  }
  while(res==1);
  fclose(fpoi);
  rows=data->GetCount();
  return ret;
}
///////////////////////////////////////////////////////////////////////////
//ids class (editor)

void Cids::new_ids()
{
  POSITION pos;
  CString tmpstr;
  CString *tmppoi;

  if(!data) return;
  header="IDS V1.0";
  pos=data->GetHeadPosition();
  while(pos)
  {
    tmppoi=(CString *) data->GetNext(pos);
    delete [] tmppoi;
  }
  data->RemoveAll();
  rows=0;
  label=true;
}

Cids::Cids()
{
  data=new CPtrList;
}

Cids::~Cids()
{
  new_ids();
  if(data)
  {
    delete data;
    data=NULL;
  }
}

int Cids::RemoveRow(int row)
{
  POSITION pos;
  CString *tmppoi;

  pos=data->FindIndex(row);
  if(pos)
  {
    tmppoi=(CString *) data->GetAt(pos);
    delete [] tmppoi;
    data->RemoveAt(pos);
    rows--;
    return 1;
  }
  return 0;
}

void Cids::AddRow(int row, CString label, CString def)
{
  POSITION pos;
  CString *add;

  add=new CString[2];
  if(!add) return;
  add[0]=label;
  add[1]=def;
  pos=data->FindIndex(row);
  if(pos)
  {
    data->InsertBefore(pos,add);
  }
  else
  {
    data->AddTail(add);
  }
  rows++;
}

int Cids::WriteIDSToFile(int fhandle)
{
  POSITION pos;
  CString *tmppoi;
  FILE *fpoi;
  CString tmpstr;
  CString format;
  int i;

  fpoi=fdopen(fhandle,"wb");
  if(!fpoi)
  {
    close(fhandle);
    return -1;
  }
  tmpstr.Format("%s\r\n",header);
  WriteString(fpoi,tmpstr);
  
  pos=data->GetHeadPosition();
  for(i=0;i<rows;i++)
  {
    tmppoi=(CString *) data->GetNext(pos);
    tmpstr.Format("%s %s\r\n",tmppoi[0], tmppoi[1]); //do not format first value
    WriteString(fpoi,tmpstr);
  }
  fclose(fpoi);
  return 0;
}

int Cids::ReadIDSFromFile(int fhandle, int length)
{
  char tmpref[MAXIDSIZE];
  CString ref;
  CString *add;
  FILE *fpoi;
  int ret;
  int res;
  int pos;

  xorflag=position=0;
  new_ids();
  if(length<0) maxlength=filelength(fhandle);
  else maxlength=length;
  fpoi=fdopen(fhandle,"rb");
  if(!fpoi)
  {
    close(fhandle);
    return -1;
  }
  read_string(fpoi, "\n", tmpref,sizeof(tmpref)); //skipping crap
  header=tmpref;
  label=true;
  ret=0;
  do
  {
    tmpref[0]=0;
    res=read_string(fpoi, "\n",tmpref,sizeof(tmpref));
    tmpref[MAXIDSIZE-1]=0;
    ref=tmpref;
    if(!ref.GetLength()) continue; //skipping empty rows
    pos=ref.FindOneOf(" \t");
    if(pos<1)
    {
      ret|=1;
      continue;
    }
    add=new CString[2]; 
    if(!add)
    {
      fclose(fpoi);
      return -3;
    }
    add[0]=ref.Left(pos);
    if(ref.FindOneOf(" ()")==-1)
    {
      ref.MakeUpper();
      label=false;
    }
    add[1]=ref.Mid(pos);
    add[1].TrimLeft();
    add[1].TrimRight();

    data->AddTail(add);
  }
  while(res==1 );
  fclose(fpoi);
  rows=data->GetCount();
  return ret;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void Cmus::new_mus()
{
  POSITION pos;
  CString tmpstr;
  CString *tmppoi;

  if(!data) return;
  pos=data->GetHeadPosition();
  while(pos)
  {
    tmppoi=(CString *) data->GetNext(pos);
    delete [] tmppoi;
  }
  data->RemoveAll();
  rows=0;
}

Cmus::Cmus()
{
  data=new CPtrList; //cannot really do anything
}

Cmus::~Cmus()
{
  new_mus();
  if(data)
  {
    delete data;
    data=NULL;
  }
}

int Cmus::RemoveRow(int row)
{
  POSITION pos;
  CString *tmppoi;

  pos=data->FindIndex(row);
  if(pos)
  {
    tmppoi=(CString *) data->GetAt(pos);
    delete [] tmppoi;
    data->RemoveAt(pos);
    rows--;
    return 1;
  }
  return 0;
}

void Cmus::AddRow(int row, CString arg1, CString arg2, CString arg3)
{
  POSITION pos;
  CString *add;

  add=new CString[MUSCOLUMN];
  if(!add) return;
  add[0]=arg1;
  add[1]=arg2;
  add[2]=arg3;
  pos=data->FindIndex(row);
  if(pos)
  {
    data->InsertBefore(pos,add);
  }
  else
  {
    data->AddTail(add);
  }
  rows++;
}

int Cmus::WriteMusToFile(int fhandle)
{
  POSITION pos;
  CString *tmppoi;
  FILE *fpoi;
  CString tmpstr;
  CString format;
  int i;

  fpoi=fdopen(fhandle,"wb");
  if(!fpoi)
  {
    close(fhandle);
    return -1;
  }
  tmpstr.Format("%s\r\n%d\r\n",internal,data->GetCount());
  WriteString(fpoi,tmpstr);
  
  pos=data->GetHeadPosition();
  for(i=0;i<rows;i++)
  {
    tmppoi=(CString *) data->GetNext(pos);
    if(tmppoi[2].IsEmpty())
    {
      tmpstr.Format("%-9s%s\r\n",tmppoi[0], tmppoi[1]); 
    }
    else
    {
      tmpstr.Format("%-9s %-9s @TAG %s\r\n",tmppoi[0], tmppoi[1], tmppoi[2]); 
    }
    WriteString(fpoi,tmpstr);
  }
  fclose(fpoi);
  return 0;
}

int Cmus::ReadMusFromFile(int fhandle, int length)
{
  char tmpref[MAXIDSIZE];
  CString ref;
  CString *add;
  FILE *fpoi;
  int ret;
  int res;
  int pos1, pos2;

  xorflag=position=0;
  new_mus();
  if(length<0) maxlength=filelength(fhandle);
  else maxlength=length;
  fpoi=fdopen(fhandle,"rb");
  if(!fpoi)
  {
    close(fhandle);
    return -1;
  }
  read_string(fpoi, "\n", tmpref,sizeof(tmpref)); //skipping crap
  internal=tmpref;
  internal.TrimLeft();
  internal.TrimRight();
  internal.MakeUpper();
  read_string(fpoi, "\n", tmpref,sizeof(tmpref)); //skipping crap
  rows=strtonum(tmpref);
  ret=0;
  do
  {
    tmpref[0]=0;
    res=read_string(fpoi, "\n",tmpref,sizeof(tmpref));
    tmpref[MAXIDSIZE-1]=0;
    ref=tmpref;
    if(!ref.GetLength()) continue; //skipping empty rows
    ref.MakeUpper();
    ref.TrimLeft();
    ref.TrimRight();
    add=new CString[MUSCOLUMN];
    if(!add)
    {
      fclose(fpoi);
      return -3;
    }
    pos1=ref.FindOneOf(" \t");
    if(pos1<1)
    {
      add[0]=ref;
      add[1]="";
      add[2]="";
    }
    else
    {
      add[0]=ref.Left(pos1);
      pos2=ref.Find("@TAG",pos1);
      if(pos2<1)
      {
        add[1]=ref.Mid(pos1);
        add[1].TrimLeft();
        add[1].TrimRight();
        add[2]="";
      }
      else
      {
        add[1]=ref.Mid(pos1,pos2-pos1);
        add[1].TrimLeft();
        add[1].TrimRight();
        add[2]=ref.Mid(pos2+4);
        add[2].TrimLeft();
        add[2].TrimRight();
      }
    }
    data->AddTail(add);
    if(!--rows) break;
  }
  while(res==1 );
  fclose(fpoi);
  rows=data->GetCount();
  return ret;
}

