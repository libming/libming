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

/* $Id$ */

#ifndef SWF_ERROR_H_INCLUDED
#define SWF_ERROR_H_INCLUDED

/* XXX - error calls should be macros to save the file/lineno */

extern void (*SWF_warn) (char *msg, ...);
extern void (*SWF_error) (char *msg, ...);

void warn_default (char *msg, ...);
void error_default (char *msg, ...);

void setSWFWarnFunction (void (*error) (char *msg, ...));
void setSWFErrorFunction (void (*error) (char *msg, ...));

void SWF_assert (int c);

#endif /* SWF_ERROR_H_INCLUDED */
