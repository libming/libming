#include <libming.h>


int main()
{
	SWFMovie m;

	m= newSWFMovieWithVersion(8);
	SWFMovie_setNetworkAccess(m, 1);
	SWFMovie_save(m,"test02.swf");
	return 0;
}
