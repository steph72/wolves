#include <conio.h>
#include <stdio.h>
#include <string.h>
#include "wolftypes.h"
#include "io.h"

extern char buf[]; // from wolves.c

extern const char color_levelTitle;
extern const char color_levelDisplay;
extern const char color_levelDescription;
extern const char color_frame;

void restoreMessageSpace();
void waitTicks();


void displayPackEnergy(int packEnergy)
{
    revers(1);
    gotoxy(30, maxY+1);
    cputs("        ");
    gotoxy(30, maxY+1);
    textcolor(color_frame);
    printf("pe: %d", packEnergy);
    revers(0);
}

void displayScore(int score)
{
    gotoxy(20, maxY+1);
    textcolor(color_frame);
    revers(1);
    printf("s: %d", score);
    revers(0);
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

void center(char *aString)
{
    char y;
    y = wherey();
    gotoxy(20 - (strlen(aString) / 2), y);
    puts(aString);
}

void displayLevelTitleCard(char num, level *aLevel)
{
    clrscr();
    textcolor(color_frame);
    chlinexy(0, 0, 40);
    chlinexy(0, maxY+1, 40);
    gotoxy(0, 3);
    textcolor(color_levelDisplay);
    sprintf(buf, "** level %d **", num);
    center(buf);
    puts("\n");
    textcolor(color_levelTitle);
    center(aLevel->title);
    puts("\n");
    textcolor(color_levelDescription);
    sprintf(buf, "hunt %d prey in a forest", aLevel->preyNeeded);
    center(buf);
    sprintf(buf, "with %d bushes and %d trees",
            aLevel->numBushes, aLevel->numTrees);
    center(buf);
    puts("\n\n");
    cgetc();
    clrscr();
}


char updateStatus(char *currentWolfName, char *statusLine)
{
	unsigned char i;
	char rvs;
	char shouldUpdateAgain;
	shouldUpdateAgain = false;

	gotoxy(minX + 1, maxY + 1);
	revers(1);
	textcolor(color_frame);
	cprintf("%s", currentWolfName);
	revers(0);
	chlinexy(minX + strlen(currentWolfName) + 1, maxY + 1, 18 - strlen(currentWolfName) - 2);
	if (statusLine != NULL)
	{
		restoreMessageSpace();
		textcolor(color_frame);
		for (i = 0; i < 5; ++i)
		{
			rvs = !rvs;
			gotoxy(2, 0);
			revers(rvs);
			cprintf(statusLine);
			waitTicks(10);
		}
		statusLine = NULL;
		shouldUpdateAgain = true;
		revers(0);
	}
	else
	{
		restoreMessageSpace();
	}
	return shouldUpdateAgain;
}
