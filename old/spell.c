#include "arenathing.h"
#include <string.h>
/*
Spell *SpellList;
int SpellListCount = 0;
*/
#define MAX_LINE_LENGHT 1024
void LoadSpellsDB()
{
  SpellListCount = 0;
  FILE *file = fopen("spelldb.csv","r");
  int count = 0;
  int num = 0;
  char line[MAX_LINE_LENGHT];
  char *tok;
// spell count
  while(fgets(line,MAX_LINE_LENGHT,file))
  {
    SpellListCount++;
  }
  SpellList = malloc(SpellListCount*sizeof(Spell));
  fclose(file);
  file = fopen("spelldb.csv","r");
  while(fgets(line,MAX_LINE_LENGHT,file))
  {
    tok = strtok(line,",");
    SpellList[count].id=atoi(tok);
    tok = strtok(NULL,",");
    strcpy(SpellList[count].name,tok);
    tok = strtok(NULL,",");
    SpellList[count].targettype=atoi(tok);
    tok = strtok(NULL,",");
    SpellList[count].TP1=atoi(tok);
    tok = strtok(NULL,",");
    SpellList[count].TP2=atoi(tok);
    tok = strtok(NULL,",");
    SpellList[count].TP3=atoi(tok);
    tok = strtok(NULL,",");
    SpellList[count].effect1=atoi(tok);
    tok = strtok(NULL,"\n");
    SpellList[count].EP1=atoi(tok);
    count++;
  }
  fclose(file);
}
void UnloadSpellsDB()
{
  free(SpellList);
  SpellListCount = 0;
}
void PrintSpellsDB()
{
  for(int i=0;i<SpellListCount;i++)
    printf("id: %i\nname: %s\ntargettype: %i\neffect1: %i\nEP1: %i\n",SpellList[i].id,SpellList[i].name,SpellList[i].targettype,SpellList[i].effect1,SpellList[i].EP1);
}

UnitList *SelectTargets(Spell spell,Unit *caster)
{
  UnitList *targets = malloc(sizeof(UnitList));
  targets->unit = NULL;
  targets->next = NULL;
  UnitList *looper;
  switch(spell.targettype)
  {
    case SELF:
      AddToList(targets,caster);
      break;
    case AREA:
      looper = FriendlyList;
      while(looper)
      {
        if(looper->unit != caster)
        {
          if((looper->unit->posx-caster->posx)*(looper->unit->posx-caster->posx)+(looper->unit->posy-caster->posy)*(looper->unit->posy-caster->posy)<=(float)(spell.TP1*spell.TP1))
            AddToList(targets,looper->unit);
        }
        looper = looper->next;
      }
      break;
    case ARC:
      looper = FriendlyList;
      while(looper)
      {
        if(looper->unit != caster)
        {
          int angle = atan2(looper->unit->posy-caster->posy,looper->unit->posx-caster->posx)*RAD2DEG;
          angle = caster->rotation*RAD2DEG-angle;
          angle = angle%360;
          if(angle>180)
            angle -=360;
          printf("angle: %i\n",angle);
          printf("%i,%i\n",spell.TP3+spell.TP2/2, spell.TP3-spell.TP2/2);
          if(angle < spell.TP3+spell.TP2/2 && angle > spell.TP3-spell.TP2/2)
            if((looper->unit->posx-caster->posx)*(looper->unit->posx-caster->posx)+(looper->unit->posy-caster->posy)*(looper->unit->posy-caster->posy)<=(float)(spell.TP1*spell.TP1))
              AddToList(targets,looper->unit);
        }
        looper = looper->next;
      }
      break;
  }
  if(targets->unit == NULL)
  {
    ClearList(targets);
    return 0;
  }
  return targets;
}
void CastSpell(Spell spell, Unit *caster)
{
  UnitList *targets = SelectTargets(spell,caster);
  if(!targets)
    return;
  switch(spell.effect1)
  {
    case DAMAGE:
      DoDamage(targets,spell.EP1);
      break;
    case HEAL:
      DoHeal(targets,spell.EP1);
      break;
  }
  ClearList(targets);
}
void DoDamage(UnitList *targets, int BP)
{
  UnitList *looper = targets;
  while(looper)
  {
    looper->unit->hp -= BP;
    looper = looper->next;
  }
}
void DoHeal(UnitList *targets, int BP)
{
  UnitList *looper = targets;
  while(looper)
  {
    looper->unit->hp += BP;
    looper = looper->next;
  }
}
