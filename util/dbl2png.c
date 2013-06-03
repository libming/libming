#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
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
  printf("dbl2png - convert a dbl file to an png file\n");
  printf("\nUsage: dbl2png [--verbose] <file.dbl>\n");
  printf("where file.dbl is your dbl file.  Writes to file.png.\n");
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


struct pngdata readDBL(FILE *fp)
{
  struct pngdata png;
  byte header[13],*data;
  unsigned long outsize,readsize=0,datsize;
  int i, ret;

  if(!fp)
    error("Couldn't open file!\n");
  
  ret = fread(header, 1, 13, fp);
  if(ret != 13)
    error("Couldn't read header\n");

  if (header[3]==2)
  {
   if(verbose) 
    printf("image data RGBA\n");
   png.color_type=PNG_COLOR_TYPE_RGB_ALPHA;
  }
  else
  {
   if (header[3]==1)
   {
    if(verbose) 
     printf("image data RGB\n");
    png.color_type=PNG_COLOR_TYPE_RGB;
   }
   else
    error("unexpected 3rd byte in header");
  }
  outsize=(header[4]<<24)+(header[5]<<16)+(header[6]<<8)+header[7];
  if(verbose)
    printf("outsize=%ld \n",outsize);

  png.bit_depth=8; 					// Can't deal with bit depth !=8
  png.width= header[9] +(header[10]<<8);
  png.height=header[11]+(header[12]<<8);

  if (header[8]==3)
  {
    png.color_type=PNG_COLOR_TYPE_PALETTE;
    readsize=outsize-6 ;
    png.num_palette=0;
    i=  fread(&png.num_palette,  1,  1 ,fp); 
    if (verbose) 
    {
      printf("image has PALETTE\npalette size=%d\n",png.num_palette+1);
      printf("size %d x %d\n", (int)png.width, (int)png.height);
    }
  }
  else
  {
   if (header[8]!=5)
    error("unexpected 8th byte in header");
   else
   {
    readsize=outsize-5 ;
    if(verbose)
    {
     printf("size %d x %d\n", (int)png.width, (int)png.height);
    }
   }
  }
  if (!readsize)
   error("internal error");
  //
  // ------ now we go --------
  //
  data=malloc( readsize );
  if (!data)
  {
   error("memory allocation");
  }
  datsize=fread(data, 1, readsize, fp);
  fclose(fp);
  if (datsize != readsize)
  {
    free(data);
    error("Didn't read all of the file! %d  vs %d",datsize , readsize);
  }
  if (png.color_type==PNG_COLOR_TYPE_PALETTE)			// for palette bit_depth==8
   datsize = (png.num_palette+1) * 3 + png.width * png.height;	// palette of RGB style
  else
   datsize = png.width *  png.height * 4;			// RGBA
  png.data=malloc(datsize);
  if (!png.data)
  {
   free(data);
   error("memory allocation");
  }

  /* uncompress the data */
  i=uncompress (png.data, &datsize, data, readsize );
  if (i)
  {
   printf("uncompress call result = %d\n",i);
   free(data);
   error("uncompress error");
  }
  if(verbose)
  {
    printf("unpacked data size t=%ld byte\n",datsize);
  }
  free(data);
  return png;
}

// callback function:
// reverse apply alpha, bytes shifting (see png2dbl.c for details)
// expecting pixel_depth==32, bit_depth==8, color_type==PNG_COLOR_TYPE_RGB_ALPHA
void alpha_apply(png_structp png_ptr, png_row_infop row_info, png_bytep p)
{
  int i;
  byte a, r, g, b;
  for(i=0;i<row_info->width;i++)
  {
   a = p[0];
   r = p[1];
   g = p[2];
   b = p[3];
   *p++ = (r<<8)/(a+1);
   *p++ = (g<<8)/(a+1);
   *p++ = (b<<8)/(a+1);
   *p++ = a;
  }
}


void writePNG(FILE *fp, struct pngdata png)
{
  png_structp png_ptr;
  png_infop info_ptr;
  int i,chan;
  byte *ptr=png.data; 

  if(!fp)
    error("Couldn't open file!\n");

  png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if(!png_ptr)
    error("Couldn't create png_ptr\n");

  info_ptr = png_create_info_struct(png_ptr);
  if(!info_ptr)
  {
    png_destroy_write_struct(&png_ptr,(png_infopp)NULL);
    error("Couldn't create info_ptr\n");
  }

  if (setjmp(png_jmpbuf(png_ptr)))
  {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
    error("Mystery error (longjmp called)\n");
  }

  png_init_io(png_ptr, fp);
  png_set_IHDR(png_ptr, info_ptr, png.width, png.height,
  		png.bit_depth, png.color_type, PNG_INTERLACE_NONE, 
  		PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
  if (png.color_type==PNG_COLOR_TYPE_PALETTE)
    png_set_PLTE(png_ptr, info_ptr,(png_colorp)png.data, png.num_palette+1);
  png_write_info(png_ptr, info_ptr);

  //
  // ------ now we go --------
  //
  chan=png_get_channels(png_ptr, info_ptr);
  if (verbose)  
    printf("channel count=%d\n",chan);
  
  if(png.color_type == PNG_COLOR_TYPE_RGB_ALPHA )
  {
   png_set_write_user_transform_fn(png_ptr, alpha_apply);
   for (i=0;i<png.height;i++)
   { 
      png_write_row(png_ptr,ptr);
      ptr+=png.width * chan;				// 4
   }
  }
  if(png.color_type == PNG_COLOR_TYPE_RGB)
  {
   png_set_filler(png_ptr, 0, PNG_FILLER_BEFORE);
   for (i=0;i<png.height;i++)
   { 
      png_write_row(png_ptr,ptr);
      ptr+=png.width * 4;
   }
  }
  if(png.color_type == PNG_COLOR_TYPE_PALETTE)
  {
    ptr+=(png.num_palette+1) * 3; 			// for palette bit_depth==8 of RGB style
    for (i=0;i<png.height;i++)
    {
      png_write_row(png_ptr,ptr);
      ptr+=png.width * chan;				// 1
    }   
  }

  png_write_end(png_ptr,NULL);
  png_destroy_write_struct(&png_ptr, &info_ptr);
  free(png.data);
  fclose(fp);
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

  if(strcmp(argv[1]+len-4, ".dbl") != 0)
    usage();

  if(argc < 3)
  {
    outfile = strdup(argv[1]);
    outfile[len-3] = 'p';
    outfile[len-2] = 'n';
    outfile[len-1] = 'g';
  }
  else
    outfile = argv[2];

  png = readDBL(fopen(argv[1], "rb"));
  writePNG(fopen(outfile, "wb"), png);
  exit(0);
}
