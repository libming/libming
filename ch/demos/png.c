
#include <ming.h>

int main(int argc, char *argv[])
{
   SWFMovie movie;
   SWFShape shape;
   FILE *file_bitmap;
   SWFBitmap bitmap;
   SWFFill fill;
   SWFDisplayItem timeline;
   char *dblFile = "png.dbl";

   Ming_init(argc, argv);
   Ming_useSWFVersion(5);
   movie= newSWFMovie();
   SWFMovie_setDimension(movie, 32, 32);
   SWFMovie_setRate(movie, 12);
   SWFMovie_setNumberOfFrames(movie, 1);
   shape= newSWFShape();

   if(!(file_bitmap= fopen(dblFile,"rb")))
   {   
      printf("Couldn't find file %s\n", dblFile);   
   }

   bitmap = (SWFCharacter) newSWFDBLBitmap(file_bitmap);
   fill= SWFShape_addBitmapFill(shape, bitmap, SWFFILL_TILED_BITMAP);
   SWFShape_setRightFill(shape, fill);
   SWFShape_drawLineTo(shape, 32.00, 0.00);
   SWFShape_drawLineTo(shape, 32.00, 32.00);
   SWFShape_drawLineTo(shape, 0.00, 32.00);
   SWFShape_drawLineTo(shape, 0.00, 0.00);

   timeline= SWFMovie_add(movie, (SWFBlock) shape);
   SWFDisplayItem_moveTo(timeline, 0.00, 0.00);
   SWFMovie_nextFrame(movie);
   SWFMovie_save(movie, "png.swf");

   fclose(file_bitmap); 
   return 0;
}


