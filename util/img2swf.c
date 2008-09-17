#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ming.h"

void usage()
{
  printf("img2swf - Converts an image in an SWF\n");
  printf("Usage: img2swf <image> <output> [<width> <height>]\n");
  exit(1);
}

void embed_image(SWFMovie movie, char *f)
{
	SWFFill fill;
	SWFBitmap bm;
	SWFShape shape;
	SWFDisplayItem i;
	FILE *raster;
	SWFInput in;

        if (!(raster = fopen (f, "rb")))
        {
                fprintf (stdout, "%s: %s\n", f, strerror (errno));
		exit(1);
        }

        if (!(in = newSWFInput_file(raster)))
        {
                fprintf (stdout, "Can't create SWFInput from file\n");
		exit(1);
        }

        if (!(bm = newSWFBitmap_fromInput (in)))
        {
                fprintf (stdout, "Error creating bitmap");
		exit(1);
        }


	shape = newSWFShape();
  
	SWFShape_movePenTo(shape, 0, 0);

	fill = SWFShape_addBitmapFill(shape, bm, SWFFILL_CLIPPED_BITMAP);
	SWFShape_setRightFill(shape, fill);
	SWFShape_drawLineTo(shape, 0, 640);
	SWFShape_drawLineTo(shape, 480, 640);
	SWFShape_drawLineTo(shape, 480, 0);
	SWFShape_drawLineTo(shape, 0, 0);

	i = SWFMovie_add(movie, (SWFBlock)shape);
}

int main(int argc, char *argv[])
{
  int len, w, h;
  char *outfile;
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
  Ming_setSWFCompression(9);

  srand(time(NULL));

  len = strlen(argv[1]);

  // No more png-specific!
  //if(strcmp(argv[1]+len-4, ".png") != 0) usage();

  if(argc == 2)
  {
    outfile = strdup(argv[1]);

    outfile[len-3] = 's';
    outfile[len-2] = 'w';
    outfile[len-1] = 'f';
  }
  else
    outfile = argv[2];

  SWFMovie_setRate(m, 12.0);
  SWFMovie_setDimension(m, w, h);
  SWFMovie_setNumberOfFrames(m, 1);

  embed_image(m, argv[1]);

  SWFMovie_save(m, outfile);
  destroySWFMovie(m);

  exit(0);
}
