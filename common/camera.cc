/* camera.cc - global camera support functions
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

#include <tiki/tikimath.h>

using namespace Tiki::Math;

float camera_x=0.0f;
float camera_y=0.0f;
float camera_z=0.0f;
float camera_xrot=0.0f;
float camera_yrot=0.0f;
float camera_zrot=0.0f;
float camera_zoom=0.0f;
float swoop_x=0.0f;
float swoop_y=0.0f;
float swoop_z=0.0f;
float walkbias=0.0f;

int walkbiasangle;

const float piover180 = 0.0174532925f;
#define rad_to_deg 57.29577951f

void zoom(float step) {
  camera_zoom+=step;
}

float cam_zoom() {
  return camera_zoom;
}

void put_camera(float x, float y, float z, float xrot, float yrot, float zrot) {
  camera_x=x;
  camera_y=y;
  camera_z=z;
  camera_xrot=xrot;
  camera_yrot=yrot;
  camera_zrot=zrot;
}

void walk_forward() {
		camera_x -= (fsin(camera_yrot)*rad_to_deg) * 0.005f;
		camera_z += (fcos(camera_yrot)*rad_to_deg) * 0.005f;
		walkbiasangle+= 10;
		walkbiasangle%=360;
		walkbias=fsin(walkbiasangle * piover180)*0.25f;
}

void walk_backward() {
		camera_x += (fsin(camera_yrot)*rad_to_deg) * 0.005f;
		camera_z -= (fcos(camera_yrot)*rad_to_deg) * 0.005f;
		walkbiasangle-= 10;
		walkbiasangle%=360;
		walkbias=fsin(walkbiasangle * piover180)*0.25f;
}

void strafe_left() {
		camera_x += fsin(camera_yrot+(90*piover180)) * 0.1f;
		camera_z -= fcos(camera_yrot+(90*piover180)) * 0.1f;
		walkbiasangle+= 10;
		walkbiasangle%=360;
		walkbias=fsin(walkbiasangle * piover180)*0.25f;
}

void strafe_right() {
		camera_x += fsin(camera_yrot+(90*piover180)) * 0.1f;
		camera_z += fcos(camera_yrot+(90*piover180)) * 0.1f;
		walkbiasangle-= 10;
		walkbiasangle%=360;
		walkbias=fsin(walkbiasangle * piover180)*0.25f;
}

void move_camera(float xstep, float ystep, float zstep) {
	camera_x+=xstep;
	camera_y+=ystep;
	camera_z+=zstep;
}

void rotate_camera(float xstep, float ystep, float zstep) {
	camera_xrot+=xstep;
	camera_yrot+=ystep;
	camera_zrot+=zstep;
}

float cam_x() {
	return camera_x;
}

float cam_y() {
	return camera_y;//+walkbias;
}
float cam_z() {
	return camera_z;
}

float cam_xrot() {
	return camera_xrot;
}

float cam_yrot() {
	return camera_yrot;
}

float cam_zrot() {
	return camera_zrot;
}

