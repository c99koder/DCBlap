/* player_1st.cc - 1st person player object
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
#include <Tiki/hid.h>
#include <Tiki/tikimath.h>
#include <Tiki/texture.h>

using namespace Tiki;
using namespace Tiki::Math;
using namespace Tiki::GL;
using namespace Tiki::Hid;

#include "objects.h"
#include "camera.h"

Player::Player() {
	// Register us for HID input callbacks.
	m_hidCookie = callbackReg(hidCallback, this);
	assert( m_hidCookie >= 0 );
}

void Player::hidCallback(const Event & evt, void * data) {
	assert( data );
	((Player *)data)->processHidEvent(evt);
}

void Player::processHidEvent(const Event & evt) {
	Vector p=getTranslate();
	Vector r=getRotate();
	
	if(evt.dev == NULL) {
		return;
	}
	
	if (evt.type==Event::EvtKeypress ) {
		switch(evt.key) {
			case Event::KeyUp:
				p.z+=fsin(r.y*(M_PI/180))*0.1;
				p.x-=fcos(r.y*(M_PI/180))*0.1;
				break;
			case Event::KeyDown:
				p.z-=fsin(r.y*(M_PI/180))*0.1;
				p.x+=fcos(r.y*(M_PI/180))*0.1;
				break;
			case Event::KeyLeft:
				r.y-=2;
				break;
			case Event::KeyRight:
				r.y+=2;
				break;
		}
		setTranslate(p);
		setRotate(r);
	}
}

void Player::nextFrame(uint64 tm) {
	float gt=tm/1000000.0f;
	Vector p=getPosition();
	Vector r=getRotate();
	
	camera_x=p.x;
	camera_y=p.y;
	camera_z=p.z;
	camera_xrot=r.x;
	camera_yrot=r.y-90;
	camera_zrot=r.z;
}