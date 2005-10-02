#include <windows.h>
#include "entity.h"
#include "objects.h"
#include "texture.h"

void tank_create(struct entity *me) {
  int fd;
  struct entity *ent;
  char tmp[100];
  if(find_tex(get_prop(me,"skin"))==-1) jpeg_to_texture(get_prop(me,"skin"),next_tex_id());
  me->model=new md2Model;
  me->model->Load(get_prop(me,"model"));
  me->tex_id=find_tex(get_prop(me,"skin"));
  me->anim_start=me->model->anim_start("stand");
  me->anim_end=me->model->anim_end("stand");
  me->blendcount=8;
  me->blendpos=0;

  /*ent=create_new_entity("rocket",me->x-96,me->y,me->z,me->xrot,me->yrot,me->zrot,"rocket.md2","rocket.bmp");
  sprintf(tmp,"%s_lrocket",get_prop(me,"targetname"));
  set_prop(ent,"targetname",tmp);
  ent=create_new_entity("rocket",me->x+96,me->y,me->z,me->xrot,me->yrot,me->zrot,"rocket.md2","rocket.bmp");
  sprintf(tmp,"%s_rrocket",get_prop(me,"targetname"));
  set_prop(ent,"targetname",tmp);
  create_new_entity("bullet",me->x,me->y,me->z+196,me->xrot,me->yrot,me->zrot,"bullet.md2","bullet.bmp");
  set_prop(ent,"owner",get_prop(me,"targetname"));*/
}

void tank_update(struct entity *me) {
}

void tank_message(struct entity *me, struct entity *them, char *message) {
  printf("Message \"%s\" recieved by a %s\n",message,them->type);
}