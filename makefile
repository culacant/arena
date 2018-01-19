CC = gcc.exe
CFLAGS=-I./include -L./lib  -l:libraylib.a -l:libopenAL32dll.a -l:libglfw3dll.a -ggdb

main:arena
	$(CC) -o arena obj/arena.o obj/spell.o obj/unit.o obj/map.o obj/projectile.o $(CFLAGS)
arena:  arena.c unit map projectile spell
	$(CC) -c -o obj/arena.o arena.c $(CFLAGS)
spell: spell.c
	$(CC) -c -o obj/spell.o spell.c $(CFLAGS)
map: map.c
	$(CC) -c -o obj/map.o map.c $(CFLAGS)
unit: unit.c
	$(CC) -c -o obj/unit.o unit.c $(CFLAGS)
projectile: projectile.c
	$(CC) -c -o obj/projectile.o projectile.c $(CFLAGS)

mainend: arena
	$(CC) -o arena obj/arena.o obj/unit.o obj/map.o obj/ai.o obj/projectile.o obj/menu.o $(CFLAGS)
arenaend:  arena.c projectile unit map ai menu
	$(CC) -c -o obj/arena.o arena.c $(CFLAGS)
menu:  menu.c
	$(CC) -c -o obj/menu.o menu.c $(CFLAGS)
ai: ai.c
	$(CC) -c -o obj/ai.o ai.c $(CFLAGS)
