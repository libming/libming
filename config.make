# set to your choice of png and / or gif formats
EXTRA_OBJS = gifdbl.o pngdbl.o

# zlib required for both image formats and for compressed swf6
EXTRA_LIBS = -lz -lungif -lpng12
