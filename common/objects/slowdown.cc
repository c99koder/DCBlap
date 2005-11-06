/* slowdown.cc - DCBlap slowdown powerup object
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
#ifdef WIN32
#include <windows.h>
#include "dsutil.h"
#endif
#ifdef DREAMCAST
#include <kos.h>
#endif
#ifdef LINUX
#include <SDL/SDL_mixer.h>
#endif
#ifdef TIKI
#include <Tiki/tiki.h>
#include <Tiki/texture.h>

using namespace Tiki;
using namespace Tiki::GL;
#endif
#include <stdlib.h>
#include <string.h>
#include "entity.h"
#include "objects.h"
#include "camera.h"
#include "hud.h"

#ifdef DREAMCAST
extern sfxhnd_t sfx_bounce;
extern sfxhnd_t sfx_slowdown;
#endif
#ifdef LINUX
extern Mix_Chunk *sfx_bounce;
extern Mix_Chunk *sfx_slowdown;
#endif
#ifdef WIN32
extern CSoundManager *g_pSoundManager;
extern CSound *sfx_bounce;
extern CSound *sfx_slowdown;
#endif

extern bool enable_sound;

Texture *slowdown_tex=NULL;

void slowdown_reset() {
	if(slowdown_tex!=NULL) {
		delete slowdown_tex;
		slowdown_tex=NULL;
	}
}

void slowdown_create(struct entity *me) {
  int fd;
  struct entity *ent;
  char tmp[100];
  me->model=new md2Model;
  me->model->Load("slowdown.md2");
  if(slowdown_tex==NULL) slowdown_tex = new Texture("slowdown.png",0);
	me->tex=slowdown_tex;
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
#ifdef WIN32
  if(sfx_bounce==NULL) g_pSoundManager->Create( &sfx_bounce, "bounce.wav", 0, GUID_NULL );
  if(sfx_slowdown==NULL) g_pSoundManager->Create( &sfx_slowdown, "slowdown.wav", 0, GUID_NULL );
#endif
#ifdef DREAMCAST
  if(sfx_slowdown==-1) sfx_slowdown=snd_sfx_load("slowdown.wav");
#endif
}

#define M_PI 3.141592653589793238

void slowdown_update(struct entity *me, float gt) {
  me->yrot++;
  me->zrot++;
  me->xrot++;
  if(me->paused==1) return;
  me->x+=me->arg1*1.5;
  me->z+=me->arg2*1.5;
  if(me->xrot>359) me->xrot=0;
  if(me->yrot>359) me->yrot=0;
  if(me->zrot>359) me->zrot=0;
}

void slowdown_message(struct entity *me, struct entity *them, char *message) {
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
			set_status_text("Slow Down!",.9,.1,.1);
			them->arg4--; //decrease speed
			if(them->arg4<1) them->arg4=1;
#ifdef DREAMCAST
      if (enable_sound) snd_sfx_play(sfx_slowdown,255,128);
#endif
#ifdef WIN32
	  if(enable_sound) sfx_slowdown->Play(0,0);
#endif
#ifdef LINUX
	if(enable_sound) Mix_PlayChannel(-1,sfx_slowdown,0);
#endif
    } else {
#ifdef DREAMCAST
      if(enable_sound) snd_sfx_play(sfx_bounce,255,128);
#endif
#ifdef WIN32
	  if(enable_sound) sfx_bounce->Play(0,0);
#endif
#ifdef LINUX
	if(enable_sound) Mix_PlayChannel(-1,sfx_bounce,0);
#endif
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
