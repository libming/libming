/* imports.h
 * 
 * $Id$
 * 
 * Notice: This header file contains declarations of functions and types that
 * are just used internally. All library functions and types that are supposed
 * to be publicly accessable are defined in ./src/ming.h.
 */

#ifndef SWF_IMPORTS_H_INCLUDED
#define SWF_IMPORTS_H_INCLUDED

#include "ming.h"
#include "block.h"

struct importitem
{	struct importitem *next;
	int id;
	char *name;
};
struct SWFImportBlock_s
{
	struct SWFBlock_s block;
	char *filename;
	struct importitem *importlist;
};
typedef struct SWFImportBlock_s *SWFImportBlock;

SWFImportBlock newSWFImportBlock(const char *filename);

#endif
