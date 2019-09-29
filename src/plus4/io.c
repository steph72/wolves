#include "../wolftypes.h"
#include "charset.h"
#include <conio.h>
#include <6502.h>
#include <plus4.h>
#include <string.h>
#include <stdio.h>

#define screen (unsigned char *)0xc00u

#define FRAMECOLOR BCOLOR_BLUE | CATTR_LUMA5
#define EARTHCOLOR BCOLOR_YELLOW | CATTR_LUMA1

#define BUSHCOLOR BCOLOR_BROWN | CATTR_LUMA4
#define TREECOLOR BCOLOR_GREEN | CATTR_LUMA5
#define WOLFCOLOR BCOLOR_CYAN | CATTR_LUMA6
#define PREYCOLOR BCOLOR_RED | CATTR_LUMA5

const char preyC[] = {0, 0, 2, 65, 253, 63, 30, 54};
const char wolfC[] = {0, 1, 65, 254, 62, 34, 34, 102};
const char florC[] = {0, 4, 64, 0, 4, 0, 32, 1};
const char bushC[] = {0, 4, 64, 0, 84, 170, 84, 40};
const char treeC[] = {0, 44, 94, 110, 60, 24, 24, 25};

const char colors[] = {PREYCOLOR, WOLFCOLOR, EARTHCOLOR, BUSHCOLOR, TREECOLOR};

const char minX = 1;
const char minY = 1;
const char maxX = 38;
const char maxY = 22;

itemType itemAtPos(char x, char y)
{
	return *(screen + x + (40 * y)) - 65;
}

void putItemAtPos(unsigned char x, unsigned char y, itemType item)
{
	char col;
	*(screen + x + (40 * y)) = 65 + item;
	item &= 0x7f; // delete rvs flag if set
	col = colors[item];
	*(COLOR_RAM + x + (40 * y)) = col;
}

void putCanvasItem(position *aPos, char item)
{
	putItemAtPos(aPos->x, aPos->y, item);
}

void displayThing(thing *aThing, char hilite)
{
	unsigned char dispChar;
	dispChar = aThing->type + (hilite * 128);
	putItemAtPos(aThing->pos.x, aThing->pos.y, dispChar);
}

void setupScreen()
{
	char i;

	memset((screen + 40), it_earth + 65, 999 - 80);
	memset((COLOR_RAM + 40), EARTHCOLOR, 999 - 80);

	memset((screen+960),160,39);
	memset((COLOR_RAM+960),FRAMECOLOR,39);

	for (i = 1; i < 39; i++)
	{
		*(screen + i) = 64;
		*(screen + 920 + i) = 64;
		*(COLOR_RAM + i) = FRAMECOLOR;
		*(COLOR_RAM + 920 + i) = FRAMECOLOR;
	}
	for (i = 1; i < 23; i++)
	{
		*(screen + (40 * i)) = 93;
		*(screen + (40 * i) + 39) = 93;
		*(COLOR_RAM + (40 * i)) = FRAMECOLOR;
		*(COLOR_RAM + (40 * i) + 39) = FRAMECOLOR;
	}
	*(screen) = 112;
	*(screen + 39) = 110;
	*(screen + 920) = 109;
	*(screen + 920 + 39) = 125;
	*(COLOR_RAM) = FRAMECOLOR;
	*(COLOR_RAM + 39) = FRAMECOLOR;
	*(COLOR_RAM + 920) = FRAMECOLOR;
	*(COLOR_RAM + 920 + 39) = FRAMECOLOR;

}

void installChars()
{
	unsigned char i;
	for (i = 0; i < 8; i++)
	{
		*((unsigned char *)(0xf000 + (65 * 8) + i)) = preyC[i]; // 66 = "A" -- prey
		*((unsigned char *)(0xf000 + (66 * 8) + i)) = wolfC[i]; // 87 = "B" -- wolf
		*((unsigned char *)(0xf000 + (67 * 8) + i)) = florC[i]; // 69 = "C" -- earth
		*((unsigned char *)(0xf000 + (68 * 8) + i)) = bushC[i]; // 66 = "D" -- bush
		*((unsigned char *)(0xf000 + (69 * 8) + i)) = treeC[i]; // 84 = "E" -- tree
	}
}

void dbgNumPrey(char num)
{
	*(screen) = '0' + num;
}

void initMachineIO()
{
	bordercolor(0);
	bgcolor(0);
	textcolor(0x55);
	gotoxy(0, 0);
	cprintf("stephan   katja");
	textcolor(0x52);
	gotoxy(8, 0);
	cprintf("%c", 211);
	copychars();
	installChars();
}

void restoreLowerFrame()
{
	char i;
	for (i = 1; i < 39; i++)
	{
		*(screen + i) = 64;
		*(COLOR_RAM + i) = FRAMECOLOR;
	}
}

void displayPackEnergy(int packEnergy) {
	gotoxy(35,24);
	textcolor(FRAMECOLOR);
	revers(1);
	printf("%4d",packEnergy);
	revers(0);
}

char updateStatus(char *currentWolfName, char *statusLine)
{
	char shouldUpdateAgain;
	shouldUpdateAgain = false;

	memset((screen+960),160,30);
	memset((COLOR_RAM+960),FRAMECOLOR,30);

	*(screen + 960) = 245;
	*(screen + 960 + 39) = 246;
	*(COLOR_RAM + 960) = FRAMECOLOR;
	*(COLOR_RAM + 960 + 39) = FRAMECOLOR;
	gotoxy(1, 24);
	revers(1);
	textcolor(FRAMECOLOR);
	cprintf("%s ", currentWolfName);
	revers(0);
	if (statusLine != NULL)
	{
		restoreLowerFrame();
		gotoxy(2, 0);
		textcolor(FRAMECOLOR);
		revers(1);
		cprintf(statusLine);
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
