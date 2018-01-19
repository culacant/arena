#include "arenathing.h"
#include "keybinds.h"

typedef enum
{
	GAME = 0,
	MENU = 1,
}GameState;

int main()
{
	GameState state = GAME;
// setup raylib
	int screenWidth 	= 800;
	int screenHeight 	= 450;
	InitWindow(screenWidth, screenHeight, "hello world");
	SetTargetFPS(60);
	switch(state)
	{
		case MENU:
			RunMenu();
			break;
		case GAME:
			RunGame();
			break;
	}
	CloseWindow();
	return 0;
}

void RunGame()
{
	LoadSpellsDB();
	PrintSpellsDB();
// map stuff
	int width, height;
	map = LoadMap(&width, &height);

	FriendlyList = NULL;

// unit stuff
	Unit *player = CreateUnit(10,10);
	CreateUnit(1,5);
	CreateUnit(5,5);

	Unit *selected = NULL;

// camera stuff
	camera.position = (Vector3){10.0f,10.0f,10.0f};
	camera.target 	= (Vector3){0.0f,0.0f,0.0f};
	camera.up 	= (Vector3){0.0f,1.0f,0.0f};
	camera.fovy 	= 45.0f;
	SetCameraMode(camera,CAMERA_FREE);

	Path *path = NULL;

	printf("w:%i,h:%i",width,height);

	float deltatime;

// main loop
	while(!WindowShouldClose())
	{
		deltatime = GetFrameTime();
		UpdateUnits(deltatime);
		UpdateProjectiles(deltatime);
		UpdateCamera(&camera);
		ShowCursor();

		BeginDrawing();
		ClearBackground(RAYWHITE);
		DrawText(FormatText("%i",GetFPS()), 10,10,10,BLACK);

		Begin3dMode(camera);

		DrawMap(map, width,height);
// input
		if(IsKeyDown(KEY_FORWARD))
			MovePlayer(player,deltatime,1);
		if(IsKeyDown(KEY_BACKWARD))
			MovePlayer(player,deltatime,-1);
		if(IsKeyDown(KEY_TURN_RIGHT))
			RotatePlayer(player,deltatime,1);
		if(IsKeyDown(KEY_TURN_LEFT))
			RotatePlayer(player,deltatime,-1);
		if(IsKeyDown(KEY_ATTACK1))
			CastSpell(SpellList[5],player);
		if(IsKeyDown(KEY_ATTACK2))
			CastSpell(SpellList[2],player);
		if(IsKeyDown(KEY_ATTACK3))
			CastSpell(SpellList[3],player);
		if(IsKeyDown(KEY_ATTACK4))
			CastSpell(SpellList[4],player);

		if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
		{
			selected = SelectUnit(camera);
		}
		if(IsKeyDown(KEY_COMMAND_MOVE))
		{
			if(selected)
			{
				Ray ray;
				ray = GetMouseRay(GetMousePosition(),camera);
				RayHitInfo groundhit = GetCollisionRayGround(ray,0.0f);
				if(groundhit.hit)
				{
					int x,y;
					x = (int)groundhit.position.x;
					y = (int)groundhit.position.z;
					printf("hit on: %i,%i\n",x,y);
					RemovePath(selected->path);
					selected->path = NULL;

					if(path = FindPath(map,width,height,(int)selected->posx,(int)selected->posy,x,y))
					{
						selected->path = path;
						path = NULL;
					}

					DrawCube(groundhit.position,0.3,0.3,0.3,RED);
				}
			}
		}
		if(IsKeyDown(KEY_COMMAND_FIRE))
		{
			if(selected)
			{
				Ray ray;
				ray = GetMouseRay(GetMousePosition(),camera);
				RayHitInfo groundhit = GetCollisionRayGround(ray,0.0f);
				if(groundhit.hit)
				{
					int x,y;
					x = (int)groundhit.position.x;
					y = (int)groundhit.position.z;
					selected->fposx = x;
					selected->fposy = y;
					DrawCube(groundhit.position,0.3,0.3,0.3,RED);
				}
			}
		}
		if(IsKeyDown(KEY_COMMAND_CLEAR))
		{
			if(selected)
			{
				selected->fposx = -1;
				selected->fposy = -1;
			}
		}
// selected
		if(selected)
		{
			if(selected->path)
				DrawPath(selected->path);
		}

		DrawUnits();
		DrawProjectiles();

		DrawPlayerSpecial(player);

		End3dMode();
// 2d drawing
		DrawUnitText();
		EndDrawing();
	}

	UnloadSpellsDB();
// done
	free(map);
}
