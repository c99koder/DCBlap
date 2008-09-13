/*
 Tiki
 
 ShadowBox.cpp
 
 Copyright (C)2005 Sam Steele
 */

#include <Tiki/tiki.h>
#include <Tiki/gl.h>
#include <Tiki/texture.h>
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
	const Vector & tv = getPosition();
	const Vector & rv = getRotate();
	
	Texture::deselect();
	
	glPushMatrix();
	glTranslatef(tv.x, tv.y, tv.z/1000.0f);
	glRotatef(rv.w,rv.x,rv.y,rv.z);
	glColor4f(c.r,c.g,c.b,c.a);
	
	glBegin(GL_QUADS);
	glVertex3f(-w/2.0f,-h/2.0f,0.0f);
	glVertex3f(w/2.0f,-h/2.0f,0.0f);
	glVertex3f(w/2.0f,h/2.0f,0.0f);
	glVertex3f(-w/2.0f,h/2.0f,0.0f);
	glEnd();
	
	glPopMatrix();
}