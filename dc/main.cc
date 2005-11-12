#include <kos.h>

KOS_INIT_FLAGS(INIT_DEFAULT|INIT_MALLOCSTATS);

extern "C" int tiki_main(int argc, char **argv);

int main(int argc, char **argv) {
  fs_chdir("/pc/Users/sam/Projects/DCBlap/data");

  tiki_main(argc, argv);
}
