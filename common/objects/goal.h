/* goal.h - DCBlap goal object
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

#ifndef __OBJ_GOAL_H
#define __OBJ_GOAL_H

class Goal : public SolidClass {
public:
	Goal();
	
	int thud(Entity * sender, Object * arg);	
	void giveScore(int points) { m_score += points; }
	
	//Overloaded functions
	void setProperty(std::string name, std::string value);
	void nextFrame(uint64 tm);
protected:
	TIKI_OBJECT_DECLS(Goal)
private:
	int m_num;
	int m_score;
};

#endif