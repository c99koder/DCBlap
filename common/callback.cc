/* callback.cc - Object callback support functions
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
#include <Tiki/texture.h>

using namespace Tiki;
using namespace Tiki::GL;

#include <string.h>
#include <stdlib.h>
#include "drawables/entity.h"
#include "callback.h"

struct callback_list_node {
  char *type;
  create_callback create;
  update_callback update;
  message_callback message;
  reset_callback reset;
  struct callback_list_node *next;
};

struct callback_list_node *callback_list=NULL;

void free_callbacks() {
  struct callback_list_node *current=callback_list;
  
  while(callback_list!=NULL) {
    current=callback_list;
    callback_list=callback_list->next;
    free(current->type);
    free(current);
  }
}

void register_callbacks(char *type, create_callback create, update_callback update, message_callback message, reset_callback reset) {
  struct callback_list_node *current=callback_list;
  struct callback_list_node *prev=NULL;

  while(current) {
    prev=current;
    current=current->next;
  }

  current=(struct callback_list_node *)malloc(sizeof(struct callback_list_node));
  if(prev!=NULL) {
    prev->next=current;
  } else {
    callback_list=current;
  }

  current->type=(char *)malloc(strlen(type)+1);
  strcpy(current->type,type);
  current->create=create;
  current->update=update;
  current->message=message;
  current->reset=reset;
  current->next=NULL;
}

void run_create_callback(struct entity *ent) {
  struct callback_list_node *current=callback_list;
  while(current!=NULL) {
    if(!strcmp(current->type,ent->type) && current->create!=NULL) {
      current->create(ent);
      break;
    }
    current=current->next;
  }
}

update_callback get_update_callback(struct entity *ent) {
  struct callback_list_node *current=callback_list;
    while(current!=NULL) {
      if(!strcmp(current->type,ent->type)) {
	      return current->update;
        break;
      }
      current=current->next;
    }
  return NULL;
}

message_callback get_message_callback(struct entity *ent) {
  struct callback_list_node *current=callback_list;
    while(current!=NULL) {
      if(!strcmp(current->type,ent->type)) {
	      return current->message;
        break;
      }
      current=current->next;
    }
  return NULL;
}

void run_reset_callbacks() {
  struct callback_list_node *current=callback_list;
  while(current!=NULL) {
	  if(current->reset!=NULL) {
			//printf("Running reset for %s\n",current->type);
			current->reset();
		}
	  current=current->next;
  }
}

void send_message(struct entity *to, struct entity *from, char *message) {
  struct callback_list_node *current=callback_list;
  while(current!=NULL) {
    if(!strcmp(current->type,to->type) && current->message!=NULL) {
      current->message(to,from,message);
      break;
    }
    current=current->next;
  }
}
