/* SolidClass.cc - DCBlap SolidClass object
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

using namespace Tiki;
using namespace Tiki::Math;
using namespace Tiki::GL;
using namespace Tiki::GL::Plxcompat;

#include "camera.h"
#include "objects.h"

SolidClass::SolidClass() {
}

SolidClass::~SolidClass() {
}

void SolidClass::loadFromFile(Tiki::File f) {
	std::list<Polygon>::iterator iter;
	Polygon *p;
	Vector v;
	
	Entity::loadFromFile(f);

	//printf("Loading %i polys\n",m_polycount);
	for(int i=0; i<m_polycount; i++) {
		p=new Polygon(i);
		p->loadFromFile(f);
		addPoly(*p);
		p->bounds(m_min,m_max);
	}
	
	v=m_min+((m_max-m_min)*0.5);
	setTranslate(v);
	
	m_min-=v; m_max-=v;

	for(iter = m_polys.begin(); iter != m_polys.end(); iter++) {
		(*iter).translate(-v);
	}
}

void SolidClass::draw(ObjType list) {
	std::list<Polygon>::iterator iter;
	
	if(list==Opaque) {
		Color c=getColor();
		Vector p=getPosition();
		Vector r=getRotate();
		
		glLoadIdentity();
		glRotatef(camera_xrot,1.0f,0.0f,0.0f);
		glRotatef(camera_zrot,0.0f,0.0f,1.0f);
		glRotatef(camera_yrot,0.0f,1.0f,0.0f);
		glTranslatef(-camera_x,-camera_y,camera_z);
		glTranslatef(p.x,p.y,-p.z);
		glRotatef(r.x,1.0f,0.0f,0.0f);
		glRotatef(r.y,0.0f,1.0f,0.0f);
		glRotatef(r.z,0.0f,0.0f,1.0f);
		
		for(iter = m_polys.begin(); iter != m_polys.end(); iter++) {
			(*iter).draw(c,p);
		}
	}
}