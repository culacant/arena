#include "arena.h"
#include "keybinds.h"

int main()
{
  int screenWidth = 800;
  int screenHeight = 600;
  float deltatime = 0;
  InitWindow(screenWidth,screenHeight,"arena");
  SetTargetFPS(60);

// camera stuff
	camera.position = (Vector3){10.0f,10.0f,10.0f};
	camera.target 	= (Vector3){0.0f,0.0f,0.0f};
	camera.up 	= (Vector3){0.0f,1.0f,0.0f};
	camera.fovy 	= 45.0f;
	SetCameraMode(camera,CAMERA_FREE);

// map
  LoadMap("");
  NodeList *navnodes;
  Node *start = malloc(sizeof(Node));
  start->position = (Vector3){0.0f,0.0f,0.0f};
  navnodes = GetNavNodes(start);
// units
  ulist = LoadUnitsDB();
//  CreateUnit((Vector3){0.0f,0.0f,0.0f},2,&ulist);
  Unit *selected = NULL;

// projectiles
  plist = NULL;
  LoadSpellDB();
  while(!WindowShouldClose())
  {


    deltatime = GetFrameTime()*100;
// updates
    UpdateCamera(&camera);
    UpdateUnits(deltatime,ulist,&plist);
    UpdateProjectiles(deltatime,&plist);


    ShowCursor();
// inputs
    if(IsKeyReleased(KEY_COMMAND_FIRE))
    {
      if(selected)
      {
        CastSpell(SpellList[7],selected);
      }
    }
    if(IsKeyReleased(KEY_COMMAND_MOVE))
    {
      if(selected)
      {
        Vector3 tpos = GetMouseHitModel(camera,map);
        tpos = Vector3Round(tpos);
        NodeList *path = FindPath(Vector3Round(selected->position),tpos,navnodes);
        if(path)
          SetPath(selected,path);
      }
    }
    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
      selected = SelectUnit(camera,ulist);
    }
// drawing
    BeginDrawing();
      ClearBackground(RAYWHITE);
      Begin3dMode(camera);

        DrawMap();
        DrawUnits(ulist);
DrawNodeList(navnodes,GREEN);

        if(selected)
        {
          if(selected->path)
          {
            DrawNodeList(selected->path,PURPLE);
          }
        }
        DrawProjectiles(plist);
      End3dMode();
// moved to 2d for testing line instead of model
// UI
      if(selected)
      {
        char *state;
        switch(selected->state)
        {
          case MOVING:
            state = "moving";
          break;
          case FIRING:
            state = "firing";
          break;
          case RELOADING:
            state = "reloading";
          break;
          case NORANGE:
            state = "out of range";
          break;
          default:
            state = "idle";
          break;
        }
        DrawText(FormatText("state: %s", state), 20, screenHeight-125, 20, RED);
        DrawText(FormatText("hp: %i", selected->hp), 20, screenHeight-25, 20, RED);
      }
    EndDrawing();
  }
  FreeNodeList(&navnodes);
  FreeUnitList(&ulist);
  FreeProjectileList(&plist);
//  free(start);
  UnloadMap();
  CloseWindow();
  return 0;
}
