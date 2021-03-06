/* func_rotating.cc - DCBlap generic rotating object
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
#include "drawables/entity.h"
#include "objects.h"

void func_rotate_create(struct entity *me) {
  me->arg1=atoi(get_prop(me,"speed"));
}

void func_rotate_update(struct entity *me, float gt) {
  me->yrot+=me->arg1;
}

void func_rotate_message(struct entity *me, struct entity *them, char *message) {
  printf("Message \"%s\" recieved by a %s\n",message,them->type);
}
