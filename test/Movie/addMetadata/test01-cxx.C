#include <mingpp.h>
#include <cstdlib>

static const char *xml = "<rdf:RDF xmlns:rdf=http://www.w3.org/1999/02/22-rdf-syntax-ns#></rdf:RDF>";
main(){
	try {
		SWFMovie* m = new SWFMovie();
		m->addMetadata((char *)xml);
		m->save("test01.swf");
	}
	catch(SWFException &e)
	{
		std::cerr << "SWFException: " << e.what() << std::endl << std::endl;
		return EXIT_FAILURE;
	}
	return 0;
}
