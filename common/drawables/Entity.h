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

#ifndef __DRW_ENTITY_H
#define __DRW_ENTITY_H

#include "Tiki/drawable.h"
#include "Tiki/file.h"
#include "md2Model.h"

namespace Tiki {
	namespace GL {
		
		class Entity : public Drawable {
public:
			Entity();
			~Entity();

			virtual void loadFromFile(Tiki::File f);
			std::string loadString(Tiki::File f);
			virtual void setProperty(std::string name, std::string value);
			void makeCoordinates(const char *text,float *x, float *y, float *z);
			Vector getMin() { return Vector(m_xmin,m_ymin,m_zmin); }
			Vector getMax() { return Vector(m_xmax,m_ymax,m_zmax); }
			void drawModel(ObjType list);
protected:
			int m_polycount;
			float m_xmin,m_ymin,m_zmin;
			float m_xmax,m_ymax,m_zmax;
			md2Model *m_model;
			int m_blendCount, m_blendPos, m_animStart, m_animEnd, m_frameNum;
			Texture *m_texture;
		};
	};
};
#endif //__DRW_ENTITY_H