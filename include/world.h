/* world.h - DCBlap world class
 * Copyright (c) 2005 Sam Steele
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

#include <Tiki/genmenu.h>

struct lightNode {
	float x,y,z;
	float r,g,b;
	float i;
	struct lightNode *next;
};

class World : public GenericMenu {
public:
	World(const char *filename);
	
	void visualPerFrame();
private:
	uint64 m_startTime;
	uint64 m_lastTime;
};

Tiki::GL::Entity *create_object(std::string type);
struct lightNode *add_light();
Color compute_light(float px, float py, float pz);

