#include <libming.h>


int main()
{
	SWFMovieClip mc;
	SWFMovie m;
	SWFAction a;

	m = newSWFMovie();
	a = newSWFAction("trace('');");
	mc = newSWFMovieClip();

	SWFMovieClip_addInitAction(mc, a);
	SWFMovie_addExport(m, mc, "test");
	
	SWFMovie_save(m, "test02.swf");
	return 0;
}
