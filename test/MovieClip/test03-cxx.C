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
	m->add(mc);
	m->save("test03.swf");
	return 0;
}
