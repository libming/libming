#!/usr/bin/python
from ming import *

m =  SWFMovie();

m.setDimension(640.000000, 480.000000);

#  SWF_SETBACKGROUNDCOLOR 
m.setBackground(0xff, 0xff, 0xff);
m.nextFrame();  # end of frame 1

m.save("test01.swf");
