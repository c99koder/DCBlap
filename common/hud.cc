/* hud.cc - Heads Up Display functions
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
#include <Tiki/font.h>

using namespace Tiki;
using namespace Tiki::GL;

#include <string.h>
#include "entity.h"
#include "objects.h"

RefPtr<Font> fnt;

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

float txt_width(char *text) {
  float top,bot,left,right;
  fnt->getTextSize(text,right,bot);
  return right;
}

void reset_HUD() {
  int i;
  for(i=0;i<7;i++) {
    hud[i].label[0]='\0';
  }
}

void hud_init() {
	fnt=new Font("helvetica_bold.txf");
	
	fnt->setSize(24);
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

	glLoadIdentity();
	
  for(i=0;i<7;i++) {
    if(hud[i].label[0]!='\0') {
			fnt->setColor(Color(0,0,0,0.4));
			fnt->draw(Vector(hud[i].x+3,hud[i].y+3,0),hud[i].label);
			fnt->setAlpha(1.0);
			fnt->setColor(Color(hud[i].r,hud[i].g,hud[i].b));
			fnt->draw(Vector(hud[i].x,hud[i].y,0),hud[i].label);
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
		fnt->setColor(Color(0,0,0));
		fnt->setAlpha(status_alpha-.2f);
		fnt->draw(Vector(320-(txt_width(status_text)/2)+2,228+2,0),status_text);
		fnt->setColor(Color(status_r,status_g,status_b));
		fnt->setAlpha(status_alpha);
		fnt->draw(Vector(320-(txt_width(status_text)/2),228,0),status_text);
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
