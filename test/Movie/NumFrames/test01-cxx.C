#include <mingpp.h>


main(){
class SWFMovie *m;
m = new SWFMovie();

m->setFrames(2);

// SWF_SETBACKGROUNDCOLOR 
m->setBackground(0xff, 0xff, 0xff);
m->nextFrame(); // end of frame 1 
m->nextFrame(); // end of frame 2 

m->save("test01.swf");
}
