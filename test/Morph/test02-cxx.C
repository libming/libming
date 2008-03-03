#include <mingpp.h>
#include <cstdlib>


int main()
{
	int i;
	try {
		SWFMovie *m = new SWFMovie(8);

		SWFMorph *morph = new SWFMorph();

		SWFShape *shape1 = morph->getShape1();
		SWFShape *shape2 = morph->getShape2();
	
		shape1->setLine2(20, 25, 0, 0, 128, SWF_LINESTYLE_FLAG_HINTING, 0);
		shape1->movePenTo(5, 5);
		shape1->drawLineTo(50, 30);

		shape2->setLine2(20, 25, 100, 100, 255, SWF_LINESTYLE_FLAG_HINTING, 0);
		shape2->movePenTo(5, 5);
		shape2->drawLineTo(50, 130);

		SWFDisplayItem *item = m->add(morph);
		for(i = 0; i < 10; i++)
		{
			item->setRatio(0.1 * i);
			m->nextFrame();
		}	 
		m->save("test02.swf");
	}
	catch (SWFException &e)
	{
		std::cerr << "SWFException: " << e.what() << std::endl << std::endl;
		return EXIT_FAILURE;
	}
	return 0;
}
