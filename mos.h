// mos.h: interface for the Cmos class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOS_H__143AC7EF_EC59_436A_8AA9_E17B960DBE01__INCLUDED_)
#define AFX_MOS_H__143AC7EF_EC59_436A_8AA9_E17B960DBE01__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct INF_MOS_HEADER
{
	char		chSignature[4]; //MOS
	char		chVersion[4];
	WORD		wWidth;
	WORD		wHeight;
	WORD		wColumn;
	WORD		wRow;
	DWORD		dwBlockSize;
	DWORD		dwPaletteOffset;
};

struct INF_MOSC_HEADER
{
	char		chSignature[4]; //MOSC
	char		chVersion[4];
  DWORD   nExpandSize;
};

class INF_MOS_FRAMEDATA
{
public:
  palettetype Palette;
  BYTE *pFrameData;
  int nFrameSize;
  int nWidth, nHeight;
  int nKilla, nKillb, nTreshold;

  INF_MOS_FRAMEDATA()
  {
    pFrameData=NULL;
    nFrameSize=0;
    memset(&Palette,0,sizeof(Palette) );
  }

  ~INF_MOS_FRAMEDATA()
  {
    if(pFrameData)
    {
      delete [] pFrameData;
      pFrameData=NULL;
      nFrameSize=0;
    }
  }

  void OrderPalette();
  void ReorderPixels();
  void MarkPixels();
  void MovePixels(BYTE chFrom, BYTE chTo);
  int TakeMosData(const BYTE *pRawBits, int width, int height);
  int ReducePalette(int treshold, int bestcolor, COLORREF newcolor, int index);
  int FindBestColor(COLORREF mycolor, int &difference);
  int FindColor(COLORREF mycolor, int palettesize);
  DWORD GetColor(int x, int y);
  int TakeWholeBmpData(const COLORREF *pRawBits);
  int TakePaletteData(const BYTE *pRawBits);
  int TakeBmpData(const BYTE *pRawBits, int row, int offset);
  bool ExpandMosBits(COLORREF clrReplace, COLORREF clrTransparent, COLORREF *pDIBits, HBITMAP &hBitmap);
  bool ExpandMosBitsWhole(COLORREF clrReplace, COLORREF clrTransparent, COLORREF *pDIBits, int nOffset, int wWidth);
  int ExpandMosLine(char *pBuffer, int nSourceOff);
};

typedef CMap<COLORREF, COLORREF&, int, int&> CPaletteMap;

class Cmos  
{
public:
	Cmos();
	virtual ~Cmos();

  CString m_name;
  int m_nQualityLoss;
  int m_nFrameNumber;
  INF_MOS_HEADER mosheader;
  tis_header tisheader;
  BOOL m_bCompressed;
  DWORD *m_pOffsets;

  void new_mos();
  void DropUnusedPalette();
  int RetrieveTisFrameCount(int fhandle, int maxlen);
  int GetFrameCount();
  CPoint GetFrameSize(int nFrame);
  COLORREF GetPixelData(int x, int y);
  int TakeOneFrame(int nFrameWanted, COLORREF *prFrameBuffer, int factor);
  unsigned long *GetFramePalette(int nFrame);
  int MakeBitmapInternal(HBITMAP &hBitmap);
  bool MakeBitmap(COLORREF clrTrans, Cmos &host, int nMode, int nXpos, int nYpos);
  bool MakeBitmap(int nFrameWanted, COLORREF clrTrans, HBITMAP &hBitmap);
  bool MakeBitmapWhole(COLORREF clrTrans, HBITMAP &hBitmap, int clipx, int clipy, int maxclipx, int maxclipy);
  int MakeTransparent(int nFrame, DWORD redgreenblue, int limit);
  int ReadBmpFromFile(int fhandle, int ml, int tis_or_mos);
  int WriteMosToBmpFile(int fhandle, int clipx=0, int clipy=0,int maxclipx=-1, int maxclipy=-1);
  int WriteTisToBmpFile(int fhandle, int clipx=0, int clipy=0,int maxclipx=-1, int maxclipy=-1);
  int WriteMosToFile(int fhandle);
  int WriteTisToFile(int fhandle, int clipx=0, int clipy=0, int maxclipx=-1, int maxclipy=-1);
  int ReadMosFromFile(int fhandle, int maxlen);
  int ReadTisFromFile(int fhandle, int maxlen, int header);
  void SetTisParameters(int size);
  int TisToMos(int x, int y);
  int Allocate(int x, int y);
  int GetImageWidth(int clipx, int maxclipx);
  int GetImageHeight(int clipy, int maxclipy);
  DWORD GetColor(int x, int y);

  inline int GetDataSize()
  {
    return m_nDataSize;
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
  }
  inline void KillOffsets()
  {
    if(m_pOffsets)
    {
      delete [] m_pOffsets;
      m_pOffsets=NULL;
    }
  }
  inline COLORREF *GetDIB() { return m_pclrDIBits; }

private:
	COLORREF *m_pclrDIBits;  
  INF_MOSC_HEADER c_header;
  BYTE *m_pData;               //uncompressed/raw mos data
  DWORD m_nDataSize;           //uncompressed/raw mos data size (sizeof m_pData)
  DWORD fullsize;
  int m_nResX, m_nResY;
  INF_MOS_FRAMEDATA *m_pFrameData;       //pointer array for individual frames
  int m_DIBsize;

  int ExplodeMosData();
  int ImplodeMosData();
  int CreateBmpHeader(int fhandle, int width, int height, int bytes);
  int ReducePalette(int fhandle, bmp_header &sHeader, LPBYTE pcBuffer,
    int scanline, int nSourceOff);
  int CollectFrameData(int fhandle, DWORD *prFrameBuffer, LPBYTE pcBuffer,
    int height, int scanline, int nFrameCol, int cols, int rows);
  int GetFrameData(DWORD *prFrameBuffer, LPBYTE pRawData, int height,
    int scanline, int nFrameCol, int cols, int rows);
};

#endif // !defined(AFX_MOS_H__143AC7EF_EC59_436A_8AA9_E17B960DBE01__INCLUDED_)
