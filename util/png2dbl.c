#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>

#include <ming_config.h>

#include <png.h>

#include <zlib.h>

int verbose = 0;
typedef unsigned char byte;

void error(char *s, ...)
{
  va_list ap;
  va_start(ap, s);
  vprintf(s, ap);
  va_end(ap);
  putchar('\n');
  exit(1);
}

void usage()
{
  printf("png2dbl - convert a png file to an SWF DefineBitsLossless\n");
  printf("          or DefineBitsLossless2 block\n");
  printf("\nUsage: png2bmap [--verbose] <file.png>\n");
  printf("where file.png is your png file.  Writes to file.dbl.\n");
  exit(1);
}

struct pngdata
{
  png_uint_32 width;
  png_uint_32 height;
  int bit_depth;
  int color_type;
  int num_palette;
  int channels;
  png_colorp palette;
  unsigned char *data;
};

struct pngdata readPNG(FILE *fp)
{
  struct pngdata png;
  unsigned char header[8];
  unsigned int x, y;
  unsigned char *p;
  byte alpha;
  byte r, g, b;
  int ret;

  png_structp png_ptr;
  png_infop info_ptr, end_info;
  png_bytep *row_pointers;

  unsigned int i, rowbytes, depth;

  if(!fp)
    error("Couldn't open file!\n");

  ret = fread(header, 1, 8, fp);
  if(ret != 8)
    error("failed read header\n");

  /*
  if(png_check_sig(header, 8))
    error("File doesn't appear to be a proper PNG file\n");
  */
  png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

  if(!png_ptr)
    error("Couldn't create png_ptr\n");

  info_ptr = png_create_info_struct(png_ptr);

  if(!info_ptr)
  {
    png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
    error("Couldn't create info_ptr\n");
  }

  end_info = png_create_info_struct(png_ptr);

  if(!end_info)
  {
    png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
    error("Couldn't create end_info\n");
  }

  if(setjmp(png_jmpbuf(png_ptr)))
  {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    fclose(fp);
    error("Mystery error (longjmp called)\n");
  }

  png_init_io(png_ptr, fp);
  png_set_sig_bytes(png_ptr, 8);
  png_read_info(png_ptr, info_ptr);

  png_get_IHDR(png_ptr, info_ptr, &png.width, &png.height,
	       &png.bit_depth, &png.color_type,
	       NULL, NULL, NULL);

  if(verbose)
  {
    printf("size %ld/%ld color %d/%d/%ld\n",
	   png.width, png.height, png.bit_depth, png.color_type,
	   png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS));
  }

  if(png.color_type == PNG_COLOR_TYPE_PALETTE)
  {
    if(verbose)
      printf("color type: PALETTE\n");

    png_get_PLTE(png_ptr, info_ptr, &png.palette, &png.num_palette);
  }

  /* force bitdepth of 8 if necessary */

  depth = png.bit_depth;

  if(png.bit_depth < 8)
    png_set_packing(png_ptr);

  if(png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
    png_set_expand(png_ptr);

  if(png.bit_depth == 16)
  {
    if(verbose)
      printf("depth: 16\n");

    png_set_strip_16(png_ptr);
  }

  if(png.color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
  {
    if(verbose)
      printf("color type: GRAY ALPHA\n");

    png_set_gray_to_rgb(png_ptr);
  }

  if(png.color_type == PNG_COLOR_TYPE_RGB)
  {
    if(verbose)
      printf("color type: RGB\n");

    png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);
  }


  /* update info w/ the set transformations */
  png_read_update_info(png_ptr, info_ptr);

  png_get_IHDR(png_ptr, info_ptr, &png.width, &png.height,
	       &png.bit_depth, &png.color_type,
	       NULL, NULL, NULL);

  png.channels = png_get_channels(png_ptr, info_ptr);


  if(png.color_type == PNG_COLOR_TYPE_GRAY)
  {
    /* turn it into a palettized image, use the gray values as indices to
       an rgb color table */

    int i;

	if (verbose) printf("color type: GRAY\n");

    png.color_type = PNG_COLOR_TYPE_PALETTE;
    png.num_palette = 1 << depth;

    png.palette = (png_color*) malloc(sizeof(png_color) * png.num_palette);

    for(i=0; i<(int)png.num_palette; ++i)
      png.palette[i].red = png.palette[i].green = png.palette[i].blue = 
	(png_byte) ( (i*255)/(png.num_palette-1) );
  }


  /* malloc stuff */
  row_pointers = (png_bytep*) malloc(png.height*sizeof(png_bytep));
  rowbytes = png_get_rowbytes(png_ptr, info_ptr);

  png.data = (unsigned char*) malloc(png.height * rowbytes);

  for(i=0; i<png.height; ++i)
    row_pointers[i] = png.data + rowbytes*i;

  png_read_image(png_ptr, row_pointers);

  if(png.color_type == PNG_COLOR_TYPE_RGB_ALPHA ||
     png.color_type == PNG_COLOR_TYPE_RGB)
  {
    /* alpha has to be pre-applied, bytes shifted */

    if(verbose)
      printf("color type: RGB ALPHA\n");

    for(y=0; y<png.height; ++y)
    {
      for(x=0; x<png.width; ++x)
      {
	p = png.data + rowbytes*y + 4*x;

        r = p[0];
        g = p[1];
        b = p[2];

	alpha = p[3];

	p[0] = alpha;
	p[1] = (char)((int)(r*alpha)>>8);
	p[2] = (char)((int)(g*alpha)>>8);
	p[3] = (char)((int)(b*alpha)>>8);
      }
    }
  }

  return png;
}

static void
alignedcopy(struct pngdata png, unsigned char *data)
{
  unsigned int row;
  int alignedrowsize;
  int pngrowsize;
  unsigned char *pngdata;

  alignedrowsize = (png.width * png.channels + 3) & ~3;
  pngdata = png.data;
  pngrowsize = png.width * png.channels;

  for( row = 0; row < png.height; row++ )
  {
    memcpy(data, pngdata, pngrowsize);
    pngdata += pngrowsize;
    data += alignedrowsize;
  }
}

void writeDBL(FILE *f, struct pngdata png)
{
  unsigned char *data, *outdata;
  int alignedsize;
  unsigned long outsize;
  int hasAlpha =
    png.color_type == PNG_COLOR_TYPE_GRAY_ALPHA ||
    png.color_type == PNG_COLOR_TYPE_RGB_ALPHA;

  fputc('D', f);
  fputc('B', f);
  fputc('l', f);
  fputc(hasAlpha ? 2 : 1, f);

  alignedsize = png.height * ((png.width * png.channels + 3) & ~3);

  if(png.color_type == PNG_COLOR_TYPE_PALETTE)
  {
    int tablesize = png.num_palette * sizeof(png_color);
  
    data = (unsigned char*) malloc(tablesize + alignedsize);
    memcpy(data, png.palette, tablesize);
    alignedcopy(png, data + tablesize);
    alignedsize += tablesize;
  }
  else
  {
    data = (unsigned char*) malloc(alignedsize);
    alignedcopy(png, data);
  }

  outdata = (unsigned char*) malloc(outsize = (int)floor(alignedsize*1.01+12));
  /* compress the RGB color table (if present) and image data one block */
  compress2(outdata, &outsize, data, alignedsize, 9);
#ifndef USE_ZLIB  
  outsize = alignedsize;		/* if using zlib use the compress2() result value!  */
#endif
  /* write the remaining file size */

  if(png.color_type == PNG_COLOR_TYPE_PALETTE)
  {
    fputc(((outsize+6)>>24)&0xff, f);
    fputc(((outsize+6)>>16)&0xff, f);
    fputc(((outsize+6)>>8)&0xff, f);
    fputc((outsize+6)&0xff, f);
  }
  else
  {
    fputc(((outsize+5)>>24)&0xff, f);
    fputc(((outsize+5)>>16)&0xff, f);
    fputc(((outsize+5)>>8)&0xff, f);
    fputc((outsize+5)&0xff, f);
  }

  /* UInt8 format
     3: 8bit data
     4: 16bit data
     5: 32bit data */

  if(png.bit_depth != 8)
    error("Can't deal with bit depth %i!", png.bit_depth);

  if(png.color_type == PNG_COLOR_TYPE_PALETTE)
    fputc(3, f);
  else
    fputc(5, f);

  fputc(png.width&0xff, f);
  fputc((png.width>>8)&0xff, f);

  fputc(png.height&0xff, f);
  fputc((png.height>>8)&0xff, f);

  if(png.color_type == PNG_COLOR_TYPE_PALETTE)
    fputc(png.num_palette-1, f);

  if(fwrite(outdata, sizeof(char), outsize, f) != outsize)
    error("Didn't write all of the file!");
}

int main(int argc, char *argv[])
{
  int len;
  char *outfile;
  struct pngdata png;

  if (argc > 2 && strcmp(argv[1], "--verbose")==0) {
	verbose = 1;
	argc--;
	argv++;
  }

  if(argc < 2)
    usage();

  len = strlen(argv[1]);

  if(strcmp(argv[1]+len-4, ".png") != 0)
    usage();

  if(argc < 3)
  {
    outfile = strdup(argv[1]);

    outfile[len-3] = 'd';
    outfile[len-2] = 'b';
    outfile[len-1] = 'l';
  }
  else
    outfile = argv[2];

  png = readPNG(fopen(argv[1], "rb"));
  writeDBL(fopen(outfile, "wb"), png);

  exit(0);
}
