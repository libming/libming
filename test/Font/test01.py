#!/usr/bin/python
from ming import *
import sys

srcdir=sys.argv[1]

m =  SWFMovie();

font = SWFFont(srcdir + "/../Media/font01.fdb")
text = SWFText(1)

text.setFont(font)
text.setColor(0,0,0,255)
text.setHeight(10)
text.moveTo(10, 100)
text.addString("The quick brown fox jumps over the lazy dog. 1234567890")

m.add(text)
m.nextFrame()
m.save("test01.swf")
