/* menuList.cc - menu list drawable
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
#include <Tiki/anims/logxymover.h>
#include <Tiki/anims/alpharotate.h>

using namespace Tiki;
using namespace Tiki::GL;

#include "menuList.h"

menuList::menuList(Texture *bg, Texture *bar, Font *fnt) {
	m_bg=new Banner(Banner::Trans,bg);
	subAdd(m_bg);
	m_bar=new Banner(Banner::Trans,bar);
	m_bar->animAdd(new AlphaRotate(1.0f/60.0f,0.4));
	subAdd(m_bar);
	m_fnt=fnt;
	m_textColor=Color(1,1,1,1);
}

menuList::~menuList() {
	while(!m_itemList.empty()) {
		m_itemList.pop_back();
	}
}

void menuList::setSize(float w, float h) {
	m_w=w;
	m_h=h;
	m_bg->setSize(m_w,m_h);
	m_bg->setTranslate(Vector(0,0,-0.02));
	m_bar->setSize(m_w,24);
	m_bar->setTranslate(Vector(0,-m_h/2 - 11 + 24,-0.01));
}

void menuList::setTextColor(Color c) {
	m_textColor = c;
}

void menuList::addItem(char *text) {
	string *s=new string(text);
	m_itemList.push_back(*s);
}

void menuList::selectItem(int num) {
	m_currentItem=num;
	m_bar->animRemoveAll();
	m_bar->animAdd(new LogXYMover(0,-m_h/2 - 11 + ((m_currentItem+1)*24)));
}

void menuList::draw(ObjType list) {
	std::vector<string>::iterator item_iter;
	int i=0;
	
	if(list==Trans) {
		Drawable::draw(list);	
		
		m_fnt->setSize(20);
		m_fnt->setColor(getTint() * m_textColor);
		if(m_itemList.size() > 0) {
			for(item_iter = m_itemList.begin(); item_iter != m_itemList.end(); item_iter++) {
				m_fnt->draw(getPosition() + Vector((-m_w/2)+4,-m_h/2 - 4 + ((i+1)*24),0),*(item_iter));
				i++;
			}
		}
	}
}
