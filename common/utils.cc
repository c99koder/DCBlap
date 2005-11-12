/* utils.cc - misc. utility functions
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

#include <ctype.h>
#include <string.h>

void debug(char *text);

void string_tolower(char *in) {
  int i;
  //printf("string_tolower(%s)\n",in);
  for(i=0;i<strlen(in);i++) {
	//printf("tolower(%c)\n",in[i]);
    in[i]=tolower(in[i]);
  }
}