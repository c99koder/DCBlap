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
#ifdef WIN32
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <malloc.h>
#include <stdio.h>
#include <kos_w32.h>
#include <time.h>
#endif
#ifdef UNIX
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <kos_w32.h>
#endif
#ifdef DREAMCAST
#include <kos.h>
#endif
#ifdef LINUX
#include <SDL/SDL.h>
#endif
#ifdef TIKI
#include <Tiki/tiki.h>
#include <Tiki/texture.h>

using namespace Tiki;
using namespace Tiki::GL;
#endif
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "camera.h"
#include "entity.h"
#include "callback.h"
#include "collide.h"
#include "word.h"
#include "utils.h"

#include <algorithm> //required for std::swap
#include <vector>

#define ByteSwap5(x) ByteSwap((unsigned char *) &x,sizeof(x))

inline void ByteSwap(unsigned char * b, int n)
{
#ifdef MACOS
#ifdef __i386__
#warning X86 not swapping
#else
	register int i = 0;
	register int j = n-1;
	while (i<j)
	{
		std::swap(b[i], b[j]);
		i++, j--;
	}
#endif
#endif
}

//std::vector<Texture> texture_list;
Texture *texture_list[20];

void free_world_textures() {
/*	std::vector<Texture>::iterator tex_iter;
	for(tex_iter = texture_list.begin(); tex_iter != texture_list.end(); tex_iter++) {
		(*tex_iter).destroy();
		tex_iter=texture_list.erase(tex_iter);
	}*/
}


void draw_md2(char *filename, int texid, float mx, float my, float mz, float rx, float ry, float rz, int frame1, int frame2, int blendpos, int blendcount);
int ent_cnt=0;

struct entity *entity_list=NULL;

void destroy_param_list(struct param *p) {
  struct param *cur=p;
  struct param *prev=NULL;
  //printf("==begin param free==\n");
  while(cur!=NULL) {
    prev=cur;
    cur=cur->next;
    //printf("Freeing: %s = %s\n",prev->name,prev->value);
    free(prev->name);
    free(prev->value);
    free(prev);
  }
  //printf("--end param free--\n");
}

void destroy_poly_list(struct poly *p) {
  struct poly *cur=p;
  struct poly *prev=NULL;

  //printf("==begin poly free==\n");
  while(cur!=NULL) {
    prev=cur;
    cur=cur->next;
    free(prev);
  }
  //printf("--end poly free--\n");
}

void destroy_world() {
  struct entity *ent=entity_list;
  struct entity *prev=NULL;

  //printf("==begin world free==\n");

  while(ent!=NULL) {
    //printf("Destroying a %s\n",ent->type);
    destroy_param_list(ent->param_list);
    destroy_poly_list(ent->poly_list);
    //printf("Free model..\n");
    if(ent->model!=NULL) delete ent->model;
    //printf("Free program..\n");
    if(ent->prog!=NULL) free(ent->prog);
    free(ent->type);
    
    prev=ent;
    ent=ent->next;
    //printf("Free ent..\n");
    free(prev);
  }
  free_world_textures();
  run_reset_callbacks();
  entity_list=NULL;
  //printf("--end world free--\n");
}

void broadcast_message(struct entity *from, char *message) {
  struct entity *current=entity_list;
  while(current!=NULL) {
    //send_message(current,from,message);
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


void load_world(char *filename) {
  int i,k,fd;
  float tmp;
  float xmin=0,ymin=0,zmin=0;
  float xmax=0,ymax=0,zmax=0;
  unsigned int texcount;
  char texname[100];
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
	Texture *tex;
  
  //fd=open(filename,O_RDONLY);
	//if(fd==-1) {
		sprintf(texname,"maps/%s",filename);
		printf("Loading: %s\n",texname);
		fd=open(texname,O_RDONLY);
	//}
  fs_read(fd,&texcount,sizeof(unsigned int));
		ByteSwap5(texcount);
		printf("Textures: %i\n",texcount);

  for(i=0;i<texcount;i++) {
    k=0;
    do {
      read(fd,&texname[k],sizeof(char));
			if(texname[k] >= 'A' && texname[k] <= 'Z') texname[k] += 32;
      k++;
    } while(texname[k-1]!='\0');
    //load_texture(texname,i);
		strcat(texname,".png");
		tex=new Texture;
		tex->loadFromFile(texname,0);
		texture_list[i]=tex;
  }
  fs_read(fd,&entcount,sizeof(unsigned int));
	ByteSwap5(entcount);
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
    k=0;
    do {
      fs_read(fd,&texname[k],sizeof(char));
      k++;
    } while(texname[k-1]!='\0');
    cur_ent->type=(char *)malloc(strlen(texname)+1);
    strcpy(cur_ent->type,texname);
    //printf("Type: %s\n",texname);
    fs_read(fd,&paramcount,sizeof(unsigned int));
		ByteSwap5(paramcount);
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
	fs_read(fd,&texname[k],sizeof(char));
	k++;
      } while(texname[k-1]!='\0');
      cur_param->name=(char *)malloc(strlen(texname)+1);
      strcpy(cur_param->name,texname);
      k=0;
      do {
	fs_read(fd,&texname[k],sizeof(char));
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
    fs_read(fd,&polycount,sizeof(unsigned int));
		ByteSwap5(polycount);
    prev_poly=NULL;
    for(pol=0;pol<polycount;pol++) {
      cur_poly=(struct poly *)malloc(sizeof(struct poly));
      if(cur_poly==NULL) { printf("Out of memory!\n"); return; }
      if(prev_poly==NULL) {
	cur_ent->poly_list=cur_poly;
      } else {
	prev_poly->next=cur_poly;
      }
      fs_read(fd,&tex_id,sizeof(unsigned int));
			ByteSwap5(tex_id);
			cur_poly->tex=texture_list[tex_id];
      fs_read(fd,&cur_poly->vert_count,sizeof(unsigned int));
			ByteSwap5(cur_poly->vert_count);
//printf("Polygon %i: texid: %i vert_count: %i\n",pol,cur_poly->tex_id,cur_poly->vert_count);
      for(vert=0;vert<cur_poly->vert_count;vert++) {
	fs_read(fd,(char *)&cur_poly->point[vert].x,sizeof(float));
	ByteSwap5(cur_poly->point[vert].x);
	fs_read(fd,(char *)&cur_poly->point[vert].y,sizeof(float));
	ByteSwap5(cur_poly->point[vert].y);
	fs_read(fd,(char *)&cur_poly->point[vert].z,sizeof(float));
	ByteSwap5(cur_poly->point[vert].z);
	fs_read(fd,(char *)&cur_poly->point[vert].s,sizeof(float));
	ByteSwap5(cur_poly->point[vert].s);
	fs_read(fd,(char *)&cur_poly->point[vert].t,sizeof(float));
	ByteSwap5(cur_poly->point[vert].t);
	cur_poly->point[vert].x-=cur_ent->x;
	cur_poly->point[vert].y-=cur_ent->y;
	cur_poly->point[vert].z-=cur_ent->z;
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
        }
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
  fs_close(fd);
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

#ifdef WIN32
extern SOCKET net_socket;
extern bool net_host;
#else
extern int net_socket;
extern bool net_host;
#endif
extern int avg_cycle_len;
extern int cycle_len;

void update_net() {
#ifdef WIN32
  char buf[100];
  struct entity *ent2;
  if(net_socket!=-1) {
    socket_read_line(net_socket,buf);
    wordify(buf,',');
    switch(atoi(get_word(0))) {
    case 1: //move paddle step x
      ent2=get_ent(atoi(get_word(1)));
      ent2->z+=atoi(get_word(2));
      break;
   /* case 2:
      if(atoi(get_word(1))>avg_cycle_len) {
        avg_cycle_len=atoi(get_word(1));
        cycle_len=atoi(get_word(1));
      }
      if(cycle_len<40) {
        cycle_len=40;
        avg_cycle_len=40;
      }
      break;*/
    case 3:
      ent2=get_ent(atoi(get_word(1)));
      ent2->x=atof(get_word(2));
      ent2->y=atof(get_word(3));
      ent2->z=atof(get_word(4));
      ent2->arg1=atoi(get_word(5));
      ent2->arg2=atoi(get_word(6));
      ent2->arg3=atoi(get_word(7));
      ent2->arg4=atoi(get_word(8));
      ent2->arg5=atoi(get_word(9));
      ent2->arg6=atoi(get_word(10));
      ent2->arg7=atoi(get_word(11));
      break;
    }
  }
#endif
}

void update_world(float gt) {
  struct entity *cur_ent=entity_list;
  struct entity *ent2;
  char buf[100];
	float ox,oy,oz;
  static int cnt=1;
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
			printf("Deleting ent\n");
			ent2=cur_ent;
			entity_list=cur_ent->next;
			destroy_param_list(ent2->param_list);
			destroy_poly_list(ent2->poly_list);
			free(ent2->type);
			if(ent2->prog) free(ent2->prog);
      if(ent2->model!=NULL) delete ent2->model;
			if(ent2->tex!=NULL) delete ent2->tex;
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
    //if(net_socket==-1||net_host==1) {
		if(ox!=cur_ent->x || oy!=cur_ent->y || oz!=cur_ent->z) {
			//printf("Doing collision tests..\n");
			if(cur_ent->model!=NULL) {
				cur_ent->model->bound_min(cur_ent->xmin,cur_ent->ymin,cur_ent->zmin);
			  cur_ent->model->bound_max(cur_ent->xmax,cur_ent->ymax,cur_ent->zmax);
			}
			do_collide(cur_ent);
		}
    cur_ent=cur_ent->next;
  }
  //printf("--End update cycle\n");	
}

void render_world_solid() {
  int i,j,frame1,frame2;
  float x,y,z,light,dz,dy,dx;
  struct entity *cur_ent=entity_list;
  struct entity *other_ent=NULL;
  struct poly *cur_pol=NULL;

	//printf("+++render_world()\n");
  
  //glPushMatrix();

  while(cur_ent!=NULL) {
    if(cur_ent->model==NULL && cur_ent->poly_list!=NULL&&cur_ent->alpha==1.0f) {
			//printf("Rendering a brush (%s)...\n",cur_ent->type);
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

				for(j=0;j<cur_pol->vert_count;j++) {
			#ifndef WIN32
					glTexCoord2f(cur_pol->point[j].s,cur_pol->point[j].t);
			#else
					glTexCoord2f(cur_pol->point[j].s,cur_pol->point[j].t*-1);
			#endif
					glVertex3f(cur_pol->point[j].x/100.0f, cur_pol->point[j].y/100.0f, -cur_pol->point[j].z/100.0f);
				}
				glEnd();
        cur_pol=cur_pol->next;
      }
    } else {
      if(cur_ent->model!=NULL&&cur_ent->alpha==1) {
	//printf("Drawing an md2 model (%s)...\n",cur_ent->type); 
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
	      //printf("model name: %s, tex: %i, x: %f, y: %f, z: %f frame1: %i frame2: %i\n",cur_ent->model->filename,cur_ent->tex_id,cur_ent->x,cur_ent->y,cur_ent->z,frame1,frame2);
        //draw_md2(cur_ent->model->filename,cur_ent->tex_id,cur_ent->x,cur_ent->y,cur_ent->z,cur_ent->xrot,cur_ent->yrot,cur_ent->zrot,frame1,frame2,cur_ent->blendpos,cur_ent->blendcount);

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
      //printf("Texid: %i\n",cur_ent->tex_id);
      cur_ent->tex->select();
			//printf("Setting frame..\n");
      if(cur_ent->anim_start!=cur_ent->anim_end)
        cur_ent->model->SetFrame(frame1,frame2,cur_ent->blendpos,cur_ent->blendcount);
				//printf("md2model->draw()!\n");
        cur_ent->model->Draw();
				//printf("inc blendpos\n");
        cur_ent->blendpos++;
        if(cur_ent->blendpos>cur_ent->blendcount) {
          cur_ent->blendpos=0;
          cur_ent->framenum++;
        }

        if(cur_ent->framenum>cur_ent->anim_end) { cur_ent->framenum=cur_ent->anim_start; }
      }
    }
    cur_ent=cur_ent->next;
  }
	//printf("--render_world()\n");
}

void render_world_trans() {
  int i,j,frame1,frame2;
  float x,y,z,light,dz,dy,dx;
  struct entity *cur_ent=entity_list;
  struct entity *other_ent=NULL;
  struct poly *cur_pol=NULL;
	//printf("+++render_world()\n");

  //glPushMatrix();
  while(cur_ent!=NULL) {
    if(cur_ent->poly_list!=NULL&&cur_ent->alpha!=1.0f) {
			//printf("Rendering a brush (%s)...\n",cur_ent->type);
      glLoadIdentity();
      glRotatef(camera_xrot,1.0f,0.0f,0.0f);
      glRotatef(camera_zrot,0.0f,0.0f,1.0f);
      glRotatef(camera_yrot,0.0f,1.0f,0.0f);
      glTranslatef(-camera_x/100.0f,-camera_y/100.0f,camera_z/100.0f);
      glTranslatef(cur_ent->x/100.0f,cur_ent->y/100.0f,-cur_ent->z/100.0f);
			glRotatef(cur_ent->yrot,0.0f,1.0f,0.0f);
			glRotatef(cur_ent->xrot,1.0f,0.0f,0.0f);
			glRotatef(cur_ent->zrot,0.0f,0.0f,1.0f);
      cur_pol=cur_ent->poly_list;
      i=0;
    	glColor4f(1.0f,1.0f,1.0f,cur_ent->alpha);

      while(cur_pol!=NULL) {
        i++;
        cur_pol->tex->select();
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
	for(j=0;j<cur_pol->vert_count;j++) {
#ifndef WIN32
		glTexCoord2f(cur_pol->point[j].s,cur_pol->point[j].t);
#else
		glTexCoord2f(cur_pol->point[j].s,cur_pol->point[j].t*-1);
#endif
		glVertex3f(cur_pol->point[j].x/100.0f, cur_pol->point[j].y/100.0f, -cur_pol->point[j].z/100.0f);
	}
	glEnd();
        cur_pol=cur_pol->next;
      }
    } else {
        if(cur_ent->model!=NULL&&cur_ent->alpha!=1) {
	//printf("Drawing an md2 model (%s)...\n",cur_ent->type); 
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
	      //printf("model name: %s, tex: %i, x: %f, y: %f, z: %f frame1: %i frame2: %i\n",cur_ent->model->filename,cur_ent->tex_id,cur_ent->x,cur_ent->y,cur_ent->z,frame1,frame2);
        //draw_md2(cur_ent->model->filename,cur_ent->tex_id,cur_ent->x,cur_ent->y,cur_ent->z,cur_ent->xrot,cur_ent->yrot,cur_ent->zrot,frame1,frame2,cur_ent->blendpos,cur_ent->blendcount);

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
      //printf("Texid: %i\n",cur_ent->tex_id);
      cur_ent->tex->select();
			//printf("Setting frame..\n");
      if(cur_ent->anim_start!=cur_ent->anim_end)
        cur_ent->model->SetFrame(frame1,frame2,cur_ent->blendpos,cur_ent->blendcount);
				//printf("md2model->draw()!\n");
        glColor4f(1.0f,1.0f,1.0f,cur_ent->alpha);
        cur_ent->model->Draw();
				//printf("inc blendpos\n");
        cur_ent->blendpos++;
        if(cur_ent->blendpos>cur_ent->blendcount) {
          cur_ent->blendpos=0;
          cur_ent->framenum++;
        }

        if(cur_ent->framenum>cur_ent->anim_end) { cur_ent->framenum=cur_ent->anim_start; }
      }
    }

	cur_ent=cur_ent->next;
  }
}
