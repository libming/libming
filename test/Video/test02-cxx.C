#include <mingpp.h>
#include <cstdlib>
 
int main ()
{
try {
  SWFMovie *movie;
  SWFVideoStream *video1;
  SWFVideoStream *video2;
 
  movie = new SWFMovie(7);
 
  video1 = new SWFVideoStream ();
  movie->add (video1);
 
  video2 = new SWFVideoStream ();
  movie->add ( video2);
 
  movie->save ("test02.swf");
}
catch(SWFException &e)
{
	std::cerr << "SWFException: " << e.what() << std::endl << std::endl;
	return EXIT_FAILURE;
}
return 0;

}
