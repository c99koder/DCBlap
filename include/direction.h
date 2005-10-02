#define MOVE_UP 1
#define MOVE_DOWN 2
#define MOVE_LEFT 3
#define MOVE_RIGHT 4
#define START_BTN 5
#define FIRE_BTN 6
#define QUIT_BTN 7

int get_controls(int num);
void analog_init();
void read_analog(float *x, float *y);
