/* water.cc - DCBlap water object
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
#include <Tiki/tiki.h>
#include <Tiki/tikimath.h>
#include <Tiki/texture.h>

using namespace Tiki;
using namespace Tiki::GL;
using namespace Tiki::Math;

#include "drawables/entity.h"

void glass_create(struct entity *me) {
  struct poly *p,*prev=NULL,*old;
  int i;
  float x,z,xstep,zstep;

  if(get_prop(me,"transf")) {
    me->alpha=(float)atoi(get_prop(me,"transf"))/255.0f;
  } else {
    me->alpha=0.7f;
  }
}

void water_create(struct entity *me) {
  struct poly *p,*prev=NULL,*old;
  Texture *tex;
  float x,z,xstep,zstep,s=0,t=0;

  if(get_prop(me,"transf")) {
    me->alpha=(float)atoi(get_prop(me,"transf"))/255.0f;
  } else {
    me->alpha=0.7f;
  }

  tex=me->poly_list->tex;

  //Delete polygons
  p=me->poly_list;
  while(p!=NULL) {
    old=p;
    p=p->next;
    free(old);
  }

  me->poly_list=NULL;

  //Create 16 new polygons in a 4x4 grid
  zstep=((me->zmax-me->zmin)/4.0f)*100;
  xstep=((me->xmax-me->xmin)/4.0f)*100;
  printf("xstep: %f\nzstep: %f\n",xstep,zstep);
  for(z=me->zmin*100;z<me->zmax*100;z+=zstep) {
    s=0;
    for(x=me->xmin*100;x<me->xmax*100;x+=xstep) {
      p=new poly;
      p->tex=tex;
      p->vert_count=4;
      p->point[0].x=x;
      p->point[0].y=me->ymax*100;
      p->point[0].z=z;
      p->point[0].s=s;
      p->point[0].t=t;
      p->point[1].x=x+xstep;
      p->point[1].y=me->ymax*100;
      p->point[1].z=z;
      p->point[1].s=s+0.25;
      p->point[1].t=t;
      p->point[2].x=x+xstep;
      p->point[2].y=me->ymax*100;
      p->point[2].z=z+zstep;
      p->point[2].s=s+0.25;
      p->point[2].t=t+0.25;
      p->point[3].x=x;
      p->point[3].y=me->ymax*100;
      p->point[3].z=z+zstep;
      p->point[3].s=s;
      p->point[3].t=t+0.25;
      p->next=me->poly_list;
      me->poly_list=p;
      s+=0.25;
    }
    t+=0.25;
  }
}

extern struct entity *player;
extern float tint_r,tint_g,tint_b;

void water_update(struct entity *me) {
  struct poly *p=me->poly_list;
  int i;
/*  sgBox b1;
  sgSphere cam;

  cam.setCenter(player->x/100.0f, (player->y+58)/100.0f, player->z/100.0f);
  cam.setRadius(0.1);

  b1.setMin((me->x/100.0f)+me->xmin,(me->y/100.0f)+me->ymin,(me->z/100.0f)+me->zmin);
  b1.setMax((me->x/100.0f)+me->xmax,(me->y/100.0f)+me->ymax,(me->z/100.0f)+me->zmax);

  if(cam.intersects(&b1)) {
    tint_r=-0.2;
    tint_g=0.1;
    tint_b=0.1;
  } else {
    tint_r=0.0;
    tint_g=0.0;
    tint_b=0.0;
  }
*/
  while(p!=NULL) {
    for(i=0;i<p->vert_count;i++) {
      p->point[i].y+=(fsin(me->arg1+p->point[i].x) + fsin(me->arg1+p->point[i].z))/10;
    }
    p=p->next;
  }
  me->arg1+=0.1;
  if(me->arg1>24.0f*M_PI) me->arg1=0;
}
