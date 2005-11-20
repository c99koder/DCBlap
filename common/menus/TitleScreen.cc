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
#include "Tiki/drawables/cursor.h"

using namespace Tiki;
using namespace Tiki::GL;

#include "TitleScreen.h"

extern RefPtr<Font> fnt;

TitleScreen::TitleScreen() {
	GenericMenu::GenericMenu();

	m_scene->setTranslate(Vector(0,0,0));
	
	bg = new imageSet;
	bg->addTexture(new Texture("title.png",0));
	/*bg->addTexture(new Texture("rt/1.png",0));
	bg->addTexture(new Texture("rt/2.png",0));
	bg->addTexture(new Texture("rt/3.png",0));
	bg->addTexture(new Texture("rt/4.png",0));
	bg->addTexture(new Texture("rt/5.png",0));
	bg->addTexture(new Texture("rt/6.png",0));
	bg->addTexture(new Texture("rt/7.png",0));
	bg->addTexture(new Texture("rt/8.png",0));
	bg->addTexture(new Texture("rt/9.png",0));
	bg->addTexture(new Texture("rt/10.png",0));
	bg->addTexture(new Texture("rt/11.png",0));
	bg->addTexture(new Texture("rt/12.png",0));
	bg->addTexture(new Texture("rt/13.png",0));*/
	bg->selectTexture(0/*rand()%14*/);
	bg->setSize(640,480);
	bg->setTranslate(Vector(320,240,0));
	m_scene->subAdd(bg);
	
	ml = new menuList(new Texture("title_box.png",0), new Texture("title_bar.png",0), fnt);
	ml->setSize(120,88);
	ml->setTranslate(Vector(320,240,0));
	ml->addItem("New Game");
	ml->addItem("Addons");
	ml->addItem("Options");
	ml->addItem("Quit");
	ml->selectItem(0);
	m_scene->subAdd(ml);
	
	m_scene->subAdd(new TikiCursor);
	
	m_selection=0;
	m_menuItems=4;
	m_repeatDelay=0;
}

void TitleScreen::FadeIn() {
	bg->setTint(Color(0,0,0));
	bg->animAdd(new TintFader(Color(1,1,1),Color(1.0f/20.0f,1.0f/20.0f,1.0f/20.0f)));
	ml->setTint(Color(0,0,0,0));
	ml->animAdd(new TintFader(Color(1,1,1,1),Color(1.0f/20.0f,1.0f/20.0f,1.0f/20.0f,1.0f/20.0f)));
	
	bg->selectTexture(0/*rand()%14*/);
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
					bg->animAdd(new TintFader(Color(0,0,0),Color(-1.0f/20.0f,-1.0f/20.0f,-1.0f/20.0f)));
					ml->animAdd(new TintFader(Color(0,0,0,0),Color(-1.0f/20.0f,-1.0f/20.0f,-1.0f/20.0f,-1.0f/20.0f)));
					startExit();
					m_exitSpeed=1.0f/20.0f;
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
			}
			break;
	}
}