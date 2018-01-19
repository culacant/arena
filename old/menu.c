#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "arenathing.h"

void RunMenu()
{
  bool RunPressed = 0;
  bool ExitPressed = 0;
  while(!WindowShouldClose())
  {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    RunPressed = GuiButton((Rectangle){5,5,20,100},"Run");
    ExitPressed = GuiButton((Rectangle){30,5,45,100},"Exit");
    EndDrawing();
  }
}
