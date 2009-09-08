#!/usr/bin/python
from ming import *
import sys

srcdir=sys.argv[1]

Ming_useSWFVersion(7);
m = SWFMovie();
m.setDimension(200, 150);
m.setRate(1);

stream1 = SWFVideoStream();
stream1.setDimension(200, 150);
it = m.add(stream1);
it.setName("video");

action = SWFAction(''
'	nc = new NetConnection ();'
'	nc.connect (null);'
'	ns = new NetStream (nc);'
'	video.attachVideo (ns);'
'	ns.play ("video.flv");'
);

m.add(action);

m.save("test03.swf");
