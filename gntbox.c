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

#include "gntinternal.h"
#include "gntbox.h"
#include "gntstyle.h"
#include "gntutils.h"

#include <string.h>

typedef struct
{
	gboolean vertical;
	gboolean homogeneous;
	gboolean fill;
	GList *list; /* List of widgets */

	GntWidget *active;
	int pad;                /* Number of spaces to use between widgets */
	GntAlignment alignment; /* How are the widgets going to be aligned? */

	char *title;
	GList *focus; /* List of widgets to cycle focus (only valid for parent
	                 boxes) */
} GntBoxPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(GntBox, gnt_box, GNT_TYPE_WIDGET)

#define PROP_LAST_RESIZE_S "last-resize"
#define PROP_SIZE_QUEUED_S "size-queued"

enum
{
	PROP_0,
	PROP_VERTICAL,
	PROP_HOMOGENEOUS
};

enum
{
	SIGS = 1,
};

static GntWidget * find_focusable_widget(GntBox *box);

static void
add_to_focus(GntWidget *w, GntBox *box)
{
	if (GNT_IS_BOX(w)) {
		GntBoxPrivate *priv = gnt_box_get_instance_private(GNT_BOX(w));
		g_list_foreach(priv->list, (GFunc)add_to_focus, box);
	} else if (gnt_widget_get_take_focus(w)) {
		GntBoxPrivate *priv = gnt_box_get_instance_private(box);
		priv->focus = g_list_append(priv->focus, w);
	}
}

static void
get_title_thingies(GntBox *box, char *title, int *p, int *r)
{
	GntWidget *widget = GNT_WIDGET(box);
	int len;
	char *end = (char*)gnt_util_onscreen_width_to_pointer(title, widget->priv.width - 4, &len);

	if (p)
		*p = (widget->priv.width - len) / 2;
	if (r)
		*r = (widget->priv.width + len) / 2;
	*end = '\0';
}

static void
gnt_box_draw(GntWidget *widget)
{
	GntBox *box = GNT_BOX(widget);
	GntBoxPrivate *priv = gnt_box_get_instance_private(box);

	if (priv->focus == NULL && gnt_widget_get_parent(widget) == NULL) {
		g_list_foreach(priv->list, (GFunc)add_to_focus, box);
	}

	g_list_foreach(priv->list, (GFunc)gnt_widget_draw, NULL);

	if (priv->title && gnt_widget_get_has_border(widget)) {
		int pos, right;
		char *title = g_strdup(priv->title);

		get_title_thingies(box, title, &pos, &right);

		if (gnt_widget_has_focus(widget))
			wbkgdset(widget->window, '\0' | gnt_color_pair(GNT_COLOR_TITLE));
		else
			wbkgdset(widget->window, '\0' | gnt_color_pair(GNT_COLOR_TITLE_D));
		mvwaddch(widget->window, 0, pos-1, ACS_RTEE | gnt_color_pair(GNT_COLOR_NORMAL));
		mvwaddstr(widget->window, 0, pos, C_(title));
		mvwaddch(widget->window, 0, right, ACS_LTEE | gnt_color_pair(GNT_COLOR_NORMAL));
		g_free(title);
	}

	gnt_box_sync_children(box);
}

static void
reposition_children(GntWidget *widget)
{
	GList *iter;
	GntBox *box = GNT_BOX(widget);
	GntBoxPrivate *priv = gnt_box_get_instance_private(box);
	int w, h, curx, cury, max;
	gboolean has_border = FALSE;

	w = h = 0;
	max = 0;
	curx = widget->priv.x;
	cury = widget->priv.y;
	if (gnt_widget_get_has_border(widget)) {
		has_border = TRUE;
		curx += 1;
		cury += 1;
	}

	for (iter = priv->list; iter; iter = iter->next) {
		if (!gnt_widget_get_visible(GNT_WIDGET(iter->data)))
			continue;
		gnt_widget_set_position(GNT_WIDGET(iter->data), curx, cury);
		gnt_widget_get_size(GNT_WIDGET(iter->data), &w, &h);
		if (priv->vertical) {
			if (h)
			{
				cury += h + priv->pad;
				if (max < w)
					max = w;
			}
		} else {
			if (w)
			{
				curx += w + priv->pad;
				if (max < h)
					max = h;
			}
		}
	}

	if (has_border)
	{
		curx += 1;
		cury += 1;
		max += 2;
	}

	if (priv->list) {
		if (priv->vertical) {
			cury -= priv->pad;
		} else {
			curx -= priv->pad;
		}
	}

	if (priv->vertical) {
		widget->priv.width = max;
		widget->priv.height = cury - widget->priv.y;
	} else {
		widget->priv.width = curx - widget->priv.x;
		widget->priv.height = max;
	}
}

static void
gnt_box_set_position(GntWidget *widget, int x, int y)
{
	GntBox *box = GNT_BOX(widget);
	GntBoxPrivate *priv = gnt_box_get_instance_private(box);
	GList *iter;
	int changex, changey;

	changex = widget->priv.x - x;
	changey = widget->priv.y - y;

	for (iter = priv->list; iter; iter = iter->next) {
		GntWidget *w = GNT_WIDGET(iter->data);
		gnt_widget_set_position(w, w->priv.x - changex,
				w->priv.y - changey);
	}
}

static void
gnt_box_size_request(GntWidget *widget)
{
	GntBox *box = GNT_BOX(widget);
	GntBoxPrivate *priv = gnt_box_get_instance_private(box);
	GList *iter;
	int maxw = 0, maxh = 0;

	g_list_foreach(priv->list, (GFunc)gnt_widget_size_request, NULL);

	for (iter = priv->list; iter; iter = iter->next) {
		int w, h;
		gnt_widget_get_size(GNT_WIDGET(iter->data), &w, &h);
		if (maxh < h)
			maxh = h;
		if (maxw < w)
			maxw = w;
	}

	for (iter = priv->list; iter; iter = iter->next) {
		int w, h;
		GntWidget *wid = GNT_WIDGET(iter->data);

		gnt_widget_get_size(wid, &w, &h);

		if (priv->homogeneous) {
			if (priv->vertical) {
				h = maxh;
			} else {
				w = maxw;
			}
		}
		if (priv->fill) {
			if (priv->vertical) {
				w = maxw;
			} else {
				h = maxh;
			}
		}

		if (gnt_widget_confirm_size(wid, w, h))
			gnt_widget_set_size(wid, w, h);
	}

	reposition_children(widget);
}

static void
gnt_box_map(GntWidget *widget)
{
	if (widget->priv.width == 0 || widget->priv.height == 0)
	{
		gnt_widget_size_request(widget);
		find_focusable_widget(GNT_BOX(widget));
	}
	GNTDEBUG;
}

/* Ensures that the current widget can take focus */
static GntWidget *
find_focusable_widget(GntBox *box)
{
	GntBoxPrivate *priv = gnt_box_get_instance_private(box);

	/* XXX: Make sure the widget is visible? */
	if (priv->focus == NULL &&
	    gnt_widget_get_parent(GNT_WIDGET(box)) == NULL) {
		g_list_foreach(priv->list, (GFunc)add_to_focus, box);
	}

	if (priv->active == NULL && priv->focus) {
		priv->active = priv->focus->data;
	}

	return priv->active;
}

static void
find_next_focus(GntBox *box)
{
	GntBoxPrivate *priv = gnt_box_get_instance_private(box);
	gpointer last = priv->active;
	do
	{
		GList *iter = g_list_find(priv->focus, priv->active);
		if (iter && iter->next) {
			priv->active = iter->next->data;
		} else if (priv->focus) {
			priv->active = priv->focus->data;
		}
		if (gnt_widget_get_visible(priv->active) &&
		    gnt_widget_get_take_focus(priv->active)) {
			break;
		}
	} while (priv->active != last);
}

static void
find_prev_focus(GntBox *box)
{
	GntBoxPrivate *priv = gnt_box_get_instance_private(box);
	gpointer last = priv->active;

	if (!priv->focus) {
		return;
	}

	do
	{
		GList *iter = g_list_find(priv->focus, priv->active);
		if (!iter)
			priv->active = priv->focus->data;
		else if (!iter->prev)
			priv->active = g_list_last(priv->focus)->data;
		else
			priv->active = iter->prev->data;
		if (gnt_widget_get_visible(priv->active)) {
			break;
		}
	} while (priv->active != last);
}

static gboolean
gnt_box_key_pressed(GntWidget *widget, const char *text)
{
	GntBox *box = GNT_BOX(widget);
	GntBoxPrivate *priv = gnt_box_get_instance_private(box);
	gboolean ret;

	if (!gnt_widget_get_disable_actions(widget))
		return FALSE;

	if (priv->active == NULL && !find_focusable_widget(box)) {
		return FALSE;
	}

	if (gnt_widget_key_pressed(priv->active, text)) {
		return TRUE;
	}

	/* This dance is necessary to make sure that the child widgets get a chance
	   to trigger their bindings first */
	gnt_widget_set_disable_actions(widget, FALSE);
	ret = gnt_widget_key_pressed(widget, text);
	gnt_widget_set_disable_actions(widget, TRUE);
	return ret;
}

static gboolean
box_focus_change(GntBox *box, gboolean next)
{
	GntBoxPrivate *priv = gnt_box_get_instance_private(box);
	GntWidget *now;

	now = priv->active;

	if (next) {
		find_next_focus(box);
	} else {
		find_prev_focus(box);
	}

	if (now && now != priv->active) {
		gnt_widget_set_focus(now, FALSE);
		gnt_widget_set_focus(priv->active, TRUE);
		return TRUE;
	}

	return FALSE;
}

static gboolean
action_focus_next(GntBindable *bindable, G_GNUC_UNUSED GList *params)
{
	return box_focus_change(GNT_BOX(bindable), TRUE);
}

static gboolean
action_focus_prev(GntBindable *bindable, G_GNUC_UNUSED GList *params)
{
	return box_focus_change(GNT_BOX(bindable), FALSE);
}

static void
gnt_box_lost_focus(GntWidget *widget)
{
	GntBoxPrivate *priv = gnt_box_get_instance_private(GNT_BOX(widget));
	GntWidget *w = priv->active;
	if (w)
		gnt_widget_set_focus(w, FALSE);
	gnt_widget_draw(widget);
}

static void
gnt_box_gained_focus(GntWidget *widget)
{
	GntBoxPrivate *priv = gnt_box_get_instance_private(GNT_BOX(widget));
	GntWidget *w = priv->active;
	if (w)
		gnt_widget_set_focus(w, TRUE);
	gnt_widget_draw(widget);
}

static void
gnt_box_destroy(GntWidget *w)
{
	GntBox *box = GNT_BOX(w);

	gnt_box_remove_all(box);
	gnt_screen_release(w);
}

static void
gnt_box_expose(GntWidget *widget, int x, int y, int width, int height)
{
	WINDOW *win = newwin(height, width, widget->priv.y + y, widget->priv.x + x);
	copywin(widget->window, win, y, x, 0, 0, height - 1, width - 1, FALSE);
	wrefresh(win);
	delwin(win);
}

static gboolean
gnt_box_confirm_size(GntWidget *widget, int width, int height)
{
	GList *iter;
	GntBox *box = GNT_BOX(widget);
	GntBoxPrivate *priv = gnt_box_get_instance_private(box);
	int wchange, hchange;
	GntWidget *child, *last;

	if (!priv->list) {
		return TRUE;
	}

	wchange = widget->priv.width - width;
	hchange = widget->priv.height - height;

	if (wchange == 0 && hchange == 0)
		return TRUE;		/* Quit playing games with my size */

	child = NULL;
	last = g_object_get_data(G_OBJECT(box), PROP_LAST_RESIZE_S);

	/* First, make sure all the widgets will fit into the box after resizing. */
	for (iter = priv->list; iter; iter = iter->next) {
		GntWidget *wid = iter->data;
		int w, h;

		gnt_widget_get_size(wid, &w, &h);

		if (wid != last && !child && w > 0 && h > 0 &&
		    gnt_widget_get_visible(wid) &&
		    gnt_widget_confirm_size(wid, w - wchange, h - hchange)) {
			child = wid;
			break;
		}
	}

	if (!child && (child = last)) {
		int w, h;
		gnt_widget_get_size(child, &w, &h);
		if (!gnt_widget_confirm_size(child, w - wchange, h - hchange))
			child = NULL;
	}

	g_object_set_data(G_OBJECT(box), PROP_SIZE_QUEUED_S, child);

	if (child) {
		for (iter = priv->list; iter; iter = iter->next) {
			GntWidget *wid = iter->data;
			int w, h;

			if (wid == child)
				continue;

			gnt_widget_get_size(wid, &w, &h);
			if (priv->vertical) {
				/* For a vertical box, if we are changing the width, make sure the widgets
				 * in the box will fit after resizing the width. */
				if (wchange > 0 &&
						w >= child->priv.width &&
						!gnt_widget_confirm_size(wid, w - wchange, h))
					return FALSE;
			} else {
				/* If we are changing the height, make sure the widgets in the box fit after
				 * the resize. */
				if (hchange > 0 &&
						h >= child->priv.height &&
						!gnt_widget_confirm_size(wid, w, h - hchange))
					return FALSE;
			}
		}
	}

	return (child != NULL);
}

static void
gnt_box_size_changed(GntWidget *widget, int oldw, int oldh)
{
	int wchange, hchange;
	GList *i;
	GntBox *box = GNT_BOX(widget);
	GntBoxPrivate *priv = gnt_box_get_instance_private(box);
	GntWidget *wid;
	int tw, th;

	wchange = widget->priv.width - oldw;
	hchange = widget->priv.height - oldh;

	wid = g_object_get_data(G_OBJECT(box), PROP_SIZE_QUEUED_S);
	if (wid) {
		gnt_widget_get_size(wid, &tw, &th);
		gnt_widget_set_size(wid, tw + wchange, th + hchange);
		g_object_set_data(G_OBJECT(box), PROP_SIZE_QUEUED_S, NULL);
		g_object_set_data(G_OBJECT(box), PROP_LAST_RESIZE_S, wid);
	}

	if (priv->vertical) {
		hchange = 0;
	} else {
		wchange = 0;
	}

	for (i = priv->list; i; i = i->next) {
		if (wid != i->data)
		{
			gnt_widget_get_size(GNT_WIDGET(i->data), &tw, &th);
			gnt_widget_set_size(i->data, tw + wchange, th + hchange);
		}
	}

	reposition_children(widget);
}

static gboolean
gnt_box_clicked(GntWidget *widget, GntMouseEvent event, int cx, int cy)
{
	GntBox *box = GNT_BOX(widget);
	GntBoxPrivate *priv = gnt_box_get_instance_private(box);
	GList *iter;
	for (iter = priv->list; iter; iter = iter->next) {
		int x, y, w, h;
		GntWidget *wid = iter->data;

		gnt_widget_get_position(wid, &x, &y);
		gnt_widget_get_size(wid, &w, &h);

		if (cx >= x && cx < x + w && cy >= y && cy < y + h) {
			if (event <= GNT_MIDDLE_MOUSE_DOWN &&
			    gnt_widget_get_take_focus(wid)) {
				widget = gnt_widget_get_toplevel(widget);
				gnt_box_give_focus_to_child(GNT_BOX(widget), wid);
			}
			return gnt_widget_clicked(wid, event, cx, cy);
		}
	}
	return FALSE;
}

static void
gnt_box_set_property(GObject *obj, guint prop_id, const GValue *value,
                     G_GNUC_UNUSED GParamSpec *spec)
{
	GntBox *box = GNT_BOX(obj);
	GntBoxPrivate *priv = gnt_box_get_instance_private(box);

	switch (prop_id) {
		case PROP_VERTICAL:
			priv->vertical = g_value_get_boolean(value);
			break;
		case PROP_HOMOGENEOUS:
			priv->homogeneous = g_value_get_boolean(value);
			break;
		default:
			g_return_if_reached();
			break;
	}
}

static void
gnt_box_get_property(GObject *obj, guint prop_id, GValue *value,
                     G_GNUC_UNUSED GParamSpec *spec)
{
	GntBox *box = GNT_BOX(obj);
	GntBoxPrivate *priv = gnt_box_get_instance_private(box);

	switch (prop_id) {
		case PROP_VERTICAL:
			g_value_set_boolean(value, priv->vertical);
			break;
		case PROP_HOMOGENEOUS:
			g_value_set_boolean(value, priv->homogeneous);
			break;
		default:
			break;
	}
}

static void
gnt_box_class_init(GntBoxClass *klass)
{
	GntBindableClass *bindable = GNT_BINDABLE_CLASS(klass);
	GObjectClass *obj_class = G_OBJECT_CLASS(klass);
	GntWidgetClass *widget_class = GNT_WIDGET_CLASS(klass);

	widget_class->destroy = gnt_box_destroy;
	widget_class->draw = gnt_box_draw;
	widget_class->expose = gnt_box_expose;
	widget_class->map = gnt_box_map;
	widget_class->size_request = gnt_box_size_request;
	widget_class->set_position = gnt_box_set_position;
	widget_class->key_pressed = gnt_box_key_pressed;
	widget_class->clicked = gnt_box_clicked;
	widget_class->lost_focus = gnt_box_lost_focus;
	widget_class->gained_focus = gnt_box_gained_focus;
	widget_class->confirm_size = gnt_box_confirm_size;
	widget_class->size_changed = gnt_box_size_changed;

	obj_class->set_property = gnt_box_set_property;
	obj_class->get_property = gnt_box_get_property;
	g_object_class_install_property(obj_class,
			PROP_VERTICAL,
			g_param_spec_boolean("vertical", "Vertical",
				"Whether the child widgets in the box should be stacked vertically.",
				TRUE,
				G_PARAM_READWRITE|G_PARAM_CONSTRUCT|G_PARAM_STATIC_STRINGS
			)
		);
	g_object_class_install_property(obj_class,
			PROP_HOMOGENEOUS,
			g_param_spec_boolean("homogeneous", "Homogeneous",
				"Whether the child widgets in the box should have the same size.",
				TRUE,
				G_PARAM_READWRITE|G_PARAM_CONSTRUCT|G_PARAM_STATIC_STRINGS
			)
		);

	gnt_bindable_class_register_action(bindable, "focus-next", action_focus_next,
			"\t", NULL);
	gnt_bindable_register_binding(bindable, "focus-next", GNT_KEY_RIGHT, NULL);
	gnt_bindable_class_register_action(bindable, "focus-prev", action_focus_prev,
			GNT_KEY_BACK_TAB, NULL);
	gnt_bindable_register_binding(bindable, "focus-prev", GNT_KEY_LEFT, NULL);

	gnt_style_read_actions(G_OBJECT_CLASS_TYPE(klass), bindable);
}

static void
gnt_box_init(GntBox *box)
{
	GntBoxPrivate *priv = gnt_box_get_instance_private(box);
	GntWidget *widget = GNT_WIDGET(box);

	/* Initially make both the height and width resizable.
	 * Update the flags as necessary when widgets are added to it. */
	gnt_widget_set_grow_x(widget, TRUE);
	gnt_widget_set_grow_y(widget, TRUE);
	gnt_widget_set_take_focus(widget, TRUE);
	gnt_widget_set_disable_actions(widget, TRUE);
	gnt_widget_set_has_border(widget, FALSE);
	gnt_widget_set_has_shadow(widget, FALSE);

	priv->pad = 1;
	priv->fill = TRUE;
}

/******************************************************************************
 * GntBox API
 *****************************************************************************/
GntWidget *
gnt_box_new(gboolean homogeneous, gboolean vert)
{
	GntWidget *widget = g_object_new(GNT_TYPE_BOX, "homogeneous",
	                                 homogeneous, "vertical", vert, NULL);
	GntBox *box = GNT_BOX(widget);
	GntBoxPrivate *priv = gnt_box_get_instance_private(box);

	priv->alignment = vert ? GNT_ALIGN_LEFT : GNT_ALIGN_MID;

	return widget;
}

void
gnt_box_add_widget(GntBox *box, GntWidget *widget)
{
	GntBoxPrivate *priv = NULL;

	g_return_if_fail(GNT_IS_BOX(box));
	priv = gnt_box_get_instance_private(box);

	priv->list = g_list_append(priv->list, widget);
	widget->parent = GNT_WIDGET(box);
}

void
gnt_box_add_widget_in_front(GntBox *box, GntWidget *widget)
{
	GntBoxPrivate *priv = NULL;

	g_return_if_fail(GNT_IS_BOX(box));
	priv = gnt_box_get_instance_private(box);

	priv->list = g_list_prepend(priv->list, widget);
	widget->parent = GNT_WIDGET(box);
}

void
gnt_box_set_title(GntBox *box, const char *title)
{
	GntBoxPrivate *priv = NULL;
	char *prev = NULL;
	GntWidget *w = NULL;

	g_return_if_fail(GNT_IS_BOX(box));
	priv = gnt_box_get_instance_private(box);

	prev = priv->title;
	priv->title = g_strdup(title);
	w = GNT_WIDGET(box);
	if (w->window && gnt_widget_get_has_border(w)) {
		/* Erase the old title */
		int pos, right;
		get_title_thingies(box, prev, &pos, &right);
		mvwhline(w->window, 0, pos - 1, ACS_HLINE | gnt_color_pair(GNT_COLOR_NORMAL),
				right - pos + 2);
	}
	g_free(prev);
}

/* Internal. */
const gchar *
gnt_box_get_title(GntBox *box)
{
	GntBoxPrivate *priv = NULL;
	g_return_val_if_fail(GNT_IS_BOX(box), NULL);
	priv = gnt_box_get_instance_private(box);
	return priv->title;
}

void gnt_box_set_pad(GntBox *box, int pad)
{
	GntBoxPrivate *priv = NULL;

	g_return_if_fail(GNT_IS_BOX(box));
	priv = gnt_box_get_instance_private(box);

	priv->pad = pad;
	/* XXX: Perhaps redraw if already showing? */
}

void gnt_box_set_toplevel(GntBox *box, gboolean set)
{
	GntWidget *widget = GNT_WIDGET(box);

	gnt_widget_set_has_border(widget, set);
	gnt_widget_set_has_shadow(widget, set);
	gnt_widget_set_take_focus(widget, set);
}

GList *
gnt_box_get_children(GntBox *box)
{
	GntBoxPrivate *priv = NULL;

	g_return_val_if_fail(GNT_IS_BOX(box), NULL);
	priv = gnt_box_get_instance_private(box);

	return g_list_copy(priv->list);
}

void gnt_box_sync_children(GntBox *box)
{
	GntBoxPrivate *priv = NULL;
	GntWidget *widget = NULL;
	GList *iter;
	int pos;

	g_return_if_fail(GNT_IS_BOX(box));
	priv = gnt_box_get_instance_private(box);

	widget = GNT_WIDGET(box);
	pos = gnt_widget_get_has_border(widget) ? 1 : 0;

	if (!priv->active) {
		find_focusable_widget(box);
	}

	for (iter = priv->list; iter; iter = iter->next) {
		GntWidget *w = GNT_WIDGET(iter->data);
		int height, width;
		int x, y;

		if (G_UNLIKELY(w == NULL)) {
			g_warn_if_reached();
			continue;
		}

		if (!gnt_widget_get_visible(w))
			continue;

		if (GNT_IS_BOX(w))
			gnt_box_sync_children(GNT_BOX(w));

		gnt_widget_get_size(w, &width, &height);

		x = w->priv.x - widget->priv.x;
		y = w->priv.y - widget->priv.y;

		if (priv->vertical) {
			x = pos;
			if (priv->alignment == GNT_ALIGN_RIGHT) {
				x += widget->priv.width - width;
			} else if (priv->alignment == GNT_ALIGN_MID) {
				x += (widget->priv.width - width)/2;
			}
			if (x + width > widget->priv.width - pos)
				x -= x + width - (widget->priv.width - pos);
		} else {
			y = pos;
			if (priv->alignment == GNT_ALIGN_BOTTOM) {
				y += widget->priv.height - height;
			} else if (priv->alignment == GNT_ALIGN_MID) {
				y += (widget->priv.height - height)/2;
			}
			if (y + height >= widget->priv.height - pos)
				y = widget->priv.height - height - pos;
		}

		copywin(w->window, widget->window, 0, 0,
				y, x, y + height - 1, x + width - 1, FALSE);
		gnt_widget_set_position(w, x + widget->priv.x, y + widget->priv.y);
		if (w == priv->active) {
			wmove(widget->window, y + getcury(w->window), x + getcurx(w->window));
		}
	}
}

void gnt_box_set_alignment(GntBox *box, GntAlignment alignment)
{
	GntBoxPrivate *priv = NULL;

	g_return_if_fail(GNT_IS_BOX(box));
	priv = gnt_box_get_instance_private(box);

	priv->alignment = alignment;
}

void gnt_box_remove(GntBox *box, GntWidget *widget)
{
	GntBoxPrivate *priv = NULL;

	g_return_if_fail(GNT_IS_BOX(box));
	priv = gnt_box_get_instance_private(box);

	priv->list = g_list_remove(priv->list, widget);
	if (gnt_widget_get_take_focus(widget) &&
	    gnt_widget_get_parent(GNT_WIDGET(box)) == NULL && priv->focus) {
		if (widget == priv->active) {
			find_next_focus(box);
			if (priv->active == widget) {
				/* There's only one widget */
				priv->active = NULL;
			}
		}
		priv->focus = g_list_remove(priv->focus, widget);
	}

	if (gnt_widget_get_mapped(GNT_WIDGET(box)))
		gnt_widget_draw(GNT_WIDGET(box));
}

void gnt_box_remove_all(GntBox *box)
{
	GntBoxPrivate *priv = NULL;

	g_return_if_fail(GNT_IS_BOX(box));
	priv = gnt_box_get_instance_private(box);

	g_list_free_full(priv->list, (GDestroyNotify)gnt_widget_destroy);
	g_list_free(priv->focus);
	priv->list = NULL;
	priv->focus = NULL;
	GNT_WIDGET(box)->priv.width = 0;
	GNT_WIDGET(box)->priv.height = 0;
}

void gnt_box_readjust(GntBox *box)
{
	GntBoxPrivate *priv = NULL;
	GList *iter;
	GntWidget *wid;
	int width, height;

	g_return_if_fail(GNT_IS_BOX(box));
	priv = gnt_box_get_instance_private(box);

	if (gnt_widget_get_parent(GNT_WIDGET(box)) != NULL) {
		return;
	}

	for (iter = priv->list; iter; iter = iter->next) {
		GntWidget *w = iter->data;

		if (G_UNLIKELY(w == NULL)) {
			g_warn_if_reached();
			continue;
		}

		if (GNT_IS_BOX(w))
			gnt_box_readjust(GNT_BOX(w));
		else
		{
			gnt_widget_set_mapped(w, FALSE);
			w->priv.width = 0;
			w->priv.height = 0;
		}
	}

	wid = GNT_WIDGET(box);
	gnt_widget_set_mapped(wid, FALSE);
	wid->priv.width = 0;
	wid->priv.height = 0;

	if (gnt_widget_get_parent(wid) == NULL) {
		g_list_free(priv->focus);
		priv->focus = NULL;
		priv->active = NULL;
		gnt_widget_size_request(wid);
		gnt_widget_get_size(wid, &width, &height);
		gnt_screen_resize_widget(wid, width, height);
		find_focusable_widget(box);
	}
}

void gnt_box_set_fill(GntBox *box, gboolean fill)
{
	GntBoxPrivate *priv = NULL;

	g_return_if_fail(GNT_IS_BOX(box));
	priv = gnt_box_get_instance_private(box);

	priv->fill = fill;
}

/* Internal. */
GntWidget *
gnt_box_get_active(GntBox *box)
{
	GntBoxPrivate *priv = NULL;
	g_return_val_if_fail(GNT_IS_BOX(box), NULL);
	priv = gnt_box_get_instance_private(box);
	return priv->active;
}

void gnt_box_move_focus(GntBox *box, int dir)
{
	GntBoxPrivate *priv = NULL;
	GntWidget *now;

	g_return_if_fail(GNT_IS_BOX(box));
	priv = gnt_box_get_instance_private(box);

	if (priv->active == NULL) {
		find_focusable_widget(box);
		return;
	}

	now = priv->active;

	if (dir == 1)
		find_next_focus(box);
	else if (dir == -1)
		find_prev_focus(box);

	if (now && now != priv->active) {
		gnt_widget_set_focus(now, FALSE);
		gnt_widget_set_focus(priv->active, TRUE);
	}

	if (GNT_WIDGET(box)->window)
		gnt_widget_draw(GNT_WIDGET(box));
}

void gnt_box_give_focus_to_child(GntBox *box, GntWidget *widget)
{
	GntBoxPrivate *priv = NULL;
	GList *find;
	gpointer now;

	g_return_if_fail(GNT_IS_BOX(box));

	box = GNT_BOX(gnt_widget_get_toplevel(GNT_WIDGET(box)));
	priv = gnt_box_get_instance_private(box);

	find = g_list_find(priv->focus, widget);
	now = priv->active;
	if (find) {
		priv->active = widget;
	}
	if (now && now != priv->active) {
		gnt_widget_set_focus(now, FALSE);
		gnt_widget_set_focus(priv->active, TRUE);
	}

	if (GNT_WIDGET(box)->window)
		gnt_widget_draw(GNT_WIDGET(box));
}

