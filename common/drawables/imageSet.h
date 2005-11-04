/*
 *  imageSet.h
 *  DCBlap
 *
 *  Created by Sam Steele on 10/27/05.
 *  Copyright 2005 __MyCompanyName__. All rights reserved.
 *
 */

#include "Tiki/drawables/banner.h"
#include <vector>

class imageSet : public Banner {
public:
	imageSet();
	void addTexture(Texture * tex);
	void selectTexture(int num);
	
	//Overloaded functions
	void draw(ObjType list);
	void nextFrame(uint64 tm);
	
private:
	std::vector< RefPtr<Texture> > m_textureList;
	int m_currentTexture;
	int m_nextTexture;
	float m_blend;
};