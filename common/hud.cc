#include <string.h>
#include "entity.h"
/* hud.cc - Heads Up Display functions
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
#include "objects.h"
#include "text.h"

int inout=0;
char status_text[256];
float status_alpha=0.0f;
float status_r,status_g,status_b;
int viscount=0;

struct hud_display {
  char label[100];
  float r,g,b;
  int x,y;
} hud[7];

void reset_HUD() {
  int i;
  for(i=0;i<7;i++) {
    hud[i].label[0]='\0';
  }
}

void set_hud(int num, int x, int y, char *label, float r, float g, float b) {
  hud[num].x=x;
  hud[num].y=y;
  strcpy(hud[num].label,label);
  hud[num].r=r;
  hud[num].g=g;
  hud[num].b=b;
}

void render_HUD() {
  int i;

  for(i=0;i<7;i++) {
    if(hud[i].label[0]!='\0') {
      draw_txt(hud[i].x+3,hud[i].y+3,hud[i].label,0,0,0,0.4f);
      draw_txt(hud[i].x,hud[i].y,hud[i].label,hud[i].r,hud[i].g,hud[i].b,1);
    }
  }

  if(inout!=0) {
		//printf("Drawing status message\n");
		if(inout==2) { status_alpha-=.05f; }
		if(inout==1) { status_alpha+=.05f; }
    if(inout==3) { viscount++; }
		if(status_alpha>1) inout=3;
    if(viscount>10) { inout=2; viscount=0; }
		if(status_alpha<0) inout=0;
		draw_txt(320-(txt_width(status_text)/2)+2,228+2,status_text,0,0,0,status_alpha-.2f);
		draw_txt(320-(txt_width(status_text)/2),228,status_text,status_r,status_g,status_b,status_alpha);
  }
	//printf("---render_HUD()\n");
}

void set_status_text(char *text,float r, float g, float b) {
	strcpy(status_text,text);
	inout=1;
	status_alpha=0.0f;
  viscount=0;
	status_r=r;
	status_g=g;
	status_b=b;
}
