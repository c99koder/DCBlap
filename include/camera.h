/* camera.h - global camera support functions
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

//Functions to get the global camera's position
float cam_x();
float cam_y();
float cam_z();
float cam_xrot();
float cam_yrot();
float cam_zrot();
float cam_zoom();

//Zoom the camera in
void zoom(float step);

//"Walk" the camera
void move_camera(float xstep, float ystep, float zstep);

//Turn the camera
void rotate_camera(float xstep, float ystep, float zstep);

//Position the camera
void put_camera(float x, float y, float z, float xrot, float yrot, float zrot);

//Disallow camera movement
void no_move_camera(int mode);

extern float camera_x;
extern float camera_y;
extern float camera_z;
extern float camera_xrot;
extern float camera_yrot;
extern float camera_zrot;
