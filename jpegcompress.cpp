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
#include <memory.h>
extern "C"
{
 #include "jpeglib.h"
}
#include "tispack.h"

static unsigned char *destbuf;
static unsigned char *buf;
static int bufsize;
static struct jpeg_destination_mgr jpg_dest;

void jpg_init_dest(j_compress_ptr cinfo) {
  //printf("jpg_init_dest called\n");
  buf = (unsigned char *) malloc(DESTBUFSIZE*10);
  bufsize = 0;
  cinfo->dest->next_output_byte = buf;
  cinfo->dest->free_in_buffer = DESTBUFSIZE;
  //printf("destbuffer is now %d bytes\n", bufsize);
}


boolean jpg_empty_destbuf(j_compress_ptr cinfo) {
  int size;

  size = DESTBUFSIZE - cinfo->dest->free_in_buffer;
  //printf("jpg_empty_destbuf called, with %d bytes in the buffer\n", size);
  memcpy(destbuf + bufsize, buf, size);
  bufsize += size;
  //printf("destbuffer is now %d bytes\n", bufsize);
  cinfo->dest->next_output_byte = buf;
  cinfo->dest->free_in_buffer = DESTBUFSIZE;
  //printf("buffer copied and pointer reset\n");
  return(TRUE);
}


void jpg_term_dest(j_compress_ptr cinfo) {
  int size;

  size = DESTBUFSIZE - cinfo->dest->free_in_buffer;
  //printf("jpg_term_dest called, with %d bytes in the buffer\n", size);
  memcpy(destbuf + bufsize, buf, size);
  bufsize += size;
  //printf("destbuffer is now %d bytes\n", bufsize);
  free(buf);
}


void jpg_membuf_dest(j_compress_ptr cinfo, unsigned char *dest) {

  //printf("jpg_membuf_dest called\n");
  destbuf = dest;
  jpg_dest.init_destination = &jpg_init_dest;
  jpg_dest.empty_output_buffer = &jpg_empty_destbuf;
  jpg_dest.term_destination = &jpg_term_dest;
  cinfo->dest = &jpg_dest;
}


/* Compress a 24-bit 64x64 pixel bitmap in {R,G,B} format using JPEG
   with the given quality setting. */
int jpegcompress(unsigned char *src, unsigned char *dest) {
  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;
  JSAMPROW row_pointer[1];

  bufsize = 0;

  //printf("jpeg_std_error\n");
  cinfo.err = jpeg_std_error(&jerr);
  //printf("jpeg_create_compress\n");
  jpeg_create_compress(&cinfo);

  //printf("jpg_membuf_dest\n");
  jpg_membuf_dest(&cinfo, dest);

  //printf("setting parameters\n");
  cinfo.image_width = 64; 	/* image width and height, in pixels */
  cinfo.image_height = 64;
  cinfo.input_components = 3;		/* # of color components per pixel */
  cinfo.in_color_space = JCS_RGB; 	/* colorspace of input image */
  jpeg_set_defaults(&cinfo);
  jpeg_set_quality(&cinfo, TP_QUALITY, TRUE /* limit to baseline-JPEG values */);
  cinfo.smoothing_factor = TP_BLUR;
  cinfo.optimize_coding = TRUE;

  //printf("jpeg_start_compress\n");
  jpeg_start_compress(&cinfo, TRUE);

  //printf("jpeg_write_scanlines\n");
  while (cinfo.next_scanline < cinfo.image_height) {
    row_pointer[0] = &src[cinfo.next_scanline * 64*3];
    (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
  }

  //(void) jpeg_write_scanlines(&cinfo, (JSAMPARRAY) src, 64);

  //printf("jpeg_finish_compress\n");
  jpeg_finish_compress(&cinfo);

  //printf("jpeg_destroy_compress\n");
  jpeg_destroy_compress(&cinfo);

  return(bufsize);
}
