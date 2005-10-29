/*
 *  menus.h
 *  DCBlap
 *
 *  Created by Sam Steele on 10/17/05.
 *  Copyright 2005 __MyCompanyName__. All rights reserved.
 *
 */

class TitleScreen : public GenericMenu {
public:
	TitleScreen();
protected:
	void inputEvent(const Event & evt);
};