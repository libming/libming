#!/usr/bin/python
from ming import *

Ming_useSWFVersion(7);

m =  SWFMovie();


#  SWF_SETBACKGROUNDCOLOR 
m.setBackground(0xff, 0xff, 0xff);
m.nextFrame(); #  end of frame 1 

m.save("test05.swf");
