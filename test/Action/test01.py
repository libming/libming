#!/usr/bin/python
from ming import *

m =  SWFMovie()

Ming_setScale(1.0)

a = SWFAction("trace('');")
init = SWFInitAction(a)
m.add( init )

m.save("test01.swf")
