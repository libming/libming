#include <libming.h>
 
int main ()
{
  SWFMovie movie;
  SWFVideoStream video1;
  SWFVideoStream video2;
 
  movie = newSWFMovieWithVersion(7);
 
  video1 = newSWFVideoStream ();
  SWFMovie_add (movie, (SWFBlock) video1);
 
  video2 = newSWFVideoStream ();
  SWFMovie_add (movie, (SWFBlock) video2);
 
  SWFMovie_save (movie, "test02.swf");
  return 0;
}
