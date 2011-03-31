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
#include "modul_menu.h"

// example
//int main(int c_args, char **args)
//{
//	int i,size=10,real_size=0;
//	char **menu;
//	menu = createMenu(size);
//	appendItem(menu,"Test 1",size,&real_size);
//	appendItem(menu,"Test 2",size,&real_size);
//	appendItem(menu,"Test 3",size,&real_size);
//	appendItem(menu,"Test 4",size,&real_size);
//	appendItem(menu,"Test 5",size,&real_size);
//	appendItem(menu,"Test 6",size,&real_size);
//	appendItem(menu,"Test 7",size,&real_size);
//	appendItem(menu,"Test 8",size,&real_size);
//	appendItem(menu,"Test 9",size,&real_size);
//	appendItem(menu,"Quit",size,&real_size);
//	do
//	{
//		i=getSelection(menu,0,real_size+1);
//		printf("%s selected\n",menu[i]);
//	}while(i!=9);
//	return 0;
//}

char** createMenu(int m_size)
{
	int i;
	char **p;
	p = (char **) malloc(m_size*sizeof(char *));
	for(i=0;i<m_size;i++)
	{
		p[i] = NULL;
	}
	return p;
}

void* appendItem(char **p, char *text, int m_size, int *rsize)
{
	int i,tmp;
	for(i=0;i<m_size;i++)
	{
		if(p[i]==NULL)
		{
			tmp = strlen(text)+1;
			p[i] = (char *) malloc(tmp*sizeof(char));
			strcpy(p[i],text);
			*rsize=i;
			return;
		}
	}
	return NULL;
}

int getSelection(char **p, int start_pos, int m_size)
{
	int i,j,pos=start_pos;
	char sel;
	while(1)
	{
		system("clear");
		printf("Game of Life - Thomas Lamprecht\nPress w/s to navigate trought the menu. Hit enter to select your choice.\nq is a exit shortcut.\n");
		printf("-----------------------\n");
		for(i=0;i<m_size;i++)
		{
			if(p[i]!=NULL)
			{
				if(i==pos) printf("→");
				printf("\t %s \n",p[i]);
			}
			else break;
		}
		printf("-----------------------\n");
		while(1)
		{
			sel=getch_nonblock();
			if(sel=='s')
			{
				pos = (pos+1)%m_size;
				break;
			}
			if(sel=='w') 
			{
				pos = (pos+m_size-1)%m_size;
				break;
			}
			if(sel=='\n') return pos;
			if(sel=='q') return m_size-1;
			//else putchar(sel);
		} 
	}
}

void freeMenu(char **menu,int m_size)
{
	int i;
	for(i=0;i<m_size;i++)
	{
		if(menu[i]!=NULL)
			free(menu[i]);
	}
	free(menu);
	return;
}

int getch_nonblock(void)
{
	struct termios term, oterm;
	int fd = 0;
	int c = 0;
	tcgetattr(fd, &oterm);
	memcpy(&term, &oterm, sizeof(term));
	term.c_lflag = term.c_lflag & (!ICANON);
	term.c_cc[VMIN] = 0;
	term.c_cc[VTIME] = 1;
	tcsetattr(fd, TCSANOW, &term);
	c = getchar();
	tcsetattr(fd, TCSANOW, &oterm);
	return c; // gibt -1 zurück, wenn kein Zeichen gelesen wurde
}
