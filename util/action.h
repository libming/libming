#ifndef ACTION_H_INCLUDED
#define ACTION_H_INCLUDED

typedef enum
{
  SWFACTION_END        = 0x00,

/* v3 actions */
  SWFACTION_NEXTFRAME     = 0x04,
  SWFACTION_PREVFRAME     = 0x05,
  SWFACTION_PLAY          = 0x06,
  SWFACTION_STOP          = 0x07,
  SWFACTION_TOGGLEQUALITY = 0x08,
  SWFACTION_STOPSOUNDS    = 0x09,
  SWFACTION_GOTOFRAME     = 0x81, /* >= 0x80 means record has args */
  SWFACTION_GETURL        = 0x83,
  SWFACTION_WAITFORFRAME  = 0x8A,
  SWFACTION_SETTARGET     = 0x8B,
  SWFACTION_GOTOLABEL     = 0x8C,

/* v4 actions */
  SWFACTION_ADD                     = 0x0A,
  SWFACTION_SUBTRACT                = 0x0B,
  SWFACTION_MULTIPLY                = 0x0C,
  SWFACTION_DIVIDE                  = 0x0D,
  SWFACTION_EQUAL                   = 0x0E,
  SWFACTION_LESSTHAN                = 0x0F,
  SWFACTION_LOGICALAND              = 0x10,
  SWFACTION_LOGICALOR               = 0x11,
  SWFACTION_LOGICALNOT              = 0x12,
  SWFACTION_STRINGEQ                = 0x13,
  SWFACTION_STRINGLENGTH            = 0x14,
  SWFACTION_SUBSTRING               = 0x15,
  SWFACTION_POP                     = 0x17,
  SWFACTION_INT                     = 0x18,
  SWFACTION_GETVARIABLE             = 0x1C,
  SWFACTION_SETVARIABLE             = 0x1D,
  SWFACTION_SETTARGETEXPRESSION     = 0x20,
  SWFACTION_STRINGCONCAT            = 0x21,
  SWFACTION_GETPROPERTY             = 0x22,
  SWFACTION_SETPROPERTY             = 0x23,
  SWFACTION_DUPLICATECLIP           = 0x24,
  SWFACTION_REMOVECLIP              = 0x25,
  SWFACTION_TRACE                   = 0x26,
  SWFACTION_STARTDRAGMOVIE          = 0x27,
  SWFACTION_STOPDRAGMOVIE           = 0x28,
  SWFACTION_STRINGCOMPARE           = 0x29,
  SWFACTION_RANDOM                  = 0x30,
  SWFACTION_MBLENGTH                = 0x31,
  SWFACTION_ORD                     = 0x32,
  SWFACTION_CHR                     = 0x33,
  SWFACTION_GETTIMER                = 0x34,
  SWFACTION_MBSUBSTRING             = 0x35,
  SWFACTION_MBORD                   = 0x36,
  SWFACTION_MBCHR                   = 0x37,

  SWFACTION_WAITFORFRAMEEXPRESSION  = 0x8D,
  SWFACTION_PUSHDATA                = 0x96,
  SWFACTION_BRANCHALWAYS            = 0x99,
  SWFACTION_GETURL2                 = 0x9A,
  SWFACTION_BRANCHIFTRUE            = 0x9D,
  SWFACTION_CALLFRAME               = 0x9E,
  SWFACTION_GOTOEXPRESSION          = 0x9F,

/* v5 actions */
  SWFACTION_DELETE                  = 0x3B,
  SWFACTION_VAREQUALS               = 0x3C,
  SWFACTION_CALLFUNCTION            = 0x3D,
  SWFACTION_RETURN                  = 0x3E,
  SWFACTION_MODULO                  = 0x3F,
  SWFACTION_NEW                     = 0x40,
  SWFACTION_VAR                     = 0x41,
  SWFACTION_MAKEHASH                = 0x43,
  SWFACTION_TYPEOF                  = 0x44,
  SWFACTION_NEWADD                  = 0x47,
  SWFACTION_NEWLESSTHAN             = 0x48,
  SWFACTION_NEWEQUAL                = 0x49,
  SWFACTION_DUP                     = 0x4C,
  SWFACTION_GETMEMBER               = 0x4E,
  SWFACTION_SETMEMBER               = 0x4F,
  SWFACTION_INCREMENT               = 0x50,
  SWFACTION_DECREMENT               = 0x51,
  SWFACTION_CALLMETHOD              = 0x52,
  SWFACTION_BITWISEAND              = 0x60,
  SWFACTION_BITWISEOR               = 0x61,
  SWFACTION_BITWISEXOR              = 0x62,
  SWFACTION_SHIFTLEFT               = 0x63,
  SWFACTION_SHIFTRIGHT              = 0x64,
  SWFACTION_SHIFTRIGHT2             = 0x65,

  SWFACTION_DECLARENAMES            = 0x88,
  SWFACTION_WITH                    = 0x94,
  SWFACTION_DEFINEFUNCTION          = 0x9B,

  SWFACTION_ITERATE                 = 0x46, /* ??? */

  SWFACTION_SETREGISTER             = 0x87  /* ??? */

} Action;



typedef enum
{
  PROPERTY_X              = 0x00,
  PROPERTY_Y              = 0x01,
  PROPERTY_XSCALE         = 0x02,
  PROPERTY_YSCALE         = 0x03,
  PROPERTY_CURRENTFRAME   = 0x04,
  PROPERTY_TOTALFRAMES    = 0x05,
  PROPERTY_ALPHA          = 0x06,
  PROPERTY_VISIBLE        = 0x07,
  PROPERTY_WIDTH          = 0x08,
  PROPERTY_HEIGHT         = 0x09,
  PROPERTY_ROTATION       = 0x0a,
  PROPERTY_TARGET         = 0x0b,
  PROPERTY_FRAMESLOADED   = 0x0c,
  PROPERTY_NAME           = 0x0d,
  PROPERTY_DROPTARGET     = 0x0e,
  PROPERTY_URL            = 0x0f,
  PROPERTY_XMOUSE         = 0x10,
  PROPERTY_YMOUSE         = 0x11,
  PROPERTY_HIGHQUALITY    = 0x12,
  PROPERTY_FOCUSRECT      = 0x13,
  PROPERTY_SOUNDBUFTIME   = 0x14,
  PROPERTY_QUALITY        = 0x15,
  PROPERTY_WTHIT          = 0x16
} Property;

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

#define DUPCLIP_NUMBER           0x4000

#endif /* ACTION_H_INCLUDED */
