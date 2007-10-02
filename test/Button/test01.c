#include <libming.h>

int main()
{
	SWFDisplayItem d;
	SWFMovie m = newSWFMovie();
	SWFButton b = newSWFButton();
	SWFShape s = newSWFShape();
	char buf[512];
	
	sprintf(buf, "_root.gotoAndStop(2);");

	SWFShape_setLine(s, 2, 255, 0, 0, 255);
	SWFShape_drawCircle(s, 20);
	
	SWFButton_addCharacter(b, (SWFCharacter)s, 
		SWFBUTTON_HIT|SWFBUTTON_UP|SWFBUTTON_DOWN|SWFBUTTON_OVER);
	
	SWFButton_addAction(b, newSWFAction(buf), SWFBUTTON_MOUSEUP);	

	d = SWFMovie_add(m, b);
	SWFMovie_add(m, newSWFAction("_root.stop();"));
	SWFDisplayItem_moveTo(d, 100, 100);
	SWFMovie_nextFrame(m);
	
	SWFDisplayItem_remove(d);
	b = newSWFButton();
	sprintf(buf, "_root.gotoAndStop(1);");
	SWFButton_addCharacter(b, (SWFCharacter)s, 
		SWFBUTTON_HIT|SWFBUTTON_UP|SWFBUTTON_DOWN|SWFBUTTON_OVER);
	
	SWFButton_addAction(b, newSWFAction(buf), SWFBUTTON_MOUSEDOWN);
	d = SWFMovie_add(m, b);
	SWFDisplayItem_moveTo(d, 50, 100);

	SWFMovie_nextFrame(m);
	SWFMovie_save(m, "test01.swf");
	
	return 0;
}
