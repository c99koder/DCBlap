/* GameSelect.h - Game Selection menu
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
#include <Tiki/drawables/label.h>
#include <Tiki/drawables/banner.h>
#include "drawables/imageSet.h"
#include "drawables/menuList.h"
#include "drawables/ShadowBox.h"


class GameSelect : public GenericMenu {
public:
	GameSelect();
	void FadeIn();
	string getSelection() { return m_gameList[m_selection]; }
protected:
	void inputEvent(const Event & evt);
private:
	std::vector<string> m_gameList;
	RefPtr<Banner> bg,a1,a2,b1;
	RefPtr<ShadowBox> h1;
	RefPtr<imageSet> is;
	RefPtr<Label> title;
	int m_selection;
	int m_menuItems;
	int m_repeatDelay;
};