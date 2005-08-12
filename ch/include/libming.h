/*
    Ming, an SWF output library
    Copyright (C) 2001  Opaque Industries - http://www.opaque.net/

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/* libming.h - global typedefs, etc.
 * 
 * $Id$
 * 
 * Notice: This header file contains declarations of functions and types that
 * are just used internally. All library functions and types that are supposed
 * to be publicly accessable are defined in ./src/ming.h.
 */

#ifndef SWF_LIBMING_H_INCLUDED
#define SWF_LIBMING_H_INCLUDED


#include "ming.h"
#include "ming_config.h"

#include "gc.h"

extern int Ming_cubicThreshold;
extern int SWF_versionNum;
extern int SWF_compression;
extern float Ming_scale;
extern void fileOutputMethod(byte b, void *data);


typedef unsigned char BOOL;

#ifndef TRUE
  #define TRUE ((BOOL)1)
#endif

#ifndef FALSE
  #define FALSE ((BOOL)0)
#endif

#ifndef M_PI
  #define M_PI 3.14159265358979f
#endif

#ifndef min
  #define min(a,b) (((a)<(b))?(a):(b))
#endif

#ifndef max
  #define max(a,b) (((a)>(b))?(a):(b))
#endif

#ifndef rint
  #define rint(d) (floor((d)+0.5))
#endif

#ifndef WIN32
  #include <unistd.h>
#endif

#endif /* SWF_LIBMING_H_INCLUDED */
