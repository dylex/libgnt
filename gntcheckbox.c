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
#include "gntcheckbox.h"

enum
{
	SIG_TOGGLED = 1,
	SIGS,
};

static guint signals[SIGS] = { 0 };

G_DEFINE_TYPE(GntCheckBox, gnt_check_box, GNT_TYPE_BUTTON)

static void
gnt_check_box_draw(GntWidget *widget)
{
	GntCheckBox *cb = GNT_CHECK_BOX(widget);
	GntColorType type;
	gboolean focus = gnt_widget_has_focus(widget);

	if (focus)
		type = GNT_COLOR_HIGHLIGHT;
	else
		type = GNT_COLOR_NORMAL;

	wbkgdset(widget->window, '\0' | gnt_color_pair(type));

	mvwaddch(widget->window, 0, 0, '[');
	mvwaddch(widget->window, 0, 1, (cb->checked ? 'X' : ' ') | (focus ? A_UNDERLINE : A_NORMAL));
	mvwaddch(widget->window, 0, 2, ']');

	wbkgdset(widget->window, '\0' | gnt_color_pair(GNT_COLOR_NORMAL));
	mvwaddstr(widget->window, 0, 4, C_(GNT_BUTTON(cb)->priv->text));
	(void)wmove(widget->window, 0, 1);

	GNTDEBUG;
}

static void
toggle_selection(GntWidget *widget)
{
	GNT_CHECK_BOX(widget)->checked = !GNT_CHECK_BOX(widget)->checked;
	g_signal_emit(widget, signals[SIG_TOGGLED], 0);
	gnt_widget_draw(widget);
}

static gboolean
gnt_check_box_key_pressed(GntWidget *widget, const char *text)
{
	if (text[0] == ' ' && text[1] == '\0')
	{
		toggle_selection(widget);
		return TRUE;
	}

	return FALSE;
}

static gboolean
gnt_check_box_clicked(GntWidget *widget, GntMouseEvent event, int x, int y)
{
	if (event == GNT_LEFT_MOUSE_DOWN) {
		toggle_selection(widget);
		return TRUE;
	}
	return FALSE;
}

static void
gnt_check_box_class_init(GntCheckBoxClass *klass)
{
	GntWidgetClass *widget_class = GNT_WIDGET_CLASS(klass);

	widget_class->draw = gnt_check_box_draw;
	widget_class->key_pressed = gnt_check_box_key_pressed;
	widget_class->clicked = gnt_check_box_clicked;

	signals[SIG_TOGGLED] =
		g_signal_new("toggled",
					 G_TYPE_FROM_CLASS(klass),
					 G_SIGNAL_RUN_LAST,
					 G_STRUCT_OFFSET(GntCheckBoxClass, toggled),
					 NULL, NULL, NULL,
					 G_TYPE_NONE, 0);
	GNTDEBUG;
}

static void
gnt_check_box_init(GntCheckBox *box)
{
	GntWidget *widget = GNT_WIDGET(box);
	widget->priv.minh = 1;
	widget->priv.minw = 4;
	gnt_widget_set_has_border(widget, FALSE);
	gnt_widget_set_has_shadow(widget, FALSE);
}

/******************************************************************************
 * GntCheckBox API
 *****************************************************************************/
GntWidget *gnt_check_box_new(const char *text)
{
	GntWidget *widget = g_object_new(GNT_TYPE_CHECK_BOX, NULL);

	GNT_BUTTON(widget)->priv->text = g_strdup(text);
	gnt_widget_set_take_focus(widget, TRUE);

	return widget;
}

void gnt_check_box_set_checked(GntCheckBox *box, gboolean set)
{
	if (set != box->checked)
	{
		box->checked = set;
		g_signal_emit(box, signals[SIG_TOGGLED], 0);
	}
}

gboolean gnt_check_box_get_checked(GntCheckBox *box)
{
	return box->checked;
}



