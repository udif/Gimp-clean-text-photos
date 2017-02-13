#include <vector>

#include "fix-text-bg.h"

void	fix_text_bg(GimpDrawable *drawable, PluginVals *v, GimpPreview  *preview)
{
	int hist[256];
	int x1, y1, x2, y2;
	int width, height;
	GimpPixelRgn	rgnDst, rgnSrc;

	/* Gets upper left and lower right coordinates,
	 * and layers number in the image */
	if (preview) {
	  gimp_preview_get_position (preview, &x1, &y1);
	  gimp_preview_get_size (preview, &width, &height);
	  x2 = x1 + width;
	  y2 = y1 + height;
	} else {
	  gimp_drawable_mask_bounds (drawable->drawable_id, &x1, &y1, &x2, &y2);
	  width = x2 - x1;
	  height = y2 - y1;
	}

	const int channels = (int)gimp_drawable_bpp(drawable->drawable_id);
	const bool grey = (channels == 1);
	gimp_pixel_rgn_init(&rgnDst, drawable, x1, y1, width, height, preview == NULL, TRUE);
	gimp_pixel_rgn_init(&rgnSrc, drawable, x1, y1, width, height, FALSE, FALSE);

	std::vector<BYTE>	buf(width * height * channels);
	std::vector<BYTE>	bufout(width * height);

	gimp_pixel_rgn_get_rect(&rgnSrc, &buf[0], x1, y1, width, height);
	const int kernel_size = v->kernel_size;
	const int inner_size = v->inner_size;
	const int inner_step = 2 * inner_size + 1;
	const int kernel_area = (kernel_size * 2 + 1) * (kernel_size * 2 + 1);
	// Desaturate first, using "Luminocity" algorithm
	// store it in the red channel only, because we'll modify it anyhow
	if (channels >= 3) {
		for (int y = 0; y < height; y++) {
			for(int x = 0; x < width; x++) {
				UINT32 &pixel = *(UINT32 *)&buf[channels * (width * y + x)];
				BYTE	&r = ((BYTE *)&pixel)[0];
				BYTE	&g = ((BYTE *)&pixel)[1];
				BYTE	&b = ((BYTE *)&pixel)[2];
				r = (21 * r + 72 * g + 7 * b) / 100;
			}
		}
	} else if (!grey) {
		g_message("Unknown image type!");
		return;
	}

	// Now, do your work
	for (int y = kernel_size; y < height - kernel_size; y += inner_step) {
		for (int x = kernel_size; x < width - kernel_size; x += inner_step) {
			memset(hist, 0, sizeof(hist));
			int avg_val = 0;
			if (grey) {
				for (int dy = -kernel_size; dy <= kernel_size; dy++) {
					for (int dx = -kernel_size; dx <= kernel_size; dx++) {
						BYTE &val = *(BYTE *)&buf[(width * (y + dy) + x + dx)];
						hist[val]++;
						avg_val += val;
					}
				}
			} else {
				for (int dy = -kernel_size; dy <= kernel_size; dy++) {
					for (int dx = -kernel_size; dx <= kernel_size; dx++) {
						UINT32 &pixel = *(UINT32 *)&buf[channels * (width * (y + dy) + x + dx)];
						BYTE &val = ((BYTE *)&pixel)[0];
						hist[val]++;
						avg_val += val;
					}
				}
			}
			avg_val /= kernel_area;
			// Find histogram maximum point
			int max_val = hist[0];
			int max_idx = 0;
			for (int i = 1; i < 256; i++) {
				if (hist[i] > max_val) {
					max_val = hist[i];
					max_idx = i;
				}
			}
			// Find first knee point in histogram below max_val
			int last_val = max_val;
			int bg_thresh;
			for (bg_thresh = max_idx-1; bg_thresh >= 0; bg_thresh--) {
				if (last_val < hist[bg_thresh])
					break;
				last_val = hist[bg_thresh];
			}

			// If avg_val is higher than peak it means that the peak is the darker characters
			// Increase threshold to prevent earsing characters
			if (max_idx < avg_val)
				bg_thresh = avg_val;
			// Write result in g,b
			bg_thresh += v->thresh_adjust;

			if (grey) {
				for (int dy = -inner_size; dy <= inner_size; dy++) {
					for (int dx = -inner_size; dx <= inner_size; dx++) {
						BYTE &val = *(BYTE *)&buf[(width * (y + dy) + x + dx)];
						BYTE &valout = *(BYTE *)&bufout[(width * (y + dy) + x + dx)];
						if (val > bg_thresh) {
							valout = 255;
						} else {
							valout = val;
						}
					}
				}
			} else {
				for (int dy = -inner_size; dy <= inner_size; dy++) {
					for (int dx = -inner_size; dx <= inner_size; dx++) {
						UINT32 &pixel = *(UINT32 *)&buf[channels * (width * (y + dy) + x + dx)];
						BYTE	&r = ((BYTE *)&pixel)[0];
						BYTE	&g = ((BYTE *)&pixel)[1];
						if (r > bg_thresh) {
							g = 255;
						}
					}
				}
			}
		}
	}
	
	// Copy result to final output
	if (grey) {
		// No need to copy back to buf, just copy to output
		gimp_pixel_rgn_set_rect(&rgnDst, &bufout[0], x1, y1, width, height);
	} else {
		for (int y = 0; y < height; y++) {
			for(int x = 0; x < width; x++) {
				UINT32 &pixel = *(UINT32 *)&buf[channels * (width * y + x)];
				BYTE	&r = ((BYTE *)&pixel)[0];
				BYTE	&g = ((BYTE *)&pixel)[1];
				BYTE	&b = ((BYTE *)&pixel)[2];
				r = b = g;
			}
		}
		gimp_pixel_rgn_set_rect(&rgnDst, &buf[0], x1, y1, width, height);
	}

	gimp_drawable_flush(drawable);
	gimp_drawable_merge_shadow(drawable->drawable_id, true);
	gimp_drawable_update(drawable->drawable_id, x1, y1, width, height);
}
