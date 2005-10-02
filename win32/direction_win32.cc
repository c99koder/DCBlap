/* direction_win32.cc - Windows input device support
 * Copyright (c) 2001-2003 Sam Steele
 *
 * This file is part of DCBlap.
 *
 * DCBlap is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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
#include <windows.h>
#include <dinput.h>
#include "direction.h"

extern bool keys[256];
bool sys_render_begin();

extern HWND hWnd;

LPDIRECTINPUT8       g_pDI    = NULL;         
LPDIRECTINPUTDEVICE8 g_pMouse = NULL;     

void read_analog(float *x, float *y) {
    HRESULT       hr;
    DIMOUSESTATE2 dims2;      // DirectInput mouse state structure

    if( NULL == g_pMouse ) 
        return;
    
    // Get the input's device state, and put the state in dims
    ZeroMemory( &dims2, sizeof(dims2) );
    hr = g_pMouse->GetDeviceState( sizeof(DIMOUSESTATE2), &dims2 );
    if( FAILED(hr) ) 
    {
        // DirectInput may be telling us that the input stream has been
        // interrupted.  We aren't tracking any state between polls, so
        // we don't have any special reset that needs to be done.
        // We just re-acquire and try again.
        
        // If input is lost then acquire and keep trying 
        hr = g_pMouse->Acquire();
        while( hr == DIERR_INPUTLOST ) 
            hr = g_pMouse->Acquire();

        return; 
    }
    
    // The dims structure now has the state of the mouse, so 
    // display mouse coordinates (x, y, z) and buttons.
    *x+=(dims2.lX/2);
    *y+=(dims2.lY/2);
}

void analog_init() {
    HRESULT hr;
    DWORD   dwCoopFlags;

    dwCoopFlags = DISCL_EXCLUSIVE;
    dwCoopFlags |= DISCL_FOREGROUND;

    // Create a DInput object
    if( FAILED( hr = DirectInput8Create( GetModuleHandle(NULL), DIRECTINPUT_VERSION, 
                                         IID_IDirectInput8, (VOID**)&g_pDI, NULL ) ) )
        return;
    
    // Obtain an interface to the system mouse device.
    if( FAILED( hr = g_pDI->CreateDevice( GUID_SysMouse, &g_pMouse, NULL ) ) )
        return;
    
    // Set the data format to "mouse format" - a predefined data format 
    //
    // A data format specifies which controls on a device we
    // are interested in, and how they should be reported.
    //
    // This tells DirectInput that we will be passing a
    // DIMOUSESTATE2 structure to IDirectInputDevice::GetDeviceState.
    if( FAILED( hr = g_pMouse->SetDataFormat( &c_dfDIMouse2 ) ) )
        return;
    
    // Set the cooperativity level to let DirectInput know how
    // this device should interact with the system and with other
    // DirectInput applications.
    hr = g_pMouse->SetCooperativeLevel( hWnd, dwCoopFlags );

    if( FAILED(hr) )
        return;

    // Acquire the newly created device
    g_pMouse->Acquire();
}

int get_controls(int num) {
  sys_render_begin();
	if (keys[VK_RETURN]) {
		return START_BTN;
  } else if (keys[VK_ESCAPE]) {
    return QUIT_BTN;
  } else if (keys[VK_SPACE]) {
    return FIRE_BTN;
  }
	switch(num) {
	case 0:
    if (keys[VK_LEFT]) {
      return MOVE_LEFT;
		}
		else if (keys[VK_RIGHT]) {
      return MOVE_RIGHT;
    }
    else if (keys[VK_UP]) {
      return MOVE_UP;
    }
    else if (keys[VK_DOWN]) {
      return MOVE_DOWN;
    }
		break;
	case 1:
		if(keys['A']) {
			return MOVE_LEFT;
		}
		else if (keys['D']) {
			return MOVE_RIGHT;
		}
		else if (keys['W']) {
			return MOVE_UP;
		}
		else if (keys['S']) {
			return MOVE_DOWN;
		}
		break;
	}
	return 0;
}
