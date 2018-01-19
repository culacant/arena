#include "arenathing.h"

// unit stuff
Unit *CreateUnit(float x, float y)
{
	UnitList *looper = FriendlyList;
	if(looper)
	{
		while(looper->next)
		{
			looper = looper->next;
		}
		looper->next = malloc(sizeof(UnitList));
		looper = looper->next;
	}
	else
	{
		FriendlyList = malloc(sizeof(UnitList));
		looper = FriendlyList;
	}


	looper->next = NULL;
	looper->unit = malloc(sizeof(Unit));
	looper->unit->model = LoadModel("resources/infantry1.obj");
	looper->unit->posx = x;
	looper->unit->posy = y;
	looper->unit->rotation = 0;
	looper->unit->rotationspeed = 10;
	looper->unit->fposx = -1;
	looper->unit->fposy = -1;
	looper->unit->path = NULL;
// in seconds
	looper->unit->movespeed = 1;
	looper->unit->movespeedtick = looper->unit->movespeed;

	looper->unit->hp = 100;


	return looper->unit;
}
void DrawUnits()
{
	UnitList *looper = FriendlyList;
	while(looper)
	{
		DrawModel(looper->unit->model,(Vector3){looper->unit->posx,0.0f,looper->unit->posy},1.0f,BLUE);
		looper = looper->next;
	}
}
void DrawUnitText()
{
	UnitList *looper = FriendlyList;
	while(looper)
	{
		char text[10];
		sprintf(text,"hp:%i",looper->unit->hp);
		Vector2 UnitScreenPos = GetWorldToScreen((Vector3){looper->unit->posx,2.5f,looper->unit->posy},camera);
		DrawText(text,UnitScreenPos.x,UnitScreenPos.y,20,BLACK);
		looper = looper->next;
	}
}
void UpdateUnits(float deltatime)
{
	UnitList *looper = FriendlyList;
	while(looper)
	{
// update unit
// path
		if(looper->unit->path != NULL)
		{
			float rotation = atan2((int)looper->unit->posy-looper->unit->path->y,(int)looper->unit->posx-looper->unit->path->x);
			if(rotation <0)
				rotation += 2*PI;
			RotateUnit(looper->unit,rotation);
			looper->unit->posy -= looper->unit->movespeed * deltatime * sin(looper->unit->rotation);
			looper->unit->posx -= looper->unit->movespeed * deltatime * cos(looper->unit->rotation);
			if((int)looper->unit->posy == looper->unit->path->y && (int)looper->unit->posx == looper->unit->path->x)
			{
				Path *lastpath = looper->unit->path;
				looper->unit->path = looper->unit->path->next;
				free(lastpath);
				lastpath = NULL;
			}
		}
		if(looper->unit->fposx!=-1 && looper->unit->fposy!=-1)
		{
		// fire
			FireProjectile(looper->unit->posx,looper->unit->posy,looper->unit->fposx, looper->unit->fposy);
		}
		looper = looper->next;
	}
}

Unit *SelectUnit(Camera camera)
{
	Ray mouseray;
	BoundingBox bbox;

	mouseray = GetMouseRay(GetMousePosition(),camera);


	UnitList *looper = FriendlyList;
	while(looper)
	{
		bbox = (BoundingBox){(Vector3){looper->unit->posx - 0.5,0,looper->unit->posy-0.5},
				     (Vector3){looper->unit->posx + 0.5,1,looper->unit->posy+0.5}};
		DrawBoundingBox(bbox,RED);
		if(CheckCollisionRayBox(mouseray,bbox))
		{
			printf("selected");
//			RotateUnit(&looper->unit,45);
			return looper->unit;
		}
		looper = looper->next;
	}

	return NULL;
}

void RotateUnit(Unit *unit, float rotation)
{
	if(rotation > 2*PI)
		rotation -= 2*PI;
	if(rotation < 0)
		rotation += 2*PI;
	unit->rotation = rotation;
	unit->model.transform = MatrixMultiply(MatrixIdentity(),MatrixRotateY((float)rotation));
}
void MovePlayer(Unit *unit,float deltatime,int dir)
{
			unit->posy += unit->movespeed * deltatime * sin(unit->rotation)*dir;
			unit->posx += unit->movespeed * deltatime * cos(unit->rotation)*dir;
}
void RotatePlayer(Unit *unit, float deltatime,int dir)
{
		float rotation;
		rotation = unit->rotation + (deltatime *unit->rotationspeed)*dir;
		RotateUnit(unit, rotation);
}
void AttackRound(Unit*unit,int range,int dmg)
{
	UnitList *looper = FriendlyList;
	while(looper)
	{
		if(looper->unit != unit)
		{
			if((looper->unit->posx - unit->posx)*(looper->unit->posx - unit->posx)+(looper->unit->posy - unit->posy)*(looper->unit->posy - unit->posy) <=(float)(range*range))
				looper->unit->hp -= dmg;
		}
		looper = looper->next;
	}
}
void DrawPlayerSpecial(Unit *player)
{
	DrawCircle3D((Vector3){player->posx,0.0f,player->posy},12.0f,(Vector3){0.0f,0.0f,0.0f},0,RED);
}

void AddToList(UnitList *list, Unit *unit)
{
	UnitList *looper = list;
	if(looper->unit)
	{
		while(looper->next)
		{
			looper = looper->next;
		}
		looper->next = malloc(sizeof(UnitList));
		looper = looper->next;
	}
	else
	{
		looper = list;
	}
	looper->unit = unit;
	looper->next = NULL;
}
void ClearList(UnitList *list)
{
	UnitList *looper = list;
	UnitList *prev;
	if(!looper)
		return;
	while(looper->next)
	{
		prev = looper;
		looper = looper->next;
		free(prev);
	}
	free(looper);
}
