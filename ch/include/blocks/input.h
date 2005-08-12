/* input.h
 * 
 * $Id$
 *
 * Notice: This header file contains declarations of functions and types that
 * are just used internally. All library functions and types that are supposed
 * to be publicly accessable are defined in ./src/ming.h.
 */

#ifndef SWF_INPUT_H_INCLUDED
#define SWF_INPUT_H_INCLUDED

#include "ming.h"

int SWFInput_read(SWFInput input, char* buffer, int count);

int SWFInput_getChar(SWFInput input);
int SWFInput_getUInt16(SWFInput input);
int SWFInput_getUInt16_BE(SWFInput input);
int SWFInput_getSInt16(SWFInput input);
unsigned long SWFInput_getUInt24_BE(SWFInput input);
unsigned long SWFInput_getUInt32(SWFInput input);
unsigned long SWFInput_getUInt32_BE(SWFInput input);

#endif /* SWF_INPUT_H_INCLUDED */
