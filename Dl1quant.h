#ifndef DL1QUANT_H
#define DL1QUANT_H

typedef struct
{
    unsigned long r, g, b;
    unsigned long pixel_count;
    unsigned long pixels_in_cube;
    BYTE children;
    BYTE palette_index;
} CUBE;

typedef struct
{
    BYTE  level;
    unsigned short index;
} FCUBE;

typedef struct
{
    BYTE palette_index,
	  red, green, blue;
    unsigned long distance;
} CLOSEST_INFO;

int dl1quant(BYTE *inbuf, BYTE *outbuf, CPoint dimension, palettetype &userpal);
static  void dl1_copy_pal(palettetype &userpal);
static  void dlq_finish(void);
static  int  dl1_build_table(BYTE *image, unsigned long pixels);
static  void dl1_fixheap(int id);
static  void dl1_reduce_table(int num_colors);
static  void dl1_set_palette(int index, int level);
static  void dl1_closest_color(int index, int level);
static  int  dl1_quantize_image(BYTE *out, int width, int height); //doesn't require inbuf
static  int  dl1_quantize_image_dither(BYTE *in, BYTE *out, int width, int height);
static  int  dl1_bestcolor(int r, int g, int b);

#endif
