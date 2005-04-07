
typedef signed char	SI8;	/* Signed  8-bit integer value */
typedef signed short	SI16;	/* Signed 16-bit integer value */
typedef signed long	SI32;	/* Signed 32-bit integer value */
typedef unsigned char	UI8;	/* Unsigned  8-bit integer value */
typedef unsigned short	UI16;	/* Unsigned 16-bit integer value */
typedef unsigned long	UI32;	/* Unsigned 32-bit integer value */

typedef unsigned long	FIXED;	/* 32-bit 16.16 fixed-point number */
typedef unsigned long	SB32;	/* Signed bit value */
typedef unsigned long	UB32;	/* Unsigned bit value */
typedef float		FB32;	/* Signed fixed-point bit value */

typedef unsigned char	*STRING;/* String */

typedef UI8 LANGCODE;

/* Types Flash Types */

typedef struct SWF_MATRIX {
	UI8	HasScale:1;
	UI8	NScaleBits:5;
	FB32	ScaleX;
	FB32	ScaleY;
	UI8	HasRotate:1;
	UI8	NRotateBits:5;
	FB32	RotateSkew0;
	FB32	RotateSkew1;
	UI8	NTranslateBits:5;
	SB32	TranslateX;
	SB32	TranslateY;
} SWF_MATRIX;

typedef struct SWF_BUTTONRECORD {
	UI8	ButtonReserver:4;
	UI8	ButtonStateHitTest:1;
	UI8	ButtonStateDown:1;
	UI8	ButtonStateOver:1;
	UI8	ButtonStateUp:1;
	UI16	CharacterId;
	UI16	PlaceDepth;
	struct SWF_MATRIX	PlaceMAtrix;
} SWF_BUTTONRECORD;

struct SWF_ACTIONRECORD { /* not use */
	UI8	ActionCode;
	UI16	Length;
};

typedef struct SWF_CXFORMWITHALPHA {
	UI8	HasAddTerms:1;
	UI8	HasMultTerms:1;
	UI8	Nbits:4;
} SWF_CXFORMWITHALPHA;

typedef struct SWF_CLIPEVENTFLAGS {
	UI8	ClipEventKeyUp:1;
	UI8	ClipEventKeyDown:1;
	UI8	ClipEventMouseUp:1;
	UI8	ClipEventMouseDown:1;
	UI8	ClipEventMouseMove:1;
	UI8	ClipEventUnload:1;
	UI8	ClipEventEnterFrame:1;
	UI8	ClipEventLoad:1;
	UI8	ClipEventDragOver:1;
	UI8	ClipEventRollOut:1;
	UI8	ClipEventRollOver:1;
	UI8	ClipEventReleaseOutside:1;
	UI8	ClipEventRelease:1;
	UI8	ClipEventPress:1;
	UI8	ClipEventInitialize:1;
	UI8	ClipEventData:1;
	UI8	Reserved:5;
	UI8	ClipEventConstruct:1;
	UI8	ClipEventKeyPress:1;
	UI8	ClipEventDragOut:1;
	UI8	Reserved2:8;
} SWF_CLIPEVENTFLAGS;

typedef struct SWF_CLIPACTIONRECORD {
	SWF_CLIPEVENTFLAGS	EventFlag;
	UI32	ActionRecordSize;
	UI8	KeyCode;
	char	*Actions;
} SWF_CLIPACTIONRECORD;

typedef struct SWF_CLIPACTIONS {
	UI16	Reserved;
	SWF_CLIPEVENTFLAGS	AllEventFlags;
	int	NumClipRecords;
	SWF_CLIPACTIONRECORD	*ClipActionRecords;
	UI32	ClipActionEndFlag;
} SWF_CLIPACTIONS;


typedef struct SWF_RGBA {
	UI8	red;
	UI8	green;
	UI8	blue;
	UI8	alpha;
} SWF_RGBA;

typedef struct SWF_RECT {
	UI8	Nbits:5;
	SB32	Xmin;
	SB32	Xmax;
	SB32	Ymin;
	SB32	Ymax;
} SWF_RECT;

typedef struct SWF_GRADIENTRECORD {
	UI8		Ratio;
	SWF_RGBA	Color;
} SWF_GRADIENTRECORD;

typedef struct SWF_GRADIENT {
	UI8	NumGradients;
	SWF_GRADIENTRECORD	GradientRecords[8];
} SWF_GRADIENT;

typedef struct SWF_FILLSTYLE {
	UI8		FillStyleType;
	SWF_RGBA	Color;
	SWF_MATRIX	GradientMatrix;
	SWF_GRADIENT	Gradient;
	UI16		BitmapId;
	SWF_MATRIX	BitmapMatrix;
} SWF_FILLSTYLE;

typedef struct SWF_FILLSTYLEARRAY {
	UI8	FillStyleCount;
	UI16	FillStyleCountExtended;
	SWF_FILLSTYLE	*FillStyles;
} SWF_FILLSTYLEARRAY;

typedef struct SWF_LINESTYLE {
	UI16		Width;
	SWF_RGBA	Color;
} SWF_LINESTYLE;

typedef struct SWF_LINESTYLEARRAY {
	UI8	LineStyleCount;
	UI16	LineStyleCountExtended;
	SWF_LINESTYLE	*LineStyles;
} SWF_LINESTYLEARRAY;

typedef struct SWF_ENDSHAPERECORD {
	UI16	TypeFlag:1;
	UI16	EndOfShape:5;
} SWF_ENDSHAPERECORD;

typedef struct SWF_STYLECHANGERECORD {
	UI16	TypeFlag:1;
	UI16	StateNewStyles:1;
	UI16	StateLineStyle:1;
	UI16	StateFillStyle1:1;
	UI16	StateFillStyle0:1;
	UI16	StateMoveTo:1;
	UI16	MoveBits:5;
	SB32	MoveDeltaX;
	SB32	MoveDeltaY;
	UB32	FillStyle0;
	UB32	FillStyle1;
	UB32	LineStyle;
	SWF_FILLSTYLEARRAY	FillStyles;
	SWF_LINESTYLEARRAY	LineStyles;
	UI8	NumFillBits:4;
	UI8	NumLineBits:4;
} SWF_STYLECHANGERECORD;

typedef struct SWF_CURVEDEDGERECORD {
	UI16	TypeFlag:1;
	UI16	StraightFlag:1;
	UI16	NumBits:4;
	SB32	ControlDeltaX;
	SB32	ControlDeltaY;
	SB32	AnchorDeltaX;
	SB32	AnchorDeltaY;
} SWF_CURVEDEDGERECORD;

typedef struct SWF_STRAIGHTEDGERECORD {
	UI16	TypeFlag:1;
	UI16	StraightEdge:1;
	UI16	NumBits:4;
	UI16	GeneralLineFlag:1;
	SB32	DeltaX;
	SB32	DeltaY;
	SI8	VertLineFlag:1;
	SB32	VLDeltaX;
	SB32	VLDeltaY;
} SWF_STRAIGHTEDGERECORD;

typedef union SWF_SHAPERECORD {
	SWF_ENDSHAPERECORD	EndShape;
	SWF_STYLECHANGERECORD	StyleChange;
	SWF_STRAIGHTEDGERECORD	StraightEdge;
	SWF_CURVEDEDGERECORD	CurvedEdge;
} SWF_SHAPERECORD;

typedef struct SWF_SHAPE {
	UI8	NumFillBits:4;
	UI8	NumLineBits:4;
	int	NumShapeRecords;
	SWF_SHAPERECORD	*ShapeRecords;
} SWF_SHAPE;

typedef struct SWF_SHAPEWITHSTYLE {
	SWF_FILLSTYLEARRAY	FillStyles;
	SWF_LINESTYLEARRAY	LineStyles;
	UI8	NumFillBits:4;
	UI8	NumLineBits:4;
	int	NumShapeRecords;
	SWF_SHAPERECORD	*ShapeRecords;
} SWF_SHAPEWITHSTYLE;


/* Types to represent Blocks */

struct SWF_CHARACTERSET
{
  int chid;
};

struct SWF_DEFINEBITS
{
  int chid;
};

struct SWF_DEFINEBITSJPEG2
{
  int chid;
};

struct SWF_DEFINEBITSJPEG3
{
  int chid;
};

struct SWF_DEFINEBITSPTR
{
  int chid;
};

struct SWF_DEFINEBUTTON
{
  int chid;
};

struct SWF_DEFINEBUTTON2
{
  int chid;
};

struct SWF_DEFINEBUTTONCXFORM
{
  int chid;
};

struct SWF_DEFINEBUTTONSOUND
{
  int chid;
};

struct SWF_DEFINECOMMANDOBJ
{
  int chid;
};

struct SWF_DEFINEEDITTEXT
{
  UI16 CharacterID;
  SWF_RECT Bounds;
  UI8	HasText:1;
  UI8	WordWrap:1;
  UI8	Multiline:1;
  UI8	Password:1;
  UI8	ReadOnly:1;
  UI8	HasTextColor:1;
  UI8	HasMaxLength:1;
  UI8	HasFont:1;
  UI8	Reserved:1;
  UI8	AutoSize:1;
  UI8	HasLayout:1;
  UI8	NoSelect:1;
  UI8	Border:1;
  UI8	Reserved2:1;
  UI8	HTML:1;
  UI8	UseOutlines:1;
  UI16	FontID;
  UI16	FontHeight;
  SWF_RGBA	TextColor;
  UI16	MaxLength;
  UI8	Align;
  UI16	LeftMargin;
  UI16	RightMargin;
  UI16	Indent;
  UI16	Leading;
  STRING	VariableName;
  STRING	InitialText;
};

struct SWF_DEFINEFONT
{
  int chid;
};

struct SWF_DEFINEFONT2
{
  UI16 FontID;
  UI8 FontFlagsHasLayout:1;
  UI8 FontFlagsShiftJis:1;
  UI8 FontFlagsSmallText:1;
  UI8 FontFlagsFlagANSI:1;
  UI8 FontFlagsWideOffsets:1;
  UI8 FontFlagsWideCodes:1;
  UI8 FontFlagsFlagsItalics:1;
  UI8 FontFlagsFlagsBold:1;
  LANGCODE LanguageCode;
  UI8 FontNameLen;
  STRING FontName;
  UI16 NumGlyphs;
  union {
    UI16 *UI16;
    UI32 *UI32;
  } OffsetTable;
  union {
    UI16 UI16;
    UI32 UI32;
  } CodeTableOffset;
  SWF_SHAPE *GlyphShapeTable;
};

struct SWF_DEFINEFONTINFO
{
  int chid;
};

struct SWF_DEFINELOSSLESS
{
  int chid;
};

struct SWF_DEFINELOSSLESS2
{
  int chid;
};

struct SWF_DEFINEMORPHSHAPE
{
  int chid;
};

struct SWF_DEFINESHAPE
{
  UI16 ShapeID;
  SWF_RECT ShapeBounds;
  SWF_SHAPEWITHSTYLE Shapes;
};

struct SWF_DEFINESHAPE2
{
  UI16 ShapeID;
  SWF_RECT ShapeBounds;
  SWF_SHAPEWITHSTYLE Shapes;
};

struct SWF_DEFINESHAPE3
{
  UI16 ShapeID;
  SWF_RECT ShapeBounds;
  SWF_SHAPEWITHSTYLE Shapes;
};

struct SWF_DEFINESOUND
{
  int chid;
};

struct SWF_DEFINESPRITE
{
  UI16 SpriteId;
  UI16 FrameCount;
  UI16 BlockCount;
  UI16 *tagTypes;
  union SWF_Parserstruct **Tags;
};

struct SWF_DEFINETEXT
{
  int chid;
};

struct SWF_DEFINETEXT2
{
  int chid;
};

struct SWF_DEFINETEXTFORMAT
{
  int chid;
};

struct SWF_DEFINEVIDEO
{
  int chid;
};

struct SWF_DEFINEVIDEOSTREAM
{
  int chid;
};

struct SWF_DOACTION
{
  int  numActions;
  struct SWF_ACTIONRECORD *Actions;
  char *AScript;
};

struct SWF_ENABLEDEBUGGER
{
  int chid;
};

struct SWF_END
{
  int chid;
};

struct SWF_EXPORTASSETS
{
  int chid;
};

struct SWF_FONTREF
{
  int chid;
};

struct SWF_FRAMELABEL
{
  STRING	Name;
};

struct SWF_FRAMETAG
{
  int chid;
};

struct SWF_FREEALL
{
  int chid;
};

struct SWF_FREECHARACTER
{
  int chid;
};

struct SWF_GENCOMMAND
{
  int chid;
};

struct SWF_IMPORTASSETS
{
  int chid;
};

struct SWF_JPEGTABLES
{
  int chid;
};

struct SWF_NAMECHARACTER
{
  int chid;
};

struct SWF_PATHSAREPOSTSCRIPT
{
  int chid;
};

struct SWF_PLACEOBJECT
{
  int chid;
};

struct SWF_PLACEOBJECT2
{
  UI8 PlaceFlagHasClipActions:1;
  UI8 PlaceFlagHasClipDepth:1;
  UI8 PlaceFlagHasName:1;
  UI8 PlaceFlagHasRatio:1;
  UI8 PlaceFlagHasColorTransform:1;
  UI8 PlaceFlagHasMatrix:1;
  UI8 PlaceFlagHasCharacter:1;
  UI8 PlaceFlagMove:1;
  UI16 Depth;
  UI16 CharacterId;
  SWF_MATRIX Matrix;
  SWF_CXFORMWITHALPHA ColorTransform;
  UI16 Ratio;
  STRING Name;
  UI16 ClipDepth;
  SWF_CLIPACTIONS ClipActions;
};

struct SWF_PREBUILT
{
  int chid;
};

struct SWF_PREBUILTCLIP
{
  int chid;
};

struct SWF_PROTECT
{
  int chid;
};

struct SWF_REMOVEOBJECT
{
  UI16 CharacterId;
  UI16 Depth;
};

struct SWF_REMOVEOBJECT2
{
  UI16 Depth;
};

struct SWF_SERIALNUMBER
{
  int chid;
};

struct SWF_SETBACKGROUNDCOLOR
{
  SWF_RGBA rgb;
};

struct SWF_SHOWFRAME
{
  int chid;
};

struct SWF_SOUNDSTREAMBLOCK
{
  int chid;
};

struct SWF_SOUNDSTREAMHEAD
{
  int chid;
};

struct SWF_SOUNDSTREAMHEAD2
{
  int chid;
};

struct SWF_STARTSOUND
{
  int chid;
};

struct SWF_SYNCFRAME
{
  int chid;
};

struct SWF_VIDEOFRAME
{
  int chid;
};


typedef union SWF_Parserstruct
{
  struct SWF_CHARACTERSET SWF_CHARACTERSET;
  struct SWF_DEFINEBITS SWF_DEFINEBITS;
  struct SWF_DEFINEBITSJPEG2 SWF_DEFINEBITSJPEG2;
  struct SWF_DEFINEBITSJPEG3 SWF_DEFINEBITSJPEG3;
  struct SWF_DEFINEBITSPTR SWF_DEFINEBITSPTR;
  struct SWF_DEFINEBUTTON SWF_DEFINEBUTTON;
  struct SWF_DEFINEBUTTON2 SWF_DEFINEBUTTON2;
  struct SWF_DEFINEBUTTONCXFORM SWF_DEFINEBUTTONCXFORM;
  struct SWF_DEFINEBUTTONSOUND SWF_DEFINEBUTTONSOUND;
  struct SWF_DEFINECOMMANDOBJ SWF_DEFINECOMMANDOBJ;
  struct SWF_DEFINEEDITTEXT SWF_DEFINEEDITTEXT;
  struct SWF_DEFINEFONT SWF_DEFINEFONT;
  struct SWF_DEFINEFONT2 SWF_DEFINEFONT2;
  struct SWF_DEFINEFONTINFO SWF_DEFINEFONTINFO;
  struct SWF_DEFINELOSSLESS SWF_DEFINELOSSLESS;
  struct SWF_DEFINELOSSLESS2 SWF_DEFINELOSSLESS2;
  struct SWF_DEFINEMORPHSHAPE SWF_DEFINEMORPHSHAPE;
  struct SWF_DEFINESHAPE SWF_DEFINESHAPE;
  struct SWF_DEFINESHAPE2 SWF_DEFINESHAPE2;
  struct SWF_DEFINESHAPE3 SWF_DEFINESHAPE3;
  struct SWF_DEFINESOUND SWF_DEFINESOUND;
  struct SWF_DEFINESPRITE SWF_DEFINESPRITE;
  struct SWF_DEFINETEXT SWF_DEFINETEXT;
  struct SWF_DEFINETEXT2 SWF_DEFINETEXT2;
  struct SWF_DEFINETEXTFORMAT SWF_DEFINETEXTFORMAT;
  struct SWF_DEFINEVIDEO SWF_DEFINEVIDEO;
  struct SWF_DEFINEVIDEOSTREAM SWF_DEFINEVIDEOSTREAM;
  struct SWF_DOACTION SWF_DOACTION;
  struct SWF_ENABLEDEBUGGER SWF_ENABLEDEBUGGER;
  struct SWF_END SWF_END;
  struct SWF_EXPORTASSETS SWF_EXPORTASSETS;
  struct SWF_FONTREF SWF_FONTREF;
  struct SWF_FRAMELABEL SWF_FRAMELABEL;
  struct SWF_FRAMETAG SWF_FRAMETAG;
  struct SWF_FREEALL SWF_FREEALL;
  struct SWF_FREECHARACTER SWF_FREECHARACTER;
  struct SWF_GENCOMMAND SWF_GENCOMMAND;
  struct SWF_IMPORTASSETS SWF_IMPORTASSETS;
  struct SWF_JPEGTABLES SWF_JPEGTABLES;
  struct SWF_NAMECHARACTER SWF_NAMECHARACTER;
  struct SWF_PATHSAREPOSTSCRIPT SWF_PATHSAREPOSTSCRIPT;
  struct SWF_PLACEOBJECT SWF_PLACEOBJECT;
  struct SWF_PLACEOBJECT2 SWF_PLACEOBJECT2;
  struct SWF_PREBUILT SWF_PREBUILT;
  struct SWF_PREBUILTCLIP SWF_PREBUILTCLIP;
  struct SWF_PROTECT SWF_PROTECT;
  struct SWF_REMOVEOBJECT SWF_REMOVEOBJECT;
  struct SWF_REMOVEOBJECT2 SWF_REMOVEOBJECT2;
  struct SWF_SERIALNUMBER SWF_SERIALNUMBER;
  struct SWF_SETBACKGROUNDCOLOR SWF_SETBACKGROUNDCOLOR;
  struct SWF_SHOWFRAME SWF_SHOWFRAME;
  struct SWF_SOUNDSTREAMBLOCK SWF_SOUNDSTREAMBLOCK;
  struct SWF_SOUNDSTREAMHEAD SWF_SOUNDSTREAMHEAD;
  struct SWF_SOUNDSTREAMHEAD2 SWF_SOUNDSTREAMHEAD2;
  struct SWF_STARTSOUND SWF_STARTSOUND;
  struct SWF_SYNCFRAME SWF_SYNCFRAME;
  struct SWF_VIDEOFRAME SWF_VIDEOFRAME;
} SWF_Parserstruct;
