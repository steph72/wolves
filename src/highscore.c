#include "highscore.h"
#include "utils.h"

#define SCOREFILE "wscores"

highscoreEntry hEntries[10];

void createDefaultHighscores()
{
    char i;
    for (i = 0; i < 10; ++i)
    {
        hEntries[i].score = (10 - i) * 50;
        strcpy(hEntries[i].name, "buba");
    }
}

void saveHighscores()
{
    FILE *hsfile;
    cputs("\r\nplease wait, saving scores...\r\n");
    hsfile = fopen(SCOREFILE, "w");
    if (!hsfile)
    {
        cputs("\r\ncould not write highscore file!");
        waitkey();
        return;
    }
    fwrite(hEntries, sizeof(hEntries), 1, hsfile);
    fclose(hsfile);
    return;
}

void checkNewHighscore(int aScore)
{
    char i, j;
    char buf[10];
    for (i = 0; i < 10; ++i)
    {
        if (aScore > hEntries[i].score)
        {
            clrscr();
            cputs("***** a new high score! *****\r\n\r\nyour name (max. 8 chars):\r\n");
            fgets(buf, 10, stdin);
            buf[strlen(buf) - 1] = 0;

            if (i != 9)
            {
                for (j = 10; j > i; j--)
                {
                    hEntries[j].score = hEntries[j - 1].score;
                    strcpy(hEntries[j].name, hEntries[j - 1].name);
                }
            }
            hEntries[i].score = aScore;
            strcpy(hEntries[i].name, buf);
            cursor(0);
#ifndef __CX16__
            // TODO:
            // for some reason, creating the scorefile fails on the cx16, so
            // we disable persisting the score until a someone figures out what's
            // going on there...
            saveHighscores();
#endif
            return;
        }
    }
}

void showHighscores()
{
    char i, y;
    char dx = 0, dy = 0;
    y = wherey();
    for (i = 0; i < 10; ++i)
    {
        if (i == 5)
        {
            dx = 20;
            dy = 0;
        }
        gotoxy(dx, y + dy);
        cprintf("%2d. %8s %5d", i + 1, hEntries[i].name, hEntries[i].score);
        dy++;
    }
}

void initHighscores()
{
    FILE *hsfile;
    hsfile = fopen(SCOREFILE, "r");
    if (!hsfile)
    {
        createDefaultHighscores();
        return;
    }
    fread(hEntries, sizeof(hEntries), 1, hsfile);
    fclose(hsfile);
}
