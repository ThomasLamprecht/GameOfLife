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
// printf("%d\n",__LINE__); // <- The famous debugging helper ;)
*/
#include "game_of_life.h"

int main(void)
{
	srand(time(NULL));
	int i,run=1,oszil_chk=1;
	// Game specific Variables
	int **field,**eval_field,**history[H];	 
	gameInfo nfo;
	nfo.pos=NULL;
	nfo.generations=0;

	int sel=0,size=5,real_size=0;
	char **menu,tmp;
	menu = createMenu(size);
	appendItem(menu,"Spielen",size,&real_size);
	appendItem(menu,"Speichern",size,&real_size);
	appendItem(menu,"Laden",size,&real_size);
	appendItem(menu,"Verlassen",size,&real_size);
	do
	{
		sel=getSelection(menu,sel,real_size+1);
		printf("Sie wählten \"%s\"\n",menu[sel]);
	
		switch(sel)
		{
			case 0: // Run game
			{				
				if(nfo.pos!=NULL)free(nfo.pos);
				init(&nfo);
				load_label:
				run=1;
				oszil_chk=1;
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
					system("clear");
					nfo.living = living(field,nfo);					
					printField(field,nfo);
					evalField(field,eval_field,nfo);
					// Dead Check
					if(nfo.living==0)
					{
						run=0;
						printf("Es ist niemand mehr am Leben (Allein, Allein), Sie Monster! >_<\nNach %d Generationen.\n'q' zum fortfahren",nfo.generations);
						waitKey('q');
					}
					// Equal Check START
					if(oszil_chk&&run!=0)
					{
						if(checkReps(field,history,H,nfo))
						{
							oszil_chk=0;
							printf("Lebensschemata wiederholt sich (Oszillierendes Objekt).\n");
							printf("Spiel pausiert! Drücken Sie 'p' um fortzufahren...\nSollten sie das Spiel beenden wollen, fahren sie fort und betätigen 'q'.\n");		
							waitKey('p');							
						}
					}
					mvFieldArray(history,H);
					cpField(field,history[0],nfo);
					// Equal Check END
					executeRules(field,eval_field,nfo);
					nfo.generations++;
					// Input Op's
					tmp = getch_nonblock();					
					if(tmp=='q')
					{
						run=0;
						continue;
					}
					else if(tmp=='p')
					{
						printf("Spiel pausiert! Drücken Sie 'p' erneut um fortzufahren...\n");
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
				char puffer[PFR_L],name[PFR_L],**data_menu;				
				if((saved_n = countSaved(f)+1)>0)
				{				
					data_menu=createMenu(saved_n);
					saved_real_s= addSavedToMenu(data_menu,f,saved_n);
					sel_game=getSelection(data_menu,0,saved_real_s+1);
					if(sel_game==saved_n-1) break;
					printf("You chose \"%s\"\n",data_menu[sel_game]);
					getSavedData(f, data_menu[sel_game], sel_game, &nfo);
					/*if(nfo.pos!=NULL) // Get this Fuck away to let loading be functionally
					{
						free(nfo.pos);
						nfo.pos=NULL;
					}*/
					freeMenu(data_menu,saved_n);
					printf("Delay/clock [ms]:\t");
					scanf("%d",&nfo.delay);
					goto load_label;
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
	printf("\nGood Bye... (:\n\n");
	freeMenu(menu,size);
	return 0;
}


// Makes some initialisation work.
void init(gameInfo *nfo)
{
	int i,fail=0,rnd=0;
	fflush(stdout);
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
	printf("| Generation: %ld\n",nfo.generations);
	printf("| Zu Begin Lebend: %ld\t Aktuell Lebend: %ld\n",nfo.start_lifes,nfo.living);
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
// Counts the lifes on the field
int living(int **field, gameInfo nfo)
{
	int i,j;
	unsigned int living=0;
	for(i=0;i<nfo.h;i++)
	{
		for(j=0;j<nfo.w;j++)
		{
			if(field[i][j]==ALIVE)
				living++;
		}
	}
	return living;
}
// inits the gameoflife.data file
FILE *initFile(char *name)
{
	FILE *f;
	f=fopen(name,"a+");
	if(f==NULL);
		printf("Fatal Error: cannot open/create gameoflife.data!");
	return f;
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
	return;	
}
// Loads a Game in the gameInfo struct
void getSavedData(FILE *f, char *name, int id, gameInfo *nfo)
{
	int i,j,k,save_no=0;
	char puffer[PFR_L],begin_row[PFR_L]="BEGIN ";
	strncat(begin_row,name,PFR_L);
	strncat(begin_row,"\n",PFR_L);
	fseek(f,0L,SEEK_SET);
	while(fgets(puffer,PFR_L,f)!=NULL)
	{
		if(strncmp(puffer,"BEGIN ",6)==0)
		{			
			if(strncmp(puffer,begin_row,PFR_L)==0)
			{							
				if(save_no==id)
				{
					fscanf(f, "H %d\n", &nfo->h);
					fscanf(f, "W %d\n", &nfo->w);
					fscanf(f, "LIFES %d\n", &nfo->start_lifes);
					if(nfo->pos!=NULL)
					{
						free(nfo->pos);
						nfo->pos=NULL;
					}
					nfo->pos = (posi *) malloc(nfo->start_lifes*sizeof(posi));
					for(i=0;i<nfo->start_lifes;i++)
					{
						fscanf(f, "%d,%d\n", &nfo->pos[i].x,&nfo->pos[i].y);
					}
					printf("Loading succeed\n%s\n%d\n%d\n%d",name,nfo->h,nfo->w,nfo->start_lifes);
					fgets(puffer,PFR_L,f);
					if(strncmp(puffer,"END",3)==0)
						return;
					else
					{
						fputs("Here should be the END!\nCheck the gameoflife.data file!",stderr);
						exit(1);
					}
				}			
			}
			save_no++;
		}					
	}
	return;
}

// Adds alle saved Levels to the select menu
int addSavedToMenu(char **menu, FILE *f, int saved_n)
{
	int i=0,j,k;
	char puffer[PFR_L],name[PFR_L];
	while(fgets(puffer,PFR_L,f)!=NULL)
	{
		if(strncmp(puffer,"BEGIN ",6)==0)
		{
			for(j=0;puffer[j]!=' '&&j<PFR_L;j++)
				continue;
			j++;
			for(k=j;puffer[k]!='\n'&&k<PFR_L;k++)
				name[k-j]=puffer[k];
			name[k]='\0';					
			appendItem(menu,name,saved_n,&i);
			clrs(name,PFR_L);
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
// Flushes the stdin
void mFlush()
{
	char c;
	while((c=getchar())!=EOF&&c!='\n')
		continue;
	return;
}
// Overwrites (one) linefeed with NUL 
void rmNl(char *s, int l)
{
	int i;
	for(i=0;s[i]!='\n'&&i<l;i++)
		continue;
	s[i]='\0';
	return;
}
// Sets a full char Array to \0
void clrs(char *s, int l)
{
	int i;
	for(i=0;i<l;i++)
		s[i]='\0';
	return;
}
