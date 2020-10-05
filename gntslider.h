/*
 * GNT - The GLib Ncurses Toolkit
 *
 * GNT is the legal property of its developers, whose names are too numerous
 * to list here.  Please refer to the COPYRIGHT file distributed with this
 * source distribution.
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111-1301  USA
 */

#ifndef GNT_SLIDER_H
#define GNT_SLIDER_H
/**
 * SECTION:gntslider
 * @section_id: libgnt-gntslider
 * @title: GntSlider
 * @short_description: A widget for selecting from a range of values
 */

#include "gnt.h"
#include "gntlabel.h"
#include "gntwidget.h"

#define GNT_TYPE_SLIDER             (gnt_slider_get_gtype())
#define GNT_SLIDER(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), GNT_TYPE_SLIDER, GntSlider))
#define GNT_SLIDER_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass), GNT_TYPE_SLIDER, GntSliderClass))
#define GNT_IS_SLIDER(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj), GNT_TYPE_SLIDER))
#define GNT_IS_SLIDER_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass), GNT_TYPE_SLIDER))
#define GNT_SLIDER_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj), GNT_TYPE_SLIDER, GntSliderClass))

#ifndef GNT_DISABLE_DEPRECATED
/**
 * GNT_SLIDER_FLAGS:
 *
 * Deprecated: 2.14.0: This is an internal implementation detail.
 */
#define GNT_SLIDER_FLAGS(obj)                (GNT_SLIDER(obj)->priv.flags)
/**
 * GNT_SLIDER_SET_FLAGS:
 *
 * Deprecated: 2.14.0: This is an internal implementation detail.
 */
#define GNT_SLIDER_SET_FLAGS(obj, flags)     (GNT_SLIDER_FLAGS(obj) |= flags)
/**
 * GNT_SLIDER_UNSET_FLAGS:
 *
 * Deprecated: 2.14.0: This is an internal implementation detail.
 */
#define GNT_SLIDER_UNSET_FLAGS(obj, flags)   (GNT_SLIDER_FLAGS(obj) &= ~(flags))
#endif

typedef struct _GntSlider			GntSlider;
typedef struct _GntSliderClass		GntSliderClass;
#ifndef GNT_DISABLE_DEPRECATED
/**
 * GntSliderPriv:
 *
 * Deprecated: 2.14.0: This is an internal implementation detail.
 */
typedef struct _GntSliderPriv GntSliderPriv;
#endif

/**
 * GntSlider:
 *
 * Access to any fields is deprecated. See inline comments for replacements.
 *
 * Since: 2.1.0
 */
struct _GntSlider
{
	GntWidget parent;

	gboolean GNTSEAL(vertical);

	int GNTSEAL(max);        /* maximum value */
	int GNTSEAL(min);        /* minimum value */
	int GNTSEAL(step);       /* amount to change at each step */
	int GNTSEAL(current);    /* current value */
	int GNTSEAL(smallstep);
	int GNTSEAL(largestep);
};

struct _GntSliderClass
{
	GntWidgetClass parent;

	void (*changed)(GntSlider *slider, int);

	/*< private >*/
	void (*gnt_reserved1)(void);
	void (*gnt_reserved2)(void);
	void (*gnt_reserved3)(void);
	void (*gnt_reserved4)(void);
};

G_BEGIN_DECLS

/**
 * gnt_slider_get_gtype:
 *
 * Returns: The GType for GntSlider
 *
 * Since: 2.1.0
 */
GType gnt_slider_get_gtype(void);

#define gnt_hslider_new(max, min) gnt_slider_new(FALSE, max, min)
#define gnt_vslider_new(max, min) gnt_slider_new(TRUE, max, min)

/**
 * gnt_slider_new:
 * @orient: A vertical slider is created if %TRUE, otherwise the slider is horizontal.
 * @max:    The maximum value for the slider
 * @min:    The minimum value for the slider
 *
 * Create a new slider.
 *
 * Returns:  The newly created slider
 *
 * Since: 2.1.0
 */
GntWidget * gnt_slider_new(gboolean orient, int max, int min);

/**
 * gnt_slider_get_vertical:
 * @slider:  The slider
 *
 * Get whether the slider is vertical or not.
 *
 * Returns:  Whether the slider is vertical.
 *
 * Since: 2.14.0
 */
gboolean gnt_slider_get_vertical(GntSlider *slider);

/**
 * gnt_slider_set_range:
 * @slider:  The slider
 * @max:     The maximum value
 * @min:     The minimum value
 *
 * Set the range of the slider.
 *
 * Since: 2.1.0
 */
void gnt_slider_set_range(GntSlider *slider, int max, int min);

/**
 * gnt_slider_get_range:
 * @slider:     The slider
 * @max: (out): The maximum value
 * @min: (out): The minimum value
 *
 * Get the range of the slider.
 *
 * Since: 2.14.0
 */
void gnt_slider_get_range(GntSlider *slider, int *max, int *min);

/**
 * gnt_slider_set_step:
 * @slider:  The slider
 * @step:    The amount for each step
 *
 * Sets the amount of change at each step.
 *
 * Since: 2.1.0
 */
void gnt_slider_set_step(GntSlider *slider, int step);

/**
 * gnt_slider_get_step:
 * @slider:  The slider
 *
 * Gets the amount of change at each step.
 *
 * Returns:  The amount for each step
 *
 * Since: 2.14.0
 */
int gnt_slider_get_step(GntSlider *slider);

/**
 * gnt_slider_set_small_step:
 * @slider:  The slider
 * @step:    The amount for a small step (for the slider)
 *
 * Sets the amount of change a small step.
 *
 * Since: 2.2.0
 */
void gnt_slider_set_small_step(GntSlider *slider, int step);

/**
 * gnt_slider_get_small_step:
 * @slider:  The slider
 *
 * Gets the amount of change for a small step.
 *
 * Returns:  The amount for a small step (of the slider)
 *
 * Since: 2.14.0
 */
int gnt_slider_get_small_step(GntSlider *slider);

/**
 * gnt_slider_set_large_step:
 * @slider:  The slider
 * @step:    The amount for a large step (for the slider)
 *
 * Sets the amount of change a large step.
 *
 * Since: 2.2.0
 */
void gnt_slider_set_large_step(GntSlider *slider, int step);

/**
 * gnt_slider_get_large_step:
 * @slider:  The slider
 *
 * Gets the amount of change for a large step.
 *
 * Returns:  The amount for a large step (of the slider)
 *
 * Since: 2.14.0
 */
int gnt_slider_get_large_step(GntSlider *slider);

/**
 * gnt_slider_advance_step:
 * @slider: The slider
 * @steps:  The number of amounts to change, positive to change forward,
 *          negative to change backward
 *
 * Advance the slider forward or backward.
 *
 * Returns:   The value of the slider after the change
 *
 * Since: 2.1.0
 */
int gnt_slider_advance_step(GntSlider *slider, int steps);

/**
 * gnt_slider_set_value:
 * @slider:  The slider
 * @value:   The current value
 *
 * Set the current value for the slider.
 *
 * Since: 2.1.0
 */
void gnt_slider_set_value(GntSlider *slider, int value);

/**
 * gnt_slider_get_value:
 * @slider: The slider
 *
 * Get the current value for the slider.
 *
 * Since: 2.1.0
 */
int gnt_slider_get_value(GntSlider *slider);

/**
 * gnt_slider_reflect_label:
 * @slider:   The slider
 * @label:    The label to update
 *
 * Update a label with the value of the slider whenever the value changes.
 *
 * Since: 2.1.0
 */
void gnt_slider_reflect_label(GntSlider *slider, GntLabel *label);

G_END_DECLS

#endif /* GNT_SLIDER_H */
