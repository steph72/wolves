#include "wolftypes.h"


void putItemAtPos(unsigned char x, unsigned char y, itemType item);
void putCanvasItem(position* aPos, char item);
void displayThing(thing* aThing, char hilite);
void setupScreen();
void initMachineIO();
char updateStatus(char *currentWolfName, char *statusLine, char level, char preyToCatch);
void displayPackEnergy(int packEnergy);
void displayLevelTitleCard(char num, level *aLevel);
void displayWinCard(int endScore);
void displayScore(int score);
void drawFrame();
void title();
void titlePrompt();
void displayHelp();
void center(char *aString);
void waitTicks(char ticks);
void installCharset();

extern unsigned char isDisplayingStatus;

extern const char minX, minY;
extern const char maxX, maxY;

extern const char color_score;
extern const char color_help;
extern const char color_prompt;
extern const char color_warn;

extern const char wolfC[];
extern const char preyC[];
extern const char treeC[];
extern const char bushC[];
extern const char florC[];

