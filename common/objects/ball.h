/* ball.h - DCBlap ball object
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

#ifndef __OBJ_BALL_H
#define __OBJ_BALL_H

class Ball : public Bouncer {
public:
	Ball();
	
	int thud(Entity * sender, Object * arg);	
	int getLastTouched() { return m_lastTouched; }
	
	//Overloaded functions
	void draw(ObjType list);
	void setProperty(std::string name, std::string value);
protected:
	TIKI_OBJECT_DECLS(Ball)
private:
	int m_lastTouched;
};

#endif