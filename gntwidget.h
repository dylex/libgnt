/**
 * @file gntwidget.h Widget API
 * @ingroup gnt
 */
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

#ifndef GNT_WIDGET_H
#define GNT_WIDGET_H

#include <stdio.h>
#include <glib.h>
#include <ncurses.h>

#include "gntbindable.h"

#define GNT_TYPE_WIDGET				(gnt_widget_get_gtype())
#define GNT_WIDGET(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj), GNT_TYPE_WIDGET, GntWidget))
#define GNT_WIDGET_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass), GNT_TYPE_WIDGET, GntWidgetClass))
#define GNT_IS_WIDGET(obj)			(G_TYPE_CHECK_INSTANCE_TYPE((obj), GNT_TYPE_WIDGET))
#define GNT_IS_WIDGET_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass), GNT_TYPE_WIDGET))
#define GNT_WIDGET_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS((obj), GNT_TYPE_WIDGET, GntWidgetClass))

#define GNT_WIDGET_FLAGS(obj)				(GNT_WIDGET(obj)->priv.flags)
#define GNT_WIDGET_SET_FLAGS(obj, flags)		(GNT_WIDGET_FLAGS(obj) |= flags)
#define GNT_WIDGET_UNSET_FLAGS(obj, flags)	(GNT_WIDGET_FLAGS(obj) &= ~(flags))
#define GNT_WIDGET_IS_FLAG_SET(obj, flags)	(GNT_WIDGET_FLAGS(obj) & (flags))

typedef struct _GntWidget			GntWidget;
typedef struct _GntWidgetPriv		GntWidgetPriv;
typedef struct _GntWidgetClass		GntWidgetClass;

typedef enum _GntWidgetFlags
{
	GNT_WIDGET_DESTROYING     = 1 << 0,
	GNT_WIDGET_CAN_TAKE_FOCUS = 1 << 1,
	GNT_WIDGET_MAPPED         = 1 << 2,
	/* XXX: Need to set the following two as properties, and setup a callback whenever these
	 * get chnaged. */
	GNT_WIDGET_NO_BORDER      = 1 << 3,
	GNT_WIDGET_NO_SHADOW      = 1 << 4,
	GNT_WIDGET_HAS_FOCUS      = 1 << 5,
	GNT_WIDGET_DRAWING        = 1 << 6,
	GNT_WIDGET_URGENT         = 1 << 7,
	GNT_WIDGET_GROW_X         = 1 << 8,
	GNT_WIDGET_GROW_Y         = 1 << 9,
	GNT_WIDGET_INVISIBLE      = 1 << 10,
	GNT_WIDGET_TRANSIENT      = 1 << 11,
	GNT_WIDGET_DISABLE_ACTIONS = 1 << 12,
} GntWidgetFlags;

/* XXX: This will probably move elsewhere */
typedef enum _GntMouseEvent
{
	GNT_LEFT_MOUSE_DOWN = 1,
	GNT_RIGHT_MOUSE_DOWN,
	GNT_MIDDLE_MOUSE_DOWN,
	GNT_MOUSE_UP,
	GNT_MOUSE_SCROLL_UP,
	GNT_MOUSE_SCROLL_DOWN
} GntMouseEvent;

/* XXX: I'll have to ask grim what he's using this for in guifications. */
typedef enum _GntParamFlags
{
	GNT_PARAM_SERIALIZABLE	= 1 << G_PARAM_USER_SHIFT
} GntParamFlags;

struct _GntWidgetPriv
{
	int x, y;
	int width, height;
	GntWidgetFlags flags;
	char *name;

	int minw, minh;    /* Minimum size for the widget */
};

struct _GntWidget
{
	GntBindable inherit;

	GntWidget *parent;

	GntWidgetPriv priv;
	WINDOW *window;

    void (*gnt_reserved1)(void);
    void (*gnt_reserved2)(void);
    void (*gnt_reserved3)(void);
    void (*gnt_reserved4)(void);
};

struct _GntWidgetClass
{
	GntBindableClass parent;

	void (*map)(GntWidget *obj);
	void (*show)(GntWidget *obj);		/* This will call draw() and take focus (if it can take focus) */
	void (*destroy)(GntWidget *obj);
	void (*draw)(GntWidget *obj);		/* This will draw the widget */
	void (*hide)(GntWidget *obj);
	void (*expose)(GntWidget *widget, int x, int y, int width, int height);
	void (*gained_focus)(GntWidget *widget);
	void (*lost_focus)(GntWidget *widget);

	void (*size_request)(GntWidget *widget);
	gboolean (*confirm_size)(GntWidget *widget, int x, int y);
	void (*size_changed)(GntWidget *widget, int w, int h);
	void (*set_position)(GntWidget *widget, int x, int y);
	gboolean (*key_pressed)(GntWidget *widget, const char *key);
	void (*activate)(GntWidget *widget);
	gboolean (*clicked)(GntWidget *widget, GntMouseEvent event, int x, int y);

	void (*gnt_reserved1)(void);
	void (*gnt_reserved2)(void);
	void (*gnt_reserved3)(void);
	void (*gnt_reserved4)(void);
};

G_BEGIN_DECLS

/**
 * @return  GType for GntWidget.
 */
GType gnt_widget_get_gtype(void);

/**
 * Destroy a widget.
 * @param widget The widget to destroy.
 */
void gnt_widget_destroy(GntWidget *widget);

/**
 * Show a widget. This should only be used for toplevel widgets. For the rest
 * of the widgets, use #gnt_widget_draw instead.
 *
 * @param widget  The widget to show.
 */
void gnt_widget_show(GntWidget *widget);

/**
 * Draw a widget.
 * @param widget   The widget to draw.
 */
void gnt_widget_draw(GntWidget *widget);

/**
 * @internal
 * Expose part of a widget.
 */
void gnt_widget_expose(GntWidget *widget, int x, int y, int width, int height);

/**
 * Hide a widget.
 * @param widget   The widget to hide.
 */
void gnt_widget_hide(GntWidget *widget);

/**
 * Get the position of a widget.
 *
 * @param widget  The widget.
 * @param x       Location to store the x-coordinate of the widget.
 * @param y       Location to store the y-coordinate of the widget.
 */
void gnt_widget_get_position(GntWidget *widget, int *x, int *y);

/**
 * Set the position of a widget.
 * @param widget   The widget to reposition.
 * @param x        The x-coordinate of the widget.
 * @param y        The x-coordinate of the widget.
 */
void gnt_widget_set_position(GntWidget *widget, int x, int y);

/**
 * Request a widget to calculate its desired size.
 * @param widget  The widget.
 */
void gnt_widget_size_request(GntWidget *widget);

/**
 * Get the size of a widget.
 * @param widget    The widget.
 * @param width     Location to store the width of the widget.
 * @param height    Location to store the height of the widget.
 */
void gnt_widget_get_size(GntWidget *widget, int *width, int *height);

/**
 * Set the size of a widget.
 *
 * @param widget  The widget to resize.
 * @param width   The width of the widget.
 * @param height  The height of the widget.
 *
 * @return  If the widget was resized to the new size.
 */
gboolean gnt_widget_set_size(GntWidget *widget, int width, int height);

/**
 * Confirm a requested a size for a widget.
 *
 * @param widget   The widget.
 * @param width    The requested width.
 * @param height    The requested height.
 *
 * @return  @c TRUE if the new size was confirmed, @c FALSE otherwise.
 */
gboolean gnt_widget_confirm_size(GntWidget *widget, int width, int height);

/**
 * Trigger the key-press callbacks for a widget.
 *
 * @param widget  The widget.
 * @param keys    The keypress on the widget.
 *
 * @return  @c TRUE if the key-press was handled, @c FALSE otherwise.
 */
gboolean gnt_widget_key_pressed(GntWidget *widget, const char *keys);

/**
 * Trigger the 'click' callback of a widget.
 *
 * @param widget   The widget.
 * @param event    The mouseevent.
 * @param x        The x-coordinate of the mouse.
 * @param y        The y-coordinate of the mouse.
 *
 * @return  @c TRUE if the event was handled, @c FALSE otherwise.
 */
gboolean gnt_widget_clicked(GntWidget *widget, GntMouseEvent event, int x, int y);

/**
 * Give or remove focus to a widget.
 * @param widget  The widget.
 * @param set     @c TRUE of focus should be given to the widget, @c FALSE if
 *                focus should be removed.
 *
 * @return @c TRUE if the focus has been changed, @c FALSE otherwise.
 */
gboolean gnt_widget_set_focus(GntWidget *widget, gboolean set);

/**
 * Activate a widget. This only applies to widgets that can be activated (eg. GntButton)
 * @param widget  The widget to activate.
 */
void gnt_widget_activate(GntWidget *widget);

/**
 * Set the name of a widget.
 * @param widget   The widget.
 * @param name     A new name for the widget.
 */
void gnt_widget_set_name(GntWidget *widget, const char *name);

/**
 * Get the name of a widget.
 * @param widget   The widget.
 * @return The name of the widget.
 */
const char *gnt_widget_get_name(GntWidget *widget);

/**
 * @internal
 * Use #gnt_widget_draw instead.
 */
void gnt_widget_queue_update(GntWidget *widget);

/**
 * Set whether a widget can take focus or not.
 *
 * @param widget   The widget.
 * @param set      @c TRUE if the widget can take focus.
 */
void gnt_widget_set_take_focus(GntWidget *widget, gboolean set);

/**
 * gnt_widget_get_take_focus:
 * @widget:   The widget.
 *
 * Get whether a widget can take focus or not.
 *
 * Returns:   %TRUE if the widget can take focus.
 *
 * Since: 2.9.0
 */
gboolean gnt_widget_get_take_focus(GntWidget *widget);

/**
 * Set the visibility of a widget.
 *
 * @param widget  The widget.
 * @param set     Whether the widget is visible or not.
 */
void gnt_widget_set_visible(GntWidget *widget, gboolean set);

/**
 * gnt_widget_get_visible:
 * @widget:  The widget.
 *
 * Get the visibility of a widget.
 *
 * Returns:  Whether the widget is visible or not.
 *
 * Since: 2.9.0
 */
gboolean gnt_widget_get_visible(GntWidget *widget);

/**
 * Check whether the widget has shadows.
 *
 * @param widget  The widget.
 *
 * @return  @c TRUE if the widget has shadows. This checks both the user-setting
 *          and whether the widget can have shadows at all.
 */
gboolean gnt_widget_has_shadow(GntWidget *widget);

/**
 * gnt_widget_in_destruction
 * @widget: The widget
 *
 * Returns whether the widget is currently being destroyed.
 *
 * This information can sometimes be used to avoid doing unnecessary work.
 *
 * Returns:  %TRUE if the widget is being destroyed.
 *
 * Since: 2.9.0
 */
gboolean gnt_widget_in_destruction(GntWidget *widget);

/**
 * gnt_widget_set_drawing
 * @widget:  The widget
 * @drawing: Whether or not the widget is being drawn
 *
 * Marks the widget as being drawn (or not).
 *
 * Since: 2.9.0
 */
void gnt_widget_set_drawing(GntWidget *widget, gboolean drawing);

/**
 * gnt_widget_get_drawing
 * @widget: The widget
 *
 * Returns whether the widget is currently being drawn.
 *
 * This information can sometimes be used to avoid doing unnecessary work.
 *
 * Returns:  %TRUE if the widget is being drawn.
 *
 * Since: 2.9.0
 */
gboolean gnt_widget_get_drawing(GntWidget *widget);

/**
 * gnt_widget_set_mapped
 * @widget: The widget
 * @mapped: Whether or not the widget is mapped
 *
 * Marks the widget as being mapped (or not).
 *
 * This should generally only be called from the widget's "map" or "unmap"
 * implementation.
 *
 * Since: 2.9.0
 */
void gnt_widget_set_mapped(GntWidget *widget, gboolean mapped);

/**
 * gnt_widget_get_mapped
 * @widget: The widget
 *
 * Whether widget is mapped or not.
 *
 * Returns: Whether the widget is mapped or not.
 *
 * Since: 2.9.0
 */
gboolean gnt_widget_get_mapped(GntWidget *widget);

/**
 * gnt_widget_set_has_border
 * @widget:     The widget
 * @has_border: Whether or not the widget has a border
 *
 * Sets the has-border property on widget to has_border.
 *
 * Since: 2.9.0
 */
void gnt_widget_set_has_border(GntWidget *widget, gboolean has_border);

/**
 * gnt_widget_get_has_border
 * @widget: The widget
 *
 * Returns the has-border property on widget.
 *
 * Returns: Whether the widget has a border or not.
 *
 * Since: 2.9.0
 */
gboolean gnt_widget_get_has_border(GntWidget *widget);

/**
 * gnt_widget_set_has_shadow
 * @widget:     The widget
 * @has_shadow: Whether or not the widget has a shadow
 *
 * Sets the has-shadow property on widget to has_shadow. Note, setting this
 * property does not necessarily mean the widget will have a shadow, depending
 * on its styling.
 *
 * Since: 2.9.0
 */
void gnt_widget_set_has_shadow(GntWidget *widget, gboolean has_shadow);

/**
 * gnt_widget_get_has_shadow
 * @widget: The widget
 *
 * Returns the has-shadow property on widget. Note, this is a property of the
 * widget, but does not necessarily mean the widget will have a shadow as that
 * depends on its styling. Use @gnt_widget_set_has_shadow to determine
 * whether the widget will actually have a shadow.
 *
 * Returns: Whether the widget has a shadow set or not.
 *
 * Since: 2.9.0
 */
gboolean gnt_widget_get_has_shadow(GntWidget *widget);

/**
 * gnt_widget_set_has_focus
 * @widget:    The widget
 * @has_focus: Whether or not the widget has focus
 *
 * Sets the has-focus flag on a widget. Note, setting this flag does not
 * necessarily mean the widget will have focus.
 *
 * This is mostly for internal use; you probably want to use
 * @gnt_widget_set_focus instead.
 *
 * Since: 2.9.0
 */
void gnt_widget_set_has_focus(GntWidget *widget, gboolean has_focus);

/**
 * gnt_widget_get_has_focus:
 * @widget:  The widget
 *
 * Check whether a widget has the focus flag.
 *
 * This is mostly for internal use; you probably want to use
 * @gnt_widget_has_focus instead.
 *
 * Returns:  %TRUE if the widget's focus flag is set, %FALSE otherwise.
 */
gboolean gnt_widget_get_has_focus(GntWidget *widget);

/**
 * gnt_widget_set_is_urgent:
 * @widget: The widget to set the URGENT hint for
 * @urgent: Whether the URGENT hint should be set or not
 *
 * Set the URGENT hint for a widget.
 *
 * Since: 2.9.0
 */
void gnt_widget_set_is_urgent(GntWidget *widget, gboolean urgent);

/**
 * gnt_widget_get_urgent
 * @widget: The widget
 *
 * Returns whether the widget has the URGENT hint set.
 *
 * Returns: Whether the URGENT hint is set or not.
 *
 * Since: 2.9.0
 */
gboolean gnt_widget_get_is_urgent(GntWidget *widget);

/**
 * gnt_widget_set_grow_x:
 * @widget: The widget
 * @grow_x: Whether the widget should grow or not
 *
 * Whether widget should grow in the x direction.
 *
 * Since: 2.9.0
 */
void gnt_widget_set_grow_x(GntWidget *widget, gboolean grow_x);

/**
 * gnt_widget_get_grow_x
 * @widget: The widget
 *
 * Returns whether the widget should grow in the x direction.
 *
 * Returns: Whether widget should grow in the x direction.
 *
 * Since: 2.9.0
 */
gboolean gnt_widget_get_grow_x(GntWidget *widget);

/**
 * gnt_widget_set_grow_y:
 * @widget: The widget
 * @grow_y: Whether the widget should grow or not
 *
 * Whether widget should grow in the y direction.
 *
 * Since: 2.9.0
 */
void gnt_widget_set_grow_y(GntWidget *widget, gboolean grow_y);

/**
 * gnt_widget_get_grow_y
 * @widget: The widget
 *
 * Returns whether the widget should grow in the y direction.
 *
 * Returns: Whether widget should grow in the y direction.
 *
 * Since: 2.9.0
 */
gboolean gnt_widget_get_grow_y(GntWidget *widget);

/**
 * gnt_widget_set_transient:
 * @widget:    The widget
 * @transient: Whether the widget is transient or not
 *
 * Whether the widget should be transient.
 *
 * Since: 2.9.0
 */
void gnt_widget_set_transient(GntWidget *widget, gboolean transient);

/**
 * gnt_widget_get_transient
 * @widget: The widget
 *
 * Returns whether the widget is transient.
 *
 * Returns: Whether the widget should be transient.
 *
 * Since: 2.9.0
 */
gboolean gnt_widget_get_transient(GntWidget *widget);

/**
 * gnt_widget_set_disable_actions:
 * @widget:          The widget
 * @disable_actions: Whether the widget actions should be disabled or not
 *
 * Whether widget actions should be disabled.
 *
 * Since: 2.9.0
 */
void gnt_widget_set_disable_actions(GntWidget *widget,
                                    gboolean disable_actions);

/**
 * gnt_widget_get_disable_actions
 * @widget: The widget
 *
 * Returns whether the widget actions are disabled.
 *
 * Returns: Whether the widget actions are disabled.
 *
 * Since: 2.9.0
 */
gboolean gnt_widget_get_disable_actions(GntWidget *widget);

G_END_DECLS

#endif /* GNT_WIDGET_H */
