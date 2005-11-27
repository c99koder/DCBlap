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
#include "utils.h"

extern RefPtr<Font> fnt;

GameSelect::GameSelect() {
	std::vector<std::string> files;
	std::vector<bool> is_dir;
	int count=0;
	char tmp[100];
	
	GenericMenu::GenericMenu();
	
	m_scene->setTranslate(Vector(0,0,0));
	
	bg = new Banner(Drawable::Trans,new Texture("tex/menu/game/bg.png",0));
	bg->setSize(640,480);
	bg->setTranslate(Vector(320,240,0));
	m_scene->subAdd(bg);

	b1 = new Banner(Drawable::Trans,new Texture("tex/menu/game/round_box.png",1));
	b1->setSize(320,260);
	b1->setTranslate(Vector(320,222,0));
	m_scene->subAdd(b1);
	
	is = new imageSet;
	is->setSize(305,200);
	is->setTranslate(Vector(-2,-2,1));
	
	m_menuItems=0;

	scan_directory("maps",&files,&is_dir,count);
	
	for(int x=0; x<count; x++) {
		if(is_dir[x]) {
			sprintf(tmp,"maps/%s.png",files[x].c_str());
			if(file_exists(tmp)) {
				is->addTexture(new Texture(tmp,1));
			} else {
				is->addTexture(new Texture("tex/noprev.png",1));
			}
			m_gameList.push_back(files[x]);
			m_menuItems++;
		}
	}
	
	is->selectTexture(0);
	b1->subAdd(is);

	a1 = new Banner(Drawable::Trans,new Texture("tex/menu/game/arrow.png",1));
	a1->setSize(-32,64);
	a1->setTranslate(Vector(-200,0,0));
	b1->subAdd(a1);
	
	a2 = new Banner(Drawable::Trans,new Texture("tex/menu/game/arrow.png",1));
	a2->setSize(32,64);
	a2->setTranslate(Vector(200,0,0));
	b1->subAdd(a2);
	
	m_selection=0;
	m_repeatDelay=0;
}

void GameSelect::FadeIn() {
	bg->animRemoveAll();
	b1->animRemoveAll();
	
	bg->setTint(Color(0,0,0));
	bg->animAdd(new TintFader(Color(1,1,1),Color(1.0f/20.0f,1.0f/20.0f,1.0f/20.0f)));
	b1->setTint(Color(0,0,0,0));
	b1->animAdd(new TintFader(Color(1,1,1,1),Color(1.0f/20.0f,1.0f/20.0f,1.0f/20.0f,1.0f/20.0f)));
	a1->setAlpha(0.4); 
	m_selection=0;
	is->selectTexture(0);
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
					b1->animAdd(new TintFader(Color(0,0,0,0),Color(-1.0f/20.0f,-1.0f/20.0f,-1.0f/20.0f,-1.0f/20.0f)));
					startExit();
					m_exitSpeed=1.0f/20.0f;
					break;
				case Event::KeyLeft:
					if(m_repeatDelay==0) {
						m_selection--;
						if(m_selection<0) {
							m_selection=0;
						} else {
							is->selectTexture(m_selection);
							//ml->selectItem(m_selection);
							m_repeatDelay=2;
						}
					} else {
						m_repeatDelay--;
					}
					break;
				case Event::KeyRight:
					if(m_repeatDelay==0) {
						m_selection++;
						if(m_selection>=m_menuItems) {
							m_selection=m_menuItems-1;
						} else {
							is->selectTexture(m_selection);
							//ml->selectItem(m_selection);
							m_repeatDelay=2;
						}
					} else {
						m_repeatDelay--;
					}
					break;
			}
			break;
	}
	if(m_selection==0) 
		a1->setAlpha(0.4); 
	else
		a1->setAlpha(1.0); 
	if(m_selection==m_menuItems-1) 
		a2->setAlpha(0.4); 
	else
		a2->setAlpha(1.0); 							
}