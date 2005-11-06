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
#include "Tiki/anims/tintfader.h"
#include "Tiki/anims/logxymover.h"

using namespace Tiki;
using namespace Tiki::GL;

#include "GameSelect.h"

extern RefPtr<Font> fnt;

GameSelect::GameSelect() {
	GenericMenu::GenericMenu();
	
	bg = new Banner(Drawable::Opaque,new Texture("game_bg.png",0));
	is = new imageSet;
	ml = new menuList(new Texture("game_box.png",0), new Texture("game_bar.png",0), fnt);
	bg->setSize(640,480);
	bg->setTranslate(Vector(320,240,0));
	m_scene->subAdd(bg);
	
	is->setSize(240,200);
	ml->setSize(240,240);
	is->setTranslate(Vector(160,200,-0.1));
	ml->setTranslate(Vector(480,220,-0.1));
	is->addTexture(new Texture("maps/BlapOut/classic.png",0));
	ml->addItem("Classic");
	m_levelList.push_back(*new string("BlapOut/classic.wld"));
	is->addTexture(new Texture("maps/BlapOut/swirl.png",0));
	ml->addItem("Swirl");
	m_levelList.push_back(*new string("BlapOut/swirl.wld"));
	is->addTexture(new Texture("maps/MultiBlapOut/m1.png",0));
	ml->addItem("m1");
	m_levelList.push_back(*new string("MultiBlapOut/m1.wld"));
	is->addTexture(new Texture("maps/Traditional/classic.png",0));
	ml->addItem("Classic");
	m_levelList.push_back(*new string("Traditional/classic.wld"));
	is->addTexture(new Texture("maps/Traditional/beamride.png",0));
	ml->addItem("BeamRide");
	m_levelList.push_back(*new string("Traditional/beamride.wld"));
	is->addTexture(new Texture("maps/Traditional/beamride.png",0));
	ml->addItem("Plasma");
	m_levelList.push_back(*new string("Traditional/beamride.wld"));
	is->addTexture(new Texture("maps/Traditional/lake.png",0));
	ml->addItem("Lake");
	m_levelList.push_back(*new string("Traditional/lake.wld"));
	is->addTexture(new Texture("maps/Other/airhockey1.png",0));
	ml->addItem("AirHockey1");
	m_levelList.push_back(*new string("Other/airhockey1.wld"));
	is->selectTexture(0);
	ml->selectItem(0);
	m_scene->subAdd(is);
	m_scene->subAdd(ml);
	
	m_selection=0;
	m_menuItems=8;
	m_repeatDelay=0;
}

void GameSelect::FadeIn() {
	bg->animRemoveAll();
	ml->animRemoveAll();
	is->animRemoveAll();
	
	bg->setTint(Color(0,0,0));
	bg->animAdd(new TintFader(Color(1,1,1),Color(1.0f/20.0f,1.0f/20.0f,1.0f/20.0f)));
	ml->setTint(Color(0,0,0,0));
	ml->setTranslate(Vector(640+240,220,-0.1));
	ml->animAdd(new LogXYMover(480,220,8));
	ml->animAdd(new TintFader(Color(1,1,1,1),Color(1.0f/20.0f,1.0f/20.0f,1.0f/20.0f,1.0f/20.0f)));
	is->setTint(Color(0,0,0,0));
	is->setTranslate(Vector(-240,200,-0.1));
	is->animAdd(new LogXYMover(160,200,8));
	is->animAdd(new TintFader(Color(1,1,1,1),Color(1.0f/20.0f,1.0f/20.0f,1.0f/20.0f,1.0f/20.0f)));
	m_selection=0;
	is->selectTexture(0);
	ml->selectItem(0);
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
					bg->animAdd(new TintFader(Color(0,0,0),Color(-1.0f/20.0f,-1.0f/20.0f,-1.0f/20.0f)));
					ml->animAdd(new TintFader(Color(0,0,0,0),Color(-1.0f/20.0f,-1.0f/20.0f,-1.0f/20.0f,-1.0f/20.0f)));
					ml->animAdd(new LogXYMover(640+240,220,10));
					is->animAdd(new TintFader(Color(0,0,0,0),Color(-1.0f/20.0f,-1.0f/20.0f,-1.0f/20.0f,-1.0f/20.0f)));
					is->animAdd(new LogXYMover(-240,220,10));
					startExit();
					m_exitSpeed=1.0f/20.0f;
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