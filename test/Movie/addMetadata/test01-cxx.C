#include <mingpp.h>

static const char *xml = "<rdf:RDF xmlns:rdf=http://www.w3.org/1999/02/22-rdf-syntax-ns#></rdf:RDF>";
main(){
SWFMovie* m = new SWFMovie();
m->addMetadata((char *)xml);
m->save("test01.swf");
}
