#include <mingpp.h>


main(){
class SWFMovie *m;
m = new SWFMovie();


// SWF_SETBACKGROUNDCOLOR 
m->setBackground(0xff, 0xff, 0xff);

// SWF_DEFINESPRITE 
SWFSprite* sp1 = new SWFSprite(); // 1 frames 

// SWF_SHOWFRAME 
sp1->nextFrame(); // end of clip frame 1 

// SWF_END 

// SWF_PLACEOBJECT2 
m->add(sp1);

// SWF_SHOWFRAME 
m->nextFrame(); // end of frame 1 

m->save("test02.swf");
}
