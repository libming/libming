#include <mingpp.h>


main(){
class SWFMovie *m;
m = new SWFMovie(6);


//  SWF_SETBACKGROUNDCOLOR 
m->setBackground(0xff, 0xff, 0xff);
m->nextFrame(); //  end of frame 1 

m->save("test04.swf");
}
