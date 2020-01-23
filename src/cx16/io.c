#include "../wolftypes.h"
#include <conio.h>
#include <string.h>
#include <6502.h>
#include <stdio.h>
#include <stdlib.h>
#include <cx16.h>

#define EARTHCOLOR COLOR_BROWN
#define BUSHCOLOR COLOR_ORANGE
#define TREECOLOR COLOR_GREEN
#define WOLFCOLOR COLOR_CYAN
#define PREYCOLOR COLOR_RED

#define RAMCHARS 0xa000

const char color_frame = COLOR_BLUE;
const char color_levelTitle = COLOR_CYAN;
const char color_levelDisplay = COLOR_PURPLE;
const char color_levelDescription = COLOR_GREEN;

const char wolfC[] = {0, 1, 65, 254, 62, 34, 34, 102};
const char preyC[] = {0, 0, 2, 65, 253, 63, 30, 54};
const char treeC[] = {0, 44, 94, 110, 60, 24, 24, 25};
const char bushC[] = {0, 4, 64, 0, 84, 170, 84, 40};
const char florC[] = {0, 4, 64, 0, 4, 0, 32, 1};

const char colors[] = {PREYCOLOR, WOLFCOLOR, EARTHCOLOR, BUSHCOLOR, TREECOLOR};

const char minX = 1;
const char minY = 1;
const char maxX = 38;
const char maxY = 22;

void waitCIATicks(char ticks)
{
	char i;
	return;
	for (i = 0; i < ticks; ++i)
	{
		while (VIA1.t1_lo != 0)
			;
	}
}

itemType itemAtPos(char x, char y)
{
	return vpeek((2 * x) + (256 * y)) - 65;
}

void putItemAtPos(unsigned char x, unsigned char y, itemType item)
{
	vpoke(65 + item, (x * 2) + (256 * y));
}

void installCharset()
{
}

void setupScreen()
{
	register int x, y;
	for (x = 1; x < 39; ++x)
	{
		for (y = 1; y < 24; ++y)
		{
			vpoke(65 + it_earth, (x * 2) + (256 * y));
		}
	}
}

void initMachineIO()
{
	videomode(VIDEOMODE_40COL);

	cbm_k_bsout(0x8e); // select graphic charset
	cbm_k_bsout(0x08); // disable c= + shift
	bgcolor(0);
	bordercolor(0);
	textcolor(5);
	clrscr();

	installCharset();
	srand(VIA1.t1_lo);
}

void restoreLowerFrame()
{
}

char updateStatus(char *currentWolfName, char *statusLine)
{
	unsigned char i;
	char rvs;
	char shouldUpdateAgain;
	shouldUpdateAgain = false;

	gotoxy(1, 24);
	revers(1);
	textcolor(color_frame);
	cprintf("%s ", currentWolfName);
	revers(0);
	if (statusLine != NULL)
	{
		restoreLowerFrame();
		textcolor(color_frame);
		for (i = 0; i < 5; ++i)
		{
			rvs = !rvs;
			gotoxy(2, 0);
			revers(rvs);
			cprintf(statusLine);
			waitCIATicks(10);
		}
		statusLine = NULL;
		shouldUpdateAgain = true;
		revers(0);
	}
	else
	{
		restoreLowerFrame();
	}
	return shouldUpdateAgain;
}
