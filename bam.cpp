// bam.cpp: implementation of the Cbam class.
// code derived from ShadowKeeper (c) 2000 Aaron O'Neil
//////////////////////////////////////////////////////////////////////
/************************************************************************************
	Copyright (c) 2000 Aaron O'Neil
	All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions
	are met:

		1) Redistributions of source code must retain the above copyright notice, 
				this list of conditions and the following disclaimer.
		2) Redistributions in binary form must reproduce the above copyright notice, 
				this list of conditions and the following disclaimer in the documentation
				and/or other materials provided with the distribution.
		3) Redistributions in binary form must reproduce the above copyright notice on
				program startup. Additional credits for program modification are acceptable
				but original copyright and credits must be visible at startup.
		4) You may charge a reasonable copying fee for any distribution of Shadow Keeper. 
				You may charge any fee you choose for support of Shadow Keeper. You may not 
				charge a fee for Shadow Keeper itself.

  THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
	IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
	OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
	IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
	INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
	NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
	THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
	THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

**************************************************************************************/

#include "stdafx.h"
#include "chitem.h"
#include "bam.h"
#include "zlib.h"
#include "oct_quan.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// INF_BAM_FRAMEDATA helper class
//////////////////////////////////////////////////////////////////////

void INF_BAM_FRAMEDATA::FlipFrame(int nWidth, int nHeight)
{
  unsigned char *poi;
  unsigned char tmp;
  int i;

  poi=pFrameData;
  while(nHeight--)
  {
    for(i=0;i<nWidth/2;i++)
    {
      tmp=poi[i];
      poi[i]=poi[nWidth-i-1];
      poi[nWidth-i-1]=tmp;
    }
    poi+=nWidth;
  }
}

void INF_BAM_FRAMEDATA::ReorderPixels(COLORREF *palette, BYTE chTransparentIndex,
                                      BYTE chNewTransparentIndex, bool bIsCompressed)
{
  int idx, color;
  unsigned int i;

  for(i=0;i<nFrameSize;i++)
  {
    color=pFrameData[i];
    if(color==chTransparentIndex)
    {
      pFrameData[i]=(BYTE) chNewTransparentIndex;
      if(bIsCompressed) i++;
    }
    else
    {
      for(idx=0;idx<256;idx++)
      {
        if(*((BYTE *) (palette+idx)+3)==color )
        {
          if(idx==chNewTransparentIndex)
          {
            idx=chTransparentIndex;
          }
          pFrameData[i]=(BYTE) idx;
          break;
        }
      }
    }
  }
}

void INF_BAM_FRAMEDATA::MarkPixels(COLORREF *palette, BYTE chTransparentIndex, bool bIsCompressed)
{
  int color;
  unsigned int i;

  for(i=0;i<nFrameSize;i++)
  {
    color=pFrameData[i];
    if(color==chTransparentIndex)
    {
      if(bIsCompressed) i++;
    }
    else
    {
      *((BYTE *) (palette+color)+3)=0;
    }
  }
}

int INF_BAM_FRAMEDATA::TakePltData(const LPWORD pRawBits, plt_header &sHeader, int row)
{
  unsigned int i;

  if(!row)
  {
    if(pFrameData) delete [] pFrameData;
    nFrameSize=sHeader.dwHeight*sHeader.dwWidth;
    pFrameData=new BYTE[nFrameSize];
    if(!pFrameData) return -3;
  }
  for(i=0;i<sHeader.dwWidth;i++)
  {
    pFrameData[(sHeader.dwHeight-row-1)*sHeader.dwWidth+i]=(BYTE) (pRawBits[i]>>8);
  }
  return 0;
}

int INF_BAM_FRAMEDATA::TakeBmpData(const LPBYTE pRawBits, bmp_header &sHeader, int row)
{
  if(!row)
  {
    if(pFrameData) delete [] pFrameData;
    nFrameSize=sHeader.height*sHeader.width;
    pFrameData=new BYTE[nFrameSize];
    if(!pFrameData) return -3;
  }
  memcpy(pFrameData+(sHeader.height-row-1)*sHeader.width,pRawBits,sHeader.width);
  return 0;
}

int INF_BAM_FRAMEDATA::TakeBmp4Data(const LPBYTE pRawBits, bmp_header &sHeader, int row)
{
  unsigned int i;

  if(!row)
  {
    if(pFrameData) delete [] pFrameData;
    nFrameSize=sHeader.height*sHeader.width;
    pFrameData=new BYTE[nFrameSize];
    if(!pFrameData) return -3;
  }
  for(i=0;i<sHeader.width;i++)
  {
    if(i&1) pFrameData[(sHeader.height-row-1)*sHeader.width+i]=(unsigned char) (pRawBits[i/2]&15);
    else pFrameData[(sHeader.height-row-1)*sHeader.width+i]=(unsigned char) (pRawBits[i/2]>>4);
  }
  return 0;
}

int INF_BAM_FRAMEDATA::TakeBamData(const LPBYTE pRawBits, unsigned int nWidth,
                                   unsigned int nHeight, BYTE chTransparentIndex,
                                   bool bIsCompressed, unsigned int nMaxLength)
{
	unsigned int nNumPixels = nWidth*nHeight;
	unsigned int nPixelCount = 0;

  if(pFrameData) delete [] pFrameData;  //deletes the old data
  
  if(!bIsCompressed)
  {
    pFrameData=new BYTE[nNumPixels];
    if(!pFrameData) return -3;             //memory problem
    nFrameSize=nNumPixels;
    memcpy(pFrameData, pRawBits, nNumPixels);
    return 0;
  }

  //fast check for valid RLE compressed data, so we don't take crap
  nFrameSize=0;
	while(nPixelCount < nNumPixels)
	{
    if(nFrameSize>=nMaxLength) break; //corrupted file
		if ( pRawBits[nFrameSize++] == chTransparentIndex)
		{
			nPixelCount += pRawBits[nFrameSize++]+1;
		}
		else
		{
			nPixelCount++;
		}
  }
  pFrameData=new BYTE[nFrameSize];
  if(!pFrameData) return -3;             //memory problem
  memcpy(pFrameData, pRawBits, nFrameSize);
  if(nPixelCount!=nNumPixels)
  { //shaking the data in place
    RLEDecompression(chTransparentIndex);
    if(nFrameSize>nWidth*nHeight) nFrameSize=nWidth*nHeight;
    RLECompression(chTransparentIndex);
    return -1; //RLE decompression resulted in different plain data size
  }
  return 0;
}

int INF_BAM_FRAMEDATA::RLEDecompression(BYTE chTransparentIndex)
{
  BYTE *pNewFrameData;
  unsigned int nPixelCount = 0;
  int nTrans = 0;
  unsigned int nNewFrameSize = 0;

  if(!pFrameData) return -1;
	while(nPixelCount < nFrameSize)
	{
		if (pFrameData[nPixelCount++] == chTransparentIndex)
		{
			nNewFrameSize += pFrameData[nPixelCount++]+1;
		}
		else
		{
			nNewFrameSize++;
		}
	} 
  pNewFrameData= new BYTE[nNewFrameSize];
  if(!pNewFrameData) return -3;
  nPixelCount=0;
  nNewFrameSize=0;
	while(nPixelCount < nFrameSize)
	{
		if (pFrameData[nPixelCount] == chTransparentIndex)
		{
      nPixelCount++;
			nTrans = pFrameData[nPixelCount++]+1;
      while(nTrans--)
      {
        pNewFrameData[nNewFrameSize++]=chTransparentIndex;
      }
		}
		else
		{
      pNewFrameData[nNewFrameSize++]=pFrameData[nPixelCount++];
		}
	} 
  
  delete [] pFrameData;
  pFrameData=pNewFrameData;
  nFrameSize=nNewFrameSize;
  return nNewFrameSize;
}

int INF_BAM_FRAMEDATA::RLECompression(BYTE chTransparentIndex)
{
  BYTE *pNewFrameData;
  unsigned int nPixelCount = 0;
  int nTrans = 0;
  unsigned int nNewFrameSize = 0;

  if(!pFrameData) return -1;
  ///////Precalculating RLE frame size
  while(nPixelCount < nFrameSize)
  {
    if(pFrameData[nPixelCount++] == chTransparentIndex)
    {
      if(nTrans)
      {
        nTrans++;
        if(nTrans==256)
        {
          nTrans=1;
          nNewFrameSize+=2;
        }
      }
      else nTrans=1;
    }
    else
    {
      if(nTrans)
      {
        nNewFrameSize+=2;
        nTrans=0;
      }
      nNewFrameSize++;
    }
  }
  if(nTrans) nNewFrameSize+=2;
//  if(nFrameSize<nNewFrameSize) return 1; //RLE compression isn't economical
  ///////Copying the data with RLE compression
  pNewFrameData= new BYTE[nNewFrameSize];
  if(!pNewFrameData) return -3;
  nPixelCount=0;
  nTrans=0;
  nNewFrameSize=0;
  while(nPixelCount < nFrameSize)
  {
    if(pFrameData[nPixelCount] == chTransparentIndex)
    {
      if(nTrans)
      {
        nTrans++;
        if(nTrans==256)
        {
          nTrans=1;
          pNewFrameData[nNewFrameSize++]=chTransparentIndex;
          pNewFrameData[nNewFrameSize++]=254;
        }
      }
      else nTrans=1;
    }
    else
    {
      if(nTrans)
      {
        pNewFrameData[nNewFrameSize++]=chTransparentIndex;
        pNewFrameData[nNewFrameSize++]=(BYTE) (nTrans-1);
        nTrans=0;
      }
      pNewFrameData[nNewFrameSize++]=pFrameData[nPixelCount];
    }
    nPixelCount++;
  }
  if(nTrans)
  {
    pNewFrameData[nNewFrameSize++]=chTransparentIndex;
    pNewFrameData[nNewFrameSize++]=(BYTE) (nTrans-1);
  }
  delete [] pFrameData;
  pFrameData=pNewFrameData;
  nFrameSize=nNewFrameSize;
  return nNewFrameSize;
}

int INF_BAM_FRAMEDATA::ExpandBamBits(BYTE chTransparentIndex, COLORREF clrTransparent,
     COLORREF *pDIBits, bool bIsCompressed, int nColumn, COLORREF *pPal, int nWidth, int nHeight)
{
	int nNumRow, nNumColumn;
	unsigned int nSourceOff = 0;
	int nPixelCount = 0;
  int nReplaced = 0;
	int nCount;
  COLORREF clr;

	// The red and blue values seem to be backwards from the RGB macro. When
	// creating the bitmap it apparently wants it in the reverse order that 
	// the RGB macro makes it.
	COLORREF clrFixed = RGB(GetBValue(clrTransparent),GetGValue(clrTransparent),GetRValue(clrTransparent));
  
  for(nNumRow=0;nNumRow<nHeight;nNumRow++)
  {	
    for(nNumColumn=0;nNumColumn<nWidth;)
    {
      if(nFrameSize<=nSourceOff) break;
      if (bIsCompressed && (pFrameData[nSourceOff] == chTransparentIndex) )
      {
        nSourceOff++;
        nCount = pFrameData[nSourceOff]+1;
        while(nCount)
        {
          if(clrFixed!=0xffffff)
          {
            pDIBits[nPixelCount]=clrFixed;
          }
          nCount--;
          nPixelCount++;
          nNumColumn++;
          if(nNumColumn>=nWidth)
          {
            nNumColumn-=nWidth;
            nNumRow++;
            nPixelCount+=nColumn-nWidth;
            if(nNumRow==nHeight) return nReplaced;
          }
        }
        nSourceOff++;
      }
      else
      {
        // If it is not compressed, still need to catch the transparent pixels and
        // fill with the transparent color.
        if (pFrameData[nSourceOff] == chTransparentIndex)
        {
          if(clrFixed!=0xffffff)
          {
            pDIBits[nPixelCount]=clrFixed;
          }
        }
        else
        {
          clr=pPal[pFrameData[nSourceOff]];
          //this is intentionally not clrFixed, we have 2 different functions here
          switch(clrTransparent)
          {
          case 0xfffffffe:
            if(nNumRow<2 || nNumColumn<2 || nNumRow>=nHeight-2 || nNumColumn>=nWidth-2)
            {
              clr=TRANSPARENT_GREEN;
            }
            pDIBits[nPixelCount]=clr;
            break;
          case 0xffffffff:
            if(ChiSquare((LPBYTE) (pDIBits+nPixelCount),(LPBYTE) &clr)<SMALL_DIFF)
            {
              nReplaced++;
              clr=TRANSPARENT_GREEN;
            }
          default:
            pDIBits[nPixelCount]=clr;
          }
        }
        nSourceOff++;
        nPixelCount++;
        nNumColumn++;    
      }
    }
    nPixelCount+=nColumn-nWidth;
	} 
	return nReplaced;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Cbam::Cbam()
{
  m_pData=NULL;
  m_pFrameData=NULL;
  m_pFrames=NULL;
  m_pCycles=NULL;
  m_pFrameLookup=NULL;
  memset(&m_header,0,sizeof(INF_BAM_HEADER) );
  memset(&c_header,0,sizeof(c_header));
  m_bCompressed=0;
  m_pclrDIBits=NULL;
}

Cbam::~Cbam()
{
  new_bam();
}

void Cbam::new_bam()
{
  KillData();
  KillFrameData();
  KillDIBits();

  memset(&m_header,0,sizeof(INF_BAM_HEADER) );
  memset(&c_header,0,sizeof(INF_BAMC_HEADER) );
  memset(m_palette,0,sizeof(m_palette));
  memcpy(&m_header,"BAM V1  ",8);
  memcpy(&c_header,"BAMCV1  ",8);
  m_palettesize=256*sizeof(COLORREF);
  m_bCompressed=false;
  m_nSaveOrder=0xe1;
}

int Cbam::Reallocate(int x, int y)
{
  DWORD newsize=x*y;
  
  if(m_pclrDIBits)
  {
    if(m_nBitSize!=newsize)
    {
      delete [] m_pclrDIBits;
      m_pclrDIBits=new COLORREF[newsize];
    }
  }
  else m_pclrDIBits=new COLORREF[newsize];
  m_nBitSize=newsize;
  return m_pclrDIBits!=NULL;
}

bool Cbam::CanCompress()
{
  BYTE *m_pCData;
  int ret;
  unsigned long tmpsize;

  if(m_bCompressed) return false;       //already compressed
  if(c_header.nExpandSize<100) return false;  //too small to bother with
  if(ImplodeBamData()) return false; //some problem at implosion
  tmpsize=m_nDataSize/2;
  m_pCData=new BYTE[tmpsize]; //if we can't halve size, don't bother with it
  if(!m_pCData) return false; //memory problem, we can't compress it now
  //dest, destlen, source, sourcelen
  ret=compress(m_pCData,&tmpsize,m_pData, c_header.nExpandSize);
  KillData();
  delete [] m_pCData;
  return ret==Z_OK; //we could compress this uncompressed file
}

int Cbam::WriteBmpToFile(int fhandle, int frame)
{
  LPBYTE poi;
  CPoint fs;
  int RLE;
  int pad;
  unsigned long padding;

  padding=0;
  fs=GetFrameSize(frame);
  RLE=GetFrameRLE(frame);
  SetFrameRLE(frame,0); //uncompressing the image
  CreateBmpHeader(fhandle, fs.x, fs.y, 1);
  if(write(fhandle,m_palette, sizeof(m_palette) )!=sizeof(m_palette) )
  {
    return -2;
  }
  poi=GetFrameData(frame)+fs.y*fs.x;
  pad=4-fs.x&3;
  while(fs.y--)
  {
    poi-=fs.x;
    if(write(fhandle,poi, fs.x)!=fs.x)
    {
      return -2;
    }
    if(pad!=4) write(fhandle,&padding,pad);
  }
  SetFrameRLE(frame,RLE);//compressing the image back
  return 0;
}

int Cbam::WriteBamToFile(int fhandle)
{
  BYTE *pCData;
  int ret;
  unsigned long tmpsize;

  if(ImplodeBamData()) return -4; //internal error
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

//assembles the uncompressed bam data according to the pre-determined
//save order. Recalculates offsets that might have changed
int Cbam::ImplodeBamData()
{
  int nActPos;
  int so;
  int i, j;

  //seeking the framedata offset so we could readjust individual frame
  //offsets, also calculating the full size of the uncompressed bam
  m_nDataSize=sizeof(INF_BAM_HEADER);
  so=m_nSaveOrder;
  for(i=0;i<4;i++)
  {
    switch(so&3)
    {
    case 0:
      m_nDataSize+=m_palettesize;                   //palette
      break;
    case 1:
      m_nDataSize+=m_header.wFrameCount*sizeof(INF_BAM_FRAME);
      m_nDataSize+=m_header.chCycleCount*sizeof(INF_BAM_CYCLE);        //frame/cycle
      break;
    case 2:
      m_nDataSize+=m_nFrameLookupSize*sizeof(short);                   //lookup size
      break;
    case 3:
      //adjusting frames to point to the correct offset in the
      //frame data stream, taking care for the RLE marker
      for(j=0;j<m_nFrames;j++)
      {
        m_pFrames[j].dwFrameDataOffset&=0x80000000;
        m_pFrames[j].dwFrameDataOffset|=m_nDataSize;
        m_nDataSize+=m_pFrameData[j].nFrameSize;
      }
    }
    so>>=2;
  }
  //allocating bytes for m_nDataSize
  m_pData = new BYTE[m_nDataSize];
  if(!m_pData) return -3;
  nActPos=sizeof(m_header);
  so=m_nSaveOrder;
  for(i=0;i<4;i++)
  {
    switch(so&3)
    {
    case 0: //palette
      if(m_palettesize)
      {
        m_header.dwPaletteOffset=nActPos;
        memcpy(m_pData+nActPos,m_palette,m_palettesize );
        nActPos+=m_palettesize;
      }
      else m_header.dwPaletteOffset=0;
      break;
    case 1:
      m_header.dwFrameOffset=nActPos;
      memcpy(m_pData+nActPos, m_pFrames, m_header.wFrameCount*sizeof(INF_BAM_FRAME) );
      nActPos+= m_header.wFrameCount*sizeof(INF_BAM_FRAME);
      memcpy(m_pData+nActPos, m_pCycles, m_header.chCycleCount*sizeof(INF_BAM_CYCLE) );
      nActPos+= m_header.chCycleCount*sizeof(INF_BAM_CYCLE);
      break;
    case 2:
      m_header.dwFrameLookupTableOffset=nActPos;
      memcpy(m_pData+nActPos,m_pFrameLookup,m_nFrameLookupSize*sizeof(short) );
      nActPos+=m_nFrameLookupSize*sizeof(short);
      break;
    case 3:
      for(j=0;j<m_nFrames;j++)
      {
        memcpy(m_pData+nActPos,m_pFrameData[j].pFrameData, m_pFrameData[j].nFrameSize);
        nActPos+=m_pFrameData[j].nFrameSize;
      }
      break;
    }
    so>>=2;
  }
  memcpy(m_pData, &m_header, sizeof(m_header) );
  //now nActPos should equal m_nDataSize
  //if not, then our code is wrong
  if((unsigned long) nActPos!=m_nDataSize)
  {
    KillData();
    return -4; 
  }
  c_header.nExpandSize=m_nDataSize;
  return 0;
}

int Cbam::ExplodeBamData(bool onlyheader)
{
  BYTE *tmppoi;
  int nFrame, nCycle;
  int nOffset;
  bool bIsCompressed;
  int order[4];
  int ret,gret;

  //We calculate the save order of .bam parts so we can save it
  //in the same order.
  //We do it by storing the offsets for the four parts, marking
  //them on the least significant bits.
  //Then we sort them, and collect the markers in the saveorder
  //variable.
  gret=0;
  memset(order,0,sizeof(order));
  m_nMinFrameOffset=0x7fffffff;
  if(memcmp(m_pData,"BAM V1  ",8))
  {
    return -4;
  }
  memcpy(&m_header,m_pData,sizeof(INF_BAM_HEADER) ); //get header
  if(onlyheader) return 0;
  if(m_header.dwPaletteOffset)                       //get palette
  {
    memcpy(m_palette,m_pData+(m_header.dwPaletteOffset),256*sizeof(COLORREF) );
    order[0]=m_header.dwPaletteOffset<<2;
    m_palettesize=256*sizeof(COLORREF);
  }
  else m_palettesize=0;
  KillFrameData();

  m_nFrames=m_header.wFrameCount;
  m_nCycles=m_header.chCycleCount;
  m_pFrameData=new INF_BAM_FRAMEDATA[m_nFrames];
  m_pFrames=new INF_BAM_FRAME[m_nFrames];
  m_pCycles=new INF_BAM_CYCLE[m_nCycles];
  if(!m_pFrameData || !m_pFrames || !m_pCycles)
  {
    return -3;
  }
  tmppoi=m_pData+m_header.dwFrameOffset;
  order[1]=(m_header.dwFrameOffset<<2)|1;
  nOffset=m_header.wFrameCount*sizeof(INF_BAM_FRAME);
  memcpy(m_pFrames,(INF_BAM_FRAME *) tmppoi, nOffset);
  memcpy(m_pCycles,(INF_BAM_CYCLE *) (tmppoi+nOffset), m_header.chCycleCount*sizeof(INF_BAM_CYCLE)  );
  //get the lookup data using cycles
  m_nFrameLookupSize=0;
  for(nCycle=0;nCycle<m_header.chCycleCount;nCycle++)
  {
    if(m_pCycles[nCycle].wFrameIndexCount)
    {
      nOffset=m_pCycles[nCycle].wFirstFrameIndex+m_pCycles[nCycle].wFrameIndexCount;
      if(m_nFrameLookupSize<nOffset) m_nFrameLookupSize=nOffset;
    }
  }
  m_pFrameLookup=new short[m_nFrameLookupSize];
  if(!m_pFrameLookup)
  {
    return -3;
  }
  memcpy(m_pFrameLookup,m_pData+m_header.dwFrameLookupTableOffset,m_nFrameLookupSize*sizeof(short) ); //get framelookup
  order[2]=(m_header.dwFrameLookupTableOffset<<2)|2;
  //get the frame data using frames, also collecting the minimal frame data
  //offset so we can use it for offset ordering
  for(nFrame=0;nFrame<m_header.wFrameCount;nFrame++)
  {
    nOffset=m_pFrames[nFrame].dwFrameDataOffset&0x7fffffff;
    if(nOffset<m_nMinFrameOffset) m_nMinFrameOffset=nOffset;
  	if (m_pFrames[nFrame].dwFrameDataOffset&0x80000000)
	  	bIsCompressed = false;
	  else
		  bIsCompressed = true;
    ret=m_pFrameData[nFrame].TakeBamData(m_pData+nOffset, m_pFrames[nFrame].wWidth, m_pFrames[nFrame].wHeight, m_header.chTransparentIndex, bIsCompressed, (int) (m_nDataSize-nOffset) );
    if(ret<gret) gret=ret;
  }
  order[3]=(m_nMinFrameOffset<<2)|3;
  qsort(order,4,sizeof(int),longsort);
  m_nSaveOrder=0;
  for(nOffset=0;nOffset<4;nOffset++)
  {
    m_nSaveOrder=(m_nSaveOrder<<2)|(order[nOffset]&3);
  }
  return gret;
}

int Cbam::ReducePalette(int fhandle, bmp_header sHeader, int scanline)
{
  DWORD *prFrameBuffer=NULL;        //buffer for the whole frame (raw form)
  LPBYTE pRawData;
  int ret;
  int rows;
  unsigned int x,y;
  unsigned int nFrameWanted, nFrameSize;
  OctQuant oc;

  nFrameWanted=0;
  //the size of the pixel (3 or 4)
  rows=sHeader.bits>>3;
  
  nFrameSize=sHeader.width*sHeader.height;
  m_pFrameData[nFrameWanted].nFrameSize=nFrameSize;

  pRawData=new BYTE[nFrameSize];
  if(!pRawData)
  {
    ret=-3;
    goto endofquest;
  }
  if(m_pFrameData[nFrameWanted].pFrameData)
  {
    delete [] m_pFrameData[nFrameWanted].pFrameData;
  }
  m_pFrameData[nFrameWanted].pFrameData=pRawData;

  //we try to load the whole picture into memory,
  //if we don't succeed we'll load it in pieces
  pRawData=new BYTE [sHeader.height*scanline];
  if(!pRawData)
  {
    ret=-3;
    goto endofquest;
  }

  //allocate a temporary buffer for the raw frame data
  prFrameBuffer=new COLORREF [nFrameSize];

  if(!prFrameBuffer)
  {
    ret=-3;
    goto endofquest;
  }

  if(read(fhandle,pRawData,sHeader.height*scanline)!=(int) (sHeader.height*scanline) )
  {
    ret=-2;
    goto endofquest;
  }

  ret=0;
  
  memset(prFrameBuffer,0,nFrameSize*sizeof(COLORREF) );
  //generate palette for frames
  //if a color collides, make intelligent color reduction
  //if it is only 3 bytes expand palette to 4 bytes
  //collect a frame's data in prFrameBuffer
  for(y=0;y<sHeader.height;y++)
  {
    for(x=0;x<sHeader.width;x++)
    {
      //pcBuffer consists of bytes, so we multiply x with 3 or 4
      memcpy(prFrameBuffer+y*sHeader.width+x,pRawData+(sHeader.height-y-1)*scanline+x*rows,rows);
    }     
  }
  //cpoint is required due to one of the color reduction algorithms
  oc.BuildTree(prFrameBuffer,GetFrameData(nFrameWanted),CPoint(nFrameSize,1),m_palette);
endofquest:
  if(prFrameBuffer) delete [] prFrameBuffer;
  if(pRawData) delete [] pRawData;
  return ret;
}

int Cbam::ReadBmpFromFile(int fhandle, int ml)
{
  bmp_header sHeader;      //BMP header
  unsigned char *pcBuffer; //buffer for a scanline
  int scanline;            //size of a scanline in bytes (ncols * bytesize of a pixel)
  long original;
  unsigned int y;
  unsigned int nSourceOff; //the source offset in the original scanline (which we cut up)
  int ret;

  new_bam();
  pcBuffer=NULL;
  if(ml<0) ml=filelength(fhandle);
  if(ml<0) return -2;
  original=tell(fhandle);
  if(read(fhandle,&sHeader,sizeof(bmp_header))!=sizeof(bmp_header) )
  {
    return -2;
  }
  if((*(unsigned short *) sHeader.signature)!='MB') return -2; //BM
  if(sHeader.fullsize>(unsigned long) ml) return -2;
  if(sHeader.headersize!=0x28) return -2;
  if(sHeader.planes!=1) return -1;                  //unsupported
  if(sHeader.compression!=BI_RGB) return -1;        //unsupported
  if(sHeader.height<0 || sHeader.width<0) return -1; //unsupported
  if(sHeader.height>1024 || sHeader.width>1024) return -1; //unsupported
  //read palette if we got one
  if(sHeader.bits==8 || sHeader.bits==4)
  {
    if(!sHeader.colors)
    {
      if(sHeader.bits==8) sHeader.colors=256;
      else sHeader.colors=16;
    }
    m_palettesize=sHeader.colors*sizeof(RGBQUAD);
    if(read(fhandle,&m_palette, m_palettesize)!=m_palettesize)
    {
      return -2;
    }
  }
  else
  {
    m_palettesize=0;
  }
  //check if the file fits in the claimed length
  if(sHeader.offset!=m_palettesize+sHeader.headersize+((unsigned char *) &sHeader.headersize-(unsigned char *) &sHeader) ) return -2;
  scanline=(sHeader.width*sHeader.bits+7)>>3;
  if(scanline&3) scanline+=4-(scanline&3);
  
  if(scanline*sHeader.height+sHeader.offset>(unsigned long) ml)
  {
    scanline=(sHeader.width*sHeader.bits+7)>>3;
    if(scanline*sHeader.height+sHeader.offset>(unsigned long) ml)
    {
      return -2;
    }
  }
  
  //check if we are on the position we wanted to be
  nSourceOff=tell(fhandle);
  if(sHeader.offset!=nSourceOff-original ) return -2;

  m_nFrameLookupSize=1;
  m_pFrameLookup=new short[m_nFrameLookupSize];
  if(!m_pFrameLookup)
  {
    ret=-3;
    goto endofquest;
  }
  m_nCycles=1;
  m_header.chCycleCount=1;
  m_pCycles=new INF_BAM_CYCLE[m_nCycles];
  if(!m_pCycles)
  {
    ret=-3;
    goto endofquest;
  }
  m_nFrames=1;  //we can import only one frame
  m_header.wFrameCount=1;
  m_pFrames=new INF_BAM_FRAME[m_nFrames];
  if(!m_pFrames)
  {
    ret=-3;
    goto endofquest;
  }
  m_pFrameData=new INF_BAM_FRAMEDATA[m_nFrames];
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

  ret=-1; //unsupported mode
  switch(sHeader.bits)
  {
  case 4:
    for(y=0;y<sHeader.height;y++)
    {
      if(read(fhandle, pcBuffer, scanline)!=scanline)
      {
        ret=-2;
        goto endofquest;
      }
      ret=m_pFrameData[0].TakeBmp4Data(pcBuffer, sHeader,y);      
      if(ret) goto endofquest;
    }
    break;
  case 8: //this format has a palette
    //throw bitmaps into separate frames, y starts from bottom up
    for(y=0;y<sHeader.height;y++)
    {
      if(read(fhandle, pcBuffer, scanline)!=scanline)
      {
        ret=-2;
        goto endofquest;
      }
      ret=m_pFrameData[0].TakeBmpData(pcBuffer, sHeader,y);      
      if(ret) goto endofquest;
    }
    break;
  case 24: case 32: //truecolor frame, needs palette reduction
    ret=ReducePalette(fhandle, sHeader, scanline);
    break;
  }
endofquest:
  close(fhandle);
  if(pcBuffer) delete [] pcBuffer;

  if(!ret)
  {
    memset(m_pFrames,0,sizeof(INF_BAM_FRAME) );
    m_pFrames[0].dwFrameDataOffset=0x80000000;
    m_pFrames[0].wWidth=(unsigned short) sHeader.width;
    m_pFrames[0].wHeight=(unsigned short) sHeader.height;
    m_header.wFrameCount=(unsigned short) m_nFrames;
    memset(m_pCycles,0,sizeof(INF_BAM_CYCLE) );
    m_pCycles[0].wFirstFrameIndex=0;
    m_pCycles[0].wFrameIndexCount=1;
    m_pFrameLookup[0]=0;
  }

  return ret;
}

int Cbam::ReadPltFromFile(int fhandle, int ml)
{
  DWORD dwSize;
  WORD *filedata;
  int i;
  int ret;

  if(ml<0) ml=filelength(fhandle);
  if((ml<sizeof(plt_header)) || (ml>100000) ) return -2; //file is bad
  if(read(fhandle,&m_pltheader,sizeof(plt_header) )!=sizeof(plt_header) )
  {
    return -2;
  }
  m_bCompressed=false;
  dwSize=ml-sizeof(plt_header);
  if(memcmp(m_pltheader.chSignature,"PLT V1  ",8) )
  {
    return -1;
  }
  if(dwSize!=m_pltheader.dwWidth*m_pltheader.dwHeight)
  {
    return -2;
  }
  filedata = new WORD [m_pltheader.dwWidth];
  if(!filedata)
  {
    return -3;
  }
  ret = 0;
  for(i=0;i<256;i++)
  {
    m_palette[i]=RGB(i,i,i);
  }
  m_nFrameLookupSize=1;
  m_pFrameLookup=new short[m_nFrameLookupSize];
  if(!m_pFrameLookup)
  {
    ret=-3;
    goto endofquest;
  }
  m_nCycles=1;
  m_pCycles=new INF_BAM_CYCLE[m_nCycles];
  if(!m_pCycles)
  {
    ret=-3;
    goto endofquest;
  }
  m_nFrames=1;  //we can import only one frame
  m_pFrames=new INF_BAM_FRAME[m_nFrames];
  if(!m_pFrames)
  {
    ret=-3;
    goto endofquest;
  }
  m_pFrameData=new INF_BAM_FRAMEDATA[m_nFrames];
  if(!m_pFrameData)
  {
    ret=-3;
    goto endofquest;
  }
  dwSize=m_pltheader.dwWidth*sizeof(WORD);
  for(i=0;i<(int) m_pltheader.dwHeight;i++)
  { 
    if(read(fhandle,filedata,dwSize)!=(int) dwSize)
    {
      ret=-2;
      goto endofquest;
    }
    m_pFrameData[0].TakePltData(filedata, m_pltheader, i);
  }
endofquest:
  delete [] filedata;
  return ret;
}

int Cbam::ReadBamFromFile(int fhandle, int ml, bool onlyheader)
{
  BYTE *pCData;
  DWORD dwSize;
  int ret;

  if(ml<0) ml=filelength(fhandle);
  if((ml<sizeof(INF_BAM_HEADER)) || (ml>10000000) ) return -2; //file is bad

  //we assume it is a compressed header (compressed headers are shorter)
  if(read(fhandle,&c_header,sizeof(INF_BAMC_HEADER) )!=sizeof(INF_BAMC_HEADER) )
  {
    return -2;
  }
  m_bCompressed=true;
  m_palettesize=256;
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

    if(onlyheader) dwSize=sizeof(m_header)-sizeof(c_header);
    else dwSize=m_nDataSize-sizeof(c_header);
    memcpy(m_pData,&c_header, sizeof(c_header) ); //move the pre-read part to the header
	  if(read(fhandle,m_pData+sizeof(c_header),dwSize)!=(int) dwSize)
    {
      KillData();
      return -2;
    }

    memcpy(&c_header,"BAMCV1  ",8);
    c_header.nExpandSize=m_nDataSize;
    goto endofquest;
  }

  ml-=sizeof(INF_BAMC_HEADER);
  if(onlyheader && (ml>8192 ) ) ml=8192;
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

  if(onlyheader) c_header.nExpandSize=sizeof(INF_BAM_HEADER);

  if(c_header.nExpandSize>10000000) //don't handle bams larger than 10M
  {
    delete [] pCData;
    return -4; //illegal data
  }

  m_nDataSize=c_header.nExpandSize;
 	m_pData = new BYTE[m_nDataSize];
  if (!m_pData)
    return -3;

  dwSize=c_header.nExpandSize;
	ret=uncompress(m_pData,&c_header.nExpandSize,pCData,ml);
  delete [] pCData;
  if(onlyheader)
  {
    ret=Z_OK;
    goto endofquest;
  }
  if(ret != Z_OK)
	{
    KillData();
		return -4; //uncompress failed
	}

  if(dwSize!=c_header.nExpandSize)
  {
    KillData();
    return -4; //pre-saved uncompressed length isn't the same, why ?
  }

endofquest:
  ret=ExplodeBamData(onlyheader);
  KillData();
  return ret;
}

int Cbam::GetFrameCount()
{
  return m_header.wFrameCount;
}

int Cbam::GetCycleCount()
{
  return m_header.chCycleCount;
}

//returns firstframeindex and frameindexcount
CPoint Cbam::GetCycleData(int nCycle)
{
  if(nCycle<0) return (DWORD) -1;
  if(nCycle>=m_nCycles) return (DWORD) -1;
  return CPoint(m_pCycles[nCycle].wFirstFrameIndex, m_pCycles[nCycle].wFrameIndexCount);
}

int Cbam::SetCycleData(int nCycle, int x, int y)
{
  if(nCycle<0) return (DWORD) -1;
  if(nCycle>=m_nCycles) return (DWORD) -1;
  m_pCycles[nCycle].wFirstFrameIndex=(unsigned short) x;
  m_pCycles[nCycle].wFrameIndexCount=(unsigned short) y;
  return 0;
}

int Cbam::GetFrameIndex(int nCycle, int nIndex)
{
  if(nCycle<0) return -1;
  if(!m_pCycles) return -1;
  if(nCycle>=m_nCycles) return -1;
  if(!m_pFrameLookup) return -2;
  if(nIndex>=m_pCycles[nCycle].wFrameIndexCount)
    return -2;
  return m_pFrameLookup[m_pCycles[nCycle].wFirstFrameIndex+nIndex];
}

//alters an existing frame index in the frame lookup table
int Cbam::SetFrameIndex(int nCycle, int nIndex, int nFrameIndex)
{
  if(nCycle<0) return -1;
  if(!m_pCycles) return -1;
  if(nCycle>=m_nCycles) return -1;
  if(!m_pFrameLookup) return -2;
  if(nIndex>=m_pCycles[nCycle].wFrameIndexCount)
    return -2;
  m_pFrameLookup[m_pCycles[nCycle].wFirstFrameIndex+nIndex]=(short) nFrameIndex;
  return 0;
}

int Cbam::FlipFrame(int nFrameWanted)
{
  int RLE;
  int ret;

  RLE=GetFrameRLE(nFrameWanted);
  ret=SetFrameRLE(nFrameWanted, false);
  if(!ret) return -1;
  m_pFrameData[nFrameWanted].FlipFrame(m_pFrames[nFrameWanted].wWidth,m_pFrames[nFrameWanted].wHeight);
  m_pFrames[nFrameWanted].wCenterX=(short) (m_pFrames[nFrameWanted].wWidth-m_pFrames[nFrameWanted].wCenterX);
  SetFrameRLE(nFrameWanted,RLE);
  return 0;
}

int Cbam::SwapFrames(int a, int b)
{
  short tmp;

  if(a<0 || a>m_nFrameLookupSize) return -1;
  if(b<0 || b>m_nFrameLookupSize) return -1;
  tmp=m_pFrameLookup[a];
  m_pFrameLookup[a]=m_pFrameLookup[b];
  m_pFrameLookup[b]=tmp;
  return 0;
}

int Cbam::SetFramePos(int nFrameWanted, int x, int y)
{
  if(nFrameWanted<0) return (DWORD) -1;
  if(!m_pFrames) return -1;
	if (m_nFrames <= nFrameWanted )  //not enough frames
		return -1;
  m_pFrames[nFrameWanted].wCenterX=(short) x;
  m_pFrames[nFrameWanted].wCenterY=(short) y;
  return 0;
}

//alters nFrameWanted
int Cbam::FindFrame(int nFrameWanted, int &nIndex)
{
  int nCycleWanted;
  CPoint data;

  for(nCycleWanted=0;nCycleWanted<m_nCycles;nCycleWanted++)
  {
    data=GetCycleData(nCycleWanted);
    for(nIndex=0;nIndex<data.y;nIndex++)
    {
      if(nFrameWanted==m_pFrameLookup[data.x+nIndex])
      {
        return nCycleWanted;
      }
    }
  }
  return -1;
}

//drops unreferenced frames, reports the number
int Cbam::DropUnusedFrame(int flag)
{
  int nFrameWanted;
  int nKilled;
  int dummy;

  nKilled=0;
  for(nFrameWanted=0;nFrameWanted<m_nFrames;nFrameWanted++)
  {
    if(FindFrame(nFrameWanted, dummy)<0 )
    {
      if(flag)
      {
        DropFrame(nFrameWanted);
        nFrameWanted--;
      }
      nKilled++;
    }
  }
  return nKilled;
}

int Cbam::ReplaceCycle(int nCycleWanted, short *pNewLookup, int nLen)
{
  int tmp;

  if(!m_pCycles) return (DWORD) -1;
  if(nCycleWanted<0) return (DWORD) -1;
  if(m_nCycles <= nCycleWanted) return (DWORD) -1;
  tmp=m_pCycles[nCycleWanted].wFrameIndexCount;
  if(nLen>tmp)
  {
    tmp=AddFrameToCycle(nCycleWanted, 0, 0, nLen-tmp);
    if(tmp) return tmp;
  }
  else
  {
    if(nLen<tmp)
    {
      tmp=RemoveFrameFromCycle(nCycleWanted,0,tmp-nLen);
      if(tmp) return tmp;
    }
  }
  memcpy(m_pFrameLookup+m_pCycles[nCycleWanted].wFirstFrameIndex,pNewLookup,nLen*sizeof(short));
  DropUnusedFrame(1); //drop unused frames
  return 0;
}

int Cbam::DropCycle(int nCycleWanted)
{
  INF_BAM_CYCLE *newCycles;

  if(!m_pCycles) return (DWORD) -1;
  if(nCycleWanted<0) return (DWORD) -1;
  if(m_nCycles <= nCycleWanted) return (DWORD) -1;
  m_header.chCycleCount--;
  newCycles=new INF_BAM_CYCLE[m_header.chCycleCount];
  if(!newCycles)
  {
    m_header.chCycleCount++;
    return -3;
  }
  memcpy(newCycles,m_pCycles,sizeof(INF_BAM_CYCLE)*nCycleWanted);
  memcpy(newCycles+nCycleWanted,m_pCycles+nCycleWanted+1,sizeof(INF_BAM_CYCLE)*(m_nCycles-nCycleWanted-1) );
  delete [] m_pCycles;
  m_pCycles=newCycles;
  m_nCycles--;
  DropUnusedFrame(1); //drop unused frames
  return 0;
}

//adds a completely new cycle, returns the cycle number
//-1 will add one to the end
int Cbam::InsertCycle(int nCycleWanted)
{
  INF_BAM_CYCLE *newCycles;
  int FirstFrameIndex;

  if(!m_pCycles && nCycleWanted) return -1;
  if(nCycleWanted<0)
  {
    nCycleWanted=m_nCycles;
  }
  if(m_nCycles <= nCycleWanted)
  {
    nCycleWanted=m_nCycles;
  }
  if(m_nCycles)
  {
    if(nCycleWanted<m_nCycles) FirstFrameIndex=m_pCycles[nCycleWanted].wFirstFrameIndex;
    else FirstFrameIndex=m_pCycles[nCycleWanted-1].wFirstFrameIndex+m_pCycles[nCycleWanted-1].wFrameIndexCount;
  }
  else FirstFrameIndex=0;
  m_nCycles++;
  newCycles=new INF_BAM_CYCLE[m_nCycles];
  if(!newCycles)
  {
    m_nCycles--;
    return -3;
  }
  if(m_pCycles)
  {
    memcpy(newCycles,m_pCycles,sizeof(INF_BAM_CYCLE)*nCycleWanted);
    memcpy(newCycles+nCycleWanted+1,m_pCycles+nCycleWanted,sizeof(INF_BAM_CYCLE)*(m_nCycles-nCycleWanted-1) );
  }
  newCycles[nCycleWanted].wFirstFrameIndex=(unsigned short) FirstFrameIndex;
  newCycles[nCycleWanted].wFrameIndexCount=0;
  if(m_pCycles) delete [] m_pCycles;
  m_pCycles=newCycles;
  m_header.chCycleCount=(BYTE) m_nCycles;
  return nCycleWanted;
}

//recalculates all the frame lookup tables, drops unused indices
int Cbam::ConsolidateFrameLookup()
{
  CPoint CycleData;
  int nCycle;
  int i,x,y;
  short *newFrameLookup;

  //consolidate framelookup tables
  x=0;
  newFrameLookup=new short[m_nFrameLookupSize];
  if(!newFrameLookup) return -3;
  for(nCycle=0;nCycle<m_nCycles;nCycle++)
  {
    CycleData=GetCycleData(nCycle);
    y=0;
    for(i=CycleData.x;i<CycleData.x+CycleData.y;i++)
    {
      if(m_pFrameLookup[i]>=0)
      {
        newFrameLookup[x+y]=m_pFrameLookup[i];
        y++;
      }
    }
    SetCycleData(nCycle, x, y);
    x+=y;
    if(x>m_nFrameLookupSize)
    {
      return -1;
    }
  }
  delete [] m_pFrameLookup;
  m_pFrameLookup=newFrameLookup;
  m_nFrameLookupSize=x;
  return 0;
}

//inserts an existing frame into a lookup table
int Cbam::AddFrameToCycle(int nCycle, int nCyclePos, int nFrameWanted, int nRepeat)
{
  int i;
  CPoint CycleData;
  short *newFrameLookup;

  if(!nRepeat) nRepeat=1;
  CycleData=GetCycleData(nCycle);
  if(CycleData.x<0) return -1;
  newFrameLookup=new short [m_nFrameLookupSize+nRepeat];
  if(!newFrameLookup) return -3;
  //
  nCyclePos+=CycleData.x;
  memcpy(newFrameLookup,m_pFrameLookup,sizeof(short)*nCyclePos);
  memcpy(newFrameLookup+nCyclePos+nRepeat,m_pFrameLookup+nCyclePos,sizeof(short)*(m_nFrameLookupSize-nCyclePos) );
  for(i=0;i<nRepeat;i++)
  {
    newFrameLookup[nCyclePos+i]=(short) nFrameWanted++;  
  }
 
  //adjusting the next cycle
  m_pCycles[nCycle].wFrameIndexCount=(unsigned short) (CycleData.y+nRepeat);
  //adjusting the cycle data
  for(i=0;i<m_nCycles;i++)
  {
    if(i!=nCycle)
    {
      if(m_pCycles[i].wFirstFrameIndex>=nCyclePos)
      {
        m_pCycles[i].wFirstFrameIndex=(unsigned short) (m_pCycles[i].wFirstFrameIndex+nRepeat);
      }
    }
  }
  delete [] m_pFrameLookup;
  m_pFrameLookup=newFrameLookup;
  m_nFrameLookupSize+=nRepeat;

  return ConsolidateFrameLookup();
}

//removes existing frames from a lookup table
int Cbam::RemoveFrameFromCycle(int nCycle, int nCyclePos, int nRepeat)
{
  int i;
  CPoint CycleData;
  short *newFrameLookup;

  CycleData=GetCycleData(nCycle);
  if(CycleData.x<0) return -1;
  newFrameLookup=new short [m_nFrameLookupSize-=nRepeat];
  if(!newFrameLookup)
  {
    m_nFrameLookupSize+=nRepeat;
    return -3;
  }
  if(m_pCycles[nCycle].wFrameIndexCount<nCyclePos+nRepeat) return -1; //can't remove so much
  //
  nCyclePos+=CycleData.x;
  memcpy(newFrameLookup,m_pFrameLookup,sizeof(short)*nCyclePos);
  memcpy(newFrameLookup+nCyclePos,m_pFrameLookup+nCyclePos+nRepeat,sizeof(short)*(m_nFrameLookupSize-nCyclePos) );
 
  //adjusting the cycle
  m_pCycles[nCycle].wFrameIndexCount=(unsigned short) (CycleData.y-nRepeat);
  //adjusting the cycle data
  for(i=0;i<m_nCycles;i++)
  {
    if(i!=nCycle)
    {
      if(m_pCycles[i].wFirstFrameIndex>nCyclePos)
      {
        m_pCycles[i].wFirstFrameIndex=(short) (m_pCycles[i].wFirstFrameIndex-nRepeat);
      }
    }
  }
  delete [] m_pFrameLookup;
  m_pFrameLookup=newFrameLookup;

  return ConsolidateFrameLookup();
}
//tmpbam.palette,tmpbam.GetFrameData(0),tmpbam.GetFrameDataSize(0)
//palettetype &oldpalette, const LPBYTE pRawData, int nFrameSize
int Cbam::ImportFrameData(int nFrameIndex, Cbam &tmpbam, int nImportFrameIndex)
{
  CPoint point;
  OctQuant oc;

  memset(m_pFrames+nFrameIndex,0,sizeof(INF_BAM_FRAME) );
  m_pFrames[nFrameIndex].dwFrameDataOffset=0x80000000;
  point=tmpbam.GetFramePos(nImportFrameIndex);
  SetFramePos(nFrameIndex,point.x,point.y);
  
  point=tmpbam.GetFrameSize(nImportFrameIndex);  
  m_pFrames[nFrameIndex].wWidth=(unsigned short) point.x;
  m_pFrames[nFrameIndex].wHeight=(unsigned short) point.y;

  oc.AddPalette(m_palette); //feeds palette into octtree
  return oc.QuantizeAllColors(m_pFrameData[nFrameIndex].pFrameData,
    tmpbam.GetFrameData(nImportFrameIndex),point,tmpbam.m_palette,m_palette);
}

//creates a new, empty frame; readjusts lookup
int Cbam::AddFrame(int nFrameWanted, int nNewFrameSize)
{
  INF_BAM_FRAME *newFrames;
  INF_BAM_FRAMEDATA *newFrameData;
  int nFrameLookupPos;
  
  if(nFrameWanted>65500) return -1;
  if (m_nFrames < nFrameWanted ) //allow frame to be one bigger (insert as last frame)
  {
    //not enough frames
    return -1;
  }
  newFrames=new INF_BAM_FRAME[m_nFrames+1];
  if(!newFrames) return -3;
  newFrameData=new INF_BAM_FRAMEDATA[m_nFrames+1];
  if(!newFrameData) return -3;
  if(m_pFrames)
  {
    memcpy(newFrames,m_pFrames,sizeof(INF_BAM_FRAME)*nFrameWanted);
    memcpy(newFrames+nFrameWanted+1,m_pFrames+nFrameWanted,sizeof(INF_BAM_FRAME)*(m_nFrames-nFrameWanted) );
    delete [] m_pFrames;
  }
  memset(newFrames+nFrameWanted,0,sizeof(INF_BAM_FRAME) );
  if(m_pFrameData)
  {
    memcpy(newFrameData,m_pFrameData,sizeof(INF_BAM_FRAMEDATA)*nFrameWanted);
    memcpy(newFrameData+nFrameWanted+1,m_pFrameData+nFrameWanted,sizeof(INF_BAM_FRAMEDATA)*(m_nFrames-nFrameWanted) );
    //we keep all the original data
    for(nFrameLookupPos=0;nFrameLookupPos<m_nFrames;nFrameLookupPos++)
    {
      m_pFrameData[nFrameLookupPos].pFrameData=NULL;
    }
    delete [] m_pFrameData;
  }

  m_pFrames=newFrames;
  m_pFrameData=newFrameData;
  m_pFrameData[nFrameWanted].pFrameData=new BYTE[nNewFrameSize]; 
  m_pFrameData[nFrameWanted].nFrameSize=nNewFrameSize;
  m_nFrames++;
  m_header.wFrameCount++;
  //increasing frame lookup positions which are higher than our insert position
  for(nFrameLookupPos=0;nFrameLookupPos<m_nFrameLookupSize;nFrameLookupPos++)
  {
    if(m_pFrameLookup[nFrameLookupPos]>=nFrameWanted) m_pFrameLookup[nFrameLookupPos]++;
  }
  return ConsolidateFrameLookup();
}

int Cbam::DropFrame(int nFrameWanted)
{
  INF_BAM_FRAME *newFrames;
  INF_BAM_FRAMEDATA *newFrameData;
  int nFrameLookupPos;

  if(!m_pFrames) return -1;
  if(nFrameWanted<0) return -1;
  if (m_nFrames <= nFrameWanted )
  {
    //not enough frames
    return -1;
  }
  m_header.wFrameCount--;
  m_nFrames--;
  newFrames=new INF_BAM_FRAME[m_nFrames];
  if(!newFrames)
  {
    m_nFrames++;
    m_header.wFrameCount++;
    return -3;
  }
  newFrameData=new INF_BAM_FRAMEDATA[m_nFrames];
  if(!newFrameData)
  {
    m_nFrames++;
    m_header.wFrameCount++;
    return -3;
  }
  memcpy(newFrames,m_pFrames,sizeof(INF_BAM_FRAME)*nFrameWanted);
  memcpy(newFrames+nFrameWanted,m_pFrames+nFrameWanted+1,sizeof(INF_BAM_FRAME)*(m_nFrames-nFrameWanted) );
  memcpy(newFrameData,m_pFrameData,sizeof(INF_BAM_FRAMEDATA)*nFrameWanted);
  memcpy(newFrameData+nFrameWanted,m_pFrameData+nFrameWanted+1,sizeof(INF_BAM_FRAMEDATA)*(m_nFrames-nFrameWanted) );
  delete [] m_pFrames;
  //delete the only raw data to be deleted
  delete m_pFrameData[nFrameWanted].pFrameData;
  //hack to keep the other raw data intact
  for(nFrameLookupPos=0;nFrameLookupPos<=m_nFrames;nFrameLookupPos++)
  {
    m_pFrameData[nFrameLookupPos].pFrameData=NULL;
  }
  delete [] m_pFrameData;
  m_pFrames=newFrames;
  m_pFrameData=newFrameData;
  for(nFrameLookupPos=0;nFrameLookupPos<m_nFrameLookupSize;nFrameLookupPos++)
  {
    if(m_pFrameLookup[nFrameLookupPos]==nFrameWanted) m_pFrameLookup[nFrameLookupPos]=-1;
    else if(m_pFrameLookup[nFrameLookupPos]>nFrameWanted) m_pFrameLookup[nFrameLookupPos]--;
  }
  return ConsolidateFrameLookup();
}

int Cbam::SetFrameData(int nFrameWanted, LPBYTE pFrameData,const CPoint &cpFrameSize)
{
  if(nFrameWanted<0) return -1;
  if(!m_pFrameData) return -1;
	if (m_nFrames <= nFrameWanted )  //not enough frames
		return -1;
  memset(m_pFrames+nFrameWanted,0,sizeof(INF_BAM_FRAME) );
  m_pFrames[nFrameWanted].dwFrameDataOffset=0x80000000;
  m_pFrames[nFrameWanted].wWidth=(unsigned short) cpFrameSize.x;
  m_pFrames[nFrameWanted].wHeight=(unsigned short) cpFrameSize.y;
  m_pFrameData[nFrameWanted].TakeBamData(pFrameData,cpFrameSize.x,cpFrameSize.y,
    0,false,cpFrameSize.x*cpFrameSize.y);
  return 0;
}

int Cbam::DetachFrameData(int nFrameWanted)
{
	if (m_nFrames <= nFrameWanted )  //not enough frames
		return -1;
  m_pFrameData[nFrameWanted].pFrameData=NULL;
  new_bam();
  return 0;
}

LPBYTE Cbam::GetFrameData(int nFrameWanted)
{
  if(nFrameWanted<0) return NULL;
  if(!m_pFrameData) return NULL;
	if (m_nFrames <= nFrameWanted )  //not enough frames
		return NULL;
  return m_pFrameData[nFrameWanted].pFrameData;
}

CPoint Cbam::GetFramePos(int nFrameWanted)
{
  if(nFrameWanted<0) return (DWORD) -1;
  if(!m_pFrames) return (DWORD) -1;
	if (m_nFrames <= nFrameWanted )  //not enough frames
		return (DWORD) -1;
  return CPoint(m_pFrames[nFrameWanted].wCenterX,m_pFrames[nFrameWanted].wCenterY);
}

bool Cbam::GetFrameRLE(int nFrameWanted)
{
  if(nFrameWanted<0) return false;
  if(!m_pFrames) return false;
	if (m_nFrames <= nFrameWanted )  //not enough frames
		return false;
  return !(m_pFrames[nFrameWanted].dwFrameDataOffset&0x80000000);
}

int Cbam::GetFrameDataSize(int nFrameWanted)
{
  if(nFrameWanted<0) return -1;
  if(!m_pFrames) return -1;
	if (m_nFrames <= nFrameWanted )  //not enough frames
		return -1;
  return m_pFrameData[nFrameWanted].nFrameSize;
}

bool Cbam::SetFrameRLE(int nFrameWanted, BOOL NewRLE)
{
  BOOL RLE;
  int ret;
  BYTE *pNewFrameData;
  int nExpectedSize;

  if(nFrameWanted<0) return false;
  if(!m_pFrames) return false;
	if (m_nFrames <= nFrameWanted )  //not enough frames
		return false;
  RLE=!(m_pFrames[nFrameWanted].dwFrameDataOffset&0x80000000);
  if(RLE==NewRLE) return true; //made it without effort!
  if(NewRLE)
  {
    ret=m_pFrameData[nFrameWanted].RLECompression(m_header.chTransparentIndex); 
    m_pFrames[nFrameWanted].dwFrameDataOffset&=0x7fffffff; //turn off the uncompressed flag
    if(ret<0) return false;
  }
  else
  {
    ret=m_pFrameData[nFrameWanted].RLEDecompression(m_header.chTransparentIndex); 
    m_pFrames[nFrameWanted].dwFrameDataOffset|=0x80000000; //turn on the uncompressed flag
    if(ret<0) return false;
    nExpectedSize=m_pFrames[nFrameWanted].wHeight*m_pFrames[nFrameWanted].wWidth;
    if(ret!=nExpectedSize) //correcting bam by cutting it to size according to its dimensions
    {
      pNewFrameData=new BYTE[nExpectedSize];
      if(!pNewFrameData) return false; //cannot correct it due to memory shortage
      memset(pNewFrameData,m_header.chTransparentIndex,nExpectedSize);
      memcpy(pNewFrameData,m_pFrameData[nFrameWanted].pFrameData,min(nExpectedSize,ret));
      delete [] m_pFrameData[nFrameWanted].pFrameData;
      m_pFrameData[nFrameWanted].pFrameData=pNewFrameData;
      m_pFrameData[nFrameWanted].nFrameSize=nExpectedSize;
      return false;
    }
  }
  return true; //made it!
}

CPoint Cbam::GetFrameSize(int nFrameWanted)
{
  if(nFrameWanted<0) return (DWORD) -1;
  if(!m_pFrames) return (DWORD) -1;
	if (m_nFrames <= nFrameWanted )  //not enough frames
		return (DWORD) -1;
  return CPoint(m_pFrames[nFrameWanted].wWidth, m_pFrames[nFrameWanted].wHeight);
}

//adds a bam to an alien m_pclrDIBits area, without creating bitmap
int Cbam::MakeBitmap(int nFrameWanted, COLORREF clrTrans, Cmos &host, int nMode, int nXpos, int nYpos)
{
  int nColumn, nRow;
  int nLimit;
  int nOffset;
  int nReplaced;
	bool bIsCompressed;

  if(nFrameWanted<0) return -1;

	if (m_nFrames <= nFrameWanted )  //not enough frames
		return -1;

	if (m_pFrames[nFrameWanted].wWidth > MAX_ICON_WIDTH || m_pFrames[nFrameWanted].wHeight > MAX_ICON_HEIGHT)
		return -1;

	if (m_pFrames[nFrameWanted].dwFrameDataOffset&0x80000000)
		bIsCompressed = false;
	else
		bIsCompressed = true;

  if(nMode&BM_OVERLAY)
  {
    if(nMode&BM_MATCH) clrTrans=(DWORD) -1;
    else if(nMode&BM_INVERT) clrTrans=(DWORD) -2;
    else clrTrans=(DWORD) 0xffffff;
    //the size of the original bitmap
    nColumn=host.mosheader.wColumn*host.mosheader.dwBlockSize;
    nRow=host.mosheader.wRow*host.mosheader.dwBlockSize;
    nOffset=nXpos+nYpos*nColumn;
    nLimit=min(m_pFrames[nFrameWanted].wHeight,nRow-nYpos);
  }
  else
  {
    return -1;
  }

  if(nOffset>=0)
  {
    nReplaced=m_pFrameData[nFrameWanted].ExpandBamBits(m_header.chTransparentIndex,clrTrans,
      host.GetDIB()+nOffset,bIsCompressed,nColumn,m_palette,
      m_pFrames[nFrameWanted].wWidth,nLimit);
  }
  else nReplaced=0;
	
	return nReplaced;
}

//nWidth/nHeight has 2 uses, thus some modes couldn't be mixed
// nMode ==BM_RESIZE  - new dimensions
// nMode ==BM_OVERLAY - overlay position

int Cbam::MakeBitmap(int nFrameWanted, COLORREF clrTrans, HBITMAP &hBitmap, int nMode, int nWidth, int nHeight)
{
  int nColumn, nRow;
  int nLimit;
  int nOffset;
  int nReplaced;
	bool bIsCompressed;

  if(hBitmap)
  {
    if(!(nMode&BM_OVERLAY) )
    {
      DeleteObject(hBitmap);
      hBitmap=0;
    }
  }
  else
  {
    if(nMode&BM_OVERLAY) return -1; //can overlay only onto existing images
  }
  if(nFrameWanted<0) return -1;

	if (m_nFrames <= nFrameWanted )  //not enough frames
		return -1;

	if (m_pFrames[nFrameWanted].wWidth > MAX_ICON_WIDTH || m_pFrames[nFrameWanted].wHeight > MAX_ICON_HEIGHT)
		return -1;

	if (m_pFrames[nFrameWanted].dwFrameDataOffset&0x80000000)
		bIsCompressed = false;
	else
		bIsCompressed = true;

  if(nMode&BM_OVERLAY)
  {
    BITMAP bm;

    if(nMode&BM_MATCH) clrTrans=(DWORD) -1;
    else clrTrans=(DWORD) 0xffffff;
    CBitmap *cb=CBitmap::FromHandle(hBitmap);
    cb->GetBitmap(&bm);
    //the size of the original bitmap
    nColumn=bm.bmWidth;
    nRow=bm.bmHeight;    
    Reallocate(nColumn,nRow);
    //this is a device dependent method, works only on 32 bit video display
    cb->GetBitmapBits(nColumn*nRow*sizeof(COLORREF),m_pclrDIBits);
//    memcpy(m_pclrDIBits,bm.bmBits,nColumn*nRow*sizeof(COLORREF) ); //doesn't work. bm.bmbits isn't set
    nOffset=nWidth+nHeight*nColumn;
    nLimit=min(m_pFrames[nFrameWanted].wHeight,nRow-nHeight);
  }
  else
  {
    //the size of the bitmap
    nColumn=m_pFrames[nFrameWanted].wWidth;
    nLimit=nRow=m_pFrames[nFrameWanted].wHeight;
    Reallocate(nColumn,nRow);
    nOffset=0;
  }

  if(nOffset>=0)
  {
    nReplaced=m_pFrameData[nFrameWanted].ExpandBamBits(m_header.chTransparentIndex,clrTrans,
      m_pclrDIBits+nOffset,bIsCompressed,nColumn,m_palette,
      m_pFrames[nFrameWanted].wWidth,nLimit);
  }
  else nReplaced=0;
  if (!MakeBitmapExternal(m_pclrDIBits,nColumn, nRow, hBitmap))
    return -1;
  
  if (nMode&BM_RESIZE)
		if (!FitAndCenterBitmap(hBitmap,clrTrans,nWidth,nHeight))
			return -1;

	return nReplaced;
}

int Cbam::GetTextExtent(CString text)
{
  int nFrameWanted;
  int nWidth;
  int i;

  nWidth=0;
  for(i=0;i<text.GetLength();i++)
  {
    nFrameWanted=GetFrameIndex(text.GetAt(i)-1,0);
    nWidth+=GetFrameSize(nFrameWanted).x;
  }
  return nWidth;
}
//drawing a text using this bam font over the bitmap
int Cbam::DrawText(CString text,COLORREF clrTrans, Cmos &host, int nMode,
                   int nXpos, int nYpos, int nWidth)
{
  int nFrameWanted;
  int ret;
  int i;

  i=GetTextExtent(text);
  switch(nMode&BM_JUSTIFY_MASK)
  {
  case BM_CENTER:
    nXpos+=(nWidth-i)/2;
      break;
  case BM_RIGHT:
    nXpos+=nWidth-i;
    break;
    //default
  }
  if(nXpos<0) nXpos=0;
  for(i=0;i<text.GetLength();i++)
  {
    nFrameWanted=GetFrameIndex(text.GetAt(i)-1,0);
    ret=MakeBitmap(nFrameWanted,clrTrans,host,nMode,nXpos,nYpos);
    if(ret) break;
    nXpos+=GetFrameSize(nFrameWanted).x;
  }
  return ret;
}

static int colourorder(const void *a, const void *b)
{
  int asum, bsum;
  int tmp;
  int i;

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

void Cbam::OrderPalette()
{
  BYTE *pClr;
  int i;

  //mark palette so we'll know the original order
  //making sure that the transparent index will shift to 0
  if(!m_palettesize) return;
  for(i=0;i<m_palettesize/4;i++)
  {
    pClr = (BYTE *) (m_palette+i);
    *(pClr+3) = (unsigned char) (i);
  }
  qsort(m_palette, m_palettesize/4, 4, colourorder);
  for(i=0;i<m_palettesize/4;i++)
  {
    pClr=(BYTE *) (m_palette+i);
    if(*(pClr+3)==m_header.chTransparentIndex)
    {
      SwapPalette(m_palette,i,0);
      break;
    }
  }
  ReorderPixels();
  m_header.chTransparentIndex=0;
}

void Cbam::ReorderPixels()
{
  BYTE *pClr;
  int nFrame;
  int i;

  //do the pixel change
  for(nFrame=0;nFrame<m_header.wFrameCount;nFrame++)
  {
    m_pFrameData[nFrame].ReorderPixels(m_palette, m_header.chTransparentIndex,0,
         !(m_pFrames[nFrame].dwFrameDataOffset&0x80000000) );
  }
  //change the palette back to clean
  for(i=0;i<m_palettesize/4;i++)
  {
    pClr = (BYTE *) (m_palette+i);
    *(pClr+3) = 0;
  }
}

void Cbam::ForcePalette(palettetype &newpalette)
{
  int nOrigIndex, nFoundIndex, nForceIndex;
  int nDiff, nMinDiff;

  for(nOrigIndex=0;nOrigIndex<m_palettesize/4;nOrigIndex++)
  {
    if(nOrigIndex==m_header.chTransparentIndex) continue; //don't bother with the transparent index
    nFoundIndex=-1;
    for(nForceIndex=0;nForceIndex<m_palettesize/4;nForceIndex++)
    {
      if(nForceIndex==m_header.chTransparentIndex) continue; //don't bother with the transparent index
      nDiff=ChiSquare((BYTE *) (m_palette+nOrigIndex), (BYTE *) (newpalette+nForceIndex) );
      if( (nFoundIndex<0) || (nDiff<nMinDiff) )
      {
        nMinDiff=nDiff;
        nFoundIndex=nForceIndex;
      }
    }
    // now we have the closest match in nFoundIndex, lets pick it
    m_palette[nOrigIndex]=newpalette[nFoundIndex];
  }
}

void Cbam::DropUnusedPalette()
{
  BYTE *pClr;
  int nFrame;
  int i;

  //mark palette so we'll know which are unused
  for(i=0;i<m_palettesize/4;i++)
  {
    pClr = (BYTE *) (m_palette+i);
    if(i!=m_header.chTransparentIndex) *(pClr+3) = (unsigned char) 1;
  }
  while(i<256) m_palette[i++]=0;
  //do the pixel change
  for(nFrame=0;nFrame<m_header.wFrameCount;nFrame++)
  {
    m_pFrameData[nFrame].MarkPixels(m_palette, m_header.chTransparentIndex,
         !(m_pFrames[nFrame].dwFrameDataOffset&0x80000000) );
  }
  //drop unused entries (turning them black)
  for(i=0;i<m_palettesize/4;i++)
  {
    pClr = (BYTE *) (m_palette+i);
    if(*(pClr+3) ) memset(pClr,0,sizeof(COLORREF));
    else *(pClr+3) = 0;
  }
}

void Cbam::ConvertToGrey(COLORREF Shade, bool keepgray)
{
	int nRed, nGreen, nBlue;
	BYTE chGrey;
	BYTE *pClr;
  int i;
  int tmp, isgray;

	for(i=0;i<m_palettesize/4;i++)
	{
    if(i==m_header.chTransparentIndex) continue; //don't bother with the transparent index
		pClr = (BYTE *) (m_palette+i);

		nBlue = *(pClr);
		nGreen = *(pClr+1);
		nRed = *(pClr+2);

    if(keepgray)
    {
      tmp=(nBlue-nGreen);
      tmp*=tmp;
      isgray=tmp;
      tmp=(nBlue-nRed);
      tmp*=tmp;
      isgray+=tmp;
      if(isgray<GREY_TRESHOLD) continue;
    }
		chGrey = (BYTE)((nRed+nGreen+nBlue)/3);
    nRed = (Shade&255)+chGrey-128;
    if(nRed>255) nRed=255;
    else if(nRed<0) nRed=0;
    nGreen = ((Shade>>8)&255)+chGrey-128;
    if(nGreen>255) nGreen=255;
    else if(nGreen<0) nGreen=0;
    nBlue = ((Shade>>16)&255)+chGrey-128;
    if(nBlue>255) nBlue=255;
    else if(nBlue<0) nBlue=0;

		*(pClr+0) = (unsigned char) nBlue;
		*(pClr+1) = (unsigned char) nGreen;
		*(pClr+2) = (unsigned char) nRed;
	}
}

bool Cbam::FitAndCenterBitmap(HBITMAP &hOriginal, COLORREF clrBackground, int nWidth, int nHeight)
{
	CBitmap *pBmp = CBitmap::FromHandle(hOriginal);
	if (!pBmp)
		return false;

	BITMAP bm;
	pBmp->GetBitmap(&bm);
	
	if (bm.bmWidth >= nWidth && bm.bmHeight >= nHeight)
		return true;

  if(bm.bmWidth>=nWidth)
      nWidth=bm.bmWidth;
  if(bm.bmHeight>=nHeight)
      nHeight=bm.bmHeight;

	CDC *pDC = AfxGetMainWnd()->GetDC();
	if (!pDC)
		return false;

	// DC to use to create the new image.
	CDC dcMem;
	dcMem.CreateCompatibleDC(pDC);

	// Need to have the original bitmap in a dc to manipulate it.
	CDC dcBmp;
	dcBmp.CreateCompatibleDC(pDC);
	CBitmap *pOldBmp = dcBmp.SelectObject(pBmp);

	// Need a new bitmap in the memory dc to copy onto.
	CBitmap bmpNew;
	bmpNew.CreateCompatibleBitmap(pDC,nWidth,nHeight);
	CBitmap *pOldMemBmp = dcMem.SelectObject(&bmpNew);

	// Erase the surface the background color.
	dcMem.FillSolidRect(0,0,nWidth,nHeight,clrBackground);

	// Find the centered coordinates.
	int nLeft = nWidth/2 - bm.bmWidth/2;
	int nTop = nHeight/2 - bm.bmHeight/2;

	// Copy the smaller original bitmap onto the 32x32 in the centered position.
	dcMem.BitBlt(nLeft,nTop,bm.bmWidth,bm.bmHeight,&dcBmp,0,0,SRCCOPY);

	// Select out the bitmaps.
	dcMem.SelectObject(pOldMemBmp);
	dcBmp.SelectObject(pOldBmp);

	AfxGetMainWnd()->ReleaseDC(pDC);

	// Delete the original and point it at the new one.
	pBmp->Detach();
	::DeleteObject(hOriginal);

	hOriginal = (HBITMAP)bmpNew;
	bmpNew.Detach();

	return true;
}
