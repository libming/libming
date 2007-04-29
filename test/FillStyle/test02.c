#include <libming.h>

int main()
{
	SWFMovie m = newSWFMovieWithVersion(8);
	FILE *file = fopen("../Media/image01.jpeg", "rb");
	if(file == NULL)
		return -1;

	SWFJpegBitmap img = newSWFJpegBitmap(file);
	SWFFillStyle fill = newSWFBitmapFillStyle((SWFCharacter)img, SWFFILL_TILED_BITMAP);
	SWFShape shape = newSWFShape();
	SWFShape_setRightFillStyle(shape, fill);
	
	SWFShape_setLineStyle(shape, 1, 0,0,0,255);
	SWFShape_drawLine(shape, 100, 0);
	SWFShape_drawLine(shape, 0, 100);
	SWFShape_drawLine(shape, -100, 0);
	SWFShape_drawLine(shape, 0, -100);

	SWFMovie_add(m, (SWFBlock)shape);
	SWFMovie_save(m, "test02.swf");
	return 0;

}
