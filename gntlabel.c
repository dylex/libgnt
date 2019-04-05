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
#include "gntlabel.h"
#include "gntutils.h"

#include <string.h>

enum
{
	PROP_0,
	PROP_TEXT,
	PROP_TEXT_FLAG
};

enum
{
	SIGS = 1,
};

G_DEFINE_TYPE(GntLabel, gnt_label, GNT_TYPE_WIDGET)

static void
gnt_label_destroy(GntWidget *widget)
{
	GntLabel *label = GNT_LABEL(widget);
	g_free(label->text);
}

static void
gnt_label_draw(GntWidget *widget)
{
	GntLabel *label = GNT_LABEL(widget);
	chtype flag = gnt_text_format_flag_to_chtype(label->flags);

	wbkgdset(widget->window, '\0' | flag);
	mvwaddstr(widget->window, 0, 0, C_(label->text));

	GNTDEBUG;
}

static void
gnt_label_size_request(GntWidget *widget)
{
	GntLabel *label = GNT_LABEL(widget);

	gnt_util_get_text_bound(label->text,
			&widget->priv.width, &widget->priv.height);
}

static void
gnt_label_set_property(GObject *obj, guint prop_id, const GValue *value,
                       G_GNUC_UNUSED GParamSpec *spec)
{
	GntLabel *label = GNT_LABEL(obj);
	switch (prop_id) {
		case PROP_TEXT:
			g_free(label->text);
			label->text = gnt_util_onscreen_fit_string(g_value_get_string(value), -1);
			break;
		case PROP_TEXT_FLAG:
			label->flags = g_value_get_int(value);
			break;
		default:
			g_return_if_reached();
			break;
	}
}

static void
gnt_label_get_property(GObject *obj, guint prop_id, GValue *value,
                       G_GNUC_UNUSED GParamSpec *spec)
{
	GntLabel *label = GNT_LABEL(obj);
	switch (prop_id) {
		case PROP_TEXT:
			g_value_set_string(value, label->text);
			break;
		case PROP_TEXT_FLAG:
			g_value_set_int(value, label->flags);
			break;
		default:
			break;
	}
}

static void
gnt_label_class_init(GntLabelClass *klass)
{
	GObjectClass *obj_class = G_OBJECT_CLASS(klass);
	GntWidgetClass *widget_class = GNT_WIDGET_CLASS(klass);

	widget_class->destroy = gnt_label_destroy;
	widget_class->draw = gnt_label_draw;
	widget_class->map = NULL;
	widget_class->size_request = gnt_label_size_request;

	obj_class->set_property = gnt_label_set_property;
	obj_class->get_property = gnt_label_get_property;

	g_object_class_install_property(obj_class,
			PROP_TEXT,
			g_param_spec_string("text", "Text",
				"The text for the label.",
				NULL,
				G_PARAM_READWRITE|G_PARAM_STATIC_STRINGS
			)
		);

	g_object_class_install_property(obj_class,
			PROP_TEXT_FLAG,
			g_param_spec_int("text-flag", "Text flag",
				"Text attribute to use when displaying the text in the label.",
				GNT_TEXT_FLAG_NORMAL,
				GNT_TEXT_FLAG_NORMAL|GNT_TEXT_FLAG_BOLD|GNT_TEXT_FLAG_UNDERLINE|
				GNT_TEXT_FLAG_BLINK|GNT_TEXT_FLAG_DIM|GNT_TEXT_FLAG_HIGHLIGHT,
				GNT_TEXT_FLAG_NORMAL,
				G_PARAM_READWRITE|G_PARAM_STATIC_STRINGS
			)
		);
	GNTDEBUG;
}

static void
gnt_label_init(GntLabel *label)
{
	GntWidget *widget = GNT_WIDGET(label);
	gnt_widget_set_take_focus(widget, FALSE);
	gnt_widget_set_has_border(widget, FALSE);
	gnt_widget_set_has_shadow(widget, FALSE);
	gnt_widget_set_grow_x(widget, TRUE);
	widget->priv.minw = 3;
	widget->priv.minh = 1;
	GNTDEBUG;
}

/******************************************************************************
 * GntLabel API
 *****************************************************************************/
GntWidget *gnt_label_new(const char *text)
{
	return gnt_label_new_with_format(text, 0);
}

GntWidget *gnt_label_new_with_format(const char *text, GntTextFormatFlags flags)
{
	GntWidget *widget = g_object_new(GNT_TYPE_LABEL, "text-flag", flags, "text", text, NULL);
	return widget;
}

void gnt_label_set_text(GntLabel *label, const char *text)
{
	g_object_set(label, "text", text, NULL);

	if (GNT_WIDGET(label)->window)
	{
		werase(GNT_WIDGET(label)->window);
		gnt_widget_draw(GNT_WIDGET(label));
	}
}

