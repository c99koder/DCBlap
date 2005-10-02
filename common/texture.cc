/* texture.cc - texture management system
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
#include <stdio.h>
#ifdef DREAMCAST
#include <kos.h>
#include <png/png.h>
#include <jpeg/jpeg.h>
extern "C" {
#include <imageload/imageload.h>
#include <imageload/jitter_table.h>
}
#include <GL/gl.h>
#endif
#include <math.h>
#ifdef WIN32
#include <windows.h>
#ifdef SDL
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#else
#include <gl\glpng.h>
#endif
#include <gl\glu.h>							// Header File For The GLu32 Library
#include <gl\glaux.h>							// Header File For The GLaux Library
#endif
#ifdef LINUX
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <GL/gl.h>
#include <string.h>
#include <malloc.h>
#define FALSE 0
#define TRUE 1
#endif
#ifdef MACOS
#include <SDL/SDL.h>
#include <SDL_Image/SDL_Image.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <string.h>
#define FALSE 0
#define TRUE 1
#endif

#include "texture.h"
int jpeg_to_texture(char *filename, int pos, bool trans);

#define MAX_TEXTURES 35

GLuint texture_ram[MAX_TEXTURES];
struct texture textures[MAX_TEXTURES];

int max_pos=0;
int gltexpos=0;

char tex_theme_dir[100]="\0";

void set_texture_theme(char *dir) {
  strcpy(tex_theme_dir,dir);
}

void clear_texture_cache() {
  int i;
  for(i=0;i<MAX_TEXTURES;i++) {
    textures[i].filename[0]='\0';
#ifdef DREAMCAST
    //if(textures[i].txraddr!=NULL) pvr_mem_free(textures[i].txraddr);
    textures[i].txraddr=NULL;
#endif
  }
  max_pos=0;
  gltexpos=0;
//#ifndef DREAMCAST
  glDeleteTextures(MAX_TEXTURES,&texture_ram[0]);
  glGenTextures(MAX_TEXTURES,&texture_ram[0]);
//#endif
}

void free_textures() {
  int i;
  for(i=0;i<MAX_TEXTURES;i++) {
    textures[i].filename[0]='\0';
#ifdef DREAMCAST
    textures[i].txraddr=NULL;
#endif
  }
  glDeleteTextures(MAX_TEXTURES,&texture_ram[0]);
}

void texture_init() {
  int i;
	printf("Texture_init()\n");
  glGenTextures(MAX_TEXTURES,&texture_ram[0]);
  for(i=0;i<MAX_TEXTURES;i++) {
    textures[i].filename[0]='\0';
#ifdef DREAMCAST
    textures[i].txraddr=NULL;
#endif
  }
}

GLuint next_tex() {
	GLuint ret=texture_ram[gltexpos];
	gltexpos++;
	return ret;
}

int next_tex_id() {
	max_pos++;
	textures[max_pos].txr=texture_ram[max_pos];
	return max_pos;
}

struct texture *get_tex(int index) {
	return &textures[index];
}

int find_tex(char *filename) {
	int index=0;
	for(index=0;index<=max_pos;index++) {
		if(!strcmp(textures[index].filename,filename)) {
			return index;
		}
	}
	return -1;
}

void switch_tex(int pos) {
	glBindTexture(GL_TEXTURE_2D, textures[pos].txr);
}

void pset(struct texture tex, int x, int y, uint16 col) {
	tex.pixbuf[x+y*(tex.xsize)]=col;
}

int load_texture(char *fn3, int pos) {
	load_texture(fn3,pos,0);
}

int load_texture(char *fn3, int pos, bool trans)									// Load Bitmaps And Convert To Textures
{
	char filename[100];
#ifdef DREAMCAST
	kos_img_t *TextureImage=new kos_img_t;
	IMG_INFO info=IMG_INFO_DEFAULTS;
	info.alpha=IMG_ALPHA_FULL;
	int fd;
  int r,g,b,a,i;
  uint16 *t;
#endif
#ifdef DIRECTX
	pngInfo info;
#endif
#ifdef SDL
	SDL_Surface *TextureImage=NULL;
#endif
	char fn2[200];
	char fn[200];
	// Load The Bitmap, Check For Errors, If Bitmap's Not Found Quit

	strcpy(fn,fn3);
	strcpy(filename,fn);
	strcat(filename,".jpg");
	
	strcpy(fn2,fn3);
	strcat(fn2,".jpg");
    textures[pos].txr=texture_ram[pos];
	glBindTexture(GL_TEXTURE_2D, textures[pos].txr);
	//printf("fn2: %s\n",fn2);
#ifdef DIRECTX
	if (pngLoad(fn2,PNG_NOMIPMAP, PNG_ALPHA, &info)) {
#endif
#ifdef SDL
	if ((TextureImage=IMG_Load(fn2))||(TextureImage=IMG_Load(filename))) {
#endif
#ifdef DREAMCAST

		jpeg_to_img(fn2, 1, TextureImage);
		//png_to_img(fn2, (trans==1) ? PNG_FULL_ALPHA : PNG_NO_ALPHA, TextureImage);
floaded:
        if(1) {
#endif
#ifdef DIRECTX
		textures[pos].xsize=info.Width;
		textures[pos].ysize=info.Height;
#else
		textures[pos].xsize=TextureImage->w;
		textures[pos].ysize=TextureImage->h;
#endif
  	if(pos>max_pos) { max_pos=pos; }
	  strcpy(textures[pos].filename,fn3);
  printf("Loading %s (%i x %i)...\n",fn3,textures[pos].xsize,textures[pos].ysize);
		// Typical Texture Generation Using Data From The Bitmap
#ifdef SDL
		glTexImage2D(GL_TEXTURE_2D, 0,  (TextureImage->format->BitsPerPixel== 32) ? GL_RGBA8 : GL_RGB8, TextureImage->w, TextureImage->h, 0, (TextureImage->format->BitsPerPixel== 32) ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, TextureImage->pixels);
#endif
#ifdef DREAMCAST
    if(textures[pos].txraddr!=NULL) {
      printf("Freeing existing texture...\n");
      pvr_mem_free(textures[pos].txraddr);
    }
		printf("Allocating memory...\n");
    textures[pos].txraddr=pvr_mem_malloc(TextureImage->w*TextureImage->h*2);
		printf("Loading texture...\n");
		//png_to_texture(fn2,textures[pos].txraddr,PNG_NO_ALPHA);
    pvr_txr_load_ex((void*)TextureImage->data, (void *)textures[pos].txraddr, TextureImage->w, TextureImage->h, PVR_TXRLOAD_16BPP|PVR_TXRLOAD_INVERT_Y);
    if(trans) {
      glKosTex2D(GL_ARGB4444_TWID, TextureImage->w, TextureImage->h, (void*)textures[pos].txraddr);
    } else {
      glKosTex2D(GL_RGB565_TWID, TextureImage->w, TextureImage->h, (void*)textures[pos].txraddr);
    }
		glTexEnvi(GL_TEXTURE_2D,GL_TEXTURE_ENV_MODE,GL_MODULATEALPHA);
#endif
#ifndef DREAMCAST
  	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR/*GL_NEAREST*/);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR/*GL_NEAREST*/);
#endif
#ifdef DREAMCAST
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_FILTER,GL_FILTER_BILINEAR);
#endif
//glTexEnvi(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_DECAL);
        } else {
          return -1;
        }

#if defined(DREAMCAST) || defined(DIRECTX)
	if (1)
#else
	if(TextureImage)
#endif
	{
#ifdef SDL
		SDL_FreeSurface(TextureImage);								// Free The Image Structure
#endif
#ifdef DREAMCAST
    printf("Freeing TextureImage...\n");
		//free(TextureImage->data);
		free(TextureImage);
#endif
    } else {
      return -1;
    }
  return pos;
}

int jpeg_to_texture(char *filename,int pos, bool trans) {
#ifdef WIN32
  return -1;
#else
  return -1;
#endif
}
