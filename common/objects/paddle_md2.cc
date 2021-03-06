/* paddle_md2.cc - DCBlap paddle object (md2 version)
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

#include <string.h>
#include <stdlib.h>
#include "drawables/entity.h"
#include "objects.h"
#include "hud.h"
#include "utils.h"
#include "camera.h"

extern int thinker[];
extern int playermap[];
extern struct entity *ball;
extern bool win_flag;

void paddlemd2_reset() {
	for(int i=0;i<4;i++) {
		thinker[i]=0;
	}
	ball=NULL;
}

void no_move_camera(int mode);

void paddlemd2_create(struct entity *me) {
  char tmp[40];
  me->model=new md2Model;
  me->model->Load(get_prop(me,"model"));
	strcpy(tmp,get_prop(me,"skin"));
	strcat(tmp,".png");
  me->tex=new Texture(tmp,0);
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

extern int player_axis_x[];
extern int player_axis_y[];

void paddlemd2_update(struct entity *me, float gt) {
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

	me->yrot--;
	
	if(playermap[(int)me->arg1-1]!=-1 && me->arg2==1) {
		thinker[(int)me->arg1-1]=0;
		if(me->arg9==0) me->xvel+=(me->arg4/3) * player_axis_x[(int)me->arg1-1];
		if(me->arg9==0) me->zvel-=(me->arg4/3) * player_axis_y[(int)me->arg1-1];
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
  } else {
    if(me->arg2==1) {
      switch((int)me->arg5) {
				case 0:
					if(ball->z>(me->z+(me->zmax*100)*1.2f)) {
						thinker[(int)me->arg1-1]=me->arg4+(rand()%3);
					}
					else if(ball->z<(me->z+(me->zmin*100)*1.2f)) {
						thinker[(int)me->arg1-1]=-(me->arg4+(rand()%3));
					} else {
						thinker[(int)me->arg1-1]=0;
					}
					break;
				case 1:
					if(ball->y>(me->y+(me->ymax*100)*1.2f)) {
						thinker[(int)me->arg1-1]=me->arg4+(rand()%3);
					}
					else if(ball->y<(me->y+(me->ymin*100)*1.2f)) {
						thinker[(int)me->arg1-1]=-(me->arg4+(rand()%3));
					} else {
						thinker[(int)me->arg1-1]=0;
					}
					break;
				case 2:
					if(ball->x>(me->x+(me->xmax*100)*1.2f)) {
						thinker[(int)me->arg1-1]=me->arg4+(rand()%3);
					}
					else if(ball->x<(me->x+(me->xmin*100)*1.2f)) {
						thinker[(int)me->arg1-1]=-(me->arg4+(rand()%3));
					} else {
						thinker[(int)me->arg1-1]=0;
					}
					break;
			}
		}
  }
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
