#include <mingpp.h>


int main()
{
	SWFMovie *m;
	SWFAction *a;
	SWFInitAction *init;

	m = new SWFMovie();
	a = new SWFAction("trace('');");
	init = new SWFInitAction(a, 99);

	m->add(init);
	
	m->save("test02.swf");
	return 0;
}
