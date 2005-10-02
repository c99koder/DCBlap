#ifdef WIN32
#include <kos_w32.h>
#include <windows.h>
#endif
#ifdef DREAMCAST
#include <kos.h>
#endif
#include <math.h>
#include <stdlib.h>
#include "entity.h"
#include "texture.h"
#include "callback.h"
#include "player.h"

void player_create(struct entity *me) {
  //if(get_model(get_prop(me,"model"))==NULL) load_md2(get_prop(me,"model"));
  if(find_tex(get_prop(me,"skin"))==-1) load_texture(get_prop(me,"skin"),next_tex_id());
  me->model=new md2Model;
  me->model->Load(get_prop(me,"model"));
  me->tex_id=find_tex(get_prop(me,"skin"));
  me->anim_start=me->model->anim_start("stand");
  me->anim_end=me->model->anim_end("stand");
  me->blendcount=5;
  me->blendpos=0;
  me->arg1=atoi(get_prop(me,"num"));
  me->arg3=me->x;
}

extern bool keys[256];

void player_callback(struct entity *me) {
  struct entity *ball=find_ent_by_type("ball");  
  int xmark;
  float dx,dz;

#ifdef DREAMCAST
  maple_device_t *caddr;
  cont_state_t *cont;
#endif

  if(me->arg5==0) { me->arg5=1; me->arg6=ball->x; }
  xmark=me->arg6;

#ifdef DREAMCAST
  if(!strcmp(get_prop(me,"num"),"1")) {
    caddr=maple_enum_type(0,MAPLE_FUNC_CONTROLLER);
  } else {
    caddr=maple_enum_type(1,MAPLE_FUNC_CONTROLLER);
  }

  if(caddr!=NULL) {
    cont=(cont_state_t *)maple_dev_status(caddr);
    if (cont->buttons&CONT_DPAD_LEFT) {
      if(me->anim_start!=me->model->anim_start("run")) {
        me->anim_start=me->model->anim_start("run");
        me->anim_end=me->model->anim_end("run");  
        me->blendpos=0;
      }
      me->x-=6;
      me->yrot=180;
    }
    else if (cont->buttons&CONT_DPAD_RIGHT) {
      if(me->anim_start!=me->model->anim_start("run")) {
        me->anim_start=me->model->anim_start("run");
        me->anim_end=me->model->anim_end("run");  
        me->blendpos=0;
      }
      me->x+=6;
      me->yrot=0;
    }
    else if (cont->buttons&CONT_DPAD_DOWN) {
      if(me->anim_start!=me->model->anim_start("run")) {
        me->anim_start=me->model->anim_start("run");
        me->anim_end=me->model->anim_end("run");  
        me->blendpos=0;
      }
      me->z-=6;
      me->yrot=-90;
    }
    else if (cont->buttons&CONT_DPAD_UP) {
      if(me->anim_start!=me->model->anim_start("run")) {
        me->anim_start=me->model->anim_start("run");
        me->anim_end=me->model->anim_end("run");  
        me->blendpos=0;
      }
      me->z+=6;
      me->yrot=90;
    } else {
      if(me->anim_start!=me->model->anim_start("stand")) {
        me->anim_start=me->model->anim_start("stand");
        me->anim_end=me->model->anim_end("stand");  
        me->blendpos=0;
      }
    }
#endif
#ifdef WIN32
  if(me->arg1==1) {
    if (keys[VK_LEFT]) {
      if(me->anim_start!=me->model->anim_start("run")) {
        me->anim_start=me->model->anim_start("run");
        me->anim_end=me->model->anim_end("run");  
        me->blendpos=0;
      }
      me->x-=6;
      me->yrot=180;
    }
    else if (keys[VK_RIGHT]) {
      if(me->anim_start!=me->model->anim_start("run")) {
        me->anim_start=me->model->anim_start("run");
        me->anim_end=me->model->anim_end("run");  
        me->blendpos=0;
      }
      me->x+=6;
      me->yrot=0;
    }
    else if (keys[VK_DOWN]) {
      if(me->anim_start!=me->model->anim_start("run")) {
        me->anim_start=me->model->anim_start("run");
        me->anim_end=me->model->anim_end("run");  
        me->blendpos=0;
      }
      me->z-=6;
      me->yrot=-90;
    }
    else if (keys[VK_UP]) {
      if(me->anim_start!=me->model->anim_start("run")) {
        me->anim_start=me->model->anim_start("run");
        me->anim_end=me->model->anim_end("run");  
        me->blendpos=0;
      }
      me->z+=6;
      me->yrot=90;
    } else {
      if(me->anim_start!=me->model->anim_start("stand")) {
        me->anim_start=me->model->anim_start("stand");
        me->anim_end=me->model->anim_end("stand");  
        me->blendpos=0;
      }
    }
    if(me->x<xmark) me->x=xmark;
#endif
  } else {
    if(ball->x>xmark/* || ball->x > me->arg3*/) {
      if(me->anim_start!=me->model->anim_start("stand")) {
        me->anim_start=me->model->anim_start("stand");
        me->anim_end=me->model->anim_end("stand");  
        me->blendpos=0;
      }
    } else {
      if(me->anim_start!=me->model->anim_start("run")) {
        me->anim_start=me->model->anim_start("run");
        me->anim_end=me->model->anim_end("run");  
        me->blendpos=0;
      }
    if((int)ball->z>(int)me->z) {
      me->z+=3;
    }
    if((int)ball->z<(int)me->z) {
      me->z-=3;
    }
    if((int)ball->x>(int)me->x) {
      me->x+=1.4+rand()%3;
    }
    if((int)ball->x<(int)me->x) {
      me->x-=1.4+rand()%3;
    }
    //if(me->z<ball->z) me->yrot=90; else me->yrot=270;
    if(me->x>xmark) me->x=xmark;
    }
    
    if(me->z>ball->z) dz=me->z-ball->z;
    if(me->z<=ball->z) dz=ball->z-me->z;
    if(me->x>ball->x) dx=me->x-ball->x;
    if(me->x<=ball->x) dx=ball->x-me->x;

    if(dz==0) dz=1;
    if(ball->x<me->x && ball->z>me->z) {
      me->yrot=(atan(dx/dz)*57.29577951f)+90; //convert rad to deg
    }
    if(ball->x<me->x && ball->z<=me->z) {
      me->yrot=270-(atan(dx/dz)*57.29577951f); //convert rad to deg
    }
    if(ball->x>=me->x && ball->z>me->z) {
      me->yrot=-(atan(dx/dz)*57.29577951f)+90; //convert rad to deg
    }
    if(ball->x>=me->x && ball->z<=me->z) {
      me->yrot=270+(atan(dx/dz)*57.29577951f); //convert rad to deg
    }
  }
}

void player_message(struct entity *me, struct entity *them, char *message) {
  if(!strcmp(message,"reset")) {
    make_coordinates(get_prop(me,"origin"),&me->x,&me->y,&me->z); //reset position
    make_coordinates(get_prop(me,"angles"),&me->xrot,&me->zrot,&me->yrot); //reset position
  } else if(!strcmp(message,"thud")) {
    //funky math to decide which way to bounce based on where we hit the other object
    if(them->xmin+them->x/100.0f<=me->xmin+me->x/100.0f && them->xmax+them->x/100.0f<=me->xmax+me->x/100.0f) me->x+=3;//(rand()%3)*2;
    if(them->xmin+them->x/100.0f>=me->xmin+me->x/100.0f && them->xmax+them->x/100.0f>=me->xmax+me->x/100.0f) me->x+=-3;//(-rand()%3)*2;
    if(them->zmin+them->z/100.0f<=me->zmin+me->z/100.0f && them->zmax+them->z/100.0f<=me->zmax+me->z/100.0f) me->z+=3;//(rand()%3)*2;
    if(them->zmin+them->z/100.0f>=me->zmin+me->z/100.0f && them->zmax+them->z/100.0f>=me->zmax+me->z/100.0f) me->z+=-3;//(-rand()%3)*2;  
  }
}
