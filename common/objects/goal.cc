/* goal.cc - DCBlap goal object
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
#include <Tiki/hid.h>
#include <Tiki/tikimath.h>
#include <Tiki/texture.h>

using namespace Tiki;
using namespace Tiki::Math;
using namespace Tiki::GL;
using namespace Tiki::Hid;

#include "objects.h"
#include "hud.h"

TIKI_OBJECT_NAME(Goal);
TIKI_OBJECT_BEGIN(Object, Goal)
TIKI_OBJECT_RECEIVER("thud", Goal::thud)
TIKI_OBJECT_END(Goal)

Goal *goals[4];

Goal::Goal() {
	m_num=1;
	m_score=0;
}

int Goal::thud(Entity * sender, Object * arg) {
	return 0;
}

void Goal::setProperty(std::string name, std::string value) {
	SolidClass::setProperty(name,value);
	if(name=="num") {
		m_num=atoi(value.c_str());
		goals[m_num-1]=this;
	}
}

void Goal::nextFrame(uint64 tm) {
	char txt[100];
	
  sprintf(txt,"Player %i: %i",m_num,m_score);
  switch(m_num) {
		case 1:
			set_hud(0,30,30,txt,1,1,1);
			break;
		case 2:
			set_hud(1,440,30,txt,1,1,1);
			break;
  }
}

#if 0
#include <Tiki/tiki.h>
#include <Tiki/texture.h>

using namespace Tiki;
using namespace Tiki::GL;

#include <stdlib.h>
#include <string.h>
#include "drawables/entity.h"
#include "callback.h"
#include "objects.h"
#include "camera.h"
#include "hud.h"

extern struct entity *wld;
extern bool win_flag;

void goal_create(struct entity *me) {
  me->arg2=atoi(get_prop(me,"num"));
  me->arg3=100;
}

void goal_callback(struct entity *me, float gt) {
  char txt[100];

  if(me->paused==0&&wld->arg3>0&&me->arg1>=wld->arg3) {
    broadcast_message(me,"win");
    sprintf(txt,"Player %0.f wins!",me->arg2);
    set_hud(0,320-(txt_width(txt)/2),240-16,txt,1,1,1);
    set_hud(1,320-(txt_width("Press Start")/2),244,"Press Start",1,1,1);
		win_flag=1;
  }
  if(me->paused==1) return;
  sprintf(txt,"Player %0.f: %0.f",me->arg2,me->arg1);
  switch((int)me->arg2) {
  case 1:
    set_hud(0,30,30,txt,1,1,1);
    break;
  case 2:
    set_hud(1,440,30,txt,1,1,1);
    break;
  }
	if(wld->arg2>0) {
		sprintf(txt,"Balls: %i",(int)wld->arg1);
		set_hud(1,440,30,txt,1,(float)wld->arg1/((float)wld->arg2/1.2f),(float)wld->arg1/(float)wld->arg2);
	}
  /*sprintf(txt,"Health: %f%%",me->arg3);
  switch((int)me->arg2) {
  case 1:
    set_hud(5,30,55,txt,1,1,1);
    break;
  case 2:
    set_hud(6,440,55,txt,1,1,1);
    break;
  }*/
}

void goal_message(struct entity *me, struct entity *them, char *message) {
  if(!strcmp(message,"win")) {
    pause_world(1);
  }
}
#endif