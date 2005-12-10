/* light.h - DCBlap light object
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

#ifndef __OBJ_LIGHT_H
#define __OBJ_LIGHT_H

class Light : public Entity {
public:
	Light();
	Light(Tiki::File f);
	
	//Overloaded functions
	void nextFrame(uint64 tm);
	void setProperty(std::string name, std::string value);
private:
	lightNode *m_light;
	float m_count, m_rate;
	int m_effect;
	float m_r, m_g, m_b;
};

#endif