#include <libming.h>

int main()
{
	int i;	
	SWFMovie m = newSWFMovieWithVersion(9);
	
	for(i = 0; i < 10; i++)
		SWFMovie_nextFrame(m);

	SWFMovie_defineScene(m, 0, "test0");
	SWFMovie_defineScene(m, 5, "test1");

	SWFMovie_save(m, "test01.swf");

	return 0;
}
	
