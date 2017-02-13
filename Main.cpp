#include "fix-text-bg.h"

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

/* Set up default values for options */
PluginVals vals =
{
	50,  // kernel size
	20, // inner size
	-10, // threshold adjustment
	FALSE
};

MAIN()

static void
query (void)
{
	g_message(PLUGIN_SHORT_NAME);
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
    },
    {
      GIMP_PDB_INT32,
      "kernel-size",
      "The square radius of the area we look at"
    },
    {
      GIMP_PDB_INT32,
      "inner-size",
      "The square radius of the area we actually work on"
    },
    {
      GIMP_PDB_INT32,
      "Threshold",
      "Threshold added to background level calculated"
    }
  };

  gimp_install_procedure (
	PLUGIN_NAME,
	"Fix-Text-Bg" DBG_SUFFIX,
	"Clear text background of text pages taken by a camera. Adapts to varying background levels and shadows across the whole image",
	"udif",
	"Copyright (C) udif",
	"2017",
	"_FixTextBg" DBG_SUFFIX,
    "RGB*, GRAY*",
    GIMP_PLUGIN,
    G_N_ELEMENTS (args), 0,
    args, NULL);

  gimp_plugin_menu_register (PLUGIN_NAME, "<Image>/Filters/VsNative-Plugin" DBG_SUFFIX);
}

typedef struct {
	char *name;
	char *label;
	int *default_val;
	int *low_val;
	int *high_val;
} dialog_params;

static int kernel_size_max;
static int inner_size_min = 1;
static int thresh_adjust_min = -255;
static int thresh_adjust_max = 255;

static dialog_params fix_text_bg_params[] = {
	{
		"Kernel Size",
		"_KSize",
		&vals.kernel_size,
		&vals.inner_size,
		&kernel_size_max
	},
	{
		"Inner Size",
		"_ISize",
		&vals.inner_size,
		&inner_size_min,
		&vals.kernel_size
	},
	{
		"Background threshold adjust",
		"_Threshold",
		&vals.thresh_adjust,
		&thresh_adjust_min,
		&thresh_adjust_max
	}
};

#define PNUM (sizeof(fix_text_bg_params)/sizeof(fix_text_bg_params[0]))

static gboolean fix_txt_bg_dialog (GimpDrawable *drawable)
{
	GtkWidget *dialog;
	GtkWidget *main_vbox;
	GtkWidget *preview;
	GtkWidget *main_hbox[PNUM];
	GtkWidget *frame[PNUM];
	GtkWidget *label[PNUM];
	GtkWidget *alignment[PNUM];
	GtkWidget *spinbutton[PNUM];
	GtkAdjustment  *spinbutton_adj[PNUM];
	gboolean   run;
	int x1, y1, x2, y2;

	g_message(PLUGIN_SHORT_NAME);
	gimp_ui_init ("Fix-Text-Bg" DBG_SUFFIX, FALSE);

#ifdef GIMP_DEBUG
	// This is enabled automatically when running a Visual C++ Debug build.
	g_message("If you would like to debug, now is a good time to select 'Debug/Attach to process'\n"
		"From the Visual Studio menu (<Ctrl>+<Alt>+P also works).\n");
#endif
	gimp_drawable_mask_bounds(drawable->drawable_id, &x1, &y1, &x2, &y2);
	const int width = x2 - x1;
	const int height = y2 - y1;
	kernel_size_max = min(width,height)/2-1;

	dialog = gimp_dialog_new ("Fix-Text-Bg" DBG_SUFFIX, "Fix-Text-Bg" DBG_SUFFIX,
							  NULL, (GtkDialogFlags)0,
							  gimp_standard_help_func, PLUGIN_NAME,

							  GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
							  GTK_STOCK_OK,     GTK_RESPONSE_OK,

							  NULL);

	main_vbox = gtk_vbox_new (FALSE, 6);
	gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog)->vbox), main_vbox);
	gtk_widget_show (main_vbox);

	//preview = gimp_drawable_preview_new (drawable, &vals.preview);
	//gtk_box_pack_start (GTK_BOX (main_vbox), preview, TRUE, TRUE, 0);
	//gtk_widget_show (preview);

	for (int i = 0; i < 3; i++) {
		frame[i] = gimp_frame_new (fix_text_bg_params[i].name);
		gtk_box_pack_start (GTK_BOX (main_vbox), frame[i], FALSE, FALSE, 0);
		gtk_widget_show (frame[i]);

		alignment[i] = gtk_alignment_new (0.5, 0.5, 1, 1);
		gtk_widget_show (alignment[i]);
		gtk_container_add (GTK_CONTAINER (frame[i]), alignment[i]);
		gtk_alignment_set_padding (GTK_ALIGNMENT (alignment[i]), 6, 6, 6, 6);

		main_hbox[i] = gtk_hbox_new (FALSE, 12);
		gtk_container_set_border_width (GTK_CONTAINER (main_hbox[i]), 12);
		gtk_container_add (GTK_CONTAINER (alignment[i]), main_hbox[i]);
		gtk_widget_show (main_hbox[i]);

		label[i] = gtk_label_new_with_mnemonic (fix_text_bg_params[i].label);
		gtk_box_pack_start (GTK_BOX (main_hbox[i]), label[i], FALSE, FALSE, 6);
		gtk_label_set_justify (GTK_LABEL (label[i]), GTK_JUSTIFY_RIGHT);
		gtk_widget_show (label[i]);

		spinbutton_adj[i] = (GtkAdjustment *)gtk_adjustment_new (*(fix_text_bg_params[i].default_val),
			*(fix_text_bg_params[i].low_val),
			*(fix_text_bg_params[i].high_val),
			1, 5, 5);
		spinbutton[i] = gtk_spin_button_new (spinbutton_adj[i], 1, 0);
		gtk_box_pack_start (GTK_BOX (main_hbox[i]), spinbutton[i], FALSE, FALSE, 0);
		gtk_widget_show (spinbutton[i]);

	#if 0
		g_signal_connect_swapped (preview, "invalidated",
								  G_CALLBACK (fix_text_bg),
								  drawable);
		g_signal_connect_swapped (spinbutton_adj, "value_changed",
								  G_CALLBACK (gimp_preview_invalidate),
								  preview);
	#endif
		g_signal_connect (spinbutton_adj[i], "value_changed",
						  G_CALLBACK (gimp_int_adjustment_update),
						  fix_text_bg_params[i].default_val);
	}
	gtk_widget_show (dialog);

	run = (gimp_dialog_run (GIMP_DIALOG (dialog)) == GTK_RESPONSE_OK);

	//fix_text_bg (drawable, GIMP_PREVIEW (preview));

	gtk_widget_destroy (dialog);

	return run;
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

	switch (run_mode) {
		case GIMP_RUN_INTERACTIVE:
			/* Get options last values if needed */
			gimp_get_data(PLUGIN_NAME, &vals);

			/* Display the dialog */
			if (! fix_txt_bg_dialog (drawable)) {
                status = GIMP_PDB_CANCEL;
				return;
			}
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

	if (status == GIMP_PDB_SUCCESS) {
		fix_text_bg(drawable, &vals, NULL);

		gimp_displays_flush();
		gimp_drawable_detach(drawable);


		/*  Finally, set options in the core  */
		if (run_mode == GIMP_RUN_INTERACTIVE)
			gimp_set_data (PLUGIN_NAME, &vals, sizeof (PluginVals));
	}
}
