#include "../wolftypes.h"
#include "../io.h"
#include "charset.h"
#include <conio.h>
#include <6502.h>
#include <plus4.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define screen (unsigned char *)0xc00u

#define EARTHCOLOR BCOLOR_YELLOW | CATTR_LUMA1

#define BUSHCOLOR BCOLOR_BROWN | CATTR_LUMA4
#define TREECOLOR BCOLOR_GREEN | CATTR_LUMA5
#define WOLFCOLOR BCOLOR_CYAN | CATTR_LUMA6
#define PREYCOLOR BCOLOR_RED | CATTR_LUMA5

const char color_frame = BCOLOR_BLUE | CATTR_LUMA3;
const char color_levelTitle = BCOLOR_CYAN | CATTR_LUMA6 | CATTR_BLINK;
const char color_levelDisplay = BCOLOR_PURPLE | CATTR_LUMA6;
const char color_levelDescription = BCOLOR_GREEN | CATTR_LUMA5;

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

unsigned char wtitle[];


void waitTicks(char t)
{
	char i;
	for (i = 0; i < t; ++i)
	{
		TED.t1_lo = 0;
		TED.t1_hi = 10;
		while (TED.t1_hi != 0)
			;
	}
}

itemType itemAtPos(char x, char y)
{
	return *(screen + x + (40 * y)) - 65;
}

void putItemAtPos(unsigned char x, unsigned char y, itemType item)
{
	char blink = 0;
	char col;

	if ((item & 0x7f) == it_wolf)
	{
		if (item & 0x80)
		{
			// TED noice-ness: let the current wolf blink
			blink = CATTR_BLINK;
		}
	}

	*(screen + x + (40 * y)) = 65 + item;
	item &= 0x7f; // delete rvs flag if set
	col = colors[item];
	*(COLOR_RAM + x + (40 * y)) = col | blink;
}

void setupScreen()
{
	char i;

	memset((screen + 40), it_earth + 65, 999 - 80);
	memset((COLOR_RAM + 40), EARTHCOLOR, 999 - 80);

	drawFrame();
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

void title()
{
	unsigned char *current;
	unsigned char *cScreen;
	unsigned char *cColor;
	unsigned char currentColor;
	unsigned char lineCount = 0;
	unsigned char columnCount=0;
	unsigned char colors[] = {BCOLOR_PURPLE,BCOLOR_DARKBLUE,BCOLOR_LIGHTBLUE,BCOLOR_LIGHTBLUE,BCOLOR_CYAN};

	textcolor(BCOLOR_WHITE | CATTR_LUMA1);
	clrscr();

	// TED rules!
	for (lineCount=0;lineCount<5;++lineCount) {
		currentColor = colors[lineCount]+0x00+(lineCount*0x10);
		for (columnCount=0;columnCount<40;columnCount++) {
			*(COLOR_RAM+((5+lineCount)*40)+columnCount) = currentColor;
		}
	}

	cScreen = screen;
	current = wtitle;
	do
	{
		*cScreen++ = *current++;
	} while (*current != 0xff);
}

void initMachineIO()
{
	bordercolor(0);
	bgcolor(0);
	textcolor(0x55);
	TED.char_addr = 0xd0;
	gotoxy(0, 0);
	cprintf("stephan   katja");
	textcolor(0x52);
	gotoxy(8, 0);
	cprintf("%c", 211);
	waitTicks(7);
	copychars();
	installChars();
	srand(TED.t1_lo);
	title();
	gotoxy(0,14);
	textcolor(BCOLOR_CYAN|CATTR_LUMA3);
	titlePrompt();
	cgetc();
}
