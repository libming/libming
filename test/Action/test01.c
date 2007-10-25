#include <libming.h>


int main()
{
	SWFMovie m;
	SWFAction a;
	SWFInitAction init;

	m = newSWFMovie();
	a = newSWFAction("trace('');");
	init = newSWFInitAction(a);
		
	SWFMovie_add(m, init);

	SWFMovie_save(m, "test01.swf");
	return 0;
}
