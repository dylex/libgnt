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

#ifndef GNT_CLIPBOARD_H
#define GNT_CLIPBOARD_H
/**
 * SECTION:gntclipboard
 * @section_id: libgnt-gntclipboard
 * @short_description: <filename>gntclipboard.h</filename>
 * @title: Clipboard
 */

#include <glib.h>
#include <glib-object.h>

#define GNT_TYPE_CLIPBOARD  gnt_clipboard_get_type()

G_BEGIN_DECLS

/**
 * gnt_clipboard_get_type:
 *
 * Returns: GType for GntClipboard.
 */
G_DECLARE_FINAL_TYPE(GntClipboard, gnt_clipboard, GNT, CLIPBOARD, GObject)

/**
 * gnt_clipboard_get_string:
 * @clip:  The clipboard.
 *
 * Get the current text from the clipboard.
 *
 * Returns:  A copy of the string in the clipboard. The caller should free the
 *          returned value.
 */
gchar * gnt_clipboard_get_string(GntClipboard *clip);

/**
 * gnt_clipboard_set_string:
 * @clip:     The clipboard.
 * @string:   New string for the clipboard.
 *
 * Set the text in the clipboard.
 */
void gnt_clipboard_set_string(GntClipboard *clip, const gchar *string);

G_END_DECLS

#endif
