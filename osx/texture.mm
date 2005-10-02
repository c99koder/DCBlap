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
extern "C" {
#include <imageload/imageload.h>
#include <imageload/jitter_table.h>
}
#include <GL/gl.h>
#endif
#include <math.h>
#ifdef WIN32
#include <windows.h>
#include <gl\glu.h>							// Header File For The GLu32 Library
#include <gl\glaux.h>							// Header File For The GLaux Library
#endif
#ifdef LINUX
#include <SDL/SDL.h>
#include <GL/gl.h>
#include <string.h>
#include <malloc.h>
#define FALSE 0
#define TRUE 1
#endif
#ifdef MACOS
#include <Cocoa/cocoa.h>
//#include <SDL/SDL.h>
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

#ifdef WIN32
AUX_RGBImageRec *LoadBMP(char *Filename)  {
	FILE *File=NULL;

	File=fopen(Filename,"r");
	if (File) {
		fclose(File);
		return auxDIBImageLoad(Filename);
	}
	return NULL;
};
#endif

int load_texture(char *fn3, int pos) {
	load_texture(fn3,pos,0);
}

int load_texture(char *fn3, int pos, bool trans)									// Load Bitmaps And Convert To Textures
{
	char filename[100];
#ifdef DREAMCAST
	kos_img_t *TextureImage=new kos_img_t;
	IMG_INFO info=IMG_INFO_DEFAULTS;
	int fd;
  int r,g,b,a,i;
  uint16 *t;
#endif
#ifdef WIN32
	AUX_RGBImageRec *TextureImage=NULL;	// Create Storage Space For The Texture
#endif
#ifdef SDL
	SDL_Surface *TextureImage=NULL;
#endif
#ifdef MACOS
  //NSBitmapImageRep *bitmap;
	//NSImage *image; 
#endif
	char fn2[200];
	char fn[200];
	// Load The Bitmap, Check For Errors, If Bitmap's Not Found Quit
#ifdef WIN32
	strcpy(fn,fn3);
	strcpy(filename,fn);
	strcat(filename,".jpg");
  if(jpeg_to_texture(filename,pos,trans)==1) {
  	if(pos>max_pos) { max_pos=pos; }
	  strcpy(textures[pos].filename,fn3);
    return pos;
  }
#endif
	strcpy(fn,fn3);
	strcpy(filename,fn);
	strcat(filename,".png");
	printf("Loading: %s\n",filename);
	if(tex_theme_dir[0]!='\0') {
	  strcpy(fn2,tex_theme_dir);
	  strcat(fn2,filename);
	  printf("Theme alternative: %s\n",fn2);
        }
#ifdef WIN32
	if ((tex_theme_dir[0]!='\0'&&(TextureImage=LoadBMP(fn2))) || (TextureImage=LoadBMP(filename))) {
#endif
#ifdef SDL
	if ((tex_theme_dir[0]!='\0'&&(TextureImage=SDL_LoadBMP(fn2)))||(TextureImage=SDL_LoadBMP(filename))) {
#endif
#ifdef DREAMCAST
    sprintf(fn2,"%s.bmp",fn);
		fd=fs_open(fn2,O_RDONLY);
		if(fd) {
			fs_close(fd);
			img_load_file(fn2, &info, TextureImage);
      goto floaded;
		}
    sprintf(fn2,"%s.jpg",fn);
		fd=fs_open(fn2,O_RDONLY);
		if(fd) {
			fs_close(fd);
			img_load_file(fn2, &info, TextureImage);
      goto floaded;
		}
    return -1;
floaded:
        if(1) {
#endif
#ifdef MACOS
 // to use a picture, it must be included in the resources (under groups and files) // or alternatively, it may have been loaded as an image and then had a name set // initialize the image to the correct file (name)

    NSImage *image=[[NSImage alloc] initByReferencingFile: [NSString stringWithCString:filename]];
// create a bitmap with the correct image data
 //NSData *raw = [NSData dataWithContentsOfFile: [NSString stringWithCString:filename]];

//NSBitmapImageRep *bitmap =[NSBitmapImageRep imageRepWithData: raw];

    NSBitmapImageRep *bitmap = [[NSBitmapImageRep alloc]initWithData: [image TIFFRepresentation]];
// if this bitmap is null, we should really free the texture

    if (bitmap != nil) {
#endif
#ifdef WIN32
		textures[pos].xsize=TextureImage->sizeX;
		textures[pos].ysize=TextureImage->sizeY;
#endif
#if defined (SDL) || defined(DREAMCAST)
		textures[pos].xsize=TextureImage->w;
		textures[pos].ysize=TextureImage->h;
#endif
#ifdef MACOS
		textures[pos].xsize=(int)[bitmap size].width;
		textures[pos].ysize=(int)[bitmap size].height;
		//Compensate for buggy NSBitmap size
		if(textures[pos].xsize == 511 || textures[pos].xsize == 1023) textures[pos].xsize++; 
		if(textures[pos].ysize == 511 || textures[pos].ysize == 1023) textures[pos].ysize++; 
#endif
  	if(pos>max_pos) { max_pos=pos; }
	  strcpy(textures[pos].filename,fn3);
    textures[pos].txr=texture_ram[pos];
		printf("Loading %s (%i x %i)...\n",fn3,textures[pos].xsize,textures[pos].ysize);
		// Typical Texture Generation Using Data From The Bitmap
		glBindTexture(GL_TEXTURE_2D, textures[pos].txr);
#ifdef WIN32
		glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage->sizeX, TextureImage->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage->data);
#endif
#ifdef SDL
		glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage->w, TextureImage->h, 0, GL_BGR, GL_UNSIGNED_BYTE, TextureImage->pixels);
#endif
#ifdef MACOS
glTexImage2D(GL_TEXTURE_2D, 0, [bitmap bitsPerPixel]==24?GL_RGB8:GL_RGBA, 
                textures[pos].xsize, textures[pos].ysize, 0, 
                [bitmap bitsPerPixel]==24?GL_RGB:GL_RGBA, [bitmap bitsPerPixel]==24?GL_UNSIGNED_BYTE:GL_UNSIGNED_INT_8_8_8_8, [bitmap bitmapData]);
#endif
#ifdef DREAMCAST
    if(textures[pos].txraddr!=NULL) {
      printf("Freeing existing texture...\n");
      pvr_mem_free(textures[pos].txraddr);
    }
    textures[pos].txraddr=pvr_mem_malloc(TextureImage->w*TextureImage->h*2);
    pvr_txr_load_ex((void*)TextureImage->data, (void *)textures[pos].txraddr, TextureImage->w, TextureImage->h, PVR_TXRLOAD_16BPP|PVR_TXRLOAD_INVERT_Y);
    if(trans) {
      t=(uint16*)textures[pos].txraddr;
      for (i=0; i<TextureImage->w*TextureImage->h; i++) {
        r = (t[i] >> 11) & 31;
        g = (t[i] >> 5) & 63;
        b = (t[i] >> 0) & 31;
        if (r < 15 && g < 15 && b <15)
          a = 0x00;
        else
          a = 0x0f;
        t[i] = (a << 12) |
               ((r >> 1) << 8) |
               ((g >> 2) << 4) |
               ((b >> 1) << 0);
      }
      glKosTex2D(GL_ARGB4444_TWID, TextureImage->w, TextureImage->h, (void*)textures[pos].txraddr);
    } else {
      glKosTex2D(GL_RGB565_TWID, TextureImage->w, TextureImage->h, (void*)textures[pos].txraddr);
  		glTexEnvi(GL_TEXTURE_2D,GL_TEXTURE_ENV_MODE,GL_MODULATEALPHA);
    }
#endif
#ifndef DREAMCAST
  	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
#endif
#ifdef DREAMCAST
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_FILTER,GL_FILTER_BILINEAR);
#endif
//glTexEnvi(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_DECAL);
        } else {
          return -1;
        }

#if defined(DREAMCAST) || defined(MACOS)
	if (1)
#else
		if(TextureImage)
#endif
	{
#ifdef WIN32
		if (TextureImage->data)							// If Texture Image Exists
		{
			free(TextureImage->data);					// Free The Texture Image Memory
		}

		free(TextureImage);								// Free The Image Structure
#endif
#ifdef SDL
		SDL_FreeSurface(TextureImage);								// Free The Image Structure
#endif
#ifdef DREAMCAST
    printf("Freeing TextureImage...\n");
		free(TextureImage->data);
		free(TextureImage);
#endif
#ifdef MACOS
	[bitmap release];
	//[raw release];
	[image release];
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
