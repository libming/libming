#!/usr/bin/python
from ming import *

import sys
mediadir=sys.argv[1]+'/../Media'

m =  SWFMovie();

f =  SWFFont(mediadir + '/font01.fdb' )

text =  SWFText(2);
text.setFont(f);
text.setColor(0, 0, 0, 0xff);
text.setHeight(20);
text.addString("abc");

m.add(text);

m.save("test01.swf");
