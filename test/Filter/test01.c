#include <ming.h>

// static char *test = "test01";

int main()
{
	SWFMovie m;
	SWFShape shape;
	SWFButton b;
	SWFDisplayItem item;
	SWFBlur blur;
	SWFFilter f;

        Ming_init();
        m = newSWFMovieWithVersion(7);
        if(m == NULL)
                return 0;
	
	shape = newSWFShape();

        SWFShape_setLine(shape, 4, 25, 0, 0, 128);
        SWFShape_movePenTo(shape, 5, 5);
        SWFShape_drawLineTo(shape, 0, 10);
	
	blur = newSWFBlur(5,5,2);
	f = newBlurFilter(blur);

	b = newSWFButton();
        SWFButton_addShape(b, (SWFCharacter)shape,
                SWFBUTTON_UP | SWFBUTTON_HIT | SWFBUTTON_OVER | SWFBUTTON_DOWN);
        item = SWFMovie_add(m, (SWFBlock)b);
	
	SWFDisplayItem_addFilter(item, f);
        SWFMovie_save(m, "test01.swf");
	return 0;
}
