
/* generic input object.  Wraps files, buffers and streams, replaces
   standard file funcs */

#ifndef SWF_INPUT_H_INCLUDED
#define SWF_INPUT_H_INCLUDED

#include "libswf.h"

struct _swfInput
{
  void (*destroy)(struct _swfInput *this);
  int (*getChar)(struct _swfInput *this);
  void (*seek)(struct _swfInput *this, long offset, int whence);
  int (*eof)(struct _swfInput *this);

  int offset;
  int length;
  void *data;
};
typedef struct _swfInput *SWFInput;


int SWFInput_length(SWFInput input);
void SWFInput_rewind(SWFInput input);
int SWFInput_tell(SWFInput input);
void SWFInput_seek(SWFInput input, long offset, int whence);
int SWFInput_eof(SWFInput input);

int SWFInput_getChar(SWFInput input);
int SWFInput_getUInt16(SWFInput input);
int SWFInput_getUInt16_BE(SWFInput input);
int SWFInput_getSInt16(SWFInput input);
unsigned long SWFInput_getUInt32(SWFInput input);
unsigned long SWFInput_getUInt32_BE(SWFInput input);

SWFInput newSWFInput_file(FILE *f);
SWFInput newSWFInput_stream(FILE *f);
SWFInput newSWFInput_buffer(unsigned char *buffer, int length);
SWFInput newSWFInput_allocedBuffer(unsigned char *buffer, int length);

void destroySWFInput(SWFInput input);

#endif /* SWF_INPUT_H_INCLUDED */
