#!/usr/bin/python
from ming import *
import sys

srcdir=sys.argv[1]

Ming_useSWFVersion(7);
m =  SWFMovie();

stream1 = SWFVideoStream();
m.add(stream1);

stream2 = SWFVideoStream();
m.add(stream2);

m.save("test02.swf");
