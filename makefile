
TARGET = DsOnPsp
OBJS =\
	./melon_core/arm_instructions.o \
	./melon_core/armcpu.o \
	./melon_core/bios.o \
	./melon_core/cflash.o \
	./melon_core/cp15.o \
	./melon_core/ctrlssdl.o \
	./melon_core/debug.o \
	./melon_core/Disassembler.o \
	./melon_core/decrypt.o \
	./melon_core/FIFO.o \
	./melon_core/fs-psp.o \
	./melon_core/gfx3d.o \
	./melon_core/GPU.o \
	./melon_core/matrix.o \
	./melon_core/mc.o \
	./melon_core/MMU.o \
	./melon_core/NDSSystem.o \
	./melon_core/render3D.o \
	./melon_core/rtc.o \
	./melon_core/ROMReader.o \
	./melon_core/saves.o \
	./melon_core/sndsdl.o \
	./melon_core/SPU.o \
	./melon_core/thumb_instructions.o \
	./melon_core/wifi.o \
	./PSP/Gudraw.o \
	./PSP/vram.o \
	./PSP/FrontEnd.o \
	./PSP/main.o


#./melon_core/matrix.o \


ASM      = psp-as
CC       = psp-gcc

PSP_FW_VERSION = 500
PSPSDK = $(shell psp-config --pspsdk-path)
PSPBIN = $(PSPSDK)/../bin
CFLAGS += $(shell $(PSPBIN)/sdl-config --cflags) -O2 -G0 -ffast-math -Wall	# todo: more flags for speedup?
# removed -O3 for stability

INCDIR = $(PSPDEV)/psp/include/SDL/ ./PSP ./melon_core

CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
#psp-as ./melon_core/matrix_psp_asm.s -o
ASFLAGS =
#$(ASM) ./melon_core/matrix.s -o ./melon_core/matrix.o \

LIBDIR = $(PSPDEV)/psp/include/SDL
LDFLAGS =
LIBS = -lSDLmain -lSDL -lGL -lGLU -glut -lpspgum -lpspgu -lpspge -lpspaudio -lm  -lpspirkeyb -lpsppower -lpspwlan -lpspvfpu -lpsprtc -lpsphprm

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = DSonPSP v0.8



PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
