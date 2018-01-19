#include "arena.h"
#include <string.h>
/*
Spell *SpellList;
int SpellListCount = 0;
*/
#define MAX_LINE_LENGHT 1024
void LoadSpellDB()
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
    tok = strtok(NULL,",");
    SpellList[count].EP1=atoi(tok);
    tok = strtok(NULL,",");
    SpellList[count].deliverytype=atoi(tok);
    tok = strtok(NULL,"\n");
    SpellList[count].DP1=atoi(tok);
    count++;
  }
  fclose(file);
}
void UnloadSpellDB()
{
  free(SpellList);
  SpellListCount = 0;
}
void PrintSpellDB()
{
  for(int i=0;i<SpellListCount;i++)
    printf("id: %i\nname: %s\ntargettype: %i\neffect1: %i\nEP1: %i\n",SpellList[i].id,SpellList[i].name,SpellList[i].targettype,SpellList[i].effect1,SpellList[i].EP1);
}

UnitList *SelectTargets(Spell spell,Unit *caster)
{
  UnitList *targets = NULL;
  UnitList *looper;
  switch(spell.targettype)
  {
    case SELF:
      enqueueUnit(caster,&targets);
      break;
    case AREA:
      looper = ulist;
      while(looper)
      {
        if(looper->unit != caster)
        {
          if(Vector3SquaredDistance(looper->unit->position,caster->position)<=(float)(spell.TP1*spell.TP1))
            enqueueUnit(looper->unit,&targets);
        }
        looper = looper->next;
      }
      break;
    case ARC:
      looper = ulist;
      while(looper)
      {
        if(looper->unit != caster)
        {
          int angle = atan2(looper->unit->position.z-caster->position.z,looper->unit->position.x-caster->position.x)*RAD2DEG;
          angle = caster->rotation*RAD2DEG-angle;
          angle = angle%360;
          if(angle>180)
            angle -=360;
//          printf("angle: %i\n",angle);
 //         printf("%i,%i\n",spell.TP3+spell.TP2/2, spell.TP3-spell.TP2/2);
          if(angle < spell.TP3+spell.TP2/2 && angle > spell.TP3-spell.TP2/2)
            if(Vector3SquaredDistance(looper->unit->position,caster->position)<=(float)(spell.TP1*spell.TP1))
              enqueueUnit(looper->unit,&targets);
        }
        looper = looper->next;
      }
      break;
  }
  return targets;
}
void CastSpell(Spell spell, Unit *caster)
{
  UnitList *targets = SelectTargets(spell,caster);
  if(!targets)
  {
printf("no targets found\n");
    return;
  }
printf("targets found\n");
printf("DP1: %i\n",spell.deliverytype);
  UnitList *looper;
  switch(spell.deliverytype)
  {
    case INSTANT:
printf("case instant\n");
      looper = targets;
      while(looper)
      {
        DoEffect(spell,looper->unit);
      }
      break;
    case PROJECTILE:
printf("case projectile\n");
      looper = targets;
      while(looper)
      {
printf("looping\n");
        CreateProjectile(caster->position,looper->unit->position,caster,&spell,&plist);
        looper = looper->next;
      }
      break;
  }
  ClearUnitList(&targets);
}
void DoEffect(Spell spell,Unit *target)
{
  switch(spell.effect1)
  {
    case DAMAGE:
      DoDamage(spell,target);
      break;
    case HEAL:
      DoHeal(spell,target);
      break;
  }
}
void DoDamage(Spell spell,Unit *target)
{
  target->hp -= spell.EP1;
}
void DoHeal(Spell spell,Unit*target)
{
  target->hp += spell.EP1;
}
