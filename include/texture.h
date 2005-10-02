#ifdef MACOS
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#ifndef _C99_TXR_H
#define _C99_TXR_H

#ifndef DREAMCAST
#ifndef uint16
#define uint16 unsigned int
#endif

#ifndef uint32
#define uint32 unsigned long
#endif
#endif


struct texture {
	unsigned int *pixbuf;
	char filename[50];
#ifdef DREAMCAST
  pvr_ptr_t txraddr;
#endif
	GLint txr;
	int xsize;
	int ysize;
};

void pset(struct texture tex, int x, int y, uint16 col);
uint16 color_fg(int color, int alpha);
void load_floortex();
int load_texture(char *filename, int pos, bool trans);
int load_texture(char *filename, int pos);
void switch_tex(int pos);
void texture_init();
void clear_texture_cache();
int find_tex(char *filename);
int next_tex_id();
void free_textures();
void set_texture_theme(char *directory);
void set_trans(int pos, float val);

#define CLEAR_PIXEL color_fg(255)

#endif
