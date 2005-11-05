/* TitleScreen.h - Title Screen menu
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
#include "Tiki/anims/tintfader.h"

using namespace Tiki;
using namespace Tiki::GL;

#include "TitleScreen.h"

extern Font *fnt;

TitleScreen::TitleScreen() {
	GenericMenu::GenericMenu();
	
	setBgm("menu.ogg",0);

	bg = new Banner(Drawable::Opaque,new Texture("title.png",0));
	bg->setSize(640,480);
	bg->setTranslate(Vector(320,240,0));
	bg->setTint(Color(0,0,0));
	bg->animAdd(new TintFader(Color(1,1,1),Color(1.0f/60.0f,1.0f/60.0f,1.0f/60.0f)));
	m_scene->subAdd(bg);
	
	ml = new menuList(new Texture("title_box.png",0), new Texture("title_bar.png",0), fnt);
	ml->setSize(120,86);
	ml->setTranslate(Vector(320,240,-0.1));
	ml->addItem("New Game");
	ml->addItem("Addons");
	ml->addItem("Options");
	ml->addItem("Quit");
	ml->selectItem(0);
	ml->setTint(Color(0,0,0,0));
	ml->animAdd(new TintFader(Color(1,1,1,1),Color(1.0f/60.0f,1.0f/60.0f,1.0f/60.0f,1.0f/60.0f)));
	m_scene->subAdd(ml);
	
	m_selection=0;
	m_menuItems=4;
	m_repeatDelay=0;
}

void TitleScreen::inputEvent(const Event & evt) {
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
					bg->animAdd(new TintFader(Color(0,0,0),Color(-1.0f/60.0f,-1.0f/60.0f,-1.0f/60.0f)));
					ml->animAdd(new TintFader(Color(0,0,0,0),Color(-1.0f/60.0f,-1.0f/60.0f,-1.0f/60.0f,-1.0f/60.0f)));
					startExit();
					break;
				case Event::KeyUp:
					if(m_repeatDelay==0) {
						m_selection--;
						if(m_selection<0) {
							m_selection=0;
						} else {
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
							ml->selectItem(m_selection);
							m_repeatDelay=2;
						}
					} else {
						m_repeatDelay--;
					}
					break;
				default:
					printf("Key: %c\n",evt.key);
			}
			break;
	}
}