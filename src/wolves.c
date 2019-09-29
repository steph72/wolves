/* main */

#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <6502.h>

#include "wolftypes.h"
#include "io.h"

const char *names[] = {"buba", "liza", "schnitzel", "darko", "coco", "ben", "tati", "lia", "tia", "laika"};
char buf[40];

thing *preyHead;
thing wolves[MAXWOLVES];

int packEnergy;

char numWolves;
char numPrey;
char currentWolfIndex;
char shouldUpdateStatus;
char *statusLine;

char itemAtPos(char x, char y);
void addScenery(char sc);

/* ---------- BEGIN PROGRAM ---------- */

void getFreePosition(position *aPos)
{
	char x, y;
	do
	{
		x = minX + rand() % (maxX - minX - 1);
		y = minY + rand() % (maxY - minY - 1);
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
	if (numWolves >= 10)
	{
		return;
	}
	newWolf = &(wolves[numWolves]);
	numWolves++;
	getFreePosition(&newPos);
	newWolf->pos = newPos;
}

void addPrey()
{
	thing *prey;
	position newPos;
	prey = insertPreyAtHead();
	getFreePosition(&newPos);
	prey->pos = newPos;
	prey->runmode = wRunModeNormal;
	prey->type = it_prey; //  'a'+numPrey;
	displayThing(prey, false);
	numPrey++;
	dbgNumPrey(numPrey);
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
	dbgNumPrey(numPrey);
}

void init()
{
	unsigned int i;
	clrscr();
	numWolves = 0;
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
	initMachineIO();
	setupScreen();
}

char positionIsOffScreen(position *aPos)
{
	return (aPos->x > maxX || aPos->y > maxY || aPos->x < minX || aPos->y < minY);
}

char isValidDestination(position *aPos)
{
	return (positionIsOffScreen(aPos) || (itemAtPos(aPos->x, aPos->y)) == it_earth);
}

void wanderPrey(thing *aPrey)
{
	signed char xdiff, ydiff;
	position oldPos, newPos;
	oldPos = aPrey->pos;
	do
	{
		xdiff = -1 + rand() % 3;
		ydiff = -1 + rand() % 3;
		newPos.x = oldPos.x + xdiff;
		newPos.y = oldPos.y + ydiff;
	} while (!isValidDestination(&newPos));
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
		if (positionIsOffScreen(&newPos))
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
	for (i = 0; i < numWolves; i++)
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

void servicePrey(char preyChance)
{
	thing *currentPrey;
	currentPrey = preyHead;
	while (currentPrey)
	{
		if (currentPrey->runmode == wRunModeNormal)
		{
			wanderPrey(currentPrey);
			lookoutPrey(currentPrey);
		}
		else if (currentPrey->runmode == wRunModeFlee)
		{
			fleePrey(currentPrey);
		}
		currentPrey = currentPrey->next;
	}
	if ((rand() % 100) < preyChance)
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
			packEnergy += 50;
			return;
		}
		currentPrey = currentPrey->next;
	}
}

void updateWolves()
{
	char i;
	for (i = 0; i < numWolves; i++)
	{
		displayThing(&wolves[i], i == currentWolfIndex);
	}
}

void moveWolf(char wolfIdx, char xDelta, char yDelta)
{
	thing *activeWolf;
	position newPos;
	itemType item;

	activeWolf = &wolves[wolfIdx];
	newPos.x = activeWolf->pos.x + xDelta;
	newPos.y = activeWolf->pos.y + yDelta;
	item = itemAtPos(newPos.x, newPos.y);

	if (item == it_prey)
	{
		catchPrey(wolfIdx, newPos.x, newPos.y);
	}
	else if (item != it_earth)
	{
		return;
	}

	putItemAtPos(activeWolf->pos.x, activeWolf->pos.y, it_earth);
	activeWolf->pos = newPos;
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

void gameLoop(char preyChance)
{

	char quit;
	char command;
	char xd, yd;

	quit = false;
	currentWolfIndex = 0;
	updateWolves();

	while (!quit)
	{
		displayStatusIfNeeded();
		xd = 0;
		yd = 0;
		command = cgetc();
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
		case ' ':
		{
			shouldUpdateStatus = true;
			currentWolfIndex++;
			if (currentWolfIndex >= numWolves)
			{
				currentWolfIndex = 0;
			}
		}
		default:
			break;
		}
		moveWolf(currentWolfIndex, xd, yd);
		updateWolves();
		if (xd || yd)
		{
			packEnergy -= numWolves;
			servicePrey(preyChance);
		}
	}
}

void test()
{

	char i;

	for (i = 0; i < 20; i++)
	{
		addScenery(it_tree);
	}

	for (i = 0; i < 20; i++)
	{
		addScenery(it_bush);
	}

	for (i = 0; i < 7; i++)
	{
		addWolf();
	}

	packEnergy = 500;

	gameLoop(10);
}

void main()
{
	init();
	test();
}
