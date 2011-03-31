// leben = 1
// tot = -1

// Besetzt_
	// Überleben = 2-3 nachbaren
	// Stirbt 0-1|4-8
// Unbesetzt
	// Neues Leben = 3 Nachbaren


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "modul_menu.h"

#define H 10
#define DEAD -1
#define ALIVE 1

typedef struct _posi
{
	int x,y;
} posi;

typedef struct _gameInfo
{
	unsigned int w,h,start_lifes,living,delay;
	posi *pos;
} gameInfo;

struct clist
{
	char *content;
	int id;
	struct clist *next;
};

const char alive_char = '#',dead_char=' ';

// Prototypes

void init(gameInfo *nfo);
void waitKey(char key);
// Field operations
int **createField(gameInfo nfo);
void clearField(int **field, gameInfo nfo);
void freeField(int **field, int h);
void rndGame(int **field, gameInfo nfo);
void writePositions(int **field, gameInfo nfo);
// Visualisation
void printField(int **field, gameInfo nfo);
void printMx(int **field, gameInfo nfo);
// Automat operations
void evalField(int **field, int **eval_field, gameInfo nfo);
void executeRules(int **field, int **eval_field, gameInfo nfo);
int isDead(int **field, gameInfo nfo);
// History operations
void cpField(int **src, int **dst, gameInfo nfo);
int checkReps(int **field, int ***history_field, int l, gameInfo nfo);
int isEqual(int **f1, int **f2, gameInfo nfo);
void mvFieldArray(int ***f, int l);
//File (Write/Save Operations)
FILE *initFile(char *name);
int countSaved(FILE *f);
int addSavedToMenu(char **menu, FILE *f, int saved_n);
void mFlush();
void rmNl(char *s, int l);
