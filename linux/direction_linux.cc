/* direction_linux.cc - Input for SDL-based systems
 * Copyright (c) 2001-2003 Sam Steele
 *
 * This file is part of DCBlap.
 *
 * DCBlap is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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
#include <SDL/SDL.h>
#include "direction.h"

extern bool keys[256];
bool sys_render_begin();

int get_controls(int num) {
  sys_render_begin();
	if (keys[SDLK_RETURN]) {
		return START_BTN;
    }
    else if (keys[SDLK_ESCAPE]) {
      return QUIT_BTN;
    }
	switch(num) {
	case 0:
    if (keys[SDLK_LEFT]) {
      return MOVE_LEFT;
		}
    else if (keys[SDLK_RIGHT]) {
      return MOVE_RIGHT;
    }
    else if (keys[SDLK_UP]) {
      return MOVE_UP;
    }
    else if (keys[SDLK_DOWN]) {
      return MOVE_DOWN;
    }
		break;
	case 1:
		if(keys['A']) {
			return MOVE_LEFT;
		}
		else if (keys['D']) {
			return MOVE_RIGHT;
		}
		else if (keys['W']) {
			return MOVE_UP;
		}
		else if (keys['S']) {
			return MOVE_DOWN;
		}
		break;
	}
	return 0;
}
