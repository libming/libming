#include <mingpp.h>


int main()
{
	SWFMovie *m = new SWFMovie(8);
	SWFCharacter *c = m->importCharacter("test.swf", "test");
	m->add(c);
	m->save("test02.swf");
	return 0;
}
