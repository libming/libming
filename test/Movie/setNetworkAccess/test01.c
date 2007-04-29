#include <libming.h>


int main()
{
	SWFMovie m;

	m= newSWFMovieWithVersion(8);
	SWFMovie_setNetworkAccess(m, 0);
	SWFMovie_save(m,"test01.swf");
	return 0;
}
