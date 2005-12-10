/* player.h - Player object
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

#ifndef __OBJ_PLAYER_H
#define __OBJ_PLAYER_H

#include <Tiki/hid.h>

class Player : public Entity {
public:
	TIKI_OBJECT_NAME(Player);
	Player();
	int getNum() { return m_num; }
	
	//Overloaded functions
	void nextFrame(uint64 tm);
protected:
	// HID input callback.
	static void hidCallback(const Tiki::Hid::Event & evt, void * data);
	virtual void processHidEvent(const Tiki::Hid::Event & evt);	
	
private:
	int m_hidCookie;
	int m_num;
};

#endif