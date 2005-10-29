/* main.cc - DCBlap main
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
#include "texture.h"
#include "game.h"
#include "hud.h"
#include "objects.h"
#include "menus.h"

volatile bool quitting = false;
void tkCallback(const Hid::Event & evt, void * data) {
	if (evt.type == Hid::Event::EvtQuit) {
		quitting = true;
	}
}

extern "C" int tiki_main(int argc, char **argv) {
	TitleScreen t;
	// Init Tiki
	Tiki::init(argc, argv);
	Hid::callbackReg(tkCallback, NULL);
	
	//Music
	Tiki::Audio::VorbisStream bgm;
	bgm.load("game.ogg", false);
	bgm.start();
	
	objects_init();

	t.doMenu();
	
	hud_init();
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
	bgm.stop();

	Tiki::shutdown();
	
	return 0;
}

