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

/* button.h */

#ifndef SWF_BUTTON_H_INCLUDED
#define SWF_BUTTON_H_INCLUDED

#include "block.h"
#include "matrix.h"
#include "action.h"
#include "cxform.h"
#include "sound.h"

#define SWFBUTTON_HIT    (1<<3)
#define SWFBUTTON_DOWN   (1<<2)
#define SWFBUTTON_OVER   (1<<1)
#define SWFBUTTON_UP     (1<<0)

#define SWFBUTTON_ONKEYPRESS(c)     (((c)&7f)<<9)
#define SWFBUTTON_KEYPRESS(c)     (((c)&7f)<<9)

#define SWFBUTTON_OVERDOWNTOIDLE    (1<<8)
#define SWFBUTTON_IDLETOOVERDOWN    (1<<7)
#define SWFBUTTON_OUTDOWNTOIDLE     (1<<6)
#define SWFBUTTON_OUTDOWNTOOVERDOWN (1<<5)
#define SWFBUTTON_OVERDOWNTOOUTDOWN (1<<4)
#define SWFBUTTON_OVERDOWNTOOVERUP  (1<<3)
#define SWFBUTTON_OVERUPTOOVERDOWN  (1<<2)
#define SWFBUTTON_OVERUPTOIDLE      (1<<1)
#define SWFBUTTON_IDLETOOVERUP      (1<<0)


struct _buttonRecord
{
  byte flags;
  unsigned short layer;
  SWFCharacter character;
  SWFMatrix matrix;
};
typedef struct _buttonRecord *SWFButtonRecord;

#define BUTTONRECORD_SIZE sizeof(struct _buttonRecord)

struct _actionRecord
{
  int flags;
  SWFAction action;
};
typedef struct _actionRecord swfActionRecord;

#define ACTIONRECORD_SIZE sizeof(struct _actionRecord)

#define SWF_BUTTON_TRACKASMENU  (1<<0)
struct _button
{
  swfCharacter character;
  int nRecords;
  SWFButtonRecord *records;
  int nActions;
  swfActionRecord *actions;
  SWFOutput out;
};
typedef struct _button *SWFButton;

#define SWFBUTTON_SIZE sizeof(struct _button)

SWFButton newSWFButton();
void destroySWFButton(SWFBlock block);

void SWFButton_addShape(SWFButton button, SWFCharacter character, byte flags);
void SWFButton_addAction(SWFButton button, SWFAction action, int flags);

SWFBlock newDefineSWFButtonBlock(SWFButton button);
SWFBlock newDefineSWFButtonCXform(SWFButton button, SWFCXform *cXforms);
SWFBlock newDefineSWFButtonSWFSound(SWFButton button, SWFSound sound);

#endif /* SWF_BUTTON_H_INCLUDED */
