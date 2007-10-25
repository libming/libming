#include <libming.h>


int main()
{
	SWFMovie m;
	SWFAction a;
	SWFInitAction init;

	m = newSWFMovie();
	a = newSWFAction("trace('');");
	init = newSWFInitAction_withId(a, 99);

	SWFMovie_add(m, init);	
	SWFMovie_save(m, "test02.swf");
	return 0;
}
