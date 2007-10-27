#include <mingpp.h>


int main()
{
	SWFMovie *m = new SWFMovie();
	SWFCharacter *c = m->importCharacter("test.swf", "test");
	m->add(c);
	m->save("test01.swf");
	return 0;
}
