#include <mingpp.h>
#include <cstdlib>


main(){
	try {
		SWFMovie* m = new SWFMovie();


		// SWF_SETBACKGROUNDCOLOR 
		m->setBackground(0x00, 0xff, 0x00);

		// SWF_SHOWFRAME 
		m->nextFrame(); // end of frame 1 

		// SWF_END 

		m->save("test02.swf");
	}
	catch(SWFException &e)
        {
                std::cerr << "SWFException: " << e.what() << std::endl << std::endl;
                return EXIT_FAILURE;
        }
        return 0;
}
