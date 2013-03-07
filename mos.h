// mos.h: interface for the Cmos class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOS_H__143AC7EF_EC59_436A_8AA9_E17B960DBE01__INCLUDED_)
#define AFX_MOS_H__143AC7EF_EC59_436A_8AA9_E17B960DBE01__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "structs.h"

class INF_MOS_FRAMEDATA
{
public:
  palettetype Palette;
  BYTE *pFrameData;
  int nFrameSize;
  DWORD nWidth, nHeight;

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
  void AlterPalette(int r, int g, int b, int strength);
  void ReorderPixels();
  void MarkPixels();
  void MovePixels(BYTE chFrom, BYTE chTo);
  bool IsTransparent(DWORD x, DWORD y);
  int TakeMosData(const BYTE *pRawBits, DWORD width, DWORD height, bool deepen=false);
  int ReducePalette(int treshold, int bestcolor, COLORREF newcolor, int index);
  int FindBestColor(COLORREF mycolor, int &difference);
  int FindColor(COLORREF mycolor, int palettesize);
  DWORD GetColor(int x, int y);
  int TakeWholeBmpData(const COLORREF *pRawBits);
  int TakePaletteData(const BYTE *pRawBits);
  int TakeBmpData(const BYTE *pRawBits, int row, int offset);
  bool ExpandMosBitsWhole(COLORREF clrReplace, COLORREF clrTransparent, COLORREF *pDIBits, int nOffset, int wWidth);
  int ExpandMosLine(char *pBuffer, int nSourceOff);
  int SaturateTransparency();
  void AllocateTransparency();
  void FlipTile();
};

typedef CMap<COLORREF, COLORREF&, int, int&> CPaletteMap;
typedef CList<INF_MOS_FRAMEDATA *, INF_MOS_FRAMEDATA *> CFrameList;

class Cmos  
{
public:
	Cmos();
	virtual ~Cmos();

  wed_tilemap *m_tileheaders;
  short *m_tileindices;
  CString m_name;
  int m_nQualityLoss;
  INF_MOS_HEADER mosheader;
  INF_MOS2_HEADER mosheader2;
  tis_header tisheader;
  BOOL m_bCompressed;
  BOOL m_pvr;
  DWORD *m_pOffsets;
  int m_overlay;
  int m_overlaytile;
  Cmos *m_friend[10];
  bool m_changed;
  bool m_drawclosed;
  int m_pixelwidth, m_pixelheight;

  INF_MOS_FRAMEDATA *GetFrameData(DWORD nFrameWanted);
  bool MosChanged() { return m_changed; }
  void FreeOverlay();
  void SetTiles(wed_tilemap *tileheaders, short *tileindices);
  void SetOverlay(int overlay, Cmos *overlaymos);
  int ResolveFrameNumber(int framenumber);
  void new_mos();
  void DropUnusedPalette();
  void ApplyPaletteRGBTile(int nFrameWanted, int r, int g, int b, int strength);
  void ApplyPaletteRGB(int r, int g, int b, int strength);
  int RetrieveTisFrameCount(int fhandle, loc_entry *fl);
  int GetFrameCount();
  CPoint GetFrameSize(DWORD nFrame);
  unsigned char *GetFrameBuffer(DWORD nFrame);
  COLORREF GetPixelData(int x, int y);
  int TakeOneFrame(DWORD nFrameWanted, COLORREF *prFrameBuffer, int factor);
  unsigned long *GetFramePalette(DWORD nFrame);
  bool MakeBitmap(COLORREF clrTrans, Cmos &host, int nMode, int nXpos, int nYpos);
  bool MakeBitmapWhole(COLORREF clrTrans, HBITMAP &hBitmap, int clipx, int clipy, int maxclipx, int maxclipy, bool norender);
  int MakeTransparent(DWORD nFrame, DWORD redgreenblue, int limit);
  int ReadBmpFromFile(int fhandle, int ml);
  int WriteMosToBmpFile(int fhandle, int clipx=0, int clipy=0,int maxclipx=-1, int maxclipy=-1);
  int WriteTisToBmpFile(int fhandle, int clipx=0, int clipy=0,int maxclipx=-1, int maxclipy=-1);
  int WriteMosToFile(int fhandle);
  //int WritePvrToFile(int fhandle, int clipx=0, int clipy=0, int maxclipx=-1, int maxclipy=-1);
  int WriteTisToFile(int fhandle, int clipx=0, int clipy=0, int maxclipx=-1, int maxclipy=-1);
  int ReadMosFromFile(int fhandle, int maxlen);
  int ReadTisFromFile(int fhandle, loc_entry *fl, bool header, bool preview);
  void SetTisParameters(loc_entry *fl, int size);
  int TisToMos(DWORD x, DWORD y);
  int Allocate(DWORD x, DWORD y);
  int GetImageWidth(int clipx, int maxclipx);
  int GetImageHeight(int clipy, int maxclipy);
  DWORD GetColor(DWORD x, DWORD y);
  int FlipTile(DWORD nFrameWanted);
  int RemoveTile(DWORD tile);
  //flags = 1 deepen, 2 = empty tile
  int AddTileCopy(DWORD original, unsigned char *optionalpixels = NULL, int flags=0);
  int SetFrameData(DWORD nFrameWanted, INF_MOS_FRAMEDATA *oldframe);
  int SaturateTransparency(DWORD tile, bool createcopy = false, bool deepen=false);

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
    POSITION pos=m_pFrameData.GetHeadPosition();
    while(pos)
    {
      INF_MOS_FRAMEDATA *p=m_pFrameData.GetNext(pos);
      if(p) delete p;
    }
    m_pFrameData.RemoveAll();
    if(m_pFrameDataPointer)
    {
      free(m_pFrameDataPointer);
      m_pFrameDataPointer=NULL;
    }
    tisheader.numtiles=0;
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
  inline int GetDIBsize() { return m_DIBsize; }

private:
	COLORREF *m_pclrDIBits;  
  INF_MOSC_HEADER c_header;
  BYTE *m_pData;               //uncompressed/raw mos data
  DWORD m_nDataSize;           //uncompressed/raw mos data size (sizeof m_pData)
  DWORD fullsize;
  int m_nResX, m_nResY;
  INF_MOS_FRAMEDATA **m_pFrameDataPointer;  //pointer array for individual frames
  CFrameList m_pFrameData;
  int m_DIBsize;

  bool CreateFrames();
  int ExplodeMosData(int fhandle);
  int ImplodeMosData();
  int ReducePalette(int fhandle, bmp_header &sHeader, LPBYTE pcBuffer,
    DWORD scanline, DWORD nSourceOff);
  int CollectFrameData(int fhandle, DWORD *prFrameBuffer, LPBYTE pcBuffer,
    DWORD height, DWORD scanline, DWORD nFrameCol,
    DWORD cols, DWORD rows);
  int GetFrameData(DWORD *prFrameBuffer, LPBYTE pRawData, DWORD height,
    DWORD scanline, DWORD nFrameCol, DWORD cols, DWORD rows);
};

#endif // !defined(AFX_MOS_H__143AC7EF_EC59_436A_8AA9_E17B960DBE01__INCLUDED_)
