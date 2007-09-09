#include <mingpp.h>

int main()
{
	int i;
	SWFMovie *m = new SWFMovie(9);
	
	for(i = 0; i < 10; i++)
		m->nextFrame();

	m->defineScene(0, "test0");
	m->defineScene(5, "test1");
	m->save("test01.swf");

	return 0;
}
	
