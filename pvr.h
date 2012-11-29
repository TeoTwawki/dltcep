
#if !defined(AFX_PVR_H__31C58D81_6869_4B93_B788_E7A2CB00D662__INCLUDED_)
#define AFX_PVR_H__31C58D81_6869_4B93_B788_E7A2CB00D662__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef struct {
  unsigned long version;
  unsigned long flags;
  __int64 format;
  unsigned long colorspace;
  unsigned long channeltype;
  unsigned long height;
  unsigned long width;
  unsigned long depth;
  unsigned long numsurface;
  unsigned long numface;
  unsigned long mipmapcount;
  unsigned long metasize;
} pvr_header;

typedef struct {
  unsigned short c0;
  unsigned short c1;
  unsigned long pixels;
} dxt1;

class Cpvr {
public:
  CString m_name;
  pvr_header header;
  dxt1 *bits;

public:
  Cpvr();
  ~Cpvr();
  void WritePvrHeader(int fhandle, unsigned long width, unsigned long height);
  int ReadPvrFromFile(int fhandle, int size);
  int BuildTile(unsigned int x, unsigned int y, COLORREF *dib);
private:
  int Uncompress(int fhandle, unsigned long outsize, unsigned long insize);
};

#endif