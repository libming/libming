
#ifndef COMPILER_H_INCLUDED
#define COMPILER_H_INCLUDED

#include <stdio.h>
#include "../blocks/output.h"
#include "../blocks/action.h"
#include "action.h"

/* shut up bison.simple */
void yyerror(char *msg);
int yylex();

#ifndef max
  #define max(x,y)	(((x)>(y))?(x):(y))
#endif

typedef enum
{
  FUNCTION_RANDOM,
  FUNCTION_LENGTH,
  FUNCTION_TIME,
  FUNCTION_INT,
  FUNCTION_CONCAT,
  FUNCTION_DUPLICATECLIP
} SWFActionFunction;

typedef enum
{
  GETURL_METHOD_NOSEND = 0,
  GETURL_METHOD_GET    = 1,
  GETURL_METHOD_POST   = 2
} SWFGetUrl2Method;

#define GETURL_LOADMOVIE 0x40
#define GETURL_LOADVARIABLES 0x80

#define MAGIC_CONTINUE_NUMBER 0x7FFE
#define MAGIC_BREAK_NUMBER    0x7FFF

#define MAGIC_CONTINUE_NUMBER_LO 0xFE
#define MAGIC_CONTINUE_NUMBER_HI 0x7F
#define MAGIC_BREAK_NUMBER_LO    0xFF
#define MAGIC_BREAK_NUMBER_HI    0x7F

#define SETPROPERTY_WTHIT_STRING "18048"

#define BUFFER_INCREMENT 128

struct _buffer
{
  byte *buffer;
  byte *pos;
  int buffersize;
  int free;
};
typedef struct _buffer *Buffer;

#define BUFFER_SIZE sizeof(struct _buffer)

/* This is the only function needs be visible: */
SWFAction compileSWFActionCode(char *script);

/* blah */
void stringPrepend(char *a, char *b);

/* create/destroy buffer object */
Buffer newBuffer();
void destroyBuffer(Buffer out);
void bufferConcat(Buffer a, Buffer b);     /* destroys b. */
void bufferWriteBuffer(Buffer a, Buffer b);   /* doesn't. */

/* write buffer's buffer to stream */
void bufferWriteToStream(Buffer out, FILE *f);

/* utilities for writing */
void bufferGrow(Buffer out);
Buffer bufferCheckSize(Buffer out, int bytes);

int bufferLength(Buffer out);

/* write data to buffer */
void bufferWriteU8(Buffer out, int data);
void bufferWriteS16(Buffer out, int data);
void bufferWriteData(Buffer out, byte *buffer, int bytes);
void bufferWriteString(Buffer out, byte *string, int length);
void bufferWriteHardString(Buffer out, byte *string, int length);
void bufferWriteGetProperty(Buffer out, char *string);
void bufferWriteSetProperty(Buffer out, char *string);
void bufferWriteWTHITProperty(Buffer out);

/* concat b to a, destroy b */
char *stringConcat(char *a, char *b);

/* duplicate and triplicate what's on top of the stack.
   uses temp var "_tmp" */
void bufferDup(Buffer out);
void bufferTrip(Buffer out);

/* resolve magic number standins to relative offsets */
void resolveJumps(Buffer out);

#endif /* COMPILER_H_INCLUDED */
