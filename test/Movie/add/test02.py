#!/usr/bin/python
from ming import *

m =  SWFMovie();


# SWF_SETBACKGROUNDCOLOR 
m.setBackground(0xff, 0xff, 0xff);

# SWF_DEFINESPRITE 

	#  MovieClip 1 
sp1 =  SWFMovieClip(); # 1 frames 

# SWF_SHOWFRAME 
sp1.nextFrame(); # end of clip frame 1 

# SWF_END 

# SWF_PLACEOBJECT2 
# PlaceFlagHasCharacter 
sp1.add(s1);
# PlaceFlagHasMatrix 
sp1.scaleTo(0.000000, nan);
sp1.rotateTo(90.000000);

# SWF_SHOWFRAME 
m.nextFrame(); # end of frame 1 

m.save("test02.swf");
