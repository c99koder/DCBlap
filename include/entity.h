#ifndef _ENTITY_H
#define _ENTITY_H
#include "md2Model.h"
struct pt {
        float x;
        float y;
        float z;
				float s;
				float t;
};

struct poly {
        struct pt point[20];
        unsigned int vert_count;
        unsigned int tex_id;
        struct poly *next;
};


struct param {
	char *name;
	char *value;
	struct param *next;
};

//typedef void (*update_callback_ent)(void *);

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
        int tex_id;
        int anim_start;
        int anim_end;
        int framenum;
        int blendpos;
        int blendcount;	
	char *prog;
        int progpos;
	int proglen;
        int arg1;
        int arg2;
        int arg3;
        int arg4;
        int arg5;
        int arg6;
        int arg7;
        int arg8;
        int arg9;
        int arg10;
        void (*update)(struct entity *);
        void (*message)(struct entity *,struct entity *, char *);
	struct param *param_list;
	struct poly *poly_list;
	struct entity *next;
};

struct entity *get_ent(int count);
struct entity *find_ent_by_type(char *type);
struct entity *find_ent_by_prop(char *type, char *prop, char *val);
struct entity *create_new_entity(char *type, float x, float y, float z, float xrot, float yrot, float zrot, char *model, char *skin);
int ent_count(char *type);

void load_world(char *filename);
void destroy_world();
void load_md2(char *filename);
void render_world_trans();
void render_world_solid();
void update_world();
void update_net();
void pause_world(bool mode);
void set_world_path(char *path);
void make_coordinates(char *text,float *x, float *y, float *z);
char *get_prop(struct entity *ent, char *prop_name);
void set_prop(struct entity *ent, char *prop_name, char *value);
struct md2_model_node *get_model(char *filename);

int anim_start(md2_model_node *model, char*name);
int anim_end(md2_model_node *model, char*name);

#endif
