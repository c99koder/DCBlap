/*
 Tiki
 
 ShadowBox.cpp
 
 Copyright (C)2005 Sam Steele
 */

#include <Tiki/tiki.h>
#include <Tiki/gl.h>
#include <Tiki/texture.h>
#include <Tiki/plxcompat.h>
#include "ShadowBox.h"

using namespace Tiki;
using namespace Tiki::GL;
using namespace Tiki::GL::Plxcompat;

ShadowBox::ShadowBox(float w, float h) {
	setSize(w,h);
}

ShadowBox::~ShadowBox() {
}

void ShadowBox::draw(ObjType list) {
	const Vector & tv = getPosition();
	
	if(list==Trans) {
		drawBox(tv.x+3,tv.y+3,tv.z-0.02,m_w,m_h,Color(0.4,0,0,0));
		drawBox(tv.x,tv.y,tv.z-0.01,m_w+2,m_h+2,Color(0,0,0));
		drawBox(tv.x,tv.y,tv.z,m_w,m_h,getColor());
	}
}

void ShadowBox::setSize(float w, float h) {
	m_w = w;
	m_h = h;
}

void ShadowBox::drawBox(float x, float y, float z, float w, float h, Color c) {
	Color a=getColor();
	plx_vertex_t vert;
	c.a=a.a;
	vert.argb = c;
	vert.z = z;
	
	Texture::deselect();
	
	vert.flags = PLX_VERT;
	vert.x = x-w/2;
	vert.y = y+h/2;
	plx_prim(&vert, sizeof(vert));
	
	vert.y = y-h/2;
	plx_prim(&vert, sizeof(vert));
	
	vert.x = x+w/2;
	vert.y = y+h/2;
	plx_prim(&vert, sizeof(vert));
	
	vert.flags = PLX_VERT_EOS;
	vert.y = y-h/2;
	plx_prim(&vert, sizeof(vert));	
}