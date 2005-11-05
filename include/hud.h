/* hud.h - Heads Up Display functions
 * Copyright (c) 2001-2005 Sam Steele
 *
 * This file is part of DCBlap.
 *
 * DCBlap is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * DCBlap is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

//Render the HUD
void render_HUD();

//Reset the HUD values
void reset_HUD();

//Set a status message that fades in / out
void set_status_text(char *text, float r, float g, float b);

//Set a HUD value
void set_hud(int num, int x, int y, char *label, float r, float g, float b);

//Initialize the list of HUD values
void hud_init();

//Get the pixel width of a text string
float txt_width(char *text);