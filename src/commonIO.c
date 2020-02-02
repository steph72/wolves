#include <conio.h>
#include <stdio.h>
#include <string.h>
#include "wolftypes.h"
#include "io.h"
#include "title.h"
#include "utils.h"

extern char buf[]; // from wolves.c

extern const char color_levelTitle;
extern const char color_levelDisplay;
extern const char color_levelDescription;
extern const char color_frame;

const char wolfC[] = {0, 1, 65, 254, 62, 34, 34, 102};
const char preyC[] = {0, 0, 2, 65, 253, 63, 30, 54};
const char treeC[] = {0, 44, 94, 110, 60, 24, 24, 25};
const char bushC[] = {0, 4, 64, 0, 84, 170, 84, 40};
const char florC[] = {0, 4, 64, 0, 4, 0, 32, 1};

void restoreMessageSpace();

void drawFrame()
{
    textcolor(color_frame);
    cvlinexy(minX - 1, minY, maxY - minY + 1);
    cvlinexy(maxX + 1, minY, maxY - minY + 1);
    chlinexy(minX, maxY + 1, maxX - minX + 1);
    cputcxy(0, 0, 176); // corners
    cputcxy(maxX + 1, 0, 174);
    cputcxy(0, maxY + 1, 173);
    cputcxy(maxX + 1, maxY + 1, 189);
}

void displayPackEnergy(int packEnergy)
{
    revers(1);
    gotoxy(maxX - 13, maxY + 1);
    if (packEnergy <= 40)
    {
        textcolor(color_warn);
    }
    else
    {
        textcolor(color_frame);
    }
    printf("pe%4d", packEnergy);
    revers(0);
    textcolor(color_frame);
}

void displayScore(int score)
{
    gotoxy(maxX - 5, maxY + 1);
    textcolor(color_frame);
    revers(1);
    printf("s%4d", score);
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
    cputsxy(20 - (strlen(aString) / 2), y, aString);
    gotoxy(0, y + 1);
}

void displayWinCard(int endScore) {
    clrscr();
    textcolor(color_frame);
    chlinexy(0, 0, 40);
    chlinexy(0, maxY + 1, 40);
    gotoxy(0, 3);
    textcolor(color_levelDisplay);
    center("** congratulations **");
    puts("\n");
    textcolor(color_levelTitle);
    center("the pack survived!");
    puts("\n");
    textcolor(color_levelDescription);
    center("bonus for winning the game:");
    center(" ** 2000 points **");
    puts("\n\n");
    textcolor(color_levelTitle);
    center ("your end score:");
    sprintf(buf,"** %d points **",endScore+2000);
    center(buf);
    textcolor(color_frame);
    waitkey();
}

void displayLevelTitleCard(char num, level *aLevel)
{
    clrscr();
    textcolor(color_frame);
    chlinexy(0, 0, 40);
    chlinexy(0, maxY + 1, 40);
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
    waitkey();
}

char updateStatus(char *currentWolfName, char *statusLine, char level, char preyToCatch)
{
    unsigned char i;
    char rvs;
    char shouldUpdateAgain;
    shouldUpdateAgain = false;

    gotoxy(minX + 1, maxY + 1);
    revers(1);
    textcolor(color_frame);
    cprintf("%s", currentWolfName);
    gotoxy(maxX - 19, maxY + 1);
    cprintf("Z%2d", preyToCatch);
    revers(0);
    chlinexy(minX + strlen(currentWolfName) + 1, maxY + 1, 15 - strlen(currentWolfName) - 2);
    if (statusLine != NULL)
    {
        restoreMessageSpace();
        textcolor(color_frame);
        for (i = 0; i < 5; ++i)
        {
            rvs = !rvs;
            gotoxy(2, 0);
            revers(rvs);
            cputs(statusLine);
            waitTicks(10);
        }
        statusLine = NULL;
        shouldUpdateAgain = true;
        revers(0);
    }
    else
    {
        restoreMessageSpace();
        revers(1);
        gotoxy(minX + 1, minY - 1);
        cprintf("level %d", level);
        revers(0);
    }
    return shouldUpdateAgain;
}

void restoreMessageSpace()
{
    textcolor(color_frame);
    chlinexy(minX, minY - 1, maxX - minX + 1);
}

void titlePrompt()
{
    gotoxy(0, maxY - 1);
    center("hit 'i' for instructions or");
    center("any other key to start the game");
}

void displayHelp()
{
    char currentPage=0;
    char cmd;
    cbm_k_bsout(14);
    do {
        clrscr();
        textcolor(color_help);
        cputs(helpPages[currentPage]);
        textcolor(color_frame);
        chlinexy(0,1,40);
        chlinexy(0,maxY,40);
        textcolor(color_prompt);
        cputsxy(0,maxY+1,"B)ack N)ext eX)it");
        cmd = cgetc();
        if ((cmd=='n') && currentPage<2) ++currentPage;
        if ((cmd=='b') && currentPage>0) --currentPage;
    } while (cmd!='x');
    cbm_k_bsout(142);
#ifdef __CX16__
    /* take care of the cx16 re-writing the vera charset data upon changing text mode */
    installCharset();
#endif
}
