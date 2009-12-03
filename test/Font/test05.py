#!/usr/bin/python

from ming import *
import sys

srcdir=sys.argv[1]

m =  SWFMovie();

font = SWFFont(srcdir + "/../Media/font01.fdb")

t = SWFText(1)
t.setFont(font)
t.setColor(0,0,255,255)
t.setHeight(20)
t.moveTo(100, 100)
t.addString("Static Text")

tf = SWFTextField()
tf.setFlags(SWFTEXTFIELD_NOEDIT);
tf.setFont(font)
tf.setColor(255,0,0,255)
tf.setHeight(20)
tf.addString("Readonly Textfield")

m.add(t);
i = m.add(tf);
i.moveTo(100, 120);

m.nextFrame(); 

m.save("test05.swf");
