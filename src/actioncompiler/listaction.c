#include <stdlib.h>
#include <stdio.h>

#include "action.h"
#include "compile.h"

#define print(x)	{fputs(x,stdout);}

int fileOffset = 0;

int readUInt8(Buffer f)
{
  return f->buffer[fileOffset++];
}

int readSInt8(Buffer f)
{
  return (signed char)readUInt8(f);
}

int readSInt16(Buffer f)
{
  return readUInt8(f) + readSInt8(f)*256;
}

int readUInt16(Buffer f)
{
  return readUInt8(f) + (readUInt8(f)<<8);
}

long readSInt32(Buffer f)
{
  return (long)readUInt8(f) + (readUInt8(f)<<8) + (readUInt8(f)<<16) + (readUInt8(f)<<24);
}

unsigned long readUInt32(Buffer f)
{
  return (unsigned long)(readUInt8(f) + (readUInt8(f)<<8) + (readUInt8(f)<<16) + (readUInt8(f)<<24));
}

char *readString(Buffer f)
{
  int len = 0, buflen = 256;
  char c, *buf, *p;

  buf = (char *)malloc(sizeof(char)*256);
  p = buf;

  while((c=(char)readUInt8(f)) != '\0')
  {
    if(len==buflen)
    {
      buf = (char *)realloc(buf, sizeof(char)*(buflen+256));
      buflen += 256;
      p = buf+len;
    }

    *(p++) = c;
    ++len;
  }

  *p = 0;

  return buf;
}

void dumpBytes(Buffer f, int length)
{
  int j=0, i, k;
  unsigned char buf[16];

  if(length==0)
    return;

  for(;;)
  {
    for(i=0; i<16; ++i)
    {
      printf("%02x ", buf[i] = readUInt8(f));
      ++j;

      if(j==length)
		break;
    }

    if(j==length)
    {
      for(k=i+1; k<16; ++k)
	print("   ");

      ++i;
    }

    print("   ");

    for(k=0; k<i; ++k)
      if((buf[k] > 31) && (buf[k] < 128))
	putchar(buf[k]);
      else
	putchar('.');

    putchar('\n');

    if(j==length)
      break;
  }
  putchar('\n');
  putchar('\n');
}

int printActionRecord(Buffer f)
{
  int length = 0, type;

  printf("(%i)\t", fileOffset);

  type = readUInt8(f);

  if((type&0x80) == 0x80)
    length = readUInt16(f);

  switch(type)
  {
    case SWFACTION_ADD:
      printf("Add\n");
      break;
    case SWFACTION_SUBTRACT:
      printf("Subtract\n");
      break;
    case SWFACTION_MULTIPLY:
      printf("Multiply\n");
      break;
    case SWFACTION_DIVIDE:
      printf("Divide\n");
      break;
    case SWFACTION_EQUAL:
      printf("Equals\n");
      break;
    case SWFACTION_LESSTHAN:
      printf("Less Than\n");
      break;
    case SWFACTION_LOGICALAND:
      printf("And\n");
      break;
    case SWFACTION_LOGICALOR:
      printf("Or\n");
      break;
    case SWFACTION_LOGICALNOT:
      printf("Not\n");
      break;
    case SWFACTION_STRINGEQ:
      printf("String eq\n");
      break;
    case SWFACTION_STRINGLENGTH:
      printf("String Length\n");
      break;
    case SWFACTION_SUBSTRING:
      printf("Substring\n");
      break;
    case SWFACTION_INT:
      printf("Int\n");
      break;
    case SWFACTION_GETVARIABLE:
      printf("Get Variable\n");
      break;
    case SWFACTION_SETVARIABLE:
      printf("Set Variable\n");
      break;
    case SWFACTION_SETTARGETEXPRESSION:
      printf("Set Target Expression\n");
      break;
    case SWFACTION_STRINGCONCAT:
      printf("String Concat\n");
      break;
    case SWFACTION_GETPROPERTY:
      printf("Get Property\n");
      break;
    case SWFACTION_SETPROPERTY:
      printf("Set Property\n");
      break;
    case SWFACTION_DUPLICATECLIP:
      printf("Duplicate Clip\n");
      break;
    case SWFACTION_REMOVECLIP:
      printf("Remove Clip\n");
      break;
    case SWFACTION_TRACE:
      printf("Trace\n");
      break;
    case SWFACTION_STARTDRAGMOVIE:
      printf("Start Drag Movie\n");
      break;
    case SWFACTION_STOPDRAGMOVIE:
      printf("Stop Drag Movie\n");
      break;
    case SWFACTION_STRINGCOMPARE:
      printf("String Compare\n");
      break;
    case SWFACTION_RANDOM:
      printf("Random\n");
      break;
    case SWFACTION_MBLENGTH:
      printf("String MB Length\n");
      break;
    case SWFACTION_ORD:
      printf("Ord\n");
      break;
    case SWFACTION_CHR:
      printf("Chr\n");
      break;
    case SWFACTION_GETTIMER:
      printf("Get Timer\n");
      break;
    case SWFACTION_MBSUBSTRING:
      printf("MB Substring\n");
      break;
    case SWFACTION_MBORD:
      printf("MB Ord\n");
      break;
    case SWFACTION_MBCHR:
      printf("MB Chr\n");
      break;
    case SWFACTION_NEXTFRAME:
      printf("Next Frame\n");
      break;
    case SWFACTION_PREVFRAME:
      printf("Previous Frame\n");
      break;
    case SWFACTION_PLAY:
      printf("Play\n");
      break;
    case SWFACTION_STOP:
      printf("Stop\n");
      break;
    case SWFACTION_TOGGLEQUALITY:
      printf("Toggle Quality\n");
      break;
    case SWFACTION_STOPSOUNDS:
      printf("Stop Sounds\n");
      break;

    /* ops with args */
    case SWFACTION_PUSHDATA:
    {
      int type = readUInt8(f);
      if(type==0)
        printf("Push String: %s\n", readString(f));
      else
      {
	readUInt16(f); /* always 0..? */
        printf("Push Property: %04x\n", readUInt16(f));
      }
      break;
    }
    case SWFACTION_GOTOFRAME:
      printf("Goto Frame %i\n", readUInt16(f));
      break;
    case SWFACTION_GETURL:
    {
      char *url = readString(f);
      printf("Get URL \"%s\" target \"%s\"\n", url, readString(f));
      break;
    }
    case SWFACTION_WAITFORFRAMEEXPRESSION:
      printf("Wait For Frame Expression\n");
      dumpBytes(f, length);
      break;
    case SWFACTION_BRANCHALWAYS:
      printf("Branch Always %i\n", readSInt16(f));
      break;
    case SWFACTION_GETURL2:
      switch(readUInt8(f))
      {
        case 0: printf("Get URL2 (Don't send)\n"); break;
        case 1: printf("Get URL2 (GET)\n"); break;
        case 2: printf("Get URL2 (POST)\n"); break;
      }
      break;
    case SWFACTION_BRANCHIFTRUE:
      printf("Branch If True %i\n", readSInt16(f));
      break;
    case SWFACTION_CALLFRAME:
      printf("Call Frame\n");
      dumpBytes(f, length);
      break;
    case SWFACTION_GOTOEXPRESSION:
      printf("Goto Expression ");

      if(readUInt8(f) == 1)
	printf("and Play\n");
      else
	printf("and Stop\n");

      break;
    case SWFACTION_WAITFORFRAME:
    {
      int frame = readUInt16(f);
      printf("Wait for frame %i else skip %i\n", frame, readUInt8(f));
      break;
    }
    case SWFACTION_SETTARGET:
      printf("Set Target %s\n", readString(f));
      break;
    case SWFACTION_GOTOLABEL:
      printf("Goto Label %s\n", readString(f));
      break;

    case SWFACTION_END:
      printf("End\n");
      return 0;
      break;
    default:
      printf("Unknown Action: %02X\n", type);
      dumpBytes(f, length);
  }
  return 1;
}

void printDoAction(Buffer f)
{
  int end;

  if(!f)
    return;

  end = bufferLength(f);
  fileOffset = 0;

  while(printActionRecord(f) && fileOffset < end) ;
}
