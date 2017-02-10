#include <libgimp/gimp.h>

static void query (void);
static void run   (const gchar      *name,
                   gint              nparams,
                   const GimpParam  *param,
                   gint             *nreturn_vals,
                   GimpParam       **return_vals);

GimpPlugInInfo PLUG_IN_INFO =
{
  NULL,
  NULL,
  query,
  run
};
#define PLUGIN_NAME "plug-in-Fix-Text-Bg"
void	fix_text_bg(GimpDrawable *drawable);

MAIN()

static void
query (void)
{
  static GimpParamDef args[] =
  {
    {
      GIMP_PDB_INT32,
      "run-mode",
      "Run mode"
    },
    {
      GIMP_PDB_IMAGE,
      "image",
      "Input image"
    },
    {
      GIMP_PDB_DRAWABLE,
      "drawable",
      "Input drawable"
    }
  };

  gimp_install_procedure (
	PLUGIN_NAME,
	"Fix-Text-Bg",
	"Clear text background of text pages taken by a camera. Adapts to varying background levels and shadows across the whole image",
	"udif",
	"Copyright (C) udif, Um6ra1",
	"2017",
	"_FixTextBg",
    "RGB*, GRAY*",
    GIMP_PLUGIN,
    G_N_ELEMENTS (args), 0,
    args, NULL);

  gimp_plugin_menu_register (PLUGIN_NAME, "<Image>/Filters/VsNative-Plugin");
}

static void
run (const gchar      *name,
     gint              nparams,
     const GimpParam  *param,
     gint             *nreturn_vals,
     GimpParam       **return_vals)
{
	static GimpParam  values[1];
	GimpPDBStatusType status = GIMP_PDB_SUCCESS;
	GimpRunMode       run_mode;

	/* Setting mandatory output values */
	*nreturn_vals = 1;
	*return_vals  = values;

	values[0].type = GIMP_PDB_STATUS;
	values[0].data.d_status = status;

	/* Getting run_mode - we won't display a dialog if 
	* we are in NONINTERACTIVE mode */
	run_mode = (GimpRunMode)param[0].data.d_int32;

	GimpDrawable *drawable = (GimpDrawable *)gimp_drawable_get(param[2].data.d_drawable);
#if 0
	switch (run_mode) {
		case GIMP_RUN_INTERACTIVE:
			/* Get options last values if needed */
			gimp_get_data(PLUGIN_NAME, &vals);

			/* Display the dialog */
			if (! fix_txt_bg_dialog (drawable))
				return;
			break;

		case GIMP_RUN_NONINTERACTIVE:
			if (nparams != 6)
				status = GIMP_PDB_CALLING_ERROR;
			if (status == GIMP_PDB_SUCCESS) {
				vals.kernel_size = param[3].data.d_int32;
				vals.inner_size = param[4].data.d_int32;
				vals.thresh_adjust = param[5].data.d_int32;
			}
			break;

		case GIMP_RUN_WITH_LAST_VALS:
			/*  Get options last values if needed  */
			gimp_get_data(PLUGIN_NAME, &vals);
			break;

		default:
			break;
	  }
#endif
	fix_text_bg(drawable);

	gimp_displays_flush();
	gimp_drawable_detach(drawable);

#if 0
	/*  Finally, set options in the core  */
	if (run_mode == GIMP_RUN_INTERACTIVE)
	  gimp_set_data (PLUGIN_NAME, &bvals, sizeof (MyBlurVals));
#endif
}
