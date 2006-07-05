/// // DL1QUANT.CPP - DL1 Color Quantization Class
#include "stdafx.h"

#include <windows.h>

#include "chitem.h"
#include "dl1quant.h"
#include "options.h"

static BYTE palette[3][256];
static CUBE *rgb_table[6];
static CLOSEST_INFO c_info;
static int tot_colors, pal_index;
static FCUBE *heap;
static short *dl_image;
static int bitspp;

/* returns 1 on success, 0 on failure */
int dl1quant(BYTE *inbuf, BYTE *outbuf, CPoint dimension, palettetype &userpal)
{
  int pixels=dimension.x*dimension.y;
  int qualityloss;
  int ret;

  // 3 or 4 bytes/pixel
  bitspp=4; 
  
  if (dl1_build_table(inbuf, pixels) )
  {
    dlq_finish();
    return -3;
  }
  
  dl1_reduce_table(256); //quant_to = 256
  dl1_set_palette(0, 0);
  
  if(editflg&DITHER) //the flag is negated
  {
    ret=dl1_quantize_image(outbuf, dimension.x, dimension.y); //doesn't require inbuf
  }
  else
  {
    ret=dl1_quantize_image_dither(inbuf, outbuf, dimension.x, dimension.y);
  }
  if(ret)
  {
    dlq_finish();
    return ret;
  }
  
  dlq_finish();
  dl1_copy_pal(userpal);

//optional quality loss measurements
  qualityloss=0;
  while(pixels--)
  {
    qualityloss+=ChiSquare((LPBYTE) inbuf,(LPBYTE) (userpal+*outbuf++) );
    inbuf+=bitspp;
  }
  return qualityloss;
}

static void dl1_copy_pal(palettetype &userpal) 
{
  int i;
  
  for (i = 0; i < 256; i++)
  {
    userpal[i]=RGB(palette[0][i],palette[1][i],palette[2][i]);
  }
}

static void dlq_finish(void) 
{
  int i;
  
  for (i=0;i<6;i++)
  {
    if (rgb_table[i] != NULL)
    {
      delete [] rgb_table[i];
      rgb_table[i]=NULL;
    }
  }
  
  if (heap != NULL)
  {
    delete [] heap;
    heap=NULL;
  }
  
  if (dl_image != NULL)
  {
    delete [] dl_image;
    dl_image=NULL;
  } 
}

/* returns 1 on success, 0 on failure */
static int dl1_build_table(BYTE *image, unsigned long pixels) 
{
  int index=0; //never exceeds 0x7fff
  unsigned long i=0;
  unsigned long cur_count=0;
  unsigned long head=0;
  unsigned long tail=0;
  long j=0;

  tot_colors=0;
  pal_index=0;
  
  heap = NULL;
  dl_image = NULL;
  
  memset(&c_info,0,sizeof(c_info));
  memset(palette,0,sizeof(palette));
  rgb_table[0] = new CUBE[1];
  rgb_table[1] = new CUBE[8];
  rgb_table[2] = new CUBE[64];
  rgb_table[3] = new CUBE[512];
  rgb_table[4] = new CUBE[4096];
  rgb_table[5] = new CUBE[32768];

  for (i = 0; i <= 5; i++)
  {
    if (rgb_table[i] == NULL)
    {
      return -3;
    }
  }

  heap = new FCUBE [32769];
  if (heap == NULL)
  {
    return -3;
  }
  
  dl_image = new short[pixels];
  if (dl_image == NULL)
  {
    return -3;
  }

  memset(rgb_table[0],0,sizeof(CUBE) );
  memset(rgb_table[1],0,sizeof(CUBE)*8 );
  memset(rgb_table[2],0,sizeof(CUBE)*64 );
  memset(rgb_table[3],0,sizeof(CUBE)*512);
  memset(rgb_table[4],0,sizeof(CUBE)*4096 );
  memset(rgb_table[5],0,sizeof(CUBE)*32768 );
  
  for (i = 0; i < pixels; i++)
  {
    index = r_offset[image[0]] + g_offset[image[1]] + b_offset[image[2]];
    dl_image[i] = (short) index;
    
    rgb_table[5][index].r += image[0];
    rgb_table[5][index].g += image[1];
    rgb_table[5][index].b += image[2];
    image+=bitspp;
    rgb_table[5][index].pixel_count++;
  }
  
  tot_colors = 0;
  for (i = 0; i < 32768; i++)
  {
    cur_count = rgb_table[5][i].pixel_count;
    if (cur_count)
    {
      heap[++tot_colors].level = 5;
      heap[tot_colors].index = (unsigned short)i;
      rgb_table[5][i].pixels_in_cube = cur_count;
      head = i;
      for (j = 4; j >= 0; j--)
      {
        tail = head & 0x7;
        head >>= 3;
        rgb_table[j][head].pixels_in_cube += cur_count;
        rgb_table[j][head].children |= 1 << tail;
      }
    }
  }
  
  for (i = tot_colors; i > 0; i--)
  {
    dl1_fixheap(i);
  }
  
  return 0;
}

static void dl1_fixheap(int id) 
{
  BYTE thres_level = heap[id].level;
  unsigned long thres_index = heap[id].index;
  int index=0;
  int half_totc = tot_colors >> 1;
  unsigned long thres_val = rgb_table[thres_level][thres_index].pixels_in_cube;
  
  while (id <= half_totc)
  {
    index = id << 1;
    
    if (index < tot_colors)
    {
      if (rgb_table[heap[index].level][heap[index].index].pixels_in_cube
        > rgb_table[heap[index+1].level][heap[index+1].index].pixels_in_cube)
      {
        index++;      
      }
    }
    if (thres_val <= rgb_table[heap[index].level][heap[index].index].pixels_in_cube)
      break;
    heap[id] = heap[index];
    id = index;
  }
  
  heap[id].level = thres_level;
  heap[id].index = (unsigned short)thres_index;
}

static void dl1_reduce_table(int num_colors) 
{
  while (tot_colors > num_colors)
  {
    BYTE tmp_level = heap[1].level;
    BYTE t_level = max((BYTE) 0,(BYTE) (tmp_level - 1));
    unsigned long tmp_index = heap[1].index, t_index = tmp_index >> 3;
    
    if (rgb_table[t_level][t_index].pixel_count)
    {
      heap[1] = heap[tot_colors--];
    }
    else
    {
      heap[1].level = t_level;
      heap[1].index = (unsigned short)t_index;
    }
    
    rgb_table[t_level][t_index].pixel_count += rgb_table[tmp_level][tmp_index].pixel_count;
    rgb_table[t_level][t_index].r += rgb_table[tmp_level][tmp_index].r;
    rgb_table[t_level][t_index].g += rgb_table[tmp_level][tmp_index].g;
    rgb_table[t_level][t_index].b += rgb_table[tmp_level][tmp_index].b;
    rgb_table[t_level][t_index].children &= ~(1 << (tmp_index & 0x7));
    
    dl1_fixheap(1);
  }
}

static void dl1_set_palette(int index, int level) 
{
  int i;
  
  if (rgb_table[level][index].children)
  {
    for (i = 7; i >= 0; i--)
    {
      if (rgb_table[level][index].children & (1 << i))
      {
        dl1_set_palette((index << 3) + i, level + 1);
      }
    }
  }
  
  if (rgb_table[level][index].pixel_count)
  {
    unsigned long r_sum, g_sum, b_sum, sum;
    
    rgb_table[level][index].palette_index = (BYTE) pal_index;
    
    r_sum = rgb_table[level][index].r;
    g_sum = rgb_table[level][index].g;
    b_sum = rgb_table[level][index].b;
    
    sum = rgb_table[level][index].pixel_count;
    
    palette[0][pal_index] = (BYTE)((r_sum + (sum >> 1)) / sum);
    palette[1][pal_index] = (BYTE)((g_sum + (sum >> 1)) / sum);
    palette[2][pal_index] = (BYTE)((b_sum + (sum >> 1)) / sum);
    
    pal_index++;
  }
}

static void dl1_closest_color(int index, int level) 
{
  int i;
  
  if (rgb_table[level][index].children)
  {
    for (i = 7; i >= 0; i--)
    {
      if (rgb_table[level][index].children & (1 << i))
      {
        dl1_closest_color((index << 3) + i, level + 1);
      }
    }
  }
  
  if (rgb_table[level][index].pixel_count)
  {
    unsigned long dist;
    int r_dist, g_dist, b_dist;
    BYTE pal_num = rgb_table[level][index].palette_index;
    
    /* Determine if this color is "closest". */
    r_dist = palette[0][pal_num] - c_info.red;
    g_dist = palette[1][pal_num] - c_info.green;
    b_dist = palette[2][pal_num] - c_info.blue;
    
    dist = squares[r_dist] + squares[g_dist] + squares[b_dist];
    
    if (dist < c_info.distance)
    {
      c_info.distance = dist;
      c_info.palette_index = pal_num;
    }
  }
}

static int dl1_quantize_image(BYTE *out, int width, int height) 
{
  int i;
  int pixels = width * height;
  int level;
  int index;
  BYTE tmp_r;
  BYTE tmp_g;
  BYTE tmp_b;
  BYTE cube;
  BYTE *lookup=NULL;
  int ret=0;
  
  lookup = new BYTE[32768];
  if (lookup == NULL)
  {
    ret=-3;
    goto endofquest;
  }
  
  for (i = 0; i < 32768; i++)
  {
    if (rgb_table[5][i].pixel_count)
    {
      tmp_r = (BYTE)((i & 0x4000) >> 7 | (i & 0x0800) >> 5 |
        (i & 0x0100) >> 3 | (i & 0x0020) >> 1 |
        (i & 0x0004) << 1);
      tmp_g = (BYTE)((i & 0x2000) >> 6 | (i & 0x0400) >> 4 |
        (i & 0x0080) >> 2 | (i & 0x0010) >> 0 |
        (i & 0x0002) << 2);
      tmp_b = (BYTE)((i & 0x1000) >> 5 | (i & 0x0200) >> 3 |
        (i & 0x0040) >> 1 | (i & 0x0008) << 1 |
        (i & 0x0001) << 3);
      //this is slower but better quality
//      lookup[i] = (BYTE) dl1_bestcolor(tmp_r, tmp_g, tmp_b);
      //this is faster but slightly worse quality
      /*begin*/
		  c_info.red   = (BYTE) (tmp_r + 4);
			c_info.green = (BYTE) (tmp_g + 4);
			c_info.blue  = (BYTE) (tmp_b + 4);
			level = 0;
			index = 0;
      for (;;)
      {
        cube =(BYTE) ((tmp_r&128) >> 5 | (tmp_g&128) >> 6 | (tmp_b&128) >> 7);
        if ((rgb_table[level][index].children & (1 << cube)) == 0)
        {
          c_info.distance = (ULONG)~0L;
          dl1_closest_color(index, level);
          lookup[i] = c_info.palette_index;
          break;
        }
        level++;
        index = (index << 3) + cube;
        tmp_r <<= 1;
        tmp_g <<= 1;
        tmp_b <<= 1;
      }
      //end
    }
  }
  
  for (i = 0; i < pixels; i++)
  {
    out[i] = lookup[dl_image[i]];
  }
endofquest:  
  if(lookup) delete [] lookup;
  return ret;
}
/* returns 1 on success, 0 on failure */
static int dl1_quantize_image_dither(BYTE *in, BYTE *out, int width, int height) 
{
  int ret;
  
  long i;
  long j;
  int r_pix;
  int g_pix;
  int b_pix;
  long offset;
  short *lookup = NULL;
  int r_err, g_err, b_err; // used in dither1
  
  lookup = new short[32768];
  if (lookup == NULL)    
  {
    ret=-3;
    goto endofquest;
  }
  memset(lookup,-1,sizeof(short)*32768);
// dither 1
  for (i = 0; i < height; i++)
  {
    r_err = g_err = b_err = 0;
    for (j = width - 1; j >= 0; j--)
    {
      r_pix = range[(r_err >> 1) + in[0]];
      g_pix = range[(g_err >> 1) + in[1]];
      b_pix = range[(b_err >> 1) + in[2]];
      
      offset = (r_pix&248) << 7 | (g_pix&248) << 2 | b_pix >> 3;
      
      if (lookup[offset] < 0)
      {
        lookup[offset] =(BYTE) dl1_bestcolor(r_pix, g_pix, b_pix);
      }
      
      *out++ = (BYTE)lookup[offset];
      
      r_err = r_pix - palette[0][lookup[offset]];
      g_err = g_pix - palette[1][lookup[offset]];
      b_err = b_pix - palette[2][lookup[offset]];
      
      in += bitspp;
    }
	}

  ret=0;
endofquest:    
  if(lookup) delete [] lookup;
  
  return ret;
}

// ez itt egy szimpla chisquare illesztes, linearisan, kis hibaturessel
static int dl1_bestcolor(int r, int g, int b) 
{
  unsigned long i=0;
  unsigned long bestcolor=0;
  unsigned long curdist=0;
  unsigned long mindist=0;
  long rdist=0;
  long gdist=0;
  long bdist=0;
  
  r = (r & 248) + 4;
  g = (g & 248) + 4;
  b = (b & 248) + 4;
  rdist = palette[0][0] - r;
  gdist = palette[1][0] - g;
  bdist = palette[2][0] - b;
  mindist = squares[rdist] + squares[gdist] + squares[bdist];
  
  for (i = 1; i < (unsigned long)tot_colors; i++)
  {
    rdist = palette[0][i] - r;
    gdist = palette[1][i] - g;
    bdist = palette[2][i] - b;
    curdist = squares[rdist] + squares[gdist] + squares[bdist];
    if (curdist < mindist)
    {
      mindist = curdist;
      bestcolor = i;
    }
  }
  return bestcolor;
}
