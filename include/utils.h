void delay(float seconds);
void set_doublespeed(int speed);
#ifdef WIN32
void audio_init();
void play_bgm(LPCWSTR fn);
void stop_bgm();
void audio_shutdown();
void c99_net_init();
void socket_write_line(SOCKET socket, char *text);
void socket_read_line(SOCKET socket, char *return_text);
SOCKET connect_to_host(char *ip, int port);
SOCKET wait_for_connect(int port);
#endif
