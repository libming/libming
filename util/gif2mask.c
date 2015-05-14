
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <ming_config.h>

#include <gif_lib.h>
#include <zlib.h>

#include "libming.h"

#define max3(a,b,c) (((a)>(b))?(((c)>(a))?(c):(a)):(((c)>(b))?(c):(b)))

void error(char *msg, int errorCode)
{
  printf("%s:\n\n", msg);
  PrintGifError(errorCode);
  exit(-1);
}

unsigned char *readGif(char *fileName, int *length)
{
  GifFileType *file;
  unsigned char *bits;
  unsigned char colorMap[256];
  unsigned char *data;
  int i, nColors, size;

#if GIFLIB_MAJOR < 5
  if((file = DGifOpenFileName(fileName)) == NULL)
    error("Error opening file", 0);
#else
  int errorCode = 0;

  if((file = DGifOpenFileName(fileName, &errorCode)) == NULL)
    error("Error opening file", errorCode);
#endif

  if(DGifSlurp(file) != GIF_OK)
#if GIFLIB_MAJOR < 5
    error("Error slurping file", 0);
#else
    error("Error slurping file", file->Error);
#endif

  /* data should now be available */

  nColors = file->SColorMap->ColorCount;

  for(i=0; i<nColors; ++i)
  {
    GifColorType c = file->SColorMap->Colors[i];

    colorMap[i] = max3(c.Blue, c.Red, c.Green);
  }

  bits = file->SavedImages[0].RasterBits;

  size = file->SWidth * file->SHeight;
  data = malloc(size);

  for(i=0; i<size; ++i)
    data[i] = colorMap[(unsigned char)bits[i]];

  *length = size;

  return data;
}

void usage()
{
  printf("gif2mask - convert a gif image to an alpha mask\n");
  printf("\nusage: gif2mask <file.gif>\n");

  exit(0);
}

int main(int argc, char *argv[])
{
  int len, size;
  unsigned long outsize;
  char *outfile;
  unsigned char *data, *outdata;
  FILE *mask;

  if(argc < 2)
    usage();

  len = strlen(argv[1]);

  if(strcmp(argv[1]+len-4, ".gif") != 0)
    usage();

  outfile = strdup(argv[1]);

  outfile[len-3] = 'm';
  outfile[len-2] = 's';
  outfile[len-1] = 'k';

  data = readGif(argv[1], &size);

  outdata = malloc(outsize = (int)floor(size*1.01+12));

  /* zlib-compress the gif data */
  compress2(outdata, &outsize, data, size, 9);
  
  /* outsize = size; */
  /* Let's better use the compress2() result value         */
  /* of compressed data size instead using original size   */
  /* else we would waste some outdata file space  -- ak'09 */

  /* dump to outfile */

  mask = fopen(outfile, "wb");

  if(fwrite(outdata, sizeof(char), outsize, mask) != outsize)
  {
    printf("Didn't write all of the file!");
    exit(-1);
  }

  exit(0);
}
