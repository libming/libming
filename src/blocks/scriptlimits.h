/*
    
   Ming, an SWF output library
   Copyright (C) 2002  Opaque Industries - http://www.opaque.net/
   
   3.3.2007 Klaus Rechert
   
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


#ifndef _SCRIPTLIMITS_H_
#define _SCRIPTLIMITS_H_

struct SWFScriptLimits_s
{
        struct SWFBlock_s block;
        int maxRecursion;
        int timeout;
};

typedef struct SWFScriptLimits_s* SWFScriptLimits;

SWFScriptLimits newSWFScriptLimits();
void destroySWFScriptLimits(SWFScriptLimits sl);
void SWFScriptLimits_maxRecursion(SWFScriptLimits sl, int count);
void SWFScriptLimits_setTimeout(SWFScriptLimits sl, int timeout);
#endif

