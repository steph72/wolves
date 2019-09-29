#include <conio.h>
#include <stdio.h>
#include <string.h>
#include "wolftypes.h"
#include "io.h"

extern char buf[]; // from wolves.c

extern const char color_levelTitle;
extern const char color_levelDisplay;
extern const char color_levelDescription;

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
    sprintf(buf, "with %d bushes and %d trees.",
            aLevel->numBushes, aLevel->numTrees);
    center(buf);
    puts("\n\n");
    cgetc();
}