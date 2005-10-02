#include <kos.h>
#include <windows.h>
#include <math.h>
#include "entity.h"
#include "callback.h"
#include "player.h"
#include "camera.h"

#define M_PI 3.141592653589793238

//float fsin(float in);
//float fcos(float in);

void player_create(struct entity *me) {
  no_move_camera(1); //disable the "camera" object so we can take over camera motion
  me->yrot+=90;
  me->xmin=-16/100.0f;
  me->ymin=-16/100.0f;
  me->zmin=-16/100.0f;
  me->xmax=16/100.0f;
  me->ymax=16/100.0f;
  me->zmax=16/100.0f;

}

extern bool keys[256];

void player_callback(struct entity *me) {
    if (keys[VK_RETURN]) {
        //arch_exit();
    }
    else if (keys[VK_LEFT]) {
      me->yrot-=2;
    }
    else if (keys[VK_RIGHT]) {
      me->yrot+=2;
    }
    else if (keys[VK_UP]) {
      me->z-=sin(me->yrot*(M_PI/180))*8;
      me->x+=cos(me->yrot*(M_PI/180))*8;
    }
    else if (keys[VK_DOWN]) {
      me->z+=sin(me->yrot*(M_PI/180))*8;
      me->x-=cos(me->yrot*(M_PI/180))*8;
    }
    /*if(cont->buttons & CONT_A) {
      vid_screen_shot("/pc/home/chip/shot.ppm");
    }*/
    put_camera(me->x,me->y+58,me->z,me->xrot,me->yrot+90,me->zrot);
}

