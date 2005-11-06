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

#ifdef TIKI
#include <Tiki/tiki.h>
#include <Tiki/plxcompat.h>
#include <Tiki/gl.h>
#include <Tiki/hid.h>
#include <Tiki/oggvorbis.h>
#include <Tiki/genmenu.h>
#include <Tiki/drawables/banner.h>

using namespace Tiki;
using namespace Tiki::Math;
using namespace Tiki::GL;
using namespace Tiki::GL::Plxcompat;
using namespace Tiki::Hid;
using namespace Tiki::Audio;
#endif

#include "entity.h"
#include "game.h"
#include "hud.h"
#include "objects.h"
#include "TitleScreen.h"
#include "GameSelect.h"

bool quitting = false;
bool playing = false;

int player_axis_x[4];
int player_axis_y[4];

void tkCallback(const Hid::Event & evt, void * data) {
	if (evt.type == Hid::Event::EvtQuit) {
		quitting = true;
		playing=false;
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
			case 27:
				playing = false;
				break;
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
}
		
extern "C" int tiki_main(int argc, char **argv) {
	TitleScreen *ts;
	GameSelect *gs;
	Banner *loading;
	VorbisStream bgm;
	
	// Init Tiki
	Tiki::init(argc, argv);
	Hid::callbackReg(tkCallback, NULL);
	
	loading = new Banner(Drawable::Opaque,new Texture("loading.png",0));
	loading->setSize(640,480);
	loading->setTranslate(Vector(320,240,0));
	
	Frame::begin();
	loading->draw(Drawable::Opaque);
	Frame::finish();
	
	objects_init();
	hud_init();
	player_axis_x[0]=0;

	srand(time(0));
	
	ts=new TitleScreen;
	gs=new GameSelect;

	while(quitting==false) {
		bgm.stop();
		bgm.load("menu.ogg",1);
		bgm.start();
		ts->FadeIn();
		ts->doMenu();
		if(quitting) break;
		switch(ts->getSelection()) {
			case 0:
				gs->FadeIn();
				gs->doMenu();
				if(quitting) break;
				
				bgm.stop();
				bgm.load("game.ogg",1);
				bgm.start();
				game_init(gs->getSelection().c_str());
				update_world(0);

				playing=true;
				while (playing==true) {
					update_world(0.01);
					Frame::begin();
					glClearDepth(1.0f);
					glDepthFunc(GL_LEQUAL);
					glClear(GL_COLOR_BUFFER_BIT+GL_DEPTH_BUFFER_BIT+GL_STENCIL_BUFFER_BIT);
					glColor4f(1,1,1,1);
					Frame::set3d();
					render_world_solid();
					Frame::transEnable();
					render_world_trans();
					Frame::set2d();
					render_HUD();
					Frame::finish();
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

