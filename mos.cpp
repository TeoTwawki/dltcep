// mos.cpp: implementation of the Cmos class.
// portions came from the SK's bam handler
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "chitem.h"
#include "chitemDlg.h"
#include "mos.h"
#include "zlib.h"
#include "oct_quan.h"

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
    asum+=tmp*tmp;
    tmp=*(((BYTE *) b)+i);
    bsum+=tmp*tmp;
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
  int nPixelCount = 0;
  
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

bool INF_MOS_FRAMEDATA::ExpandMosBits(COLORREF clrReplace, COLORREF clrTransparent, COLORREF *pDIBits, HBITMAP &hBitmap)
{
	int nPixelCount;

  for(nPixelCount=0;nPixelCount<nFrameSize;nPixelCount++)
	{
    // If it is not compressed, still need to catch the transparent pixels and
    // fill with the transparent color.
    if(memcmp(Palette+pFrameData[nPixelCount],&clrReplace, 3) )
    {
      memcpy(pDIBits+nPixelCount,Palette+pFrameData[nPixelCount],sizeof(COLORREF));
    }
    else
    {
      memcpy(pDIBits+nPixelCount,&clrTransparent,sizeof(COLORREF));
    }
	} 
	return MakeBitmapExternal(pDIBits,nWidth,nHeight,hBitmap);
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
      pDIBits[nPixelCount+nOffset]=clrTransparent;
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
      if(pixel==0)
      {
        nop();
      }
      pFrameData[y*nWidth+x]=pixel; //second attempt to remove transparency
      ret=1;
    }
  }
  return ret;
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
  m_pFrameData=NULL;
  m_pOffsets=NULL;
  m_pclrDIBits=NULL;
  m_DIBsize=0;
  m_pData=NULL;
  m_overlay=0;
  m_drawclosed=0;
  m_tileheaders=NULL;
  m_tileindices=NULL;
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
  m_overlay=0;
  m_drawclosed=0;
  m_tileheaders=NULL;
  m_tileindices=NULL;
  m_name.Empty();
}

int Cmos::RetrieveTisFrameCount(int fhandle, int ml)
{
  if(ml<0)
  {
    ml=filelength(fhandle);
    SetTisParameters(ml-24);
    return tisheader.numtiles;
  }
  read(fhandle,&tisheader,sizeof(tis_header) );
  return tisheader.numtiles;
}

int Cmos::GetFrameCount()
{
  return tisheader.numtiles;
}

void Cmos::SetOverlay(int overlay, wed_tilemap *tileheaders, short *tileindices)
{
  m_overlay=overlay;
  m_tileheaders=tileheaders;
  m_tileindices=tileindices;
}

int Cmos::ResolveFrameNumber(int framenumber)
{
  int idx;

  if(!m_tileheaders) return framenumber;
  if(m_overlay)
  {
    if(m_tileheaders[framenumber].flags&(1<<m_overlay))
    {
//      if(!bg1_compatible_area())
      {
        idx=m_tileheaders[framenumber].alternate;
        return idx;
      }
    }
  }
  if(m_drawclosed)
  {
    idx=m_tileheaders[framenumber].alternate;
    if(idx!=-1) return idx;
  }
  idx=m_tileheaders[framenumber].firsttileprimary;
  if(m_tileindices) return m_tileindices[idx];  
  return idx;
}

CPoint Cmos::GetFrameSize(DWORD nFrameWanted)
{
	if (tisheader.numtiles <= nFrameWanted )  //not enough frames
		return (DWORD) -1;
  return CPoint(m_pFrameData[nFrameWanted].nWidth,m_pFrameData[nFrameWanted].nHeight);
}
COLORREF Cmos::GetPixelData(int x, int y)
{
  int nFrameWanted;
  COLORREF *Palette;

  nFrameWanted=ResolveFrameNumber(y/tisheader.pixelsize*mosheader.wColumn+x/tisheader.pixelsize);
  Palette=GetFramePalette(nFrameWanted);
  return Palette[m_pFrameData[nFrameWanted].pFrameData[y%tisheader.pixelsize*m_pFrameData[nFrameWanted].nWidth+x%tisheader.pixelsize]];
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

  for(y=0;y<m_pFrameData[nFrameWanted].nHeight;y++)
  { 
    for(x=0;x<m_pFrameData[nFrameWanted].nWidth;x++)
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
  return m_pFrameData[nFrameWanted].TakeWholeBmpData(prFrameBuffer);
}

unsigned long *Cmos::GetFramePalette(DWORD nFrameWanted)
{
  if(!m_pFrameData) return NULL;

	if (tisheader.numtiles <= nFrameWanted )  //not enough frames
		return NULL;
  return m_pFrameData[nFrameWanted].Palette;
}

bool Cmos::MakeBitmap(COLORREF clrTrans, Cmos &host, int nMode, int nXpos, int nYpos)
{
  int nColumn, nRow, nBase, nOffset, nLimit;
  DWORD nFrameWanted;

  if(!m_pFrameData) return false;

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
    for(nXpos=0;nXpos<mosheader.wColumn ;nXpos++)
    {
      nFrameWanted=ResolveFrameNumber(nYpos*mosheader.wColumn+nXpos);
      if(nFrameWanted<tisheader.numtiles)
      {
        if (!m_pFrameData[nFrameWanted].ExpandMosBitsWhole(TRANSPARENT_GREEN,clrTrans,host.GetDIB(),nOffset, nColumn))
        {
          return false;
        }
      }      
      nOffset+=mosheader.dwBlockSize;
    }
  }
  return true;
}

bool Cmos::MakeBitmap(DWORD nFrameWanted, COLORREF clrTrans, HBITMAP &hBitmap)
{
  if(hBitmap)
  {
    DeleteObject(hBitmap);
    hBitmap=0;
  }
  if(!m_pFrameData) return false;

	if (tisheader.numtiles <= nFrameWanted )  //not enough frames
		return false;

	if (!m_pFrameData[nFrameWanted].ExpandMosBits(TRANSPARENT_GREEN,clrTrans,m_pclrDIBits, hBitmap))
		return false;

	return true;
}

bool Cmos::MakeBitmapWhole(COLORREF clrTrans, HBITMAP &hBitmap, int clipx, int clipy, int maxclipx, int maxclipy)
{
  int nXpos,nYpos;
  int nOffset;
  DWORD nFrameWanted;
  bool ret;
  int pixelwidth, pixelheight;

  if(hBitmap)
  {
    DeleteObject(hBitmap);
    hBitmap=0;
  }

  if(!m_pFrameData) return false;

  if(!maxclipx || maxclipx*mosheader.dwBlockSize>(DWORD) mosheader.wWidth+63)
  {
    maxclipx=(mosheader.wWidth+63)/mosheader.dwBlockSize;
  }
  if(!maxclipy || maxclipy*mosheader.dwBlockSize>(DWORD) mosheader.wHeight+63)
  {
    maxclipy=(mosheader.wHeight+63)/mosheader.dwBlockSize;
  }
  pixelwidth=(maxclipx-clipx)*mosheader.dwBlockSize;
  pixelheight=(maxclipy-clipy)*mosheader.dwBlockSize;
  if(pixelwidth<0) pixelwidth=0;
  if(pixelheight<0) pixelheight=0;

  nXpos=pixelwidth*pixelheight;
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
    nOffset=mosheader.dwBlockSize*pixelwidth*nYpos;
    for(nXpos=0;(nXpos+clipx<maxclipx) /*&& (nXpos<VIEW_MAXEXTENT)*/;nXpos++)
    {
      nFrameWanted=ResolveFrameNumber((nYpos+clipy)*mosheader.wColumn+nXpos+clipx);
      if(nFrameWanted<tisheader.numtiles)
      {
        if (!m_pFrameData[nFrameWanted].ExpandMosBitsWhole(TRANSPARENT_GREEN,clrTrans,m_pclrDIBits,nOffset, pixelwidth))
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

  ret=MakeBitmapExternal(m_pclrDIBits,pixelwidth,pixelheight,hBitmap);

  return ret;
}

int Cmos::MakeBitmapInternal(HBITMAP &hBitmap)
{
  if((int) (mosheader.wColumn*mosheader.dwBlockSize*
     mosheader.wRow*mosheader.dwBlockSize)!=m_DIBsize)
  {
    return false;
  }

  if(hBitmap)
  {
    DeleteObject(hBitmap);
    hBitmap=0;
  }

  return MakeBitmapExternal(m_pclrDIBits,mosheader.wColumn*mosheader.dwBlockSize,
    mosheader.wRow*mosheader.dwBlockSize,hBitmap);
}

int Cmos::MakeTransparent(DWORD nFrameWanted, DWORD redgreenblue, int limit)
{
  int i;
  int nHits;

  if(tisheader.numtiles <= nFrameWanted ) return -1;
  nHits=0;

  // don't force transparent index to be 0
  for(i=0;i<256;i++)
  {
    if(ChiSquare((BYTE *) &redgreenblue,(BYTE *) (m_pFrameData[nFrameWanted].Palette+i))<=limit )
    {
      m_pFrameData[nFrameWanted].Palette[i]=TRANSPARENT_GREEN;
      nHits++;
    }
  }  
  return nHits;
}

int Cmos::WriteTisToFile(int fhandle, int clipx, int clipy, int maxclipx, int maxclipy)
{
  int esize;
  DWORD i;
  DWORD x,y;
  BYTE *zerobuffer;
  int ret;
  DWORD tmpsave;
  DWORD nFrameWanted;

  esize=tisheader.pixelsize*tisheader.pixelsize;
  zerobuffer=new BYTE[esize];
  if(!zerobuffer) return -3;
  memset(zerobuffer,0,esize);
  ret=0;
  //this hack makes sure the header contains as many tiles as we wanted
  memcpy(&tisheader.filetype,"TIS V1  ",8);
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
  //end of hack
  //this hack makes sure we write out the tiles we wanted
  //maximum contains the last tile
  for(i=0;i<tisheader.numtiles;i++)
  {
    if(maxclipx!=-1)
    {
      y=i/(maxclipx-clipx+1);
      x=i%(maxclipx-clipx+1);
      nFrameWanted=ResolveFrameNumber( (y+clipy)*mosheader.wColumn+x+clipx);
    }
    else nFrameWanted=i;
    esize=sizeof(palettetype); //sizeof palettetype
    if(write(fhandle,m_pFrameData[nFrameWanted].Palette,esize )!=esize)
    {
      ret=-2;
      goto endofquest;
    }
    esize=tisheader.pixelsize*tisheader.pixelsize;
    if(esize!=m_pFrameData[nFrameWanted].nFrameSize)
    {
      //outputting incomplete tiles
      ret=1;
      for(y=0;y<m_pFrameData[nFrameWanted].nHeight;y++)
      {
        esize-=write(fhandle,m_pFrameData[nFrameWanted].pFrameData+y*m_pFrameData[nFrameWanted].nWidth,m_pFrameData[nFrameWanted].nWidth);
        esize-=write(fhandle,zerobuffer,tisheader.pixelsize-m_pFrameData[nFrameWanted].nWidth);
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
      if(write(fhandle,m_pFrameData[nFrameWanted].pFrameData,esize)!=esize)
      {
        ret=-2;
        goto endofquest;
      }
    }
  }
endofquest:
  tisheader.numtiles=tmpsave;
  delete [] zerobuffer;
  return ret;
}

int Cmos::GetImageWidth(int clipx, int maxclipx)
{
  if(!m_pFrameData) return -1;
  if(!maxclipx && !clipx)
  {
    return mosheader.wWidth;
  }
  return (maxclipx-clipx)*mosheader.dwBlockSize+m_pFrameData[maxclipx].nWidth;
}

int Cmos::GetImageHeight(int clipy, int maxclipy)
{
  if(!m_pFrameData) return -1;
  if(!maxclipy && !clipy)
  {    
    return mosheader.wHeight;    
  }
  return (maxclipy-clipy)*mosheader.dwBlockSize+m_pFrameData[mosheader.wColumn*maxclipy].nHeight;
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
    cols=m_pFrameData[nFrameWanted].nWidth;
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
    m_nQualityLoss+=m_pFrameData[nFrameWanted].TakeWholeBmpData(prFrameBuffer);
  }
  m_nQualityLoss=m_nQualityLoss/nFrameWanted;
endofquest:
  if(prFrameBuffer) delete [] prFrameBuffer;
  if(pRawData) delete [] pRawData;
  ((CChitemDlg *) AfxGetMainWnd())->end_progress();
  return ret;
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
  SetTisParameters(0);
  cols=(sHeader.width+tisheader.pixelsize-1)/tisheader.pixelsize;
  rows=(sHeader.height+tisheader.pixelsize-1)/tisheader.pixelsize;
  SetTisParameters(rows*cols*(tisheader.pixelsize*tisheader.pixelsize+sizeof(palettetype)));
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

  m_pFrameData=new INF_MOS_FRAMEDATA[tisheader.numtiles];
  if(!m_pFrameData)
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
    //if it has a palette then we take it now
    if(sHeader.bits==8) m_pFrameData[y].TakePaletteData((LPBYTE) &sPalette);
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
    ret=m_pFrameData[y].TakeMosData(0,cols,rows, false); //setting the size of the frame now
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
        nSourceOff+=m_pFrameData[nFrameWanted++].TakeBmpData(pcBuffer, nFrameRow, nSourceOff);
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
      nSourceOff=m_pFrameData[nFrameWanted].nWidth*(y%mosheader.dwBlockSize);
      nFullLine+=m_pFrameData[nFrameWanted++].ExpandMosLine(pcBuffer+nFullLine,nSourceOff);
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
  return 0;
}

void Cmos::SetTisParameters(int size)
{    
  tisheader.offset=sizeof(tisheader);
  tisheader.pixelsize=64;
  //palette + tiledata
  tisheader.tilelength=sizeof(palettetype)+tisheader.pixelsize*tisheader.pixelsize;
  tisheader.numtiles=size/tisheader.tilelength;
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
  m_pFrameData = new INF_MOS_FRAMEDATA[tisheader.numtiles];
  if(!m_pFrameData)
  {
    tisheader.numtiles=0;
    return -3;
  }
  for(i=0;i<tisheader.numtiles;i++)
  {
    nWidth=nHeight=tisheader.pixelsize;
    nRow=i/mosheader.wColumn;
    nCol=i%mosheader.wColumn;
    if((nCol==mosheader.wColumn-1) && m_nResX) nWidth=m_nResX;
    if((nRow==mosheader.wRow-1) && m_nResY) nHeight=m_nResY;
    ret=m_pFrameData[i].TakeMosData(0,nWidth,nHeight,false);
    if(ret) return ret;
  }
  return 0;
}

int Cmos::ReadTisFromFile(int fhandle, int ml, int header)
{
  int esize;
  DWORD i;

  new_mos();
  if(ml<0) ml=filelength(fhandle);
  if(header)
  {
    read(fhandle,&tisheader,sizeof(tis_header) );
    ml-=sizeof(tis_header);
    mosheader.wRow=(WORD) tisheader.numtiles;
    mosheader.wColumn=1;
    mosheader.wHeight=(WORD) (tisheader.pixelsize*tisheader.numtiles);
    mosheader.wWidth=(WORD) tisheader.pixelsize;
  }
  else SetTisParameters(ml);
  m_pclrDIBits=new COLORREF[tisheader.pixelsize*tisheader.pixelsize];
  if(!m_pclrDIBits) return -3;

  m_pFrameData=new INF_MOS_FRAMEDATA[tisheader.numtiles];
  if(!m_pFrameData) return -3;

  for(i=0;i<tisheader.numtiles;i++)
  {
    if(read(fhandle, m_pFrameData[i].Palette,sizeof(palettetype) )!=sizeof(palettetype) )
    {
      return -2;
    }
    esize=tisheader.pixelsize*tisheader.pixelsize;
    m_pFrameData[i].pFrameData=new BYTE[esize];
    if(!m_pFrameData[i].pFrameData) return -3; //out of memory
    m_pFrameData[i].nFrameSize=esize;
    m_pFrameData[i].nHeight=m_pFrameData[i].nWidth=tisheader.pixelsize;
    if(read(fhandle, m_pFrameData[i].pFrameData, esize)!=esize)
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
  if((ml<sizeof(INF_MOS_HEADER)) || (ml>10000000) ) return -2; //file is bad
  if(read(fhandle,&c_header,sizeof(INF_MOSC_HEADER) )!=sizeof(INF_MOSC_HEADER) )
  {
    return -2;
  }
  m_bCompressed=true;
  //not a compressed header, we read the raw data and compress the bam if needed (on save, if compressed=true)
  if(c_header.chSignature[3]!='C')
  {
    // If this is an uncompressed bam we can just attach the buffer to the
    // bam file object and be done with it.
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
  
  if(c_header.nExpandSize>10000000) //don't handle bams larger than 10M
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
  ret=ExplodeMosData();
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
    memcpy(m_pData+fullsize,&m_pFrameData[i].Palette,sizeof(palettetype) );
    fullsize+=sizeof(palettetype);
  }
  
  esize=nCount*sizeof(DWORD);
  memcpy(m_pData+fullsize,m_pOffsets,esize);
  fullsize+=esize;
  for(i=0;i<nCount;i++)
  {
    esize=m_pFrameData[i].nFrameSize;
    memcpy(m_pData+fullsize,m_pFrameData[i].pFrameData,esize);
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

int Cmos::ExplodeMosData()
{
  int nCount;
  int ret, gret;
  int esize;
  int i,j;
  int nHeight, nWidth;
  DWORD nFrameSize;
  bool ietmebug=false;
  
  KillFrameData();
  KillOffsets();
  gret=0;
  if(memcmp(m_pData,"MOS V1  ",8))
  {
    return -4;
  }
  memcpy(&mosheader,m_pData,sizeof(INF_MOS_HEADER) ); //get header
  fullsize=sizeof(INF_MOS_HEADER);
  if(mosheader.dwPaletteOffset!=fullsize) return -2;
  if(fullsize>m_nDataSize) return -2;
  if(mosheader.dwBlockSize<8 || mosheader.dwBlockSize>256) return -2;
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
  m_pFrameData=new INF_MOS_FRAMEDATA[tisheader.numtiles];
  if(!m_pFrameData) return -3;
  nCount=0;
  for(i=0;i<mosheader.wRow;i++)
  {
    for(j=0;j<mosheader.wColumn;j++)
    {
      //skipping palette for the empty frame
      if( (j!=mosheader.wColumn-1) || !ietmebug)
      {
        if(fullsize+sizeof(palettetype)>m_nDataSize) return -2;
        m_pFrameData[nCount++].TakePaletteData(m_pData+fullsize);
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
      if( (j!=mosheader.wColumn-1) || !ietmebug)
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
    if((i==mosheader.wRow-1) && m_nResY) nHeight=m_nResY;
    nWidth=mosheader.dwBlockSize;
    for(j=0;j<mosheader.wColumn;j++)
    {
      if((j==mosheader.wColumn-1) )
      {
        if(m_nResX || ietmebug) nWidth=m_nResX;
      }
      esize=nWidth*nHeight;
      if(nWidth) //ietmebug
      {
        if(nFrameSize!=m_pOffsets[nCount]) return -2;
        ret=m_pFrameData[nCount].TakeMosData(m_pData+fullsize+nFrameSize, nWidth, nHeight,false);
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
  DWORD nFrame;

  //do the pixel change
  for(nFrame=0;nFrame<tisheader.numtiles;nFrame++)
  {
    m_pFrameData[nFrame].MarkPixels();
    m_pFrameData[nFrame].OrderPalette();
  }
}

DWORD Cmos::GetColor(DWORD x, DWORD y)
{
  DWORD nFrameWanted;

  if(x>=mosheader.wWidth || y>=mosheader.wHeight) return 0;
  nFrameWanted=(x/mosheader.dwBlockSize)+(y/mosheader.dwBlockSize)*mosheader.wColumn;
  return m_pFrameData[nFrameWanted].GetColor(x%mosheader.dwBlockSize,y%mosheader.dwBlockSize);
}

int Cmos::AddTileCopy(DWORD original, unsigned char *optionalpixels, bool deepen)
{
  INF_MOS_FRAMEDATA *newframedata;
  DWORD i;
  int ret;

  if(tisheader.numtiles>=32767) return -1;
  if(original>=tisheader.numtiles) return -1;
  ret=tisheader.numtiles;
  newframedata=new INF_MOS_FRAMEDATA[tisheader.numtiles+1];
  if(!newframedata)
  {
    return -3;
  }
  memcpy(newframedata,m_pFrameData,tisheader.numtiles*sizeof(INF_MOS_FRAMEDATA));
  newframedata[tisheader.numtiles].TakePaletteData((unsigned char *) newframedata[original].Palette);
  if(optionalpixels)
  {
    newframedata[tisheader.numtiles].TakeMosData(optionalpixels,
      newframedata[original].nWidth,newframedata[original].nHeight, deepen);
  }
  else
  {
    newframedata[tisheader.numtiles].TakeMosData(newframedata[original].pFrameData,
      newframedata[original].nWidth,newframedata[original].nHeight,false);
  }
  //detach pixel pointers so they don't get freed!
  for(i=0;i<tisheader.numtiles;i++)
  {
    m_pFrameData[i].pFrameData=NULL;
  }
  tisheader.numtiles++;
  delete [] m_pFrameData;
  m_pFrameData=newframedata;  
  m_changed=true;
  return ret;
}

int Cmos::SaturateTransparency(DWORD tile, bool createcopy, bool deepen)
{
  int ret;
  unsigned char *backup;

  if(tile>=tisheader.numtiles) return -1;
  if(createcopy)
  {
    backup = new unsigned char[m_pFrameData[tile].nFrameSize];
    if(!backup) return -3;
    memcpy(backup, m_pFrameData[tile].pFrameData,m_pFrameData[tile].nFrameSize);
  }
  else backup=NULL;
  ret=m_pFrameData[tile].SaturateTransparency();
  if(ret>0)
  {
    m_changed=true;
    ret=AddTileCopy(tile, backup, deepen);
  }
  else ret=-1;
  if(backup) delete [] backup;
  return ret;
}
