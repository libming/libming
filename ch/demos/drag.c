
#include <ming.h>

int main(int argc, char *argv[])
{
   SWFMovie movie;
   SWFShape _i1;
   SWFFill _fill1_1;
   SWFButton _i2;
   char * strAction= ";";
   SWFMovieClip _i3; // SWFSprite _i3;
   SWFDisplayItem timeline;

   Ming_init(argc, argv);
   Ming_useSWFVersion(4);

   _i1= newSWFShape();
   /* SWFShape_setNoLineStyle(_i1);*/
   _fill1_1= SWFShape_addSolidFill(_i1, 0xFF, 0x00, 0x00, 0xFF);
   SWFShape_setRightFill(_i1, _fill1_1);
   SWFShape_movePenTo(_i1, -25.00, -25.00);
   SWFShape_drawLineTo(_i1, 25.00, -25.00);
   SWFShape_drawLineTo(_i1, 25.00, 25.00);
   SWFShape_drawLineTo(_i1, -25.00, 25.00);
   SWFShape_drawLineTo(_i1, -25.00, -25.00);

   _i2= newSWFButton();
   SWFButton_addShape(_i2, (SWFCharacter) _i1, 
      SWFBUTTON_HIT | SWFBUTTON_DOWN | SWFBUTTON_OVER | SWFBUTTON_UP);

   SWFButton_addAction( _i2, compileSWFActionCode("startDrag('/test', 0);"), 
      SWFBUTTON_MOUSEDOWN);

   SWFButton_addAction(_i2, compileSWFActionCode("stopDrag();"), 
      SWFBUTTON_MOUSEUP | SWFBUTTON_MOUSEUPOUTSIDE);

   _i3= newSWFMovieClip();
   SWFMovieClip_add((SWFMovieClip) _i3, (SWFBlock) _i2);
   SWFMovieClip_nextFrame((SWFMovieClip) _i3);

   movie= newSWFMovie();
   SWFMovie_setDimension(movie, 320, 240);

   timeline= SWFMovie_add(movie, (SWFBlock) _i3);

   SWFDisplayItem_setName(timeline, "test");
   SWFDisplayItem_moveTo(timeline, 160,120);
   SWFMovie_nextFrame(movie);

   SWFMovie_save(movie, "drag.swf");

   return 0;
}
