/* direction_dc.c - controller polling for Dreamcast
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
#include <kos.h>
#include "direction.h"

extern "C" {
  void update_lcds();
}

void read_analog(float *x, float *y) {
  mouse_cond_t mcond;
  cont_state_t *ccond;
  uint8 mmouse=maple_first_mouse();
  maple_device_t *caddr=maple_enum_dev(0,0);

  if(caddr) {
    ccond=(cont_state_t*)maple_dev_status(caddr);
    *x+=ccond->joyx/8;
    *y+=ccond->joyy/8;
  }

  if(mmouse) {
    if (mouse_get_cond(mmouse, &mcond) < 0) return;
    *x+=mcond.dx;
    *y+=mcond.dy;
  }
}

int get_controls(int num) {
  maple_device_t *caddr=maple_enum_dev(num,0);
  cont_state_t *cont;
  update_lcds();
  if(caddr==NULL) return -1;
  cont=(cont_state_t*)maple_dev_status(caddr);

  if(cont->buttons & CONT_DPAD_UP) {
    return MOVE_UP;
  }
  if(cont->buttons & CONT_DPAD_DOWN) {
    return MOVE_DOWN;
  }
  if(cont->buttons & CONT_DPAD_LEFT) {
    return MOVE_LEFT;
  }
  if(cont->buttons & CONT_DPAD_RIGHT) {
    return MOVE_RIGHT;
  }
  if(cont->buttons & CONT_A) {
    return FIRE_BTN;
  }
  if(cont->buttons & CONT_B) {
    return QUIT_BTN;
  }
  if(cont->buttons & CONT_START) {
    return START_BTN;
  }
  return 0;
}
