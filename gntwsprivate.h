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

#ifndef GNT_WS_PRIVATE_H
#define GNT_WS_PRIVATE_H

#include "gnt.h"
#include "gntws.h"

G_BEGIN_DECLS

/* Private access to some internals. Contact us if you need these. */
G_GNUC_INTERNAL
gboolean gnt_ws_is_single(GntWS *ws);
G_GNUC_INTERNAL
gboolean gnt_ws_is_top_widget(GntWS *ws, GntWidget *widget);
G_GNUC_INTERNAL
GList *gnt_ws_get_last(GntWS *ws);
G_GNUC_INTERNAL
void gnt_ws_append_widget(GntWS *ws, GntWidget *widget);
G_GNUC_INTERNAL
void gnt_ws_bring_to_front(GntWS *ws, GntWidget *widget);

G_GNUC_INTERNAL
void gnt_ws_set_list(GntWS *ws, GList *list);

G_END_DECLS

#endif /* GNT_WS_PRIVATE_H */
