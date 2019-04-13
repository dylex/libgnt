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

#ifndef GNT_COMBO_BOX_H
#define GNT_COMBO_BOX_H
/**
 * SECTION:gntcombobox
 * @section_id: libgnt-gntcombobox
 * @title: GntComboBox
 * @short_description: A widget for selecting between multiple choices
 */

#include "gnt.h"
#include "gntcolors.h"
#include "gntkeys.h"
#include "gntwidget.h"

#define GNT_TYPE_COMBO_BOX gnt_combo_box_get_type()

G_BEGIN_DECLS

/**
 * gnt_combo_box_get_type:
 *
 * Returns:  Get the GType for GntComboBox
 */
G_DECLARE_FINAL_TYPE(GntComboBox, gnt_combo_box, GNT, COMBO_BOX, GntWidget)

/**
 * gnt_combo_box_new:
 *
 * Create a new GntComboBox
 *
 * Returns: A new GntComboBox
 */
GntWidget * gnt_combo_box_new(void);

/**
 * gnt_combo_box_add_data:
 * @box: The GntComboBox
 * @key: The data
 * @text: The text to display
 *
 * Add an entry
 */
void gnt_combo_box_add_data(GntComboBox *box, gpointer key, const char *text);

/**
 * gnt_combo_box_remove:
 * @box: The GntComboBox
 * @key: The data to be removed
 *
 * Remove an entry
 */
void gnt_combo_box_remove(GntComboBox *box, gpointer key);

/**
 * gnt_combo_box_remove_all:
 * @box: The GntComboBox
 *
 * Remove all entries
 */
void gnt_combo_box_remove_all(GntComboBox *box);

/**
 * gnt_combo_box_get_selected_data:
 * @box: The GntComboBox
 *
 * Get the data that is currently selected
 *
 * Returns: (transfer none): The data of the currently selected entry
 */
gpointer gnt_combo_box_get_selected_data(GntComboBox *box);

/**
 * gnt_combo_box_set_selected:
 * @box: The GntComboBox
 * @key: The data to be set to
 *
 * Set the current selection to a specific entry
 */
void gnt_combo_box_set_selected(GntComboBox *box, gpointer key);

G_END_DECLS

#endif /* GNT_COMBO_BOX_H */
