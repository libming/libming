#include <libming.h>

int main()
{
	SWFMovie m = newSWFMovieWithVersion(8);
	SWFDBLBitmapData img = newSWFDBLBitmapData_fromPngFile("../Media/image01.png");
	
	SWFMovie_add(m, (SWFBlock)img);
	SWFMovie_save(m, "test01.swf");
	return 0;

}
