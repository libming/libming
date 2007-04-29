#include <libming.h>


main()
{
	SWFMovie m = newSWFMovieWithVersion(8);
	SWFMovie_setScriptLimits(m, 10, 11);
	SWFMovie_save(m,"test01.swf");
	return 0;
}
