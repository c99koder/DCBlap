/* zztoop.cc - a script interpreter for DCBlap
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
#ifndef LINUX
#ifdef WIN32
#include <windows.h>
#include <kos_w32.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#endif
#ifdef DREAMCAST
#include <kos.h>
#endif
#include "entity.h"
#include "objects.h"
#include "texture.h"
#include "word.h"

#define ZZTOOP_BLEND 8
#define ZZT_MOTION_STEP 4

void zzt_goto(struct entity *myobj, char *label);

#ifdef DREAMCAST
off_t filelength(int fd) {
  off_t tmp=fs_tell(fd);
  off_t tmp2;  
  fs_seek(fd,0,SEEK_END);
  tmp2=fs_tell(fd);
  fs_seek(fd,tmp,SEEK_SET);
  return tmp2;
}
#endif

void zztoop_create(struct entity *me) {
  int fd;
  //if(get_model(get_prop(me,"model"))==NULL) load_md2(get_prop(me,"model"));
  if(find_tex(get_prop(me,"skin"))==-1) load_texture(get_prop(me,"skin"),next_tex_id());
  me->model=new md2Model;
  me->model->Load(get_prop(me,"model"));
  me->tex_id=find_tex(get_prop(me,"skin"));
  printf("Texid: %i\n",me->tex_id);
  me->anim_start=me->model->anim_start("stand");
  me->anim_end=me->model->anim_end("stand");
  me->blendcount=ZZTOOP_BLEND;
  me->blendpos=0;
  fd=fs_open(get_prop(me,"script"),O_RDONLY);
  me->prog=(char *)malloc(filelength(fd)+1);
  fs_read(fd,me->prog,filelength(fd));
  me->prog[filelength(fd)]='\0';
  me->progpos=0;
  me->proglen=filelength(fd)-1;
  printf("Program: %s\n",me->prog);
  printf("length: %i\n",me->proglen);
  me->ZZTOOP_XSTEP=0;
  me->ZZTOOP_ZSTEP=0;
  me->ZZTOOP_MOVECNT=0;
  fs_close(fd);
}

void zzt_send(struct entity *me, char *cmd);

void zztoop_message(struct entity *me, struct entity *them, char *message) {
  //myobj->arg2=them;
  zzt_send(me,message);
}

void zzt_send(struct entity *me, char *cmd) {
  int x,y;
  char target[100];
  char label[100];
  char name[100];
  struct entity *them;
  for(x=0;x<strlen(cmd);x++) {
    if(cmd[x]==':') {
      break;
    }
    target[x]=cmd[x];
  }
  target[x]='\0';
  //printf("x: %i strlen(%s): %i\n",x,cmd,strlen(cmd));
  if(x==strlen(cmd)) {
    zzt_goto(me,target);
    return;
  }
  x++;
  for(y=0;y<strlen(cmd)-x;y++) {
    label[y]=cmd[x+y];
  }
  label[y]='\0';
  them=find_ent_by_prop(me->type,"targetname",target);
  zzt_goto(them,label);
}

void zzt_zap(struct entity *myobj, char *label) {
  char text[256];
  int x,y,newline=0,goagain=1;
  for(x=0;x<myobj->proglen;x++) {
    switch(myobj->prog[x]) {
    case '\n':
      newline=1;
      break;
    case ':':
      x++;
      if(newline==1) {
	y=0;
	while(myobj->prog[x+y]!='\n') {
	  text[y]=myobj->prog[x+y];
	  y++;
	}
	text[y]='\0';
	if(!strcmp(label,text)) {
	  myobj->prog[x]='\'';
	  return;
	}
	x+=y;
	newline=0;
      }
      break;
    default:
      newline=0;
      break;
    }
  }
}

void zzt_goto(struct entity *myobj, char *label) {
  char text[256];
  int x,y,newline=1,goagain=1;
  if(label==NULL) return;
  for(x=0;x<myobj->proglen;x++) {
    switch(myobj->prog[x]) {
    case '\n':
      newline=1;
      break;
    case ':':
      x++;
      if(newline==1) {
	y=0;
	while(myobj->prog[x+y]!='\n') {
	  text[y]=myobj->prog[x+y];
	  y++;
	}
	text[y]='\0';
        //printf("Compare: %s and %s\n",label,text);
	if(!strcmp(label,text)) {
	  myobj->progpos=x+y;
	  //printf("Matched %s for %s\n",text,get_prop(myobj,"targetname"));
	  return;
	}
	x+=y;
	newline=0;
      }
      break;
    default:
      newline=0;
      break;
    }
  }
}

#if 0
shot_handler zztoop_shot(int me, int them) {
  struct object *theirobj=get_obj_by_id(them);
  if(theirobj->type==OBJECT_PLAYER) {
    zzt_goto(me,"shot");
    zztoop_update(me);
  }
}

collide_handler zztoop_collide(int me, int them, int atfault) {
  struct object *myobj=get_obj_by_id(me);
  struct object *theirobj=get_obj_by_id(them);
  if(atfault==me) { myobj->progpos-=2; return; }
  if(theirobj->type==OBJECT_PLAYER) {
    zzt_goto(me,"touch");
    zztoop_update(me);
  }
}
#endif

void zztoop_update(struct entity *myobj) {
  struct entity *player=find_ent_by_type("player_entity");
  int in;
  char *text;
  char filename[100];
  char pathdest[100];
  int x=0,y=0,newline=0,linecount=0,goagain=1,tx=0,ty=0;
  struct entity *pathnode;

  if(myobj->ZZTOOP_MOVECNT>0) {
    //printf("MOVECNT: %i\n",myobj->ZZTOOP_MOVECNT);
    myobj->ZZTOOP_MOVECNT--;
    myobj->x+=myobj->ZZTOOP_XSTEP;
    myobj->z+=myobj->ZZTOOP_ZSTEP;
    if((myobj->ZZTOOP_XSTEP != 0 || myobj->ZZTOOP_ZSTEP != 0) && myobj->anim_start!=myobj->model->anim_start("run")) {
      myobj->anim_start=myobj->model->anim_start("run");
      myobj->anim_end=myobj->model->anim_end("run");
      myobj->blendpos=0;
    }
    if(myobj->ZZTOOP_MOVECNT==0) {
      myobj->ZZTOOP_XSTEP=0;
      myobj->ZZTOOP_ZSTEP=0;
    }
    return;
  }

  while(goagain) {
    goagain=0;
    if(get_prop(myobj,"path_target")) {
      strcpy(pathdest,get_prop(myobj,"path_target"));
      pathnode=find_ent_by_prop("path_node","targetname",pathdest);
      if((int)(myobj->yrot-pathnode->yrot)%360>0) {
          myobj->yrot-=2;
      } else if((int)(myobj->yrot-pathnode->yrot)%360<0) {
          myobj->yrot+=2;
      } else {
        if(myobj->x<pathnode->x) {
          if(myobj->x+ZZT_MOTION_STEP>pathnode->x) {
            myobj->x=pathnode->x;
          } else {
            myobj->x+=ZZT_MOTION_STEP;
          }
        } else if(myobj->x>pathnode->x) {
          if(myobj->x+ZZT_MOTION_STEP<pathnode->x) {
            myobj->x=pathnode->x;
          } else {
            myobj->x-=ZZT_MOTION_STEP;
          }
        }
        if(myobj->z<pathnode->z) {
          if(myobj->z+ZZT_MOTION_STEP>pathnode->z) {
            myobj->z=pathnode->z;
          } else {
            myobj->z+=ZZT_MOTION_STEP;
          }
        } else if(myobj->z>pathnode->z) {
          if(myobj->z+ZZT_MOTION_STEP<pathnode->z) {
            myobj->z=pathnode->z;
          } else {
            myobj->z-=ZZT_MOTION_STEP;
          }
        }
        if(myobj->x==pathnode->x&&myobj->z==pathnode->z) {
          set_prop(myobj,"path_target",get_prop(pathnode,"target"));
        }
      }
    }

    if(myobj->progpos>myobj->proglen || myobj->progpos==-1) { return; }
    text=(char *)malloc(myobj->proglen);
    switch(myobj->prog[myobj->progpos]) {
    case '\'':
    case ':':
      while(myobj->prog[myobj->progpos]!='\n') {
	myobj->progpos++;
      }
      break;
    case '/':
      myobj->progpos++;
      switch(myobj->prog[myobj->progpos]) {
      case 'n':
        myobj->ZZTOOP_XSTEP=0;
        myobj->ZZTOOP_ZSTEP=ZZT_MOTION_STEP;
        myobj->ZZTOOP_MOVECNT=64/ZZT_MOTION_STEP;
	myobj->yrot=90;
	break;
      case 's':
        myobj->ZZTOOP_XSTEP=0;
        myobj->ZZTOOP_ZSTEP=-ZZT_MOTION_STEP;
        myobj->ZZTOOP_MOVECNT=64/ZZT_MOTION_STEP;
	myobj->yrot=270;
	break;
      case 'e':
        myobj->ZZTOOP_XSTEP=ZZT_MOTION_STEP;
        myobj->ZZTOOP_ZSTEP=0;
        myobj->ZZTOOP_MOVECNT=64/ZZT_MOTION_STEP;
	myobj->yrot=0;
	break;
      case 'w':
        myobj->ZZTOOP_XSTEP=-ZZT_MOTION_STEP;
        myobj->ZZTOOP_ZSTEP=0;
        myobj->ZZTOOP_MOVECNT=64/ZZT_MOTION_STEP;
	myobj->yrot=180;
	break;
      case 'i':
        myobj->ZZTOOP_XSTEP=0;
        myobj->ZZTOOP_ZSTEP=0;
        myobj->ZZTOOP_MOVECNT=64/ZZT_MOTION_STEP;
        if(myobj->anim_start==myobj->model->anim_start("run")) {
          myobj->anim_start=myobj->model->anim_start("stand");
          myobj->anim_end=myobj->model->anim_end("stand");
          myobj->blendpos=0;
        }
	break;
      }
      break;
#if 0 //this is archaic, our object system supports names natively
    case '@':
      x=++myobj->progpos;
      y=0;
      while(myobj->prog[y+x]!='\r') {
	//printf("searching for EOL after @...");
	text[y]=myobj->prog[y+x];
	y++;
	myobj->progpos++;
      }
      text[y]='\0';
      set_name(myobj,text);
      break;
#endif
    case '#':
      x=++myobj->progpos;
      y=0;
      while(myobj->prog[y+x]!='\n') {
	text[y]=myobj->prog[y+x];
	y++;
	myobj->progpos++;
      }
      text[y]='\0';
      wordify(text,' ');
      if(!strcmp("turn",get_word(0))) {
        if(!strncmp("n",get_word(1),1)) {
          myobj->yrot=90;
        }
        if(!strncmp("s",get_word(1),1)) { 
          myobj->yrot=270;
        }
        if(!strncmp("e",get_word(1),1)) { 
          myobj->yrot=0;
        }
        if(!strncmp("w",get_word(1),1)) { 
          myobj->yrot=180;
        }
      }
      if(!strcmp("path",get_word(0))) {
        set_prop(myobj,"path_target",get_word(1));
        goagain=1;
      }
      if(!strcmp("anim",get_word(0))) {
        myobj->anim_start=myobj->model->anim_start(get_word(1));
        myobj->anim_end=myobj->model->anim_end(get_word(1));
        myobj->ZZTOOP_MOVECNT=(myobj->anim_end-myobj->anim_start+1)*myobj->blendcount;
        myobj->blendpos=0;
      }
      if(!strcmp("end",get_word(0))) {
	myobj->progpos=-1;
        free(text);
        return;
      }
      if(!strcmp("zap",get_word(0))) {
	zzt_zap(myobj,get_word(1));
      }
      if(!strcmp("send",get_word(0))) {
	zzt_send(myobj,get_word(1));
	goagain=1;
      }
      if(!strcmp("restart",get_word(0))) {
	myobj->progpos=0;
	return;
      }
//untested, unsupported, and outdated commands follow
#if 0
      if(!strcmp("lock",get_word(0))) {
	myobj->flags|=F_SLEEPING;
	goagain=1;
      }
      if(!strcmp("unlock",get_word(0))) {
	myobj->flags&=~F_SLEEPING;
      }
      if(!strcmp("teleport",get_word(0))) {
	if(is_empty(atoi(get_word(1)),atoi(get_word(2)))==0) {
	  //set_msg("Invalid teleport.");
	} else {
	  myobj->x=atoi(get_word(1));
	  myobj->y=atoi(get_word(2));
	}
      }
      if(!strcmp("board",get_word(0))) {
	//main_board_switch(atoi(get_word(1)));
	return;
      }
      if(myobj->arg2!=-1 && !strcmp("give",get_word(0))) {
	if(!strcmp("ammo",get_word(1))) {
	  give_ammo(myobj->arg2,atoi(get_word(2)));
	}
	if(!strcmp("score",get_word(1))) {
	  //give_score(myobj->arg2,atoi(get_word(2)));
	}
	if(!strcmp("health",get_word(1))) {
	  give_health(myobj->arg2,atoi(get_word(2)));
	}
	goagain=1;
      }
      if(myobj->arg2!=-1 && !strcmp("take",get_word(0))) {
	if(!strcmp("ammo",get_word(1))) {
	  take_ammo(myobj->arg2,atoi(get_word(2)));
	}
	if(!strcmp("score",get_word(1))) {
	  //take_score(myobj->arg2,atoi(get_word(2)));
	}
	if(!strcmp("health",get_word(1))) {
printf("theirobj->PLAYER_HEALTH: %i  get_word(2): %i\n",theirobj->PLAYER_HEALTH,atoi(get_word(2)));
	  if(theirobj->PLAYER_HEALTH<atoi(get_word(2))) { zzt_goto(me,get_word(3)); }
	  take_health(myobj->arg2,atoi(get_word(2)));
	}
	goagain=1;
      }
      if(!strcmp("die",get_word(0))) {
	myobj->flags|=F_DELETED;
	myobj->progpos=-1;
      }
      if(!strcmp("shoot",get_word(0))) {
	if(!strcmp("n",get_word(1)) || !strcmp("north",get_word(1))) {
	  shoot(me,UP);
	}
	if(!strcmp("s",get_word(1)) || !strcmp("south",get_word(1))) {
	  shoot(me,DOWN);
	}
	if(!strcmp("e",get_word(1)) || !strcmp("east",get_word(1))) {
	  shoot(me,RIGHT);
	}
	if(!strcmp("w",get_word(1)) || !strcmp("west",get_word(1))) {
	  shoot(me,LEFT);
	}
	if(!strcmp("seek",get_word(1))) {
	  //shoot(me,toward(me,get_player(0)));
	}
      }
#endif
      break;
    case '\n':
      goagain=1;
      break;
    default:
	printf("Text creation code called!! %i (%c)\n",myobj->prog[myobj->progpos],(unsigned int)myobj->prog[myobj->progpos]);
      x=myobj->progpos;
      y=0;
      while(myobj->prog[y+x]!='\0') {
	//printf("building text...");
	if(myobj->prog[y+x]=='\n') {
	  newline=1;
	  linecount++;
	}
	if((newline==1 && myobj->progpos!=-1) && (myobj->prog[y+x]=='#'||myobj->prog[y+x]=='/' || myobj->prog[x+y]==':' || myobj->proglen <= x+y+1)) {
#if 0
	  text[y-1]='\0';
	  if(text[0]!=':') {
	    //if(linecount>1) {
	    //  text_window(text);
	    if(myobj->arg2==-1) {
	      txtwin_to_all(myobj->board,myobj->id,myobj->name,text);
	    } else {
	      txtwin_to_one(myobj->arg2,myobj->id,myobj->name,text);
	    }
	    //} else {
	    //  set_msg(text);
	    //  goagain=1;
	    //}
	  }
	  myobj->progpos--;
	  //myobj->update(me);
	  break;
#endif
	}
	text[y]=myobj->prog[y+x];
	y++;
	myobj->progpos++;
      }
    }
    myobj->progpos++;
    free(text);
  }
}
#endif
