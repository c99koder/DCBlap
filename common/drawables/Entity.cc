/* Entity.h - DCBlap Entity
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

#include <Tiki/tiki.h>
#include <Tiki/gl.h>
#include <Tiki/texture.h>
#include <Tiki/plxcompat.h>
#include "Entity.h"
#include "camera.h"

using namespace Tiki;
using namespace Tiki::GL;
using namespace Tiki::GL::Plxcompat;

Entity::Entity() {
	m_model=NULL;
	m_blendCount=0;
	m_animStart=0;
	m_animEnd=0;
	m_blendPos=0;
}

Entity::~Entity() {
}

std::string Entity::loadString(Tiki::File f) {
	char c;
	std::string s;
	
	do {
		f.read(&c,1);
		if(c!='\0') s += c;
	} while(c!='\0');

	return s;
}

void Entity::loadFromFile(Tiki::File f) {
	int paramcount=0;
	
	f.readle32(&paramcount,1);
	for(int i=0; i<paramcount; i++) {
		setProperty(loadString(f),loadString(f));
	}
	f.readle32(&m_polycount,1);
}

void Entity::drawModel(ObjType list) {
	int frame1,frame2;
	Vector p = getPosition();
	Vector r = getRotate();
	Color c = getColor();
	
	if(m_model != NULL && list==Opaque) {
		if(m_frameNum < m_animStart) {
			m_frameNum=m_animStart;
		}
		frame1=m_frameNum;
		if(m_frameNum==m_animEnd) {
			frame2=m_animStart;
		} else {
			frame2=m_frameNum+1;
		}
		if(frame2>m_animEnd) { frame2=m_animEnd; }
		
		glLoadIdentity();
		glRotatef(camera_xrot,1.0f,0.0f,0.0f);
		glRotatef(camera_zrot,0.0f,0.0f,1.0f);
		glRotatef(camera_yrot,0.0f,1.0f,0.0f);
		glTranslatef(-camera_x,-camera_y,camera_z);
		glTranslatef(p.x,p.y,-p.z);
		glTranslatef(0,.16,0);
		glRotatef(r.x,1.0f,0.0f,0.0f);
		glRotatef(r.y,0.0f,1.0f,0.0f);
		glRotatef(r.z,0.0f,0.0f,1.0f);
		glRotatef(-90,1.0f,0.0f,0.0f);
		
		if(m_animStart!=m_animEnd)
			m_model->SetFrame(frame1,frame2,m_blendPos,m_blendCount);
				//printf("md2model->draw()!\n");
		m_model->bound_min(m_xmin,m_ymin,m_zmin);
		m_model->bound_max(m_xmax,m_ymax,m_zmax);

		m_texture->select();
		glColor4f(c.r,c.g,c.b,c.a);

		m_model->Draw();

		m_blendPos++;
		if(m_blendPos>m_blendCount) {
			m_blendPos=0;
			m_frameNum++;
		}
		
		if(m_frameNum>m_animEnd) { m_frameNum=m_animStart; }
	}
}

void Entity::setProperty(std::string name, std::string value) {
	float a,b,c;

	if(name=="origin") {
		makeCoordinates(value.c_str(),&a,&b,&c);
		setTranslate(Vector(a/100.0f,b/100.0f,c/100.0f));
	} else if(name=="angles") {
		makeCoordinates(value.c_str(),&a,&b,&c);
		setRotate(Vector(a,c,b));
	}
}

void Entity::makeCoordinates(const char *text,float *x, float *y, float *z) {
  int i,j;
  char tmp[100];
  j=0; i=0;
  while(text[j]!=' ') {
    tmp[i]=text[j];
    i++; j++;
  }
  tmp[i]='\0';
  *x=atoi(tmp);
  
  i=0; j++;
  while(text[j]!=' ') {
    tmp[i]=text[j];
    i++; j++;
  }
  tmp[i]='\0';
  *z=atoi(tmp);
  
  i=0; j++;
  while(text[j]!='\0') {
    tmp[i]=text[j];
    i++; j++;
  }
  tmp[i]='\0';
  *y=atoi(tmp);
}