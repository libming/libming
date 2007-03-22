/****************************************************************************
 *
 *  makeswf - a command line actionscript compiler
 *
 *  Copyright (C) 2003 2004 2005 2006 "Sandro Santilli" <strk@keybit.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 ****************************************************************************
 * 
 * Utility functions for 'makeswf'
 *
 ***************************************************************************/

/* This is needed to get vasprintf definition */
#define _GNU_SOURCE 1

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <ming.h>
#include "makeswf.h"
#ifdef HAVE_GETOPT
#include <getopt.h>
#endif

// Cheating, but it works (not sure why the above ifdef for getopt isn't)
#ifdef _WIN32
#include <getopt.h>
#endif

#define DEFSWFVERSION 6
#define DEFSWFCOMPRESSION 9

//#define CPP "cpp -P -C -Wall"
/* we don't need comments, do we ? */
#define CPP "cpp -P -Wall"
#define MAXERRORMSG 1024

/* prototypes */
static int makeswf_preprocess (const char *file, const char *out);
static char *makeswf_readfile (const char *file);
static void compileError(const char *fmt, ...);
static void printCompileMessage(SWFMsgFunc);

/* data */
static int lastcompilefailed = 0;
static char lastcompilemessage[MAXERRORMSG];
static int swfversion = DEFSWFVERSION;

#define MAXCPPARGS 1024

static char cppargs[MAXCPPARGS];
static int dopreprocess = 1; /* use preprocessor by default */

void
makeswf_append_cpparg(const char* buf)
{
	strncat(cppargs, buf, MAXCPPARGS-1);
}

void
makeswf_set_dopreprocess(int value)
{
	dopreprocess=value;
}

void
makeswf_set_swfversion(int value)
{
	swfversion=value;
}

SWFAction
makeswf_compile_source(const char* filename, const char* ppfile)
{
	SWFAction ac;
	char *code;
	char ppfile_fallback[PATH_MAX];        /* preprocessed file */
	SWFMsgFunc old_error_func;

	if ( dopreprocess )
	{
		if ( ! ppfile ) 
		{
			sprintf(ppfile_fallback, "%s.pp", filename);
			ppfile = ppfile_fallback;
		}

		// TODO: make sure ppfile is writable


		printf("Preprocessing %s... ", filename);
		fflush(stdout);
		if ( ! makeswf_preprocess(filename, ppfile) )
		{
			return NULL;
		}
		//unlink(ppfile);
		filename = ppfile;
		printf("done.\n");
	}
	if ( ! (code=makeswf_readfile(filename)) )
	{
		return NULL;
	}

	old_error_func = Ming_setErrorFunction(compileError);

	printf("Compiling `%s'... ", filename);
	ac = newSWFAction(code);
	if ( lastcompilefailed )
	{
		printf("failed:\n"); 
		printCompileMessage(old_error_func);
		exit(EXIT_FAILURE);
	}
	else
	{
		printf("done.\n"); 
	}
	free(code);

	Ming_setErrorFunction(old_error_func);

	return ac;
}

/*
 * Should translate preprocessed file's line number to
 * original file's line number (seems not so easy)
 */
static void
printCompileMessage(SWFMsgFunc func)
{
   char *ptr1;

   printf("  %s\n", strtok(lastcompilemessage, "\n"));
   while  ( (ptr1=strtok(NULL, "\n")) )
   {
      func("  %s\n", ptr1);
   }
}


/* 
 * This is here to handle line number reporting
 * due to preprocessor scrambling of it
 */
static void 
compileError(const char *fmt, ...)
{
	char *msg;
	va_list ap;
	size_t msglen;

	va_start (ap, fmt);

	/*
	 * This is a GNU extension.
	 * Dunno how to handle errors here.
	 */
	if ( ! vasprintf(&msg, fmt, ap) )
	{
		fprintf(stderr, "vasnprintf allocated 0 bytes\n");
		va_end(ap);
		return;
	}
	va_end(ap);

	msglen = strlen(msg);
	if ( msglen > MAXERRORMSG-1 ) msglen = MAXERRORMSG-1;
	memcpy(lastcompilemessage, msg, msglen);
	free(msg);
	lastcompilemessage[MAXERRORMSG-1] = '\0';
	lastcompilefailed = 1;

	return;
}

static char *
makeswf_readfile (const char *file)
{
   FILE *fd;
   struct stat buf;
   int size;
   char *ret;

   fd = fopen(file, "r");
   if ( ! fd )
   {
      perror(file);
      return NULL;
   }
   fstat(fileno(fd), &buf);
   size = buf.st_size;
   ret = (char *) malloc( size+1 );
   if ( ! ret ) 
   {
      perror("readfile");
      return NULL;
   }
   memset(ret, '\0', size+1);
   fread(ret, 1, size, fd);
   fclose(fd);

   return ret;
}

static int
makeswf_preprocess (const char *file, const char *out)
{
	char buf[1024];
	int ret;

	sprintf(buf, "%s -D__SWF_VERSION__=%d %s %s > %s", CPP,
		swfversion, cppargs, file, out);
	/*printf("preprocess[%s]\n", buf);*/

	ret = system(buf);
	if ( ret )
	{
		exit(EXIT_FAILURE);
	}

	return 1;
}

/**************************************************************
 *
 * $Log$
 * Revision 1.3  2007/03/22 10:59:20  strk
 * Updated to use newSWFAction() instead of compileSWFActionCode()
 *
 * Revision 1.2  2006/07/10 16:11:26  strk
 * Changed makeswf_compile_source signature to accept path to preprocessor output file. Changed preprocessed file paths to (<output>.frame#.pp) to reduce probability of filesystem permission problems.
 *
 * Revision 1.1  2006/07/08 13:47:18  strk
 * Split makeswf general functionalities in a separate file, for use by unit testers
 *
 *
 **************************************************************/
