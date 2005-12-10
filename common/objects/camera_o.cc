/* camera_o.cc - DCBlap camera object
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
#include <Tiki/tiki.h>
#include <Tiki/texture.h>

using namespace Tiki;
using namespace Tiki::GL;

#include "objects.h"
#include "camera.h"

int cam_no_move=0;

Camera::Camera() {
}

void Camera::nextFrame(uint64 tm) {
	float gt=tm/1000000.0f;
	Vector p=getPosition();
	Vector r=getRotate();

/*  if(me->arg1>0) {
		me->arg1-=gt;
		if(me->arg1<=0) {
			set_status_text("Go!",1.0f,1.0f,1.0f);
			pause_world(0);
		}
	}
  if(!cam_no_move) {
    if(me->arg1>0) {
      camera_x=me->x;
      camera_y=me->y+(((float)me->arg1/2.0f)*200.0f);
      camera_z=me->z-(((float)me->arg1/2.0f)*200.0f);
      camera_xrot=me->xrot;//-me->arg1*((float)me->xrot/60.0f);
      camera_yrot=me->yrot-90;
      camera_zrot=me->zrot;
    } else {*/
      camera_x=p.x;
      camera_y=p.y;
      camera_z=p.z;
      camera_xrot=r.x;
      camera_yrot=r.y-90;
      camera_zrot=r.z;
/*    }
  }*/
}

void no_move_camera(int mode) {
  cam_no_move=mode;
}
