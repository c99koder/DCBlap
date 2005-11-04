/*
 *  menus.h
 *  DCBlap
 *
 *  Created by Sam Steele on 10/17/05.
 *  Copyright 2005 __MyCompanyName__. All rights reserved.
 *
 */

#include "imageSet.h"
#include "menuList.h"

class TitleScreen : public GenericMenu {
public:
	TitleScreen();
protected:
	void inputEvent(const Event & evt);
private:
	imageSet *is;
	menuList *ml;
	int m_selection;
	int m_menuItems;
	int m_repeatDelay;
};