/* paddle.cc - DCBlap paddle object
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
#ifdef WIN32
#include <windows.h>
#endif
#ifdef DREAMCAST
#include <kos.h>
#endif
#include <string.h>
#include <stdlib.h>
#include "entity.h"
#include "objects.h"
#include "texture.h"
#include "direction.h"
#include "text.h"
#include "hud.h"
#include "utils.h"
#include "camera.h"

extern int thinker[];
extern int playermap[];
extern struct entity *ball;
extern bool win_flag;

#ifdef WIN32
extern SOCKET net_socket;
#else
extern int net_socket;
#endif

void paddlemd2_reset() {
	for(int i=0;i<4;i++) {
		thinker[i]=0;
	}
	ball=NULL;
}

void no_move_camera(int mode);

void paddlemd2_create(struct entity *me) {
  char tmp[40];
  if(find_tex(get_prop(me,"skin"))==-1) load_texture(get_prop(me,"skin"),next_tex_id());
  me->model=new md2Model;
  me->model->Load(get_prop(me,"model"));
  me->tex_id=find_tex(get_prop(me,"skin"));
  me->anim_start=me->model->anim_start("stand");
  me->anim_end=me->model->anim_end("stand");
  me->blendcount=8;
  me->blendpos=0;
  me->model->SetFrame(0,0,0,1);
  me->arg1=atoi(get_prop(me,"num"));
  me->arg4=atoi(get_prop(me,"speed"));
	if(get_prop(me,"orientation")) {
		me->arg5=atoi(get_prop(me,"orientation"));
	}
  sprintf(tmp,"%f %f %f",me->x, me->z, me->y);
  set_prop(me,"origin",tmp);
  if(thinker[(int)me->arg1-1]==0) {
    me->arg2=1;
    thinker[(int)me->arg1-1]=1;
  }
  if(get_prop(me,"intro_effect")!=NULL) {
    me->arg6=atoi(get_prop(me,"intro_effect"));
    me->arg7=atoi(get_prop(me,"intro_speed"));
  }
  if(me->arg7>50) me->arg7=50;
  me->arg8=me->y;
  me->arg3=0;
  //if(playermap[me->arg1-1]!=-1) me->physics=1;
}

bool sys_render_begin();
void sys_render_finish();
void render_bg(float a);

void paddlemd2_update(struct entity *me) {
  char buf[100];
  float x=0,y=0;
	if(ball==NULL) ball=find_ent_by_type("ball");
  if(me->arg7>0) {
    switch((int)me->arg6) {
    case 1: //fade in
      me->alpha=(50.0f-(float)me->arg7)/50.0f;
      me->arg7--;
      break;
    case 2: //drop from sky
      me->y=me->arg8+(me->arg7*10);
      me->arg7--;
      break;
    case 3: //fade out
      me->alpha=(me->arg7)/50.0f;
      me->arg7--;
      break;
    case 4: //fly into sky
      me->y=me->arg8+((50-me->arg7)*10);
      me->arg7--;
      break;
    case 5: //winning spin
      me->anim_end=me->anim_start;
      me->zrot++;
      me->yrot++;
      if(me->zrot>=360) me->zrot=0;
      if(me->yrot>=360) me->yrot=0;
      break;
    }
  }

  if(me->paused==1&&me->arg9==0) return;

  if(me->arg3>0) me->arg3--;

  /*read_analog(&x,&y);
  me->xvel+=x;
  me->zvel-=y;

	if(playermap[me->arg1-1]!=-1 && me->arg2==1) {
    switch(get_controls(me->arg1-1)) {
      case MOVE_RIGHT:
        if(me->arg9==0) me->xvel+=me->arg4/3;
        break;
      case MOVE_UP:
        if(me->arg9==0) me->zvel+=me->arg4/3;
        break;
      case MOVE_LEFT:
        if(me->arg9==0) me->xvel-=me->arg4/3;
        break;
      case MOVE_DOWN:
        if(me->arg9==0) me->zvel-=me->arg4/3;
        me->zrot--;
        break;
      case FIRE_BTN:
        if(me->arg3==0) {
          me->arg3=10;
          create_new_entity("bullet",me->x+me->xmax+30,me->y,me->z,0,me->yrot-90,0,"bullet.md2","bullet");
        }
        break;
      case START_BTN:
        if(me->arg9==1) {
          if(me->arg5==10) {
            win_flag=1;
          } else {
            pause_world(0);
            set_hud(3,320-(txt_width("Paused")/2),240-16,"",1,1,1);
            me->arg9=0;
          }
        } else {
          me->arg9=1;
          pause_world(1);
          set_hud(3,320-(txt_width("Paused")/2),240-16,"Paused",1,1,1);
        }
        delay(.1);
        break;
      default:
        thinker[me->arg1-1]=0;
    }
    if(me->xvel>me->arg4*1.5) me->xvel=me->arg4*1.5;
    if(me->xvel<-me->arg4*1.5) me->xvel=-me->arg4*1.5;
    if(me->zvel>me->arg4*1.5) me->zvel=me->arg4*1.5;
    if(me->zvel<-me->arg4*1.5) me->zvel=-me->arg4*1.5;
    me->zrot=-me->xvel*(15.0f/me->arg4);
    me->xrot=-me->zvel*(15.0f/me->arg4);
    me->x+=me->xvel;
    me->y+=me->yvel;
    me->z+=me->zvel;
    if(me->xvel>0) {
      me->xvel-=me->arg4/8;
      if(me->xvel<0) me->xvel=0;
    }
    if(me->xvel<0) {
      me->xvel+=me->arg4/8;
      if(me->xvel>0) me->xvel=0;
    }
    if(me->zvel>0) {
      me->zvel-=me->arg4/8;
      if(me->zvel<0) me->zvel=0;
    }
    if(me->zvel<0) {
      me->zvel+=me->arg4/8;
      if(me->xvel>0) me->zvel=0;
    }
    
#ifdef WIN32
    if(net_socket!=-1 && thinker[me->arg1-1]!=0) {
      sprintf(buf,"1,%i,%i\n",me->id,thinker[me->arg1-1]);
      socket_write_line(net_socket,buf);
    }
#endif
  } else {*/
    if(me->arg2==1) {
      switch((int)me->arg5) {
      case 0:
				if((int)ball->z>(int)me->z) {
					thinker[(int)me->arg1-1]=me->arg4+(rand()%2);
				}
				if((int)ball->z<(int)me->z) {
					thinker[(int)me->arg1-1]=-(me->arg4+(rand()%2));
				}
        break;
      case 1:
				if((int)ball->y>(int)me->y) {
					thinker[(int)me->arg1-1]=me->arg4+(rand()%2);
				}
				if((int)ball->y<(int)me->y) {
					thinker[(int)me->arg1-1]=-(me->arg4+(rand()%2));
				}
        break;
      case 2:
        if((int)ball->x>(int)me->x) {
					thinker[(int)me->arg1-1]=me->arg4+(rand()%2);
				}
				if((int)ball->x<(int)me->x) {
					thinker[(int)me->arg1-1]=-(me->arg4+(rand()%2));
				}
        break;
			}
		}
  //}
  if(me->paused==1) return;
  me->alpha=1.0f;
	switch((int)me->arg5) {
  case 0:
		me->z+=thinker[(int)me->arg1-1];
    break;
  case 1:
		me->y+=thinker[(int)me->arg1-1];
    break;
  case 2:
    me->x+=thinker[(int)me->arg1-1];
    break;
	}
  //if(me->arg1==1) put_camera(me->x,me->y+5,me->z,me->xrot,me->yrot-90,me->zrot);
}

void paddlemd2_message(struct entity *me, struct entity *them, char *message) {
  if(!strcmp(message,"reset")) {
    make_coordinates(get_prop(me,"origin"),&me->x,&me->y,&me->z);
    //me->z=0;
  } else if(!strcmp(message,"win")) {
  	me->arg4=atoi(get_prop(me,"speed")); //undo the powerup changes
    if(them->arg2!=me->arg1) {
      me->arg6+=2;
    } else {
      me->arg6=5;
      me->y=me->arg8+20;
    }
    me->arg7=50;
    me->arg5=10;
    me->arg9=1;
  }
}
