/* ball.cc - DCBlap ball object
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
#include <Tiki/sound.h>

using namespace Tiki;
using namespace Tiki::GL;
using namespace Tiki::Audio;

#include <stdlib.h>
#include <string.h>
#include "entity.h"
#include "objects.h"
#include "camera.h"
#include "utils.h"
#include "hud.h"

Sound *sfx_bounce=NULL;
Sound *sfx_score=NULL;

extern bool enable_sound;
extern struct entity *wld;
extern bool lose_flag;
extern int doublespeed;
bool ball_spin=1;

Texture *ball_tex=NULL;

void ball_reset() {
	if(ball_tex!=NULL) {
		delete ball_tex;
		ball_tex=NULL;
	}
}

void ball_create(struct entity *me) {
  me->model=new md2Model;
	//if(get_prop(me,"model")!=NULL) {
	//	me->model->Load(get_prop(me,"model"));
	//} else {
		me->model->Load("model/ball.md2");
	//}
	if(ball_tex==NULL) ball_tex= new Texture("tex/ball.png",0);
	me->tex=ball_tex;
	me->blendcount=1;
  me->anim_start=me->model->anim_start("stand");
  me->anim_end=me->model->anim_end("stand");
  me->blendpos=0;
  me->model->SetFrame(0,0,0,1);
	if(get_prop(me,"speed")) {
		me->arg3=atoi(get_prop(me,"speed"));
	} else {
		me->arg3=3;
		me->arg6=1;
	}
	if(get_prop(me,"orientation")) {
		me->arg5=atoi(get_prop(me,"orientation"));
	}
  if(get_prop(me,"spin")) {
    ball_spin=atoi(get_prop(me,"spin"));
  } else {
    ball_spin=1;
  }
  //ball_spin=0;
  switch((int)me->yrot) {
  case 0:
    me->arg1=me->arg3;
    me->arg2=0;
    break;
  case 90:
    me->arg1=0;
    me->arg2=me->arg3;
    break;
  case 180:
    me->arg1=-me->arg3;
    me->arg2=0;
    break;
  case 270:
    me->arg1=0;
    me->arg2=-me->arg3;
    break;
  }

#if TIKI_PLAT != TIKI_DC
	if(sfx_bounce==NULL) sfx_bounce = new Sound("sound/bounce.wav");
	if(sfx_score==NULL) sfx_score = new Sound("sound/score.wav");
#endif
}

void ball_update(struct entity *me, float gt) {
  static int cnt=0;
  char buf[100];
  if(ball_spin) {
    me->yrot+=6 * gt;
    //me->xrot+=6 * gt * 40;
    if(me->yrot>360) me->yrot=0;
    //if(me->xrot>360) me->xrot=0;
  }

  if(me->paused==1) return;

  me->x+=me->arg1 * gt * 40;
  if(me->arg5==0) {
    me->z+=me->arg2 * gt * 40;
  } else {
    me->y+=me->arg2 * gt * 40;
  }
}

void ball_message(struct entity *me, struct entity *them, char *message) {
  char buf[200];
  int x,y;
  if(!strcmp(message,"win")) {
    me->deleted=1;
  } else if(!strcmp(message,"out") && me->paused==0) {
    set_status_text("Out of bounds",1,1,1);
    broadcast_message(me,"reset");
  } else if(!strcmp(message,"reset")) {
		if(me->arg6==1) me->deleted=1;
    make_coordinates(get_prop(me,"origin"),&me->x,&me->y,&me->z); //reset position
    make_coordinates(get_prop(me,"angles"),&me->xrot,&me->zrot,&me->yrot); //reset position
    me->anim_start=me->model->anim_start("stand");
    me->anim_end=me->model->anim_end("stand");
    me->blendpos=0;
    switch((int)me->yrot) {
    case 0:
      me->arg1=me->arg3;
      me->arg2=0;
      break;
    case 90:
      me->arg1=0;
      me->arg2=me->arg3;
      break;
    case 180:
      me->arg1=-me->arg3;
      me->arg2=0;
      break;
    case 270:
      me->arg1=0;
      me->arg2=-me->arg3;
      break;
    }
  } else if(!strcmp(message,"thud") && me->anim_start==me->model->anim_start("stand")) {
    if(!strcmp(them->type,"goal")) {
      doublespeed=0;
      if(wld->arg2>0) { //take away a ball
        wld->arg1--;
        if(wld->arg1<1) {
          pause_world(1);
          set_hud(0,320-(txt_width("Game Over")/2),240-16,"Game Over",1,1,1);
          set_hud(2,320-(txt_width("Press Start")/2),244,"Press Start",1,1,1);
          set_prop(wld,"next_map","none");
          broadcast_message(me,"win");
					lose_flag=1;
        }
      } else {
        them->arg1++; //give the goal a point
        if(!strcmp(get_prop(them,"num"),"1")) {
          me->arg1=me->arg3;
        } else {
          me->arg1=-me->arg3;
        }
      }
	    if(enable_sound && sfx_score!=NULL) sfx_score->play();
      me->arg2=0;
      broadcast_message(me,"reset");
    } else {
			if(enable_sound && sfx_bounce!=NULL) sfx_bounce->play();
      /*me->x-=me->arg1;
	  	if(me->arg5==0) {
		  	me->z-=me->arg2;
  		} else {
	  		me->y-=me->arg2;
		  }*/
      if(!strcmp(them->type,"paddle")) {
        me->arg7=them->arg1;
        //me->arg3=them->arg4+1;
      }
      if(them->physics==1) {
        x=me->arg1;
        y=me->arg2;
        if(them->xvel!=0) me->arg1=them->xvel*2;
        if(them->zvel!=0) me->arg2=them->zvel*2;
        them->xvel=x/2;
        them->zvel=y/2;
      } else {
    //funky math to decide which way to bounce based on where we hit the other object
        if(them->xmin+them->x/100.0f<=me->xmin+me->x/100.0f && them->xmax+them->x/100.0f<=me->xmax+me->x/100.0f) me->arg1=me->arg3+(rand()%2);
        if(them->xmin+them->x/100.0f>=me->xmin+me->x/100.0f && them->xmax+them->x/100.0f>=me->xmax+me->x/100.0f) me->arg1=-me->arg3-(rand()%2);
			  if(me->arg5==0) {
				  if(them->zmin+them->z/100.0f<=me->zmin+me->z/100.0f && them->zmax+them->z/100.0f<=me->zmax+me->z/100.0f) me->arg2=me->arg3+(rand()%2);
				  if(them->zmin+them->z/100.0f>=me->zmin+me->z/100.0f && them->zmax+them->z/100.0f>=me->zmax+me->z/100.0f) me->arg2=-me->arg3-(rand()%2);  
			  } else {
				  if(them->ymin+them->y/100.0f<=me->ymin+me->y/100.0f && them->ymax+them->y/100.0f<=me->ymax+me->y/100.0f) me->arg2=me->arg3+(rand()%2);
				  if(them->ymin+them->y/100.0f>=me->ymin+me->y/100.0f && them->ymax+them->y/100.0f>=me->ymax+me->y/100.0f) me->arg2=-me->arg3-(rand()%2);  
			  }
        if(me->arg2==0) {
          me->arg2=((rand()%2)-1)*(me->arg3-(rand()%2));
        }
        if(me->arg1==0) {
          me->arg1=((rand()%2)-1)*(me->arg3-(rand()%2));
        }
      }
      /*me->x+=me->arg1;
		  if(me->arg5==0) {
			  me->z+=me->arg2;
		  } else {
			  me->y+=me->arg2;
		  }*/
    }
  }
}
