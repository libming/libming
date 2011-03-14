#!/usr/bin/python
from ming import *

Ming_useSWFVersion(8);

m =  SWFMovie();

# SWF_DEFINESHAPE3 

# Shape 1 (TYPE=3, RECT=0,0 0,0)
character1 =  SWFShape();
#0 fillstyle(s)
#0 linestyles(s)

# SWF_PLACEOBJECT2 
i1 = m.add(character1);
i1.setDepth(1);
# PlaceFlagHasMatrix 

# SWF_DEFINESHAPE4 

# Shape 2 (TYPE=4, RECT=0,0 0,0)
character2 =  SWFShape();

character2.useVersion(SWF_SHAPE4);

#0 fillstyle(s)
#0 linestyles(s)

# SWF_PLACEOBJECT2 
i2 = m.add(character2);
i2.setDepth(2);
# PlaceFlagHasMatrix 

# SWF_SHOWFRAME 
m.nextFrame(); # end of frame 1 

# SWF_END 

m.save("test01.swf");
