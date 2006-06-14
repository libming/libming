#!/usr/bin/python
from ming import *

Ming_useSWFVersion(6);

m =  SWFMovie();


#  SWF_SETBACKGROUNDCOLOR 
m.setBackground(0xff, 0xff, 0xff);
m.nextFrame();  # end of frame 1

m.save("test04.swf");
