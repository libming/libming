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

/* action.h */

#ifndef SWF_ACTION_H_INCLUDED
#define SWF_ACTION_H_INCLUDED

#include "output.h"
#include "block.h"
#include "outputblock.h"

typedef SWFOutputBlock SWFAction;


#define SWF_ACTION_END           0x00

/* v3 actions */
#define SWF_ACTION_NEXTFRAME     0x04
#define SWF_ACTION_PREVFRAME     0x05
#define SWF_ACTION_PLAY          0x06
#define SWF_ACTION_STOP          0x07
#define SWF_ACTION_TOGGLEQUALITY 0x08
#define SWF_ACTION_STOPSOUNDS    0x09
#define SWF_ACTION_GOTOFRAME     0x81 /* >=0x80 means record has args */
#define SWF_ACTION_GETURL        0x83
#define SWF_ACTION_WAITFORFRAME  0x8A
#define SWF_ACTION_SETTARGET     0x8B
#define SWF_ACTION_GOTOLABEL     0x8C

/* v4 actions */
#define SWF_ACTION_ADD                     0x0A
#define SWF_ACTION_SUBTRACT                0x0B
#define SWF_ACTION_MULTIPLY                0x0C
#define SWF_ACTION_DIVIDE                  0x0D
#define SWF_ACTION_EQUAL                   0x0E
#define SWF_ACTION_LESSTHAN                0x0F
#define SWF_ACTION_LOGICALAND              0x10
#define SWF_ACTION_LOGICALOR               0x11
#define SWF_ACTION_LOGICALNOT              0x12
#define SWF_ACTION_STRINGEQ                0x13
#define SWF_ACTION_STRINGLENGTH            0x14
#define SWF_ACTION_SUBSTRING               0x15
#define SWF_ACTION_INT                     0x18
#define SWF_ACTION_GETVARIABLE             0x1C
#define SWF_ACTION_SETVARIABLE             0x1D
#define SWF_ACTION_SETTARGETEXPRESSION     0x20
#define SWF_ACTION_STRINGCONCAT            0x21
#define SWF_ACTION_GETPROPERTY             0x22
#define SWF_ACTION_SETPROPERTY             0x23
#define SWF_ACTION_DUPLICATECLIP           0x24
#define SWF_ACTION_REMOVECLIP              0x25
#define SWF_ACTION_TRACE                   0x26
#define SWF_ACTION_STARTDRAGMOVIE          0x27
#define SWF_ACTION_STOPDRAGMOVIE           0x28
#define SWF_ACTION_STRINGCOMPARE           0x29
#define SWF_ACTION_RANDOM                  0x30
#define SWF_ACTION_MBLENGTH                0x31
#define SWF_ACTION_ORD                     0x32
#define SWF_ACTION_CHR                     0x33
#define SWF_ACTION_GETTIMER                0x34
#define SWF_ACTION_MBSUBSTRING             0x35
#define SWF_ACTION_MBORD                   0x36
#define SWF_ACTION_MBCHR                   0x37

#define SWF_ACTION_WAITFORFRAMEEXPRESSION  0x8D
#define SWF_ACTION_PUSHDATA                0x96
#define SWF_ACTION_BRANCHALWAYS            0x99
#define SWF_ACTION_GETURL2                 0x9A
#define SWF_ACTION_BRANCHIFTRUE            0x9D
#define SWF_ACTION_CALLFRAME               0x9E
#define SWF_ACTION_GOTOEXPRESSION          0x9F

void SWFAction_addPushString(SWFAction action, char *string);
void SWFAction_addPushProperty(SWFAction action, int property);
void SWFAction_addGetURL2(SWFAction action, byte type);
void SWFAction_addBranchAlways(SWFAction action, int offset); /* UInt? inst offset */
void SWFAction_addBranchIfTrue(SWFAction action, int offset);   /* or byte offset? */


#define SWF_GETPROPERTY_X                0
#define SWF_GETPROPERTY_Y                1
#define SWF_GETPROPERTY_XSCALE           2
#define SWF_GETPROPERTY_YSCALE           3
#define SWF_GETPROPERTY_CURRENTFRAME     4
#define SWF_GETPROPERTY_TOTALFRAMES      5
#define SWF_GETPROPERTY_ALPHA            6
#define SWF_GETPROPERTY_VISIBLE          7
#define SWF_GETPROPERTY_WIDTH            8
#define SWF_GETPROPERTY_HEIGHT           9
#define SWF_GETPROPERTY_ROTATION        10
#define SWF_GETPROPERTY_TARGET          11
#define SWF_GETPROPERTY_FRAMESLOADED    12
#define SWF_GETPROPERTY_NAME            13
#define SWF_GETPROPERTY_DROPTARGET      14
#define SWF_GETPROPERTY_URL             15
#define SWF_GETPROPERTY_HIGHQUALITY     16
#define SWF_GETPROPERTY_FOCUSRECT       17
#define SWF_GETPROPERTY_SOUNDBUFTIME    18

#define SWF_SETPROPERTY_X               0x0000
#define SWF_SETPROPERTY_Y               0x3F80
#define SWF_SETPROPERTY_XSCALE          0x4000
#define SWF_SETPROPERTY_YSCALE          0x4040
#define SWF_SETPROPERTY_ALPHA           0x40C0
#define SWF_SETPROPERTY_VISIBILITY      0x40E0
#define SWF_SETPROPERTY_ROTATION        0x4120
#define SWF_SETPROPERTY_NAME            0x4140
#define SWF_SETPROPERTY_HIGHQUALITY     0x4180
#define SWF_SETPROPERTY_SHOWFOCUSRECT   0x4188
#define SWF_SETPROPERTY_SOUNDBUFFERTIME 0x4190
#define SWF_SETPROPERTY_WTHIT           0x4680

SWFAction newSWFAction();
SWFAction newSWFAction_fromOutput(SWFOutput out);
void destroySWFAction(SWFAction action);

void SWFAction_end(SWFAction action);
void SWFAction_addShortRecord(SWFAction action, byte record);
void SWFAction_addGotoFrame(SWFAction action, int frame);
void SWFAction_addGetURL(SWFAction action, byte *url, byte *target);
void SWFAction_addWaitForFrame(SWFAction action, int frame, byte skip);
void SWFAction_addSetTarget(SWFAction action, byte *target);
void SWFAction_addGotoLabel(SWFAction action, byte *label);

#endif /* SWF_ACTION_H_INCLUDED */
