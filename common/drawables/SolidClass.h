/* SolidClass.h - DCBlap SolidClass object
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

#ifndef __DRW_SOLID_CLASS_H
#define __DRW_SOLID_CLASS_H

#include "Tiki/drawable.h"
#include "Tiki/color.h"
#include "Tiki/texture.h"
#include "Tiki/file.h"
#include "Tiki/vector.h"

#include <vector>

extern std::vector< RefPtr<Texture> > texture_list;

namespace Tiki {
	namespace GL {
		
		/* A GL point in 3D space */
		class Point {
public:
			Point(float x, float y, float z, float s, float t) : m_x(x), m_y(y), m_z(z), m_s(s), m_t(t) {};

			Point(Tiki::File f) {
				loadFromFile(f);
			}
			
			void loadFromFile(Tiki::File f) {
				f.readle32(&m_x,1);
				f.readle32(&m_y,1);
				f.readle32(&m_z,1);
				f.readle32(&m_s,1);
				f.readle32(&m_t,1);		
				m_x/=100.0f;
				m_y/=100.0f;
				m_z/=100.0f;
				//printf("Point: %f %f %f %f %f\n",m_x,m_y,m_z,m_s,m_t);
			}
			
			void translate(Vector v) {
				m_x += v.x;
				m_y += v.y;
				m_z += v.z;
			}
			
			void draw(Color t, Vector p) {
				Color c = t * compute_light(m_x+p.x,m_y+p.y,-(m_z+p.z));
				glColor4f(c.r,c.g,c.b,c.a);
				glTexCoord2f(m_s,m_t);
				glVertex3f(m_x,m_y,-m_z);
			}
			
			float getX() { return m_x; }
			float getY() { return m_y; }
			float getZ() { return m_z; }
			float getS() { return m_s; }
			float getT() { return m_t; }
			
private:
			float m_x, m_y, m_z;
			float m_s, m_t;
		};
		
		/* A GL polygon in 3D space */
		class Polygon {
public:
			Polygon(int num) {
				m_num=num;
			}
			
			Polygon(int num, Tiki::File f) {
				m_num=num;
				loadFromFile(f);
			}
			
			~Polygon() {
			}
			
			void setTexture(Texture *t) {
				m_tex=t;
			}
			
			void addPoint(Point p) {
				m_points.push_back(p);
			}
			
			void translate(Vector v) {
				std::list<Point>::iterator iter;
				
				for(iter = m_points.begin(); iter != m_points.end(); iter++) {
					(*iter).translate(v);
				}
			}
			
			void bounds(Vector &min, Vector &max) {
				std::list<Point>::iterator iter;

				for(iter = m_points.begin(); iter != m_points.end(); iter++) {
					if(m_num==0 && iter == m_points.begin()) {
						min.x=max.x=(*iter).getX();
						min.y=max.y=(*iter).getY();
						min.z=max.z=(*iter).getZ();
					} else {
						if((*iter).getX() < min.x) min.x=(*iter).getX();
						if((*iter).getY() < min.y) min.y=(*iter).getY();
						if((*iter).getZ() < min.z) min.z=(*iter).getZ();
						if((*iter).getX() > max.x) max.x=(*iter).getX();
						if((*iter).getY() > max.y) max.y=(*iter).getY();
						if((*iter).getZ() > max.z) max.z=(*iter).getZ();
					}
				}
			}
			
			void loadFromFile(Tiki::File f) {
				int vertcount,tex_id;
				
				f.readle32(&tex_id,1);
				m_tex=texture_list[tex_id];
				f.readle32(&vertcount,1);
				//printf("Loading %i points\n",vertcount);
				for(int i=0; i<vertcount; i++)
					addPoint(*new Point(f));
			}
			
			void draw(Color t, Vector p) {
				std::list<Point>::iterator iter;
				m_tex->select();
				glBegin(GL_POLYGON);
				for(iter = m_points.begin(); iter != m_points.end(); iter++) {
					(*iter).draw(t,p);
				}
				glEnd();
			}
			
private:
			RefPtr<Texture> m_tex;
			std::list<Point> m_points;
			int m_num;
		};
		
		/* SolidClass is a collection of polygons */
		class SolidClass : public Entity {
public:
			SolidClass();
			~SolidClass();
			
			virtual void loadFromFile(Tiki::File f);
			void addPoly(Polygon p) { m_polys.push_back(p); }
			
			//Overloaded functions
			void draw(ObjType list);
private:
			std::list<Polygon> m_polys;
		};
		
	};
};
#endif //__DRW_SOLID_CLASS_H