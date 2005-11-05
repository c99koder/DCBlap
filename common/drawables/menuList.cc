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

#ifdef TIKI
#include <Tiki/tiki.h>
#include <Tiki/gl.h>

using namespace Tiki;
using namespace Tiki::GL;
#endif
#include "menuList.h"

menuList::menuList(Texture *bg, Texture *bar, Font *fnt) {
	m_bg=new Banner(Banner::Trans,bg);
	m_bg->setAlpha(0.4f);
	subAdd(m_bg);
	m_bar=new Banner(Banner::Trans,bar);
	m_bar->setAlpha(0.4f);
	subAdd(m_bar);
	m_fnt=fnt;
}

void menuList::setSize(float w, float h) {
	m_w=w;
	m_h=h;
	m_bg->setSize(m_w,m_h);
	m_bar->setSize(m_w,22);
}

void menuList::addItem(char *text) {
	string *s=new string(text);
	m_itemList.push_back(*s);
}

void menuList::selectItem(int num) {
	m_currentItem=num;
}

void menuList::draw(ObjType list) {
	std::vector<string>::iterator item_iter;
	int i=0;
	
	Drawable::draw(list);
	
	if(list==Trans) {
		m_fnt->setSize(20);
		m_fnt->setColor(getTint());
		if(m_itemList.size() > 0) {
			for(item_iter = m_itemList.begin(); item_iter != m_itemList.end(); item_iter++) {
				m_fnt->draw(getTranslate() + Vector(-m_w/2,-m_h/2 - 4 + ((i+1)*22),-0.2),*(item_iter));
				i++;
			}
		}
	}
}

void menuList::nextFrame(uint64 tm) {
	m_bar->setTranslate(Vector(0,-m_h/2 - 11 + ((m_currentItem+1)*22),0));
	Drawable::nextFrame(tm);
}
