/****************************************************************************
 *
 *  Copyright (C) 2005-2006 "Stuart R. Anderson" <anderson@netsweng.com>
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
 ****************************************************************************/

#include <stdio.h>

struct Rect
{ 
  int xMin;
  int xMax;
  int yMin;
  int yMax;
};

struct Movie
{ 
  int version;
  int size;                     /* in bytes */
  struct Rect frame;
  float rate;
  int nFrames;
};


#include "swftypes.h"

typedef SWF_Parserstruct *(*SWFParseFunc)(FILE *, int);

#include "parserdecl.h"
