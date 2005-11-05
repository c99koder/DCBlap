/* imageSet.h - image set drawable
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

#ifndef _IMAGESET_H
#define _IMAGESET_H

#include "Tiki/drawables/banner.h"
#include <vector>

class imageSet : public Banner {
public:
	imageSet();
	void addTexture(Texture * tex); //Add a texture
	void selectTexture(int num); //Select the active texture
	
	//Overloaded functions
	void draw(ObjType list);
	void nextFrame(uint64 tm);
	
private:
	std::vector< RefPtr<Texture> > m_textureList; //List of textures
	int m_currentTexture; //Currently selected texture
	int m_nextTexture;    //Texture we are transitioning to
	float m_blend;        //Transition progress
};

#endif