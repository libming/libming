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
	SWFShadow shadow;
	SWFColor c1, c2;

	Ming_init();
	m = newSWFMovieWithVersion(7);
	if(m == NULL)
		return EXIT_FAILURE;
	
	shape = newSWFShape();

	SWFShape_setLine(shape, 4, 25, 0, 0, 128);	
	SWFShape_movePenTo(shape, 5, 5);
	SWFShape_drawLineTo(shape, 0, 10);
	
	blur = newSWFBlur(5,5,2);
	shadow = newSWFShadow(0.79, 5, 1.0);
	
	c1.red = 0;
	c1.green = 0;
	c1.blue = 0;
	c1.alpha = 0xff;

	c2.red = 0xff;
	c2.green = 0xff;
	c2.blue = 0xff;
	c2.alpha = 0xff;
	f = newBevelFilter(c1, c2, blur, shadow, FILTER_MODE_INNER | FILTER_MODE_KO);	

	b = newSWFButton();
	SWFButton_addCharacter(b, (SWFCharacter)shape,
		SWFBUTTON_UP | SWFBUTTON_HIT | SWFBUTTON_OVER | SWFBUTTON_DOWN);
	item = SWFMovie_add(m, (SWFBlock)b);
	
	SWFDisplayItem_addFilter(item, f);
	SWFMovie_save(m, "test04.swf");
	return 0;
}
