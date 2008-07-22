#include <mingpp.h>
#include <cstdlib>

int main()
{
	try {
		Ming_setCubicThreshold(10);
		SWFMovie *m = new SWFMovie(8);
		SWFShape *shape1 = new SWFShape();
		SWFFill* f = shape1->addSolidFill(255, 0, 0);
		shape1->setRightFill(f);
		shape1->movePenTo(167.000000, 69.875040);
		shape1->drawCubicTo(198.796531,69.875040,
			224.624960,95.703469,224.624960,127.500000);
		shape1->drawCubicTo(224.624960,159.296531,
			198.796531,185.124960,167.000000,185.124960);
		shape1->drawCubicTo(135.203469,185.124960,
			109.375040,159.296531,109.375040,127.500000);
		shape1->drawCubicTo(109.375040,95.703469,
			135.203469,69.875040,167.000000,69.875040);
		m->add(shape1);
		m->save("test02.swf");
	}
	catch(SWFException &e)
        {
                std::cerr << "SWFException: " << e.what() << std::endl << std::endl;
                return EXIT_FAILURE;
        }
        return 0;
}
