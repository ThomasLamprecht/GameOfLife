/*
This is (part of) a simple implementation of the famous cellular automaton "Game of Life" from John Horton Conway
Copyright (C) 2011  Thomas Lamprecht

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
or look at http://www.gnu.org/licenses/gpl-2.0-standalone.html
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "modul_menu.h"

#define H 20
#define DEAD -1
#define ALIVE 1
#define PFR_L 160

typedef struct _posi
{
	int x,y;
} posi;

typedef struct _gameInfo
{
	unsigned int w,h,start_lifes,living,delay;
	unsigned long int generations;
	posi *pos;
} gameInfo;

struct clist
{
	char *content;
	int id;
	struct clist *next;
};

struct posList
{
	posi pos;
	struct posList *next;
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
// Automaton operations
void evalField(int **field, int **eval_field, gameInfo nfo);
void executeRules(int **field, int **eval_field, gameInfo nfo);
int isDead(int **field, gameInfo nfo);
void game(gameInfo *nfo, int loaded);
// History operations
void cpField(int **src, int **dst, gameInfo nfo);
int checkReps(int **field, int ***history_field, int l, gameInfo nfo);
int isEqual(int **f1, int **f2, gameInfo nfo);
void mvFieldArray(int ***f, int l);
//File (Write/Save Operations)
FILE *initFile(char *name);
int countSaved(FILE *f);
int addSavedToMenu(char **menu, FILE *f, int saved_n);
void getSavedData(FILE *f, char *name, int id, gameInfo *nfo);
void mFlush();
void rmNl(char *s, int l);
void clrs(char *s, int l);
