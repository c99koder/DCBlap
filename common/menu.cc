/* menu.cc - DCBlap menus and main loop
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
#ifdef DREAMCAST
#include <kos.h>
#include <GL/gl.h>
#include <zlib/zlib.h>
//#include <mp3/sndserver.h>
#include <oggvorbis/sndoggvorbis.h>
#endif
#ifdef WIN32
#include <stdio.h>
#include <fcntl.h>
#include <windows.h>
#include <dshow.h>
#include <time.h>
#include <atlbase.h>
#include <GL\gl.h>
#include "dirwin32.h"
#endif
#ifdef UNIX
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#endif
#ifdef LINUX
#include <GL/gl.h>
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#endif
#include <stdlib.h>
#include "texture.h"
#include "text.h"
#include "utils.h"
#include "entity.h"
#include "hud.h"
#include "callback.h"
#include "camera.h"
#include "direction.h"
#include "objects.h"
#include "word.h"
#include "utils.h"

void debug(char *text);

#ifdef DREAMCAST
void exit(int x) {
  sndoggvorbis_stop();
  destroy_world();
  arch_exit();
}
#endif

extern char tex_theme_dir[100];
void render_bg(float alpha);
void level_select(char *path);

int titletex=20;
int playersel=21;
int gamesel=22;
int options=23;
int loading=24;
int stars=25;
int logo=26;
int doublespeed=0;
bool randtitle=0;

void set_doublespeed(int speed) { doublespeed=speed; }

#ifdef WIN32
extern bool keys[256];
extern HDC hDC;
extern HWND hWnd;
extern bool active;

SOCKET net_socket=-1;
#endif

#ifdef LINUX
extern bool keys[256];
extern bool isActive=1;
Mix_Music *bgm;

SDL_TimerID game_timer;

Uint32 update_timer(Uint32 interval, void *param) {
  update_world();
  if(doublespeed) update_world();
  return interval;
}

bool sys_render_begin() {
  SDL_Event event;

  if(SDL_PollEvent(&event)) {
    switch(event.type) {
      case SDL_ACTIVEEVENT:
        isActive= event.active.gain;
        break;
      case SDL_KEYDOWN:
        keys[event.key.keysym.sym]=1;
        break;
      case SDL_KEYUP:
        keys[event.key.keysym.sym]=0;
        break;
      case SDL_QUIT:
        exit(0);
        break;
      default:
        break;
    }
  }
  return isActive;
}
#endif

#ifdef DREAMCAST
bool sys_render_begin() {
  glKosBeginFrame();
  return 1;
}

void copy_file(char *src, char *dst) {
  int fd1,fd2,l;
  char buf[1024];

  printf("Copying %s to %s...",src,dst);

  fd1=fs_open(src,O_RDONLY);
  fd2=fs_open(dst,O_WRONLY);

  do {
    l=fs_read(fd1,buf,1024);
    fs_write(fd2,buf,l);
  } while(l==1024);
  fs_close(fd2);
  fs_close(fd1);
  printf("\n");
}
#endif

#ifdef WIN32 //pump the message queue
extern IMediaControl *pControl;
extern IMediaSeeking *pSeek;
extern IMediaEvent *pEvent;

bool sys_render_begin() {
  MSG msg;
  LONG evCode, evParam1, evParam2;
  LONGLONG pos=0;
  if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))	{
		if (msg.message==WM_QUIT)	{
      stop_bgm();
			exit(0);							// If So done=TRUE
		}	else {
			TranslateMessage(&msg);				// Translate The Message
			DispatchMessage(&msg);				// Dispatch The Message
		}
  }
  if(pEvent!=NULL) {
    while(SUCCEEDED(pEvent->GetEvent(&evCode, &evParam1, &evParam2,0))) {
      if(evCode==EC_COMPLETE)
        pSeek->SetPositions(&pos, AM_SEEKING_AbsolutePositioning ,NULL, AM_SEEKING_NoPositioning);
      pEvent->FreeEventParams(evCode, evParam1, evParam2);
    }
  }
	return active;
}
#endif

void sys_render_finish() {
#ifdef DREAMCAST
  glKosFinishFrame();
#endif
#ifdef WIN32
    SwapBuffers(hDC);
#endif
#ifdef LINUX
    SDL_GL_SwapBuffers();
#endif
}

#ifdef DREAMCAST
uint8 *rd2;
#endif

bool enable_sound=1;
bool enable_music=0;
bool enable_powerups=1;
bool net_host=0;

void shad_txt(float x, float y, char *text, float r, float g, float b) {
	draw_txt(x+4,y+4,text,0,0,0,0.4f);
	draw_txt(x,y,text,r,g,b,1);
}

void draw_menu_list(float x, float y, char *list,int active) {
  int i;
  wordify(list,',');
  for(i=0;i<get_word_count();i++) {
    if(i==active) {
      draw_txt(x+4,y+4,get_word(i),0,0,0,0.6f);
      draw_txt(x,y,get_word(i),1.0f,1.0f,1.0f,1);
    } else {
      draw_txt(x+4,y+4,get_word(i),0,0,0,0.6f);
      draw_txt(x,y,get_word(i),0.6f,0.6f,0.6f,1);
    }
    x+=txt_width(get_word(i))+20;
  }
}

void do_options() {
#ifdef WIN32
  USES_CONVERSION;
#endif
	int sel=0,i;
	float alpha,x=0;
  char title[100];
  switch_tex(options);
	for(alpha=0;alpha<1;alpha+=.05) {
		sys_render_begin();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		render_bg(alpha);
		sys_render_finish();
	}
	while(1) {
		switch(get_controls(0)) {
		case MOVE_UP:
			sel--; if(sel<0) sel=0;
			delay(.1);
			break;
		case MOVE_DOWN:
			sel++; if(sel>2) sel=2;
			delay(.1);
			break;
		case MOVE_LEFT:
			switch(sel) {
			case 0:
				if(enable_music) {
					enable_music=0;
#ifdef WIN32
					stop_bgm();
#endif
#ifdef LINUX
					Mix_HaltMusic();
#endif
#ifdef DREAMCAST
          sndoggvorbis_stop();
#endif
				}
				break;
			case 1:
			  enable_sound=0;
				break;
			case 2:
				enable_powerups=0;
				break;
      case 3:
        net_host=0;
			}
			break;
		case MOVE_RIGHT:
			switch(sel) {
			case 0:
				if(!enable_music) {
					enable_music=1;
#ifdef DREAMCAST
  if(enable_music) { 
    sndoggvorbis_stop();
    sprintf(title,"/cd/%s/menu.ogg",tex_theme_dir);
    if(sndoggvorbis_start(title,1)<0) {
      sndoggvorbis_start("/cd/menu.ogg",1);
    }
  }
#endif
#ifdef WIN32
  if(enable_music) {
    sprintf(title,"%s\\menu.ogg",tex_theme_dir);
    FILE *fd=fopen(title,"r");
    if(fd) {
      fclose(fd);
      play_bgm(A2W(title));
    } else {
      play_bgm(L"menu.ogg");
    }
  }
#endif
#ifdef LINUX
  					if(bgm && Mix_PlayMusic(bgm,-1)==-1) {
    			printf("BGM play failed: %s\n",Mix_GetError());
  					}
#endif
				}
				break;
			case 1:
			  enable_sound=1;
				break;
			case 2:
				enable_powerups=1;
				break;
      case 3:
        net_host=1;
			}
			break;
		case START_BTN:
				for(alpha=1;alpha>0;alpha-=.05) {
					sys_render_begin();
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
          switch_tex(options);
					render_bg(alpha);
					sys_render_finish();
				}
			return;
		}
	  if (sys_render_begin()) {
   	  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
      switch_tex(options);
      render_bg(1);
#ifdef DREAMCAST
      glKosFinishList();
#else
      glEnable(GL_BLEND);
#endif
      glColor4f(0.2,0.2,0.2,0.6);
      glDisable(GL_TEXTURE_2D);
      glBegin(GL_QUADS);
      glVertex3f(0,480-60,0);
      glVertex3f(640,480-60,0);
      glVertex3f(640,480-175,0);
      glVertex3f(0,480-175,0);
      glEnd();
      glColor4f(1,1,1,0.6);
      glEnable(GL_TEXTURE_2D);
      glTranslatef(x,0,0);
      x--;
      if(x<=-720) x=0;
      for(i=0;i<8;i++) {
        switch_tex((i%4)+1);
        glBegin(GL_QUADS);
#ifdef WIN32
        glTexCoord2f(0,1); glVertex3f(i*180,480-65,0);
        glTexCoord2f(1,1); glVertex3f(i*180+165,480-65,0);
        glTexCoord2f(1,0); glVertex3f(i*180+165,480-170,0);
        glTexCoord2f(0,0); glVertex3f(i*180,480-170,0);
#else
        glTexCoord2f(0,0); glVertex3f(i*180,480-65,0);
        glTexCoord2f(1,0); glVertex3f(i*180+165,480-65,0);
        glTexCoord2f(1,1); glVertex3f(i*180+165,480-170,0);
        glTexCoord2f(0,1); glVertex3f(i*180,480-170,0);
#endif
        glEnd();
      }
      glLoadIdentity();
#ifdef WIN32
      glDisable(GL_BLEND);
#endif
      shad_txt(220,200,"Music:",0,0,1);
      draw_menu_list(220+124,200,"off,on",enable_music);
      shad_txt(220,200+24,"Sound:",0,0,1);
      draw_menu_list(220+124,200+24,"off,on",enable_sound);
      shad_txt(220,200+48,"Extras:",0,0,1);
      draw_menu_list(220+124,200+48,"off,on",enable_powerups);
      //shad_txt(220,200+72,"Net:",0,0,1);
      //draw_menu_list(220+124,200+72,"client,server",net_host);

      shad_txt(220+100,200+(24*sel),">",0,0,1);
      sys_render_finish();
		}
	}
}

void player_select() {
  int p1=0,p2=-1,go=1;
	float alpha;
  int i,x=0;
  switch_tex(playersel);
	for(alpha=0;alpha<1;alpha+=.05) {
		sys_render_begin();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		render_bg(alpha);
		sys_render_finish();
	}
  while(go) {
		switch(get_controls(0)) {
		case MOVE_LEFT:
			p1=0;
			break;
		case MOVE_RIGHT:
			p1=-1;
			break;
		case START_BTN:
			while(get_controls(0)==START_BTN);
			go=0;
			break;
		}
		switch(get_controls(1)) {
		case MOVE_LEFT:
			p2=1;
			break;
		case MOVE_RIGHT:
			p2=-1;
			break;
		}
    if (sys_render_begin()) {
   		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
      switch_tex(playersel);
      render_bg(1);
#ifdef DREAMCAST
      glKosFinishList();
#else
      glEnable(GL_BLEND);
#endif
      glColor4f(0.2,0.2,0.2,0.4);
      glDisable(GL_TEXTURE_2D);
      glBegin(GL_QUADS);
      glVertex3f(0,480-50,0);
      glVertex3f(640,480-50,0);
      glVertex3f(640,480-165,0);
      glVertex3f(0,480-165,0);
      glEnd();
      glColor4f(1,1,1,0.4);
      glEnable(GL_TEXTURE_2D);
      glLoadIdentity();
      glTranslatef(x,0,0);
      x--;
      if(x<=-720) x=0;
      for(i=0;i<8;i++) {
        switch_tex((i%4)+1);
        glBegin(GL_QUADS);
#ifdef WIN32
        glTexCoord2f(0,1); glVertex3f(i*180,480-55,0);
        glTexCoord2f(1,1); glVertex3f(i*180+165,480-55,0);
        glTexCoord2f(1,0); glVertex3f(i*180+165,480-160,0);
        glTexCoord2f(0,0); glVertex3f(i*180,480-160,0);
#else
        glTexCoord2f(0,0); glVertex3f(i*180,480-55,0);
        glTexCoord2f(1,0); glVertex3f(i*180+165,480-55,0);
        glTexCoord2f(1,1); glVertex3f(i*180+165,480-160,0);
        glTexCoord2f(0,1); glVertex3f(i*180,480-160,0);
#endif
        glEnd();
      }
      glLoadIdentity();
#ifdef WIN32
      glDisable(GL_BLEND);
#endif
      shad_txt(140,180,"Player 1:",1,0,0);
      draw_menu_list(140+144,180,"human,computer",(p1==-1)?1:0);
      shad_txt(140,180+24,"Player 2:",1,0,0);
      draw_menu_list(140+144,180+24,"human,computer",(p2==-1)?1:0);
      sys_render_finish();
		}
	}
  set_player_map(0,p1);
	set_player_map(1,p2);
	for(alpha=1;alpha>0;alpha-=.05) {
		sys_render_begin();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    switch_tex(playersel);
		render_bg(alpha);
		sys_render_finish();
	}
}

int cycle_len=20;
int avg_cycle_len=20;
bool win_flag=0;
bool lose_flag=0;
char next_map[256]="";
struct entity *g1=NULL;
struct entity *g2=NULL;
struct entity *wld=NULL;
int p1_score=0;
int p2_score=0;
int ballcnt=0;
int ballmax=0;

void play_pong() {
  char title[256];
  char author[256];
  float i;
  int t;
  bool sfxplaying=0;
#ifdef DREAMCAST
  static sfxhnd_t sfx_win=snd_sfx_load("win.wav");
  uint32 s,ms,tm,otm=0,fps=0;
  uint32 avgupd=0;
  uint32 avgren=0;
#endif
  win_flag=0;
  lose_flag=0;
  doublespeed=0;

#ifdef WIN32
  USES_CONVERSION;
  DWORD tm;

  /*if(net_host==1) {
    net_socket=wait_for_connect(5555);
    t=time(NULL);
    srand(t);
    sprintf(title,"%i\n",t);
    socket_write_line(net_socket,title);
    WSAAsyncSelect(net_socket, hWnd, WM_USER+100, FD_READ);
  } else {
    net_socket=connect_to_host("192.168.11.103",5555);
    socket_read_line(net_socket,title);
    srand(atoi(title));
    WSAAsyncSelect(net_socket, hWnd, WM_USER+100, FD_READ);
  }*/
#endif
#ifdef DREAMCAST
  if(enable_music) { 
    sndoggvorbis_stop();
    sprintf(title,"/cd/%s/game.ogg",tex_theme_dir);
    if(sndoggvorbis_start(title,1)<0) {
      sndoggvorbis_start("/cd/game.ogg",1);
    }
  }
  fs_chdir("/ram");
#endif
#ifdef WIN32
  mciSendString("open win.wav alias win",0,0,NULL);
  if(enable_music) {
    sprintf(title,"%s\\game.ogg",tex_theme_dir);
    FILE *fd=fopen(title,"r");
    if(fd) {
      fclose(fd);
      play_bgm(A2W(title));
    } else {
      play_bgm(L"game.ogg");
    }
  }
#endif
#ifdef LINUX
  bgm=Mix_LoadMUS("game.ogg");
  if(!bgm) { printf("BGM load failed: %s\n",Mix_GetError()); }
  if(enable_music && bgm && Mix_PlayMusic(bgm,-1)==-1) {
    printf("BGM play failed: %s\n",Mix_GetError());
  }
#endif
  g1=find_ent_by_prop("goal","num","1");
  g2=find_ent_by_prop("goal","num","2");
  wld=find_ent_by_type("worldspawn");
  if(get_prop(wld,"ballcnt")!=NULL) {
    wld->arg1=atoi(get_prop(wld,"ballcnt"));
    wld->arg2=atoi(get_prop(wld,"ballcnt"));
  }
  if(get_prop(wld,"score")!=NULL) wld->arg3=atoi(get_prop(wld,"score"));
  if(p1_score>0) g1->arg1=p1_score;
  if(p2_score>0) g2->arg1=p2_score;
  if(ballcnt>0) wld->arg1=ballcnt;
  if(ballmax>0) wld->arg2=ballmax;
  strcpy(title,get_prop(wld,"title"));
  strcpy(author,get_prop(wld,"author"));

  for(i=0;i<1;i+=.02) {
    if(sys_render_begin()) {
   		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
			render_bg(0);
			draw_txt(320-(txt_width(title)/2),228,title,i,i,i,1);
  		draw_txt(320-(txt_width(author)/2),228+24,author,i,i,i,1);
			sys_render_finish();
		}
  }
  delay(1);
  for(i=1;i>0;i-=.02) {
    if(sys_render_begin()) {
   		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
			render_bg(0);
			draw_txt(320-(txt_width(title)/2),228,title,i,i,i,1);
  		draw_txt(320-(txt_width(author)/2),228+24,author,i,i,i,1);
			sys_render_finish();
		}
  }
  glColor4f(1.0f,1.0f,1.0f,1.0f);
  reset_HUD();
  pause_world(1);
  update_world();
  if(doublespeed) update_world();
#ifdef WIN32
  SetTimer( hWnd, 0, 25, NULL );
#endif
#ifdef LINUX
  game_timer=SDL_AddTimer(25,update_timer,0);
#endif
  while(1) {
    if(win_flag) {
      strcpy(next_map,get_prop(wld,"next_map"));
      if(!strcmp(next_map,"none")) {
        p1_score=0;
        p2_score=0;
        ballcnt=0;
        ballmax=0;
      } else {
        if(g1!=NULL) p1_score=g1->arg1;
        if(g2!=NULL) p2_score=g2->arg1;
        ballcnt=wld->arg1;
        ballmax=wld->arg2;
      }
      break;
    }
    if(lose_flag) {
      p1_score=0;
      p2_score=0;
      ballcnt=0;
      ballmax=0;
      break;
    }
		if(get_controls(0)==QUIT_BTN) {
      p1_score=0;
      p2_score=0;
      ballcnt=0;
      ballmax=0;
      break;
    }
    sprintf(title,"Balls: %i",wld->arg1);
    if(wld->arg2>0) set_hud(1,440,30,title,1,(float)wld->arg1/((float)wld->arg2/1.2f),(float)wld->arg1/(float)wld->arg2);
    if(sys_render_begin()) {
    	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
	    glColor4f(1.0f,1.0f,1.0f,1.0f);
#ifdef DREAMCAST
				timer_ms_gettime(&s,&ms);
				tm=(s*1000)+ms;
#endif
        update_net();
#ifdef DREAMCAST
        update_world();
				if(doublespeed) update_world();
#endif
				render_world_solid();
#ifdef DREAMCAST
				glKosFinishList();

#endif
#ifdef WIN32
				glEnable(GL_BLEND);
#endif
				render_world_trans();
#ifdef WIN32
				glDisable(GL_BLEND);
#endif
				render_HUD();
#ifdef DREAMCAST
				fps++;
				do{
					timer_ms_gettime(&s,&ms);
				} while((s*1000)+ms<tm+20);
				timer_ms_gettime(&s,&ms);
				if(otm==0) {
					otm=(s*1000)+ms;
				}
				if(((s*1000)+ms)-otm>1000) {
					printf("FPS: %i\n",fps);
					fps=0;
					otm=((s*1000)+ms);
				}
#endif
			sys_render_finish();
		}
  }
 	destroy_world(); 
#ifdef DREAMCAST
	//sndoggvorbis_stop();
	if(enable_music) sndoggvorbis_stop();
  fs_chdir("/cd");
#endif
#ifdef WIN32
	stop_bgm();
  KillTimer(hWnd,0);
#endif
#ifdef LINUX
	SDL_RemoveTimer(game_timer);
	Mix_HaltMusic();
	Mix_FreeMusic(bgm);
#endif
}

void render_bg(float alpha) {
  glBegin(GL_QUADS);
  glColor3f(alpha,alpha,alpha);
#ifdef WIN32
  glTexCoord2f(0,0);
  glVertex3f(0,0,0);
  glTexCoord2f(1,0);
  glVertex3f(640,0,0);
  glTexCoord2f(1,1);
  glVertex3f(640,480,0);
  glTexCoord2f(0,1);
  glVertex3f(0,480,0);
#else
  glTexCoord2f(0,1);
  glVertex3f(0,0,0);
  glTexCoord2f(1,1);
  glVertex3f(640,0,0);
  glTexCoord2f(1,0);
  glVertex3f(640,480,0);
  glTexCoord2f(0,0);
  glVertex3f(0,480,0);
#endif
  glEnd();
}

void do_addons() {
  int x=0,i,j,y=0;
  unsigned int maxlen=0;
	float alpha;
#ifdef WIN32
  DIR* de;
  USES_CONVERSION;
#endif
#ifdef DREAMCAST
  uint8 *ftmp;
  int fd,fd2;
  gzFile f;
  int fs;
  vmu_hdr_t vmuhdr;
  dirent_t *de;
  uint32 d;
  int vmend=16;
  int vmumount=0;
  char title[100];
#endif
#ifdef UNIX
  dirent *de;
  DIR *d;
#endif
  char filenames[50][100];
  int sel=0;
  int loc[50];
  char tmp[110];

  strcpy(filenames[0],"(none)");
  x++;

#ifdef DREAMCAST
  sndoggvorbis_stop();
  
  d = fs_open("/cd/addons",O_RDONLY|O_DIR);
#endif
#ifdef WIN32
  de = opendir("addons");
#endif
#ifdef UNIX
  d = opendir("addons");
#endif

#ifdef DREAMCAST
  while ( (de=fs_readdir(d)) ) {
#endif
#ifdef WIN32
  while ( (de=readdir(de)) ) {
#endif
#ifdef UNIX
  while ( (de=readdir(d)) ) {
#endif

#ifdef UNIX
	if(de->d_name[0]!='.') {
  		strcpy(filenames[x],de->d_name);
#else
	if(de->name[0]!='.') {
  		strcpy(filenames[x],de->name);
#endif
        if(strlen(filenames[x])>maxlen) maxlen=strlen(filenames[x]);
	loc[x]=0;
        x++;
      }
    }

#ifdef DREAMCAST
  fs_close(d);
#endif
#ifdef WIN32
  closedir(de);
#endif
#ifdef UNIX
  closedir(d);
#endif


#ifdef DREAMCAST
  d = fs_open("/vmu/a1",O_RDONLY|O_DIR);

  while ( (de=fs_readdir(d)) ) {
	for(int q=0;q<strlen(de->name);q++) {
	  if(de->name[q]==' '||de->name[q]=='\0') { vmend=q; break; }
	}
	if(de->name[0]!='.' &&
       	(de->name[vmend-3]=='D'||de->name[vmend-3]=='d')&&
       	(de->name[vmend-2]=='C'||de->name[vmend-2]=='c')&&
       	(de->name[vmend-1]=='B'||de->name[vmend-1]=='b')) {
  		strcpy(filenames[x],de->name);
	loc[x]=1;
        if(strlen(filenames[x])>maxlen) maxlen=strlen(filenames[x]);
        x++;
      }
    }
  fs_close(d);
#endif

  switch_tex(gamesel);
  for(alpha=0;alpha<1;alpha+=.05) {
		sys_render_begin();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		render_bg(alpha);
		sys_render_finish();
	}
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  glLoadIdentity();

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0,640,0,480,-1,1);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  while(1) {
    sys_render_begin();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
    switch_tex(gamesel);
    render_bg(1);
    glDisable(GL_TEXTURE_2D);
#ifndef DREAMCAST
    glEnable(GL_BLEND);			// Turn Blending On
    glDisable(GL_DEPTH_TEST);	// Turn Depth Testing Off
#else
    glKosFinishList();
#endif
    glLoadIdentity();
    glColor4f(0.1f,0.1f,0.1f,0.8f);
    glBegin(GL_QUADS);
    glVertex3f(220,(240-(x*24)/2)-3,0);
    glVertex3f(640-220,(240-(x*24)/2)-3,0);
    glVertex3f(640-220,(240-(x*24)/2)+(x*24)-3,0);
    glVertex3f(220,(240-(x*24)/2)+(x*24)-3,0);
    glEnd();
    glEnable(GL_TEXTURE_2D);
#ifndef DREAMCAST
		glDisable(GL_BLEND);			// Turn Blending On
		glEnable(GL_DEPTH_TEST);	// Turn Depth Testing Off
#endif
    glColor4f(1.0f,1.0f,1.0f,1.0f);

    for(i=0;i<x;i++) {
      draw_txt(240,(240-(x*24)/2)+((i+1)*24),filenames[i],1,1,1,1);
    }
    draw_txt(225,(240-(x*24)/2)+((sel+1)*24),">",1,1,1,1);
		sys_render_finish();
		switch(get_controls(0)) {
      case MOVE_DOWN:
        sel++;
        delay(0.1);
        break;
      case MOVE_UP:
        sel--;
        delay(0.1);
        break;
      case QUIT_BTN:
        return;
        break;
      case START_BTN:
      case FIRE_BTN:
#ifdef WIN32
	      sprintf(tmp,"addons\\%s\\",filenames[sel]);
#else
      	sprintf(tmp,"addons/%s/",filenames[sel]);
#endif
#ifdef DREAMCAST
      	if(loc[sel]==1) {
          sprintf(tmp,"/vmu/a1/%s",filenames[sel]);
          fd=fs_open(tmp,O_RDONLY);
          fs_read(fd,&vmuhdr,128);
          ftmp=(uint8 *)malloc(vmuhdr.data_len+1);
          fs_read(fd,ftmp,vmuhdr.data_len);
          fs_close(fd);
          fd=fs_open("/ram/tmp.gz",O_WRONLY);
          fs_write(fd,ftmp,vmuhdr.data_len);
          fs_close(fd);
          free(ftmp);
          fs=zlib_getlength("/ram/tmp.gz");
          rd2=(uint8 *)malloc(fs);
          f=gzopen("/ram/tmp.gz","rb");
          gzread(f,rd2,fs);
          gzclose(f);
          fs_unlink("/ram/tmp.gz");
          if(vmumount==1) fs_romdisk_unmount("/rd2");
          vmumount=1;
          fs_romdisk_mount("/rd2",rd2,1);
          set_texture_theme("/rd2/");
        } else {
          vmumount=0;
#endif
          if(x==0) {
            set_texture_theme("");
          } else {
	          set_texture_theme(tmp);        
          }
#ifdef DREAMCAST
        }
#endif
	      for(alpha=1;alpha>0;alpha-=.05) {
		      sys_render_begin();
		      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
          switch_tex(gamesel);
		      render_bg(alpha);
		      sys_render_finish();
    	  }
        clear_texture_cache();
        load_texture("loading",loading);
        for(i=0;i<5;i++) {
          if(sys_render_begin()) {
		        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            switch_tex(loading);
		        render_bg((float)i/5.0f);
            sys_render_finish();
          }
        }
        load_texture("title",titletex);
        load_texture("playerselect",playersel);
        load_texture("gamesel",gamesel);
        load_texture("options",options);
        load_texture("intro_stars",stars);
        load_texture("intro_logo",logo);
        load_texture("ss/1",1);
        load_texture("ss/2",2);
        load_texture("ss/3",3);
        load_texture("ss/4",4);
#ifdef DREAMCAST
  if(enable_music) { 
    sndoggvorbis_stop();
    sprintf(title,"/cd/%s/menu.ogg",tex_theme_dir);
    if(sndoggvorbis_start(title,1)<0) {
      sndoggvorbis_start("/cd/menu.ogg",1);
    }
  }
#endif
#ifdef WIN32
  if(enable_music) {
    stop_bgm();
    sprintf(tmp,"%s\\menu.mp3",tex_theme_dir);
    FILE *fd=fopen(tmp,"r");
    if(fd) {
      fclose(fd);
      play_bgm(A2W(tmp));
    } else {
      play_bgm(L"menu.ogg");
    }
  }
#endif

        return;
        break;
    }
    if(sel<0) sel=0;
    if(sel>=x) sel=x-1;
  }
}

void game_select() {
  int x=0,i;
  int maxlen=0;
	float alpha;
#ifdef WIN32
  DIR* de;
#endif
#ifdef DREAMCAST
  dirent_t *de;
  uint32 d;
  char fn[256];
#endif
#ifdef UNIX
  dirent *de;
  DIR *d;
#endif
  char filenames[15][100];
  int sel=0;
  player_select();

#ifdef DREAMCAST
  d = fs_open("/cd/maps",O_RDONLY|O_DIR);
#endif
#ifdef WIN32
  de = opendir("maps");
#endif
#ifdef LINUX
  d = opendir("maps");
#endif

#ifdef DREAMCAST
  while ( (de=fs_readdir(d)) ) {
#endif
#ifdef WIN32
  while ( (de=readdir(de)) ) {
#endif
#ifdef LINUX
  while ( (de=readdir(d)) ) {
#endif

#ifdef LINUX
    if(de->d_name[0]!='.') {
  	  strcpy(filenames[x],de->d_name);
#else
    if(de->name[0]!='.') {
  	  strcpy(filenames[x],de->name);
#endif
      if(strlen(filenames[x])>maxlen) maxlen=strlen(filenames[x]);
      x++;
	  }
    }

#ifdef DREAMCAST
  fs_close(d);
#endif
#ifdef WIN32
  closedir(de);
#endif
#ifdef LINUX
  closedir(d);
#endif

  switch_tex(gamesel);
  for(alpha=0;alpha<1;alpha+=.05) {
		sys_render_begin();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		render_bg(alpha);
		sys_render_finish();
	}
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  glLoadIdentity();

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0,640,0,480,-1,1);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  while(1) {
		sys_render_begin();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
    switch_tex(gamesel);
    render_bg(1);
    glDisable(GL_TEXTURE_2D);
#ifndef DREAMCAST
    glEnable(GL_BLEND);			// Turn Blending On
		glDisable(GL_DEPTH_TEST);	// Turn Depth Testing Off
#else
    glKosFinishList();
#endif
    glColor4f(0.1f,0.1f,0.1f,0.8f);
    glBegin(GL_QUADS);
    glVertex3f(220,(240-(x*24)/2)-3,0);
    glVertex3f(640-220,(240-(x*24)/2)-3,0);
    glVertex3f(640-220,(240-(x*24)/2)+(x*24)-3,0);
    glVertex3f(220,(240-(x*24)/2)+(x*24)-3,0);
    glEnd();
    glEnable(GL_TEXTURE_2D);
#ifndef DREAMCAST
		glDisable(GL_BLEND);			// Turn Blending On
		glEnable(GL_DEPTH_TEST);	// Turn Depth Testing Off
#endif
    glColor4f(1.0f,1.0f,1.0f,1.0f);

    for(i=0;i<x;i++) {
      draw_txt(240,(240-(x*24)/2)+((i+1)*24),filenames[i],1,1,1,1);
    }
    draw_txt(225,(240-(x*24)/2)+((sel+1)*24),">",1,1,1,1);
		sys_render_finish();
		switch(get_controls(0)) {
      case MOVE_DOWN:
        sel++;
        delay(0.1);
        break;
      case MOVE_UP:
        sel--;
        delay(0.1);
        break;
      case QUIT_BTN:
        return;
        break;
      case START_BTN:
      case FIRE_BTN:
				for(alpha=1;alpha>0;alpha-=.05) {
					sys_render_begin();
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
          switch_tex(gamesel);
					render_bg(alpha);
					sys_render_finish();
				}
        set_world_path(filenames[sel]);
        level_select(filenames[sel]);
        return;
        break;
    }
    if(sel<0) sel=0;
    if(sel>=x) sel=x-1;
  }
}

void level_select(char *path) {
  int x=0,i;
  int maxlen=0;
	float alpha;
#ifdef WIN32
  DIR* de;
  USES_CONVERSION;
#endif
#ifdef DREAMCAST
  dirent_t *de;
  uint32 d;
  char fn[256];
#endif
#ifdef LINUX
  dirent *de;
  DIR *d;
#endif
  char filenames[15][100];
  int sel=0;
  char tmp[100];

#ifdef DREAMCAST
  sprintf(tmp,"maps/%s",path);
  d = fs_open(tmp,O_RDONLY|O_DIR);
#endif
#ifdef WIN32
  sprintf(tmp,"maps/%s",path);
  de = opendir(tmp);
#endif
#ifdef LINUX
  sprintf(tmp,"maps/%s",path);
  d = opendir(tmp);
#endif

#ifdef DREAMCAST
  while ( (de=fs_readdir(d)) ) {
#endif
#ifdef WIN32
  while ( (de=readdir(de)) ) {
#endif
#ifdef LINUX
  while ( (de=readdir(d)) ) {
#endif

#ifdef LINUX
    if((de->d_name[strlen(de->d_name)-3]=='w' || de->d_name[strlen(de->d_name)-3]=='w') &&
       (de->d_name[strlen(de->d_name)-2]=='l' || de->d_name[strlen(de->d_name)-2]=='l') &&
       (de->d_name[strlen(de->d_name)-1]=='d' || de->d_name[strlen(de->d_name)-1]=='d')) {
  		strcpy(filenames[x],de->d_name);
#else
    if((de->name[strlen(de->name)-3]=='W' || de->name[strlen(de->name)-3]=='w') &&
       (de->name[strlen(de->name)-2]=='L' || de->name[strlen(de->name)-2]=='l') &&
       (de->name[strlen(de->name)-1]=='D' || de->name[strlen(de->name)-1]=='d')) {
  		strcpy(filenames[x],de->name);
#endif
      if(strlen(filenames[x])>maxlen) maxlen=strlen(filenames[x]);
      x++;
    }
	}

#ifdef DREAMCAST
  fs_close(d);
#endif
#ifdef WIN32
  closedir(de);
#endif
#ifdef LINUX
  closedir(d);
#endif

  switch_tex(gamesel);
  for(alpha=0;alpha<1;alpha+=.05) {
		sys_render_begin();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		render_bg(alpha);
		sys_render_finish();
	}
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  glLoadIdentity();

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0,640,0,480,-1,1);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  while(1) {
		sys_render_begin();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
    switch_tex(gamesel);
    render_bg(1);
    glDisable(GL_TEXTURE_2D);
#ifndef DREAMCAST
    glEnable(GL_BLEND);			// Turn Blending On
		glDisable(GL_DEPTH_TEST);	// Turn Depth Testing Off
#else
    glKosFinishList();
#endif
    glColor4f(0.1f,0.1f,0.1f,0.8f);
    glBegin(GL_QUADS);
    glVertex3f(220,(240-(x*24)/2)-3,0);
    glVertex3f(640-220,(240-(x*24)/2)-3,0);
    glVertex3f(640-220,(240-(x*24)/2)+(x*24)-3,0);
    glVertex3f(220,(240-(x*24)/2)+(x*24)-3,0);
    glEnd();
    glEnable(GL_TEXTURE_2D);
#ifndef DREAMCAST
		glDisable(GL_BLEND);			// Turn Blending On
		glEnable(GL_DEPTH_TEST);	// Turn Depth Testing Off
#endif
    glColor4f(1.0f,1.0f,1.0f,1.0f);

    for(i=0;i<x;i++) {
      draw_txt(240,(240-(x*24)/2)+((i+1)*24),filenames[i],1,1,1,1);
    }
    draw_txt(225,(240-(x*24)/2)+((sel+1)*24),">",1,1,1,1);
		sys_render_finish();
		switch(get_controls(0)) {
      case MOVE_DOWN:
        sel++;
        delay(0.1);
        break;
      case MOVE_UP:
        sel--;
        delay(0.1);
        break;
      case QUIT_BTN:
        return;
        break;
      case START_BTN:
      case FIRE_BTN:
#ifdef WIN32
		stop_bgm();
#endif
#ifdef LINUX
		Mix_HaltMusic();
		Mix_FreeMusic(bgm);
#endif
#ifdef DREAMCAST
    sndoggvorbis_stop();
#endif
				for(alpha=1;alpha>0;alpha-=.05) {
					sys_render_begin();
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
          switch_tex(gamesel);
					render_bg(alpha);
					sys_render_finish();
				}

#ifdef DREAMCAST
        strcpy(fn,fs_getwd());
#endif
        set_world_path(path);
        strcpy(next_map,filenames[sel]);
        while(strcmp(next_map,"none")) {
          for(i=0;i<5;i++) {
            if(sys_render_begin()) {
		          glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
              switch_tex(loading);
		          render_bg((float)i/5.0f);
              sys_render_finish();
            }
          }
          glMatrixMode(GL_PROJECTION);
          glPopMatrix();
          glMatrixMode(GL_MODELVIEW);
          glPopMatrix();
          glLoadIdentity();

          load_world(next_map);
          strcpy(next_map,"none");
          play_pong();
          
          load_texture("loading",loading);
          glMatrixMode(GL_PROJECTION);
          glPushMatrix();
          glLoadIdentity();
          glOrtho(0,640,0,480,-1,1);
          glMatrixMode(GL_MODELVIEW);
          glPushMatrix();
          glLoadIdentity();
        }

        for(i=0;i<5;i++) {
          if(sys_render_begin()) {
		        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            switch_tex(loading);
		        render_bg((float)i/5.0f);
            sys_render_finish();
          }
        }

        load_texture("title",titletex);
        load_texture("playerselect",playersel);
        load_texture("gamesel",gamesel);
        load_texture("options",options);
        load_texture("intro_stars",stars);
        load_texture("intro_logo",logo);
        load_texture("ss/1",1);
        load_texture("ss/2",2);
        load_texture("ss/3",3);
        load_texture("ss/4",4);

#ifdef DREAMCAST
        if(enable_music) {
          sndoggvorbis_stop();
	        sndoggvorbis_start("/cd/menu.ogg",1);
        }
#endif
#ifdef WIN32
  if(enable_music) {
    sprintf(tmp,"%s\\menu.mp3",tex_theme_dir);
    FILE *fd=fopen(tmp,"r");
    if(fd) {
      fclose(fd);
      play_bgm(A2W(tmp));
    } else {
      play_bgm(L"menu.ogg");
    }
  }
#endif
#ifdef LINUX
  bgm=Mix_LoadMUS("menu.mp3");
  if(!bgm) { printf("BGM load failed: %s\n",Mix_GetError()); }
  if(enable_music && bgm && Mix_PlayMusic(bgm,-1)==-1) {
    printf("BGM play failed: %s\n",Mix_GetError());
  }
#endif
        
return;
        break;
    }
    if(sel<0) sel=0;
    if(sel>=x) sel=x-1;
  }
}

void do_credits() {
  int i=0;
  float alpha;
  char tmp[100];
#ifdef WIN32
  USES_CONVERSION;
  stop_bgm();
#endif
#ifdef DREAMCAST
  sndoggvorbis_stop();
#endif
  for(i=0;i<5;i++) {
    if(sys_render_begin()) {
		  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      switch_tex(loading);
		  render_bg((float)i/5.0f);
      sys_render_finish();
    }
  }
  clear_texture_cache();
#ifdef WIN32
  if(enable_music) play_bgm(L"credits.ogg");
#endif
#ifdef DREAMCAST
  copy_file("/cd/credits.ogg","/ram/credits.ogg");
  if(enable_music) sndoggvorbis_start("/ram/credits.ogg",1);
#endif
  for(i=1;i<10;i++) {
    sprintf(tmp,"credits/%i",i);
    load_texture(tmp,0);
    switch_tex(0);
	  for(alpha=0;alpha<1;alpha+=.02) {
		  sys_render_begin();
		  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		  render_bg(alpha);
		  sys_render_finish();
	  }
    delay(3);  
	  for(alpha=1;alpha>0;alpha-=.02) {
		  sys_render_begin();
		  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		  render_bg(alpha);
		  sys_render_finish();
	  }
  }
  load_texture("loading",loading);
  for(i=0;i<5;i++) {
    if(sys_render_begin()) {
		  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      switch_tex(loading);
		  render_bg((float)i/5.0f);
      sys_render_finish();
    }
  }
#ifdef DREAMCAST
  sndoggvorbis_stop();
#endif
#ifdef WIN32
  stop_bgm();
#endif
  clear_texture_cache();
  load_texture("title",titletex);
  load_texture("playerselect",playersel);
  load_texture("gamesel",gamesel);
  load_texture("options",options);
  load_texture("intro_stars",stars);
  load_texture("intro_logo",logo);
  load_texture("ss/1",1);
  load_texture("ss/2",2);
  load_texture("ss/3",3);
  load_texture("ss/4",4);
#ifdef WIN32
  if(enable_music) {
    sprintf(tmp,"%s\\menu.ogg",tex_theme_dir);
    FILE *fd=fopen(tmp,"r");
    if(fd) {
      fclose(fd);
      play_bgm(A2W(tmp));
    } else {
      play_bgm(L"menu.ogg");
    }
  }
#endif
#ifdef DREAMCAST
  if (enable_music) sndoggvorbis_start("/cd/menu.ogg",1);
#endif
}

void do_intro() {
  int x;
  float alpha;
  int step=1;
  //return;
  glBlendFunc(GL_SRC_ALPHA,GL_ONE);
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
	for(alpha=0;alpha<1;alpha+=.02) {
    if(sys_render_begin()) {
		  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      switch_tex(stars);
      glLoadIdentity();
		  render_bg(alpha);
      sys_render_finish();
    }
  }
  for(x=-100;x<200;x+=2) {
    if(sys_render_begin()) {
		  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      switch_tex(stars);
      glLoadIdentity();
		  render_bg(1);
      switch_tex(logo);
      glTranslatef(320,480-x,0);
      glRotatef(((float)x/200.0f)*360.0f,0,0,1);
#ifdef DREAMCAST
      glKosFinishList();
#endif
      glBegin(GL_QUADS);
#ifdef WIN32
      glTexCoord2f(0,0); glVertex3f(-100,-40,0);
      glTexCoord2f(1,0); glVertex3f(100,-40,0);
      glTexCoord2f(1,1); glVertex3f(100,40,0);
      glTexCoord2f(0,1); glVertex3f(-100,40,0);
#else
      glTexCoord2f(0,1); glVertex3f(-100,-40,0);
      glTexCoord2f(1,1); glVertex3f(100,-40,0);
      glTexCoord2f(1,0); glVertex3f(100,40,0);
      glTexCoord2f(0,0); glVertex3f(-100,40,0);
#endif
      glEnd();
  		sys_render_finish();
    }
	}
  glLoadIdentity();
  x=1;
  step=1;
  for(alpha=0;alpha<1;alpha+=.02) {
    x+=step;
    if(alpha>.5) step=-1;
    if(sys_render_begin()) {
		  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glColor4f(1,1,1,1);
      switch_tex(stars);
      glLoadIdentity();
		  render_bg(1);
      switch_tex(logo);
      glTranslatef(320,280,0);
#ifdef DREAMCAST
      glKosFinishList();
#endif
#ifdef WIN32
      glEnable(GL_BLEND);
#endif
      glBegin(GL_QUADS);
#ifdef WIN32
      glTexCoord2f(0,0); glVertex3f(-100-x,-40,0);
      glTexCoord2f(1,0); glVertex3f(100+x,-40,0);
      glTexCoord2f(1,1); glVertex3f(100+x,40-x,0);
      glTexCoord2f(0,1); glVertex3f(-100-x,40-x,0);
#else
      glTexCoord2f(0,1); glVertex3f(-100-x,-40,0);
      glTexCoord2f(1,1); glVertex3f(100+x,-40,0);
      glTexCoord2f(1,0); glVertex3f(100+x,40-x,0);
      glTexCoord2f(0,0); glVertex3f(-100-x,40-x,0);
#endif
      glEnd();
      glLoadIdentity();
      draw_txt(320-(txt_width("Presents")/2),280,"Presents",1,1,1,alpha);
  		sys_render_finish();
    }
	}

  for(x=0;x<5;x++) {
    if(sys_render_begin()) {
		  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glColor4f(1,1,1,1);
      switch_tex(stars);
      glLoadIdentity();
		  render_bg(1);
      switch_tex(logo);
      glTranslatef(320,280,0);
#ifdef DREAMCAST
      glKosFinishList();
#endif
#ifdef WIN32
      glEnable(GL_BLEND);
#endif
      glBegin(GL_QUADS);
#ifdef WIN32
      glTexCoord2f(0,0); glVertex3f(-100,-40,0);
      glTexCoord2f(1,0); glVertex3f(100,-40,0);
      glTexCoord2f(1,1); glVertex3f(100,40,0);
      glTexCoord2f(0,1); glVertex3f(-100,40,0);
#else
      glTexCoord2f(0,1); glVertex3f(-100,-40,0);
      glTexCoord2f(1,1); glVertex3f(100,-40,0);
      glTexCoord2f(1,0); glVertex3f(100,40,0);
      glTexCoord2f(0,0); glVertex3f(-100,40,0);
#endif
      glEnd();
      draw_txt(320-(txt_width("Presents")/2),280,"Presents",1,1,1,1);
  		sys_render_finish();
    }
	}
  delay(2);
	for(alpha=1;alpha>0;alpha-=.02) {
    if(sys_render_begin()) {
		  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      switch_tex(stars);
      glLoadIdentity();
		  render_bg(alpha);
      glColor3f(alpha,alpha,alpha);
      switch_tex(logo);
      glTranslatef(320,280,0);
#ifdef DREAMCAST
      glKosFinishList();
#endif
#ifdef WIN32
      glEnable(GL_BLEND);
#endif
      glBegin(GL_QUADS);
#ifdef WIN32
      glTexCoord2f(0,0); glVertex3f(-100,-40,0);
      glTexCoord2f(1,0); glVertex3f(100,-40,0);
      glTexCoord2f(1,1); glVertex3f(100,40,0);
      glTexCoord2f(0,1); glVertex3f(-100,40,0);
#else
      glTexCoord2f(0,1); glVertex3f(-100,-40,0);
      glTexCoord2f(1,1); glVertex3f(100,-40,0);
      glTexCoord2f(1,0); glVertex3f(100,40,0);
      glTexCoord2f(0,0); glVertex3f(-100,40,0);
#endif
      glEnd();
      draw_txt(320-(txt_width("Presents")/2),280,"Presents",1,1,1,alpha);
  		sys_render_finish();
    }
	}
  glDisable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  glLoadIdentity();
}

void main_menu() {
#ifdef WIN32
  USES_CONVERSION;
#endif
  float alpha=0;
  int sel=0;
  int menuactive=0;
  int x=0;
  float a=1,astep=-.01;
  int menubgtex=0;
  char title[100];

  debug("Putting OpenGL in 2D mode");
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0,640,0,480,-1,1);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  menubgtex=17;
#ifdef DREAMCAST
  load_texture("disclaimer",menubgtex);
#else
  load_texture("loading",menubgtex);
#endif

  for(alpha=0;alpha<1;alpha+=.02) {
    if(sys_render_begin()) {
		  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		  render_bg(alpha);
		  sys_render_finish();
    }
	}

  if(randtitle) {
    sprintf(title,"rt/%i",(rand()%13)+1);
    load_texture(title,titletex);
  } else {
    load_texture("title",titletex);
  }
  load_texture("playerselect",playersel);
  load_texture("gamesel",gamesel);
  load_texture("options",options);
  load_texture("loading",loading);
  load_texture("intro_stars",stars);
  load_texture("intro_logo",logo);
  load_texture("ss/1",1);
  load_texture("ss/2",2);
  load_texture("ss/3",3);
  load_texture("ss/4",4);

#ifdef DREAMCAST
  copy_file("/cd/hyper.jpg","/ram/hyper.jpg");
  copy_file("/cd/hyper.md2","/ram/hyper.md2");
  copy_file("/cd/speedup.jpg","/ram/speedup.jpg");
  copy_file("/cd/speedup.md2","/ram/speedup.md2");
  copy_file("/cd/slowdown.jpg","/ram/slowdown.jpg");
  copy_file("/cd/slowdown.md2","/ram/slowdown.md2");
#endif

  
  switch_tex(menubgtex);
  
	for(alpha=1;alpha>0;alpha-=.02) {
    if(sys_render_begin()) {
		  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		  render_bg(alpha);
		  sys_render_finish();
    }
	}

#ifdef LINUX
  bgm=Mix_LoadMUS("menu.mp3");
  if(!bgm) { printf("BGM load failed: %s\n",Mix_GetError()); }
  if(enable_music && bgm && Mix_PlayMusic(bgm,-1)==-1) {
    printf("BGM play failed: %s\n",Mix_GetError());
  }
#endif
#ifdef DREAMCAST
  if(enable_music) { 
    sndoggvorbis_stop();
    sprintf(title,"/cd/%s/menu.ogg",tex_theme_dir);
    if(sndoggvorbis_start(title,1)<0) {
      sndoggvorbis_start("/cd/menu.ogg",1);
    }
  }
#endif
#ifdef WIN32
  if(enable_music) {
    debug("Starting background music");
    sprintf(title,"%s\\menu.ogg",tex_theme_dir);
    FILE *fd=fopen(title,"r");
    if(fd) {
      fclose(fd);
      play_bgm(A2W(title));
    } else {
      play_bgm(L"menu.ogg");
    }
  }
#endif

  do_intro();

  debug("Activating title texture");
  switch_tex(titletex);

  debug("Fading in title screen");
	for(alpha=0;alpha<1;alpha+=.05) {
		sys_render_begin();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		render_bg(alpha);
		sys_render_finish();
	}
  debug("Menu is ready!");
  while(1) {
		switch(get_controls(0)) {
		case QUIT_BTN:
			sel=0;
      if(menuactive) menuactive=0;// else return;
			break;
		case START_BTN:
			while(get_controls(0)==START_BTN);
      if(menuactive) {
        switch_tex(titletex);
				for(alpha=1;alpha>0;alpha-=.05) {
					sys_render_begin();
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
					render_bg(alpha);
					sys_render_finish();
				}
        menuactive=0;
        if(sel==0) { game_select(); }
        if(sel==1) { do_options(); }
      	if(sel==2) { do_addons(); }
        if(sel==3) { do_credits(); }
				if(sel==4) return;
        if(sel!=1) do_intro();
				sel=0;
        if(randtitle) {
          sprintf(title,"rt/%i",(rand()%13)+1);
          load_texture(title,titletex);
        }
        switch_tex(titletex);
        for(alpha=0;alpha<1;alpha+=.05) {
					sys_render_begin();
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
					render_bg(alpha);
					sys_render_finish();
				}
      } else {
				menuactive=1;
			}
			break;
		case MOVE_DOWN:
      sel++;
      if(sel>4) sel=4;
      delay(.1);
			break;
		case MOVE_UP:
      sel--;
      if(sel<0) sel=0;
      delay(.1);
      break;
		}
		sys_render_begin();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
    switch_tex(titletex);
    render_bg(1);
#ifdef DREAMCAST
		glKosFinishList();
#endif
		if(menuactive) {
			glDisable(GL_TEXTURE_2D);
#ifndef DREAMCAST
  		glEnable(GL_BLEND);			// Turn Blending On
			glDisable(GL_DEPTH_TEST);	// Turn Depth Testing Off
#endif
			glColor4f(0.1f,0.1f,0.1f,0.8f);
			glBegin(GL_QUADS);
			glVertex3f(220,210-56,0);
			glVertex3f(640-220,210-56,0);
			glVertex3f(640-220,210+50,0);
			glVertex3f(220,210+50,0);
			glEnd();
			glEnable(GL_TEXTURE_2D);
#ifndef DREAMCAST
			glDisable(GL_BLEND);			// Turn Blending On
			glEnable(GL_DEPTH_TEST);	// Turn Depth Testing Off
#endif
			draw_txt(225,242,"  New Game",1,1,1,1);
			draw_txt(225,262,"  Options",1,1,1,1);
			draw_txt(225,282,"  Addons",1,1,1,1);
			draw_txt(225,302,"  Credits",1,1,1,1);
			draw_txt(225,322,"  Quit",1,1,1,1);
			draw_txt(225,242+(sel*20),">",1,0,0,1);
		} else {
			draw_txt((640/2)-(txt_width("Press Start")/2),262,"Press Start",1,1,1,a);
		}
		a+=astep;
		if(a<0.4) { a=0.4; astep=.01;}
		if(a>1) { a=1; astep=-.01;}
		sys_render_finish();
		delay(0.01);
	}
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  glLoadIdentity();
}
