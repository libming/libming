#!/usr/bin/python
from ming import *

Ming_useSWFVersion(8);
m =  SWFMovie();

s =  SWFShape();

f = s.addSolidFill(255,0,0);
s.setRightFill(f);

Ming_setCubicThreshold(10);

s.movePenTo(167.000000, 69.875040);
s.drawCubicTo(198.796531,69.875040,224.624960,95.703469,224.624960,127.500000);
s.drawCubicTo(224.624960,159.296531,198.796531,185.124960,167.000000,185.124960);
s.drawCubicTo(135.203469,185.124960,109.375040,159.296531,109.375040,127.500000);
s.drawCubicTo(109.375040,95.703469,135.203469,69.875040,167.000000,69.875040);



i1 = m.add(s);
i1.setDepth(1);

m.nextFrame(); # end of frame 1 

m.save("test02.swf");
