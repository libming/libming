/****************************************************************************
 *
 *  makeswf - a command line actionscript compiler
 *
 *  Copyright (C) 2003-2009 "Sandro Santilli" <strk@keybit.net>
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
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
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
#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif
#include "vasprintf.h"

// Cheating, but it works (not sure why the above ifdef for getopt isn't)
#ifdef _WIN32
#include <getopt.h>
#endif

#define DEFSWFVERSION 6
#define DEFSWFCOMPRESSION 9

//#define CPP "cpp -P -C -Wall"
/* we don't need comments, do we ? */
#define CPP "cpp -xc++ -P -Wall"
#define MAXERRORMSG 1024

/* prototypes */
static int makeswf_preprocess (const char *file, const char *out);
static char *makeswf_readfile (const char *file);
static void compileError(const char *fmt, ...);
static void printCompileMessage(SWFMsgFunc);

/* data */
static char lastcompilemessage[MAXERRORMSG];
static int lastcompilefailed = 0;
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
makeswf_compile_source(const char* filename, const char* ppfile, int debug)
{
	SWFAction ac;
	char *code;
	char ppfile_fallback[PATH_MAX];        /* preprocessed file */
	SWFMsgFunc old_error_func;
	int length;
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
	SWFAction_setDebug(ac, debug);
	if (SWFAction_compile(ac, swfversion, &length) || lastcompilefailed)
	{
		printf("failed:\n"); 
		printCompileMessage(old_error_func);
		return NULL;
	}
	else
	{
		printf("successfully compiled %i bytes bytecode.\n", length);
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

   fprintf(stderr, "  %s\n", strtok(lastcompilemessage, "\n"));
   while  ( (ptr1=strtok(NULL, "\n")) )
   {
      fprintf(stderr, "  %s\n", ptr1);
   }
   func("\n");
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

	lastcompilefailed++;

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
   if(fread(ret, 1, size, fd) != size)
   {
	fprintf(stderr, "makeswf_readfile: failed\n");
	fclose(fd);
	free(ret);
	return NULL;	
   }
   ret[size] = '\0';
   fclose(fd);

   return ret;
}

static int
makeswf_preprocess (const char *file, const char *out)
{
	char buf[1024];
	int ret;
	struct stat statbuf;

	if ( -1 == stat(file, &statbuf) )
	{
		perror(file); 
		exit(EXIT_FAILURE);
	}

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
 * Revision 1.14  2010/04/28 12:09:58  strk
 * Don't trust SWFAction_compile return code to be the only sign of failure.
 *
 * Revision 1.13  2009/09/08 22:26:17  strk
 * Update copyright notice (and FSF address)
 *
 * Revision 1.12  2008/10/13 21:57:19  krechert
 * fix warning
 *
 * Revision 1.11  2008/06/26 19:36:12  krechert
 * fix linker error and make enabling SWFAction's debug mode generic
 *
 * Revision 1.10  2007/12/17 17:28:26  strk
 * Check existance of input file before attempting to preprocess it
 *
 * Revision 1.9  2007/11/02 10:18:08  krechert
 * fix warnings
 *
 * Revision 1.8  2007/10/31 16:53:29  krechert
 * x
 *
 * Revision 1.7  2007/10/27 15:12:58  krechert
 * pass through real swf version
 *
 * Revision 1.6  2007/10/27 15:03:14  krechert
 * handle compile errors. do not output empty swf files
 *
 * Revision 1.5  2007/04/30 09:58:32  strk
 * Don't include getopt.h if it's not found. Patch by Nils Goroll <nils.goroll@mcs.de>.
 *
 * Revision 1.4  2007/04/14 16:58:46  strk
 * Removed unused CPP define from makeswf.c, fix the *used* one in makeswf_utils.
 * Thanks to Bastiaan Jacques for finding this out.
 *
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
