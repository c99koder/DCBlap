float cam_x();
float cam_y();
float cam_z();
float cam_xrot();
float cam_yrot();
float cam_zrot();
float cam_zoom();
void zoom(float step);
void move_camera(float xstep, float ystep, float zstep);
void rotate_camera(float xstep, float ystep, float zstep);
void put_camera(float x, float y, float z, float xrot, float yrot, float zrot);
void no_move_camera(int mode);

extern float camera_x;
extern float camera_y;
extern float camera_z;
extern float camera_xrot;
extern float camera_yrot;
extern float camera_zrot;
