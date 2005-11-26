/* world.cc - DCBlap world loader and renderer
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
#include <Tiki/tikimath.h>
#include <Tiki/texture.h>
#include <Tiki/file.h>

using namespace Tiki;
using namespace Tiki::GL;
using namespace Tiki::Math;

#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "camera.h"
#include "entity.h"
#include "callback.h"
#include "collide.h"
#include "word.h"
#include "utils.h"

#include <vector>

std::vector< RefPtr<Texture> > texture_list;

void free_world_textures() {
	while(!texture_list.empty()) {
		texture_list.pop_back();
	}
}


int ent_cnt=0;

struct entity *entity_list=NULL;
struct entity *light_list=NULL;
extern struct entity *wld;
float tint_r=0.0f;
float tint_g=0.0f;
float tint_b=0.0f;

void build_light_list() {
  struct entity *e;
  struct entity *p=NULL;

  e=entity_list;

  while(e!=NULL) {
    if(!strcmp(e->type,"light")) {
      if(p==NULL) {
        entity_list=e->next;
      } else {
        p->next=e->next;
      }
      e->next=light_list;
      light_list=e;
      if(p!=NULL) {
        e=p;
      } else {
        e=entity_list;
      }
    } else {
      p=e;
      e=e->next;
    }
  }
}

void compute_light(float px, float py, float pz, float trans) {
  struct entity *l;
  float x,y,z,dx,dy,dz,light;
  float r=0,g=0,b=0,r1,g1,b1;

	if(light_list==NULL) {
		r1=1;
		g1=1;
		b1=1;
	} else {
		r1=0;
		g1=0;
		b1=0;

		l=light_list;
		while(l!=NULL) {
			x=l->x;
			y=l->y;
			z=l->z;
			r=l->arg1/100.0f;
			g=l->arg2/100.0f;
			b=l->arg3/100.0f;
			dx=px-x;
			dy=py-y;
			dz=pz-z;
			light=1.6-(fsqrt(dx*dx+dy*dy+dz*dz)/10.0f)/(l->arg10);
			if(light>0) {
				r1+=r*light;
				g1+=g*light;
				b1+=b*light;
			}
			l=l->next;
		}
	}

	glColor4f(tint_r+r1,tint_g+g1,tint_b+b1,trans);
}

void destroy_param_list(struct param *p) {
  struct param *cur=p;
  struct param *prev=NULL;

  while(cur!=NULL) {
    prev=cur;
    cur=cur->next;
    free(prev->name);
    free(prev->value);
    free(prev);
  }
}

void destroy_poly_list(struct poly *p) {
  struct poly *cur=p;
  struct poly *prev=NULL;

  while(cur!=NULL) {
    prev=cur;
    cur=cur->next;
    free(prev);
  }
}

void destroy_world() {
  struct entity *ent=entity_list;
  struct entity *prev=NULL;

  while(ent!=NULL) {
    destroy_param_list(ent->param_list);
    destroy_poly_list(ent->poly_list);
    if(ent->model!=NULL) delete ent->model;
    if(ent->prog!=NULL) free(ent->prog);
    free(ent->type);
    
    prev=ent;
    ent=ent->next;
    free(prev);
  }
  free_world_textures();
  run_reset_callbacks();
  entity_list=NULL;
}

void broadcast_message(struct entity *from, char *message) {
  struct entity *current=entity_list;
  while(current!=NULL) {
    if(current->message) current->message(current,from,message);
    current=current->next;
  }
}

struct entity *create_new_entity(char *type, float x, float y, float z, float xrot, float yrot, float zrot, char *model, char *skin) {
  struct entity *cur_ent=NULL;
  cur_ent=new struct entity;
  char tmp[100];
  cur_ent->next=entity_list;
  entity_list=cur_ent;
  cur_ent->id=ent_cnt++;
	cur_ent->deleted=0;
	cur_ent->alpha=1.0f;
	cur_ent->framenum=0;
  cur_ent->paused=0;
  cur_ent->always_collide=0;
  cur_ent->xmin=x;
	cur_ent->xmax=x;
	cur_ent->ymin=y;
	cur_ent->ymax=y;
	cur_ent->zmin=z;
	cur_ent->zmax=z;
	cur_ent->prog=NULL;
  cur_ent->poly_list=NULL;
  cur_ent->param_list=NULL;
  cur_ent->model=NULL;
	cur_ent->tex=NULL;
  cur_ent->x=x;
  cur_ent->y=y;
  cur_ent->z=z;
  cur_ent->xrot=xrot;
  cur_ent->yrot=yrot;
  cur_ent->zrot=zrot;
  cur_ent->xvel=0;
  cur_ent->yvel=0;
  cur_ent->zvel=0;
  cur_ent->physics=0;
	cur_ent->arg1=0;
	cur_ent->arg2=0;
	cur_ent->arg3=0;
	cur_ent->arg4=0;
	cur_ent->arg5=0;
	cur_ent->arg6=0;
  cur_ent->type=(char *)malloc(strlen(type)+1);
  strcpy(cur_ent->type,type);

  if(model!=NULL) {
    set_prop(cur_ent,"model",model);
  }
  if(skin!=NULL) {
    set_prop(cur_ent,"skin",skin);
  }
  sprintf(tmp,"%f %f %f",x,y,z);
	set_prop(cur_ent,"origin",tmp);
  run_create_callback(cur_ent);
  cur_ent->update=get_update_callback(cur_ent);
  cur_ent->message=get_message_callback(cur_ent);
  return cur_ent;
}

int pol_total=0;
extern char tex_theme_dir[100];
char world_path[256];

void set_world_path(char *path) {
  strcpy(world_path,path);
}

void load_world_info(char *filename, char *title, char *author, char *rating) {
  unsigned int texcount;
  unsigned int entcount;
  unsigned int paramcount;
  unsigned int i, k;
  char name[100],val[100];
  char texname[100];
	File worldFile;

  strcpy(title,"No title");
  strcpy(author,"No author");
  strcpy(rating,"Not rated");

	assert(file_exists(filename));
	
  worldFile.open(filename,"rb");
	
	//Load textures
	worldFile.readle32(&texcount,1);
  for(i=0;i<texcount;i++) {
    k=0;
    do {
			worldFile.read(&texname[k],1);
      k++;
    } while(texname[k-1]!='\0');
  }
	
	//Load entities
	worldFile.readle32(&entcount,1);
		
	//Load type
	k=0;
	do {
		worldFile.read(&texname[k],1);
		k++;
	} while(texname[k-1]!='\0');
	
	//Load parameters
	worldFile.readle32(&paramcount,1);
		
  for(i=0;i<paramcount;i++) {
    k=0;
    do {
      worldFile.read(&name[k],sizeof(char));
	    k++;
    } while(name[k-1]!='\0');
    k=0;
    do {
	    worldFile.read(&val[k],sizeof(char));
	    k++;
    } while(val[k-1]!='\0');
    if(!strcmp(name,"title")) strcpy(title,val);
    if(!strcmp(name,"author")) strcpy(author,val);
    if(!strcmp(name,"rating")) strcpy(rating,val);
  }
  worldFile.close();
}

void load_world(const char *filename) {
  unsigned int i,k;
  float xmin=0,ymin=0,zmin=0;
  float xmax=0,ymax=0,zmax=0;
  float pmaxx=0,pmaxy=0,pmaxz=0;
  float pminx=0,pminy=0,pminz=0;
  float smax=0,tmax=0;
  float smin=0,tmin=0;  
  unsigned int texcount;
  char texname[100];
	char texname2[100];
  unsigned int polycount;
  unsigned int entcount;
  unsigned int paramcount;
  struct entity *cur_ent=entity_list;
  struct entity *prev_ent=NULL;
  struct param *cur_param=NULL;
  struct param *prev_param=NULL;
  struct poly *cur_poly=NULL;
  struct poly *prev_poly=NULL;
  unsigned int ent,pol,vert,tex_id;
	RefPtr<Texture> tex;
	File worldFile;
  
	assert(file_exists((char *)filename));
	printf("Loading: %s\n",filename);
	worldFile.open(filename,"rb");

	//Load textures
	worldFile.readle32(&texcount,1);
	printf("Textures: %i\n",texcount);
  for(i=0;i<texcount;i++) {
    k=0;
    do {
			worldFile.read(&texname[k],1);
			//Convert to lowercase
			if(texname[k] >= 'A' && texname[k] <= 'Z') texname[k] += 32;
      k++;
    } while(texname[k-1]!='\0');
		sprintf(texname2,"tex/%s.png",texname);
		tex=new Texture(texname2,0);
		texture_list.push_back(tex);
  }
	
	//Load entities
	worldFile.readle32(&entcount,1);
	printf("Entities: %i\n",entcount);
  for(ent=0;ent<entcount;ent++) {
    //printf("Loading entity %i...\n",ent);
    cur_ent=(struct entity *)malloc(sizeof(struct entity));
    if(prev_ent==NULL) {
      entity_list=cur_ent;
    } else {
      prev_ent->next=cur_ent;
    }
    cur_ent->poly_list=NULL;
    cur_ent->xmin=0;
    cur_ent->ymin=0;
    cur_ent->zmin=0;
    cur_ent->xmax=0;
    cur_ent->ymax=0;
    cur_ent->zmax=0;
		cur_ent->deleted=0;
    cur_ent->prog=NULL;
		
		//Load type
    k=0;
    do {
      worldFile.read(&texname[k],1);
      k++;
    } while(texname[k-1]!='\0');
    cur_ent->type=(char *)malloc(strlen(texname)+1);
    strcpy(cur_ent->type,texname);
    //printf("Type: %s\n",texname);
		
		//Load parameters
		worldFile.readle32(&paramcount,1);
    cur_ent->param_list=NULL;
    prev_param=NULL;
    for(i=0;i<paramcount;i++) {
      cur_param=(struct param *)malloc(sizeof(struct param));
      if(prev_param==NULL) {
				cur_ent->param_list=cur_param;
      } else {
				prev_param->next=cur_param;
      }
      k=0;
      do {
				worldFile.read(&texname[k],1);
				k++;
      } while(texname[k-1]!='\0');
      cur_param->name=(char *)malloc(strlen(texname)+1);
      strcpy(cur_param->name,texname);
      k=0;
      do {
				worldFile.read(&texname[k],1);
				k++;
      } while(texname[k-1]!='\0');
      cur_param->value=(char *)malloc(strlen(texname)+1);
      strcpy(cur_param->value,texname);
      cur_param->next=NULL;
      //printf("%s = %s\n",cur_param->name,cur_param->value);
      prev_param=cur_param;
      cur_param=cur_param->next;
    }
    if(get_prop(cur_ent,"origin")!=NULL) {
      make_coordinates(get_prop(cur_ent,"origin"),&cur_ent->x,&cur_ent->y,&cur_ent->z);
    } else {
      cur_ent->x=0;
      cur_ent->y=0;
      cur_ent->z=0;
    }
    if(get_prop(cur_ent,"angles")!=NULL) {
      make_coordinates(get_prop(cur_ent,"angles"),&cur_ent->xrot,&cur_ent->zrot,&cur_ent->yrot);
    } else {
      cur_ent->xrot=0;
      cur_ent->yrot=0;
      cur_ent->zrot=0;
    }
		
		//Load polygons
		worldFile.readle32(&polycount,1);
    prev_poly=NULL;
    for(pol=0;pol<polycount;pol++) {
      cur_poly=(struct poly *)malloc(sizeof(struct poly));
      if(cur_poly==NULL) { printf("Out of memory!\n"); return; }
      if(prev_poly==NULL) {
				cur_ent->poly_list=cur_poly;
      } else {
				prev_poly->next=cur_poly;
      }
			worldFile.readle32(&tex_id,1);
			cur_poly->tex=texture_list[tex_id];
			worldFile.readle32(&cur_poly->vert_count,1);

//printf("Polygon %i: texid: %i vert_count: %i\n",pol,cur_poly->tex_id,cur_poly->vert_count);
      for(vert=0;vert<cur_poly->vert_count;vert++) {
				worldFile.readle32(&cur_poly->point[vert].x,1);
				worldFile.readle32(&cur_poly->point[vert].y,1);
				worldFile.readle32(&cur_poly->point[vert].z,1);
				worldFile.readle32(&cur_poly->point[vert].s,1);
				worldFile.readle32(&cur_poly->point[vert].t,1);

	      if(pol==0&&vert==0) {
					xmax=cur_poly->point[vert].x;
					ymax=cur_poly->point[vert].y;
					zmax=cur_poly->point[vert].z;
          xmin=cur_poly->point[vert].x;
          ymin=cur_poly->point[vert].y;
          zmin=cur_poly->point[vert].z;
				} else {
					if(cur_poly->point[vert].x>xmax) xmax=cur_poly->point[vert].x;
					if(cur_poly->point[vert].y>ymax) ymax=cur_poly->point[vert].y;
					if(cur_poly->point[vert].z>zmax) zmax=cur_poly->point[vert].z;
					if(cur_poly->point[vert].x<xmin) xmin=cur_poly->point[vert].x;
					if(cur_poly->point[vert].y<ymin) ymin=cur_poly->point[vert].y;
					if(cur_poly->point[vert].z<zmin) zmin=cur_poly->point[vert].z;
				}  
			}
			
      cur_poly->next=NULL;
      prev_poly=cur_poly;
      cur_poly=cur_poly->next;
    }
    if(polycount==0) {
      //printf("PointClass detected\n");
      cur_ent->poly_list=NULL;
    } else {
      //printf("Adjusting SolidClass size\n");
      cur_ent->x=xmin+((xmax-xmin)/2);
      cur_ent->y=ymin+((ymax-ymin)/2);
      cur_ent->z=zmin+((zmax-zmin)/2);

      xmin-=cur_ent->x; xmax-=cur_ent->x;
      ymin-=cur_ent->y; ymax-=cur_ent->y;
      zmin-=cur_ent->z; zmax-=cur_ent->z;
      cur_poly=cur_ent->poly_list;
      while(cur_poly!=NULL) {
        //printf("Poly has %i verts\n",cur_poly->vert_count);
        for(vert=0;vert<cur_poly->vert_count;vert++) {
          cur_poly->point[vert].x-=cur_ent->x;
          cur_poly->point[vert].y-=cur_ent->y;
          cur_poly->point[vert].z-=cur_ent->z;
					if(vert==0) {
						pmaxx=cur_poly->point[vert].x;
						pmaxy=cur_poly->point[vert].y;
						pmaxz=cur_poly->point[vert].z;
						pminx=cur_poly->point[vert].x;
						pminy=cur_poly->point[vert].y;
						pminz=cur_poly->point[vert].z;
						smax=cur_poly->point[vert].s;
						tmax=cur_poly->point[vert].t;
						smin=cur_poly->point[vert].s;
						tmin=cur_poly->point[vert].t;
					} else {
						if(cur_poly->point[vert].x>pmaxx) pmaxx=cur_poly->point[vert].x;
						if(cur_poly->point[vert].y>pmaxy) pmaxy=cur_poly->point[vert].y;
						if(cur_poly->point[vert].z>pmaxz) pmaxz=cur_poly->point[vert].z;
						if(cur_poly->point[vert].s>smax) smax=cur_poly->point[vert].s;
						if(cur_poly->point[vert].t>tmax) tmax=cur_poly->point[vert].t;
						if(cur_poly->point[vert].x<pminx) pminx=cur_poly->point[vert].x;
						if(cur_poly->point[vert].y<pminy) pminy=cur_poly->point[vert].y;
						if(cur_poly->point[vert].z<pminz) pminz=cur_poly->point[vert].z;
						if(cur_poly->point[vert].s<smin) smin=cur_poly->point[vert].s;
						if(cur_poly->point[vert].t<tmin) tmin=cur_poly->point[vert].t;
					}  
				}
				cur_poly->cx=pminx+((pmaxx-pminx)/2.0f);
				cur_poly->cy=pminy+((pmaxy-pminy)/2.0f);
				cur_poly->cz=pminz+((pmaxz-pminz)/2.0f);
				cur_poly->cs=smin+(smax-smin)/2.0f;
				cur_poly->ct=tmin+(tmax-tmin)/2.0f;
        cur_poly=cur_poly->next;
      }
      //printf("Done!\n");
    }
    cur_ent->id=ent_cnt++;
    cur_ent->alpha=1.0f;
    cur_ent->blendcount=2;
    cur_ent->blendpos=0;
    cur_ent->framenum=1;
    cur_ent->model=NULL;
    cur_ent->xmax=xmax/100.0f;
    cur_ent->ymax=ymax/100.0f;
    cur_ent->zmax=zmax/100.0f;
    cur_ent->xmin=xmin/100.0f;
    cur_ent->ymin=ymin/100.0f;
    cur_ent->zmin=zmin/100.0f;
    cur_ent->xvel=0;
    cur_ent->yvel=0;
    cur_ent->zvel=0;
    cur_ent->paused=0;
    cur_ent->always_collide=0;
    cur_ent->physics=0;
    cur_ent->arg1=0;
    cur_ent->arg2=0;
    cur_ent->arg3=0;
    cur_ent->arg4=0;
    cur_ent->arg5=0;
    cur_ent->arg6=0;
    cur_ent->arg7=0;
    cur_ent->arg8=0;
    cur_ent->arg9=0;
    cur_ent->arg10=0;
    cur_ent->update=get_update_callback(cur_ent);
    cur_ent->message=get_message_callback(cur_ent);
    cur_ent->next=NULL;
    run_create_callback(cur_ent);
	  if(cur_ent->model!=NULL) {
		  cur_ent->model->bound_min(cur_ent->xmin,cur_ent->ymin,cur_ent->zmin);
		  cur_ent->model->bound_max(cur_ent->xmax,cur_ent->ymax,cur_ent->zmax);
		}
    prev_ent=cur_ent;
    cur_ent=cur_ent->next;
  }
  printf("World load complete.\n");
	worldFile.close();
	
	build_light_list();
}

void pause_world(bool mode) {
  struct entity *current=entity_list;

  while(current!=NULL) {
    current->paused=mode;
    current=current->next;
  }
}

char *get_prop(struct entity *ent, char *prop_name) {
  struct param *cur_prop=ent->param_list;
  while(cur_prop!=NULL) {
    if(!strcmp(cur_prop->name,prop_name)) {
      return cur_prop->value;
    }
    cur_prop=cur_prop->next;
  }
  return NULL;
}

void set_prop(struct entity *ent, char *prop_name, char *value) {
  struct param *cur_prop=ent->param_list;
  struct param *prev_prop=NULL;

  while(cur_prop!=NULL) {
    if(!strcmp(cur_prop->name,prop_name)) {
      free(cur_prop->value);
      cur_prop->value=(char *)malloc(strlen(value)+1);
      strcpy(cur_prop->value,value);
      return;
    }
    prev_prop=cur_prop;
    cur_prop=cur_prop->next;
  }

  cur_prop=new struct param;
  if(prev_prop!=NULL) {
    prev_prop->next=cur_prop;
  } else {
    ent->param_list=cur_prop;
  }
  cur_prop->next=NULL;

  cur_prop->name=(char *)malloc(strlen(prop_name)+1);
  strcpy(cur_prop->name,prop_name);

  cur_prop->value=(char *)malloc(strlen(value)+1);
  strcpy(cur_prop->value,value);
}

int ent_count(char *type) {
  struct entity *cur_ent=entity_list;
  int cnt=0;
  while(cur_ent!=NULL) {
    if(!strcmp(cur_ent->type,type)) {
      cnt++;
    }
    cur_ent=cur_ent->next;
  }
  return cnt;
}  

struct entity *find_ent_by_type(char *type) {
  struct entity *cur_ent=entity_list;
  while(cur_ent!=NULL) {
    if(!strcmp(cur_ent->type,type)) {
      return cur_ent;
    }
    cur_ent=cur_ent->next;
  }
  printf("find_ent_by_type: %s not found.\n",type);
  return NULL;
}

struct entity *find_ent_by_prop(char *type, char *propname, char *value) {
  struct entity *cur_ent=entity_list;
  while(cur_ent!=NULL) {
    if(!strcmp(cur_ent->type,type) && !strcmp(get_prop(cur_ent,propname),value)) {
      return cur_ent;
    }
    cur_ent=cur_ent->next;
  }
  printf("find_ent_by_prop: %s.%s=%s not found\n",type,propname,value);
  return NULL;
}

struct entity *get_ent(int count) {
  struct entity *cur_ent=entity_list;
  int i;
  for(i=0;i<count;i++) {
    cur_ent=cur_ent->next;
    if(cur_ent==NULL) { printf("Invalid entity number (%i)\n",count); return NULL; }
  }
  return cur_ent;
}

void make_coordinates(char *text,float *x, float *y, float *z) {
  int i,j;
  char tmp[100];
  j=0; i=0;
  while(text[j]!=' ') {
    tmp[i]=text[j];
    i++; j++;
  }
  tmp[i]='\0';
  *x=atoi(tmp);
  
  i=0; j++;
  while(text[j]!=' ') {
    tmp[i]=text[j];
    i++; j++;
  }
  tmp[i]='\0';
  *z=atoi(tmp);
  
  i=0; j++;
  while(text[j]!='\0') {
    tmp[i]=text[j];
    i++; j++;
  }
  tmp[i]='\0';
  *y=atoi(tmp);
}

void update_world(float gt) {
  struct entity *cur_ent=entity_list;
  struct entity *ent2;
	float ox,oy,oz;
  //printf("++Begin update cycle\n");

  while(cur_ent!=NULL) {
		if(cur_ent->next && cur_ent->next->deleted) {
			//printf("Deleting ent->next\n");
			ent2=cur_ent->next;
			cur_ent->next=cur_ent->next->next;

			destroy_param_list(ent2->param_list);
			destroy_poly_list(ent2->poly_list);
			free(ent2->type);
			if(ent2->prog) free(ent2->prog);
      if(ent2->model!=NULL) delete ent2->model;
			free(ent2);
			ent2=NULL;
		}
		if(cur_ent==entity_list && cur_ent->deleted) {
			//printf("Deleting ent\n");
			ent2=cur_ent;
			entity_list=cur_ent->next;
			destroy_param_list(ent2->param_list);
			destroy_poly_list(ent2->poly_list);
			free(ent2->type);
			if(ent2->prog) free(ent2->prog);
      if(ent2->model!=NULL) delete ent2->model;
			free(ent2);
			ent2=NULL;
			cur_ent=entity_list;
			continue;
		}
		ox=cur_ent->x;
		oy=cur_ent->y;
		oz=cur_ent->z;
		//printf("Running update callback for %s\n",cur_ent->type);
    if(cur_ent->update) cur_ent->update(cur_ent, gt);
		if(ox!=cur_ent->x || oy!=cur_ent->y || oz!=cur_ent->z) {
			if(cur_ent->model!=NULL) {
				cur_ent->model->bound_min(cur_ent->xmin,cur_ent->ymin,cur_ent->zmin);
			  cur_ent->model->bound_max(cur_ent->xmax,cur_ent->ymax,cur_ent->zmax);
			}
			do_collide(cur_ent);
		}

    //bounds checking
    if(cur_ent->xmin+cur_ent->x/100.0f>wld->xmax+wld->x/100.0f||cur_ent->xmax+cur_ent->x/100.0f<wld->xmin+wld->x/100.0f) {
      send_message(cur_ent,wld,"out");
    }
    if(cur_ent->zmin+cur_ent->z/100.0f>wld->zmax+wld->z/100.0f||cur_ent->zmax+cur_ent->z/100.0f<wld->zmin+wld->z/100.0f) {
      send_message(cur_ent,wld,"out");
    }
    cur_ent=cur_ent->next;
  }
  
  cur_ent=light_list;
  while(cur_ent!=NULL) {
    cur_ent->update(cur_ent, gt);
    cur_ent=cur_ent->next;
  }
  
  //printf("--End update cycle\n");	
}

void render_world(bool solid) {
  unsigned int i,j,frame1,frame2;
  struct entity *cur_ent=entity_list;
  struct poly *cur_pol=NULL;

	//printf("+++render_world()\n");
  
  while(cur_ent!=NULL) {
    if(cur_ent->model==NULL && cur_ent->poly_list!=NULL&&( (solid && cur_ent->alpha==1.0f) || (!solid && cur_ent->alpha<1.0f) )) {
      glLoadIdentity();
      glRotatef(camera_xrot,1.0f,0.0f,0.0f);
      glRotatef(camera_zrot,0.0f,0.0f,1.0f);
      glRotatef(camera_yrot,0.0f,1.0f,0.0f);
      glTranslatef(-camera_x/100.0f,-camera_y/100.0f,camera_z/100.0f);
      glTranslatef(cur_ent->x/100.0f,cur_ent->y/100.0f,-cur_ent->z/100.0f);
			glRotatef(cur_ent->xrot,1.0f,0.0f,0.0f);
			glRotatef(cur_ent->yrot,0.0f,1.0f,0.0f);
			glRotatef(cur_ent->zrot,0.0f,0.0f,1.0f);
      cur_pol=cur_ent->poly_list;
      i=0;
      while(cur_pol!=NULL) {
        i++;
        cur_pol->tex->select();
				//glDisable(GL_TEXTURE_2D);
        switch(cur_pol->vert_count) {
					case 3:
						glBegin(GL_TRIANGLES);
						break;
					case 4:
						glBegin(GL_QUADS);
						break;
					default:
						glBegin(GL_POLYGON);
						break;
				}

				//glBegin(GL_TRIANGLE_FAN);
				//glBegin(GL_LINE_LOOP);

				//compute_light(cur_ent->x+cur_pol->cx,cur_ent->y+cur_pol->cy,cur_ent->z+cur_pol->cz,cur_ent->alpha);
				//glTexCoord2f(cur_pol->cs,cur_pol->ct);
				//glVertex3f(cur_pol->cx/100.0f, cur_pol->cy/100.0f, -cur_pol->cz/100.0f);
				
				for(j=0;j<cur_pol->vert_count;j++) {
					compute_light(cur_ent->x+(cur_pol->point[j].x),cur_ent->y+(cur_pol->point[j].y),cur_ent->z+(cur_pol->point[j].z),cur_ent->alpha);
					glTexCoord2f(cur_pol->point[j].s,cur_pol->point[j].t);
					glVertex3f(cur_pol->point[j].x/100.0f, cur_pol->point[j].y/100.0f, -cur_pol->point[j].z/100.0f);
				}
				
				//compute_light(cur_ent->x+(cur_pol->point[0].x),cur_ent->y+(cur_pol->point[0].y),cur_ent->z+(cur_pol->point[0].z),cur_ent->alpha);
				//glTexCoord2f(cur_pol->point[0].s,cur_pol->point[0].t);
				//glVertex3f(cur_pol->point[0].x/100.0f, cur_pol->point[0].y/100.0f, -cur_pol->point[0].z/100.0f);

				glEnd();
        cur_pol=cur_pol->next;
      }
    } else if(cur_ent->model!=NULL&&( (solid && cur_ent->alpha==1.0f) || (!solid && cur_ent->alpha<1.0f) )) {
			if(cur_ent->framenum<cur_ent->anim_start) {
				cur_ent->framenum=cur_ent->anim_start;
			}
			frame1=cur_ent->framenum;
			if(cur_ent->framenum==cur_ent->anim_end) {
				frame2=cur_ent->anim_start;
			} else {
				frame2=cur_ent->framenum+1;
			}
			if(frame2>cur_ent->anim_end) { frame2=cur_ent->anim_end; }

			glLoadIdentity();
			glRotatef(camera_xrot,1.0f,0.0f,0.0f);
			glRotatef(camera_zrot,0.0f,0.0f,1.0f);
			glRotatef(camera_yrot,0.0f,1.0f,0.0f);
			glTranslatef(-camera_x/100.0f,-camera_y/100.0f,camera_z/100.0f);
			glTranslatef(cur_ent->x/100.0f,cur_ent->y/100.0f,-cur_ent->z/100.0f);
			glTranslatef(0,.16,0);
			glRotatef(cur_ent->xrot,1.0f,0.0f,0.0f);
			glRotatef(cur_ent->zrot,0.0f,0.0f,1.0f);
			glRotatef(cur_ent->yrot,0.0f,1.0f,0.0f);
			glRotatef(-90,1.0f,0.0f,0.0f);
			cur_ent->tex->select();
			if(cur_ent->anim_start!=cur_ent->anim_end)
				cur_ent->model->SetFrame(frame1,frame2,cur_ent->blendpos,cur_ent->blendcount);
			compute_light(cur_ent->x,cur_ent->y,cur_ent->z,cur_ent->alpha);
			cur_ent->model->Draw();
			cur_ent->blendpos++;
			if(cur_ent->blendpos>cur_ent->blendcount) {
				cur_ent->blendpos=0;
				cur_ent->framenum++;
			}

			if(cur_ent->framenum>cur_ent->anim_end) { cur_ent->framenum=cur_ent->anim_start; }
    }
    cur_ent=cur_ent->next;
  }
	//printf("--render_world()\n");
}