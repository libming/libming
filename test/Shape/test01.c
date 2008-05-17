#include <libming.h>
#include <stdlib.h>

int main()
{
	SWFMovie m = newSWFMovieWithVersion(8);
	SWFShape shape1 = newSWFShape();
	SWFShape shape2 = newSWFShape();
	SWFShape_useVersion(shape2, SWF_SHAPE4);
	SWFMovie_add(m, (SWFBlock)shape1);
	SWFMovie_add(m, (SWFBlock)shape2);
	SWFMovie_save(m, "test01.swf");

	return EXIT_SUCCESS;
}
