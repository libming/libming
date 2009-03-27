#include <libming.h>
#include <stdlib.h>

#define SIZE 20

int main(void)
{
	SWFMovie m;
	SWFShape shape;
	SWFButton b;
	SWFDisplayItem item;
	SWFFilterMatrix cmf;
	SWFFilter filter;
	SWFBitmap bm;
	SWFFillStyle fill;
	FILE *f;

	Ming_init();
	m = newSWFMovieWithVersion(9);
	if(m == NULL)
	{
		fprintf(stderr, "Could not create SWF movie with version 9\n");
		return EXIT_FAILURE;
	}
	SWFMovie_setBackground(m, 0xcc, 0xcc, 0xcc);
	shape = newSWFShape();
	f = fopen(MEDIADIR "/image01.dbl", "rb");
	if(!f)
	{
		fprintf(stderr, "Could not open " MEDIADIR "/image01.dbl\n");
		return EXIT_FAILURE;
	}

	bm = (SWFBitmap) newSWFDBLBitmap(f);
	fill = newSWFBitmapFillStyle(bm, SWFFILL_TILED_BITMAP);
	SWFShape_setRightFillStyle(shape, fill);
	int w = SWFBitmap_getWidth(bm);
	int h = SWFBitmap_getHeight(bm);
	SWFShape_drawLine(shape, w, 0);
	SWFShape_drawLine(shape, 0, h);
	SWFShape_drawLine(shape, -w, 0);
	SWFShape_drawLine(shape, 0, -h);

	float tab[SIZE] = {0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1};
	cmf = newSWFFilterMatrix(5, 4, tab);
	filter = newColorMatrixFilter(cmf);
	
	b = newSWFButton();
	SWFButton_addCharacter(b, (SWFCharacter)shape, SWFBUTTON_UP | SWFBUTTON_HIT | SWFBUTTON_OVER | SWFBUTTON_DOWN);
	item = SWFMovie_add(m, b);
	SWFDisplayItem_addFilter(item, filter);

	SWFMovie_save(m, "test05.swf");
	return EXIT_SUCCESS;
}
