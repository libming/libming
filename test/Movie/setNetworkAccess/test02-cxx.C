#include <mingpp.h>
#include <cstdlib>


int main(){
	try {
		SWFMovie* m = new SWFMovie(8);
		m->setNetworkAccess(1);
		m->save("test02.swf");
	}
	catch(SWFException &e)
	{
		std::cerr << "SWFException: " << e.what() << std::endl << std::endl;
		return EXIT_FAILURE;
	}
	return 0;
}
