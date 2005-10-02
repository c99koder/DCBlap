#include <windows.h>
#include "entity.h"
#include "objects.h"
#include "texture.h"

void rocket_create(struct entity *me) {
  int fd;
  struct entity *ent;
  if(find_tex(get_prop(me,"skin"))==-1) jpeg_to_texture(get_prop(me,"skin"),next_tex_id());
  me->model=new md2Model;
  me->model->Load(get_prop(me,"model"));
  me->tex_id=find_tex(get_prop(me,"skin"));
  me->anim_start=me->model->anim_start("stand");
  me->anim_end=me->model->anim_end("stand");
  me->blendcount=8;
  me->blendpos=0;
}

void rocket_update(struct entity *me) {
}