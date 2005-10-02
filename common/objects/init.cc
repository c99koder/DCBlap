/* init.cc - DCBlap object initialization
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
#include "objects.h"
#include "callback.h"
#define NULL 0

void objects_init() {
  register_callbacks("camera",camera_create,camera_callback,NULL,NULL);
  register_callbacks("paddle",paddle_create,paddle_update,paddle_message,paddle_reset);
  register_callbacks("paddle_md2",paddlemd2_create,paddlemd2_update,paddlemd2_message,paddlemd2_reset);
  register_callbacks("ball",ball_create,ball_update,ball_message,NULL);
  register_callbacks("goal",goal_create,goal_callback,goal_message,NULL);
  register_callbacks("speedup",speedup_create,speedup_update,speedup_message,NULL);
  register_callbacks("slowdown",slowdown_create,slowdown_update,slowdown_message,NULL);
  register_callbacks("bonus_spawn",bonus_create,bonus_update,bonus_message,NULL);
  register_callbacks("hyper",hyper_create,hyper_update,hyper_message,NULL);
  register_callbacks("func_rotating",func_rotate_create,func_rotate_update,func_rotate_message,NULL);
  register_callbacks("water",water_create,NULL,NULL,NULL);
  register_callbacks("glass",water_create,NULL,NULL,NULL);
  register_callbacks("func_breakable",func_break_create,func_break_update,func_break_message,NULL);
}
