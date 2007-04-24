#include <mingpp.h>


int main()
{
        SWFMovie *m = new SWFMovie(7);

	m->setTabIndex(1, 2);
	m->setTabIndex(2, 2);

        m->save("test01.swf");
        return 0;
}

