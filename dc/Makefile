TIKI_BASE = /Users/sam/tiki
KOS_CFLAGS += -g -I../include -I../common -DTIKI -I$(TIKI_BASE)/include -I$(TIKI_BASE)/dc/include -I$(KOS_BASE)/../kos-ports/include/dcplib
KOS_LDFLAGS += -L$(TIKI_BASE)/dc -L$(TIKI_BASE)/3rdparty/kglx/lib

OBJS = main.o \
../common/camera.o \
../common/drawables/imageSet.o \
../common/drawables/menuList.o \
../common/drawables/ShadowBox.o \
../common/drawables/Entity.o \
../common/drawables/SolidClass.o \
../common/hud.o \
../common/main.o \
../common/md2Model.o \
../common/menus/GameSelect.o \
../common/menus/LevelSelect.o \
../common/menus/TitleScreen.o \
../common/objects/ball.o \
../common/objects/bouncer.o \
../common/objects/camera_o.o \
../common/objects/init.o \
../common/objects/light.o \
../common/objects/paddle.o \
../common/objects/player_1st.o \
../common/objects/unknown.o \
../common/utils.o \
../common/word.o \
../common/world.o

all: $(OBJS)
	kos-c++ -L../ $(OBJS) $(TIKI_BASE_LIBS) -o dcblap.elf
	sh-elf-objcopy -R .stack -O binary dcblap.elf dcblap.bin

clean:
	-rm -f $(OBJS) tikitest

include Makefile.rules

