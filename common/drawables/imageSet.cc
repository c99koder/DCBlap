/*
 *  imageSet.cpp
 *  DCBlap
 *
 *  Created by Sam Steele on 10/27/05.
 *  Copyright 2005 __MyCompanyName__. All rights reserved.
 *
 */

#ifdef TIKI
#include <Tiki/tiki.h>
#include <Tiki/gl.h>

using namespace Tiki;
using namespace Tiki::GL;
#endif
#include <vector>
#include "imageSet.h"

imageSet::imageSet() {
	m_currentTexture=-1;
	m_nextTexture=-1;
	m_blend=0;
}

void imageSet::addTexture(Texture * tex) {
	m_textureList.push_back(tex);
}

void imageSet::selectTexture(int num) {
	m_nextTexture=num;
	m_blend=1;
}

void imageSet::draw(ObjType list) {
	if(m_currentTexture>=0) {
		setType(Banner::Opaque);
		setAlpha(1.0);
		setTexture(m_textureList[m_currentTexture]);
		Banner::draw(list);
	}
	if(m_nextTexture>=0) {
		setType(Banner::Trans);
		setAlpha(1.0 - m_blend);
		setTexture(m_textureList[m_nextTexture]);
		Banner::draw(list);
	}
}

void imageSet::nextFrame(uint64 tm) {
	if(m_blend>0) {
		m_blend -= 0.01;
		if(m_blend==0) {
			m_currentTexture = m_nextTexture;
			m_nextTexture=-1;
		}
	}
}