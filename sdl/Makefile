TIKI_BASE = /home/sam/tiki-dc
CFLAGS += -g -I../include -I../common -DTIKI -I$(TIKI_BASE)/include -I$(TIKI_BASE)/SDL/include -I/usr/include/plib
LD_FLAGS += -L$(TIKI_BASE)/SDL

OBJS = main.o ../common/callback.o \
../common/camera.o \
../common/collide.o \
../common/drawables/imageSet.o \
../common/drawables/menuList.o \
../common/game.o \
../common/hud.o \
../common/main.o \
../common/md2Model.o \
../common/menus/GameSelect.o \
../common/menus/TitleScreen.o \
../common/objects/ball.o \
../common/objects/bonusspawn.o \
../common/objects/camera_o.o \
../common/objects/func_breakable.o \
../common/objects/func_rotating.o \
../common/objects/goal.o \
../common/objects/hyper.o \
../common/objects/init.o \
../common/objects/paddle.o \
../common/objects/paddle_md2.o \
../common/objects/slowdown.o \
../common/objects/speedup.o \
../common/objects/water.o \
../common/word.o \
../common/world.o

all: $(OBJS)
	g++ -L../ $(OBJS) $(TIKI_BASE_LIBS) $(LD_FLAGS) -o tikitest

clean:
	-rm -f $(OBJS) tikitest

include Makefile.rules

