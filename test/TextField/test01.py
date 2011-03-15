#!/usr/bin/python
from ming import *

import sys
srcdir=sys.argv[1]
mediadir=srcdir+'/../Media'

m =  SWFMovie();

f2 =  SWFFont(mediadir + "/test.ttf" );

tf =  SWFTextField(SWFTEXTFIELD_NOEDIT);
tf.setFont(f2);
tf.setHeight(20);
tf.setColor(0x00, 0x00, 0x00, 0xff);
tf.addString('The quick brown fox jumps over the lazy dog. 1234567890');

i1 = m.add(tf);
i1.setDepth(1);

m.nextFrame(); # end of frame 1 

m.save("test01.swf");
