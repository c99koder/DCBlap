/* light.cc - DCBlap light object
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

#include <stdlib.h>
#include "entity.h"

void light_create(struct entity *me) {
  if(get_prop(me,"red")!=NULL) me->arg1=atoi(get_prop(me,"red"));
  if(get_prop(me,"green")!=NULL) me->arg2=atoi(get_prop(me,"green"));
  if(get_prop(me,"blue")!=NULL) me->arg3=atoi(get_prop(me,"blue"));
  if(get_prop(me,"radius")!=NULL) me->arg10=atoi(get_prop(me,"radius"));
  if(get_prop(me,"effect")!=NULL) {
    me->arg4=atoi(get_prop(me,"effect"));
    me->arg5=100;
    me->arg6=1;
    me->arg7=me->arg1;
    me->arg8=me->arg2;
    me->arg9=me->arg3;
  }
}

void light_update(struct entity *me, float gt) {
  switch((int)me->arg4) {
  case 0:
    break;
  case 1: //pulse
    if(me->arg5>=100) {
      me->arg6=-1;
    } else if (me->arg5<=0) { 
      me->arg6=1;
    }
    me->arg5+=(me->arg6 * gt);
    me->arg1=me->arg7*(float(me->arg5)/100.0f);
    me->arg2=me->arg8*(float(me->arg5)/100.0f);
    me->arg3=me->arg9*(float(me->arg5)/100.0f);
    break;
  case 2: //flicker
    if(me->arg5>=100) {
      me->arg5=0;
    }
    me->arg5+=me->arg6;
    if(me->arg5>10) {
      me->arg1=me->arg7;
      me->arg2=me->arg8;
      me->arg3=me->arg9;
    } else {
      me->arg1=0;
      me->arg2=0;
      me->arg3=0;
    }
    break;
  }
}
