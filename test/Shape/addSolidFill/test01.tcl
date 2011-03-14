load mingc.so mingc
set m [newSWFMovie]
#set m [newSWFMovie](8);

#add setscale here
#Unknown block type 69

# SWF_DEFINESHAPE3 

# Shape 1 (TYPE=3, RECT=0,0 0,0)
set character1 [newSWFShape]();
#2 fillstyle(s)
$character1_f0_red   = 0x20;
$character1_f0_green = 0x40;
$character1_f0_blue  = 0x80;
$character1_f0_alpha = 0xdf;
SWFFill* character1_f0 = addSolidFill $character1 ($character1_f0_red, $character1_f0_green, $character1_f0_blue, $character1_f0_alpha ); #SWFFILL_SOLID
$character1_f1_red   = 0x40;
$character1_f1_green = 0x80;
$character1_f1_blue  = 0x20;
$character1_f1_alpha = 0x18;
SWFFill* character1_f1 = addSolidFill $character1 ($character1_f1_red, $character1_f1_green, $character1_f1_blue, $character1_f1_alpha ); #SWFFILL_SOLID
#0 linestyles(s)

# SWF_PLACEOBJECT2 
SWFDisplayItem* i1 = add $m ($character1)
setDepth $i1 (1)
# PlaceFlagHasMatrix 

# SWF_SHOWFRAME 
nextFrame $m  # end of frame 1 

# SWF_END 

save $m "test01.swf"
