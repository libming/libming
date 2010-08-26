#include <libming.h>
#include <stdlib.h>

int main()
{
	SWFMovie m = newSWFMovieWithVersion(8);
	SWFJpegBitmap img;
	FILE *file;
	
	file = fopen(MEDIADIR "/image01.jpeg", "rb");
	if(!file) {
		perror(MEDIADIR "/image01.jpeg");
		return EXIT_FAILURE;
	}

	img = newSWFJpegBitmap(file);
	if(img == NULL) return EXIT_FAILURE;
	
	SWFMovie_addExport(m, (SWFBlock)img, "BitmapExport");
	SWFMovie_writeExports(m);

	SWFMovie_save(m, "test02.swf");

	return 0;

}
