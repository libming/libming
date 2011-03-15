#!/usr/bin/python
from ming import *

import sys
mediadir = sys.argv[1]+'/../Media'

m =  SWFMovie();

font =  SWFFont(mediadir + "/font01.fdb")

text =  SWFText(2);
text.setFont(font);
text.setHeight(20);
text.setColor(0x00, 0x00, 0x00, 0xff);
text.addString("abc");

font2 =  SWFFont(mediadir + "/test.ttf")

text.setFont(font2);
text.setHeight(40);
text.setColor(0xff, 0x00, 0x00, 0xff);
text.addString("def");

m.add(text);

m.save("test02.swf");
