CC = gcc
version = 0.9.1b
out = gof-$(version).bin

gameoflife: game_of_life.o modul_menu.o
	$(CC) -o $(out) game_of_life.o modul_menu.o 

game_of_life.o: game_of_life.c game_of_life.h
	$(CC) -c game_of_life.c 

modul_menu.o: modul_menu.c modul_menu.h
	$(CC) -c modul_menu.c
	
debug:
	$(CC) -c game_of_life.c -g
	$(CC) -c modul_menu.c -g
	$(CC) -o $(out) game_of_life.o modul_menu.o -g
