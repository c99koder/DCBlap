/* main.cc - DCBlap main
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
#include <Tiki/gl.h>
#include <Tiki/hid.h>
#include <Tiki/tikitime.h>
#include <Tiki/oggvorbis.h>
#include <Tiki/genmenu.h>
#include <Tiki/drawables/banner.h>

using namespace Tiki;
using namespace Tiki::GL;
using namespace Tiki::Hid;
using namespace Tiki::Audio;
using namespace Tiki::Time;

#include "entity.h"
#include "game.h"
#include "hud.h"
#include "objects.h"
#include "menus/TitleScreen.h"
#include "menus/GameSelect.h"
#include "menus/LevelSelect.h"

bool quitting = false;
bool playing = false;

int player_axis_x[4];
int player_axis_y[4];

extern bool win_flag;
extern bool lose_flag;
extern bool game_paused;

void tkCallback(const Hid::Event & evt, void * data) {
	if (evt.type == Hid::Event::EvtQuit) {
		quitting = true;
		playing=false;
	}
	if (evt.type == Hid::Event::EvtAttach) {
		printf("Device attached: %s\n",evt.dev->getName().c_str());
	}
	if (evt.type == Hid::Event::EvtDetach) {
		printf("Device detached: %s\n",evt.dev->getName().c_str());
	}
	if(evt.type == Hid::Event::EvtAxis) {
		printf("Axis %i moved to %f on port %i\n",evt.axis,evt.axisValue,evt.port);
	}
	if (evt.type == Hid::Event::EvtKeyDown) {
		switch(evt.key) {
			case Hid::Event::KeyLeft:
				player_axis_x[0]=-1;
				break;
			case Hid::Event::KeyRight:
				player_axis_x[0]=1;
				break;
			case Hid::Event::KeyUp:
				player_axis_y[0]=-1;
				break;
			case Hid::Event::KeyDown:
				player_axis_y[0]=1;
				break;
			case Hid::Event::KeyEsc:
				playing = false;
				break;
		}
	}
	if(evt.type == Hid::Event::EvtKeypress) {
		if(evt.key == 13) {
			if(win_flag || lose_flag) {
				playing = false;
			} else {
				if(game_paused) {
					pause_world(0);
					set_hud(3,320-(txt_width("Paused")/2),240-16,"",1,1,1);
					game_paused=0;
				} else {
					pause_world(1);
					set_hud(3,320-(txt_width("Paused")/2),240-16,"Paused",1,1,1);
					game_paused=1;
				}
			}
		}
	}
	if(evt.type == Hid::Event::EvtKeyUp) {
		if(evt.key == Hid::Event::KeyLeft || evt.key == Hid::Event::KeyRight) {
			player_axis_x[0]=0;
		}
		if(evt.key == Hid::Event::KeyUp || evt.key == Hid::Event::KeyDown) {
			player_axis_y[0]=0;
		}
	}
#if TIKI_PLAT == TIKI_DC
	if(evt.type == Hid::Event::EvtBtnRelease && evt.btn == Hid::Event::BtnY)
		vid_screen_shot("/pc/Users/sam/sshot.ppm");
#endif
}
		
extern "C" int tiki_main(int argc, char **argv) {
	uint64 gt=0;
	uint64 ogt=0;
	uint64 st=0;
	TitleScreen *ts;
	GameSelect *gs;
	LevelSelect *ls;
	Banner *loading;
	VorbisStream bgm;
	char tmp[100];
	
	// Init Tiki
	Tiki::init(argc, argv);
	Hid::callbackReg(tkCallback, NULL);
	//Tiki::GL::showCursor(false);
	
	loading = new Banner(Drawable::Opaque,new Texture("tex/loading.png",0));
	loading->setSize(640,480);
	loading->setTranslate(Vector(320,240,10));
	
	for(int i=0; i<3; i++) {
		Frame::begin();
		loading->draw(Drawable::Opaque);
		Frame::finish();
	}

	objects_init();
	hud_init();
	player_axis_x[0]=0;

	srand(gettime());
	
	ts=new TitleScreen;
	gs=new GameSelect;
	
	while(quitting==false) {
		bgm.stop();
		bgm.load("sound/menu.ogg",1);
		bgm.start();
		ts->FadeIn();
		ts->doMenu();
		if(quitting) break;
		switch(ts->getSelection()) {
			case 0:
				gs->FadeIn();
				gs->doMenu();
				if(quitting) break;
				sprintf(tmp,"maps/%s",gs->getSelection().c_str());
				ls=new LevelSelect(tmp);
				ls->FadeIn();
				ls->doMenu();
				bgm.stop();
				for(int i=0; i<3; i++) {
					Frame::begin();
					loading->draw(Drawable::Opaque);
					Frame::finish();
				}				
				game_init(ls->getSelection().c_str());
				delete ls;
				bgm.load("sound/game.ogg",1);
				bgm.start();

				playing=true;
				
				st = gettime();
				ogt=0;
				
				while (playing==true) {
					gt=gettime() - st;
					update_world(float(gt-ogt)/1000000.0f);
					Frame::begin();
					glClearDepth(1.0f);
					glDepthFunc(GL_LEQUAL);
					glClear(GL_COLOR_BUFFER_BIT+GL_DEPTH_BUFFER_BIT);
					glColor4f(1,1,1,1);
					Frame::set3d();
					render_world(true);
					Frame::transEnable();
					render_world(false);
					Frame::set2d();
					render_HUD();
					Frame::finish();
					ogt=gt;
				}
					
				destroy_world();
				break;
			case 3:
				quitting=true;
				break;
		}
	}
	
	bgm.stop();
	
	delete gs;
	delete ts;
	
	// Shut down Tiki
	Tiki::shutdown();
	
	return 0;
}

