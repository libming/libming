#include <mingpp.h>
#include <cstdlib>


main(){
SWFMovie *m;
try {
m = new SWFMovie(8);

m->save("test06.swf");
}
catch(SWFException &e)
{
std::cerr << "SWFException: " << e.what() << std::endl << std::endl;
return EXIT_FAILURE;
}
return 0;
}
