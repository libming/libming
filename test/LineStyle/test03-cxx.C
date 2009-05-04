#include <mingpp.h>
#include <cstdlib>


int main()
{
    try {
        SWFMovie *m = new SWFMovie(8);
        SWFShape *shape = new SWFShape();
        SWFShape *shape2 = new SWFShape();
        SWFBitmap *bmp = new SWFBitmap(MEDIADIR"/image01.png");
        SWFGradient *g = new SWFGradient();

        g->addEntry(0.0, 0xff, 0xff, 0, 0x88);
        g->addEntry(0.3, 0, 0xff, 0, 0x33);
        g->addEntry(0.7, 0, 0, 0xff, 0x33);
        g->addEntry(1.0, 0xff, 0, 0, 0x88);

        SWFFillStyle *fill =  SWFFillStyle::BitmapFillStyle(bmp,
	                                       SWFFILL_TILED_BITMAP);
        SWFFillStyle *fill2 = SWFFillStyle::GradientFillStyle(g,
	                                     SWFFILL_LINEAR_GRADIENT);


        shape->setLine2Filled(40, fill, SWF_LINESTYLE_FLAG_HINTING |
	      SWF_LINESTYLE_JOIN_BEVEL | SWF_LINESTYLE_FLAG_ENDCAP_SQUARE, 0);
        shape->movePenTo(5, 5);
        shape->drawLineTo(50, 100);
        shape->drawLineTo(100, 100);

        m->add(shape);

        shape2->setLine2Filled(10, fill2, SWF_LINESTYLE_FLAG_HINTING |
	       SWF_LINESTYLE_JOIN_ROUND | SWF_LINESTYLE_FLAG_ENDCAP_ROUND, 0);
        shape2->movePenTo(125, 125);
        shape2->drawLineTo(150, 100);
        shape2->drawLineTo(200, 200);
        shape2->drawLineTo(250, 100);

        m->add(shape2);
        m->save("test03.swf");
    }
    catch(SWFException &e)
    {
        std::cerr << "SWFException: " << e.what() << std::endl << std::endl;
        return EXIT_FAILURE;
    }
    return 0;
}

