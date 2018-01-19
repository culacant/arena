#ifndef ARENA_H
#define ARENA_H

#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "math.h"
#include "raymath.h"

#include "raylib.h"

// globals
#define NUMNEIGHBORS 8
#define GRIDRESOLUTION 1
#define RAYHITLENGTHHOR GRIDRESOLUTION+5.0f
#define RAYHITLENGTHVERT GRIDRESOLUTION+5.0f
#define MAX_LINE_LENGHT 1024

#define DISTANCEMARGIN 1.0f

Model map;
Camera camera;

/*
spell DB
id
name
targettype:	0 SELF 	1 AREA 	2 ARC
TP1					NONE 		RANGE 	RANGE
TP2 				NONE 		NONE 		ARCSIZE
TP3 				NONE 		NONE 		ARCOFFSET
effect:	0 NONE  1 DAMAGE 	2 HEAL  3 PROJECTILE
EP1			NONE		AMNT			AMNT    SPEED
EP2     NONE    NONE      NONE    effect2
delivery: 0 INSTANT 1 PROJECTILE
DP1       NONE      SPEED
*/
enum spell_target_type
{
  SELF       = 0,
  AREA       = 1,
  ARC        = 2,
};
enum spell_effect_type
{
  NONE       = 0,
  DAMAGE     = 1,
  HEAL       = 2,
};
enum spell_delivery_type
{
  INSTANT    = 0,
  PROJECTILE = 1,
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
  int deliverytype;
  int DP1;
} Spell;
// structs
typedef struct Node Node;
struct Node
{
  Vector3 position;
  Node *neighbors[NUMNEIGHBORS];
  float fcost;
  float gcost;
  Node *prev;
};

typedef struct NodeList NodeList;
struct NodeList
{
  Node *node;
  NodeList *next;
};

typedef enum
{
  IDLE,
  FIRING,
  RELOADING,
  NORANGE,
  MOVING
} UnitState;
typedef struct Unit
{
  int hp;
  Vector3 position;
  float movespeed;
  float rotation;
  float rotationy;
  Model model;
  NodeList *path;
  Vector3 movetarget;
  UnitState state;
} Unit;

typedef struct UnitList UnitList;
struct UnitList
{
  Unit *unit;
  UnitList *next;
};

typedef struct Projectile
{
  Vector3 position;
  Vector3 movespeed;
  int ttl;
  Spell* spell;
  Unit *caster;
  Model model;
} Projectile;

typedef struct ProjectileList ProjectileList;
struct ProjectileList
{
  Projectile *projectile;
  ProjectileList *next;
};
// spell functions
void LoadSpellDB();
void UnloadSpellDB();
void PrintSpellDB();
void CastSpell(Spell spell, Unit *caster);
UnitList *SelectTargets(Spell spell,Unit *caster);
void DoEffect(Spell spell,Unit *target);
void DoDamage(Spell spell,Unit *target);
void DoHeal(Spell spell,Unit *target);

// math functions
float Vector3SquaredDistance(Vector3 A, Vector3 B);
Vector3 Vector3Round(Vector3 A);
Vector3 Vector3ProjectOnLine(Vector3 A, Vector3 B, Vector3 Q);
float Vector3SquaredDistanceToLine(Vector3 A, Vector3 B, Vector3 Q);

Vector3 GetMouseHitModel(Camera camera, Model model);

// projectiles
Projectile *CreateProjectile(Vector3 opos,Vector3 tpos,Unit *caster,Spell *spell,ProjectileList **list);
void enqueueProjectile(Projectile *projectile,ProjectileList **list);
Projectile *dequeueProjectile(ProjectileList **list);
void RemoveProjectile(Projectile *projectile,ProjectileList **list);
void FreeProjectileList(ProjectileList **list);
int IsInRange(Vector3 opos, Vector3 tpos, float vel);

void DrawProjectiles(ProjectileList *list);
void UpdateProjectiles(int deltatime,ProjectileList **list);

// map
void enqueueNode(Node *node,NodeList **list);
Node *dequeueNode(NodeList **list);
Node *popNode(NodeList **list);
Node *dequeueNodeLowestf(NodeList **list);
Node *GetNodeFromQueueWithPosition(Vector3 position, NodeList *list);
void ClearNodeList(NodeList **list);
void FreeNodeList(NodeList **list);
void PrintNodeList(NodeList *list);
void DrawNodeList(NodeList *list,Color color);

NodeList *GetNavNodes(Node *start);
NodeList *FindPath(Vector3 opos,Vector3 tpos,NodeList *NavNodes);

void LoadMap(const char *filename);
void UnloadMap();
void DrawMap();

// unit
void enqueueUnit(Unit *unit,UnitList **list);
Unit *dequeueUnit(UnitList **list);
void ClearUnitList(UnitList **list);
void FreeUnitList(UnitList **list);
void PrintUnitList(UnitList *list);

Unit *CreateUnit(Vector3 pos,int hp,UnitList **list);
void DrawUnits(UnitList *list);
void UpdateUnits(int deltatime,UnitList *list,ProjectileList **plist);
Unit *SelectUnit(Camera camera,UnitList *list);
void FaceUnit(Unit *unit, Vector3 target);
void SetPath(Unit *unit,NodeList *path);
void SetFireTarget(Unit *unit,Vector3 fpos);

UnitList *LoadUnitsDB();
void PrintUnitsDB();

// globals(2)
Spell *SpellList;
int SpellListCount;
UnitList *ulist;
ProjectileList *plist;

#endif
