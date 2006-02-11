
#include <ming.h>

int main(int argc, char *argv[])
{
   SWFMovie m;
   SWFShape shape;
   SWFBitmap b;
   SWFFill fill;
   SWFDisplayItem i;
   FILE *file_bitmap;
   float n;
   float height;
   float width;
   char *imageFile = "backyard.jpg";

   Ming_init(argc, argv);
   Ming_useSWFVersion(5);
   m = newSWFMovie();

   shape = newSWFShape();

   if(!(file_bitmap = fopen(imageFile,"rb")))
   {   
      printf("Couldn't find file '%s'", imageFile);   
   }

   b = (SWFCharacter) newSWFJpegBitmap(file_bitmap);


   SWFMovie_setDimension(m, SWFBitmap_getWidth(b), SWFBitmap_getHeight(b));

   fill = SWFShape_addBitmapFill(shape, b, SWFFILL_TILED_BITMAP);

   SWFShape_setRightFill(shape, fill);

   height = (float) SWFBitmap_getHeight(b);
   width = (float) SWFBitmap_getWidth(b);
   SWFShape_drawLineTo(shape, width, 0.00);
   SWFShape_drawLineTo(shape, width, height);
   SWFShape_drawLineTo(shape, 0.00, height);
   SWFShape_drawLineTo(shape, 0.00, 0.00);


   i = SWFMovie_add(m, (SWFBlock) shape);

   for(n=0; n<20.0; ++n)
   {
      SWFDisplayItem_multColor(i, 1.0- n/10.0, 1.0, 1.0, 1.0);
      SWFDisplayItem_addColor(i, 0xff* n/20.0, 0, 0, 0);
      SWFMovie_nextFrame(m);
   }

   for(n=20.0; n>0; --n)
   {
      SWFDisplayItem_multColor(i, 1.0- n/10.0, 1.0, 1.0, 1.0);
      SWFDisplayItem_addColor(i, 0xff* n/20.0, 0, 0, 0);
      SWFMovie_nextFrame(m);
   }

   SWFMovie_save(m, "cxform.swf");

   fclose(file_bitmap); /* Do not close earlier or an error will happen */
   return 0;
}


