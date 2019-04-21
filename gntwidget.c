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
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

/* Stuff brutally ripped from Gflib */

#include "gntinternal.h"
#include "gntwidget.h"
#include "gntstyle.h"
#include "gntutils.h"

/* Yes, there are two "private" types; we should migrate contents of
 * GntWidgetPriv here when we break ABI. */
typedef struct
{
	guint queue_update;
} GntWidgetPrivate;

enum
{
	SIG_DESTROY,
	SIG_DRAW,
	SIG_HIDE,
	SIG_GIVE_FOCUS,
	SIG_LOST_FOCUS,
	SIG_KEY_PRESSED,
	SIG_MAP,
	SIG_ACTIVATE,
	SIG_EXPOSE,
	SIG_SIZE_REQUEST,
	SIG_CONFIRM_SIZE,
	SIG_SIZE_CHANGED,
	SIG_POSITION,
	SIG_CLICKED,
	SIG_CONTEXT_MENU,
	SIGS
};

static guint signals[SIGS] = { 0 };

static void init_widget(GntWidget *widget);

G_DEFINE_TYPE_WITH_PRIVATE(GntWidget, gnt_widget, GNT_TYPE_BINDABLE)

/******************************************************************************
 * GObject Implementation
 *****************************************************************************/
static void
gnt_widget_init(GntWidget *widget)
{
	widget->priv.name = NULL;
}

static void
gnt_widget_map(GntWidget *widget)
{
	/* Get some default size for the widget */
	GNTDEBUG;
	g_signal_emit(widget, signals[SIG_MAP], 0);
	gnt_widget_set_mapped(widget, TRUE);
}

static void
gnt_widget_dispose(GObject *obj)
{
	GntWidget *self = GNT_WIDGET(obj);
	GntWidgetPrivate *priv = gnt_widget_get_instance_private(self);

	g_signal_emit(self, signals[SIG_DESTROY], 0);
	g_source_remove(priv->queue_update);

	G_OBJECT_CLASS(gnt_widget_parent_class)->dispose(obj);
}

static void
gnt_widget_focus_change(GntWidget *widget)
{
	if (gnt_widget_get_mapped(widget)) {
		gnt_widget_draw(widget);
	}
}

static gboolean
gnt_widget_dummy_confirm_size(GntWidget *widget, int width, int height)
{
	gboolean shadow;
	if (width < widget->priv.minw || height < widget->priv.minh) {
		return FALSE;
	}

	shadow = gnt_widget_has_shadow(widget);
	if (widget->priv.width + shadow != width &&
	    !gnt_widget_get_grow_x(widget)) {
		return FALSE;
	}
	if (widget->priv.height + shadow != height &&
	    !gnt_widget_get_grow_y(widget)) {
		return FALSE;
	}

	return TRUE;
}

static gboolean
context_menu(GntBindable *bind, G_GNUC_UNUSED GList *params)
{
	gboolean ret = FALSE;
	g_signal_emit(bind, signals[SIG_CONTEXT_MENU], 0, &ret);
	return ret;
}

static void
gnt_widget_class_init(GntWidgetClass *klass)
{
	GObjectClass *obj_class = G_OBJECT_CLASS(klass);

	obj_class->dispose = gnt_widget_dispose;

	klass->destroy = gnt_widget_destroy;
	klass->show = gnt_widget_show;
	klass->draw = gnt_widget_draw;
	klass->expose = gnt_widget_expose;
	klass->map = gnt_widget_map;
	klass->lost_focus = gnt_widget_focus_change;
	klass->gained_focus = gnt_widget_focus_change;
	klass->confirm_size = gnt_widget_dummy_confirm_size;

	klass->key_pressed = NULL;
	klass->activate = NULL;
	klass->clicked = NULL;

	signals[SIG_DESTROY] =
		g_signal_new("destroy",
					 G_TYPE_FROM_CLASS(klass),
					 G_SIGNAL_RUN_LAST,
					 G_STRUCT_OFFSET(GntWidgetClass, destroy),
					 NULL, NULL, NULL,
					 G_TYPE_NONE, 0);
	signals[SIG_GIVE_FOCUS] =
		g_signal_new("gained-focus",
					 G_TYPE_FROM_CLASS(klass),
					 G_SIGNAL_RUN_LAST,
					 G_STRUCT_OFFSET(GntWidgetClass, gained_focus),
					 NULL, NULL, NULL,
					 G_TYPE_NONE, 0);
	signals[SIG_LOST_FOCUS] =
		g_signal_new("lost-focus",
					 G_TYPE_FROM_CLASS(klass),
					 G_SIGNAL_RUN_LAST,
					 G_STRUCT_OFFSET(GntWidgetClass, lost_focus),
					 NULL, NULL, NULL,
					 G_TYPE_NONE, 0);
	signals[SIG_ACTIVATE] =
		g_signal_new("activate",
					 G_TYPE_FROM_CLASS(klass),
					 G_SIGNAL_RUN_LAST,
					 G_STRUCT_OFFSET(GntWidgetClass, activate),
					 NULL, NULL, NULL,
					 G_TYPE_NONE, 0);
	signals[SIG_MAP] =
		g_signal_new("map",
					 G_TYPE_FROM_CLASS(klass),
					 G_SIGNAL_RUN_LAST,
					 G_STRUCT_OFFSET(GntWidgetClass, map),
					 NULL, NULL, NULL,
					 G_TYPE_NONE, 0);
	signals[SIG_DRAW] =
		g_signal_new("draw",
					 G_TYPE_FROM_CLASS(klass),
					 G_SIGNAL_RUN_LAST,
					 G_STRUCT_OFFSET(GntWidgetClass, draw),
					 NULL, NULL, NULL,
					 G_TYPE_NONE, 0);
	signals[SIG_HIDE] =
		g_signal_new("hide",
					 G_TYPE_FROM_CLASS(klass),
					 G_SIGNAL_RUN_LAST,
					 G_STRUCT_OFFSET(GntWidgetClass, hide),
					 NULL, NULL, NULL,
					 G_TYPE_NONE, 0);
	signals[SIG_EXPOSE] =
		g_signal_new("expose",
					 G_TYPE_FROM_CLASS(klass),
					 G_SIGNAL_RUN_LAST,
					 G_STRUCT_OFFSET(GntWidgetClass, expose),
					 NULL, NULL, NULL,
					 G_TYPE_NONE, 4, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT);
	signals[SIG_POSITION] =
		g_signal_new("position-set",
					 G_TYPE_FROM_CLASS(klass),
					 G_SIGNAL_RUN_LAST,
					 G_STRUCT_OFFSET(GntWidgetClass, set_position),
					 NULL, NULL, NULL,
					 G_TYPE_NONE, 2, G_TYPE_INT, G_TYPE_INT);
	signals[SIG_SIZE_REQUEST] =
		g_signal_new("size_request",
					 G_TYPE_FROM_CLASS(klass),
					 G_SIGNAL_RUN_LAST,
					 G_STRUCT_OFFSET(GntWidgetClass, size_request),
					 NULL, NULL, NULL,
					 G_TYPE_NONE, 0);
	signals[SIG_SIZE_CHANGED] =
		g_signal_new("size_changed",
					 G_TYPE_FROM_CLASS(klass),
					 G_SIGNAL_RUN_LAST,
					 G_STRUCT_OFFSET(GntWidgetClass, size_changed),
					 NULL, NULL, NULL,
					 G_TYPE_NONE, 2, G_TYPE_INT, G_TYPE_INT);
	signals[SIG_CONFIRM_SIZE] =
		g_signal_new("confirm_size",
					 G_TYPE_FROM_CLASS(klass),
					 G_SIGNAL_RUN_LAST,
					 G_STRUCT_OFFSET(GntWidgetClass, confirm_size),
					 NULL, NULL, NULL,
					 G_TYPE_BOOLEAN, 2, G_TYPE_INT, G_TYPE_INT);
	signals[SIG_KEY_PRESSED] =
		g_signal_new("key_pressed",
					 G_TYPE_FROM_CLASS(klass),
					 G_SIGNAL_RUN_LAST,
					 G_STRUCT_OFFSET(GntWidgetClass, key_pressed),
					 g_signal_accumulator_true_handled, NULL, NULL,
					 G_TYPE_BOOLEAN, 1, G_TYPE_STRING);

	signals[SIG_CLICKED] =
		g_signal_new("clicked",
					 G_TYPE_FROM_CLASS(klass),
					 G_SIGNAL_RUN_LAST,
					 G_STRUCT_OFFSET(GntWidgetClass, clicked),
					 g_signal_accumulator_true_handled, NULL, NULL,
					 G_TYPE_BOOLEAN, 3, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT);

	signals[SIG_CONTEXT_MENU] =
		g_signal_new("context-menu",
					 G_TYPE_FROM_CLASS(klass),
					 G_SIGNAL_RUN_LAST,
					 0,
					 g_signal_accumulator_true_handled, NULL, NULL,
					 G_TYPE_BOOLEAN, 0);

	/* This is relevant for all widgets */
	gnt_bindable_class_register_action(GNT_BINDABLE_CLASS(klass), "context-menu", context_menu,
				GNT_KEY_POPUP, NULL);
	gnt_bindable_register_binding(GNT_BINDABLE_CLASS(klass), "context-menu", GNT_KEY_F11, NULL);
	gnt_bindable_register_binding(GNT_BINDABLE_CLASS(klass), "context-menu", GNT_KEY_CTRL_X, NULL);

	gnt_style_read_actions(G_OBJECT_CLASS_TYPE(klass), GNT_BINDABLE_CLASS(klass));
	GNTDEBUG;
}

/******************************************************************************
 * GntWidget API
 *****************************************************************************/
void gnt_widget_set_take_focus(GntWidget *widget, gboolean can)
{
	if (can)
		GNT_WIDGET_SET_FLAGS(widget, GNT_WIDGET_CAN_TAKE_FOCUS);
	else
		GNT_WIDGET_UNSET_FLAGS(widget, GNT_WIDGET_CAN_TAKE_FOCUS);
}

gboolean
gnt_widget_get_take_focus(GntWidget *widget)
{
	g_return_val_if_fail(GNT_IS_WIDGET(widget), FALSE);

	return GNT_WIDGET_IS_FLAG_SET(widget, GNT_WIDGET_CAN_TAKE_FOCUS);
}

void
gnt_widget_destroy(GntWidget *widget)
{
	g_return_if_fail(GNT_IS_WIDGET(widget));

	if (!gnt_widget_in_destruction(widget)) {
		GNT_WIDGET_SET_FLAGS(widget, GNT_WIDGET_DESTROYING);
		gnt_widget_hide(widget);
		delwin(widget->window);
		g_object_run_dispose(G_OBJECT(widget));
	}
	GNTDEBUG;
}

void
gnt_widget_show(GntWidget *widget)
{
	g_return_if_fail(widget != NULL);

	gnt_widget_draw(widget);
	gnt_screen_occupy(widget);
}

void
gnt_widget_draw(GntWidget *widget)
{
	/* Draw the widget */
	if (gnt_widget_get_drawing(widget))
		return;

	gnt_widget_set_drawing(widget, TRUE);
	if (!gnt_widget_get_mapped(widget)) {
		gnt_widget_map(widget);
	}

	if (widget->window == NULL)
	{
		widget->window = newpad(widget->priv.height + 20, widget->priv.width + 20);

		init_widget(widget);
	}

	g_signal_emit(widget, signals[SIG_DRAW], 0);
	gnt_widget_queue_update(widget);
	gnt_widget_set_drawing(widget, FALSE);
}

gboolean
gnt_widget_key_pressed(GntWidget *widget, const char *keys)
{
	gboolean ret;
	if (!gnt_widget_get_take_focus(widget))
		return FALSE;

	if (!gnt_widget_get_disable_actions(widget) &&
	    gnt_bindable_perform_action_key(GNT_BINDABLE(widget), keys)) {
		return TRUE;
	}

	keys = gnt_bindable_remap_keys(GNT_BINDABLE(widget), keys);
	g_signal_emit(widget, signals[SIG_KEY_PRESSED], 0, keys, &ret);
	return ret;
}

gboolean
gnt_widget_clicked(GntWidget *widget, GntMouseEvent event, int x, int y)
{
	gboolean ret;
	g_signal_emit(widget, signals[SIG_CLICKED], 0, event, x, y, &ret);
	if (!ret && event == GNT_RIGHT_MOUSE_DOWN)
		ret = gnt_bindable_perform_action_named(GNT_BINDABLE(widget), "context-menu", NULL);
	return ret;
}

void
gnt_widget_expose(GntWidget *widget, int x, int y, int width, int height)
{
	g_signal_emit(widget, signals[SIG_EXPOSE], 0, x, y, width, height);
}

void
gnt_widget_hide(GntWidget *widget)
{
	g_signal_emit(widget, signals[SIG_HIDE], 0);
	wbkgdset(widget->window, '\0' | gnt_color_pair(GNT_COLOR_NORMAL));
	gnt_screen_release(widget);
	gnt_widget_set_visible(widget, FALSE);
	gnt_widget_set_mapped(widget, FALSE);
}

GntWidget *
gnt_widget_get_parent(GntWidget *widget)
{
	g_return_val_if_fail(GNT_IS_WIDGET(widget), NULL);

	return widget->parent;
}

GntWidget *
gnt_widget_get_toplevel(GntWidget *widget)
{
	g_return_val_if_fail(GNT_IS_WIDGET(widget), NULL);

	while (widget->parent) {
		widget = widget->parent;
	}

	return widget;
}

void
gnt_widget_set_position(GntWidget *wid, int x, int y)
{
	g_signal_emit(wid, signals[SIG_POSITION], 0, x, y);
	/* XXX: Need to install properties for these and g_object_notify */
	wid->priv.x = x;
	wid->priv.y = y;
}

void
gnt_widget_get_position(GntWidget *wid, int *x, int *y)
{
	if (x)
		*x = wid->priv.x;
	if (y)
		*y = wid->priv.y;
}

void
gnt_widget_size_request(GntWidget *widget)
{
	g_signal_emit(widget, signals[SIG_SIZE_REQUEST], 0);
}

void
gnt_widget_get_size(GntWidget *wid, int *width, int *height)
{
	gboolean shadow = TRUE;
	if (!gnt_widget_has_shadow(wid))
		shadow = FALSE;

	if (width)
		*width = wid->priv.width + shadow;
	if (height)
		*height = wid->priv.height + shadow;
}

static void
init_widget(GntWidget *widget)
{
	gboolean shadow = TRUE;

	if (!gnt_widget_has_shadow(widget))
		shadow = FALSE;

	wbkgd(widget->window, gnt_color_pair(GNT_COLOR_NORMAL));
	werase(widget->window);

	if (gnt_widget_get_has_border(widget)) {
		/* - This is ugly. */
		/* - What's your point? */
		mvwvline(widget->window, 0, 0, ACS_VLINE | gnt_color_pair(GNT_COLOR_NORMAL), widget->priv.height);
		mvwvline(widget->window, 0, widget->priv.width - 1,
				ACS_VLINE | gnt_color_pair(GNT_COLOR_NORMAL), widget->priv.height);
		mvwhline(widget->window, widget->priv.height - 1, 0,
				ACS_HLINE | gnt_color_pair(GNT_COLOR_NORMAL), widget->priv.width);
		mvwhline(widget->window, 0, 0, ACS_HLINE | gnt_color_pair(GNT_COLOR_NORMAL), widget->priv.width);
		mvwaddch(widget->window, 0, 0, ACS_ULCORNER | gnt_color_pair(GNT_COLOR_NORMAL));
		mvwaddch(widget->window, 0, widget->priv.width - 1,
				ACS_URCORNER | gnt_color_pair(GNT_COLOR_NORMAL));
		mvwaddch(widget->window, widget->priv.height - 1, 0,
				ACS_LLCORNER | gnt_color_pair(GNT_COLOR_NORMAL));
		mvwaddch(widget->window, widget->priv.height - 1, widget->priv.width - 1,
				ACS_LRCORNER | gnt_color_pair(GNT_COLOR_NORMAL));
	}

	if (shadow)
	{
		wbkgdset(widget->window, '\0' | gnt_color_pair(GNT_COLOR_SHADOW));
		mvwvline(widget->window, 1, widget->priv.width, ' ', widget->priv.height);
		mvwhline(widget->window, widget->priv.height, 1, ' ', widget->priv.width);
	}
}

gboolean
gnt_widget_set_size(GntWidget *widget, int width, int height)
{
	gboolean ret = TRUE;

	if (gnt_widget_has_shadow(widget))
	{
		width--;
		height--;
	}
	if (width <= 0)
		width = widget->priv.width;
	if (height <= 0)
		height = widget->priv.height;

	if (gnt_widget_get_mapped(widget)) {
		ret = gnt_widget_confirm_size(widget, width, height);
	}

	if (ret)
	{
		gboolean shadow = TRUE;
		int oldw, oldh;

		if (!gnt_widget_has_shadow(widget))
			shadow = FALSE;

		oldw = widget->priv.width;
		oldh = widget->priv.height;

		widget->priv.width = width;
		widget->priv.height = height;
		if (width + shadow >= getmaxx(widget->window) || height + shadow >= getmaxy(widget->window)) {
			delwin(widget->window);
			widget->window = newpad(height + 20, width + 20);
		}

		g_signal_emit(widget, signals[SIG_SIZE_CHANGED], 0, oldw, oldh);

		if (widget->window)
		{
			init_widget(widget);
		}
		if (gnt_widget_get_mapped(widget)) {
			init_widget(widget);
		} else {
			gnt_widget_set_mapped(widget, TRUE);
		}
	}

	return ret;
}

gboolean
gnt_widget_set_focus(GntWidget *widget, gboolean set)
{
	if (!gnt_widget_get_take_focus(widget))
		return FALSE;

	if (set && !gnt_widget_get_has_focus(widget)) {
		gnt_widget_set_has_focus(widget, TRUE);
		g_signal_emit(widget, signals[SIG_GIVE_FOCUS], 0);
	} else if (!set && gnt_widget_get_has_focus(widget)) {
		gnt_widget_set_has_focus(widget, FALSE);
		g_signal_emit(widget, signals[SIG_LOST_FOCUS], 0);
	} else {
		return FALSE;
	}

	return TRUE;
}

void gnt_widget_set_name(GntWidget *widget, const char *name)
{
	g_free(widget->priv.name);
	widget->priv.name = g_strdup(name);
}

const char *gnt_widget_get_name(GntWidget *widget)
{
	return widget->priv.name;
}

void gnt_widget_activate(GntWidget *widget)
{
	g_signal_emit(widget, signals[SIG_ACTIVATE], 0);
}

static gboolean
update_queue_callback(gpointer data)
{
	GntWidget *widget = GNT_WIDGET(data);
	GntWidgetPrivate *priv = gnt_widget_get_instance_private(widget);

	if (priv->queue_update == 0) {
		return FALSE;
	}
	if (gnt_widget_get_mapped(widget)) {
		gnt_screen_update(widget);
	}
	priv->queue_update = 0;
	return FALSE;
}

void gnt_widget_queue_update(GntWidget *widget)
{
	GntWidgetPrivate *priv = NULL;

	g_return_if_fail(GNT_IS_WIDGET(widget));

	if (widget->window == NULL)
		return;
	while (widget->parent)
		widget = widget->parent;

	priv = gnt_widget_get_instance_private(widget);
	if (priv->queue_update == 0) {
		priv->queue_update =
		        g_timeout_add(0, update_queue_callback, widget);
	}
}

gboolean gnt_widget_confirm_size(GntWidget *widget, int width, int height)
{
	gboolean ret = FALSE;
	g_signal_emit(widget, signals[SIG_CONFIRM_SIZE], 0, width, height, &ret);
	return ret;
}

void gnt_widget_set_visible(GntWidget *widget, gboolean set)
{
	if (set)
		GNT_WIDGET_UNSET_FLAGS(widget, GNT_WIDGET_INVISIBLE);
	else
		GNT_WIDGET_SET_FLAGS(widget, GNT_WIDGET_INVISIBLE);
}

gboolean
gnt_widget_get_visible(GntWidget *widget)
{
	g_return_val_if_fail(GNT_IS_WIDGET(widget), FALSE);

	return !GNT_WIDGET_IS_FLAG_SET(widget, GNT_WIDGET_INVISIBLE);
}

gboolean gnt_widget_has_shadow(GntWidget *widget)
{
	return (!GNT_WIDGET_IS_FLAG_SET(widget, GNT_WIDGET_NO_SHADOW) &&
			gnt_style_get_bool(GNT_STYLE_SHADOW, FALSE));
}

gboolean
gnt_widget_in_destruction(GntWidget *widget)
{
	g_return_val_if_fail(GNT_IS_WIDGET(widget), FALSE);

	return GNT_WIDGET_IS_FLAG_SET(widget, GNT_WIDGET_DESTROYING);
}

void
gnt_widget_set_drawing(GntWidget *widget, gboolean drawing)
{
	g_return_if_fail(GNT_IS_WIDGET(widget));

	if (drawing) {
		GNT_WIDGET_SET_FLAGS(widget, GNT_WIDGET_DRAWING);
	} else {
		GNT_WIDGET_UNSET_FLAGS(widget, GNT_WIDGET_DRAWING);
	}
}

gboolean
gnt_widget_get_drawing(GntWidget *widget)
{
	g_return_val_if_fail(GNT_IS_WIDGET(widget), FALSE);

	return GNT_WIDGET_IS_FLAG_SET(widget, GNT_WIDGET_DRAWING);
}

void
gnt_widget_set_mapped(GntWidget *widget, gboolean mapped)
{
	g_return_if_fail(GNT_IS_WIDGET(widget));

	if (mapped) {
		GNT_WIDGET_SET_FLAGS(widget, GNT_WIDGET_MAPPED);
	} else {
		GNT_WIDGET_UNSET_FLAGS(widget, GNT_WIDGET_MAPPED);
	}
}

gboolean
gnt_widget_get_mapped(GntWidget *widget)
{
	g_return_val_if_fail(GNT_IS_WIDGET(widget), FALSE);

	return GNT_WIDGET_IS_FLAG_SET(widget, GNT_WIDGET_MAPPED);
}

void
gnt_widget_set_has_border(GntWidget *widget, gboolean has_border)
{
	g_return_if_fail(GNT_IS_WIDGET(widget));

	if (has_border) {
		GNT_WIDGET_UNSET_FLAGS(widget, GNT_WIDGET_NO_BORDER);
	} else {
		GNT_WIDGET_SET_FLAGS(widget, GNT_WIDGET_NO_BORDER);
	}
}

gboolean
gnt_widget_get_has_border(GntWidget *widget)
{
	g_return_val_if_fail(GNT_IS_WIDGET(widget), FALSE);

	return !GNT_WIDGET_IS_FLAG_SET(widget, GNT_WIDGET_NO_BORDER);
}

void
gnt_widget_set_has_shadow(GntWidget *widget, gboolean has_shadow)
{
	g_return_if_fail(GNT_IS_WIDGET(widget));

	if (has_shadow) {
		GNT_WIDGET_UNSET_FLAGS(widget, GNT_WIDGET_NO_SHADOW);
	} else {
		GNT_WIDGET_SET_FLAGS(widget, GNT_WIDGET_NO_SHADOW);
	}
}

gboolean
gnt_widget_get_has_shadow(GntWidget *widget)
{
	g_return_val_if_fail(GNT_IS_WIDGET(widget), FALSE);

	return !GNT_WIDGET_IS_FLAG_SET(widget, GNT_WIDGET_NO_SHADOW);
}

void
gnt_widget_set_has_focus(GntWidget *widget, gboolean has_focus)
{
	g_return_if_fail(GNT_IS_WIDGET(widget));

	if (has_focus) {
		GNT_WIDGET_SET_FLAGS(widget, GNT_WIDGET_HAS_FOCUS);
	} else {
		GNT_WIDGET_UNSET_FLAGS(widget, GNT_WIDGET_HAS_FOCUS);
	}
}

gboolean
gnt_widget_get_has_focus(GntWidget *widget)
{
	g_return_val_if_fail(GNT_IS_WIDGET(widget), FALSE);

	return GNT_WIDGET_IS_FLAG_SET(widget, GNT_WIDGET_HAS_FOCUS);
}

void
gnt_widget_set_is_urgent(GntWidget *widget, gboolean urgent)
{
	g_return_if_fail(GNT_IS_WIDGET(widget));

	if (urgent) {
		GNT_WIDGET_SET_FLAGS(widget, GNT_WIDGET_URGENT);
	} else {
		GNT_WIDGET_UNSET_FLAGS(widget, GNT_WIDGET_URGENT);
	}
}

gboolean
gnt_widget_get_is_urgent(GntWidget *widget)
{
	g_return_val_if_fail(GNT_IS_WIDGET(widget), FALSE);

	return GNT_WIDGET_IS_FLAG_SET(widget, GNT_WIDGET_URGENT);
}

void
gnt_widget_set_grow_x(GntWidget *widget, gboolean grow_x)
{
	g_return_if_fail(GNT_IS_WIDGET(widget));

	if (grow_x) {
		GNT_WIDGET_SET_FLAGS(widget, GNT_WIDGET_GROW_X);
	} else {
		GNT_WIDGET_UNSET_FLAGS(widget, GNT_WIDGET_GROW_X);
	}
}

gboolean
gnt_widget_get_grow_x(GntWidget *widget)
{
	g_return_val_if_fail(GNT_IS_WIDGET(widget), FALSE);

	return GNT_WIDGET_IS_FLAG_SET(widget, GNT_WIDGET_GROW_X);
}

void
gnt_widget_set_grow_y(GntWidget *widget, gboolean grow_y)
{
	g_return_if_fail(GNT_IS_WIDGET(widget));

	if (grow_y) {
		GNT_WIDGET_SET_FLAGS(widget, GNT_WIDGET_GROW_Y);
	} else {
		GNT_WIDGET_UNSET_FLAGS(widget, GNT_WIDGET_GROW_Y);
	}
}

gboolean
gnt_widget_get_grow_y(GntWidget *widget)
{
	g_return_val_if_fail(GNT_IS_WIDGET(widget), FALSE);

	return GNT_WIDGET_IS_FLAG_SET(widget, GNT_WIDGET_GROW_Y);
}

void
gnt_widget_set_transient(GntWidget *widget, gboolean transient)
{
	g_return_if_fail(GNT_IS_WIDGET(widget));

	if (transient) {
		GNT_WIDGET_SET_FLAGS(widget, GNT_WIDGET_TRANSIENT);
	} else {
		GNT_WIDGET_UNSET_FLAGS(widget, GNT_WIDGET_TRANSIENT);
	}
}

gboolean
gnt_widget_get_transient(GntWidget *widget)
{
	g_return_val_if_fail(GNT_IS_WIDGET(widget), FALSE);

	return GNT_WIDGET_IS_FLAG_SET(widget, GNT_WIDGET_TRANSIENT);
}

void
gnt_widget_set_disable_actions(GntWidget *widget, gboolean disable_actions)
{
	g_return_if_fail(GNT_IS_WIDGET(widget));

	if (disable_actions) {
		GNT_WIDGET_SET_FLAGS(widget, GNT_WIDGET_DISABLE_ACTIONS);
	} else {
		GNT_WIDGET_UNSET_FLAGS(widget, GNT_WIDGET_DISABLE_ACTIONS);
	}
}

gboolean
gnt_widget_get_disable_actions(GntWidget *widget)
{
	g_return_val_if_fail(GNT_IS_WIDGET(widget), FALSE);

	return GNT_WIDGET_IS_FLAG_SET(widget, GNT_WIDGET_DISABLE_ACTIONS);
}
