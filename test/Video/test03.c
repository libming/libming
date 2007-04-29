#include <libming.h>
 
int main (int argc, char **argv)
{
  SWFMovie movie;
  SWFVideoStream video;
  SWFDisplayItem item;
  SWFAction action;
 
  movie = newSWFMovieWithVersion(7);
  SWFMovie_setRate (movie, 1);
  SWFMovie_setDimension (movie, 200, 150);
  video = newSWFVideoStream ();
  SWFVideoStream_setDimension (video, 200, 150);
  item = SWFMovie_add (movie, (SWFBlock) video);
  SWFDisplayItem_setName (item, "video");
  action = newSWFAction (""
      "nc = new NetConnection ();"
      "nc.connect (null);"
      "ns = new NetStream (nc);"
      "video.attachVideo (ns);"
      "ns.play (\"video.flv\");"
      "");
  SWFMovie_add (movie, (SWFBlock) action);
  SWFMovie_save (movie, "test03.swf");
  return 0;
}
