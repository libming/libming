
int sprite; /* sprite we're making.  0 if movie */

struct Matrix
{
  float xScale;
  float yScale;
  float rot0;
  float rot1;
  int x;
  int y;
};

struct Rect
{
  int xMin;
  int xMax;
  int yMin;
  int yMax;
};

struct RGBA
{
  int r;
  int g;
  int b;
  int a;
};

struct CXForm
{
  int aMult;
  int rMult;
  int gMult;
  int bMult;
  int aAdd;
  int rAdd;
  int gAdd;
  int bAdd;
};

struct Gradient
{
  int nGradients;
  struct RGBA *colors;
  int *ratio;
};

struct LineStyle
{
  int width;
  int width2;
  struct RGBA color;
  struct RGBA color2;
};

struct LineStyles
{
  int nLines;
  struct LineStyle *line;
};

struct BitmapFill
{
  int characterID;
  struct Matrix matrix;
  struct Matrix matrix2; /* not actually used by player! */
};

struct FillStyle
{
  byte type;
  struct Matrix matrix;
  struct Matrix matrix2;
  union
  {
    struct RGBA color;
    struct Gradient gradient;
    int bitmap;
  } fill;
  union
  {
    struct RGBA color;
    struct Gradient gradient;
  } fill2;
};

struct FillStyles
{
  int nFills;
  struct FillStyle *fill;
};

enum shapeRecordType
{
  SHAPERECORD_END,
  SHAPERECORD_LINE,
  SHAPERECORD_CURVE,
  SHAPERECORD_STATECHANGE
};

struct ShapeRecord
{
  enum shapeRecordType type;
  union
  {
    struct
    {
      int x;
      int y;
    } line;

    struct
    {
      int controlx;
      int controly;
      int anchorx;
      int anchory;
    } curve;

    struct
    {
      int movetox;
      int movetoy;
      int line;
      int fill0;
      int fill1;
    } change;

  } data;
};

struct ShapeRecords
{
  int nShapes;
  struct ShapeRecord *shape;
};

struct Shape
{
  Blocktype shapeType;
  struct Rect bounds; /* not actually used for anything.. */
  struct LineStyles lines;
  struct FillStyles fills;
  struct ShapeRecords shapes;
  byte lineBits;
  byte fillBits;
  short lineOffset;
  short fillOffset;
};

struct Movie
{
  int version;
  int size; /* in bytes */
  struct Rect frame;
  float rate;
  int nFrames;
};
