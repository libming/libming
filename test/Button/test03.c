#include <libming.h>

int main()
{
	SWFMovie m = newSWFMovie();

	SWFShape s = newSWFShape();
	SWFShape_setLine(s, 10, 255, 0, 0, 255);
	SWFFillStyle fill = newSWFSolidFillStyle(255, 255, 0, 255);
	SWFShape_setRightFillStyle(s, fill);
	SWFShape_movePenTo(s, -50, -20);
	SWFShape_drawLine(s, 100, 0);
	SWFShape_drawLine(s, 0, 40);
	SWFShape_drawLine(s, -100, 0);
	SWFShape_drawLineTo(s, -50, -20);

	// simple animation by relative scaling
	SWFMovieClip mc1 = newSWFMovieClip();
	SWFDisplayItem si1 = SWFMovieClip_add(mc1, (SWFBlock) s);
	SWFMovieClip_nextFrame(mc1);
	SWFDisplayItem_scale(si1, 0.8, 0.8);
	SWFMovieClip_nextFrame(mc1);
	SWFDisplayItem_scale(si1, 0.8, 0.8);
	SWFMovieClip_nextFrame(mc1);
	SWFDisplayItem_scale(si1, 0.8, 0.8);
	SWFMovieClip_nextFrame(mc1);
	SWFDisplayItem_scale(si1, 0.8, 0.8);
	SWFMovieClip_nextFrame(mc1);
	SWFDisplayItem_scale(si1, 0.8, 0.8);
	SWFMovieClip_nextFrame(mc1);

	// another animation by absolute scaling
	SWFMovieClip mc2 = newSWFMovieClip();
	SWFDisplayItem si2 = SWFMovieClip_add(mc2, (SWFBlock) s);
	SWFDisplayItem_scaleTo(si2, 0.2, 0.2);
	SWFMovieClip_nextFrame(mc2);
	SWFDisplayItem_scaleTo(si2, 0.4, 0.4);
	SWFMovieClip_nextFrame(mc2);
	SWFDisplayItem_scaleTo(si2, 0.6, 0.6);
	SWFMovieClip_nextFrame(mc2);
	SWFDisplayItem_scaleTo(si2, 1, 1);
	SWFMovieClip_nextFrame(mc2);

	SWFButton b = newSWFButton();

	SWFButton_addCharacter(b, (SWFCharacter)s, SWFBUTTON_HIT|SWFBUTTON_UP);
	SWFButton_addCharacter(b, (SWFCharacter)mc1, SWFBUTTON_OVER);
	SWFButton_addCharacter(b, (SWFCharacter)mc2, SWFBUTTON_DOWN);

	SWFDisplayItem d;
	d = SWFMovie_add(m, b);
	SWFDisplayItem_moveTo(d, 150, 100);

	SWFMovie_nextFrame(m);
	SWFMovie_save(m, "test03.swf");
	
	return 0;
}
