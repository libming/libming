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

#include "action.h"
#include "outputblock.h"

SWFAction newSWFAction()
{
  return newSWFOutputBlock(newSWFOutput(), SWF_DOACTION);
}
void destroySWFAction(SWFAction action)
{
  destroySWFOutputBlock((SWFBlock)action);
}

SWFAction newSWFAction_fromOutput(SWFOutput out)
{
  return newSWFOutputBlock(out, SWF_DOACTION);
}

/*
void SWFAction_end(SWFAction action)
{
  SWFOutput output = ((SWFOutputBlock)action)->output;
  SWFOutput_writeUInt8(output, 0);
}

void SWFAction_addShortRecord(SWFAction action, byte record)
{
  SWFOutput output = ((SWFOutputBlock)action)->output;
  SWF_assert(record < 0x80);
  SWFOutput_writeUInt8(output, record);
}

void SWFAction_addGotoFrame(SWFAction action, int frame)
{
  SWFOutput output = ((SWFOutputBlock)action)->output;
  SWFOutput_writeUInt8(output, SWF_ACTION_GOTOFRAME);
  SWFOutput_writeUInt8(output, 2);
  SWFOutput_writeUInt16(output, frame);
}

void SWFAction_addGetURL(SWFAction action, byte *url, byte *target)
{
  SWFOutput output = ((SWFOutputBlock)action)->output;
  SWFOutput_writeUInt8(output, SWF_ACTION_GETURL);
  SWFOutput_writeUInt8(output, strlen(url)+strlen(target)+2);
  SWFOutput_writeString(output, url);
  SWFOutput_writeString(output, target);
}

void SWFAction_addWaitForFrame(SWFAction action, int frame, byte skip)
{
  SWFOutput out = ((SWFOutputBlock)action)->output;
  SWFOutput_writeUInt8(out, SWF_ACTION_WAITFORFRAME);
  SWFOutput_writeUInt8(out, 3);
  SWFOutput_writeUInt16(out, frame);
  SWFOutput_writeUInt8(out, skip);
}

void SWFAction_addSetTarget(SWFAction action, byte *target)
{
  SWFOutput out = ((SWFOutputBlock)action)->output;
  SWFOutput_writeUInt8(out, SWF_ACTION_SETTARGET);
  SWFOutput_writeUInt16(out, strlen(target)+1);
  SWFOutput_writeString(out, target);
}

void SWFAction_addGotoLabel(SWFAction action, byte *label)
{
  SWFOutput out = ((SWFOutputBlock)action)->output;
  SWFOutput_writeUInt8(out, SWF_ACTION_GOTOLABEL);
  SWFOutput_writeUInt16(out, strlen(label)+1);
  SWFOutput_writeString(out, label);
}

void SWFAction_addPushString(SWFAction action, char *string)
{
  SWFOutput out = ((SWFOutputBlock)action)->output;
  SWFOutput_writeUInt8(out, SWF_ACTION_PUSHDATA);
  SWFOutput_writeUInt16(out, strlen(string)+2);
  SWFOutput_writeUInt8(out, 0); *//* string *//*
  SWFOutput_writeString(out, string);
}

void SWFAction_addPushProperty(SWFAction action, int property)
{
  SWFOutput out = ((SWFOutputBlock)action)->output;
  SWFOutput_writeUInt8(out, SWF_ACTION_PUSHDATA);
  SWFOutput_writeUInt16(out, 5);
  SWFOutput_writeUInt8(out, 1); *//* property *//*
  SWFOutput_writeUInt16(out, 0); *//* unknown *//*
  SWFOutput_writeUInt16(out, property); *//* from setproperty table *//*
}
*/
