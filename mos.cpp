// mos.cpp: implementation of the Cmos class.
// portions came from the SK's bam handler
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "chitem.h"
#include "chitemDlg.h"
#include "mos.h"
#include "zlib.h"
#include "oct_quan.h"
#include "options.h"
#include "pvr.h"

#define PIXEL_INCREMENT  17

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// INF_MOS_FRAMEDATA helper class
//////////////////////////////////////////////////////////////////////
static int colourorder(const void *a, const void *b)
{
  int asum, bsum;
  int tmp;
  int i;

  //transparent colours go first
  if((*(unsigned long *) a&0xffffff)==TRANSPARENT_GREEN)
  {
    return -1;
  }
  if((*(unsigned long *) b&0xffffff)==TRANSPARENT_GREEN)
  {
    return 1;
  }
  asum=0;
  bsum=0;
  for(i=0;i<3;i++)
  {
    tmp=*(((BYTE *) a)+i);
    asum+=squares[tmp];
    tmp=*(((BYTE *) b)+i);
    bsum+=squares[tmp];
  }
  if(asum>bsum) return -1;
  return asum<bsum;
}

DWORD INF_MOS_FRAMEDATA::GetColor(int x, int y)
{
  return Palette[pFrameData[x+y*nWidth]];
}

void INF_MOS_FRAMEDATA::OrderPalette()
{
  BYTE *pClr;
  int i;

  //mark palette so we'll know the original order
  for(i=0;i<256;i++)
  {
    pClr = (BYTE *) (Palette+i);
    *(pClr+3) = (unsigned char) i;
  }
  qsort(Palette, 256, 4, colourorder);
  ReorderPixels();
  //change the palette back to clean
  for(i=0;i<256;i++)
  {
    pClr = (BYTE *) (Palette+i);
    *(pClr+3) = 0;
  }
}

void INF_MOS_FRAMEDATA::AlterPalette(int r, int g, int b, int strength)
{
  BYTE *pClr;
  int i;

  //don't mess with the first entry
  for(i=0;i<256;i++)
  {
    if (Palette[i]==TRANSPARENT_GREEN) continue;
    pClr = (BYTE *) (Palette+i);
    pClr[0] = (BYTE) ((pClr[0]*r)>>8);
    pClr[1] = (BYTE) ((pClr[1]*g)>>8);
    pClr[2] = (BYTE) ((pClr[2]*b)>>8);
    pClr[3] = (BYTE) ((pClr[3]*strength)>>8);
  }
}

void INF_MOS_FRAMEDATA::ReorderPixels()
{
  int idx, color;
  int i;

  for(i=0;i<nFrameSize;i++)
  {
    color=pFrameData[i];
    for(idx=0;idx<256;idx++)
    {
      if(*((BYTE *) (Palette+idx)+3)==color )
      {
        pFrameData[i]=(BYTE) idx;
        break;
      }
    }
  }
}

void INF_MOS_FRAMEDATA::MarkPixels()
{
  BYTE *pClr;
  int color;
  int i;

  //mark palette so we'll know which are unused
  for(i=0;i<256;i++)
  {
    *((BYTE *) (Palette+i)+3)=1;
  }
  for(i=0;i<nFrameSize;i++)
  {
    color=pFrameData[i];
    *((BYTE *) (Palette+color)+3)=0;
  }
  //drop unused entries (turning them black)
  for(i=0;i<256;i++)
  {
    pClr = (BYTE *) (Palette+i);
    if(*(pClr+3) ) memset(pClr,0,sizeof(COLORREF));
    else *(pClr+3) = 0;
  }
}

int INF_MOS_FRAMEDATA::TakePaletteData(const BYTE *pRawBits)
{
  memcpy(&Palette,pRawBits,sizeof(Palette) );
  return 0;
}

int INF_MOS_FRAMEDATA::TakeBmpData(const BYTE *pRawBits, int row, int offset)
{
  memcpy(pFrameData+row*nWidth,pRawBits+offset,nWidth);
  return nWidth;
}

//color reduction using Ian Ashdown's octree algorithm

int INF_MOS_FRAMEDATA::TakeWholeBmpData(const DWORD *pRawBits)
{
  OctQuant oc; 

  return oc.BuildTree(pRawBits,pFrameData, CPoint(nHeight,nWidth), Palette);
}

bool INF_MOS_FRAMEDATA::IsTransparent(DWORD x, DWORD y)
{
  if(x>=nWidth) return true;
  if(y>=nHeight) return true;
  return !pFrameData[y*nWidth+x];
}

int INF_MOS_FRAMEDATA::TakeMosData(const BYTE *pRawBits, DWORD width, DWORD height, bool deepen)
{
  DWORD x,y;
  
  nWidth=width;
  nHeight=height;
  nFrameSize=nWidth*nHeight;
  if(pFrameData) delete [] pFrameData;
  pFrameData=new BYTE[nFrameSize];
  if(!pFrameData) return -3;             //memory problem
  if(pRawBits)                           //null pointer means only allocation
  {
    memcpy(pFrameData, pRawBits, nFrameSize);
    if(deepen)
    {
      for(y=0;y<height;y++) for(x=0;x<width;x++)
      {
        if(IsTransparent(x-1,y) && IsTransparent(x+1,y) &&
          IsTransparent(x,y-1) && IsTransparent(x,y+1) )
        {
          pFrameData[y*width+x]=0;
        }
      }
    }
  }
  else
  {
    memset(pFrameData, 0, nFrameSize);
  }
  return 0;
}

int INF_MOS_FRAMEDATA::ExpandMosLine(char *pBuffer, int nSourceOff)
{
  DWORD i;

  for(i=0;i<nWidth;i++)
  {
    memcpy(pBuffer,Palette+pFrameData[nSourceOff],3);
    pBuffer+=3;
    nSourceOff++;
  }
  return 3*nWidth;
}

bool INF_MOS_FRAMEDATA::ExpandMosBitsWhole(COLORREF clrReplace, COLORREF clrTransparent, COLORREF *pDIBits, int nOffset, int wWidth)
{
  int nCount;
	int nPixelCount;

  nCount=nWidth;
	for(nPixelCount=0;nPixelCount < nFrameSize;nPixelCount++)
	{
    // If it is not compressed, still need to catch the transparent pixels and
    // fill with the transparent color.
    if(memcmp(Palette+pFrameData[nPixelCount],&clrReplace, 3) )
    {
      pDIBits[nPixelCount+nOffset]=Palette[pFrameData[nPixelCount]];
    }
    else
    {
      if(clrTransparent!=TRANSPARENT_GREEN)
      {
        pDIBits[nPixelCount+nOffset]=clrTransparent;
      }
    }
    nCount--;
    if(!nCount) //reached end of line
    {
      nOffset+=wWidth-nWidth;
      nCount=nWidth;      
    }
	}
  return true;
}

int INF_MOS_FRAMEDATA::SaturateTransparency()
{
  unsigned char pixel;
  DWORD x,y;
  int ret;

  ret=0;
  for(y=0;y<nHeight;y++) for(x=0;x<nWidth;x++)
  {
    if(!pFrameData[y*nWidth+x])
    {
      if(x) pixel = pFrameData[y*nWidth+x-1];
      else pixel = pFrameData[y*nWidth+x+1];
      pFrameData[y*nWidth+x]=pixel; //second attempt to remove transparency
      ret=1;
    }
  }
  return ret;
}

void INF_MOS_FRAMEDATA::AllocateTransparency()
{
  DWORD x,y;  

  if(Palette[0]==TRANSPARENT_GREEN) return;
  Palette[255]=Palette[0];
  Palette[0]=TRANSPARENT_GREEN;
  
  for(y=0;y<nHeight;y++) for(x=0;x<nWidth;x++)
  {
    if(!pFrameData[y*nWidth+x])
    {
      pFrameData[y*nWidth+x]=255;
    }
  }
  return;
}

void INF_MOS_FRAMEDATA::FlipTile()
{
  unsigned char *pos;
  DWORD x,y;
  unsigned char tmp;

  if(!pFrameData) return;
  pos=pFrameData;
  for(y=0;y<nHeight;y++)
  {
    for(x=0;x<nWidth/2;x++)
    {
      tmp=pos[x];
      pos[x]=pos[nWidth-x-1];
      pos[nWidth-x-1]=tmp;
    }
    pos+=nWidth;
  }
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Cmos::Cmos()
{
  memset(&mosheader,0,sizeof(INF_MOS_HEADER) );
  memset(&tisheader,0,sizeof(tis_header) );
  memset(&c_header,0,sizeof(INF_MOSC_HEADER) );
  memcpy(&mosheader,"MOS V1  ",8);
  memcpy(&c_header,"MOSCV1  ",8);
  m_pFrameDataPointer=NULL;
  m_pOffsets=NULL;
  m_pclrDIBits=NULL;
  m_DIBsize=0;
  m_pData=NULL;
  m_overlay=0;
  m_drawclosed=0;
  m_tileheaders=NULL;
  m_tileindices=NULL;
  memset(m_friend,0,sizeof(m_friend));
}

Cmos::~Cmos()
{
  new_mos();
}

void Cmos::new_mos()
{
  KillData();
  KillFrameData();
  KillOffsets();
  if(m_pclrDIBits)
  {
    delete [] m_pclrDIBits;
    m_pclrDIBits=NULL;
    m_DIBsize=0;
  }
  memset(&mosheader,0,sizeof(INF_MOS_HEADER) );
  memset(&tisheader,0,sizeof(tis_header) );
  memset(&c_header,0,sizeof(INF_MOSC_HEADER) );
  memcpy(&mosheader,"MOS V1  ",8);
  memcpy(&c_header,"MOSCV1  ",8);
  FreeOverlay();
  m_drawclosed=0;
  m_tileheaders=NULL;
  m_tileindices=NULL;
  mosheader.dwBlockSize=64;
  m_name.Empty();
  m_changed=false;
}

int Cmos::RetrieveTisFrameCount(int fhandle, loc_entry *fl)
{
  if(fhandle)
  {
    lseek(fhandle, 0, SEEK_SET);
    read(fhandle, &tisheader, sizeof(tis_header) );
    return tisheader.numtiles;
  }

  if (fl)
  {
    return fl->numtiles;
  }

  return -1;
}

int Cmos::GetFrameCount()
{
  return tisheader.numtiles;
}

void Cmos::FreeOverlay()
{
  int i;

  for(i=0;i<10;i++)
  {
    if(m_friend[i]) delete m_friend[i];
    m_friend[i]=NULL;
  }
  m_overlay=0;
  m_tileheaders=NULL;
  m_tileindices=NULL;
}

void Cmos::SetTiles(wed_tilemap *tileheaders, short *tileindices)
{
  m_tileheaders=tileheaders;
  m_tileindices=tileindices;
}

void Cmos::SetOverlay(int overlay, Cmos *overlaymos)
{
  if(overlay>0 && overlay<10)
  {
    if(overlaymos) m_overlay|=1<<overlay;
    else m_overlay&=~(1<<overlay);
    if(m_friend[overlay]) delete m_friend[overlay];
    m_friend[overlay]=overlaymos;
  }
  else delete overlaymos;
}

int Cmos::ResolveFrameNumber(int framenumber)
{
  int idx;

  if(!m_tileheaders)
  {
    m_overlaytile=0;
    return framenumber;
  }
  m_overlaytile=m_overlay & m_tileheaders[framenumber].overlayflags;
  if(m_drawclosed)
  {
    idx=m_tileheaders[framenumber].alternate;
    if (idx!=-1) return idx;
  }
  idx=m_tileheaders[framenumber].firsttileprimary;
  if(m_tileindices) return m_tileindices[idx];
  return idx;
}

INF_MOS_FRAMEDATA *Cmos::GetFrameData(DWORD nFrameWanted)
{
  if(nFrameWanted>=tisheader.numtiles) return NULL;
  INF_MOS_FRAMEDATA *p=m_pFrameDataPointer[nFrameWanted];
  if(!p)
  {
    p=m_pFrameData.GetAt(m_pFrameData.FindIndex(nFrameWanted));
    m_pFrameDataPointer[nFrameWanted]=p;
  }
  return p;
}

CPoint Cmos::GetFrameSize(DWORD nFrameWanted)
{
  INF_MOS_FRAMEDATA *p=GetFrameData(nFrameWanted);
  if(!p)
  {
    return (DWORD) -1;
  }
  return CPoint(p->nWidth,p->nHeight);
}

unsigned char *Cmos::GetFrameBuffer(DWORD nFrameWanted)
{
  INF_MOS_FRAMEDATA *p=GetFrameData(nFrameWanted);
  if(!p)
  {
    return NULL;
  }
  return p->pFrameData;
}

COLORREF Cmos::GetPixelData(int x, int y)
{
  int nFrameWanted;
  INF_MOS_FRAMEDATA *p;
  int offset;
  COLORREF val;
  BYTE *pClr1, *pClr2;
  int ov;

  //creates m_overlaytile
  nFrameWanted=ResolveFrameNumber(y/tisheader.pixelsize*mosheader.wColumn+x/tisheader.pixelsize);
  p=GetFrameData(nFrameWanted);
  if (!p)
  {
    //this is an invalid condition, no such tile
    val = TRANSPARENT_GREEN;
    return val;
  }
  offset = y%tisheader.pixelsize*p->nWidth+x%tisheader.pixelsize;
  val = p->Palette[p->pFrameData[offset]];
  if (!m_overlaytile)
  {
    return val;
  }

  if (bg1_compatible_area())
  {
    if (val!=TRANSPARENT_GREEN) return val;
  }
  for(ov=1;ov<10;ov++)
  {
    if((m_overlaytile&(1<<ov) ) && m_friend[ov])
    {
      INF_MOS_FRAMEDATA *f=m_friend[ov]->GetFrameData(0);
      COLORREF val2 = f->Palette[f->pFrameData[offset]];
      if (val==TRANSPARENT_GREEN)
      {
        val=val2;
      }
      else if (val2!=TRANSPARENT_GREEN)
      {
        if (!bg1_compatible_area())
        {
          pClr1 = (BYTE *) &val;
          pClr2 = (BYTE *) &val2;
          pClr1[0] =(BYTE) ((pClr1[0] + pClr2[0])/2);
          pClr1[1] =(BYTE) ((pClr1[1] + pClr2[1])/2);
          pClr1[2] =(BYTE) ((pClr1[2] + pClr2[2])/2);        
          return val;
        }
      }
    }
  }
  return val;
}

int Cmos::TakeOneFrame(DWORD nFrameWanted, COLORREF *prFrameBuffer, int factor)
{
  int r,g,b; //color sums
  DWORD x,y;
  int z,w;
  int point;
  COLORREF color;
  int sz, sw;
  int oz, ow;
  int rf;

	if (tisheader.numtiles <= nFrameWanted )  //not enough frames
		return -1;

  point=0;
  sw=nFrameWanted/mosheader.wColumn*tisheader.pixelsize*tisheader.pixelsize/factor;
  sz=nFrameWanted%mosheader.wColumn*tisheader.pixelsize*tisheader.pixelsize/factor;
  rf=tisheader.pixelsize/factor;

  m_overlaytile=0;
  INF_MOS_FRAMEDATA *p=GetFrameData(nFrameWanted);
  for(y=0;y<p->nHeight;y++)
  { 
    for(x=0;x<p->nWidth;x++)
    {
      r=g=b=0;
      oz=sz+x*tisheader.pixelsize/factor;
      for(z=oz;z<oz+rf;z++)
      {
        ow=sw+y*tisheader.pixelsize/factor;
        for(w=ow;w<ow+rf;w++)
        {/////get the color value and sum it up
          color=the_mos.GetPixelData(z,w);
          r+=*(BYTE *) &color;
          g+=*(((BYTE *) &color)+1);
          b+=*(((BYTE *) &color)+2);
        }
      }
      z=rf*rf;
      prFrameBuffer[point++]=RGB(r/z,g/z,b/z); //average
    }
  }
  return p->TakeWholeBmpData(prFrameBuffer);
}

unsigned long *Cmos::GetFramePalette(DWORD nFrameWanted)
{
  INF_MOS_FRAMEDATA *p=GetFrameData(nFrameWanted);
  if(!p)
  {
    return NULL;
  }
  return p->Palette;
}

bool Cmos::MakeBitmap(COLORREF clrTrans, Cmos &host, int nMode, int nXpos, int nYpos)
{
  int nColumn, nRow, nBase, nOffset, nLimit;
  DWORD nFrameWanted;
  int ov;

  if(!(nMode&BM_OVERLAY)) return false;
  if(nMode&BM_MATCH) clrTrans=(DWORD) -1;
  else if(nMode&BM_INVERT) clrTrans=(DWORD) -2;
  //the size of the original bitmap
  nColumn=host.mosheader.wColumn*host.mosheader.dwBlockSize;
  nRow=host.mosheader.wRow*host.mosheader.dwBlockSize;
  nBase=nXpos+nYpos*nColumn;
  nLimit=host.mosheader.wRow-(nYpos+63)/host.mosheader.dwBlockSize;

  for(nYpos=0;nYpos<mosheader.wRow && nYpos<nLimit;nYpos++)
  {
    nOffset=nBase+mosheader.dwBlockSize*nColumn*nYpos;
    for(nXpos=0;nXpos<mosheader.wColumn;nXpos++)
    {
      m_overlaytile=0;
      nFrameWanted=ResolveFrameNumber(nYpos*mosheader.wColumn+nXpos);
      for(ov=1;ov<10;ov++)
      {
        if((m_overlaytile&(1<<ov) ) && m_friend[ov])
        {
          INF_MOS_FRAMEDATA *f=m_friend[ov]->GetFrameData(0);
          f->ExpandMosBitsWhole(TRANSPARENT_GREEN,clrTrans,host.GetDIB(),nOffset, nColumn);
        }
      }
      INF_MOS_FRAMEDATA *p=GetFrameData(nFrameWanted);
      if(p)
      {        
        if(!p->ExpandMosBitsWhole(TRANSPARENT_GREEN,m_overlaytile!=0?TRANSPARENT_GREEN:clrTrans,host.GetDIB(),nOffset, nColumn))
        {
          return false;
        }
      }      
      nOffset+=mosheader.dwBlockSize;
    }
  }
  return true;
}

bool Cmos::MakeBitmapWhole(COLORREF clrTrans, HBITMAP &hBitmap, int clipx, int clipy, int maxclipx, int maxclipy, bool norender)
{
  int nXpos,nYpos;
  int nOffset;
  DWORD nFrameWanted;
  bool ret;
  int ov;

  if(hBitmap)
  {
    DeleteObject(hBitmap);
    hBitmap=NULL;
  }

  if(!maxclipx || maxclipx*mosheader.dwBlockSize>(DWORD) mosheader.wWidth+63)
  {
    maxclipx=(mosheader.wWidth+63)/mosheader.dwBlockSize;
  }
  if(!maxclipy || maxclipy*mosheader.dwBlockSize>(DWORD) mosheader.wHeight+63)
  {
    maxclipy=(mosheader.wHeight+63)/mosheader.dwBlockSize;
  }
  m_pixelwidth=(maxclipx-clipx)*mosheader.dwBlockSize;
  m_pixelheight=(maxclipy-clipy)*mosheader.dwBlockSize;
  if(m_pixelwidth<0) m_pixelwidth=0;
  if(m_pixelheight<0) m_pixelheight=0;

  nXpos=m_pixelwidth*m_pixelheight;
  //check for overflow
  if(nXpos<=0) return false;
  if(m_DIBsize!=nXpos)
  {
    if(m_pclrDIBits) delete [] m_pclrDIBits;
    m_pclrDIBits=new COLORREF[nXpos];
    if(!m_pclrDIBits) return false;
    m_DIBsize=nXpos;
  }

  //fill area with base transparency
  while(nXpos--)
  {
    m_pclrDIBits[nXpos]=clrTrans;
  }
  
  for(nYpos=0;(nYpos+clipy<maxclipy) /*&& (nYpos<VIEW_MAXEXTENT)*/;nYpos++)
  {
    nOffset=mosheader.dwBlockSize*m_pixelwidth*nYpos;
    for(nXpos=0;(nXpos+clipx<maxclipx) /*&& (nXpos<VIEW_MAXEXTENT)*/;nXpos++)
    {
      nFrameWanted=ResolveFrameNumber((nYpos+clipy)*mosheader.wColumn+nXpos+clipx);
      for(ov=8;ov>0;ov--)
      {
        if(m_overlaytile&(1<<ov) )
        {
          if(m_friend[ov])
          {
            INF_MOS_FRAMEDATA *f=m_friend[ov]->GetFrameData(0);
            f->ExpandMosBitsWhole(TRANSPARENT_GREEN,clrTrans,m_pclrDIBits,nOffset, m_pixelwidth);
          }
        }
      }
      INF_MOS_FRAMEDATA *p=GetFrameData(nFrameWanted);
      if(p)
      {
        if (!p->ExpandMosBitsWhole(TRANSPARENT_GREEN,m_overlaytile?TRANSPARENT_GREEN:clrTrans,m_pclrDIBits,nOffset, m_pixelwidth))
        {
          delete [] m_pclrDIBits;
          m_pclrDIBits=NULL;
          m_DIBsize=0;
          return false;
        }
      }      
      nOffset+=mosheader.dwBlockSize;
    }
  }

  if(norender) return false;

  ret=MakeBitmapExternal(m_pclrDIBits,m_pixelwidth,m_pixelheight,hBitmap);

  return ret;
}

int Cmos::MakeTransparent(DWORD nFrameWanted, DWORD redgreenblue, int limit)
{
  int i;
  int nHits;

  INF_MOS_FRAMEDATA *p=GetFrameData(nFrameWanted);
  if(!p)
  {
    return -1;
  }
  nHits=0;
  // don't force transparent index to be 0
  for(i=0;i<256;i++)
  {
    if(ChiSquare((BYTE *) &redgreenblue,(BYTE *) (p->Palette+i))<=limit )
    {
      if (p->Palette[i]!=TRANSPARENT_GREEN)
      {
        p->Palette[i]=TRANSPARENT_GREEN;
        nHits++;
      }
    }
  }  
  return nHits;
}

//write pvr and tis files
/*
int Cmos::WritePvrToFile(int fhandle, int clipx, int clipy, int maxclipx, int maxclipy)
{
  Cpvr pvr;
  int ret;

  ret = 0;
  memcpy(&tisheader.filetype,"TIS V2  ",8);

  return ret;
}
*/

int Cmos::WriteTisToFile(int fhandle, int clipx, int clipy, int maxclipx, int maxclipy)
{
  int esize;
  DWORD i;
  DWORD x,y;
  BYTE *zerobuffer;
  int ret;
  DWORD tmpsave, newcount;
  DWORD nFrameWanted;

  esize=tisheader.pixelsize*tisheader.pixelsize;
  zerobuffer=new BYTE[esize];
  if(!zerobuffer) return -3;
  memset(zerobuffer,0,esize);
  ret=0;
  //this hack makes sure the header contains as many tiles as we wanted
  memcpy(&tisheader.filetype,"TIS V1  ",8);
  tisheader.offset=sizeof(tisheader);
  tmpsave=tisheader.numtiles;
  if(maxclipx!=-1)
  {
    tisheader.numtiles=(maxclipy-clipy+1)*(maxclipx-clipx+1);
  }
  if(write(fhandle, &tisheader, sizeof(tis_header) )!=sizeof(tis_header) )
  {
    ret=-2;
    goto endofquest;
  }
  newcount = tisheader.numtiles;
  tisheader.numtiles = tmpsave;

  //end of hack
  //this hack makes sure we write out the tiles we wanted
  //maximum contains the last tile
  for(i=0;i<newcount;i++)
  {
    if(maxclipx!=-1)
    {
      y=i/(maxclipx-clipx+1);
      x=i%(maxclipx-clipx+1);
      nFrameWanted=ResolveFrameNumber( (y+clipy)*mosheader.wColumn+x+clipx);
    }
    else nFrameWanted=i;
    INF_MOS_FRAMEDATA *p=GetFrameData(nFrameWanted);
    esize=sizeof(palettetype); //sizeof palettetype
    if(write(fhandle,p->Palette,esize )!=esize)
    {
      ret=-2;
      goto endofquest;
    }
    esize=tisheader.pixelsize*tisheader.pixelsize;
    if(esize!=p->nFrameSize)
    {
      //outputting incomplete tiles
      ret=1;
      for(y=0;y<p->nHeight;y++)
      {
        esize-=write(fhandle,p->pFrameData+y*p->nWidth,p->nWidth);
        esize-=write(fhandle,zerobuffer,tisheader.pixelsize-p->nWidth);
      }
      esize-=write(fhandle,zerobuffer,(tisheader.pixelsize-y)*tisheader.pixelsize);
      if(esize)
      {
        ret=-2;
        goto endofquest;
      }
    }
    else
    {
      //outputting complete tile
      if(write(fhandle,p->pFrameData,esize)!=esize)
      {
        ret=-2;
        goto endofquest;
      }
    }
  }
endofquest:
  delete [] zerobuffer;
  return ret;
}

int Cmos::GetImageWidth(int clipx, int maxclipx)
{
  if(maxclipx==-1 && clipx==-1)
  {
    return mosheader.wWidth;
  }
  INF_MOS_FRAMEDATA *p=GetFrameData(maxclipx);
  if(!p) return -1;
  return (maxclipx-clipx)*mosheader.dwBlockSize+p->nWidth;
}

int Cmos::GetImageHeight(int clipy, int maxclipy)
{
  if(maxclipy==-1 && clipy==-1)
  {    
    return mosheader.wHeight;    
  }
  INF_MOS_FRAMEDATA *p=GetFrameData(mosheader.wColumn*maxclipy);
  if(!p) return -1;
  return (maxclipy-clipy)*mosheader.dwBlockSize+p->nHeight;
}

int Cmos::CollectFrameData(int fhandle, DWORD *prFrameBuffer, LPBYTE pcBuffer, DWORD height,
                           DWORD scanline, DWORD nFrameCol, DWORD cols, DWORD rows)
{
  DWORD x,y;
  
  for(y=0;y<tisheader.pixelsize;y++)
  {
    if(height<y) break;
    lseek(fhandle,nFrameCol+(height-y)*scanline,SEEK_SET);
    if(read(fhandle,pcBuffer,cols*rows)!=(int) (cols*rows))
    {
      return -2;
    }
    
    for(x=0;x<cols;x++)
    {
      //pcBuffer consists of bytes, so we multiply x with 3 or 4
      memcpy(prFrameBuffer+y*cols+x,pcBuffer+x*rows,rows);
    }     
  }
  return 0;
}

int Cmos::GetFrameData(DWORD *prFrameBuffer, LPBYTE pRawData, DWORD height,
                       DWORD scanline, DWORD nFrameCol, 
                       DWORD cols, DWORD rows)
{
  DWORD x,y;
  
  for(y=0;y<tisheader.pixelsize;y++)
  {
    if(height<y) break;
    for(x=0;x<cols;x++)
    {
      //pcBuffer consists of bytes, so we multiply x with 3 or 4
      memcpy(prFrameBuffer+y*cols+x,pRawData+nFrameCol+(height-y)*scanline+x*rows,rows);
    }     
  }
  return 0;
}

int Cmos::ReducePalette(int fhandle, bmp_header &sHeader, LPBYTE pcBuffer,
                        DWORD scanline, DWORD nSourceOff)
{
  DWORD *prFrameBuffer=NULL;        //buffer for the whole frame (raw form)
  LPBYTE pRawData=NULL;
  int ret;
  DWORD rows,cols;
  DWORD nFrameRow;               //the pixel row (scanline) of this frame
  DWORD nFrameCol;               //
  DWORD nFrameWanted;

  //the size of the pixel (3 or 4)
  rows=sHeader.bits>>3;

  //allocate a temporary buffer for the raw frame data
  prFrameBuffer=new COLORREF [tisheader.pixelsize*tisheader.pixelsize];

  if(!prFrameBuffer)
  {
    ret=-3;
    goto endofquest;
  }

  //we try to load the whole picture into memory,
  //if we don't succeed we'll load it in pieces
  pRawData=new BYTE [sHeader.height*scanline];
  if(pRawData)
  {
    if(read(fhandle,pRawData,sHeader.height*scanline)!=(int) (sHeader.height*scanline) )
    {
      ret=-2;
      goto endofquest;
    }
  }

  ret=0;
  ((CChitemDlg *) AfxGetMainWnd())->start_progress(tisheader.numtiles, "Reducing palette...");
  
  memset(prFrameBuffer,0,tisheader.pixelsize*tisheader.pixelsize*sizeof(DWORD) );
  //generate palette for frames
  //if a color collides, make intelligent color reduction
  //if it is only 3 bytes expand palette to 4 bytes
  for(nFrameWanted=0;nFrameWanted<tisheader.numtiles;nFrameWanted++)
  {
    ((CChitemDlg *) AfxGetMainWnd())->set_progress(nFrameWanted); 
    //collect a frame's data in prFrameBuffer
    //cols is the pixel number of the frame
    INF_MOS_FRAMEDATA *p=GetFrameData(nFrameWanted);
    cols=p->nWidth;
    nFrameRow=tisheader.pixelsize*(nFrameWanted/mosheader.wColumn);
    nFrameCol=nFrameWanted%mosheader.wColumn*tisheader.pixelsize*rows;
    //cols is the width of the frame, rows is the size of a pixel
    if(pRawData)
    {
      GetFrameData(prFrameBuffer, pRawData, sHeader.height-nFrameRow-1,
        scanline, nFrameCol, cols, rows);
    }
    else
    {
      ret=CollectFrameData(fhandle, prFrameBuffer, pcBuffer, sHeader.height-nFrameRow-1,
        scanline, nSourceOff+nFrameCol, cols, rows);
    }
    if(ret) goto endofquest;

    //generates a palette and palette indices from raw binary data
    m_nQualityLoss+=p->TakeWholeBmpData(prFrameBuffer);
  }
  m_nQualityLoss=m_nQualityLoss/nFrameWanted;
endofquest:
  if(prFrameBuffer) delete [] prFrameBuffer;
  if(pRawData) delete [] pRawData;
  ((CChitemDlg *) AfxGetMainWnd())->end_progress();
  return ret;
}

bool Cmos::CreateFrames()
{
  DWORD y;

  y=tisheader.numtiles;
  KillFrameData();
  tisheader.numtiles=y;
  m_pFrameDataPointer=(INF_MOS_FRAMEDATA **) calloc(tisheader.numtiles, sizeof(INF_MOS_FRAMEDATA *));
  if(!m_pFrameDataPointer)
  {
    return false;
  }
  for(y=0;y<tisheader.numtiles;y++)  
  {
    INF_MOS_FRAMEDATA *p=new INF_MOS_FRAMEDATA;
    m_pFrameDataPointer[y]=p;
    if(!p)
    {
      KillFrameData();
      return false;
    }
    m_pFrameData.AddTail(p);
  }
  return true;
}

//reads and converts a bitmap to .tis or .mos format
//BMP header: BITMAPFILEHEADER + BITMAPINFOHEADER
int Cmos::ReadBmpFromFile(int fhandle, int ml)
{
  bmp_header sHeader;       //BMP header
  LPBYTE pcBuffer;          //buffer for a scanline
  DWORD scanline;    //size of a scanline in bytes (ncols * bytesize of a pixel)
  palettetype sPalette;     //256 * bytesize of color
  DWORD palettesize; //size of the palette in file (ncolor * bytesize of color)
  long original;
  DWORD cols, rows;
  DWORD x,y;
  DWORD nFrameWanted;//the number of the tile of the mos/tis
  DWORD nFrameRow;   //the pixel row (scanline) of this frame
  DWORD nSourceOff;  //the source offset in the original scanline (which we cut up)
  int ret;

  new_mos();
  m_nQualityLoss=0;
  pcBuffer=NULL;
  memset(&sPalette,0,sizeof(sPalette) );
  original=tell(fhandle);
  if(ml<0) ml=filelength(fhandle);
  if(ml<0) return -2;
  if(read(fhandle,&sHeader,sizeof(bmp_header))!=sizeof(bmp_header) )
  {
    return -2;
  }
  if((*(unsigned short *) sHeader.signature)!='MB') return -2; //BM
  if(sHeader.fullsize>(DWORD) ml) return -2;
  if(sHeader.headersize!=0x28) return -2;
  if(sHeader.planes!=1) return -1;                  //unsupported
  if(sHeader.compression!=BI_RGB) return -1;        //unsupported
  if(sHeader.height<0 || sHeader.width<0) return -1; //unsupported
  if(sHeader.bits==4) return -1; //unsupported for mos, tis
  //read palette if we got one
  if(sHeader.bits==8)
  {
    if(!sHeader.colors)
    {
      sHeader.colors=256;
    }
    palettesize=sHeader.colors*sizeof(RGBQUAD);
    if(read(fhandle,&sPalette, palettesize)!=(int) palettesize)
    {
      return -2;
    }
  }
  else
  {
    palettesize=0;
  }
  //check if the file fits in the claimed length
  if(sHeader.offset<palettesize+sHeader.headersize+((unsigned char *) &sHeader.headersize-(unsigned char *) &sHeader) ) return -2;
  scanline=sHeader.width*(sHeader.bits>>3);
  if(scanline&3) scanline+=4-(scanline&3);
  if(scanline*sHeader.height+sHeader.offset>sHeader.fullsize) return -2;
  //check if we are on the position we wanted to be
  nSourceOff=tell(fhandle);
  if(sHeader.offset!=nSourceOff-original ) return -2;

  //create the frames, set up the internal headers
  //a bit hackish
  tisheader.offset = sizeof(tisheader);
  tisheader.pixelsize = 64;
  cols=(sHeader.width+tisheader.pixelsize-1)/tisheader.pixelsize;
  rows=(sHeader.height+tisheader.pixelsize-1)/tisheader.pixelsize;
  tisheader.numtiles = cols*rows;
  if (m_pvr)
  {
    tisheader.tilelength = 12;
  }
  else
  {
    tisheader.tilelength = sizeof(palettetype)+tisheader.pixelsize*tisheader.pixelsize;
  }
  
  TisToMos(cols,rows);
  mosheader.wWidth=(WORD) (sHeader.width);
  mosheader.wHeight=(WORD) (sHeader.height);
  m_nResX=mosheader.wWidth%mosheader.dwBlockSize;
  m_nResY=mosheader.wHeight%mosheader.dwBlockSize;
  if(!tisheader.numtiles) return -2;

//quest started, go to endofquest if in trouble
  m_pclrDIBits=new COLORREF[tisheader.pixelsize*tisheader.pixelsize];
  if(!m_pclrDIBits)
  {
    ret=-3;
    goto endofquest;
  }

  if(!CreateFrames())
  {
    ret=-3;
    goto endofquest;
  }
  pcBuffer=new BYTE[scanline];
  if(!pcBuffer)
  {
    ret=-3;
    goto endofquest;
  }

  //lets allocate memory for all frames
  for(y=0;y<tisheader.numtiles;y++)
  {
    INF_MOS_FRAMEDATA *p=GetFrameData(y);
    //if it has a palette then we take it now
    if(sHeader.bits==8) p->TakePaletteData((LPBYTE) &sPalette);
    //calculate the size of this frame
    if(!((y+1)%mosheader.wColumn))
    {
      cols=m_nResX;
      if(!cols) cols=tisheader.pixelsize;
    }
    else cols=tisheader.pixelsize;
    if(y>=(DWORD) (mosheader.wRow-1)*mosheader.wColumn)
    {
      rows=m_nResY;
      if(!rows) rows=tisheader.pixelsize;
    }
    else rows=tisheader.pixelsize;
    ret=p->TakeMosData(0,cols,rows, false); //setting the size of the frame now
    if(ret) goto endofquest; //TakeMosData allocates memory, if it failed we escape
  }
  //now we just have to fill the palettes and bitmap frames
  ret=-1; //unsupported mode
  switch(sHeader.bits)
  {
  case 8: //this format has a palette
    //copy palettes to all frame palettes
    //throw bitmaps into separate frames, y starts from bottom up
    for(y=0;y<mosheader.wHeight;y++)
    {
      if(read(fhandle, pcBuffer, scanline)!=(int ) scanline)
      {
        ret=-2;
        goto endofquest;
      }
      //which frame gets this stuff?
      nFrameWanted=(sHeader.height-y-1)/tisheader.pixelsize*mosheader.wColumn;
      nFrameRow=(sHeader.height-y-1)%tisheader.pixelsize;
      nSourceOff=0;
      for(x=0;x<mosheader.wColumn;x++)
      {
        INF_MOS_FRAMEDATA *p=GetFrameData(nFrameWanted);
        nSourceOff+=p->TakeBmpData(pcBuffer, nFrameRow, nSourceOff);
        nFrameWanted++;
      }
    }
    ret=0;
    break;
  case 24: case 32: //loading the file in small pieces, only one frame at a time
    ret=ReducePalette(fhandle, sHeader, pcBuffer, scanline, nSourceOff);
    break;
  }
endofquest:
  if(pcBuffer) delete [] pcBuffer;
  if(ret) new_mos(); //destroy crippled structures
  return ret; //return value
}

int Cmos::WriteTisToBmpFile(int fhandle, int clipx, int clipy, int maxclipx, int maxclipy)
{
  char *pcBuffer;
  int maxx, maxy;
  int x,y;
  int nFrameWanted;
  int nSourceOff;
  int nScanLine;
  int nFullLine;
  int ret;

  ret=0;
  if(maxclipx==-1)
  {
    maxclipx=mosheader.wColumn-1;
  }
  if(maxclipy==-1)
  {
    maxclipy=mosheader.wRow-1;
  }
  maxx=GetImageWidth(clipx, maxclipx);
  maxy=GetImageHeight(clipy, maxclipy);
  nScanLine=GetScanLineLength(maxx, 24);
  pcBuffer=new char[nScanLine];
  if(!pcBuffer) return -3;
  memset(pcBuffer,0,nScanLine);
  if(!CreateBmpHeader(fhandle, maxx, maxy, 3))
  {
    ret=-2;
    goto endofquest;
  }
  for(y=maxy-1;y>=0;y--) //backwards???
  {
    nFullLine=0;
    for(x=clipx;x<=maxclipx;x++)
    {
      nFrameWanted=ResolveFrameNumber((y/mosheader.dwBlockSize+clipy)*mosheader.wColumn+x);
      INF_MOS_FRAMEDATA *p=GetFrameData(nFrameWanted);
      nSourceOff=p->nWidth*(y%mosheader.dwBlockSize);
      nFullLine+=p->ExpandMosLine(pcBuffer+nFullLine,nSourceOff);
    }
    if(write(fhandle,pcBuffer,nScanLine)!=nScanLine)
    {
      ret=-2;
      goto endofquest;
    }
  }
endofquest:
  delete [] pcBuffer;
  return ret;
}

int Cmos::WriteMosToFile(int fhandle)
{
  BYTE *pCData;
  int ret;
  unsigned long tmpsize;

  if(ImplodeMosData()) return -4; //internal error
  if(m_bCompressed)
  {
    tmpsize=c_header.nExpandSize; //allocate space 
    pCData=new BYTE[tmpsize];
    if(!pCData) return -3; // out of memory
    ret=compress(pCData,&tmpsize,m_pData, c_header.nExpandSize); //nDatasize is the uncompressed length
    if(!ret) //we couldn't compress it
    {
      if(write(fhandle,&c_header, sizeof(c_header))!=sizeof(c_header) )
      {
        delete [] pCData;
        KillData();
        return -2;
      }
      if(write(fhandle, pCData, tmpsize)!=(int) tmpsize)
      {
        delete [] pCData;
        KillData();
        return -2;
      }
      delete [] pCData;
      KillData();
      return 0;
    }
    delete [] pCData;
//we cannot compress it, ok, lets save it as uncompressed
    m_bCompressed=false;
    lseek(fhandle,0,SEEK_SET);
  }
  if(write(fhandle, m_pData, m_nDataSize)!=(int) m_nDataSize)
  {
    KillData();
    return -2;
  }
  KillData();
  m_changed=false;
  return 0;
}

void Cmos::SetTisParameters(loc_entry *fl, int size)
{
  if (!fl || (fl->size==-1) )
  {
    tisheader.tilelength=sizeof(palettetype)+tisheader.pixelsize*tisheader.pixelsize;
    tisheader.numtiles=size/tisheader.tilelength;
  }
  else
  {
    size = fl->size;
    tisheader.tilelength = fl->tilesize;
    tisheader.numtiles = fl->numtiles;
  }

  tisheader.offset = sizeof(tisheader);
  tisheader.pixelsize = 64;
  //palette + tiledata
  TisToMos(1,tisheader.numtiles);
}

int Cmos::TisToMos(DWORD x, DWORD y)
{
  if(x*y>tisheader.numtiles) return -1;
  //mos compatibility
  mosheader.wColumn=(WORD) x;
  mosheader.wRow=(WORD) y;
  mosheader.wWidth=(WORD) (tisheader.pixelsize*x);
  mosheader.wHeight=(WORD) (tisheader.pixelsize*y);
  mosheader.dwBlockSize=tisheader.pixelsize;
  mosheader.dwPaletteOffset=sizeof(mosheader);
  return 0;
}

int Cmos::Allocate(DWORD x, DWORD y)
{
  DWORD i;
  int nCol,nRow;
  int nWidth,nHeight;
  int ret;

  tisheader.pixelsize=64;
  mosheader.dwPaletteOffset=sizeof(mosheader);
  mosheader.dwBlockSize=tisheader.pixelsize;
  mosheader.wWidth=(unsigned short) x;
  mosheader.wHeight=(unsigned short) y;
  m_nResX=mosheader.wWidth%mosheader.dwBlockSize;
  m_nResY=mosheader.wHeight%mosheader.dwBlockSize;
  mosheader.wColumn=(unsigned short) ((x+tisheader.pixelsize-1)/tisheader.pixelsize);
  mosheader.wRow=(unsigned short) ((y+tisheader.pixelsize-1)/tisheader.pixelsize);
  tisheader.numtiles=mosheader.wColumn*mosheader.wRow;
  m_nQualityLoss=0;
  //creating the frames
  if(!CreateFrames())
  {
    return -3;
  }

  for(i=0;i<tisheader.numtiles;i++)
  {
    nWidth=nHeight=tisheader.pixelsize;
    nRow=i/mosheader.wColumn;
    nCol=i%mosheader.wColumn;
    if((nCol==mosheader.wColumn-1) && m_nResX) nWidth=m_nResX;
    if((nRow==mosheader.wRow-1) && m_nResY) nHeight=m_nResY;
    INF_MOS_FRAMEDATA *p=GetFrameData(i);
    ret=p->TakeMosData(0,nWidth,nHeight,false);
    if(ret) return ret;
  }
  return 0;
}

int Cmos::ReadTisFromFile(int fhandle, loc_entry *fl, bool header, bool preview)
{
  int esize;
  DWORD i;
  int ret;

  ret = 0;
  new_mos();
  if(header)
  {
    read(fhandle,&tisheader,sizeof(tis_header) );
    mosheader.wRow=(WORD) tisheader.numtiles;
    mosheader.wColumn=1;
    mosheader.wHeight=(WORD) (tisheader.pixelsize*tisheader.numtiles);
    mosheader.wWidth=(WORD) tisheader.pixelsize;
    mosheader.dwBlockSize=tisheader.pixelsize;
  }
  else
  {
     SetTisParameters(fl, filelength(fhandle));
  }
  if (preview) return 0;

  if(!CreateFrames())
  {
    return -3;
  }

  m_pclrDIBits=new COLORREF[tisheader.pixelsize*tisheader.pixelsize];
  if(!m_pclrDIBits) return -3;

  //version 2 tileset
  if ((tisheader.tilelength==sizeof(pvr_entry)) )
  {
    CPtrList pvrs;
    POSITION pos;

    m_pvr = true;
    ((CChitemDlg *) AfxGetMainWnd())->start_progress(tisheader.numtiles,"Decompressing tileset");
    for(i=0;i<tisheader.numtiles;i++)
    {
      pvr_entry entry;
      CString pvrname;

      ((CChitemDlg *) AfxGetMainWnd())->set_progress(i);
      if (read(fhandle, &entry, sizeof(pvr_entry))!= sizeof(pvr_entry))
      {
        ret=-2;
        break;
      }
      CString tmpstr = fl->resref;
      int x = tmpstr.Find(".");
      if (x>0) tmpstr = tmpstr.Left(x);
      if (entry.index==0xffffffff)
      {
        //no need of transparency
        memset(m_pclrDIBits,0, tisheader.pixelsize*tisheader.pixelsize*sizeof(COLORREF) );
      }
      else
      {
        Cpvr *pvr = NULL;
        
        pvrname.Format("%c%-.5s%02d",tmpstr[0], tmpstr.Mid(2), entry.index);
        pos = pvrs.GetHeadPosition();
        while(pos)
        {
          pvr = (Cpvr *) pvrs.GetNext(pos);
          CString name = pvr->m_name;
          name.Replace(".pvrz","");
          if (name==pvrname)
          {
            break;
          }
          pvr = NULL;
        }
        if (!pvr)
        {
          pvr = new Cpvr();
          if (read_pvrz(pvrname, pvr, true))
          {
            delete pvr;
            ret=-4;
            break;
          }
          else
          {
            pvrs.AddHead(pvr);
          }
        }
        pvr->BuildTile(entry.x, entry.y, m_pclrDIBits);
      }
      esize=tisheader.pixelsize*tisheader.pixelsize;
      INF_MOS_FRAMEDATA *p=m_pFrameDataPointer[i];
      p->pFrameData=new BYTE[esize];
      if(!p->pFrameData)
      {
        ret = -3; //out of memory
        break;
      }
      p->nFrameSize=esize;
      p->nHeight=p->nWidth=tisheader.pixelsize;      
      p->TakeWholeBmpData(m_pclrDIBits);
    }
    ((CChitemDlg *) AfxGetMainWnd())->end_progress();

    pos = pvrs.GetHeadPosition();
    while(pos)
    {
      delete pvrs.GetNext(pos);
    }
    return ret;    
  }

  m_pvr = false;
  for(i=0;i<tisheader.numtiles;i++)
  {
    INF_MOS_FRAMEDATA *p=m_pFrameDataPointer[i];
    if(read(fhandle, p->Palette,sizeof(palettetype) )!=sizeof(palettetype) )
    {
      return -2;
    }
    esize=tisheader.pixelsize*tisheader.pixelsize;
    p->pFrameData=new BYTE[esize];
    if(!p->pFrameData) return -3; //out of memory
    p->nFrameSize=esize;
    p->nHeight=p->nWidth=tisheader.pixelsize;
    if(read(fhandle, p->pFrameData, esize)!=esize)
    {
      return -2;
    }
  }
  return 0;
}

int Cmos::ReadMosFromFile(int fhandle, int ml)
{
  BYTE *pCData;
  DWORD dwSize;
  int ret;
  
  if(ml<0) ml=filelength(fhandle);
  if(ml<sizeof(INF_MOS_HEADER) ) return -2; //file is bad
  if(!(editflg&CHECKSIZE) && ml>10000000) return -2;

  if(read(fhandle,&c_header,sizeof(INF_MOSC_HEADER) )!=sizeof(INF_MOSC_HEADER) )
  {
    return -2;
  }
  m_bCompressed = true;
  m_pvr = false;
  //not a compressed header, we read the raw data and compress the bam if needed (on save, if compressed=true)
  if(c_header.chSignature[3]!='C')
  {
    // If this is an uncompressed mos we can just attach the buffer to the
    // mos file object and be done with it.
    m_bCompressed=false;
    m_nDataSize=ml;
    m_pData = new BYTE[m_nDataSize];
    if (!m_pData)
      return -3;
    
    dwSize=m_nDataSize-sizeof(c_header);
    memcpy(m_pData,&c_header, sizeof(c_header) ); //move the pre-read part to the header
    if(read(fhandle,m_pData+sizeof(c_header),dwSize)!=(int) dwSize)
    {
      KillData();
      return -2;
    }
    
    memcpy(&c_header,"MOSCV1  ",8);
    c_header.nExpandSize=m_nDataSize;
    goto endofquest;
  }
  
  ml-=sizeof(INF_MOSC_HEADER);
  pCData = new BYTE[ml];  
  
  if (!pCData)
  {
    return -3;
  }
  if(read(fhandle, pCData, ml) !=ml)
  {
    delete [] pCData;
    return -2;//cannot read compressed data
  }
  
  if(!(editflg&CHECKSIZE) && c_header.nExpandSize>25000000) //don't handle mos larger than 25M
  {
    delete [] pCData;
    return -2; //illegal data
  }
  
  m_nDataSize=c_header.nExpandSize;
 	m_pData = new BYTE[m_nDataSize];
  if (!m_pData)
    return -3;
  
  dwSize=c_header.nExpandSize;
  if (uncompress(m_pData,&c_header.nExpandSize,pCData,ml) != Z_OK)
  {
    KillData();
    delete [] pCData;
    return -2; //uncompress failed
  }
  delete [] pCData;
  
  if(dwSize!=c_header.nExpandSize)
  {
    KillData();
    return -2; //pre-saved uncompressed length isn't the same, why ?
  }
  
endofquest:
  ret=ExplodeMosData(fhandle);
  KillData();
  return ret; 
}

int Cmos::ImplodeMosData()
{
  int nCount;
  int nFrameSize; //total frame data size (must sum up to wheight*wwidth)
  int esize;
  int i, j;
  int nHeight, nWidth;

  if(!m_pOffsets)
  {
    m_pOffsets=new DWORD[tisheader.numtiles];
  }
  if(!m_pOffsets) return -3;  

  //calculating the full size of the uncompressed mos
  m_nDataSize=sizeof(INF_MOS_HEADER);
  m_nDataSize+=mosheader.wRow*mosheader.wColumn*sizeof(palettetype); //adding palettes
  m_nDataSize+=mosheader.wRow*mosheader.wColumn*sizeof(DWORD); //adding frameoffsets
  nCount=0;
  nFrameSize=0;
  nHeight=mosheader.dwBlockSize;
  for(i=0;i<mosheader.wRow;i++)
  {
    if((i==mosheader.wRow-1) && m_nResY) nHeight=m_nResY;
    nWidth=mosheader.dwBlockSize;
    for(j=0;j<mosheader.wColumn;j++)
    {
      if((j==mosheader.wColumn-1) && m_nResX) nWidth=m_nResX;
      esize=nWidth*nHeight;
      m_pOffsets[nCount]=nFrameSize;
      nFrameSize+=esize;
      nCount++;
    }
  }
  if(nFrameSize!=mosheader.wHeight*mosheader.wWidth) return -2;
  m_nDataSize+=nFrameSize; //adding pixels
  //allocating bytes for m_nDataSize
  m_pData = new BYTE[m_nDataSize];
  if(!m_pData) return -3;
  memcpy(m_pData,&mosheader,sizeof(INF_MOS_HEADER) );
  fullsize=sizeof(INF_MOS_HEADER);
  for(i=0;i<nCount;i++)
  {
    INF_MOS_FRAMEDATA *p=GetFrameData(i);
    memcpy(m_pData+fullsize,p->Palette,sizeof(palettetype) );
    fullsize+=sizeof(palettetype);
  }
  
  esize=nCount*sizeof(DWORD);
  memcpy(m_pData+fullsize,m_pOffsets,esize);
  fullsize+=esize;
  for(i=0;i<nCount;i++)
  {
    INF_MOS_FRAMEDATA *p=GetFrameData(i);
    esize=p->nFrameSize;
    memcpy(m_pData+fullsize,p->pFrameData,esize);
    fullsize+=esize;
  }
  //now nActPos should equal m_nDataSize
  //if not, then our code is wrong
  if(fullsize!=m_nDataSize)
  {
    KillData();
    return -4; 
  }
  c_header.nExpandSize=m_nDataSize;
  return 0;
}

int Cmos::ExplodeMosData(int fhandle)
{
  int nCount;
  int ret, gret;
  int esize;
  DWORD i,j;
  int nHeight, nWidth;
  DWORD nFrameSize;
  bool ietmebug=false;
  
  KillFrameData();
  KillOffsets();
  gret=0;
  if(memcmp(m_pData,"MOS V1  ",8))
  {
    if(memcmp(m_pData,"MOS V2  ",8))
    {
      return -4;
    }
    m_pvr = true;
  }
  memcpy(&mosheader,m_pData,sizeof(INF_MOS_HEADER) ); //get header
  fullsize=sizeof(INF_MOS_HEADER);
  if(mosheader.dwPaletteOffset!=fullsize) return -2;
  if(fullsize>m_nDataSize) return -2;
  if (m_pvr)
  {
    //pvrz style entries
    memcpy(&mosheader2, &mosheader, sizeof(mosheader2));
    mosheader2.dwTextureCount = mosheader.dwBlockSize;
    mosheader.dwBlockSize = 64;
    mosheader.wWidth = (WORD) mosheader2.dwWidth;
    mosheader.wHeight = (WORD) mosheader2.dwHeight;
    mosheader.wColumn = (WORD) ((mosheader.wWidth+mosheader.dwBlockSize-1)/mosheader.dwBlockSize);
    mosheader.wRow = (WORD) ((mosheader.wHeight+mosheader.dwBlockSize-1)/mosheader.dwBlockSize);
    //real size
  }
  else
  {
    if(mosheader.dwBlockSize<8 || mosheader.dwBlockSize>256) return -2;
  }
  if(m_pclrDIBits) delete [] m_pclrDIBits;
  m_DIBsize=mosheader.dwBlockSize*mosheader.dwBlockSize;
  m_pclrDIBits=new COLORREF[m_DIBsize];
  
  if(!m_pclrDIBits)
  {
    m_DIBsize=0;
    return -3;
  }

  m_nResX=mosheader.wWidth%mosheader.dwBlockSize;
  m_nResY=mosheader.wHeight%mosheader.dwBlockSize;
  esize=(mosheader.wWidth+mosheader.dwBlockSize-1)/mosheader.dwBlockSize;

  if(esize!=mosheader.wColumn)
  {
    if(!m_nResX && (esize==mosheader.wColumn-1) )
    {
      //ietme saves an empty column if the width is divideable by blocksize
      //we do this hack to fix the problem on the fly
      ietmebug=true;
    }
    else return -2;
  }
  if((mosheader.wHeight+mosheader.dwBlockSize-1)/mosheader.dwBlockSize!=mosheader.wRow)
  {
    return -2;
  }

  if(ietmebug) tisheader.numtiles=(mosheader.wColumn-1)*mosheader.wRow;
  else tisheader.numtiles=mosheader.wColumn*mosheader.wRow;

  if(!CreateFrames())
  {
    return -3;
  }

  nCount=0;

  if (m_pvr)
  {
    for(i=0;i<mosheader2.dwTextureCount;i++)
    {
      pvr_entry entry;

      read(fhandle, &entry, sizeof(pvr_entry));
    }
    //more difficult than looks
    return gret;
  }

  for(i=0;i<mosheader.wRow;i++)
  {
    for(j=0;j<mosheader.wColumn;j++)
    {
      //skipping palette for the empty frame
      if( (j!=(DWORD)mosheader.wColumn-1) || !ietmebug)
      {
        if(fullsize+sizeof(palettetype)>m_nDataSize) return -2;
        INF_MOS_FRAMEDATA *p=m_pFrameDataPointer[nCount++];
        p->TakePaletteData(m_pData+fullsize);
      }
      fullsize+=sizeof(palettetype);
    }
  }    

  m_pOffsets=new DWORD[tisheader.numtiles];
  if(!m_pOffsets) return -3;  
  nCount=0;
  for(i=0;i<mosheader.wRow;i++)
  {
    for(j=0;j<mosheader.wColumn;j++)
    {
      //skipping palette for the empty frame
      if( (j!=(DWORD) mosheader.wColumn-1) || !ietmebug)
      {
        if(fullsize+sizeof(DWORD)>m_nDataSize) return -2;
        m_pOffsets[nCount++]=* (DWORD *) (m_pData+fullsize);
      }
      fullsize+=sizeof(DWORD);
    }
  }
  
  esize=mosheader.wWidth*mosheader.wHeight;
  if(fullsize+esize>m_nDataSize) return -2;
  nCount=0;
  nFrameSize=0;
  nHeight=mosheader.dwBlockSize;
  for(i=0;i<mosheader.wRow;i++)
  {
    if((i==(DWORD) mosheader.wRow-1) && m_nResY) nHeight=m_nResY;
    nWidth=mosheader.dwBlockSize;
    for(j=0;j<mosheader.wColumn;j++)
    {
      if((j==(DWORD) mosheader.wColumn-1) )
      {
        if(m_nResX || ietmebug) nWidth=m_nResX;
      }
      esize=nWidth*nHeight;
      if(nWidth) //ietmebug
      {
        if(nFrameSize!=m_pOffsets[nCount]) return -2;
        INF_MOS_FRAMEDATA *p=m_pFrameDataPointer[nCount];
        ret=p->TakeMosData(m_pData+fullsize+nFrameSize, nWidth, nHeight,false);
        if(ret)
        {
          new_mos(); //delete the crippled data
          return ret;
        }
        nFrameSize+=esize;
        nCount++;
      }
    }
  }

  if(ietmebug) mosheader.wColumn--;
  if(fullsize+nFrameSize!=m_nDataSize) return -1;
  if(ietmebug)
  {
    return 1; //ietme mos bug successfully fixed
  }
  return gret;
}

void Cmos::DropUnusedPalette()
{
  DWORD nFrameWanted;

  //do the pixel change
  for(nFrameWanted=0;nFrameWanted<tisheader.numtiles;nFrameWanted++)
  {
    INF_MOS_FRAMEDATA *p=GetFrameData(nFrameWanted);
    p->MarkPixels();
    p->OrderPalette();
  }
}

void Cmos::ApplyPaletteRGBTile(int nFrameWanted, int r, int g, int b, int strength)
{
  m_changed=true;
  INF_MOS_FRAMEDATA *p=GetFrameData(nFrameWanted);
  p->MarkPixels();
  p->AlterPalette(r,g,b,strength);
}

void Cmos::ApplyPaletteRGB(int r, int g, int b, int strength)
{
  DWORD nFrameWanted;

  //do the pixel change
  for(nFrameWanted=0;nFrameWanted<tisheader.numtiles;nFrameWanted++)
  {
    ApplyPaletteRGBTile(nFrameWanted, r, g, b, strength);
  }
}

DWORD Cmos::GetColor(DWORD x, DWORD y)
{
  DWORD nFrameWanted;

  if(x>=mosheader.wWidth || y>=mosheader.wHeight) return 0;
  nFrameWanted=(x/mosheader.dwBlockSize)+(y/mosheader.dwBlockSize)*mosheader.wColumn;
  INF_MOS_FRAMEDATA *p=GetFrameData(nFrameWanted);
  if(!p)
  {
    return 0;
  }
  return p->GetColor(x%mosheader.dwBlockSize,y%mosheader.dwBlockSize);
}

int Cmos::FlipTile(DWORD nFrameWanted)
{
  INF_MOS_FRAMEDATA *p=GetFrameData(nFrameWanted);
  if(!p)
  {
    return -1;
  }
  p->FlipTile();
  return 0;
}

//this doesn't adjust alternate tiles bigger than the removed one
int Cmos::RemoveTile(DWORD nFrameWanted)
{  
  POSITION pos=m_pFrameData.FindIndex(nFrameWanted);
  if (!pos) return -1;
  INF_MOS_FRAMEDATA *p=m_pFrameData.GetAt(pos);
  delete p;
  m_pFrameData.RemoveAt(pos);
  if(m_pFrameDataPointer)
  {
    free(m_pFrameDataPointer);
  }
  tisheader.numtiles--;
  m_pFrameDataPointer=(INF_MOS_FRAMEDATA **) calloc(tisheader.numtiles, sizeof(INF_MOS_FRAMEDATA *));
  m_changed=true;
  return 0;
}

//this function replaces an existing tile with new palette/bitmap data
//the tile comes from another tileset
int Cmos::SetFrameData(DWORD nFrameWanted, INF_MOS_FRAMEDATA *oldframe)
{
  if(oldframe==NULL) return -1;
  INF_MOS_FRAMEDATA *p=GetFrameData(nFrameWanted);
  if(!p)
  {
    return -1;
  }
  p->TakePaletteData((LPBYTE) oldframe->Palette);
  p->TakeMosData(oldframe->pFrameData,oldframe->nWidth,oldframe->nHeight,false);
  m_changed=true;
  return 0;
}

int Cmos::AddTileCopy(DWORD nFrameWanted, unsigned char *optionalpixels, int flags)
{
  int ret;

  if(tisheader.numtiles>=32767) return -1;
  if(nFrameWanted>=tisheader.numtiles) return -1;
  ret=tisheader.numtiles;
  INF_MOS_FRAMEDATA *p=new INF_MOS_FRAMEDATA;
  if(!p) return -3;
  m_pFrameData.AddTail(p);
  INF_MOS_FRAMEDATA *o=GetFrameData(nFrameWanted);
  p->TakePaletteData((unsigned char *) o->Palette);
  if(optionalpixels || (flags&2) )
  {
    p->TakeMosData(optionalpixels, o->nWidth,o->nHeight, flags&1);
  }
  else
  {
    p->TakeMosData(o->pFrameData, o->nWidth,o->nHeight,false);
  }
  //create transparent green color on 0. palette entry
  if(flags&2)
  {
    p->Palette[255]=p->Palette[0];
    p->Palette[0]=TRANSPARENT_GREEN;
  }
  else p->AllocateTransparency();
  tisheader.numtiles++;
  if(m_pFrameDataPointer)
  {
    free(m_pFrameDataPointer);
  }
  m_pFrameDataPointer=(INF_MOS_FRAMEDATA **) calloc(tisheader.numtiles, sizeof(INF_MOS_FRAMEDATA *));
  if(m_pFrameDataPointer)
  {
    //setting already known pointers
    m_pFrameDataPointer[nFrameWanted]=o;
    m_pFrameDataPointer[ret]=p;
  }
  else
  {
    return -3;
  }
  m_changed=true;
  return ret;
}

int Cmos::SaturateTransparency(DWORD nFrameWanted, bool createcopy, bool deepen)
{
  int ret;
  unsigned char *backup;

  if(nFrameWanted>=tisheader.numtiles) return -1;
  INF_MOS_FRAMEDATA *p=GetFrameData(nFrameWanted);
  if(createcopy)
  {
    backup = new unsigned char[p->nFrameSize];
    if(!backup)
    {
      return -3;
    }
    memcpy(backup, p->pFrameData,p->nFrameSize);
  }
  else backup=NULL;
  ret=p->SaturateTransparency();
  if(ret>0)
  {
    ret=AddTileCopy(nFrameWanted, backup, deepen);
  }
  else ret=-1;
  if(backup) delete [] backup;
  return ret;
}
