#include "../wolftypes.h"
#include "../io.h"
#include <conio.h>
#include <string.h>
#include <6502.h>
#include <stdio.h>
#include <stdlib.h>
#include <c64.h>

#define vicctl (unsigned char *)53265u
#define vicadr (unsigned char *)53272u
#define vicbank (unsigned char *)56576u
#define screen (unsigned char *)0x8000u
#define procio (unsigned char *)1u
#define kernalScreenPtr (unsigned char *)648u

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
const char maxY = 23;

unsigned char wtitle[];

void waitTicks(char ticks)
{
	char i;
	for (i = 0; i < ticks; ++i)
	{
		while (CIA1.ta_lo != 0)
			;
	}
}

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

void installCharset()
{
	char i;

	SEI();
	*procio = (*procio & 251); // enable RAM ,disable IO
	memcpy((char *)RAMCHARS, (char *)0xd000, 0x1000);
	*procio = (*procio | 4); // re-enable IO

	*vicbank = ((*vicbank) & 252) | 1; // select $8000-$bfff as vic bank
	*vicadr = 8;					   // $D018 = %xxxx100x -> charmem is at base + $2000

	*kernalScreenPtr = 128;
	// *vicadr = *vicadr & 240 | 12;
	CLI();

	for (i = 0; i < 8; i++)
	{
		*((unsigned char *)(RAMCHARS + (65 * 8) + i)) = preyC[i];				  // 66 = "P"
		*((unsigned char *)(RAMCHARS + ((65 + 128) * 8) + i)) = 255 - preyC[i];   // 66 = "P"
		*((unsigned char *)(RAMCHARS + (66 * 8) + i)) = wolfC[i];				  // 87 = "W"
		*((unsigned char *)(RAMCHARS + ((66 + 128) * 8) + i)) = 255 - (wolfC[i]); // 215 = inverse "W"
		*((unsigned char *)(RAMCHARS + (67 * 8) + i)) = florC[i];				  // 69 = "E"
		*((unsigned char *)(RAMCHARS + (68 * 8) + i)) = bushC[i];				  // 66 = "B"
		*((unsigned char *)(RAMCHARS + (69 * 8) + i)) = treeC[i];				  // 84 = "T"
	}
}

void setupScreen()
{
	memset((screen + 40), it_earth + 65, 999 - 40);
	memset((COLOR_RAM + 40), EARTHCOLOR, 999 - 40);
	drawFrame();
}

void title()
{
	unsigned char *current;
	unsigned char *cScreen;
	unsigned char *cColor;
	unsigned char currentColor;
	unsigned char lineCount = 0;
	unsigned char columnCount=0;
	unsigned char colors[] = {COLOR_BLUE,COLOR_BLUE,COLOR_LIGHTBLUE,COLOR_LIGHTBLUE,COLOR_CYAN};

	textcolor(COLOR_GRAY1);
	clrscr();

	for (lineCount=0;lineCount<5;++lineCount) {
		currentColor = colors[lineCount];
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
	*vicadr = 21; // uppercase
	textcolor(5);
	gotoxy(0, 0);
	cprintf("stephan   katja");
	textcolor(2);
	gotoxy(8, 0);
	cprintf("%c", 211);
	waitTicks(10);
	installCharset();
	srand(CIA1.ta_lo);
	title();
	gotoxy(0,15);
	titlePrompt();
	cgetc();
}
