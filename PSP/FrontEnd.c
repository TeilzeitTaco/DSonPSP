#include <pspkernel.h>
#include <pspctrl.h>
#include <pspdebug.h>
#include <pspgu.h>
#include <psprtc.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <string.h>

#include "FrontEnd.h"
#include "Version.h"

typedef struct configparm{
	char name[32];
	long *var;
}configP;

typedef struct fname{
	char name[256];
}f_name;

typedef struct flist{
	f_name fname[256];
	int cnt;
}f_list;

enum {
	EXT_NDS = 1,
	EXT_GZ = 2,
	EXT_ZIP = 4,
	EXT_UNKNOWN = 8,
};

const struct {
	char *szExt;
	int nExtId;
} stExtentions[] = {
	{"nds",EXT_NDS},
	{NULL, EXT_UNKNOWN}
};

configP configparms[30];
f_list filelist;

int totalconfig=0;
int selposconfig=0;
int frameposconfig=0;
int langposconfig=0;
int selpos=0;

char app_path[128];
char romname[256];

void initConfigParms() {
	int c=0;

	strcpy(configparms[c].name, "Screen <--> SWAP");
	swap = configparms[c].var;
	c++;
	strcpy(configparms[c].name, "Show FPS");
	showfps = configparms[c].var;
	c++;
	strcpy(configparms[c].name, "Enable Audio");
	enable_sound = configparms[c].var;
	c++;
	strcpy(configparms[c].name, "frameskip");
	frameskip = configparms[c].var;
	c++;
	strcpy(configparms[c].name, "language");
	lang = configparms[c].var;
	c++;

	totalconfig = c;
}

void printConfigParms() {
	int c=0;

	for (c=0; c<totalconfig; c++) {
		if (selposconfig == c) {
			pspDebugScreenSetTextColor(0x0000ffff); // Yellow
		} else {
			pspDebugScreenSetTextColor(0xffffffff); // Red
		}

		pspDebugScreenPrintf("  %s :  %d\n", configparms[c].name, configparms[c].var);
	}
}

void doConfig() {
	int done=0;

	SceCtrlData pad, oldPad;
	pspDebugScreenSetXY(0,0);

	while (!done) {
		sceDisplayWaitVblankStart();

		pspDebugScreenSetTextColor(0xffffffff);
		pspDebugScreenSetXY(1, 0);

		pspDebugScreenPrintf("\n\n");
		pspDebugScreenPrintf("  DSonPSP configuration\n\n");
		pspDebugScreenPrintf("  When you have finished your configuration, press START.\n\n");
		pspDebugScreenPrintf("  Language configuration: 0 = JAP, 1 = ENG, 2 = FRE, 3 = GER,\n");
		pspDebugScreenPrintf("  4 = ITA, 5 = SPA, 6 = CHI, 7 = RES\n");
		pspDebugScreenPrintf("\n\n");

		initConfigParms();
		printConfigParms();

		if (sceCtrlPeekBufferPositive(&pad, 1)) {
			if (pad.Buttons != oldPad.Buttons) {
				if(pad.Buttons & PSP_CTRL_LEFT) {
					if(!strcmp(configparms[selposconfig].name, "language")) {
						configparms[selposconfig].var = langposconfig--;
						if(langposconfig == -1)langposconfig = 0;
					} else {
						if(!strcmp(configparms[selposconfig].name, "frameskip")) {
							configparms[selposconfig].var = frameposconfig--;
							if(frameposconfig == -1)frameposconfig = 0;
						} else {
							configparms[selposconfig].var = 0;
						}
					}
				}

				if(pad.Buttons & PSP_CTRL_RIGHT) {
					if(!strcmp(configparms[selposconfig].name, "language")) {
						configparms[selposconfig].var = langposconfig++;
						if(langposconfig == 8)langposconfig = 7;
					} else
					if(!strcmp(configparms[selposconfig].name, "frameskip")) {
						configparms[selposconfig].var = frameposconfig++;
						if(frameposconfig == 10)frameposconfig = 9;
					} else {
						configparms[selposconfig].var = 1;
					}
				}

				if(pad.Buttons & PSP_CTRL_START) {
					done = 1; // delay
					break;
				}
				if(pad.Buttons & PSP_CTRL_UP) {
					if(selposconfig > 0)selposconfig--;
				}
				if(pad.Buttons & PSP_CTRL_DOWN) {
					if(selposconfig < totalconfig-1)selposconfig++;
				}

			}
			oldPad = pad;
		}
	}
}

//5SM2SF // The f*ck is this?

void clearFileList() {
	filelist.cnt =0;
}

int hasExtension(char *filename) {
	if(filename[strlen(filename)-4] == '.') {
		return 1;
	}
	return 0;
}

void getExtension(const char *srcfile,char *outext) {
	if(hasExtension((char *)srcfile)){
		strcpy(outext, srcfile + strlen(srcfile) - 3);
	} else {
		strcpy(outext, "");
	}
}

int getExtId(const char *szFilePath) {
	char *pszExt;

	if ((pszExt = strrchr(szFilePath, '.'))) {
		pszExt++;
		int i=0;

		for (i=0; stExtentions[i].nExtId != EXT_UNKNOWN; i++) {
			if (!stricmp(stExtentions[i].szExt,pszExt)) {
				return stExtentions[i].nExtId;
			}
		}
	}
	return EXT_UNKNOWN;
}

void getFileList(const char *root) {
	int dfd=0;
	dfd = sceIoDopen(root);

	if (dfd > 0) {
		SceIoDirent dir;

		while (sceIoDread(dfd, &dir) > 0) {
			if(dir.d_stat.st_attr & FIO_SO_IFDIR) {
				// ignore directories
			} else {
				if (getExtId(dir.d_name) != EXT_UNKNOWN) {
					strcpy(filelist.fname[filelist.cnt].name, dir.d_name);
					filelist.cnt++;
				}
			}
		}
		sceIoDclose(dfd);
	}
}

void displayFileList() {
	int c=0;
	for (c=0; c<filelist.cnt; c++) {
		if (selpos == c) {
			pspDebugScreenSetTextColor(0x0000ffff);
		} else {
			pspDebugScreenSetTextColor(0xffffffff);
		}

		pspDebugScreenPrintf("  %s\n",filelist.fname[c].name);
	}
}

void mainMenu(char *path, char *out) {
	char tmp[256];
	SceCtrlData pad, oldPad;

	clearFileList();
	pspDebugScreenSetXY(0,0);

	getcwd(app_path,256);
	sprintf(tmp, "%s/NDSROM", app_path);
	getFileList(tmp);

	while(1) {
		sceDisplayWaitVblankStart();

		pspDebugScreenSetTextColor(0xffffffff);
		pspDebugScreenSetXY(1, 0);

		pspDebugScreenPrintf("\n\n");
		pspDebugScreenPrintf("  Welcome to DSonPSP | Based on *DeSmuMe*\n\n");
		pspDebugScreenPrintf("  press CROSS to launch your game\n\n");
		pspDebugScreenPrintf("  press SQUARE now to exit :=X\n\n");
		pspDebugScreenPrintf("  ROM path: %s\n\n\n",tmp);

		displayFileList();

		if (sceCtrlPeekBufferPositive(&pad, 1)) {
			if (pad.Buttons != oldPad.Buttons) {
				if (pad.Buttons & PSP_CTRL_SQUARE) {
					sceKernelExitGame();
				}

				if (pad.Buttons & PSP_CTRL_CROSS) {
					sprintf(out, "%s/%s", tmp, filelist.fname[selpos].name);
					break;
				}

				if (pad.Buttons & PSP_CTRL_UP) {
					if(selpos > 0)selpos--;
				}

				if (pad.Buttons & PSP_CTRL_DOWN) {
					if(selpos < filelist.cnt-1)selpos++;
				}
			}
			oldPad = pad;
		}
	}
}
