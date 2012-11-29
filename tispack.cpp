#include "stdafx.h"

// mods (C) Avenger@TeamBG
/* TisPack, a tileset compression utility
version 0.9, January 14th, 2003

 Copyright (C) 2003 Per Olofsson
 
  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the author be held liable for any damages
  arising from the use of this software.
  
   Permission is granted to anyone to use this software for any purpose,
   including commercial applications, and to alter it and redistribute it
   freely, subject to the following restrictions:
   
    1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would be
    appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.
    
     Per Olofsson (MagerValp@cling.gu.se)
     
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <io.h>
#include <direct.h>
#include <string.h>
#include "zlib.h"
#include "tispack.h"
#include "jpegcompress.h"

unsigned char progname[] = "tispack";

#define DESTBUFSIZE 8192

/* arg defaults */
int silent = 0;
int quality = 75;

//stores data in big endian order (wonder why)
inline void store_data(unsigned char *tmp, unsigned int data)
{
  tmp[0]=(unsigned char) (data>>8);
  tmp[1]=(unsigned char) data;
}

/* Nonlossy compression of src to dest, returns size of dest. */
int zlibcompress(unsigned char *src, int size, unsigned char *dest) {
  unsigned long dlen = DESTBUFSIZE-10; /* a few bytes for headers */
  
  switch (compress2(dest, &dlen, src, size, Z_BEST_COMPRESSION)) {
  case Z_OK:
    return(dlen);
    break;
  case Z_MEM_ERROR:
    return(-3);
    break;
  case Z_BUF_ERROR:
    return(-1);
    break;
  }
  
  return(0); /* -Wall */
}


/* Compress a tile using zlib */
int til0compress(unsigned char *src, unsigned char *dest) {
  int len;
  
  strcpy((char *) dest, "TIL0");
  if ((len = zlibcompress(src, 0x1400, dest + 6)) <= 0) {
    return(-1);
  } else {
    dest[4] = (unsigned char) (len >>8);
    dest[5] = (unsigned char) (len & 0xff);
    return(len + 6);
  }
}

unsigned char makealpha(unsigned char *p) {
  unsigned char a = 0;
  unsigned char mask = 0x80;
  int i;
  
  for (i = 0; i <= 7; ++i) {
    if (*p++) {
      a |= mask;
    }
    mask >>= 1;
  }
  return(a);
}

/* Compress a tile using JPEG and preserve palette and alpha with zlib */
int til1compress(unsigned char *src, unsigned char *dest) {
  unsigned char *bmptr = src + 0x400;
  unsigned char rgbbuf[64*64*3];
  unsigned char *rgbptr = rgbbuf;
  unsigned char header[512+768];
  unsigned char *r = header;
  unsigned char *g = header + 256;
  unsigned char *b = header + 512;
  unsigned char *alpha = header + 768;
  int i, j, y;
  int lendata, lenimg;
  
  if (quality == 100) {
    return(32767);
  }
  
  /* extract palette */
  for (i = 0; i <= 255; ++i) {
    r[i] = src[i*4];
    g[i] = src[i*4+1];
    b[i] = src[i*4+2];
  }
  
  for (y = 0; y <= 63; ++y) {
    for (i = 0; i <= 7; ++i) {
      
      /* generate alpha mask */
      alpha[y*8 + i] = makealpha(bmptr);
      
      /* convert to true colour */
      for (j = 0; j <= 7; ++j) {
        *rgbptr++ = r[*bmptr];
        *rgbptr++ = g[*bmptr];
        *rgbptr++ = b[*bmptr++];
      }
      
    }
  }
  
  strcpy((char *) dest, "TIL1");
  
  if ((lendata = zlibcompress(header, 512+768, dest + 8)) <= 0) {
    return(-1);
  } else {
    store_data(dest+6,lendata);
  }
  
  if ((lenimg = jpegcompress(rgbbuf, dest + lendata + 8)) <= 0) {
    return(-1);
  } else {
    store_data(dest+4, lendata + lenimg + 2);
  }
  
  return(lendata + lenimg + 8);
}

/* Compress a tile using JPEG only */
int til2compress(unsigned char *src, unsigned char *dest) {
  unsigned char *bmptr = src + 0x400;
  unsigned char rgbbuf[64*64*3];
  unsigned char *rgbptr = rgbbuf;
  unsigned char r[256];
  unsigned char g[256];
  unsigned char b[256];
  int i, j, y;
  int lenimg;
  int got0s = 0;
  
  if (quality == 100) {
    return(32767);
  }
  
  /* extract palette */
  for (i = 0; i <= 255; ++i) {
    r[i] = src[i*4];
    g[i] = src[i*4+1];
    b[i] = src[i*4+2];
  }
  
  for (y = 0; y <= 63; ++y) {
    for (i = 0; i <= 7; ++i) {
      
      /* convert to true colour */
      for (j = 0; j <= 7; ++j) {
        if (*bmptr == 0) {
          got0s = 1;
        }
        *rgbptr++ = r[*bmptr];
        *rgbptr++ = g[*bmptr];
        *rgbptr++ = b[*bmptr++];
      }
      
    }
  }
  
  /* don't compress if we find a green alpha mask */
  if (r[0] == 0 && g[0] == 0xff && b[0] == 0 && got0s) {
    return(32767);
  }
  
  strcpy((char *) dest, "TIL2");
  
  if ((lenimg = jpegcompress(rgbbuf, dest + 6)) <= 0) {
    return(-1);
  } else {
    store_data(dest+4,lenimg);
  }
  
  return(lenimg + 6);
}

int writetile(int outfile, unsigned char *buf, int buflen) {
  if (write(outfile, buf, buflen) != buflen) {
    return(-1);
  } else {
    return(buflen);
  }
}

int tispack(int infile, int outfile) {
  int tile = 0;
  int len;
  unsigned char buffer[0x1400];
  int methodlen;
  unsigned char *methodbuf;
  unsigned char destbuf0[DESTBUFSIZE];
  unsigned char destbuf1[DESTBUFSIZE];
  unsigned char destbuf2[DESTBUFSIZE];
  int lentil0, lentil1, lentil2;
  int packedsum = 8;		/* header is 8 bytes */
  int filesize = 0;
  int mctr0 = 0;
  int mctr1 = 0;
  int mctr2 = 0;
  
  while(1) {
    
    /* read tile into the buffer */
    len = read(infile, buffer, 0x1400);
    if (len != 0x1400) {
      if (len == 0) {
        return(0);
      } else {
        return(1);
      }
    }
    
    /* compress the tile with the different methods */
    if ((lentil0 = til0compress(buffer, destbuf0)) <= 0) {
      return(1);
    }
    if ((lentil1 = til1compress(buffer, destbuf1)) <= 0) {
      return(1);
    }
    if ((lentil2 = til2compress(buffer, destbuf2)) <= 0) {
      return(1);
    }
    
    /* select the smallest one */
    if (lentil0 < lentil1 && lentil0 < lentil2) {
      methodlen = lentil0;
      methodbuf = destbuf0;
      ++mctr0;
    } else if (lentil1 < lentil2) {
      methodlen = lentil1;
      methodbuf = destbuf1;
      ++mctr1;
    } else {
      methodlen = lentil2;
      methodbuf = destbuf2;
      ++mctr2;
    }
        
    /* write the tile */
    if (writetile(outfile, methodbuf, methodlen) < 0) {
      return(1);
    }
    
    filesize += 0x1400;
    packedsum += methodlen;
    
    /* next tile */
    ++tile;
  }
  
  return(0); /* -Wall */
}

/* read TIS file header and return number of tiles */
int readtisheader(int infile) {
  unsigned char h[24];
  
  if (read(infile, &h, 24) != 24) {
    return(-1);
  }
  if (strncmp((char *) h, "TIS V1  ", 8) == 0) {
    if (h[20] == 0x40 && h[21] == 0x00 && h[22] == 0x00 && h[23] == 0x00) {
      if (h[12] == 0x00 && h[13] == 0x14 && h[14] == 0x00 && h[15] == 0x00) {
        if (h[10] == 0x00 && h[11] == 0x00) {
          return(h[8] | h[9]<<8);
        }
      }
    }
  }
  return(-1);
}

int writetizheader(int outfile, int numtiles) {
  unsigned char h[8];
  /* 0x54 0x49 0x5a 0x30 */
  
  h[0] = 'T';
  h[1] = 'I';
  h[2] = 'Z';
  h[3] = '0';
  store_data(h+4,numtiles);
  h[6] = 0x00;
  h[7] = 0x00;
  
  if (write(outfile, h, 8) != 8) {
    return(-1);
  } else {
    return(8);
  }
}

int perform_tis2tiz(CString infname, CString outpath)
{
  CString outfname;  
  int size;
  int infile, outfile, status, numtiles;

  /* set default output name */
  outfname=outpath+"\\"+infname.Mid(infname.ReverseFind('\\')+1);
  outfname.MakeLower();
  outfname.Replace(".tis",".tiz");
  
  if ((infile = open(infname,O_BINARY| O_RDONLY)) == -1) {
    return -2;
  }
  
  size = filelength(infile);
  if (size == 0) {
    close(infile);
    return -1;
  }
  
  switch (size % 0x1400) {
  case 0:
    numtiles = size / 0x1400;
    break;
  case 24:
    if ((numtiles = readtisheader(infile)) == -1) {
      close(infile);
      return -1;
    }
    if (numtiles != (size - 24) / 0x1400) {
      close(infile);
      return -1;
    }
    break;
  default:
    close(infile);
    return -1;
    break;
  }
     
  if ((outfile = open(outfname, O_BINARY|O_WRONLY|O_CREAT|O_TRUNC, S_IWRITE|S_IREAD)) == -1)
  {
    close(infile);
    return -2;
  }
  
  if (writetizheader(outfile, numtiles) < 0) {
    close(outfile);
    close(infile);
    return(1);
  }
  
  status = tispack(infile, outfile);
  close(outfile);
  close(infile);
  if (status) {
    unlink(outfname);
  }
  
  return(status);
}

int pvrunpack(int infile, int outfile, unsigned long insize, unsigned long outsize)
{
  unsigned char *indata;
  unsigned char *outdata;

  indata = (unsigned char *) malloc(insize);
  if (!indata) return -1;
  outdata = (unsigned char *) calloc(outsize, 1);
  if (!outdata) {
    free(indata);
    return -1;
  }

  if (read(infile, indata, insize)!=(int) insize)
  {
    free(indata);
    free(outdata);
    return -1;
  }

  int err = uncompress(outdata, &outsize, indata, insize);
  if (write(outfile, outdata, outsize)!=(int) outsize)
  {
    free(indata);
    free(outdata);
    return -1;
  }
  free(indata);
  free(outdata);
  return err!=Z_OK;
}

int pvrpack(int infile, int outfile, unsigned long insize)
{
  unsigned char *indata;
  unsigned char *outdata;
  unsigned long outsize = insize; //we don't care about safety, these files should always compress
  
  indata = (unsigned char *) malloc(insize);
  if (!indata) return -1;
  outdata = (unsigned char *) calloc(outsize, 1);
  if (!outdata) return -1;

  if (read(infile, indata, insize)!=(int) insize)
  {
    free(indata);
    free(outdata);
    return -1;
  }
  int err = compress2(outdata, &outsize, indata, insize, Z_BEST_COMPRESSION);
  //int err = zlibcompress(indata, insize, outdata);
  if (write(outfile, &insize, sizeof(insize) )!=sizeof(insize) )
  {
    free(indata);
    free(outdata);
    return -1;
  }
  if (write(outfile, outdata, outsize)!=(int) outsize)
  {
    free(indata);
    free(outdata);
    return -1;
  }

  free(indata);
  free(outdata);
  return err!=Z_OK;
}

int perform_pvrz2pvr(CString infname, CString outpath)
{
  CString outfname;  
  unsigned long insize, outsize;
  int infile, outfile, status;

  /* set default output name */
  outfname=outpath+"\\"+infname.Mid(infname.ReverseFind('\\')+1);
  outfname.MakeLower();
  outfname.Replace(".pvrz",".pvr");
  
  if ((infile = open(infname,O_BINARY| O_RDONLY)) == -1) {
    return -2;
  }
  
  insize = filelength(infile);
  if (insize < 8)
  {
    close(infile);
    return -1;
  }
  
  if (read(infile, &outsize, 4)!=4)
  {
    close(infile);
    return -1;
  }
  insize-=4;

  if ((outfile = open(outfname, O_BINARY|O_WRONLY|O_CREAT|O_TRUNC, S_IWRITE|S_IREAD)) == -1)
  {
    close(infile);
    return -2;
  }
   
  status = pvrunpack(infile, outfile, insize, outsize);
  close(outfile);
  close(infile);
  if (status) {
    unlink(outfname);
  }
  
  return(status);
}

int perform_pvr2pvrz(CString infname, CString outpath)
{
  CString outfname;  
  int insize;
  int infile, outfile, status;

  /* set default output name */
  outfname=outpath+"\\"+infname.Mid(infname.ReverseFind('\\')+1);
  outfname.MakeLower();
  outfname.Replace(".pvr",".pvrz");
  
  if ((infile = open(infname,O_BINARY| O_RDONLY)) == -1) {
    return -2;
  }
       
  insize = filelength(infile);
  //PVR files should have a header of this size
  if (insize < 0x34)
  {
    close(infile);
    return -1;
  }

  if ((outfile = open(outfname, O_BINARY|O_WRONLY|O_CREAT|O_TRUNC, S_IWRITE|S_IREAD)) == -1)
  {
    close(infile);
    return -2;
  } 
  
  status = pvrpack(infile, outfile, insize);
  close(outfile);
  close(infile);
  if (status) {
    unlink(outfname);
  }
  
  return(status);
}
