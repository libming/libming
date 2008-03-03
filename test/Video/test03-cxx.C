#include <mingpp.h>
#include <cstdlib>


int main (int argc, char **argv)
{
	SWFMovie *movie;
	SWFVideoStream *video;
	SWFDisplayItem *item;
	SWFAction *action;

	try {
		movie = new SWFMovie(7);
		movie->setRate (1);
		movie->setDimension (200, 150);
		video = new SWFVideoStream ();
		video->setDimension (200, 150);
		item = movie->add (video);
		item->setName ("video");
		action = new SWFAction(""
			"nc = new NetConnection ();"
			"nc.connect (null);"
			"ns = new NetStream (nc);"
			"video.attachVideo (ns);"
			"ns.play (\"video.flv\");"
			"");
		movie->add (action);
		movie->save ("test03.swf");
	}
	catch(SWFException &e)
	{
		std::cerr << "SWFException: " << e.what() << std::endl << std::endl;
		return EXIT_FAILURE;
	}
	return 0;
}
