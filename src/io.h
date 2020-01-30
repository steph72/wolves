#include "wolftypes.h"

void putItemAtPos(unsigned char x, unsigned char y, itemType item);
void putCanvasItem(position* aPos, char item);
void displayThing(thing* aThing, char hilite);
void setupScreen();
void initMachineIO();
char updateStatus(char *currentWolfName, char *statusLine);
void displayPackEnergy(int packEnergy);
void displayLevelTitleCard(char num, level *aLevel);
void displayScore(int score);
void drawFrame();
void titlePrompt();
void displayHelp();

void installCharset();

extern const char minX, minY;
extern const char maxX, maxY;