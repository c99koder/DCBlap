/* main.cc - system intialization for Dreamcast
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
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "texture.h"
#include "text.h"
#include "camera.h"
#include "entity.h"
#include "callback.h"
#include <math.h>
//#include <mp3/sndserver.h>
#include <oggvorbis/sndoggvorbis.h>
#include "objects.h"
#include "word.h"


//extern uint8 romdisk[];

void main_menu();
void play_pong();
void debug(char *) { }

KOS_INIT_FLAGS(INIT_DEFAULT|INIT_THD_PREEMPT|INIT_MALLOCSTATS);
//KOS_INIT_ROMDISK(romdisk);

pvr_init_params_t params = {
        /* Enable opaque and translucent polygons with size 16 */
        { PVR_BINSIZE_16, PVR_BINSIZE_0, PVR_BINSIZE_16, PVR_BINSIZE_0, PVR_BINSIZE_0 },

        /* Vertex buffer size 512K */
        512*1024
};

extern "C" {
  extern char dcblogo[];
  void dcb_vmu_init();
}

int main(int argc, char **argv) {
        pvr_stats_t stats;
        char tmp[100];
	dcb_vmu_init();

	//arch_set_exit_path(ARCH_EXIT_MENU);
  pvr_init(&params);
	sndoggvorbis_init();
	//mp3_init();
	texture_init();
	objects_init();
	fs_chdir("/cd");
	srand(time(NULL));
	vmu_set_icon(dcblogo);
	/* Get basic stuff initialized */
	glKosInit();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, 640.0f / 480.0f, 0.1f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_KOS_AUTO_UV);
	glDisable(GL_CULL_FACE);
  glShadeModel(GL_SMOOTH);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	text_init("helvetica_medium.txf",24);
	main_menu();
	free_textures();
	free_callbacks();
	//sndmp3_shutdown();
	sndoggvorbis_shutdown();
	snd_sfx_unload_all();
	destroy_word_list();
  cdrom_spin_down();
  return 0;
}
