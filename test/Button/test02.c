#include <libming.h>

int main()
{
	SWFMovie m = newSWFMovie();

	SWFShape s = newSWFShape();
	SWFShape_setLine(s, 20, 255, 0, 0, 255);
	SWFShape_addSolidFill(s, 255, 255, 0, 255);
	SWFShape_drawLine(s, 100, 0);
	SWFShape_drawLine(s, 0, 40);
	SWFShape_drawLineTo(s, 0, 0);

	SWFButton b = newSWFButton();
	SWFButton br1 = newSWFButtonRecord();
	SWFButton br2 = newSWFButtonRecord();
	SWFButton br3 = newSWFButtonRecord();

	br1 = SWFButton_addCharacter(b, (SWFCharacter)s, SWFBUTTON_HIT|SWFBUTTON_UP|SWFBUTTON_OVER|SWFBUTTON_DOWN);

	br2 = SWFButton_addCharacter(b, (SWFCharacter)s, SWFBUTTON_OVER|SWFBUTTON_DOWN);
	SWFButtonRecord_rotate(br2,10);
	SWFButtonRecord_move(br2,20,0);

	br3 = SWFButton_addCharacter(b, (SWFCharacter)s, SWFBUTTON_DOWN);
	SWFButtonRecord_rotate(br3,20);
	SWFButtonRecord_move(br3,40,0);

	SWFDisplayItem d;
	d = SWFMovie_add(m, b);
	SWFDisplayItem_moveTo(d, 50, 100);

	SWFMovie_nextFrame(m);
	SWFMovie_save(m, "test02.swf");
	
	return 0;
}
