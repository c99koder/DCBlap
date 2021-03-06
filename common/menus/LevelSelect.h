/* LevelSelect.h - Level Selection menu
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

#include <vector> 
#include "drawables/imageSet.h"
#include "drawables/menuList.h"

class LevelSelect : public GenericMenu {
public:
	LevelSelect(char *path);
	void FadeIn();
	string getSelection() { return m_levelList[m_selection]; }
protected:
	void inputEvent(const Event & evt);
private:
	std::vector<string> m_levelList;
	RefPtr<Banner> bg,b1,b2;
	RefPtr<imageSet> is;
	RefPtr<menuList> ml;
	int m_selection;
	int m_menuItems;
	int m_repeatDelay;
};