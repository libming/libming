#include <mingpp.h>
#include <cstdlib>

int main()
{
	try {
		SWFMovie *m = new SWFMovie();
		SWFShape *s = new SWFShape();

		s->setLine(10, 255, 0, 0, 255);
		SWFFillStyle *fill = SWFFillStyle::SolidFillStyle(255,255,0,255);
		s->setRightFillStyle(fill);
		s->movePenTo(-50, -20);
		s->drawLine(100,0);
		s->drawLine(0,40);
		s->drawLine(-100, 0);
		s->drawLineTo(-50 ,-20);

		SWFMovieClip *mc1 = new SWFMovieClip();
		SWFDisplayItem *si1 = mc1->add(s);
		mc1->nextFrame();
		si1->scale(0.8, 0.8);
		mc1->nextFrame();
		si1->scale(0.8, 0.8);
		mc1->nextFrame();
		si1->scale(0.8, 0.8);
		mc1->nextFrame();
		si1->scale(0.8, 0.8);
		mc1->nextFrame();
		si1->scale(0.8, 0.8);
		mc1->nextFrame();


		SWFMovieClip *mc2 = new SWFMovieClip();
		SWFDisplayItem *si2 = mc2->add(s);
		si2->scaleTo(0.2, 0.2);
		mc2->nextFrame();
		si2->scaleTo(0.4, 0.4);
		mc2->nextFrame();
		si2->scaleTo(0.6, 0.6);
		mc2->nextFrame();
		si2->scaleTo(1, 1);
		mc2->nextFrame();

		SWFButton *b = new SWFButton();
		SWFButtonRecord *br1, *br2, *br3;
		br1 = b->addCharacter(s, SWFBUTTON_HIT|SWFBUTTON_UP);
		br2 = b->addCharacter(mc1, SWFBUTTON_OVER);
		br3 = b->addCharacter(mc2, SWFBUTTON_DOWN);

		SWFDisplayItem *d = m->add(b);
		d->moveTo(150, 100);
		m->nextFrame();
		m->save("test03.swf");
	}
	catch(SWFException &e)
	{
		std::cerr << "SWFException: " << e.what() << std::endl << std::endl;
		return EXIT_FAILURE;
	}
	return 0;
}

