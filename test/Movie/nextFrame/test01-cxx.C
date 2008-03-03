#include <mingpp.h>
#include <cstdlib>


main(){
try {
SWFMovie* m = new SWFMovie();

m->setFrames(2);

// SWF_SETBACKGROUNDCOLOR 
m->setBackground(0xff, 0xff, 0xff);

// SWF_SHOWFRAME 
m->nextFrame(); // end of frame 1 

// SWF_DOACTION 
m->add(new SWFAction("var a = 1;\
") );

// SWF_SHOWFRAME 
m->nextFrame(); // end of frame 2 

// SWF_END 

m->save("test01.swf");
}
catch(SWFException &e)
{
std::cerr << "SWFException: " << e.what() << std::endl << std::endl;
return EXIT_FAILURE;
}
return 0;
}
