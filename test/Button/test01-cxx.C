#include <mingpp.h>
#include <cstdlib>

int main()
{
	try {
		SWFDisplayItem *d;
		SWFMovie *m = new SWFMovie();
		SWFButton *b = new SWFButton();
		SWFShape *s = new SWFShape();
		char buf[512];
	
		sprintf(buf, "_root.gotoAndStop(2);");

		s->setLine(2, 255, 0, 0, 255);
		s->drawCircle(20);
	
		b->addCharacter(s, SWFBUTTON_HIT|SWFBUTTON_UP|SWFBUTTON_DOWN|SWFBUTTON_OVER);
	
		b->addAction(new SWFAction(buf), SWFBUTTON_MOUSEUP);	

		d = m->add(b);
		m->add(new SWFAction("_root.stop();"));
		d->moveTo(100, 100);
		m->nextFrame();
	
		d->remove();
		b = new SWFButton();
		sprintf(buf, "_root.gotoAndStop(1);");
		b->addCharacter(s, SWFBUTTON_HIT|SWFBUTTON_UP|SWFBUTTON_DOWN|SWFBUTTON_OVER);
	
		b->addAction(new SWFAction(buf), SWFBUTTON_MOUSEDOWN);
		d = m->add(b);
		d->moveTo(50, 100);

		m->nextFrame();
		m->save("test01.swf");
	}
	catch(SWFException &e)
	{
		std::cerr << "SWFException: " << e.what() << std::endl << std::endl;
		return EXIT_FAILURE;
	}
	return 0;
}
