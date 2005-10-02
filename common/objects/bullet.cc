#include <windows.h>
#include <math.h>
#include "entity.h"
#include "objects.h"
#include "texture.h"

#ifdef WIN32
#define fsin sin
#define fcos cos
#endif

#define rad_to_deg 57.29577951f

void bullet_create(struct entity *me) {
  if(find_tex("bullet")==-1) load_texture("bullet",next_tex_id());
  me->model=new md2Model;
  me->model->Load("bullet.md2");
  me->tex_id=find_tex("bullet");
  me->anim_start=me->model->anim_start("stand");
  me->anim_end=me->model->anim_end("stand");
  me->blendcount=8;
  me->blendpos=0;
}

void bullet_update(struct entity *me) {
  me->x -= (fsin(me->yrot)*rad_to_deg) * 0.5f;
	//me->z += (1-fcos(me->yrot)*rad_to_deg) * 0.5f;
}

void bullet_message(struct entity *me, struct entity *them, char *message) {
  struct entity *tmp;
  if(!strcmp(message,"thud")&&me->deleted==0) {
    if(!strcmp(them->type,"paddle") || !strcmp(them->type,"paddle_md2")) {
      tmp=find_ent_by_prop("goal","num",get_prop(them,"num"));
      tmp->arg3-=5;
    }
    me->deleted=1;
  }
}
