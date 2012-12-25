// bam.h: interface for the Cbam class.
// Code derived from ShadowKeeper (c) 2000 Aaron O'Neil
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

#if !defined(AFX_BAM_H__9CAD5CDC_9FBD_4CDB_A1D1_16DF19E95E29__INCLUDED_)
#define AFX_BAM_H__9CAD5CDC_9FBD_4CDB_A1D1_16DF19E95E29__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "mos.h"

#define EBD_ALL   0
#define EBD_HEAD  1
#define EBD_FIRST 2

#define MAX_ICON_WIDTH		640
#define MAX_ICON_HEIGHT		480

//MakeBitmap options (1 - resize bitmap,   2 - don't delete previous content)
#define BM_JUSTIFY_MASK  3
#define BM_LEFT     0
#define BM_RIGHT    1
#define BM_CENTER   2
#define BM_EDIT     3

#define BM_NORMAL   0
#define BM_RESIZE   0x10
#define BM_OVERLAY  0x20
#define BM_MATCH    0x40
#define BM_INVERT   0x80
#define BM_ZOOM     0x100

struct INF_BAMC_HEADER
{
	char		      chSignature[4];
	char		      chVersion[4];
  unsigned long nExpandSize;
};

struct plt_header
{
	char		chSignature[4];
	char		chVersion[4];
  WORD    wUnknowns[4]; //8 bytes
  DWORD   dwWidth;
  DWORD   dwHeight;
};

struct INF_BAM_HEADER
{
	char		chSignature[4];
	char		chVersion[4];
	WORD		wFrameCount;
	BYTE		chCycleCount;
	BYTE		chTransparentIndex;
	DWORD		dwFrameOffset;
	DWORD		dwPaletteOffset;
	DWORD		dwFrameLookupTableOffset;
};

struct INF_BAM_CYCLE
{
	WORD		wFrameIndexCount;
	WORD		wFirstFrameIndex;			// Index into the frame lookup table.
};

struct INF_BAM_FRAME
{
	WORD		wWidth;
	WORD		wHeight;
	short		wCenterX;
	short		wCenterY;
	DWORD		dwFrameDataOffset;		// bit 31 is a flag for RLE. 0 = RLE
};

class INF_BAM_FRAMEDATA
{
public:
  BYTE *pFrameData;
  unsigned int nFrameSize;

  INF_BAM_FRAMEDATA()
  {
    pFrameData=NULL;
    nFrameSize=0;
  }

  ~INF_BAM_FRAMEDATA()
  {
    if(pFrameData)
    {
      delete [] pFrameData;
      pFrameData=NULL;
      nFrameSize=0;
    }
  }

  void FlipFrame(int nWidth, int nHeight);
  int RLECompression(BYTE chTransparentIndex);
  int RLEDecompression(BYTE chTransparentIndex);
  void ReorderPixels(COLORREF *palette, BYTE chTransparentIndex, 
      BYTE chNewTransparentIndex, bool bIsCompressed);
  void MarkPixels(COLORREF *palette, BYTE chTransparentIndex, bool bIsCompressed);
	int INF_BAM_FRAMEDATA::TakePartialBamData(const LPBYTE pRawBits, unsigned int nWidth,
			unsigned int nHeight, unsigned int nDataWidth, unsigned int nDataSkip,
			unsigned int nRowSkip, unsigned int nMaxLength);
	int INF_BAM_FRAMEDATA::TakeBamDataPartial(const LPBYTE pRawBits, unsigned int nWidth,
			unsigned int nHeight, unsigned int nDataWidth, unsigned int nDataSkip,
			unsigned int nRowSkip, unsigned int nMaxLength);
  int TakeBamData(const LPBYTE pRawBits, unsigned int nWidth, unsigned int nHeight,
       BYTE chTransparentIndex, bool bIsCompressed, unsigned int nMaxLength);
  int TakePltData(const LPWORD pRawBits, plt_header &sHeader, int row);
  int TakeBmpData(const LPBYTE pRawBits, bmp_header &sHeader, int row);
  int TakeBmp4Data(const LPBYTE pRawBits, bmp_header &sHeader, int row);
  int ExpandBamBits(BYTE chTransparentIndex, COLORREF clrTransparent, COLORREF *pDIBits,
      bool bIsCompressed, int nColumn, COLORREF *pPal, int nHeight, int nWidth);
};

class Cbam  
{
public:
	Cbam();
	virtual ~Cbam();
  void new_bam();
  int ExplodeBamData(int flags);
  int ImplodeBamData();
  bool CanCompress();
  int WriteBmpToFile(int fhandle, int frame);
  int WritePltToFile(int fhandle, int frame);
  int WriteBamToFile(int fhandle);
  int ReadBmpFromFile(int fhandle, int ml);
  int ReadPltFromFile(int fhandle, int ml);
  int ReadBamFromFile(int fhandle, int maxlen, bool onlyheader);
  int ReadBamPreviewFromFile(int fhandle, int maxlen);
  int GetTextExtent(CString text); //returns the string width in pixels using this font bam
  void DrawText(CString text, COLORREF clrTrans, Cmos &host,
    int nMode, int nXpos, int nYPos, int nWidth);
  int MakeBitmap(int nFrameWanted, COLORREF clrTrans, Cmos &host,
    int nMode, int nXpos, int nYpos);
  int MakeBitmap(int nFrameWanted, COLORREF clrTrans, HBITMAP &hBitmap,
	  int nMode, int nWidth, int nHeight); //creates a bitmap returns it in hbitmap
  int GetFrameCount();
  int GetCycleCount();
  int DetachFrameData(int nFrameWanted);
  CPoint GetCompositeSize();
  CPoint GetFrameSize(int nFrameWanted);
  void GetEmpty(INF_BAM_FRAMEDATA *fdata);
	void GetQuarterSplit(INF_BAM_FRAMEDATA *fdata, int nFrameWanted, 
    int skipwidth, int skipheight, int splitwidth, int splitheight);
	void GetSplitQuarter(INF_BAM_FRAMEDATA *fdata, int nFrameWanted, 
    int skipwidth, int skipheight, int splitwidth, int splitheight);
  LPBYTE GetFrameData(int nFrameWanted);
  int SetFrameData(int nFrameWanted, LPBYTE pFrameData, const CPoint &cpFrameSize);
  CPoint GetFramePos(int nFrameWanted);
  int GetFrameDataSize(int nFrameWanted);
  void ForcePalette(palettetype &palette);
  void OrderPalette();
  void ReorderPixels();
  void DropUnusedPalette();
  bool GetFrameRLE(int nFrameWanted);
  bool SetFrameRLE(int nFrameWanted, BOOL NewRLE);
  CPoint GetCycleData(int nCycle);
  int SetCycleData(int nCycle, int x, int y);
  int DropFrame(int nFrameWanted);
  int AddFrame(int nFrameWanted, int nNewFrameSize);//inserts a new frame into the bam (doesn't take data)
  //returns quality loss
  //palettetype &oldpalette,const LPBYTE pRawData, int nFrameSize);
  int ImportFrameData(int nFrameIndex, Cbam &tmpbam, int nImportFrameIndex = 0);
  int AddFrameToCycle(int nCycle, int nCyclePos, int nFrameWanted, int nRepeat); //cycle, cyclepos, framewanted, repeat
  int RemoveFrameFromCycle(int nCycle, int nCyclePos, int nRepeat);
  int CheckFrameSizes();
  int DropUnusedFrame(int bDropIt);
  int InsertCycle(int nCycleWanted);
  int ReplaceCycle(int nCycleWanted, short *pNewLookup, int nLen);
  int DropCycle(int nCycleWanted);
  int FindFrame(int nFrameWanted, int &nIndex); //finds the first cycle containing this frame
  int GetFrameIndex(int nCycle, int nIndex);
  int SetFrameIndex(int nCycle, int nIndex, int nFrameIndex);
  int SetFrameSize(int nFrameWanted, int x, int y);
  int AllocateFrameSize(int nFrameWanted, int x, int y);
  int SetFramePos(int nFrameWanted, int x, int y);
   //use RGB(128,128,128) for a greyscale
  void ConvertToGrey(COLORREF shade, bool keepgray=false);
  int FlipFrame(int nFrameWanted);
  int SwapFrames(int a, int b); //swaps frames in lookup table
  int Reallocate(int x, int y);
  bool ShrinkFrame(int nFrameWanted);
  void CreateAlpha();
  bool MergeStructure(Cbam &original, int width, int height);
	bool CopyStructure(Cbam &original, int skipwidth, int skipheight, int width, int height);

  inline int GetTransparentIndex()
  {
    return m_header.chTransparentIndex;
  }

  inline void SetCompress(bool flg)
  {
    m_bCompressed=flg;
  }

  inline BOOL IsCompressed()
  {
    return m_bCompressed;
  }
  inline int GetSaveOrder()
  {
    return m_nSaveOrder;
  }
  inline void SetSaveOrder(int so)
  {
    m_nSaveOrder=so;
  }
  inline BOOL HasPalette()
  {
    return !!m_palettesize;
  }
  inline void InitPalette()
  {
    m_palettesize=256*sizeof(COLORREF);
    memset(&m_palette,0,sizeof(m_palette) );
  }

  inline int GetDataSize()
  {
    return m_nDataSize;
  }

  inline void KillDIBits()
  {
    if(m_pclrDIBits)
    {
      delete [] m_pclrDIBits;
      m_pclrDIBits=NULL;
    }
  }

  inline void KillData()
  {
    if(m_pData)
    {
      delete [] m_pData;
      m_pData=NULL;
    }
  } 
  inline void KillFrameData()
  {
    if(m_pFrameData)
    {
      delete [] m_pFrameData;
      m_pFrameData=NULL;
    }
    if(m_pFrames)
    {
      delete [] m_pFrames;
      m_pFrames=NULL;
    }
    if(m_pCycles)
    {
      delete [] m_pCycles;
      m_pCycles=NULL;
    }
    if(m_pFrameLookup)
    {
      delete [] m_pFrameLookup;
      m_pFrameLookup=NULL;
    }
    m_nCycles=0;
    m_nFrames=0;
    m_nFrameLookupSize=0;
  }

  CString m_name;
  //these are public for easier access
  palettetype m_palette;
  int m_palettesize;
  INF_BAM_HEADER m_header;
  plt_header m_pltheader;
  short *m_pFrameLookup;     //frame lookup tables (made them global for convenience)

  BOOL m_bCompressed; //save as (leave it public for simplicity)
  //palette : 0,  frame/cycle : 1, framelookup: 2, frame data: 3
  //palette, frame/cycle, framelookup, frame data  <00011011>  
  //palette, frame/cycle, frame data, framelookup  <00011110>
  //palette, framelookup, frame/cycle, frame data  <00100111>
  //palette, framelookup, frame data, frame/cycle  <00101101>
  //palette, frame data, frame/cycle, framelookup  <00110110>
  //palette, frame data, framelookup, frame/cycle  <00111001> etc. etc
private:
  int m_nSaveOrder;   //you can change this to optimise performance
  INF_BAM_FRAME *m_pFrames;  //frames
  INF_BAM_FRAMEDATA *m_pFrameData;       //pointer array for individual frames
  int m_nFrames;
  int m_nMinFrameOffset;       //minimal frame offset
  INF_BAM_CYCLE *m_pCycles;  //cycles
  int m_nCycles;
  int m_nFrameLookupSize;      //size of the frame lookup table (counter)
  INF_BAMC_HEADER c_header;

  BYTE *m_pData;               //uncompressed/raw bam data
  DWORD m_nDataSize;           //uncompressed/raw bam data size (sizeof m_pData)
  COLORREF *m_pclrDIBits;
  DWORD m_nBitSize;            //bitmap size
	
  int ConsolidateFrameLookup();
  bool ExpandBamBits(const LPBYTE pRawBits,int nWidth, int nHeight,
       BYTE chTransparentIndex, COLORREF clrTransparent, const COLORREF *pPal,
       COLORREF *pDIBits, bool bIsCompressed, int nMaxLength, int nColumn);
  bool FitAndCenterBitmap(HBITMAP &hOriginal, COLORREF clrBackground,
	     int nWidth, int nHeight, int zoom);
  int ReducePalette(int fhandle, bmp_header sHeader, int scanline);
};

#endif // !defined(AFX_BAM_H__9CAD5CDC_9FBD_4CDB_A1D1_16DF19E95E29__INCLUDED_)
