/*
 *  menus.cc
 *  DCBlap
 *
 *  Created by Sam Steele on 10/17/05.
 *  Copyright 2005 __MyCompanyName__. All rights reserved.
 *
 */

#include "Tiki/tiki.h"
#include "Tiki/genmenu.h"
#include "Tiki/texture.h"
#include "Tiki/drawables/banner.h"

using namespace Tiki;
using namespace Tiki::GL;

#include "imageSet.h"
#include "menus.h"

TitleScreen::TitleScreen() {
	Texture *tex;
	Banner *ban;
	imageSet *is;
	GenericMenu::GenericMenu();
	
	tex = new Texture("title.png",1);
	ban = new Banner(Drawable::Trans,tex);
	is = new imageSet;
	ban->setSize(640,480);
	ban->setTranslate(Vector(320,240,0));
	m_scene->subAdd(ban);
	
	is->setSize(320,240);
	is->setTranslate(Vector(320,240,0));
	is->addTexture(tex);
	is->selectTexture(0);
	m_scene->subAdd(is);
}

void TitleScreen::inputEvent(const Event & evt) {
	switch(evt.type) {
		case Event::EvtQuit:
			quitNow();
			break;
		case Event::EvtKeypress:
			switch(evt.key) {
				case Event::KeySelect:
					startExit();
					break;
				default:
					printf("Key: %c\n",evt.key);
			}
			break;
	}
}