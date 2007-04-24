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
	
	SWFMovie_add(m, (SWFBlock)mc);

	SWFMovie_save(m, "test01.swf");
	return 0;
}
