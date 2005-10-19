/* camera_o.cc - DCBlap camera object
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
#ifdef TIKI
#include <Tiki/tiki.h>
#include <Tiki/texture.h>

using namespace Tiki;
using namespace Tiki::GL;
#endif
#include "entity.h"
#include "callback.h"
#include "objects.h"
#include "camera.h"
#include "hud.h"

int cam_no_move=0;

void camera_create(struct entity *me) {
  me->arg1=60;
  set_status_text("Ready?",1.0f,1.0f,1.0f);
}

void camera_callback(struct entity *me, float gt) {
  if(me->arg1>0) me->arg1-=gt * 40;
  if(!cam_no_move) {
    if(me->arg1>0) {
      camera_x=me->x;
      camera_y=me->y+(((float)me->arg1/60.0f)*200.0f) * gt * 40;
      camera_z=me->z-(((float)me->arg1/60.0f)*200.0f) * gt * 40;
      camera_xrot=me->xrot;//-me->arg1*((float)me->xrot/60.0f);
      camera_yrot=me->yrot-90;
      camera_zrot=me->zrot;
    } else {
      camera_x=me->x;
      camera_y=me->y;
      camera_z=me->z;
      camera_xrot=me->xrot;
      camera_yrot=me->yrot-90;
      camera_zrot=me->zrot;
    }
  }
  if(me->arg1==1) {
    set_status_text("Go!",1.0f,1.0f,1.0f);
    pause_world(0);
  }
}

void no_move_camera(int mode) {
  cam_no_move=mode;
}
