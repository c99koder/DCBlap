/* hyper.cc - DCBlap hyperspeed powerup object
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

extern Sound *sfx_bounce;
Sound *sfx_hyper=NULL;

extern bool enable_sound;

Texture *hyper_tex=NULL;

void hyper_reset() {
	if(hyper_tex!=NULL) {
		delete hyper_tex;
		hyper_tex=NULL;
	}
}

void hyper_create(struct entity *me) {
  me->model=new md2Model;
  me->model->Load("hyper.md2");
  if(hyper_tex==NULL) hyper_tex=new Texture("hyper.png",0);
	me->tex=hyper_tex;
  me->anim_start=me->model->anim_start("stand");
  me->anim_end=me->model->anim_end("stand");
  me->arg3=2;
  me->blendcount=8;
  me->blendpos=0;
  do {
		me->arg1=-2+(rand()%5);
		me->arg2=-2+(rand()%5);
	} while(me->arg1==0 || me->arg2==0);
  me->arg3=1+rand()%3;
	if(sfx_hyper==NULL) sfx_hyper=new Sound("hyper.wav");
}

void hyper_update(struct entity *me, float gt) {
  me->yrot++;
  me->zrot++;
  me->xrot++;
  if(me->paused==1) return;
  me->x+=me->arg1*1.5f;
  me->z+=me->arg2*1.5f;
  if(me->xrot>359) me->xrot=0;
  if(me->yrot>359) me->yrot=0;
  if(me->zrot>359) me->zrot=0;
}

void hyper_message(struct entity *me, struct entity *them, char *message) {
  if(!strcmp(message,"reset")) {
		me->deleted=1;
    me->anim_start=me->model->anim_start("stand");
    me->anim_end=me->model->anim_end("stand");
    me->blendpos=0;
	  me->yrot=0;
  } else if(!strcmp(message,"thud")) {
    if(!strcmp(them->type,"goal")) {
			me->deleted=1;
    } else if(!strcmp(them->type,"paddle")||!strcmp(them->type,"paddle_md2")) {
			me->deleted=1;
			set_status_text("Hyper Speed!!",.968f,.866f,.054f);
      set_doublespeed(1);
      them->arg4++; //increase speed
			if(enable_sound) sfx_hyper->play();
    } else {
			if(enable_sound) sfx_bounce->play();
      me->x-=me->arg1;
	  	if(me->arg5==0) {
		  	me->z-=me->arg2;
  		} else {
	  		me->y-=me->arg2;
		  }

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
      me->x+=me->arg1;
		  if(me->arg5==0) {
			  me->z+=me->arg2;
		  } else {
			  me->y+=me->arg2;
		  }
    }
  }
}
