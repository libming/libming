#include <libming.h>
#include <stdlib.h>

int main()
{
	SWFMovie m;
	SWFShape shape;
	SWFButton b;
	SWFDisplayItem item;
	SWFBlur blur;
	SWFFilter f;
	SWFColor c;

	Ming_init();
	m = newSWFMovieWithVersion(7);
	if(m == NULL)
		return EXIT_FAILURE;
	
	shape = newSWFShape();

	SWFShape_setLine(shape, 4, 25, 0, 0, 128);
	SWFShape_movePenTo(shape, 5, 5);
	SWFShape_drawLineTo(shape, 0, 10);
	
	blur = newSWFBlur(5,5,2);
	
	c.red = 0;
	c.green = 0;
	c.blue = 0;
	c.alpha = 0xff;

	f = newGlowFilter(c, blur, 1.0, FILTER_MODE_INNER | FILTER_MODE_KO);	

	b = newSWFButton();
	SWFButton_addCharacter(b, (SWFCharacter)shape,
		SWFBUTTON_UP | SWFBUTTON_HIT | SWFBUTTON_OVER | SWFBUTTON_DOWN);
	item = SWFMovie_add(m, (SWFBlock)b);
	
	SWFDisplayItem_addFilter(item, f);
	SWFMovie_save(m, "test03.swf");
	return 0;
}
