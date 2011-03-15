#!/usr/bin/python
from ming import *

import sys
mediadir = sys.argv[1]+'/../Media'

m =  SWFMovie();

f2 =  SWFFont(mediadir + "/font01.fdb")

character1 =  SWFText(2);
character1.setFont(f2);
character1.setHeight(20);
character1.setColor(0x00, 0x00, 0x00, 0xff);
character1.moveTo(10, 20);
character1.addString("abc");
character1.moveTo(0, 40);
character1.addString("bca");
character1.moveTo(60, 0);
character1.addString("cab");

i1 = m.add(character1);

m.nextFrame(); # end of frame 1 

m.save("test03.swf");
