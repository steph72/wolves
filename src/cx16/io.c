#include "../wolftypes.h"
#include <conio.h>
#include <string.h>
#include <6502.h>
#include <stdio.h>
#include <stdlib.h>
#include <cx16.h>
#include <time.h>

#define EARTHCOLOR COLOR_BROWN
#define BUSHCOLOR COLOR_ORANGE
#define TREECOLOR COLOR_GREEN
#define WOLFCOLOR COLOR_CYAN
#define PREYCOLOR COLOR_RED

#define RAMCHARS 0xa000

const char color_frame = COLOR_LIGHTBLUE;
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
const char maxY = 28;

unsigned char wtitle[];
void center(char *aString);

void waitTicks(char ticks)
{
	int i;
	int cx16ticks;
	cx16ticks = ticks*24;
	for (i = 0; i < cx16ticks; ++i)
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
	char col;
	vpoke(65 + item, (x * 2) + (256 * y));
	item &= 0x7f; // delete rvs flag if set
	col = colors[item];
	vpoke(col, 1 + (x * 2) + (256 * y));
}

void installCharset()
{
	unsigned char i;
	const long charset = 0x0f800;

	for (i = 0; i < 8; ++i)
	{
		vpoke(preyC[i], charset + (65 * 8) + i);
		vpoke(255 - preyC[i], charset + ((128 + 65) * 8) + i);
		vpoke(wolfC[i], charset + (66 * 8) + i);
		vpoke(255 - wolfC[i], charset + ((128 + 66) * 8) + i);
		vpoke(florC[i], charset + (67 * 8) + i);
		vpoke(bushC[i], charset + (68 * 8) + i);
		vpoke(treeC[i], charset + (69 * 8) + i);
	}
}

void setupScreen()
{
	register int x, y;
	for (x = minX; x <= maxX; ++x)
	{
		for (y = minY; y <= maxY; ++y)
		{
			vpoke(65 + it_earth, (x * 2) + (256 * y));
			vpoke(colors[it_earth], 1 + (x * 2) + (256 * y));
		}
	}
	textcolor(color_frame);
	cvlinexy(minX - 1, minY, maxY - minY + 1);
	cvlinexy(maxX + 1, minY, maxY - minY + 1);
	chlinexy(minX, maxY + 1, maxX - minX + 1);
	cputcxy(0, 0, 176); // corners
	cputcxy(maxX + 1, 0, 174);
	cputcxy(0, maxY + 1, 173);
	cputcxy(maxX + 1, maxY + 1, 189);
}

void title(void) {
	unsigned char* current;
	int adr;
	unsigned char x = 255;
	adr = 0;
	current = wtitle;
	do {
		if (++x==40) {
			adr += 0x100;
			x=0;
		}			
		vpoke(*current++, (x * 2) + adr);
	} while (*current!=0xff);
	gotoxy(0,14);
	center ("a strategy game for the cx16");
	center ("by stephan kleinert");
	gotoxy(0,18);
	center ("hit 'i' for instructions or");
	center ("any other key to start the game");
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
	title();
	cgetc();
}



void restoreMessageSpace()
{
	textcolor(color_frame);
	chlinexy(minX, minY - 1, maxX - minX + 1);
}
