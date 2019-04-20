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

#ifndef GNT_LINE_H
#define GNT_LINE_H
/**
 * SECTION:gntline
 * @section_id: libgnt-gntline
 * @title: GntLine
 * @short_description: A widget that draws a horizontal or vertical line
 */

#include "gnt.h"
#include "gntcolors.h"
#include "gntkeys.h"
#include "gntwidget.h"

#define GNT_TYPE_LINE gnt_line_get_type()

G_BEGIN_DECLS

/**
 * gnt_line_get_type:
 *
 * Returns: GType for GntLine.
 */
G_DECLARE_FINAL_TYPE(GntLine, gnt_line, GNT, LINE, GntWidget)

#define gnt_hline_new() gnt_line_new(FALSE)
#define gnt_vline_new() gnt_line_new(TRUE)

/**
 * gnt_line_new:
 * @vertical:  %TRUE if the line should be vertical, %FALSE for a horizontal line.
 *
 * Create new line
 *
 * Returns:  The newly created line.
 */
GntWidget * gnt_line_new(gboolean vertical);

G_END_DECLS

#endif /* GNT_LINE_H */
