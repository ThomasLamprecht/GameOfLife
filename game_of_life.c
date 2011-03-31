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
#include "game_of_life.h"

int main(void)
{
	srand(time(NULL));
	int i,run=1;
	// Game specific Variables
	int **field,**eval_field,**history[H];
	unsigned long int generations=0;
	gameInfo nfo;
	nfo.pos=NULL;

	int sel,size=5,real_size=0;
	char **menu,tmp;
	menu = createMenu(size);
	appendItem(menu,"Run Game",size,&real_size);
	appendItem(menu,"Save",size,&real_size);
	appendItem(menu,"Load",size,&real_size);
	appendItem(menu,"Quit",size,&real_size);
	do
	{
		sel=getSelection(menu,0,real_size+1);
		printf("You chose \"%s\"\n",menu[sel]);
	
		switch(sel)
		{
			case 0: // Run game
			{			
				run=1;
				if(nfo.pos!=NULL)free(nfo.pos);
				init(&nfo);
				field = createField(nfo);
				eval_field = createField(nfo);
				for(i=0;i<H;i++)
				{
					history[i]=createField(nfo);
					clearField(history[i], nfo);
				}
				clearField(field, nfo);
				writePositions(field, nfo);

				// debugging purposed
				// rndGame(field,nfo);

				// main loop
				while(run)
				{
					// Equal Check
					system("clear");
					if(checkReps(field,history,H,nfo))
					{
						run=0;
						printf("Lebensschemata wiederholt sich (Oszillierendes Objekt)\n");
					}
					mvFieldArray(history,H);
					cpField(field,history[0],nfo);
					// Equal Check END
					// Dead Check
					if(isDead(field,nfo))
					{
						run=0;
						printf("Es ist niemand mehr am Leben (Allein, Allein), Sie Monster! >_<\nNach %d Generationen.\n",generations);
					}					
					printf("%ld\n",generations);
					printField(field,nfo);
					evalField(field,eval_field,nfo);
					//printMx(eval_field, nfo);
					executeRules(field,eval_field,nfo);
					generations++;
					tmp = getch_nonblock();					
					if(tmp=='q')
					{
						run=0;
						continue;
					}
					else if(tmp=='p')
					{
						printf("Game Paused! Press 'p' again to continue...\n");
						waitKey('p');
						continue;
					}
					usleep(nfo.delay*1000);

				}

				// tidy up
				for(i=0;i<H;i++)
					freeField(history[i],nfo.h);
				freeField(field,nfo.h);
				freeField(eval_field,nfo.h);				
				break;
			}
			case 1: // Save
			{
				if(nfo.pos==NULL)
				{
					printf("Spielen sie zuerst ein Spiel, um es speichern zu können!\n'q' zum fortfahren\n");
					waitKey('q');
					break;
				}
				FILE *f = initFile("gameoflife.data");
				char puffer[150];
				printf("Name des Spieles [Max 150 Zeichen;]:\n");
				fflush(stdout);//mFlush();
				fgets(puffer,150,stdin);				
				rmNl(puffer,150);
				fprintf(f,"BEGIN %s\n",puffer);
				fprintf(f,"H %d\nW %d\nLIFES %d\n",nfo.h,nfo.w,nfo.start_lifes);
				for(i=0;i<nfo.start_lifes;i++)
					fprintf(f,"\t%d,%d\n",nfo.pos[i].x,nfo.pos[i].y);
				fprintf(f,"END\n\n");
				printf("Speichern von \"%s\" Erfolgreich!\nWeiter mit 'q'...\n",puffer);						
				fclose(f);
				waitKey('q');
				break;
			}
			case 2: // Load
			{
				int saved_n=0,saved_real_s,sel_game;
				FILE *f = initFile("gameoflife.data");
				char puffer[160],name[160],**data_menu;				
				if((saved_n = countSaved(f)+1)>0)
				{				
					data_menu=createMenu(saved_n);
					saved_real_s= addSavedToMenu(data_menu,f,saved_n);
					sel_game=getSelection(data_menu,0,saved_real_s+1);
					if(sel_game==saved_n-1) break;
					printf("You chose \"%s\"\n",data_menu[sel_game]);
					freeMenu(data_menu,saved_n);
					waitKey('q');					
				}
				else
				{
					printf("Es sind keine gespeicherten Spiele vorhanden!\nDrücken Sie 'q' um zum Menü zurückzukehren.\n");
					waitKey('q');
				}
				fclose(f);				
				break;
			}	
		}
	}while(sel!=3);
	printf("Good Bye... (:\n");
	freeMenu(menu,size);
	return 0;
}

// Waits for key pressing
void waitKey(char key)
{
	int run=1;
	while(run)
	{
		usleep(20*1000);
		if(getch_nonblock()==key) run=0;
	}	
}

//
int addSavedToMenu(char **menu, FILE *f, int saved_n)
{
	int i=0,j,k;
	char puffer[160],name[160];
	while(fgets(puffer,160,f)!=NULL)
	{
		if(strncmp(puffer,"BEGIN ",6)==0)
		{
			for(j=0;puffer[j]!=' '&j<160;j++)
				continue;
			j++;
			for(k=j;puffer[k]!='\n';k++)
				name[k-j]=puffer[k];
			name[k]='\0';				
			//rmNl(name,160);						
			appendItem(menu,name,saved_n,&i);
		}					
	}
	appendItem(menu,"Quit",saved_n,&i);
	return i;
}
// Counts Saved Games in a file
int countSaved(FILE *f)
{
	int i=0;
	char puffer[150];
	while((fgets(puffer,150,f))!= NULL)
	{
		if(strncmp(puffer,"BEGIN ",6)==0)
		{	
			i++;
		}					
	}
	fseek(f,0L,SEEK_SET);
	return i;
}

// Makes some initialisation work.
void init(gameInfo *nfo)
{
	int i,fail=0,rnd=0;
	printf("Height:\t");
	scanf("%d",&nfo->h);
	printf("Width:\t");
	scanf("%d",&nfo->w);
	printf("Delay/clock [ms]:\t");
	scanf("%d",&nfo->delay);
	printf("Number of Lifes:\t");
	do
	{
		if(fail) printf("Error, number of lifes must be between 1 and %d!\n",nfo->w*nfo->h);
		scanf("%d",&nfo->start_lifes);
		fail=1;
	}while(nfo->start_lifes<1||nfo->start_lifes>nfo->w*nfo->h);
	nfo->living = nfo->start_lifes;
	nfo->pos = (posi *) malloc(nfo->start_lifes*sizeof(posi));
	printf("Insert life positions manually (0) or automatically(1):\t");
	scanf("%d",&rnd);
	for(i=0;i<nfo->start_lifes;i++)
	{
		if(rnd)
		{
			nfo->pos[i].x = rand()%nfo->w;
			nfo->pos[i].y = rand()%nfo->h;
		}
		else
		{
			do
			{
				fail=0;
				if(fail) printf("Error, position of life must be in x{0,%d} and y{0,%d} range.\n",nfo->w-1,nfo->h-1);
				printf("[%d]->x:\t",i+1);
				scanf("%d",&nfo->pos[i].x);
				printf("[%d]->y:\t",i+1);
				scanf("%d",&nfo->pos[i].y);
				fail=1;
			}while((nfo->pos->x <0||nfo->pos->x > nfo->w-1)||(nfo->pos->y <0||nfo->pos->y > nfo->h-1));
		}
	}
	return;
}

// Writes the lifes from the position Array in the field -- ATTENTION abuse may cause segemtation fault!
void writePositions(int **field, gameInfo nfo)
{
	int i;
	for(i=0;i<nfo.start_lifes;i++)
		field[nfo.pos[i].y][nfo.pos[i].x] = ALIVE;
	return;
}

// Returns an (dynamical) integer array with w*h dimensions
int **createField(gameInfo nfo)
{
	int i,**p;
	p = (int **) malloc(nfo.h*sizeof(int *));
	for(i=0;i<nfo.h;i++)
	{
		p[i] = (int *)malloc(nfo.w*sizeof(int));
	}
	return p;
}

// Free's an 2D dynamic integer array
void freeField(int **field, int h)
{
	int i;
	for(i=0;i<h;i++)
	{
		free(field[i]);
	}
	free(field);
	return;
}
// The "drawing" directive for the field
void printField(int **field, gameInfo nfo)
{
	int i,j,k,l;

	for(i=0;i<nfo.h;i++)
	{
		if(i==0)
		{
			for(j=0;j<nfo.w;j++)
			{
				putchar('-');
				putchar('-');
			}
			putchar('-');
			putchar('\n');
		}
		putchar('|');
		for(j=0;j<nfo.w;j++)
		{
			if(field[i][j]==ALIVE)
				putchar(alive_char);
			else
				putchar(dead_char);
			if(j+1!=nfo.w) printf("|");
		}
		putchar('|');
		putchar('\n');
		if(i+1!=nfo.h)
		{
			putchar('|');
			for(k=0;k<nfo.w;k++)
			{
				printf("-");
				if(k+1!=nfo.w) printf("+");
			}
			putchar('|');
		}
		else
		{
			for(j=0;j<nfo.w;j++)
			{
				putchar('-');
				putchar('-');
			}
			putchar('-');
		}
		putchar('\n');
	}
	putchar('\n');
	return;
}
// fills the field randomly with Lifes
void rndGame(int **field, gameInfo nfo)
{
	int i,j;
	for(i=0;i<nfo.h;i++)
	{
		for(j=0;j<nfo.w;j++)
		{
			field[i][j] = (rand()%2)?ALIVE:DEAD;
		}
	}
	return;
}
// Sets all fields to dead
void clearField(int **field, gameInfo nfo)
{
	int i,j;
	for(i=0;i<nfo.h;i++)
	{
		for(j=0;j<nfo.w;j++)
		{
			field[i][j] = DEAD;
		}
	}
	return;
}
// Prints a 2d int array
void printMx(int **field, gameInfo nfo)
{
	int i,j;
	for(i=0;i<nfo.h;i++)
	{
		for(j=0;j<nfo.w;j++)
		{
			printf("%d ",field[i][j]);
		}
		putchar('\n');
	}
	return;
}
// Writes the number of neightbors in the eval_field 
void evalField(int **field, int **eval_field, gameInfo nfo)
{
	int i,j,k,l,eval=0,x,y;
	for(i=0;i<nfo.h;i++) // Go trought Y Axis
	{
		for(j=0;j<nfo.w;j++) // Go trought X Axis
		{
			for(k=-1;k<2;k++) // Check neigthborhood
			{
				for(l=-1;l<2;l++)
				{
					y=i+k;
					x=j+l;
					if((y>=0&&y<nfo.h)&&(x>=0&&x<nfo.w)&&!(k==0&&l==0))
					{
						if(field[y][x]==ALIVE) eval++;
					}
				}
			}
			eval_field[i][j] = eval;
			eval=0;
		}
	}
	return;
}
// Excecute the Game of Life rules
void executeRules(int **field, int **eval_field, gameInfo nfo)
{
	int i,j;
	for(i=0;i<nfo.h;i++)
	{
		for(j=0;j<nfo.w;j++)
		{
			if(field[i][j]==ALIVE)
			{
				if(eval_field[i][j]<2||eval_field[i][j]>3)
					field[i][j]=DEAD;
			}
			else
			{
					if(eval_field[i][j]==3)
					field[i][j]=ALIVE;
			}
		}
	}
	return;
}
// Checks if two fields are equal
int isEqual(int **f1, int **f2, gameInfo nfo)
{
	int i,j;
	for(i=0;i<nfo.h;i++)
	{
		for(j=0;j<nfo.w;j++)
		{
			if(f1[i][j]!=f2[i][j])
				return 0;
		}
	}
	return 1;	
}
// Moves the History Array ones "rightwards"
void mvFieldArray(int ***f, int l)
{
	int **tmp,i;
	tmp = f[l-1];
	for(i=l-1;i>0;i--)
	{
		f[i] = f[i-1];
	}
	f[0]=tmp;
	return;
}
// Should check if the actual field is a repetition from one of the history fields
int checkReps(int **field, int ***history_field, int l, gameInfo nfo)
{
	int i;
	for(i=0;i<l;i++)
	{
		if(isEqual(field,history_field[i],nfo))
		 return 1;
	}
	return 0;
}
// Copys the content from src into dst -field
void cpField(int **src, int **dst, gameInfo nfo)
{
	int i,j;
	for(i=0;i<nfo.h;i++)
	{
		for(j=0;j<nfo.w;j++)
		{
			dst[i][j]=src[i][j];
		}
	}
	return;
}
// Checks if anybody is alive
int isDead(int **field, gameInfo nfo)
{
	int i,j;
	for(i=0;i<nfo.h;i++)
	{
		for(j=0;j<nfo.w;j++)
		{
			if(field[i][j]==ALIVE)
				return 0;
		}
	}
	return 1;
}
FILE *initFile(char *name)
{
	FILE *f;
	f=fopen(name,"a+");
	return f;
}
void mFlush()
{
	char c;
	while((c=getchar())!=EOF&&c!='\n')
		continue;
	return;
}
void rmNl(char *s, int l)
{
	int i;
	for(i=0;s[i]!='\n'&&i<l;i++)
		continue;
	s[i]='\0';
	return;
}
