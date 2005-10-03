#ifndef _CALLBACK_H
#define _CALLBACK_H
#include "entity.h"

typedef void (*create_callback)(struct entity *me);
typedef void (*update_callback)(struct entity *, float gt);
typedef void (*message_callback)(struct entity *to, struct entity *from, char *msg);
typedef void (*reset_callback)(void);

void register_callbacks(char *type,create_callback create, update_callback update,message_callback message, reset_callback reset);
void send_message(struct entity *to, struct entity *from, char *message);
void broadcast_message(struct entity *from, char *message);
void run_create_callback(struct entity *ent);
update_callback get_update_callback(struct entity *ent);
message_callback get_message_callback(struct entity *ent);
void run_reset_callbacks();
void free_callbacks();
#endif
