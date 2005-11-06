/* imageSet.cc - image set drawable
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
#include <vector>
#include "imageSet.h"

imageSet::imageSet() {
	m_currentTexture=-1;
	m_nextTexture=-1;
	m_blend=0;
}

imageSet::~imageSet() {
}

void imageSet::addTexture(Texture * tex) {
	m_textureList.push_back(tex);
}

void imageSet::selectTexture(int num) {
	if(m_nextTexture!=-1) m_currentTexture=m_nextTexture;
	m_nextTexture=num;
	m_blend=1.0f;
}

void imageSet::draw(ObjType list) {
	if(list==Trans) {
		if(m_currentTexture>=0) {
			setType(Banner::Trans);
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
}

void imageSet::nextFrame(uint64 tm) {
	if(m_blend>0) {
		m_blend-=1.0f/20.0f;
		if(m_blend<=0) {
			m_currentTexture = m_nextTexture;
			m_nextTexture=-1;
		}
	}
	Drawable::nextFrame(tm);
}