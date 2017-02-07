#pragma once

#include <Windows.h>
#include "glib.h"

#define G_BEGIN_DECLS	extern "C" {
#define G_END_DECLS	}
#define G_GNUC_NORETURN
#define G_GNUC_CONST
#define G_N_ELEMENTS(arr)		(sizeof (arr) / sizeof ((arr)[0]))

typedef struct _GimpRGB	{	double r, g, b, a;	} GimpRGB;
typedef struct _GimpHSV	{	double h, s, v, a;	} GimpHSV;
typedef struct _GimpHSL	{	double h, s, l, a;	} GimpHSL;
typedef struct _GimpCMYK	{	double c, m, y, k, a;	} GimpCMYK;
struct GimpTile;
typedef struct _GimpParasite
{
	char    *name;   /* The name of the parasite. USE A UNIQUE PREFIX! */
	UINT32   flags;  /* save Parasite in XCF file, etc.                */
	UINT32   size;   /* amount of data                                 */
	LPVOID  data;   /* a pointer to the data.  plugin is              *
                     * responsible for tracking byte order            */
} GimpParasite;
typedef struct {
  gint32    drawable_id;   /* drawable ID */
  guint     width;         /* width of drawble */
  guint     height;        /* height of drawble */
  guint     bpp;           /* bytes per pixel of drawable */
  guint     ntile_rows;    /* # of tile rows */
  guint     ntile_cols;    /* # of tile columns */
  GimpTile *tiles;         /* the normal tiles */
  GimpTile *shadow_tiles;  /* the shadow tiles */
} GimpDrawable;
typedef struct
{
  guchar       *data;          /* pointer to region data */
  GimpDrawable *drawable;      /* pointer to drawable */
  gint          bpp;           /* bytes per pixel */
  gint          rowstride;     /* bytes per pixel row */
  gint          x, y;          /* origin */
  gint          w, h;          /* width and height of region */
  guint         dirty : 1;     /* will this region be dirtied? */
  guint         shadow : 1;    /* will this region use the shadow or normal tiles */
  gint          process_count; /* used internally */
} GimpPixelRgn;

typedef enum {
  GIMP_RUN_INTERACTIVE,     /*< desc="Run interactively"         >*/
  GIMP_RUN_NONINTERACTIVE,  /*< desc="Run non-interactively"     >*/
  GIMP_RUN_WITH_LAST_VALS   /*< desc="Run with last used values" >*/
} GimpRunMode;
typedef enum {
  GIMP_ICON_TYPE_STOCK_ID,      /*< desc="Stock ID"      >*/
  GIMP_ICON_TYPE_INLINE_PIXBUF, /*< desc="Inline pixbuf" >*/
  GIMP_ICON_TYPE_IMAGE_FILE     /*< desc="Image file"    >*/
} GimpIconType;
typedef enum _GimpPDBArgType
{
	GIMP_PDB_INT32,
	GIMP_PDB_INT16,
	GIMP_PDB_INT8,
	GIMP_PDB_FLOAT,
	GIMP_PDB_STRING,
	GIMP_PDB_INT32ARRAY,
	GIMP_PDB_INT16ARRAY,
	GIMP_PDB_INT8ARRAY,
	GIMP_PDB_FLOATARRAY,
	GIMP_PDB_STRINGARRAY,
	GIMP_PDB_COLOR,
	GIMP_PDB_ITEM,
	GIMP_PDB_DISPLAY,
	GIMP_PDB_IMAGE,
	GIMP_PDB_LAYER,
 	GIMP_PDB_CHANNEL,
	GIMP_PDB_DRAWABLE,
	GIMP_PDB_SELECTION,
	GIMP_PDB_COLORARRAY,
	GIMP_PDB_VECTORS,
	GIMP_PDB_PARASITE,
	GIMP_PDB_STATUS,
	GIMP_PDB_END,

	/*  the following aliases are deprecated  */
	GIMP_PDB_PATH     = GIMP_PDB_VECTORS,     /*< skip >*/
	GIMP_PDB_BOUNDARY = GIMP_PDB_COLORARRAY,  /*< skip >*/
	GIMP_PDB_REGION   = GIMP_PDB_ITEM         /*< skip >*/
} GimpPDBArgType;
typedef enum
{
  GIMP_PDB_EXECUTION_ERROR,
  GIMP_PDB_CALLING_ERROR,
  GIMP_PDB_PASS_THROUGH,
  GIMP_PDB_SUCCESS,
  GIMP_PDB_CANCEL
} GimpPDBStatusType;
typedef enum
{
	GIMP_INTERNAL,   /*< desc="Internal GIMP procedure" >*/
	GIMP_PLUGIN,     /*< desc="GIMP Plug-In" >*/
	GIMP_EXTENSION,  /*< desc="GIMP Extension" >*/
	GIMP_TEMPORARY   /*< desc="Temporary Procedure" >*/
} GimpPDBProcType;
typedef enum
{
	GIMP_DEBUG_PID = 1 << 0,
	GIMP_DEBUG_FATAL_WARNINGS = 1 << 1,
	GIMP_DEBUG_QUERY = 1 << 2,
	GIMP_DEBUG_INIT = 1 << 3,
	GIMP_DEBUG_RUN = 1 << 4,
	GIMP_DEBUG_QUIT = 1 << 5,

	GIMP_DEBUG_DEFAULT = (GIMP_DEBUG_RUN | GIMP_DEBUG_FATAL_WARNINGS),
} GimpDebugFlag;
typedef enum  /*< pdb-skip >*/
{
  GIMP_CHECK_SIZE_SMALL_CHECKS  = 0,  /*< desc="Small"  >*/
  GIMP_CHECK_SIZE_MEDIUM_CHECKS = 1,  /*< desc="Medium" >*/
  GIMP_CHECK_SIZE_LARGE_CHECKS  = 2   /*< desc="Large"  >*/
} GimpCheckSize;
typedef enum  /*< pdb-skip >*/
{
  GIMP_CHECK_TYPE_LIGHT_CHECKS = 0,  /*< desc="Light checks"    >*/
  GIMP_CHECK_TYPE_GRAY_CHECKS  = 1,  /*< desc="Mid-tone checks" >*/
  GIMP_CHECK_TYPE_DARK_CHECKS  = 2,  /*< desc="Dark checks"     >*/
  GIMP_CHECK_TYPE_WHITE_ONLY   = 3,  /*< desc="White only"      >*/
  GIMP_CHECK_TYPE_GRAY_ONLY    = 4,  /*< desc="Gray only"       >*/
  GIMP_CHECK_TYPE_BLACK_ONLY   = 5   /*< desc="Black only"      >*/
} GimpCheckType;
typedef enum
{
  GIMP_STACK_TRACE_NEVER,
  GIMP_STACK_TRACE_QUERY,
  GIMP_STACK_TRACE_ALWAYS
} GimpStackTraceMode;
typedef enum /*< skip >*/
{
  GIMP_UNIT_PIXEL   = 0,
  GIMP_UNIT_INCH    = 1,
  GIMP_UNIT_MM      = 2,
  GIMP_UNIT_POINT   = 3,
  GIMP_UNIT_PICA    = 4,
  GIMP_UNIT_END     = 5,
  GIMP_UNIT_PERCENT = 65536 /*< pdb-skip >*/
} GimpUnit;
/*
typedef void (__cdecl *GimpInitProc)();
typedef void (__cdecl *GimpQuitProc)();
typedef void (__cdecl *GimpQueryProc)();
typedef void (__cdecl *GimpRunProc)(LPCSTR name, int n_params, const GimpParam *param, int *n_return_vals, GimpParam **return_vals);
*/

struct GPParamDef;

typedef struct _GPProcInstall
{
	char      *name;
	char      *blurb;
	char      *help;
	char      *author;
	char      *copyright;
	char      *date;
	char      *menu_path;
	char      *image_types;
	UINT32     type;
	UINT32     nparams;
	UINT32     nreturn_vals;
	GPParamDef *params;
	GPParamDef *return_vals;
} GPProcInstall;

typedef struct _GimpPlugInInfo	GimpPlugInInfo;
typedef struct _GimpParamDef	GimpParamDef;
typedef struct _GimpParamRegion	GimpParamRegion;
typedef union _GimpParamData	GimpParamData;
typedef struct _GimpParam		GimpParam;
