#include <mingpp.h>


int main(){
	SWFMovie* m = new SWFMovie(8);
	m->setNetworkAccess(0);
	m->save("test01.swf");
	return 0;
}
