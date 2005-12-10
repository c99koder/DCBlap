/* light.cc - DCBlap light object
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

using namespace Tiki;
using namespace Tiki::GL;

#include "objects.h"
#include <stdlib.h>

Light::Light() {
	m_effect=0;
	m_light=add_light();
}

void Light::setProperty(std::string name, std::string value) {
	printf("Light: %s = %s\n",name.c_str(),value.c_str());

	if(name=="red") {
		m_r=m_light->r=atoi(value.c_str())/100.0f;
	} else if(name=="green") {
		m_g=m_light->g=atoi(value.c_str())/100.0f;
	} else if(name=="blue") {
		m_b=m_light->b=atoi(value.c_str())/100.0f;
	} else if(name=="radius") {
		m_light->i=atoi(value.c_str())/100.0f;
	} else if(name=="effect") {
		m_effect = atoi(value.c_str());
		m_count=100;
		m_rate=1;
	} else {
		Entity::setProperty(name,value);
	}
}

void Light::nextFrame(uint64 tm) {
	float gt=tm/1000000.0f;
	Vector p=getPosition();
	
	m_light->x=p.x;
	m_light->y=p.y;
	m_light->z=p.z;
	
  switch(m_effect) {
  case 0:
    break;
  case 1: //pulse
    if(m_count>=100) {
      m_rate=-1;
    } else if (m_count<=0) { 
      m_rate=1;
    }
    m_count += (m_rate * gt);
		m_light->r = m_r * (m_count / 100.0f); 
		m_light->g = m_g * (m_count / 100.0f); 
		m_light->b = m_b * (m_count / 100.0f); 
    break;
  case 2: //flicker
    if(m_count>=100) {
      m_count=0;
    }
    m_count += m_rate * gt;
    if(m_count>10) {
      m_light->r = m_r;
      m_light->g = m_g;
      m_light->b = m_b;
    } else {
      m_light->r = 0;
      m_light->g = 0;
      m_light->b = 0;
    }
    break;
  }
}
