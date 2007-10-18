#!/usr/bin/python
from ming import *

Ming_useSWFVersion(8);

m =  SWFMovie();
m.setBackground(0, 0, 0);
m.save("test07.swf");
