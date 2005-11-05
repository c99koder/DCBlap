/* callback.h - Object callback support functions
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
#ifndef _CALLBACK_H
#define _CALLBACK_H
#include "entity.h"

//Callback type definitions
typedef void (*create_callback)(struct entity *me);
typedef void (*update_callback)(struct entity *, float gt);
typedef void (*message_callback)(struct entity *to, struct entity *from, char *msg);
typedef void (*reset_callback)(void);

//Register an object template
void register_callbacks(char *type,create_callback create, update_callback update,message_callback message, reset_callback reset);

//Send a message to an object
void send_message(struct entity *to, struct entity *from, char *message);

//Send a message to all objects
void broadcast_message(struct entity *from, char *message);

//Execute the create callback for an entity
void run_create_callback(struct entity *ent);

//Get an entity's update callback
update_callback get_update_callback(struct entity *ent);

//Get an entitiy's message callback
message_callback get_message_callback(struct entity *ent);

//Run the reset callbacks on all objects
void run_reset_callbacks();

//Free the object template list
void free_callbacks();
#endif
