MING_VER = 0.3beta1

OS             = $(shell uname -s)

CC             = gcc
CXX	       = g++

INCLUDES       := -I. $(INCLUDES)
LDLIBS         = $(LIBS)

STATICLIB      = libming.a
SHAREDLIB      = libming.so.$(MING_VER)

CFLAGS  += -g -Wall $(INCLUDES) 

SHLIBLDFLAGS   = -static

# Uncomment the following lines to compile for Darwin
#override CFLAGS         += -fno-common
#SHLIBLDFLAGS            = -dynamiclib
#SHAREDLIB                       = libming.$(MING_VER).dylib
