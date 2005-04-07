#ifndef SWF_READ_H_INCLUDED
#define SWF_READ_H_INCLUDED

extern int gIndent;
extern int fileOffset;

typedef unsigned char boolean;
#define true (boolean)1
#define false (boolean)0

void error(const char *s, ...);
void warning(const char *s, ...);
char *indent();
void byteAlign();
int readBits(FILE *f, int number);
int readSBits(FILE *f, int number);
int readUInt8(FILE *f);
int readSInt8(FILE *f);
int readUInt16(FILE *f);
int readSInt16(FILE *f);
int readUInt32(FILE *f);
int readSInt32(FILE *f);
char *readString(FILE *f);
double readDouble(FILE *f);
void dumpBytes(FILE *f, int length);

void dumpBuffer(unsigned char *buf, int length);

#endif /* SWF_READ_H_INCLUDED */
