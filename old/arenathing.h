#ifndef CCRIP_H
#define CCRIP_H

#include "raylib.h"
#include "raymath.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

//main
void RunGame();
void RunMenu();


/*
map construction from mesh
1 start somewhere sensible
2 raycast down to mesh
3 pos = x,z,y
4 raycast to neighbors on height of max slope
5 if it hits, add no movement tile
6 if not, repeat 1
*/
// map stuff
typedef struct Tile
{
	int graphic;
	bool canWalk;
	bool canSee;
} Tile;

Tile *LoadMap(int *w, int *h);
void DrawMap(Tile *map, int w, int h);

// AI stuff
typedef struct Node Node;
struct Node
{
	int x;
	int y;
	float f;
	float g;
	float h;
	Node *prev;
};

typedef struct Path Path;
struct Path
{
	int x;
	int y;
	Path *next;
};

Path *FindPath(Tile*map, int w, int h, int ox, int oy, int tx, int ty);
void RemovePath(Path *path);
void DrawPath(Path *path);

typedef struct Projectile
{
	float x;
	float y;
	float z;
	float dirx;
	float diry;
	float dirz;
	int supressionrange;
} Projectile;
typedef struct ProjectileList ProjectileList;
struct ProjectileList
{
	Projectile projectile;
	ProjectileList *next;
};

int FireProjectile(int ox, int oy, int tx, int ty);
void UpdateProjectiles(float deltatime);
void DrawProjectiles();

float DistancePointLine(Vector3 A, Vector3 B, Vector3 Q);
Vector3 ProjectOnSegment(Vector3 A, Vector3 B, Vector3 Q);
float SquareDistance(Vector3 A, Vector3 B);
int InSupressionRange(Projectile projectile, Vector3 pos);

// unit stuff
typedef struct Unit
{
	float posx;
	float posy;
	float rotation;
	float rotationspeed;
	Model model;
	Path *path;

	int fposx;
	int fposy;

	float movespeed;
	float movespeedtick;

	int hp;
} Unit;
typedef struct UnitList UnitList;
struct UnitList
{
	Unit *unit;
	UnitList *next;
};


Unit *CreateUnit(float x, float y);
void DrawUnits();
void DrawUnitText();
void UpdateUnits(float deltatime);

Unit *SelectUnit(Camera camera);
void RotateUnit(Unit *unit, float rotation);

void DrawPlayerSpecial(Unit *player);
void MovePlayer(Unit *unit,float deltatime,int dir);
void RotatePlayer(Unit *unit, float deltatime,int dir);

void AddToList(UnitList *list, Unit *unit);
void ClearList(UnitList *list);
// spell stuff
/*
spell DB
id
name
targettype:	0 SELF 	1 AREA 	2 ARC
TP1					NONE 		RANGE 	RANGE
TP2 				NONE 		NONE 		ARCSIZE
TP3 				NONE 		NONE 		ARCOFFSET
effect:	0 NONE  1 DAMAGE 	2 HEAL
EP1			NONE		AMNT			AMNT
*/
enum spell_target_type
{
	SELF 			= 0,
	AREA 			= 1,
	ARC 			= 2,
};
enum spell_effect_type
{
	NONE 			= 0,
	DAMAGE 		= 1,
	HEAL 			= 2,
};
typedef struct Spell
{
	int id;
	char name[20];
	int targettype;
	int TP1;
	int TP2;
	int TP3;
	int effect1;
	int EP1;
} Spell;

void LoadSpellsDB();
void UnloadSpellsDB();
void PrintSpellsDB();
void CastSpell(Spell spell, Unit *caster);

UnitList *SelectTargets(Spell spell,Unit *caster);

void DoDamage(UnitList *targets,int BP);
void DoHeal(UnitList *targets,int BP);
// player stuff
/*
typedef struct Player
{
		int posx;
		int posy;
		Model model;

		int movespeed;
		int movespeedtick;
} Player;

Player *CreatePlayer(int x, int y);
void DrawPlayer();
void UpdatePlayer();
*/
// Camera
//void UpdateCamera(Camera *camera);
// globals
#define DISTANCEMARGIN 1

Camera camera;

Tile *map;
Model mapmodel;
Texture2D maptexture;

ProjectileList *projectilelist;

UnitList *FriendlyList;

Spell *SpellList;
int SpellListCount;
#endif
