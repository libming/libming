
#ifndef ASSEMBLER_H_INCLUDED
#define ASSEMBLER_H_INCLUDED

#include "compile.h"

extern Buffer asmBuffer;

void bufferPatchLength(Buffer buffer, int len);
int bufferBranchTarget(Buffer buffer, char *label);

void addLabel(char *label);
int findLabel(char *label);

#endif /* ASSEMBLER_H_INCLUDED */
