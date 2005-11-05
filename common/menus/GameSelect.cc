/* GameSelect.cc - Game Selection menu
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

#include "Tiki/tiki.h"
#include "Tiki/genmenu.h"
#include "Tiki/texture.h"
#include "Tiki/drawables/banner.h"

using namespace Tiki;
using namespace Tiki::GL;

#include "GameSelect.h"

extern Font *fnt;

GameSelect::GameSelect() {
	Texture *tex;
	Banner *ban;
	GenericMenu::GenericMenu();
	
	tex = new Texture("game_bg.png",1);
	ban = new Banner(Drawable::Trans,tex);
	is = new imageSet;
	ml = new menuList(new Texture("game_box.png",1), new Texture("game_bar.png",1), fnt);
	ban->setSize(640,480);
	ban->setTranslate(Vector(320,240,0));
	m_scene->subAdd(ban);
	
	is->setSize(240,200);
	ml->setSize(240,240);
	is->setTranslate(Vector(160,200,0));
	ml->setTranslate(Vector(480,220,0));
	is->addTexture(new Texture("maps/BlapOut/classic.png",1));
	ml->addItem("Classic");
	is->addTexture(new Texture("maps/BlapOut/buggy.png",1));
	ml->addItem("Buggy");
	is->addTexture(new Texture("maps/BlapOut/blapberry.png",1));
	ml->addItem("Blapberry Pie");
	is->selectTexture(0);
	ml->selectItem(0);
	m_scene->subAdd(is);
	m_scene->subAdd(ml);
	
	m_selection=0;
	m_menuItems=3;
	m_repeatDelay=0;
}

void GameSelect::inputEvent(const Event & evt) {
	switch(evt.type) {
		case Event::EvtQuit:
			quitNow();
			break;
		case Event::EvtKeyUp:
			m_repeatDelay=0;
			break;
		case Event::EvtKeypress:
			switch(evt.key) {
				case Event::KeySelect:
					startExit();
					break;
				case Event::KeyUp:
					if(m_repeatDelay==0) {
						m_selection--;
						if(m_selection<0) {
							m_selection=0;
						} else {
							is->selectTexture(m_selection);
							ml->selectItem(m_selection);
							m_repeatDelay=2;
						}
					} else {
						m_repeatDelay--;
					}
					break;
				case Event::KeyDown:
					if(m_repeatDelay==0) {
						m_selection++;
						if(m_selection>=m_menuItems) {
							m_selection=m_menuItems-1;
						} else {
							is->selectTexture(m_selection);
							ml->selectItem(m_selection);
							m_repeatDelay=2;
						}
					} else {
						m_repeatDelay--;
					}
					break;
			}
			break;
	}
}