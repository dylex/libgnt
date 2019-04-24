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

#ifndef GNT_MENUITEM_PRIVATE_H
#define GNT_MENUITEM_PRIVATE_H

#include "gnt.h"
#include "gntmenuitem.h"

G_BEGIN_DECLS

/* Private access to some internals. Contact us if you need these. */
gboolean gnt_menuitem_has_callback(GntMenuItem *item);
void gnt_menuitem_set_position(GntMenuItem *item, gint x, gint y);
void gnt_menuitem_get_position(GntMenuItem *item, gint *x, gint *y);
const gchar *gnt_menuitem_get_text(GntMenuItem *item);

G_END_DECLS

#endif /* GNT_MENUITEM_PRIVATE_H */
