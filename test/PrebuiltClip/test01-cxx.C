#include <mingpp.h>


int main()
{
	SWFMovie *m;
	SWFPrebuiltClip *pbc;

	m = new SWFMovie();
	m->save("empty.swf");
	
	m = new SWFMovie();
	pbc = new SWFPrebuiltClip("empty.swf");
	m->add(pbc);

	pbc = new SWFPrebuiltClip("empty.swf");
	m->add(pbc);

	m->save("test01.swf");
}
