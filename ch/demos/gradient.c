
#include <ming.h>

int main(int argc, char *argv[])
{
   SWFMovie movie;
   SWFShape shape1;
   SWFGradient grad_1;
   SWFFill fill1;
   SWFDisplayItem timeline;
   SWFShape shape2;
   SWFGradient grad_2;
   SWFFill fill2;

   Ming_init(argc, argv);
   Ming_useSWFVersion(5);
   movie= newSWFMovie();
   SWFMovie_setDimension(movie, 320, 240);

   shape1= newSWFShape();

   /* first gradient- black to white */
   grad_1= newSWFGradient();
   SWFGradient_addEntry(grad_1, 0, 0x00, 0x00, 0x00, 0xFF);
   SWFGradient_addEntry(grad_1, 1, 0xFF, 0xFF, 0xFF, 0xFF);

   fill1= SWFShape_addGradientFill(shape1, grad_1, SWFFILL_LINEAR_GRADIENT);
   SWFFill_scaleTo(fill1, 0.170, 0.170);
   SWFFill_moveTo(fill1, 160.00, 120.00);
   SWFShape_setRightFill(shape1, fill1);
   SWFShape_drawLineTo(shape1, 320.00, 0.00);
   SWFShape_drawLineTo(shape1, 320.00, 240.00);
   SWFShape_drawLineTo(shape1, 0.00, 240.00);
   SWFShape_drawLineTo(shape1, 0.00, 0.00);

   timeline= SWFMovie_add(movie, (SWFBlock) shape1);
   /* SWFDisplayItem_moveTo(timeline, 0.00, 0.00);*/

   shape2= newSWFShape();

   /* second gradient- radial gradient from white to red to transparent */
   grad_2= newSWFGradient();
   SWFGradient_addEntry(grad_2, 0, 0xFF, 0x00, 0x00, 0xFF);
   SWFGradient_addEntry(grad_2, 1, 0xFF, 0x00, 0x00, 0x00);

   fill2= SWFShape_addGradientFill(shape2, grad_2, SWFFILL_RADIAL_GRADIENT);
   SWFFill_scaleTo(fill2, 0.120, 0.120);
   SWFFill_moveTo(fill2, 160.00, 120.00);
   SWFShape_setRightFill(shape2, fill2);
   SWFShape_drawLineTo(shape2, 320.00, 0.00);
   SWFShape_drawLineTo(shape2, 320.00, 240.00);
   SWFShape_drawLineTo(shape2, 0.00, 240.00);
   SWFShape_drawLineTo(shape2, 0.00, 0.00);

   timeline= SWFMovie_add(movie, (SWFBlock) shape2);

   SWFMovie_nextFrame(movie);

   SWFMovie_save(movie, "gradient.swf");

   return 0;
}


