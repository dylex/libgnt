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

#ifndef GNT_BUTTON_H
#define GNT_BUTTON_H
/**
 * SECTION:gntbutton
 * @section_id: libgnt-gntbutton
 * @title: GntButton
 * @short_description: A widget that can be activated
 * @see_also: #GntCheckBox
 */

#include <glib.h>
#include <glib-object.h>
#include "gnt.h"
#include "gntwidget.h"

#define GNT_TYPE_BUTTON				(gnt_button_get_gtype())
#define GNT_BUTTON(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj), GNT_TYPE_BUTTON, GntButton))
#define GNT_BUTTON_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass), GNT_TYPE_BUTTON, GntButtonClass))
#define GNT_IS_BUTTON(obj)			(G_TYPE_CHECK_INSTANCE_TYPE((obj), GNT_TYPE_BUTTON))
#define GNT_IS_BUTTON_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass), GNT_TYPE_BUTTON))
#define GNT_BUTTON_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS((obj), GNT_TYPE_BUTTON, GntButtonClass))

typedef struct _GntButton			GntButton;
typedef struct _GntButtonClass		GntButtonClass;
#ifndef GNT_DISABLE_DEPRECATED
/**
 * GntButtonPriv:
 *
 * Deprecated: 2.14.0: This is an internal implementation detail.
 */
typedef struct _GntButtonPriv GntButtonPriv;
#endif

struct _GntButtonPriv
{
	char *GNTSEAL(text);
};

/**
 * GntButton:
 *
 * Access to any fields is deprecated. See inline comments for replacements.
 */
struct _GntButton
{
	GntWidget parent;

	struct _GntButtonPriv *GNTSEAL(priv);

	/*< private >*/
    void (*gnt_reserved1)(void);
    void (*gnt_reserved2)(void);
    void (*gnt_reserved3)(void);
    void (*gnt_reserved4)(void);
};

struct _GntButtonClass
{
	GntWidgetClass parent;

	/*< private >*/
	void (*gnt_reserved1)(void);
	void (*gnt_reserved2)(void);
	void (*gnt_reserved3)(void);
	void (*gnt_reserved4)(void);
};

G_BEGIN_DECLS

/**
 * gnt_button_get_gtype:
 *
 * Returns:  GType for Gntbutton
 */
GType gnt_button_get_gtype(void);

/**
 * gnt_button_new:
 * @text:   The text for the button.
 *
 * Create a new button.
 *
 * Returns:  The newly created button.
 */
GntWidget * gnt_button_new(const char *text);

/**
 * gnt_button_set_text:
 * @button: The button.
 * @text:   The text for the button.
 *
 * Set the text of a button.
 *
 * Since: 2.14.0
 */
void gnt_button_set_text(GntButton *button, const gchar *text);

/**
 * gnt_button_get_text:
 * @button: The button.
 *
 * Get the text of a button.
 *
 * Returns:  The text for the button.
 *
 * Since: 2.14.0
 */
const gchar *gnt_button_get_text(GntButton *button);

G_END_DECLS

#endif /* GNT_BUTTON_H */
