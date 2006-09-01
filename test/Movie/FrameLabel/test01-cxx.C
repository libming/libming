#include <mingpp.h>


main(){
class SWFMovie *m;
m = new SWFMovie();


// SWF_SETBACKGROUNDCOLOR 
m->setBackground(0xff, 0xff, 0xff);

// SWF_FRAMELABEL 
m->labelFrame("frame0");

// SWF_SHOWFRAME 
m->nextFrame(); // end of frame 1 

m->save("test01.swf");
}
