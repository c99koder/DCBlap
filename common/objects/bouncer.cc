/* bouncer.cc - DCBlap bouncer object
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
#include <Tiki/texture.h>
#include <Tiki/tikimath.h>

using namespace Tiki;
using namespace Tiki::GL;
using namespace Tiki::Math;

#include "objects.h"
#include <stdlib.h>

Bouncer::Bouncer() {
	m_orientation=XZ;
	m_velocity=Vector(0,0,0);
	m_speed=0;
}

int Bouncer::thud(Entity * sender, Object * arg) {
	Vector tmin,tmax;
	Vector mmin,mmax;
	
	tmin=sender->getMin() + sender->getTranslate();
	tmax=sender->getMax() + sender->getTranslate();

	mmin=getMin() + getTranslate();
	mmax=getMax() + getTranslate();
	
	if((tmin.x<=mmin.x && tmax.x<=mmax.x) ||
		 (tmin.x>=mmin.x && tmax.x>=mmax.x))
		m_velocity.x*=-1;
	if(m_orientation==XZ && ((tmin.z<=mmin.z && tmax.z<=mmax.z) ||
		 (tmin.z>=mmin.z && tmax.z>=mmax.z)))
		m_velocity.z*=-1;
	if(m_orientation==XY && ((tmin.y<=mmin.y && tmax.y<=mmax.y) ||
		 (tmin.y>=mmin.y && tmax.y>=mmax.y)))
		m_velocity.y*=-1;
	
	setTranslate(m_oldPos);
	return 0;
}

void Bouncer::nextFrame(uint64 tm) {
	float gt=tm/1000000.0f;
	Vector p=getTranslate();
	Vector r=getRotate();
	
	m_oldPos = p;
	p += (m_velocity * ((m_speed * gt * 40.0f)/100.0f));  
	
	setTranslate(p);
	setRotate(r+Vector(60,60,0)*gt);
}
