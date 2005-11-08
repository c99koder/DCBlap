/* lcd.c - draw a logo on any VMUs when attached to the system
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
#include <kos.h>
#include "squareslogo"

static void dcb_vmu_detach(maple_driver_t *drv, maple_device_t *dev) {
}

maple_device_t *lcds[8];
uint8   bitmap[48*32/8];
int lcdcount=0;
                                                                                
static int dcb_vmu_attach(maple_driver_t *drv, maple_device_t *dev) {
        dev->status_valid = 1;
	lcds[lcdcount++]=dev; 
        return 0;
}

void update_lcds() {
	int i;
	if(lcdcount==0) return;
	for(i=0;i<lcdcount;i++) {
          vmu_draw_lcd(lcds[i], bitmap);
	}
	lcdcount=0;
}
                                                                                
/* Device Driver Struct */
static maple_driver_t dcb_vmu_drv = {
        functions:      MAPLE_FUNC_MEMCARD | MAPLE_FUNC_LCD | MAPLE_FUNC_CLOCK,
        name:           "DCBlap VMU Driver",
        periodic:       NULL,
        attach:         dcb_vmu_attach,
        detach:         dcb_vmu_detach
};

void dcb_vmu_init() {
        int     x, y, xi, xb, i;
	maple_device_t *dev;
        memset(bitmap, 0, 48*32/8);
        if (squareslogo_xpm) {
                for (y=0; y<32; y++)
                        for (x=0; x<48; x++) {
                                xi = x / 8;
                                xb = 0x80 >> (x % 8);
                                if (squareslogo_xpm[(31-y)*48+(47-x)] == '.')
                                        bitmap[y*(48/8)+xi] |= xb;
                        }
        }
        i = 0;
        while ( (dev = maple_enum_type(i++, MAPLE_FUNC_LCD)) ) {
                vmu_draw_lcd(dev, bitmap);
        }

        vmu_shutdown();
        maple_driver_reg(&dcb_vmu_drv);
	/*maple_hw_shutdown();
	maple_hw_init();*/
}
