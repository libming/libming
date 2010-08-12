#include <libming.h>
#include <stdlib.h>

int main()
{
	SWFMovie m = newSWFMovieWithVersion(8);
	SWFDBLBitmapData img = newSWFDBLBitmapData_fromPngFile(MEDIADIR "/image01.png");
	if(img == NULL)
		return EXIT_FAILURE;

	SWFMovie_addExport(m, (SWFBlock)img, "BitmapExport");
	SWFMovie_writeExports(m);
	
	SWFMovie_save(m, "test01.swf");
	return 0;

}
