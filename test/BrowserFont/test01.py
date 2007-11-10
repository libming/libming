#!/usr/bin/python
from ming import *

m = SWFMovie()
text = SWFTextField()
font = SWFBrowserFont("_sans")
text.setFont(font)
text.setColor(0,0,0,0xff)
text.setHeight(20.0)
text.addString("abc")
m.add(text)
m.nextFrame()
m.save("test01.swf")
