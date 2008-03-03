#include <mingpp.h>
#include <cstdlib>


main(){
SWFMovie *m;
try {
m = new SWFMovie(8);
m->setBackground(0, 0, 0);
m->save("test07.swf");
}
catch(SWFException &e)
{
std::cerr << "SWFException: " << e.what() << std::endl << std::endl;
return EXIT_FAILURE;
}
return 0;
}
