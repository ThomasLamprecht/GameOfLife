#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <GL/glut.h>
#define HAVE_UI 1
#ifdef HAVE_MENU
#include "modul_menu.h"
#endif
#ifdef HAVE_GOF
#include "gof.func.c"
#endif
typedef struct _cord
{
	float x,y,z;
} cord;

typedef	 struct _rgb
{
	float r,g,b;
} rgb;

struct _window
{
	int x,y,w,h;
	float ratio;
} window;

void keyboard(unsigned char key, int x, int y);
void drawSquare(cord size, rgb color, char middle);
cord calcSquareSize(unsigned int x_count, unsigned int y_count, unsigned short int bar_height, float padding);
void display(void);

// General display stuff
void changeSize(int w, int h);
void initGL( GLvoid );
