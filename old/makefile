CC = C:\MinGW\bin\mingw32-gcc.exe
CFLAGS=-I./include -L./lib  -l:libraylib.a -l:libopenAL32dll.a -l:libglfw3dll.a -ggdb

main: arenathing
	$(CC) -o arena obj/arenathing.o obj/unit.o obj/map.o obj/ai.o obj/projectile.o obj/spell.o obj/menu.o $(CFLAGS)
arenathing:  arenathing.c projectile unit map ai menu spell
	$(CC) -c -o obj/arenathing.o arenathing.c $(CFLAGS)
menu:  menu.c
	$(CC) -c -o obj/menu.o menu.c $(CFLAGS)
spell: spell.c
	$(CC) -c -o obj/spell.o spell.c $(CFLAGS)
projectile: projectile.c
	$(CC) -c -o obj/projectile.o projectile.c $(CFLAGS)
unit: unit.c
	$(CC) -c -o obj/unit.o unit.c $(CFLAGS)
ai: ai.c
	$(CC) -c -o obj/ai.o ai.c $(CFLAGS)
map: map.c
	$(CC) -c -o obj/map.o map.c $(CFLAGS)
