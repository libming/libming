#include <mingpp.h>


int main()
{
	SWFMovie *m;
	SWFAction *a;
	SWFInitAction *init;

	m = new SWFMovie();
	a = new SWFAction("trace('');");
	init = new SWFInitAction(a);
	
	m->add(init);

	m->save("test01.swf");
	return 0;
}
