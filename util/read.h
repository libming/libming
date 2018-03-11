#ifndef SWF_READ_H_INCLUDED
#define SWF_READ_H_INCLUDED

extern int bufbits;
extern int fileOffset;

typedef unsigned char boolean;
#define true (boolean)1
#define false (boolean)0

char *indent();
void byteAlign();
int readBits(FILE *f, int number);
int readSBits(FILE *f, int number);
int readUInt8(FILE *f);
int readSInt8(FILE *f);
unsigned int readUInt16(FILE *f);
int readSInt16(FILE *f);
unsigned long readUInt32(FILE *f);
long readSInt32(FILE *f);
unsigned long readEncUInt32(FILE *f);
unsigned long readEncUInt30(FILE *f);
long readEncSInt32(FILE *f);
char *readBytes(FILE *f,unsigned long size);
char *readString(FILE *f);
char *readSizedString(FILE *f,int size);
double readDouble(FILE *f);
float readFloat(FILE *f);

struct Rect; // forward declaration avoids including parser.h here
void readRect(FILE *f, struct Rect *s);

void dumpBytes(FILE *f, int length);
void peekBytes(FILE *f, int length);
void dumpBuffer(unsigned char *buf, int length);
void silentSkipBytes(FILE *f, int length);

void dumpBuffer(unsigned char *buf, int length);

#endif /* SWF_READ_H_INCLUDED */
