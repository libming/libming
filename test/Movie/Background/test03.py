#!/usr/bin/python
from ming import *

m =  SWFMovie();


# SWF_SETBACKGROUNDCOLOR 
m.setBackground(0x00, 0x00, 0xff);

# SWF_SHOWFRAME 
m.nextFrame(); # end of frame 1 

# SWF_END 

m.save("test03.swf");
