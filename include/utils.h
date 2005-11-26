/* utils.h - misc. utility functions
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

#include <vector>

//Pause for x seconds
void delay(float seconds);

//Toggle whether to update twice per frame
void set_doublespeed(int speed);

//Scan a directory
void scan_directory(char *path, std::vector<std::string> *s, std::vector<bool> *d, int &count);

//Whether a file exists
bool file_exists(char *path);