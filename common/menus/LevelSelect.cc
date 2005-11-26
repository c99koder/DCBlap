/* LevelSelect.cc - Level Selection menu
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

#include "LevelSelect.h"
#include "utils.h"

extern RefPtr<Font> fnt;

LevelSelect::LevelSelect(char *path) {
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
	b1->setSize(280,260);
	b1->setTranslate(Vector(162,222,0));
	m_scene->subAdd(b1);

	b2 = new Banner(Drawable::Trans,new Texture("tex/menu/game/round_box.png",1));
	b2->setSize(280,260);
	b2->setTranslate(Vector(482,222,0));
	m_scene->subAdd(b2);
	
	is = new imageSet;
	ml = new menuList(new Texture("tex/menu/game/box.png",1), new Texture("tex/menu/game/bar.png",1), fnt);
	
	is->setSize(240,200);
	ml->setSize(240,240);
	ml->setTextColor(Color(0,0,0));
	is->setTranslate(Vector(-2,-2,1));
	ml->setTranslate(Vector(-2,-2,1));
	scan_directory(path,&files,&is_dir,count);
	
	for(int x=0; x<count; x++) {
		if((int)files[x].find(".wld") > 0) {
			files[x].resize(files[x].find_last_of("."));
			sprintf(tmp,"%s/%s.png",path,files[x].c_str());
			if(file_exists(tmp)) {
				is->addTexture(new Texture(tmp,1));
			} else {
				is->addTexture(new Texture("tex/noprev.png",1));
			}
			ml->addItem((char *)files[x].c_str());
			sprintf(tmp,"%s/%s.wld",path,files[x].c_str());
			m_levelList.push_back(tmp);
			m_menuItems++;
		}
	}
	
	is->selectTexture(0);
	ml->selectItem(0);
	b1->subAdd(is);
	b2->subAdd(ml);
	
	m_selection=0;
	m_repeatDelay=0;
}

void LevelSelect::FadeIn() {
	bg->animRemoveAll();
	ml->animRemoveAll();
	is->animRemoveAll();
	
	bg->setTint(Color(0,0,0));
	bg->animAdd(new TintFader(Color(1,1,1),Color(1.0f/20.0f,1.0f/20.0f,1.0f/20.0f)));
	b1->setTint(Color(0,0,0,0));
	b1->setTranslate(Vector(-280,224,0.1));
	b1->animAdd(new LogXYMover(162,224,8));
	b1->animAdd(new TintFader(Color(1,1,1,1),Color(1.0f/20.0f,1.0f/20.0f,1.0f/20.0f,1.0f/20.0f)));
	b2->setTint(Color(0,0,0,0));
	b2->setTranslate(Vector(640+280,224,0.1));
	b2->animAdd(new LogXYMover(482,224,8));
	b2->animAdd(new TintFader(Color(1,1,1,1),Color(1.0f/20.0f,1.0f/20.0f,1.0f/20.0f,1.0f/20.0f)));
	m_selection=0;
	is->selectTexture(0);
	ml->selectItem(0);
}

void LevelSelect::inputEvent(const Event & evt) {
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
					b2->animAdd(new TintFader(Color(0,0,0,0),Color(-1.0f/20.0f,-1.0f/20.0f,-1.0f/20.0f,-1.0f/20.0f)));
					b2->animAdd(new LogXYMover(640+280,224,10));
					b1->animAdd(new TintFader(Color(0,0,0,0),Color(-1.0f/20.0f,-1.0f/20.0f,-1.0f/20.0f,-1.0f/20.0f)));
					b1->animAdd(new LogXYMover(-280,224,10));
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