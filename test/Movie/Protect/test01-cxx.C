#include <mingpp.h>


main(){
class SWFMovie *m;
m = new SWFMovie();

// SWF_PROTECT 
m->protect();

// SWF_SHOWFRAME 
m->nextFrame(); // end of frame 1 

m->save("test01.swf");
}
