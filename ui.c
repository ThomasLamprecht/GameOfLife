#include "ui.h"

int main(int argc, char** argv)
{
	glutInit(&argc, argv);	
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(0,0);
	glutInitWindowSize(1440,900);
	glutCreateWindow("GLUT Test");
	// register callbacks
	glutReshapeFunc(changeSize);
	glutKeyboardFunc(keyboard);
	glutDisplayFunc(display);
	
	initGL();
	
	glutMainLoop();

	return 0;
}


void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
		case '\x1B': case 'q':
			exit(0);
		break;
	}
}

void changeSize(int w, int h)
{

	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if (h<100) h = 100;
	window.h=h;
	if(w<100) w = 100; 
	window.w=w;
	float ratio = (float) w / (float) h;
	window.ratio=ratio;
	// Use the Projection Matrix
	glMatrixMode(GL_PROJECTION);

	// Reset Matrix
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set the correct perspective.
	gluPerspective(150.0f, ratio, 1.0f, 300.0f);

	// Get Back to the Modelview
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity( );
}

void display(void)
{
	int i,j,counter=-1;
	cord sqrsz = calcSquareSize(100,100,10,.0f),tmp={.0f,.0f,.0f};
	rgb clrs[4] = {{.1f,.0f,.0f},{.0f,.1f,.0f},{.0f,.0f,.1f},{.0f,.1f,.1f}};
	// Clear Color and Depth Buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glLoadIdentity( );
	drawSquare((cord){window.w,window.h,.0f} , clrs[0], 1);
	
	for(i=0;i<100;i++)
	{
		for(j=0;j<100;j++)
		{
			counter++;
			if(counter>3) counter = 0;
			glLoadIdentity( );
			glTranslatef( tmp.x, tmp.y, -20.f );
				drawSquare(sqrsz , clrs[counter], 1);
			//glLoadIdentity( );
			tmp.x += sqrsz.x;
		}
		counter++;
		if(counter>3) counter = 0;
		tmp.x=.0f;
		tmp.y += sqrsz.y;	
	}
	glLoadIdentity( );
	    
	glutSwapBuffers();
}

void drawSquare(cord size, rgb color, char middle)
{
	glColor3f(color.r, color.g, color.b); // Set color	
	glBegin( GL_QUADS );
		if(middle==1)
		{		
			glVertex3f(size.x/2.0f, -size.y/2.0f, size.z); // top right
			glVertex3f(-size.x/2.0f, -size.y/2.0f, size.z); // top left
			glVertex3f(-size.x/2.0f, size.y/2.0f, size.z); //bottom left
			glVertex3f(size.x/2.0f,size.y/2.0f, size.z); // bottom right
		}
		else
		{
			glVertex3f(size.x,.0f, size.z); // top right
			glVertex3f(.0f, .0f, size.z); // top 
			glVertex3f(.0f, -size.y, size.z); // bottom
			glVertex3f(size.x, -size.y, size.z); // bottom right			
		}
	glEnd( );
}

cord calcSquareSize(unsigned int x_count, unsigned int y_count, unsigned short int bar_height, float padding)
{
	cord square;
	square.x = (float) window.w/(10.f*(float)x_count) - padding;
	square.y = (float) (window.h-bar_height)/(10.f*(float)y_count) - padding;
	
	return square;
}

void initGL( GLvoid )
{

    /* Enable smooth shading */
    glShadeModel( GL_SMOOTH );

    /* Set the background black */
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

    /* Depth buffer setup */
    glClearDepth( 1.0f );

    /* Enables Depth Testing */
    glEnable( GL_DEPTH_TEST );

    /* The Type Of Depth Test To Do */
    glDepthFunc( GL_LEQUAL );

    /* Really Nice Perspective Calculations */
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

    return;
} 
