
#include <ming.h> /* Header files and some helping functions */

int main(int argc, char *argv[])
{
   /* some typical movie variables */
   SWFMovie movie;
   SWFShape s;
   SWFFill f;
   SWFMovieClip p; // and not SWFSprite p ;
   SWFDisplayItem i;
   SWFDisplayItem pi;
   int n;

   Ming_init();

   Ming_useSWFVersion(6);
   Ming_setScale(1.00000000);
   movie= newSWFMovieWithVersion(6);
   SWFMovie_setDimension(movie, 6000,4000);
   SWFMovie_setBackground(movie, 0xcc,0xcc,0xcc);
   SWFMovie_setRate(movie, 12); 

   s = newSWFShape();
   f = SWFShape_addSolidFill(s, 0xff, 0, 0, 0xFF);
   SWFShape_setRightFill(s, f);

   SWFShape_movePenTo(s, -500,-500);
   SWFShape_drawLineTo(s, 500,-500);
   SWFShape_drawLineTo(s, 500,500);
   SWFShape_drawLineTo(s, -500,500);
   SWFShape_drawLineTo(s, -500,-500);

   p = newSWFMovieClip(); // notice that it's not newSWFSprite();

   i= SWFMovieClip_add((SWFMovieClip) p, (SWFBlock) s);
   SWFDisplayItem_setDepth(i, 1);
   SWFMovieClip_nextFrame( (SWFMovieClip) p);

   for(n=0; n<5; ++n)
   {
     SWFDisplayItem_rotate(i, -15);
     SWFMovieClip_nextFrame( (SWFMovieClip) p);
   }

   pi = SWFMovie_add(movie, (SWFBlock) p);
   SWFDisplayItem_setDepth(pi, 1);
   SWFDisplayItem_moveTo(pi, -500,2000);
   SWFDisplayItem_setName(pi,"box");

   SWFMovie_add(movie, (SWFBlock)compileSWFActionCode("x=-100;"));
   SWFMovie_nextFrame(movie);
   SWFMovie_add(movie, (SWFBlock)compileSWFActionCode("x+=3; box._x=x;"));
   SWFMovie_nextFrame(movie);
   SWFMovie_add(movie, (SWFBlock)compileSWFActionCode("gotoFrame(1); play();"));
   SWFMovie_nextFrame(movie);

   SWFMovie_save(movie, "action.swf");
   return 0;
}
