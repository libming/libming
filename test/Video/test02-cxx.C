#include <mingpp.h>
 
int main ()
{
  SWFMovie *movie;
  SWFVideoStream *video1;
  SWFVideoStream *video2;
 
  movie = new SWFMovie(7);
 
  video1 = new SWFVideoStream ();
  movie->add (video1);
 
  video2 = new SWFVideoStream ();
  movie->add ( video2);
 
  movie->save ("test02.swf");
  return 0;
}
