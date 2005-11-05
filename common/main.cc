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

volatile bool quitting = false;

int player_axis_x[4];

void tkCallback(const Hid::Event & evt, void * data) {
	if (evt.type == Hid::Event::EvtQuit) {
		quitting = true;
	}
	if (evt.type == Hid::Event::EvtKeyDown) {
		switch(evt.key) {
			case Hid::Event::KeyLeft:
				player_axis_x[0]=-1;
				break;
			case Hid::Event::KeyRight:
				player_axis_x[0]=1;
				break;
		}
	}
	if(evt.type == Hid::Event::EvtKeyUp) {
		if(evt.key == Hid::Event::KeyLeft || evt.key == Hid::Event::KeyRight) {
			player_axis_x[0]=0;
		}
	}
}
		
extern "C" int tiki_main(int argc, char **argv) {
	TitleScreen *t;
	// Init Tiki
	Tiki::init(argc, argv);
	Hid::callbackReg(tkCallback, NULL);
	
	objects_init();
	hud_init();
	player_axis_x[0]=0;

	t=new TitleScreen;
	t->doMenu();
	
	game_init("BlapOut/classic.wld");
	update_world(0);
	
	while (!quitting) {
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

	// Shut down Tiki
	Tiki::shutdown();
	
	return 0;
}

