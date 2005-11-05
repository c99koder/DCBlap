/* objects.h - meta-header for Objects
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
#include "callback.h"
#include "objects/camera_o.h"
#include "objects/goal.h"
#include "objects/paddle.h"
#include "objects/paddle_md2.h"
#include "objects/ball.h"
#include "objects/speedup.h"
#include "objects/slowdown.h"
#include "objects/bonusspawn.h"
#include "objects/hyper.h"
#include "objects/func_rotating.h"
#include "objects/water.h"
#include "objects/func_breakable.h"

//Initialize the object templates
void objects_init();
