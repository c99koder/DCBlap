/* bonusspawn.cc - DCBlap bonus spawn object
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

extern bool enable_powerups;

extern Sound *sfx_speedup;
extern Sound *sfx_slowdown;
extern Sound *sfx_hyper;

extern Texture *slowdown_tex;
extern Texture *speedup_tex;
extern Texture *hyper_tex;

void bonus_create(struct entity *me) {
	struct entity *ent2=find_ent_by_type("worldspawn");
	me->arg1=ent2->x;
	me->arg2=ent2->z;
	
	if(enable_powerups) {
		//Cache the powerup resources
		if(sfx_speedup==NULL) sfx_speedup=new Sound("sound/speedup.wav");
		if(sfx_slowdown==NULL) sfx_speedup=new Sound("sound/slowdown.wav");
		if(sfx_hyper==NULL) sfx_speedup=new Sound("sound/hyper.wav");
		
		if(speedup_tex==NULL) speedup_tex = new Texture("tex/speedup.png",0);
		if(slowdown_tex==NULL) slowdown_tex = new Texture("tex/slowdown.png",0);
		if(hyper_tex==NULL) hyper_tex = new Texture("tex/hyper.png",0);
	}
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
