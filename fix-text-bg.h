#ifdef STANDALONE
// Standalone used to work for the original code,
// But since then we've used many more gimp/glib/gtk functions that this will
// not compile as-is
#include <cstdio>
#include "Typedefs.h"
#include "gimp.h"
#else
#include <libgimp/gimp.h>
#include <libgimp/gimpui.h>
#include <gtk/gtk.h>
#endif

#if defined (_WIN32)
#include <Windows.h>
#endif

typedef struct {
	gint kernel_size;
	gint inner_size;
	gint thresh_adjust;
	gboolean preview;
	//gboolean desaturate;
} PluginVals;

extern PluginVals vals;
void	fix_text_bg(GimpDrawable *drawable, PluginVals *v, GimpPreview  *preview);

#ifdef GIMP_DEBUG
#define DBG_SUFFIX "-dbg"
#else
#define DBG_SUFFIX ""
#endif

#define PLUGIN_SHORT_NAME "Fix-Text-Bg" DBG_SUFFIX
#define PLUGIN_NAME "plug-in-" PLUGIN_SHORT_NAME
