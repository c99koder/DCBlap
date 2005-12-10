/* world.cc - DCBlap world loader and renderer
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
#include <Tiki/gl.h>
#include <Tiki/tikimath.h>
#include <Tiki/texture.h>
#include <Tiki/file.h>
#include <Tiki/tikitime.h>

using namespace Tiki;
using namespace Tiki::GL;
using namespace Tiki::Math;

#include <sg.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "camera.h"
#include "word.h"
#include "utils.h"
#include "objects.h"
#include "hud.h"

#include <vector>

class DCBlapEntityList : public Drawable {
public:
	void addEntity(Entity *e) {
		m_entityList.push_back(e);
		subAdd(e);
	}
	void nextFrame(uint64 tm);
private:
	std::list<Entity*> m_entityList;
};

RefPtr<DCBlapEntityList> EntityList;
	
//Check for collisions
void DCBlapEntityList::nextFrame(uint64 tm) {
	std::list<Entity *>::iterator ent1;
	std::list<Entity *>::iterator ent2;
	Vector p;
	sgBox b1,b2;
	
	for(ent1 = m_entityList.begin(); ent1 != m_entityList.end(); ent1++) {
		if((*ent1)->getClassName()=="worldspawn") continue;
		
		for(ent2 = m_entityList.begin(); ent2 != m_entityList.end(); ent2++) {
			if((*ent2)->getClassName()=="worldspawn") continue;
			if(ent1 != ent2) {
				p=(*ent1)->getMin() + (*ent1)->getTranslate();
				b1.setMin(p.x,p.y,p.z);
				p=(*ent1)->getMax() + (*ent1)->getTranslate();
				b1.setMax(p.x,p.y,p.z);
				
				p=(*ent2)->getMin() + (*ent2)->getTranslate();
				b2.setMin(p.x,p.y,p.z);
				p=(*ent2)->getMax() + (*ent2)->getTranslate();
				b2.setMax(p.x,p.y,p.z);
				
				if(b1.intersects(&b2)) {
					(*ent1)->perform("thud",(*ent2),NULL);
				}
			}
		}
	}
	
	Drawable::nextFrame(tm);
	subRemoveFinished();
}

Entity *create_object(std::string type) {
	Entity *e;
	if(type=="camera") {
		e=new Camera;
	} else if(type=="player") {
		e=new Player;
	} else if(type=="light") {
		e=new Light;
	} else if(type=="ball") {
		e=new Ball;
	} else if(type=="paddle") {
		e=new Paddle;
	} else {
		e=new Unknown(type);
	}
	
	EntityList->addEntity(e);
	return e;
}

std::vector< RefPtr<Texture> > texture_list;

lightNode *lightList=NULL;

struct lightNode *add_light() {
	struct lightNode *current=new lightNode;

	current->next=lightList;
	lightList=current;
	return current;
}

Color compute_light(float px, float py, float pz) {
  struct lightNode *l=lightList;
  float x,y,z,dx,dy,dz,light;
  float r=0,g=0,b=0,r1,g1,b1;
	
	if(l==NULL) return Color(1,1,1);
	
	r1=0;
	g1=0;
	b1=0;
	
	while(l!=NULL) {
		x=l->x;
		y=l->y;
		z=l->z;
		r=l->r;//atoi(get_prop(l,"red"))/100.0f;
		g=l->g;//atoi(get_prop(l,"green"))/100.0f;
		b=l->b;//atoi(get_prop(l,"blue"))/100.0f;
		dx=px-x;
		dy=py-y;
		dz=pz-z;
		light=1.6-(fsqrt(dx*dx+dy*dy+dz*dz)/10.0f)/(l->i);
		if(light>0) {
			r1+=r*light;
			g1+=g*light;
			b1+=b*light;
		}
		l=l->next;
	}
	
	return Color(r1,g1,b1);
}


void free_world_textures() {
	while(!texture_list.empty()) {
		texture_list.pop_back();
	}
}

void load_world_info(char *filename, char *title, char *author, char *rating) {
  unsigned int texcount;
  unsigned int entcount;
  unsigned int paramcount;
  unsigned int i, k;
  char name[100],val[100];
  char texname[100];
	File worldFile;

  strcpy(title,"No title");
  strcpy(author,"No author");
  strcpy(rating,"Not rated");

	assert(file_exists(filename));
	
  worldFile.open(filename,"rb");
	
	//Load textures
	worldFile.readle32(&texcount,1);
  for(i=0;i<texcount;i++) {
    k=0;
    do {
			worldFile.read(&texname[k],1);
      k++;
    } while(texname[k-1]!='\0');
  }
	
	//Load entities
	worldFile.readle32(&entcount,1);
		
	//Load type
	k=0;
	do {
		worldFile.read(&texname[k],1);
		k++;
	} while(texname[k-1]!='\0');
	
	//Load parameters
	worldFile.readle32(&paramcount,1);
		
  for(i=0;i<paramcount;i++) {
    k=0;
    do {
      worldFile.read(&name[k],sizeof(char));
	    k++;
    } while(name[k-1]!='\0');
    k=0;
    do {
	    worldFile.read(&val[k],sizeof(char));
	    k++;
    } while(val[k-1]!='\0');
    if(!strcmp(name,"title")) strcpy(title,val);
    if(!strcmp(name,"author")) strcpy(author,val);
    if(!strcmp(name,"rating")) strcpy(rating,val);
  }
  worldFile.close();
}

World::World(const char *filename) {
  unsigned int i,k,entcount,texcount;
  char texname[100];
	char texname2[100];
	RefPtr<Texture> tex;
	File worldFile;
	RefPtr<Entity> ent;
  
	EntityList = new DCBlapEntityList();
	m_scene->subAdd(EntityList);
	
	assert(file_exists((char *)filename));
	printf("Loading: %s\n",filename);
	worldFile.open(filename,"rb");

	//Load textures
	worldFile.readle32(&texcount,1);
	printf("Textures: %i\n",texcount);
  for(i=0;i<texcount;i++) {
    k=0;
    do {
			worldFile.read(&texname[k],1);
			//Convert to lowercase
			if(texname[k] >= 'A' && texname[k] <= 'Z') texname[k] += 32;
      k++;
    } while(texname[k-1]!='\0');
		sprintf(texname2,"tex/%s.png",texname);
		tex=new Texture(texname2,0);
		texture_list.push_back(tex);
  }
	
	//Load entities
	worldFile.readle32(&entcount,1);
	printf("Entities: %i\n",entcount);

  for(i=0;i<entcount;i++) {
    //printf("Loading entity %i...\n",i);
		
		//Load type
    k=0;
    do {
      worldFile.read(&texname[k],1);
      k++;
    } while(texname[k-1]!='\0');
    //printf("Type: %s\n",texname);
		
		ent=create_object(texname);
		ent->loadFromFile(worldFile);
		printf("Added: %s\n",ent->getClassName().c_str());
  }
  printf("World load complete.\n");
	worldFile.close();
	
	m_startTime=m_lastTime=Time::gettime();
}

void World::visualPerFrame() {
	uint64 frameTime;
	
	m_scene->subRemoveFinished();
	m_scene->createSceneList();
	
	Frame::begin();
	
	if (m_exiting)
		glClearColor(m_bg[0]*m_exitCount, m_bg[1]*m_exitCount, m_bg[2]*m_exitCount, 1.0f);
	else
		glClearColor(m_bg[0], m_bg[1], m_bg[2], 1.0f);

	Frame::set3d();
	glClearDepth(1.0f);
	glDepthFunc(GL_LEQUAL);
	glClear(GL_COLOR_BUFFER_BIT+GL_DEPTH_BUFFER_BIT);
	
	visualOpaqueList();
	
	Frame::transEnable();
	visualTransList();

	Frame::set2d();
	render_HUD();
	
	Frame::finish();
	
	m_scene->destroySceneList();
	
	frameTime=Time::gettime();
	m_scene->nextFrame(frameTime-m_lastTime);
	m_lastTime=frameTime;
}