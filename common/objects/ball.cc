/* ball.cc - DCBlap ball object
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
#include <dxerr9.h>
#include "dsutil.h"
#endif
#ifdef DREAMCAST
#include <kos.h>
#endif
#ifdef LINUX
#include <SDL/SDL_mixer.h>
#endif
#ifdef TIKI
#include <Tiki/tiki.h>
#include <Tiki/texture.h>
#include <Tiki/sound.h>

using namespace Tiki;
using namespace Tiki::GL;
using namespace Tiki::Audio;
#endif
#include <stdlib.h>
#include <string.h>
#include "entity.h"
#include "objects.h"
#include "camera.h"
#include "utils.h"
#include "hud.h"

#ifdef DREAMCAST
sfxhnd_t sfx_bounce=-1;
sfxhnd_t sfx_score=-1;
#endif
#ifdef LINUX
Mix_Chunk *sfx_bounce=NULL;
Mix_Chunk *sfx_score=NULL;
#endif
#ifdef WIN32
extern CSoundManager *g_pSoundManager;
CSound*        sfx_bounce=NULL;
CSound*        sfx_score=NULL;
#endif
#ifdef TIKI
Sound *sfx_bounce=NULL;
Sound *sfx_score=NULL;
#endif
extern bool enable_sound;
extern struct entity *wld;
extern bool lose_flag;
extern int doublespeed;
bool ball_spin=1;

Texture *ball_tex=NULL;

void ball_reset() {
	if(ball_tex!=NULL) {
		delete ball_tex;
		ball_tex=NULL;
	}
}

void ball_create(struct entity *me) {
#ifdef WIN32
  HRESULT hr;
#endif
  me->model=new md2Model;
	if(get_prop(me,"model")!=NULL) {
		me->model->Load(get_prop(me,"model"));
	} else {
		me->model->Load("ball.md2");
	}
	if(ball_tex==NULL) ball_tex= new Texture("ball.png",0);
	me->tex=ball_tex;
	me->blendcount=1;
  me->anim_start=me->model->anim_start("stand");
  me->anim_end=me->model->anim_end("stand");
  me->blendpos=0;
  me->model->SetFrame(0,0,0,1);
	if(get_prop(me,"speed")) {
		me->arg3=atoi(get_prop(me,"speed"))+4;
	} else {
		me->arg3=3;
		me->arg6=1;
	}
	if(get_prop(me,"orientation")) {
		me->arg5=atoi(get_prop(me,"orientation"));
	}
  if(get_prop(me,"spin")) {
    ball_spin=atoi(get_prop(me,"spin"));
  } else {
    ball_spin=1;
  }
  //ball_spin=0;
  switch((int)me->yrot) {
  case 0:
    me->arg1=me->arg3;
    me->arg2=0;
    break;
  case 90:
    me->arg1=0;
    me->arg2=me->arg3;
    break;
  case 180:
    me->arg1=-me->arg3;
    me->arg2=0;
    break;
  case 270:
    me->arg1=0;
    me->arg2=-me->arg3;
    break;
  }
#ifdef DREAMCAST
  if(sfx_bounce==-1) sfx_bounce = snd_sfx_load("bounce.wav");
  if(sfx_score==-1) sfx_score = snd_sfx_load("score.wav");
#endif
#ifdef WIN32
  if(sfx_bounce==NULL) {
    hr=g_pSoundManager->Create( &sfx_bounce, "bounce.wav", 0, GUID_NULL );
  }
  if(sfx_score==NULL) {
    hr=g_pSoundManager->Create( &sfx_score, "score.wav", 0, GUID_NULL );
  }
#endif
#ifdef LINUX
  if(sfx_bounce==NULL) sfx_bounce=Mix_LoadWAV("bounce.wav");
  if(sfx_score==NULL) sfx_score=Mix_LoadWAV("score.wav");
#endif
#ifdef TIKI
	//if(sfx_bounce==NULL) sfx_bounce = new Sound("bounce.wav");
	//if(sfx_score==NULL) sfx_score = new Sound("score.wav");
#endif
}

#ifndef M_PI
#define M_PI 3.141592653589793238
#endif

#ifdef WIN32
extern SOCKET net_socket;
extern bool net_host;
#endif

void ball_update(struct entity *me, float gt) {
  static int cnt=0;
  char buf[100];
  if(ball_spin) {
    me->yrot+=6 * gt * 40;
    //me->xrot+=6 * gt * 40;
    if(me->yrot>360) me->yrot=0;
    //if(me->xrot>360) me->xrot=0;
  }

  if(me->paused==1) return;

  me->x+=me->arg1 * gt * 40;
  if(me->arg5==0) {
    me->z+=me->arg2 * gt * 40;
  } else {
    me->y+=me->arg2 * gt * 40;
  }
#ifdef WIN32
  cnt++;
  if(cnt>3) {
    if(net_host==1&&net_socket!=-1) {
      sprintf(buf,"3,%i,%f,%f,%f,%i,%i,%i,%i,%i,%i,%i\n",me->id,me->x,me->y,me->z,me->arg1,me->arg2,me->arg3,me->arg4,me->arg5,me->arg6,me->arg7);
      socket_write_line(net_socket,buf);
    }
    cnt=0;
  }
#endif
}

void ball_message(struct entity *me, struct entity *them, char *message) {
  char buf[200];
  int x,y;
  if(!strcmp(message,"win")) {
    me->deleted=1;
  } else if(!strcmp(message,"reset")) {
		if(me->arg6==1) me->deleted=1;
    make_coordinates(get_prop(me,"origin"),&me->x,&me->y,&me->z); //reset position
    make_coordinates(get_prop(me,"angles"),&me->xrot,&me->zrot,&me->yrot); //reset position
    me->anim_start=me->model->anim_start("stand");
    me->anim_end=me->model->anim_end("stand");
    me->blendpos=0;
    switch((int)me->yrot) {
    case 0:
      me->arg1=me->arg3;
      me->arg2=0;
      break;
    case 90:
      me->arg1=0;
      me->arg2=me->arg3;
      break;
    case 180:
      me->arg1=-me->arg3;
      me->arg2=0;
      break;
    case 270:
      me->arg1=0;
      me->arg2=-me->arg3;
      break;
    }
  } else if(!strcmp(message,"thud") && me->anim_start==me->model->anim_start("stand")) {
    if(!strcmp(them->type,"goal")) {
      doublespeed=0;
      if(wld->arg2>0) { //take away a ball
        wld->arg1--;
        if(wld->arg1<1) {
          pause_world(1);
          set_hud(0,320-(txt_width("Game Over")/2),240-16,"Game Over",1,1,1);
          set_hud(2,320-(txt_width("Press Start")/2),244,"Press Start",1,1,1);
          set_prop(wld,"next_map","none");
          broadcast_message(me,"win");
					lose_flag=1;
        }
      } else {
        them->arg1++; //give the goal a point
        if(!strcmp(get_prop(them,"num"),"1")) {
          me->arg1=me->arg3;
        } else {
          me->arg1=-me->arg3;
        }
      }
#ifdef DREAMCAST
      if(enable_sound) snd_sfx_play(sfx_score,255,128);
#endif
#ifdef WIN32
    if(enable_sound) sfx_score->Play(0,0);
#endif
#ifdef LINUX
	    if(enable_sound) Mix_PlayChannel(-1,sfx_score,0);
#endif
#ifdef TIKI
	    if(enable_sound && sfx_score!=NULL) sfx_score->play();
#endif
      me->arg2=0;
      broadcast_message(me,"reset");
    } else {
#ifdef DREAMCAST
      if(enable_sound) snd_sfx_play(sfx_bounce,255,128);
#endif
#ifdef WIN32
    if(enable_sound) sfx_bounce->Play(0,0);
#endif
#ifdef LINUX
	  if(enable_sound) Mix_PlayChannel(-1,sfx_bounce,0);
#endif
#ifdef TIKI
		if(enable_sound && sfx_bounce!=NULL) sfx_bounce->play();
#endif
#ifdef WIN32
    if(net_host==1&&net_socket!=-1) {
      sprintf(buf,"3,%i,%f,%f,%f,%i,%i,%i,%i,%i,%i,%i\n",me->id,me->x,me->y,me->z,me->arg1,me->arg2,me->arg3,me->arg4,me->arg5,me->arg6,me->arg7);
      socket_write_line(net_socket,buf);
    }
#endif
      me->x-=me->arg1;
	  	if(me->arg5==0) {
		  	me->z-=me->arg2;
  		} else {
	  		me->y-=me->arg2;
		  }
      if(!strcmp(them->type,"paddle")) {
        me->arg7=them->arg1;
        //me->arg3=them->arg4+1;
      }
      if(them->physics==1) {
        x=me->arg1;
        y=me->arg2;
        if(them->xvel!=0) me->arg1=them->xvel*2;
        if(them->zvel!=0) me->arg2=them->zvel*2;
        them->xvel=x/2;
        them->zvel=y/2;
      } else {
    //funky math to decide which way to bounce based on where we hit the other object
        if(them->xmin+them->x/100.0f<=me->xmin+me->x/100.0f && them->xmax+them->x/100.0f<=me->xmax+me->x/100.0f) me->arg1=me->arg3+(rand()%2);
        if(them->xmin+them->x/100.0f>=me->xmin+me->x/100.0f && them->xmax+them->x/100.0f>=me->xmax+me->x/100.0f) me->arg1=-me->arg3-(rand()%2);
			  if(me->arg5==0) {
				  if(them->zmin+them->z/100.0f<=me->zmin+me->z/100.0f && them->zmax+them->z/100.0f<=me->zmax+me->z/100.0f) me->arg2=me->arg3+(rand()%2);
				  if(them->zmin+them->z/100.0f>=me->zmin+me->z/100.0f && them->zmax+them->z/100.0f>=me->zmax+me->z/100.0f) me->arg2=-me->arg3-(rand()%2);  
			  } else {
				  if(them->ymin+them->y/100.0f<=me->ymin+me->y/100.0f && them->ymax+them->y/100.0f<=me->ymax+me->y/100.0f) me->arg2=me->arg3+(rand()%2);
				  if(them->ymin+them->y/100.0f>=me->ymin+me->y/100.0f && them->ymax+them->y/100.0f>=me->ymax+me->y/100.0f) me->arg2=-me->arg3-(rand()%2);  
			  }
        if(me->arg2==0) {
          me->arg2=((rand()%2)-1)*(me->arg3-(rand()%2));
        }
        if(me->arg1==0) {
          me->arg1=((rand()%2)-1)*(me->arg3-(rand()%2));
        }
      }
      me->x+=me->arg1;
		  if(me->arg5==0) {
			  me->z+=me->arg2;
		  } else {
			  me->y+=me->arg2;
		  }
    }
  }
}
