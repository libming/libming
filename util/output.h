/* output.h */

#ifndef OUTPUT_H_INCLUDED
#define OUTPUT_H_INCLUDED

#include "libswf.h"

#define max(x,y)	(((x)>(y))?(x):(y))

#define BUFFER_INCREMENT 1024

struct _output
{
  struct _output *next;
  byte *buffer;
  byte *pos;
  int buffersize;
  int free;
  int bitpos;
};
typedef struct _output *Output;

#define OUTPUT_SIZE sizeof(struct _output)

/* create/destroy output object */
Output newOutput();
Output newSizedOutput(int size);
void destroyOutput(Output out);

/* write output's buffer to stream */
void outputWriteToStream(Output out, FILE *f);

/* utilities for writing */
void outputGrow(Output out);
void outputCheckSize(Output out, int bytes);
void outputByteAlign(Output out);

int outputLength(Output out);

/* write data to output */
void outputWriteBits(Output out, int data, int bits);
void outputWriteSBits(Output out, int data, int bits);
void outputWriteUInt8(Output out, int data);
void outputWriteSInt8(Output out, int data);
void outputWriteUInt16(Output out, int data);
void outputWriteSInt16(Output out, int data);
void outputWriteUInt32(Output out, long data);
void outputWriteSInt32(Output out, long data);
void outputWriteBuffer(Output out, char *buffer, int bytes);

/* number of bits required to store num */
int numBits(int num);
int numSBits(int num);

void outputWriteRect(Output out, int xmin, int xmax, int ymin, int ymax);
void outputWriteString(Output out, byte *string);

#endif /* OUTPUT_H_INCLUDED */
