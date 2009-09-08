/****************************************************************************
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
 ***************************************************************************/

/* Set this to the default include path */
#define DEFAULT_FLAGS "-I/usr/share/actionscript -I/usr/local/share/actionscript -I."

/**
 * Use a value of 0 to skip the preprocessing phase (unreccommended)
 */
void makeswf_set_dopreprocess(int value);

/**
 * Set SWF version to the given number
 * !! Make sure you'll save your SWF with that version !!
 */
void makeswf_set_swfversion(int value);

/**
 * Add CPP flags to the default ones
 */
void makeswf_append_cpparg(const char* buf);

/**
 * Return an SWFAction by compiling code in the given sourcefile.
 *
 * Note that the source code can use the preprocessor, unless
 * preprocessing phase is skipped by a call to makeswf_set_dopreprocess(0).
 *
 * If the preprocessing step is not skipped, preprocessed file is wrote
 * into the provided 'ppfile', or <filename>.pp if NULL.
 *
 * The debug parameter, if set, enables SWFAction's debugmode. 
 */
SWFAction makeswf_compile_source(const char* filename, const char* ppfile, int debug);

