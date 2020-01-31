/* main */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>
#include <6502.h>
#include <time.h>

#include "wolftypes.h"
#include "io.h"
#include "utils.h"
#include "levels.h"
#include "highscore.h"

#define PE_PER_LEVEL 100
#define PE_PER_PREY 25
#define PE_MOVEMENT_COST 2
#define PE_WAIT_COST 1
#define SCORE_PER_PREY 10

const char *names[] = {"buba", "candor", "liza", "schnitzel", "darko", "coco", "ben", "tati", "lia", "tia", "laika"};
char buf[40];

thing *preyHead;
thing wolves[MAXWOLVES];

int packEnergy;
int score;

char wolfCount;
char numPrey;
char currentWolfIndex;
char shouldUpdateStatus;
char *statusLine;

char itemAtPos(char x, char y);
void addScenery(char sc);

void chooseRandomDest(thing *aPrey);

void getFreePosition(position *aPos)
{
	char x, y;
	do
	{
		x = minX + drand(maxX - minX + 1);
		y = minY + drand(maxY - minY + 1);
	} while (itemAtPos(x, y) != it_earth);
	aPos->x = x;
	aPos->y = y;
}

void addScenery(char sc)
{
	position newPos;
	getFreePosition(&newPos);
	putCanvasItem(&newPos, sc);
}

thing *newThing()
{
	thing *n;
	n = (thing *)malloc(sizeof(thing));
	n->next = NULL;
	n->prev = NULL;
	return n;
}

thing *insertPreyAtHead()
{
	thing *n = newThing();
	if (preyHead == NULL)
	{
		preyHead = n;
		return n;
	}
	preyHead->prev = n;
	n->next = preyHead;
	preyHead = n;
	return n;
}

void addWolf()
{
	thing *newWolf;
	position newPos;
	if (wolfCount >= 10)
	{
		return;
	}
	newWolf = &(wolves[wolfCount]);
	wolfCount++;
	getFreePosition(&newPos);
	newWolf->pos = newPos;
}

void getNewPreyPosition(position *aPos)
{
	char horizontalLock;
	char x, y;

	horizontalLock = drand(100) > 50;

	if (drand(100) < 50)
	{
		x = minX;
	}
	else
	{
		x = maxX;
	}

	if (drand(100) < 50)
	{
		y = minY;
	}
	else
	{
		y = maxY;
	}

	do
	{
		if (horizontalLock)
		{
			x = minX + drand(maxX - minX - 1);
		}
		else
		{
			y = minY + drand(maxY - minY - 1);
		}

	} while (itemAtPos(x, y) != it_earth);

	aPos->x = x;
	aPos->y = y;
}

void addPrey()
{
	thing *prey;
	position newPos;
	prey = insertPreyAtHead();
	getNewPreyPosition(&newPos);
	prey->pos = newPos;
	prey->runmode = wRunModeNormal;
	prey->type = it_prey; //  'a'+numPrey;
	displayThing(prey, false);
	numPrey++;
	prey->movX = 0;
	prey->movY = 0;
	if (prey->pos.x == minX)
	{
		prey->movX = 1;
	}
	if (prey->pos.x == maxX)
	{
		prey->movX = -1;
	}
	if (prey->pos.y == minY)
	{
		prey->movY = 1;
	}
	if (prey->pos.y == maxY)
	{
		prey->movY = -1;
	}
}

void removePrey(thing *aPrey)
{
	if (aPrey->prev)
	{
		aPrey->prev->next = aPrey->next;
	}
	if (aPrey->next)
	{
		aPrey->next->prev = aPrey->prev;
	}
	if (aPrey == preyHead)
	{
		preyHead = aPrey->next;
	}
	free(aPrey);
	numPrey--;
}

void init()
{
	initMachineIO();
	title();
	initHighscores();
}

void startOver()
{
	unsigned int i;

	score = 0;
	wolfCount = 0;
	packEnergy = 0;
	numPrey = 0;
	preyHead = NULL;
	for (i = 0; i < MAXWOLVES; i++)
	{
		wolves[i].pos.x = 0;
		wolves[i].pos.y = 0;
		wolves[i].type = it_wolf;
		wolves[i].runmode = wRunModeNone;
	}
	statusLine = "welcome! help the wolves survive";
	textcolor(color_prompt);
	titlePrompt();
	textcolor(color_score);
	gotoxy(0, 10);
	center("B B  high scores  B B");
	cputs("\r\n");
	showHighscores();
}

char positionIsOffScreen(position *aPos)
{
	return (aPos->x > maxX || aPos->y > maxY || aPos->x < minX || aPos->y < minY);
}

char isValidDestination(position *aPos)
{
	return (positionIsOffScreen(aPos) ||
			(itemAtPos(aPos->x, aPos->y)) == it_earth ||
			(itemAtPos(aPos->x, aPos->y)) == it_bush);
}

void movePrey(thing *aPrey)
{
	position oldPos, newPos;

	oldPos = aPrey->pos;
	newPos.x = oldPos.x + aPrey->movX;
	newPos.y = oldPos.y + aPrey->movY;

	putItemAtPos(oldPos.x, oldPos.y, it_earth);
	if (positionIsOffScreen(&newPos))
	{
		removePrey(aPrey);
		return;
	}
	else
	{
		aPrey->pos = newPos;
		displayThing(aPrey, false);
	}
}

void chooseRandomDest(thing *aPrey)
{
	signed char xdiff, ydiff;
	position oldPos, newPos;
	oldPos = aPrey->pos;
	do
	{
		xdiff = -1 + drand(3);
		ydiff = -1 + drand(3);
		newPos.x = oldPos.x + xdiff;
		newPos.y = oldPos.y + ydiff;
	} while (!isValidDestination(&newPos));

	aPrey->movX = xdiff;
	aPrey->movY = ydiff;
}

void wanderPrey(thing *aPrey)
{
	position oldPos, newPos;

	oldPos = aPrey->pos;
	newPos.x = aPrey->pos.x + aPrey->movX;
	newPos.y = aPrey->pos.y + aPrey->movY;

	if (!isValidDestination(&newPos))
	{
		chooseRandomDest(aPrey);
	}

	movePrey(aPrey);
}

void fleePrey(thing *aPrey)
{
	signed char xdiff, ydiff;
	position oldPos, newPos;
	oldPos = aPrey->pos;
	xdiff = (aPrey->hunter->pos.x - aPrey->pos.x);
	ydiff = (aPrey->hunter->pos.y - aPrey->pos.y);
	xdiff = (xdiff > 0) - (xdiff < 0);
	ydiff = (ydiff > 0) - (ydiff < 0);
	newPos.x = oldPos.x - xdiff;
	newPos.y = oldPos.y - ydiff;
	if (isValidDestination(&newPos))
	{
		aPrey->pos = newPos;
		putItemAtPos(oldPos.x, oldPos.y, it_earth);
		if (positionIsOffScreen(&newPos) ||
			(itemAtPos(newPos.x, newPos.y)) == it_bush)
		{
			removePrey(aPrey);
			statusLine = "prey got away!";
		}
		else
		{
			aPrey->pos = newPos;
			displayThing(aPrey, false);
		}
	}
	else
	{
		wanderPrey(aPrey);
	}
}

void lookoutPrey(thing *aPrey)
{
	thing *testWolf;
	signed char xdiff, ydiff;
	char i;
	for (i = 0; i < wolfCount; i++)
	{
		testWolf = &(wolves[i]);
		xdiff = (testWolf->pos.x - aPrey->pos.x);
		ydiff = (testWolf->pos.y - aPrey->pos.y);
		if (abs(xdiff) <= 2 && abs(ydiff) <= 2)
		{
			aPrey->runmode = wRunModeFlee;
			aPrey->hunter = testWolf;
			displayThing(aPrey, true);
			statusLine = "prey spotted you!";
			break;
		}
	}
}

void clearPrey(void)
{
	thing *currentPrey;
	thing *delPrey;
	currentPrey = preyHead;
	while (currentPrey)
	{
		delPrey = currentPrey;
		currentPrey = currentPrey->next;
		removePrey(delPrey);
	}
	preyHead = NULL;
}

void servicePrey(char preyChance)
{
	thing *currentPrey;
	currentPrey = preyHead;
	while (currentPrey)
	{
		if (currentPrey->runmode == wRunModeNormal)
		{
			if (drand(100) > 25) // let prey eat once in a while
			{
				wanderPrey(currentPrey);
			}
			lookoutPrey(currentPrey);
		}
		else if (currentPrey->runmode == wRunModeFlee)
		{
			fleePrey(currentPrey);
		}
		currentPrey = currentPrey->next;
	}
	if (drand(100) < preyChance)
	{
		addPrey();
	}
}

void catchPrey(char wolfIdx, char x, char y)
{
	thing *currentPrey;
	currentPrey = preyHead;
	while (currentPrey)
	{
		if (currentPrey->pos.x == x && currentPrey->pos.y == y)
		{
			removePrey(currentPrey);
			sprintf(buf, "%s catches the prey!", names[wolfIdx]);
			statusLine = buf;
			packEnergy += PE_PER_PREY;
			score += SCORE_PER_PREY;
			displayScore(score);
			return;
		}
		currentPrey = currentPrey->next;
	}
}

void updateWolves()
{
	char i;
	for (i = 0; i < wolfCount; i++)
	{
		displayThing(&wolves[i], i == currentWolfIndex);
	}
}

char moveWolf(char wolfIdx, char xDelta, char yDelta)
{
	thing *activeWolf;
	position newPos;
	itemType item;

	char caught = 0;

	activeWolf = &wolves[wolfIdx];
	newPos.x = activeWolf->pos.x + xDelta;
	newPos.y = activeWolf->pos.y + yDelta;
	item = itemAtPos(newPos.x, newPos.y);

	if (item == it_prey)
	{
		caught = 1;
		catchPrey(wolfIdx, newPos.x, newPos.y);
	}
	else if (item != it_earth)
	{
		return 0;
	}

	putItemAtPos(activeWolf->pos.x, activeWolf->pos.y, it_earth);
	activeWolf->pos = newPos;

	return caught;
}

void displayStatusIfNeeded()
{
	displayPackEnergy(packEnergy);
	if (shouldUpdateStatus == false && statusLine == NULL)
	{
		return;
	}
	shouldUpdateStatus = updateStatus((char *)names[currentWolfIndex], statusLine);
	statusLine = NULL;
}

void nextWolf(char back)
{

	char idx;

	int nextWolfDistance = 9999;
	int prevWolfDistance = 9999;
	char nextWolfIndex = currentWolfIndex;
	char prevWolfIndex = currentWolfIndex;
	int cwp; // current wolf position
	int twp; // test wolf position
	int dist;

	cwp = (100 * wolves[currentWolfIndex].pos.x) + wolves[currentWolfIndex].pos.y;

	for (idx = 0; idx < wolfCount; ++idx)
	{

		if (idx != currentWolfIndex)
		{

			twp = (100 * wolves[idx].pos.x) + wolves[idx].pos.y;
			dist = abs(cwp - twp);
			if (cwp - twp > 0)
			{
				if (dist < nextWolfDistance)
				{
					nextWolfDistance = dist;
					nextWolfIndex = idx;
				}
			}
			else
			{
				if (dist < prevWolfDistance)
				{
					prevWolfDistance = dist;
					prevWolfIndex = idx;
				}
			}
		}
	}
	if (back)
	{
		currentWolfIndex = nextWolfIndex;
	}
	else
	{
		currentWolfIndex = prevWolfIndex;
	}
}

void gameLoop(char preyChance, char preyNeeded)
{

	char quit;
	char command;
	char xd, yd;

	char huntSuccess;
	char preyCaught = 0;

	quit = false;
	currentWolfIndex = 0;
	updateWolves();
	displayScore(score);
	displayStatusIfNeeded();

	while (!quit)
	{
		xd = 0;
		yd = 0;
		huntSuccess = 0;

		do
		{
			command = cgetc();
		} while (strchr("ijklnb ", command) == NULL);

		switch (command)
		{
		case 'i':
			yd--;
			break;
		case 'k':
			yd++;
			break;
		case 'j':
			xd--;
			break;
		case 'l':
			xd++;
			break;
		case 'n':
		{
			shouldUpdateStatus = true;
			nextWolf(false);
			break;
		}
		case 'b':
		{
			shouldUpdateStatus = true;
			nextWolf(true);
			break;
		}
		default:
			break;
		}
		huntSuccess = moveWolf(currentWolfIndex, xd, yd);
		preyCaught += huntSuccess;
		updateWolves();

		if (command != 'n' && command != 'b')
		{
			if (xd || yd)
			{
				packEnergy -= PE_MOVEMENT_COST;
			}
			else
			{
				packEnergy -= PE_WAIT_COST;
			}

			servicePrey(preyChance);
		}

		if (packEnergy < 0 || preyCaught >= preyNeeded)
		{
			quit = true;
		}

		displayStatusIfNeeded();
	}

	if (packEnergy >= 0)
	{
		gotoxy(0,(maxY/2)-1);
		center("                          ");
		center("   ** level complete **   ");
		center("                          ");
		sprintf(buf, "bonus: %d points", packEnergy);
		statusLine = buf;
		score += packEnergy;
		displayScore(score);
		displayStatusIfNeeded();
		waitkey();
	}
}

void runGame(char numTrees, char numBushes, char numWolves, char preyChance, char preyNeeded)
{
	char i;

	setupScreen();

	clearPrey();

	packEnergy += PE_PER_LEVEL;

	for (i = 0; i < numTrees; i++)
	{
		addScenery(it_tree);
	}

	for (i = 0; i < numBushes; i++)
	{
		addScenery(it_bush);
	}

	for (i = 0; i < numWolves; i++)
	{
		addWolf();
	}

	gameLoop(preyChance, preyNeeded);
}

void main()
{
	level aLevel;
	char c;
	char level = 0;
	char quit = false;

	init(); // init machine

	do
	{
		startOver(); // reset everything for a new game
		level = 0;

		c = cgetc();
		if (c == 'i')
		{
			displayHelp();
		}

		do
		{

			if (level < 10)
			{
				aLevel = wLevels[level];

				displayLevelTitleCard(level + 1, &aLevel);

				runGame(aLevel.numTrees,
						aLevel.numBushes,
						aLevel.numWolves,
						aLevel.preyChance,
						aLevel.preyNeeded);
			}

			++level;

		} while (packEnergy >= 0);

		gotoxy(0,(maxY/2)-3);
		revers(1);
		center("                   ");
		center(" AAA game over AAA ");
		center("                   ");
		center(" E press any key E ");
		center("                   ");
		waitTicks(10);
		waitkey();
		revers(0);
		clrscr();
		checkNewHighscore(200);
		title();
		
	} while (!quit);
}
