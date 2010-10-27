#!/usr/bin/python
from ming import *

m =  SWFMovie();


# SWF_SETBACKGROUNDCOLOR 
m.setBackground(0xff, 0xff, 0xff);

# SWF_DEFINESPRITE 

	#  MovieClip 1 
character1 =  SWFMovieClip(); # 1 frames 

# SWF_SHOWFRAME 
character1.nextFrame(); # end of clip frame 1 

# SWF_END 

# SWF_PLACEOBJECT2 
# PlaceFlagHasCharacter 
m.add(character1);
# PlaceFlagHasMatrix 
# outputSWF_MATRIX is broken, so it is being skipped.. 

# SWF_SHOWFRAME 
m.nextFrame(); # end of frame 1 

# SWF_END 

m.save("test02.swf");
