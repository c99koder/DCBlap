/* entity.h - entity object definitions
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

#ifndef _ENTITY_H
#define _ENTITY_H
#include "md2Model.h"

//Point in 3D space
struct pt {
        float x;
        float y;
        float z;
				float s;
				float t;
};

//Polygon
struct poly {
        struct pt point[20];
        unsigned int vert_count;
				Texture *tex;
        struct poly *next;
};

//Parameter
struct param {
	char *name;
	char *value;
	struct param *next;
};

//Entity
struct entity {
  int id;
	float x,y,z;
	float xrot,yrot,zrot;
	float xmin,ymin,zmin;
	float xmax,ymax,zmax;
  float xvel,yvel,zvel;
	float alpha;
	bool deleted;
	char *type;
  bool paused;
  bool always_collide;
  bool physics;
	md2Model *model;
	Texture *tex;
	int anim_start;
	int anim_end;
	int framenum;
	int blendpos;
	int blendcount;	
	char *prog;
	int progpos;
	int proglen;
	float arg1;
	float arg2;
	float arg3;
	float arg4;
	float arg5;
	float arg6;
	float arg7;
	float arg8;
	float arg9;
	float arg10;
	void (*update)(struct entity *, float gt);
	void (*message)(struct entity *,struct entity *, char *);
	struct param *param_list;
	struct poly *poly_list;
	struct entity *next;
};

//Get an entity by ID number
struct entity *get_ent(int count);

//Get an entity by type
struct entity *find_ent_by_type(char *type);

//Get an entity by a property value
struct entity *find_ent_by_prop(char *type, char *prop, char *val);

//Create a new entity
struct entity *create_new_entity(char *type, float x, float y, float z, float xrot, float yrot, float zrot, char *model, char *skin);

//Count how many entities of a specific type exist
int ent_count(char *type);

//Load a world file
void load_world(const char *filename);

//Free a world
void destroy_world();

//Render the translucent polys
void render_world_trans();

//Render the opaque polys
void render_world_solid();

//Run the update functions on all entities
void update_world(float gt);

//Network update
void update_net();

//Pause / unpause the world
void pause_world(bool mode);

//Set the directory for world files
void set_world_path(char *path);

//Convert a text string into coordinates
void make_coordinates(char *text,float *x, float *y, float *z);

//Get a property
char *get_prop(struct entity *ent, char *prop_name);

//Set a property
void set_prop(struct entity *ent, char *prop_name, char *value);
#endif
