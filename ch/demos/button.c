
#include <ming.h>

SWFShape makeRect(char r, char g, char b)
{
   SWFShape s;
   SWFFill f;

   s= newSWFShape();

   f= SWFShape_addSolidFill(s, r, g, b, 0xFF);
   SWFShape_setRightFill(s, f);
   SWFShape_movePenTo(s, -50.00, -50.00);
   SWFShape_drawLineTo(s, 50.00, -50.00);
   SWFShape_drawLineTo(s, 50.00, 50.00);
   SWFShape_drawLineTo(s, -50.00, 50.00);
   SWFShape_drawLineTo(s, -50.00, -50.00);

   return s;
}

int main(int argc, char *argv[])
{
   SWFMovie movie;
   SWFButton b;
   SWFDisplayItem timeline;

   Ming_init();
   Ming_useSWFVersion(5);

   movie= newSWFMovie();
   SWFMovie_setDimension(movie,320,240);
   SWFMovie_setRate(movie, 12);
   SWFMovie_setNumberOfFrames(movie,1);
   SWFMovie_setBackground(movie, 0xFF, 0xFF, 0xFF);

   b= newSWFButton();
   SWFButton_addShape(b, (SWFCharacter) makeRect(0xff, 0, 0), SWFBUTTON_UP);
   SWFButton_addShape(b, (SWFCharacter) makeRect(0, 0xff, 0), SWFBUTTON_OVER);
   SWFButton_addShape(b, (SWFCharacter) makeRect(0, 0, 0xff), SWFBUTTON_DOWN);
   SWFButton_addShape(b, (SWFCharacter) makeRect(0, 0, 0), SWFBUTTON_HIT);

   timeline= SWFMovie_add(movie,(SWFBlock) b);
   SWFDisplayItem_moveTo(timeline,160.00, 120.00);
   SWFMovie_nextFrame(movie);
   SWFMovie_save(movie, "button.swf");

   return 0;
}


