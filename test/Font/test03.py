#!/usr/bin/python
from ming import *
import sys

srcdir=sys.argv[1]

m =  SWFMovie();

font = SWFFont(srcdir + "/../Media/test.ttf")
text = SWFText(1)

w = font.getStringWidth("The quick brown fox jumps over the lazy dog. 1234567890")

text.setFont(font)
text.setColor(0,0,0,255)
text.setHeight(20)
text.moveTo(w,0)
text.addString("|")

m.add(text)
m.nextFrame()
m.save("test03.swf")
