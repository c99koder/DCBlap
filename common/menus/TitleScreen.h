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

#include "drawables/imageSet.h"
#include "drawables/menuList.h"

class TitleScreen : public GenericMenu {
public:
	TitleScreen();
	void FadeIn();
	int getSelection() { return m_selection; }
protected:
	void inputEvent(const Event & evt);
private:
	RefPtr<menuList> ml;
	RefPtr<imageSet> bg;
	
	int m_selection;
	int m_menuItems;
	int m_repeatDelay;
};