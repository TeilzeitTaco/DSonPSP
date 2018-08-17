
TARGET = DsOnPsp
OBJS =\
	./melon_core/ARM_InstrTable.o \
	./melon_core/ARM.o \
	./melon_core/ARMInterpreter_ALU.o \
	./melon_core/ARMInterpreter_Branch.o \
	./melon_core/ARMInterpreter_LoadStore.o \
	./melon_core/ARMInterpreter.o \
	./melon_core/Config.o \
	./melon_core/CP15.o \
	./melon_core/DMA.o \
	./melon_core/FIFO.o \
	./melon_core/GPU.o \
	./melon_core/GPU2D.o \
	./melon_core/GPU3D_Soft.o \
	./melon_core/GPU_3D.o \
	./melon_core/melon_fopen.o \
	./melon_core/NDS.o \
	./melon_core/NDSCart.o \
	./melon_core/Platform.o \
	./melon_core/RTC.o \
	./melon_core/ROMReader.o \
	./melon_core/SPI.o \
	./melon_core/SPU.o \
	./melon_core/types.o \
	./melon_core/version.o \
	./melon_core/Wifi.o \
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
