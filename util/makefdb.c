#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "blocktypes.h"
#include "read.h"

void readRect(FILE *f)
{
  int nBits = readBits(f, 5);
  readBits(f, nBits);
  readBits(f, nBits);
  readBits(f, nBits);
  readBits(f, nBits);
}

#define FONTINFO2_HASLAYOUT		(1<<7)
#define FONTINFO2_SHIFTJIS		(1<<6)
#define FONTINFO2_UNICODE		(1<<5)
#define FONTINFO2_ANSI			(1<<4)
#define FONTINFO2_WIDEOFFSETS	(1<<3)
#define FONTINFO2_WIDECODES		(1<<2)
#define FONTINFO2_ITALIC		(1<<1)
#define FONTINFO2_BOLD			(1<<0)

void printDefineFont2(FILE *f, int length)
{
  int flags, namelen, i, reserved;
  char name[264];
  FILE *out;
  int wide = 0;

  readUInt16(f); /* font id */

  flags = readUInt8(f);

  if(flags & FONTINFO2_WIDECODES) {
#if 1
    wide = 1;
    warning("Sorry, wide code fonts not supported.");
#else
    error("Sorry, wide code fonts not supported.");
#endif
  }

  if(!(flags & FONTINFO2_HASLAYOUT))
    error("Hrm.  I was expecting some layout info."); 

  reserved = readUInt8(f); /* "reserved" */

  namelen = readUInt8(f);

  for(i=0; i<namelen; ++i)
    name[i] = (unsigned char)readUInt8(f);

  if(flags & FONTINFO2_BOLD)
  {
    name[i++] = '-';
    name[i++] = 'B';
  }
  if(flags & FONTINFO2_ITALIC)
  {
    name[i++] = '-';
    name[i++] = 'I';
  }

  name[i++] = '.';
  name[i++] = 'f';
  name[i++] = 'd';
  name[i++] = 'b';
  name[i] = '\0';

  printf("Writing %s, %i bytes\n", name, length+2);

  if((out=fopen(name,"wb"))==NULL)
    error("Couldn't open output file!");

  fputc('f', out);
  fputc('d', out);
  fputc('b', out);
  fputc('0', out);

  fputc(flags, out);
  fputc(reserved, out);
  fputc(namelen, out);

  for(i=0; i<namelen; ++i)
    fputc(name[i], out);

  length -= namelen + 5;

  for(; length>0; --length)
    fputc(fgetc(f), out); /* prolly faster buffered, eh? */

  fclose(out);

  if(wide)
  {
    printf("Removing %s\n", name);
    unlink(name);
  }
}

int main(int argc, char *argv[])
{
  FILE *f;
  int size, offset, block, type, length, i;

  if(argc<2)
    error("Give me a filename.\n");

  if(!(f = fopen(argv[1],"rb")))
    error("Sorry, can't seem to read that file.\n");

  if(!(readUInt8(f)=='F' && readUInt8(f)=='W' && readUInt8(f)=='S'))
    error("Doesn't look like a flash file to me..\n");

  readUInt8(f); /* version */
  size = readUInt32(f);
  readRect(f); /* bounds */
  readUInt16(f); /* frame rate */
  readUInt16(f); /* total frames */

  for(;;)
  {
    block = readUInt16(f);
    type = block>>6;
	offset = fileOffset;

    length = block & ((1<<6)-1);

    if(length == 63) /* it's a long block. */
      length = readUInt32(f);

    if(type == 0 || fileOffset > size)
      break;

    switch(type)
    {
      case DEFINEFONT2:
	    printf("Found DefineFont2 block at offset %i\n", offset);
	    printf("Block length: %i\n", length);
      	printDefineFont2(f, length);
      	break;

      default:
      	for(i=length; i>0; --i) readUInt8(f);
      	break;
    }
  }

  return 0;
}
