#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ming.h"

void usage()
{
  printf("png2swf - Converte uma imagem PNG em SWF\n\n");
  printf("Uso: png2swf arquivo.png [arquivo.swf] width height\n");
  exit(1);
}

void convertePNG(SWFMovie movie, char *f)
{
  SWFFill fill;
  SWFBitmap png;
  SWFShape shape;
  SWFDisplayItem i;

  png = newSWFDBLBitmap_fromPngFile(f);
  shape = newSWFShape();
  
  SWFShape_movePenTo(shape, 0, 0);

  fill = SWFShape_addBitmapFill(shape, png, SWFFILL_CLIPPED_BITMAP);
  SWFShape_setRightFill(shape, fill);
  SWFShape_drawLineTo(shape, 0, 640);
  SWFShape_drawLineTo(shape, 480, 640);
  SWFShape_drawLineTo(shape, 480, 0);
  SWFShape_drawLineTo(shape, 0, 0);

  i = SWFMovie_add(movie, (SWFCharacter)shape);
}

int main(int argc, char *argv[])
{
  int len, w, h;
  char *outfile;
  FILE *f;
  SWFMovie m = newSWFMovie();

  if((argc < 2) || (argc > 5))
    usage();

  if(argc < 5) {
    w = 640;
    h = 480;
  } else {
    w = atoi(argv[3]);
    h = atoi(argv[4]);
  }

  Ming_init();

  srand(time(NULL));

  len = strlen(argv[1]);

  if(strcmp(argv[1]+len-4, ".png") != 0)
    usage();

  if(argc == 2)
  {
    outfile = strdup(argv[1]);

    outfile[len-3] = 's';
    outfile[len-2] = 'w';
    outfile[len-1] = 'f';
  }
  else
    outfile = argv[2];

  f = fopen(outfile, "wb");

  SWFMovie_setRate(m, 12.0);
  SWFMovie_setDimension(m, w, h);
  SWFMovie_setNumberOfFrames(m, 1);

  convertePNG(m, argv[1]);

  SWFMovie_output(m, fileOutputMethod, f, 9);
  destroySWFMovie(m);

  exit(0);
}
