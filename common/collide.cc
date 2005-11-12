/* collide.cc - collision support functions
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

#include "entity.h"
#include "sg.h"

void do_collide(struct entity *ent1) {
  struct entity *ent2=get_ent(0);
  sgBox b1,b2;
  
  if(!strcmp(ent1->type,"worldspawn")) return;
  b1.setMin(ent1->xmin+(ent1->x/100.0f),ent1->ymin+(ent1->y/100.0f),ent1->zmin+(ent1->z/100.0f));
  b1.setMax(ent1->xmax+(ent1->x/100.0f),ent1->ymax+(ent1->y/100.0f),ent1->zmax+(ent1->z/100.0f));

  while(ent2!=NULL) {
    b2.setMin(ent2->xmin+(ent2->x/100.0f),ent2->ymin+(ent2->y/100.0f),ent2->zmin+(ent2->z/100.0f));
    b2.setMax(ent2->xmax+(ent2->x/100.0f),ent2->ymax+(ent2->y/100.0f),ent2->zmax+(ent2->z/100.0f));
    //if((ent1 != ent2) && b2.intersects(&b1)) printf("Intersection..\n");
    if((ent1 != ent2) && (!strcmp(ent2->type,"worldspawn")==0) && (!strcmp(ent2->type,"glass")==0) && (b2.intersects(&b1)||b1.intersects(&b2))) {
      //printf("Collision between %s and %s (sending thud)\n",ent1->type,ent2->type);
      if(ent2->message!=NULL) ent2->message(ent2,ent1,"thud");
      if(ent1->message!=NULL) ent1->message(ent1,ent2,"thud");
      //printf("message complete!\n");
    }
    ent2=ent2->next;
  }
}
