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
	
	m->add(mc);

	m->save("test01.swf");
	return 0;
}
