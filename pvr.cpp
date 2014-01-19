// pvr.cpp: implementation of the Cpvr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "chitem.h"
#include "tispack.h"
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>

void Cpvr::WritePvrHeader(int fhandle, unsigned long width, unsigned long height)
{
  memset(&header,0,sizeof(header));

  header.version = '\3RVP';
  header.flags = 0;
  header.format = 7;
  header.colorspace = 0;
  header.channeltype = 0;
  header.height = height;
  header.width = width;
  header.depth = 1;
  header.numsurface = 1;
  header.numface = 1;
  header.mipmapcount = 1;
  header.metasize = 0;
  write(fhandle, &header, sizeof(pvr_header) );
}

Cpvr::Cpvr()
{
  bits = NULL;
}

Cpvr::~Cpvr()
{
  if (bits)
  {
    free(bits);
  }
}

int Cpvr::ReadPvrFromFile(int fhandle, int size)
{
  int ret;
  unsigned long origsize;
  int myhandle = fhandle;

  if (size==-1)
  {
    size = filelength(fhandle);
  }
  if (read(fhandle, &origsize, 4)!=4) return -1;
  if (origsize!='\3RVP')
  {
    myhandle = Uncompress(fhandle, origsize, size-4);
    ret = read(myhandle, &header, sizeof(pvr_header) )!=sizeof(pvr_header);
    lseek(myhandle,0,SEEK_SET);
    if (read(myhandle, &origsize, 4)!=4) return -1;
    if (origsize!='\3RVP') return -1;
  }
  ret = read(myhandle, &header.flags, sizeof(pvr_header)-4)!=(sizeof(pvr_header)-4);
  if (ret)
  {
    return -1;
  }
  //restricted fileformat support (only dxt1)
  if (header.format!=7) return -2;
  if (header.colorspace!=0) return -2;
  if (header.channeltype!=0) return -2;
  if (header.depth!=1) return -2;
  if (header.numsurface!=1) return -2;
  if (header.numface!=1) return -2;
  if (header.mipmapcount!=1) return -2;
  lseek(fhandle, header.metasize, SEEK_CUR);
  int pixelsize = header.width*header.height/2;
  bits = (dxt1 *) calloc(pixelsize/sizeof(dxt1), sizeof(dxt1));
  if (read(myhandle, bits, pixelsize)!=pixelsize)
  {
    ret=-2;
  }
  if (myhandle!=fhandle)
  {
    close(myhandle);
  }
  return ret;
}

int Cpvr::Uncompress(int fhandle, unsigned long outsize, unsigned long insize)
{
  int outfile;

  CString path = bgfolder+"/override/"+m_name;
  path.Replace(".pvrz",".pvr");
  outfile = open(path, O_BINARY|O_RDWR|O_CREAT|O_TRUNC, S_IREAD|S_IWRITE);
  pvrunpack(fhandle, outfile, insize, outsize);

  lseek(outfile, 0, SEEK_SET);
  return outfile;
}

inline COLORREF average(COLORREF a, COLORREF b)
{  
  int rc = ( ((a&0xff0000)>>16) + ((b&0xff0000)>>16) )/2;
  int gc = ( ((a&0xff00)>>8) + ((b&0xff00)>>8) )/2;
  int bc = ( (a&0xff) + (b&0xff) )/2;
  return (rc<<16)|(gc<<8)|bc;
}

inline COLORREF third(COLORREF a, COLORREF b)
{
  int rc = ( ((a&0xff0000)>>15) + ((b&0xff0000)>>16) )/3;
  int gc = ( ((a&0xff00)>>7) + ((b&0xff00)>>8) )/3;
  int bc = ( ((a&0xff)<<1) + (b&0xff) )/3;
  return (rc<<16)|(gc<<8)|bc;
}

inline COLORREF unpack(unsigned short rgb565)
{
  return ((rgb565<<3)&0xf8)|((rgb565<<5)&0xfc00)|((rgb565<<8)&0xf80000);
}

//1 dxt1 block contains original 4x4 pixels packed in 
//2 rgb565 color values and 16 2 bit indices stored on a dword
//1 tile is 16 dxt1 blocks wide and 16 dxt1 blocks tall (256 dxt1)
//as the dib tile contains 64x64 rgba pixels (4096)
int Cpvr::BuildTile(unsigned int x, unsigned int y, COLORREF *dib)
{
  //this should't happen
  if (x+64>header.width || y+64>header.height) return 1;
  dxt1 *pdxt1 = bits+y/4*header.width/4+x/4;
  COLORREF c[4];

  for(int tmpy=0;tmpy<16;tmpy++)
  {
    for(int tmpx=0;tmpx<16;tmpx++)
    {
      c[0] = unpack(pdxt1->c0);
      c[1] = unpack(pdxt1->c1);

      if (c[0]>c[1])
      {
        c[2] = third(c[0],c[1]);
        c[3] = third(c[1],c[0]);
      }
      else
      {
        c[2] = average(c[0],c[1]);
        c[3] = TRANSPARENT_GREEN;
      }

      unsigned long idx = pdxt1->pixels; //get the dxt1 index value
      for(int i=0;i<4;i++)
      {
        for (int j=0;j<4;j++)
        {
          *dib = c[idx&3];
          dib++;
          idx>>=2;
        }
        dib+=60; //64-4 (4 was already added by the inner loop)
      }
      dib-=252; //3*64+60 (go back to the top for the next tile)
      pdxt1++;  //next dxt1 block
    }
    dib+=3*64;  //go down a full stripe length
    pdxt1+=(header.width-64)/4; //same for the original dxt1 blob
  }
  return 0;
}
