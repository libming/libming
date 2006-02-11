
#include <ming.h>


int main(int argc, char *argv[])
{
   SWFMovie movie;
   SWFMorph p;
   SWFShape shape_1;
   SWFGradient grad_1;
   SWFFill fill1;
   SWFShape shape_2;
   SWFGradient grad_2;
   SWFFill fill2;
   SWFDisplayItem timeline;
   float n;

   Ming_init(argc, argv);
   Ming_useSWFVersion(5);
   movie= newSWFMovie();
   SWFMovie_setDimension(movie, 320, 240);

   p= newSWFMorphShape();

   grad_1= newSWFGradient();
   SWFGradient_addEntry(grad_1, 0, 0x00, 0x00, 0x00, 0xFF);
   SWFGradient_addEntry(grad_1, 0.16, 0xFF, 0xFF, 0xFF, 0xFF);
   SWFGradient_addEntry(grad_1, 0.32, 0x00, 0x00, 0x00, 0xFF);
   SWFGradient_addEntry(grad_1, 0.48, 0xFF, 0xFF, 0xFF, 0xFF);
   SWFGradient_addEntry(grad_1, 0.64, 0x00, 0x00, 0x00, 0xFF);
   SWFGradient_addEntry(grad_1, 0.8, 0xFF, 0xFF, 0xFF, 0xFF);
   SWFGradient_addEntry(grad_1, 1, 0x00, 0x00, 0x00, 0xFF);

   shape_1= SWFMorph_getShape1(p);
   fill1= SWFShape_addGradientFill(shape_1,grad_1, SWFFILL_RADIAL_GRADIENT);
   SWFFill_scaleTo(fill1, 0.100, 0.100);
   SWFShape_setLeftFill(shape_1, fill1);
   SWFShape_movePenTo(shape_1, -160.00, -120.00);
   SWFShape_drawLineTo(shape_1, 160.00, -120.00);
   SWFShape_drawLineTo(shape_1, 160.00, 120.00);
   SWFShape_drawLineTo(shape_1, -160.00, 120.00);
   SWFShape_drawLineTo(shape_1, -160.00, -120.00);

   grad_2= newSWFGradient();
   SWFGradient_addEntry(grad_2, 0, 0x00, 0x00, 0x00, 0xFF);
   SWFGradient_addEntry(grad_2, 0.16, 0xFF, 0x00, 0x00, 0xFF);
   SWFGradient_addEntry(grad_2, 0.32, 0x00, 0x00, 0x00, 0xFF);
   SWFGradient_addEntry(grad_2, 0.48, 0x00, 0xFF, 0x00, 0xFF);
   SWFGradient_addEntry(grad_2, 0.64, 0x00, 0x00, 0x00, 0xFF);
   SWFGradient_addEntry(grad_2, 0.8, 0x00, 0x00, 0xFF, 0xFF);
   SWFGradient_addEntry(grad_2, 1, 0x00, 0x00, 0x00, 0xFF);

   shape_2= SWFMorph_getShape2(p);
   fill2= SWFShape_addGradientFill(shape_2,grad_2, SWFFILL_RADIAL_GRADIENT);

   SWFFill_scaleTo(fill2, 0.16, 0.16);
   SWFFill_skewXTo(fill2, -0.50);
   //SWFFill_rotateTo(fill2, -26.57);

   SWFShape_setLeftFill(shape_2, fill2);
   SWFShape_movePenTo(shape_2, -160.00, -120.00);
   SWFShape_drawLineTo(shape_2, 160.00, -120.00);
   SWFShape_drawLineTo(shape_2, 160.00, 120.00);
   SWFShape_drawLineTo(shape_2, -160.00, 120.00);
   SWFShape_drawLineTo(shape_2, -160.00, -120.00);

   timeline= SWFMovie_add(movie,(SWFBlock) p);
   SWFDisplayItem_moveTo(timeline, 160.00, 120.00);

   for(n=0; n<=1.001; n+=0.01)
   {
      SWFDisplayItem_setRatio(timeline, n);
      SWFMovie_nextFrame(movie);
   }

   SWFMovie_save(movie, "gradientxform.swf");

   return 0;
}
