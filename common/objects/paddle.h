void paddle_create(struct entity *me);
void paddle_update(struct entity *me,float gt);
void paddle_message(struct entity *me, struct entity *them, char *message);
void paddle_reset();
void set_player_map(int pos, int val);