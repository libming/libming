
#include <ming.h>

int main(int argc, char *argv[])
{
   SWFMovie m;
   SWFShape shape;
   FILE *file_bitmap;
   SWFBitmap b;
   SWFFill fill;
   float height= 480.0;
   float width= 640.0;


   Ming_init(argc, argv);
   Ming_useSWFVersion(5);
   m= newSWFMovie();
   SWFMovie_setDimension(m, width, height);

   shape= newSWFShape();
   if(!(file_bitmap= fopen("backyard.jpg","rb")))
   {   
       printf("Couldn't find file backyard.jpg");   
   }
   b= (SWFCharacter) newSWFJpegBitmap(file_bitmap);

   fill= SWFShape_addBitmapFill(shape, b, SWFFILL_TILED_BITMAP);

   SWFShape_setRightFill(shape, fill);
   SWFShape_drawLineTo(shape, width,  0.00);
   SWFShape_drawLineTo(shape, width, height);
   SWFShape_drawLineTo(shape, 0.00, height);
   SWFShape_drawLineTo(shape, 0.00, 0.00);

   SWFMovie_add(m, (SWFBlock) shape);

   SWFMovie_save(m, "jpegfill.swf");

   fclose(file_bitmap); /* Do not close earlier or an error will happen */
   return 0;
}


