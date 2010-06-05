#include <libming.h>

int main()
{
	SWFMovie m = newSWFMovie();

	SWFShape s = newSWFShape();
	SWFShape_setLine(s, 1, 255, 0, 0, 255);
	SWFFillStyle fill = newSWFSolidFillStyle(255, 255, 0, 255);
	SWFShape_setRightFillStyle(s, fill);
	SWFShape_drawLine(s, 100, 0);
	SWFShape_drawLine(s, 0, 40);
	SWFShape_drawLineTo(s, 0, 0);

	SWFButton b = newSWFButton();
	SWFButtonRecord br1;
	SWFButtonRecord br2;
	SWFButtonRecord br3;

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

	SWFMovie_addExport(m, (SWFBlock)b, "ButtonExport");
	SWFMovie_writeExports(m);

	SWFMovie_nextFrame(m);
	SWFMovie_save(m, "test02.swf");
	
	return 0;
}
