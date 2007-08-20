#include <mingpp.h>


int main()
{
	SWFMovieClip *mc;
	SWFMovie *m;
	SWFAction *a;

	m = new SWFMovie();
	a = new SWFAction("trace('');");
	mc = new SWFMovieClip();

	mc->addInitAction(a);
	m->addExport(mc, "test");	

	m->save("test02.swf");
	return 0;
}
