/* func_breakable.cc - DCBlap generic breakable object
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
#include "hud.h"

extern bool enable_powerups;
extern bool win_flag;
extern struct entity *wld;

void func_break_create(struct entity *me) {
  me->arg1=atoi(get_prop(me,"hits"));
  me->arg2=atoi(get_prop(me,"points"));
  if(get_prop(me,"intro_effect")!=NULL) {
    me->arg3=atoi(get_prop(me,"intro_effect"));
    me->arg4=atoi(get_prop(me,"intro_speed"));
  }
  if(me->arg4>50) me->arg4=50;
  me->arg5=me->y;
}

extern struct entity *g1;
extern struct entity *g2;

void func_break_update(struct entity *me, float gt) {
	struct entity *ent2=0;
  char txt[100];
  int num;
  if(me->arg4>0) {
    switch((int)me->arg3) {
    case 1: //fade in
      me->alpha=(50.0f-(float)me->arg4)/50.0f * gt * 40;
      me->arg4-= gt * 40;
      break;
    case 2: //drop from sky
      me->y=me->arg5+(me->arg4*10) * gt * 40;
      me->arg4-= gt * 40;
    }
  } else {
    me->alpha=1.0f;
  }
  if(me->arg1<1) {
    me->deleted=1;
    if(ent_count("func_breakable")<=1) {
      if(!strcmp(get_prop(wld,"next_map"),"none")) {
        if(ent_count("goal")>1) {
          if(g1->arg1>g2->arg1) {
            num=1;
          } else {
            num=2;
          }
          sprintf(txt,"Player %i wins!",num);
          me->arg2=num;
        } else {
          sprintf(txt,"You win!");
          me->arg2=0;
          ent2=find_ent_by_prop("paddle","num","1");
          ent2->arg7=50;
          ent2->arg5=10;
          ent2->arg9=1;
        }
        set_hud(0,320-(txt_width(txt)/2),240-16,txt,1,1,1);
        set_hud(2,320-(txt_width("Press Start")/2),244,"Press Start",1,1,1);
        broadcast_message(me,"win");
      } else {
        win_flag=1;
      }
    }
	  if(enable_powerups && rand()%5==1) {
		  switch(rand()%5) {
			  case 0:
			  case 3:
				  ent2=create_new_entity("speedup",me->x,me->y,me->z,0,0,0,NULL,NULL);
				  break;
			  case 1:
			  case 4:
				  ent2=create_new_entity("slowdown",me->x,me->y,me->z,0,0,0,NULL,NULL);
				  break;
			  case 2:
				  ent2=create_new_entity("hyper",me->x,me->y,me->z,0,0,0,NULL,NULL);
				  break;
		  }
	  }
  }
}

void func_break_message(struct entity *me, struct entity *them, char *message) {
  //char tmp[10];
  //struct entity *ent2;
  //printf("Message %s recieved by a %s\n",message,them->type);
  if(!strcmp("thud",message)&&!strcmp("ball",them->type)) {
    me->arg1--;
    switch((int)them->arg7) {
    case 1:
      g1->arg1++;
      break;
    case 2:
      g2->arg1++;
    }
  }
}
