#include <stdio.h>
#include <conio.h>
#include <string.h>

typedef struct {
    unsigned int score;
    char name[16];
} highscoreEntry;

void initHighscores();
void showHighscores();
void checkNewHighscore(int aScore);