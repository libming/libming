#include <mingpp.h>


int main(){
	SWFMovie* m = new SWFMovie();
	m->setScriptLimits(10,11);
	m->save("test01.swf");
}
