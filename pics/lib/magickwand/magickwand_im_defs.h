/* MagickWand for PHP MagickWand C API type definitions file; used to allow access to the internals of the
   wands / PixelIterator, so that their exception status can be tested quickly (without needing to
   call the *Exception() functions)

   Author: Ouinnel Watson
   Homepage: 
   Current Version: 0.1.3
   Release Date: 2005-01-11
*/

/* ************************************************************************************************************** */
/* DrawingWand definition */
/*
  Typedef declarations.
*/
typedef enum
{
  PathDefaultOperation,
  PathCloseOperation,                        /* Z|z (none) */
  PathCurveToOperation,                      /* C|c (x1 y1 x2 y2 x y)+ */
  PathCurveToQuadraticBezierOperation,       /* Q|q (x1 y1 x y)+ */
  PathCurveToQuadraticBezierSmoothOperation, /* T|t (x y)+ */
  PathCurveToSmoothOperation,                /* S|s (x2 y2 x y)+ */
  PathEllipticArcOperation,                  /* A|a (rx ry x-axis-rotation large-arc-flag sweep-flag x y)+ */
  PathLineToHorizontalOperation,             /* H|h x+ */
  PathLineToOperation,                       /* L|l (x y)+ */
  PathLineToVerticalOperation,               /* V|v y+ */
  PathMoveToOperation                        /* M|m (x y)+ */
} PathOperation;

typedef enum
{
  DefaultPathMode,
  AbsolutePathMode,
  RelativePathMode
} PathMode;

struct _DrawingWand
{
  unsigned long
    id;

  char
    name[MaxTextExtent];

  /* Support structures */
  Image
    *image;

  ExceptionInfo
    exception;

  /* MVG output string and housekeeping */
  char
    *mvg;               /* MVG data */

  size_t
    mvg_alloc,          /* total allocated memory */
    mvg_length;         /* total MVG length */

  unsigned long
    mvg_width;          /* current line width */

  /* Pattern support */
  char
    *pattern_id;

  RectangleInfo
    pattern_bounds;

  size_t
    pattern_offset;

  /* Graphic wand */
  unsigned long
    index;              /* array index */

  DrawInfo
    **graphic_context;

  MagickBooleanType
    filter_off;         /* true if not filtering attributes */

  /* Pretty-printing depth */
  unsigned long
    indent_depth;       /* number of left-hand pad characters */

  /* Path operation support */
  PathOperation
    path_operation;

  PathMode
    path_mode;

  MagickBooleanType
    destroy,
    debug;

  unsigned long
    signature;
};

/* ************************************************************************************************************** */
/* MagicWand definition */
/*
  Typedef declarations.
*/
struct _MagickWand
{
  unsigned long
    id;

  char
    name[MaxTextExtent];

  ExceptionInfo
    exception;

  ImageInfo
    *image_info;

  QuantizeInfo
    *quantize_info;

  Image
    *images;

  MagickBooleanType
    iterator,
    debug;

  unsigned long
    signature;
};

/* ************************************************************************************************************** */
/* Pixeliterator definition */
/*
  Typedef declarations.
*/
struct _PixelIterator
{
  unsigned long
    id;

  char
    name[MaxTextExtent];

  ExceptionInfo
    exception;

  ViewInfo
    *view;

  RectangleInfo
    region;

  long
    y;

  PixelWand
    **pixel_wand;

  MagickBooleanType
    debug;

  unsigned long
    signature;
};

/* ************************************************************************************************************** */
/* PixelWand definition */
/*
  Typedef declarations.
*/
struct _PixelWand
{
  unsigned long
    id;

  char
    name[MaxTextExtent];

  ExceptionInfo
    exception;

  MagickPixelPacket
    pixel;

  unsigned long
    count;

  MagickBooleanType
    debug;

  unsigned long
    signature;
};
