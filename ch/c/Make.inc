###################################################################### 
#
# Makefile fo building the dynamically loaded lib libming.dl
# 
###################################################################### 

###################################################################### 
# Variables

LIB_NAME = libming
TARGET = $(LIB_NAME).dl
LNK    = ch dllink $(TARGET)
CC     = ch dlcomp $(TARGET)

######################################################################
# Objects 

OBJS  =  ming_chdl.$(OBJ)
OBJS2 =  SWFMovie_output_chdl.$(OBJ) \
         Ming_setWarnFunction_chdl.$(OBJ) \
         Ming_setErrorFunction_chdl.$(OBJ)

######################################################################
# Targets

all: $(TARGET)

$(TARGET):  $(OBJS) $(OBJS2)
	$(LNK) $(OBJS) $(OBJS2) $(LFLAGS)

$(OBJS): 
	$(CC) $(CFLAGS) -c $*.c $(IFLAGS)

$(OBJS2): 
	$(CC) $(CFLAGS) -c handmade/$*.c $(IFLAGS)

clean:
	rm -f *.o *.obj *.a *.dll *.exp *.lib logfile

######################################################################
