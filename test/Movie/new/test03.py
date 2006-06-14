#!/usr/bin/python
from ming import *

m =  SWFMovie();


#  SWF_SETBACKGROUNDCOLOR 
m.setBackground(0xff, 0xff, 0xff);
m.nextFrame();  # end of frame 1

m.save("test03.swf");
