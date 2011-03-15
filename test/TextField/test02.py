#!/usr/bin/python
from ming import *

import sys
srcdir=sys.argv[1]
mediadir=srcdir+'/../Media'

m =  SWFMovie();

f2 =  SWFFont(mediadir + "/test.ttf" );

tf =  SWFTextField();
tf.setFont(f2);
tf.setHeight(20);
tf.setColor(0x00, 0x00, 0x00, 0xff);

i1 = m.add(tf);

m.nextFrame(); 

m.save("test02.swf");
