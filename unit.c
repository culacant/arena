#include "arena.h"

void enqueueUnit(Unit *unit,UnitList **list)
{
  UnitList *newunit = malloc(sizeof(UnitList));
  newunit->unit = unit;
  newunit->next = *list;
  *list = newunit;
}
Unit *dequeueUnit(UnitList **list)
{
  UnitList *looper = NULL;
  UnitList *last = NULL;
  Unit *unit = NULL;
  if(*list == NULL)
    return unit;
  looper = *list;
  while(looper->next != NULL)
  {
    last = looper;
    looper = looper->next;
  }
  unit = looper->unit;
  free(looper);
  if(last)
    last->next = NULL;
  else
    *list = NULL;
  return unit;
}
void ClearUnitList(UnitList **list)
{
	while(*list)
	{
		dequeueUnit(list);
	}
}
void FreeUnitList(UnitList **list)
{
  Unit *unit;
  while(*list)
  {
    unit = dequeueUnit(list);
    UnloadModel(unit->model);
    free(unit);
  }
}
void PrintUnitList(UnitList *list)
{
  UnitList *looper = list;
  while(looper != NULL)
  {
    printf("unit at: x:%f,z:%f,y:%f",looper->unit->position.x,looper->unit->position.z,looper->unit->position.y);
    looper = looper->next;
  }
}
Unit *CreateUnit(Vector3 pos,int hp,UnitList **list)
{
  Unit *newunit = malloc(sizeof(Unit));
  newunit->position.x = pos.x;
  newunit->position.z = pos.z;
  newunit->position.y = pos.y;
  newunit->movespeed = 0.1f;
  newunit->rotation = 5.0f;
  newunit->rotationy = 0.0f;
  newunit->model = LoadModel("resources/infantry1.obj");
  newunit->path = NULL;
  newunit->hp = hp;

  enqueueUnit(newunit,list);
  return newunit;
}
void DrawUnits(UnitList *list)
{
  UnitList *looper = list;
  while(looper)
  {
    DrawModel(looper->unit->model,looper->unit->position,1.0f,BLUE);
    looper = looper->next;
  }
}
void UpdateUnits(int deltatime,UnitList *list,ProjectileList **plist)
{
  UnitList *looper = list;
  Unit *current;
  while(looper)
  {
// do updates
    current = looper->unit;
    switch(current->state)
    {
      case MOVING:
        if(current->path)
        {
          Vector3 rounded1 = Vector3Round(current->position);
          Vector3 rounded2 = Vector3Round(current->path->node->position);
    // movement
    			current->position.z -= current->movespeed * sin(current->rotation) * deltatime;
    			current->position.x -= current->movespeed * cos(current->rotation) * deltatime;
     			current->position.y -= current->movespeed * sin(current->rotationy) * deltatime;
    // next node
          if(Vector3SquaredDistance(rounded1,rounded2) <= DISTANCEMARGIN)
          {
            popNode(&current->path);
            if(current->path)
              FaceUnit(current,current->path->node->position);
            else
              current->state = IDLE;
          }
        }
      break;
      case FIRING:
      break;
      case RELOADING:
      break;
      default:
      break;
    }
    looper = looper->next;
  }
}
Unit *SelectUnit(Camera camera,UnitList *list)
{
  Ray ray;
  BoundingBox bbox;

  ray = GetMouseRay(GetMousePosition(),camera);

  UnitList *looper = list;
  while(looper)
  {
    bbox = CalculateBoundingBox(looper->unit->model.mesh);
    bbox.min = Vector3Add(bbox.min,looper->unit->position);
    bbox.max= Vector3Add(bbox.max,looper->unit->position);
    if(CheckCollisionRayBox(ray,bbox))
    {
      return looper->unit;
    }
    looper = looper->next;
  }
  return NULL;
}
void FaceUnit(Unit *unit, Vector3 target)
{
      Vector3 rounded1 = Vector3Round(unit->position);
      Vector3 rounded2 = Vector3Round(target);
  		float rotation = atan2(rounded1.z-rounded2.z,rounded1.x-rounded2.x);
      if(rotation < 0)
        rotation += 2*PI;
      rotation = fmod(rotation,2.0f*PI);
      unit->rotation = rotation;
    	unit->model.transform = MatrixMultiply(MatrixIdentity(),MatrixRotateY((float)rotation));
    float deltaz = (abs(rounded1.z)-abs(rounded2.z))+(abs(rounded1.x)-abs(rounded2.x));
  		rotation = atan2(rounded1.y-rounded2.y,deltaz);
      if(rotation < 0)
        rotation += 2*PI;
      rotation = fmod(rotation,2.0f*PI);
      unit->rotationy = rotation;
}
void SetPath(Unit *unit,NodeList *path)
{
  unit->state = MOVING;
  ClearNodeList(&unit->path);
  unit->path = path;
  FaceUnit(unit,unit->path->node->position);
}
UnitList *LoadUnitsDB()
{
  FILE *file = fopen("units.csv","r");
  char line[MAX_LINE_LENGHT];
  char *token;

  UnitList *ulist = NULL;
  Vector3 position;
  int hp;
// first line
  fgets(line,MAX_LINE_LENGHT,file);
  while(fgets(line,MAX_LINE_LENGHT,file))
  {
    token = strtok(line,",");
//id
    token = strtok(NULL,",");
    position.x = (float)atoi(token);
    token = strtok(NULL,",");
    position.z = (float)atoi(token);
    token = strtok(NULL,",");
    position.y = (float)atoi(token);
    token = strtok(NULL,"\n");
    hp = atoi(token);
// addunit
    CreateUnit(position,hp,&ulist);
  }
  fclose(file);
  return ulist;
}
void PrintUnitsDB();
