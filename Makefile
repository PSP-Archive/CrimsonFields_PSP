TARGET = CrimsonFields
PSPSDK = $(shell psp-config --pspsdk-path)
PSPBIN = $(PSPSDK)/../bin

SDL_CFLAGS := $(shell $(PSPBIN)/sdl-config --cflags)
SDL_LIBS := $(shell $(PSPBIN)/sdl-config --libs)

INCDIR = src/cf src/common
DEFINES = -DHAVE_LIBZ -DHAVE_DIRENT_H
CXXFLAGS+= $(SDL_CFLAGS)
CXXFLAGS+= -O2 -G0 -Wall -fno-exceptions
CXXFLAGS+= $(DEFINES)
 
OBJS =	src/common/button.o \
		src/common/extwindow.o \
		src/common/fileio.o \
		src/common/filewindow.o \
		src/common/font.o \
		src/common/gamewindow.o \
		src/common/hexsup.o \
		src/common/lang.o \
		src/common/list.o \
		src/common/listselect.o \
		src/common/lset.o \
		src/common/mapview.o \
		src/common/mapwidget.o \
		src/common/misc.o \
		src/common/rect.o \
		src/common/SDL_zlib.o \
		src/common/SDL_fakemouse.o \
		src/common/slider.o \
		src/common/sound.o \
		src/common/strutil.o \
		src/common/surface.o \
		src/common/textbox.o \
		src/common/view.o \
		src/common/widget.o \
		src/common/window.o \
		src/cf/ai.o \
		src/cf/building.o \
		src/cf/combat.o \
		src/cf/container.o \
		src/cf/event.o \
		src/cf/game.o \
		src/cf/history.o \
		src/cf/initwindow.o \
		src/cf/main.o \
		src/cf/map.o \
		src/cf/mapwindow.o \
		src/cf/mission.o \
		src/cf/path.o \
		src/cf/player.o \
		src/cf/unit.o \
		src/cf/unitwindow.o \

DEPS = $(OBJS:.o=.d)
SRCS = $(OBJS:.o=.cpp)

LIBS = -lSDL_image -lSDL_mixer -lSDL_ttf $(SDL_LIBS) -lfreetype -lz -lstdc++

.cpp.o:
	@echo Compiling $<...
	@$(CXX) $(CXXFLAGS) -MMD -c $< -o $*.o
	
EXTRA_TARGETS = EBOOT.PBP
EXTRA_CLEAN = cleandeps
PSP_EBOOT_TITLE = Crimson Fields for PSP
PSP_EBOOT_ICON = icon1.png
PSP_EBOOT_ICON1 = icon1.png
PSP_EBOOT_PIC1 = pic1.png
	
include $(PSPSDK)/lib/build.mak

cleandeps:
	rm -f $(DEPS)
	
-include $(DEPS)

#Visual Studio support

ifneq ($VS_PATH),)
CC = vs-psp-gcc
CXX = vs-psp-g++
endif

kx-install: kxploit
ifeq ($(PSP_MOUNT),)
	@echo '*** Error: $$(PSP_MOUNT) undefined. Please set it to for example /cygdrive/e'
	@echo if your PSP is mounted to E: in cygwin.
else
	cp -r "$(TARGET)" $(PSP_MOUNT)/PSP/GAME/
	cp -r "$(TARGET)%" $(PSP_MOUNT)/PSP/GAME/
endif
