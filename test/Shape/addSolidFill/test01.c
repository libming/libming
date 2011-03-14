#include <libming.h>
#include <stdlib.h>

int main()
{
	SWFMovie m = newSWFMovieWithVersion(8);
	SWFShape shape1 = newSWFShape();
	SWFShape_addSolidFill(shape1, 32, 64, 128, 223);
	SWFShape_addSolidFill(shape1, 64, 128, 32, 24);
	SWFMovie_add(m, (SWFBlock)shape1);
	SWFMovie_save(m, "test01.swf");

	return EXIT_SUCCESS;
}
