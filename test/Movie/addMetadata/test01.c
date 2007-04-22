#include <libming.h>

static char *test = "test01";
static const char *xml = "<rdf:RDF xmlns:rdf=http://www.w3.org/1999/02/22-rdf-syntax-ns#></rdf:RDF>";

int main()
{
	SWFMovie m;

	m= newSWFMovie();
	SWFMovie_addMetadata(m, (char *)xml);
	SWFMovie_save(m,"test01.swf");
	return 0;
}
