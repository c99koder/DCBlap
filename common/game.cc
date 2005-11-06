/* game.cc - Game support functions
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
#ifdef TIKI
#include <Tiki/tiki.h>
#include <Tiki/texture.h>

using namespace Tiki;
using namespace Tiki::GL;
#endif
#include <stdlib.h>
#include "game.h"
#include "entity.h"

bool win_flag=0;
bool lose_flag=0;
int doublespeed=0;
bool enable_sound=1;
bool enable_music=0;
bool enable_powerups=1;
bool net_host=0;
char next_map[256]="";
struct entity *g1=NULL;
struct entity *g2=NULL;
struct entity *wld=NULL;
int p1_score=0;
int p2_score=0;
int ballcnt=0;
int ballmax=0;

void set_doublespeed(int speed) { doublespeed=speed; }

void game_init(const char *filename) {
	load_world(filename);
	
	g1=find_ent_by_prop("goal","num","1");
  g2=find_ent_by_prop("goal","num","2");
  wld=find_ent_by_type("worldspawn");
  if(get_prop(wld,"ballcnt")!=NULL) {
    wld->arg1=atoi(get_prop(wld,"ballcnt"));
    wld->arg2=atoi(get_prop(wld,"ballcnt"));
  }
  if(get_prop(wld,"score")!=NULL) wld->arg3=atoi(get_prop(wld,"score"));
  if(p1_score>0) g1->arg1=p1_score;
  if(p2_score>0) g2->arg1=p2_score;
  if(ballcnt>0) wld->arg1=ballcnt;
  if(ballmax>0) wld->arg2=ballmax;	
}