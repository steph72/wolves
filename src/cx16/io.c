#include "../wolftypes.h"
#include "../io.h"
#include "../title.h"
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
const char color_help = COLOR_CYAN;
const char color_prompt = COLOR_GRAY2;
const char color_score = COLOR_PURPLE;

const char colors[] = {PREYCOLOR, WOLFCOLOR, EARTHCOLOR, BUSHCOLOR, TREECOLOR};

const char minX = 1;
const char minY = 1;
const char maxX = 38;
const char maxY = 28;

void waitTicks(char ticks)
{
	int i;
	int cx16ticks;
	cx16ticks = ticks * 12; // cx16's via timer runs much faster than c64's cia timer
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

void title(void)
{
	unsigned char *current;
	int adr;
	unsigned char currentColor;
	unsigned char lineCount = 0;
	unsigned char columnCount = 0;
	unsigned char colors[] = {COLOR_BLUE, COLOR_BLUE, COLOR_LIGHTBLUE, COLOR_LIGHTBLUE, COLOR_CYAN};
	unsigned char i = 0;
	unsigned char x = 255;

	textcolor(COLOR_GRAY2);
	clrscr();

	adr = 0x100;
	current = wtitle;

	do
	{
		if (++x == 40)
		{
			adr += 0x100;
			x = 0;
		}
		vpoke(*current++, (x * 2) + adr);
	} while (*current != 0xff);

	for (lineCount = 0; lineCount < 5; ++lineCount)
	{
		currentColor = colors[lineCount];
		for (columnCount = 0; columnCount < 40; columnCount++)
		{
			vpoke(currentColor, 1 + (columnCount * 2) + ((2 + lineCount) * 0x100));
		}
	}
}

void initMachineIO()
{
	videomode(VIDEOMODE_40x30);
	cbm_k_bsout(0x8e); // select graphic charset
	cbm_k_bsout(0x08); // disable c= + shift
	bgcolor(0);
	bordercolor(0);
	textcolor(5);
	clrscr();
	installCharset();
	srand(VIA1.t1_lo);
	title();
}
