/* menuList.h - menu list drawable
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

#ifndef _MENULIST_H
#define _MENULIST_H

#include <Tiki/font.h>
#include "Tiki/drawables/banner.h"
#include <vector>
#include <string>

class menuList : public Drawable {
public:
	menuList(Texture *bg, Texture *bar, Font *fnt);
	~menuList();
	void addItem(char *text);        //Add an item to the list
	void selectItem(int num);        //Select an item
	void setSize(float w, float h);  //Set the size of the menu
	
	//Overloaded functions
	void draw(ObjType list);
	
private:
	std::vector<string> m_itemList; //List of menh items
	int m_currentItem;							//Currently selected item
	RefPtr<Banner> m_bg;						//Background banner
	RefPtr<Banner> m_bar;						//Highlight bar banner
	RefPtr<Font> m_fnt;							//Font to draw with
	float	m_w, m_h;									//Width and height of menu
};

#endif