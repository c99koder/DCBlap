/* bonusspawn.cc - DCBlap bonus spawn object
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
#include "utils.h"

#ifdef DREAMCAST
sfxhnd_t sfx_speedup=-1;
sfxhnd_t sfx_slowdown=-1;
sfxhnd_t sfx_hyper=-1;
#endif 
#ifdef LINUX
Mix_Chunk *sfx_speedup=NULL;
Mix_Chunk *sfx_slowdown=NULL;
Mix_Chunk *sfx_hyper=NULL;
#endif
#ifdef WIN32
extern CSoundManager *g_pSoundManager;
CSound *sfx_slowdown=NULL;
CSound *sfx_speedup=NULL;
CSound *sfx_hyper=NULL;
#endif

extern bool enable_powerups;

void bonus_create(struct entity *me) {
	struct entity *ent2=find_ent_by_type("worldspawn");
	me->arg1=ent2->x;
	me->arg2=ent2->z;
#ifdef DREAMCAST
	if(sfx_speedup==-1) sfx_speedup=snd_sfx_load("speedup.wav");
	if(sfx_slowdown==-1) sfx_slowdown=snd_sfx_load("slowdown.wav");
	if(sfx_hyper==-1) sfx_hyper=snd_sfx_load("hyper.wav");
#endif
#ifdef LINUX
	if(sfx_speedup==NULL) sfx_speedup=Mix_LoadWAV("speedup.wav");
	if(sfx_slowdown==NULL) sfx_slowdown=Mix_LoadWAV("slowdown.wav");
	if(sfx_hyper==NULL) sfx_hyper=Mix_LoadWAV("hyper.wav");
#endif
#ifdef WIN32
  if(sfx_slowdown==NULL) g_pSoundManager->Create( &sfx_slowdown, "slowdown.wav", 0, GUID_NULL );
  if(sfx_speedup==NULL) g_pSoundManager->Create( &sfx_speedup, "speedup.wav", 0, GUID_NULL );
  if(sfx_hyper==NULL) g_pSoundManager->Create( &sfx_hyper, "hyper.wav", 0, GUID_NULL );
#endif
}

void bonus_update(struct entity *me, float gt) {
	struct entity *ent2=0;
  if(me->paused==1) return;
	if(enable_powerups && (rand()%800==0)) {
		switch(rand()%5) {
			case 0:
			case 3:
				ent2=create_new_entity("speedup",me->arg1,me->y,me->arg2,0,0,0,NULL,NULL);
				break;
			case 1:
			case 4:
				ent2=create_new_entity("slowdown",me->arg1,me->y,me->arg2,0,0,0,NULL,NULL);
				break;
			case 2:
				ent2=create_new_entity("hyper",me->arg1,me->y,me->arg2,0,0,0,NULL,NULL);
				break;
		}
	}
}

void bonus_message(struct entity *me, struct entity *them, char *message) {
  if(!strcmp(message,"reset")) {
    set_doublespeed(0);
  }
}
