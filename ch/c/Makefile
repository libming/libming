###################################################################### 
#
# Unix Makefile for building the dynamically loaded lib libming.dl
# 
###################################################################### 


###################################################################### 
# Unix Specific Variables
###################################################################### 

OBJ    = o
IFLAGS = -I../../src/ 
# If libgif or libpng were not used during the compilation of libming,
# please remove it from the following line
LFLAGS = -L../.. -L/sw/lib -lming -lgif -lpng -lz


###################################################################### 
# Include Make.inc
###################################################################### 

include Make.inc
