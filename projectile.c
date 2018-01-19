#include "arena.h"
#define GRAVITY 1

Projectile *CreateProjectile(Vector3 opos,Vector3 tpos,Unit *caster,Spell *spell,ProjectileList **list)
{
  Projectile *newprojectile = malloc(sizeof(Projectile));
  float velocity = spell->DP1;
// angle vertical
  float deltaz = (abs(opos.z)-abs(tpos.z))+(abs(opos.x)-abs(tpos.x));
  float rotationy = atan2(opos.y-tpos.y,deltaz);
  rotationy = fmod(rotationy,2*PI);

// facing horizontal
  float rotation = atan2(tpos.z-opos.z,tpos.x-opos.x);
  rotation = fmod(rotation,2*PI);

//  Vector3Normalize(&tpos);
  tpos.y = sin(rotationy);
	tpos.z = sin(rotation);
	tpos.x = cos(rotation);
  tpos = Vector3Multiply(tpos,velocity);

  newprojectile->caster = caster;
  newprojectile->position = opos;
  newprojectile->position->y += 1.0f;
  newprojectile->movespeed = tpos;
  newprojectile->ttl = 100;
  newprojectile->spell = spell;
  newprojectile->model = LoadModel("./resources/rocket.obj");
	newprojectile->model.transform = MatrixMultiply(MatrixIdentity(),MatrixRotateY((float)rotation));
  enqueueProjectile(newprojectile,list);
  return newprojectile;
}
void enqueueProjectile(Projectile *projectile,ProjectileList **list)
{
  ProjectileList *newprojectile = malloc(sizeof(ProjectileList));
  newprojectile->projectile = projectile;
  newprojectile ->next = *list;
  *list = newprojectile;
}
Projectile *dequeueProjectile(ProjectileList **list)
{
  ProjectileList *looper = NULL;
  ProjectileList *last = NULL;
  Projectile *projectile = NULL;
  if(*list == NULL)
    return projectile;
  looper = *list;
  while(looper->next != NULL)
  {
    last = looper;
    looper = looper->next;
  }
  projectile = looper->projectile;
  free(looper);
  if(last)
    last->next = NULL;
  else
    *list = NULL;
  return projectile;
}
void RemoveProjectile(Projectile *projectile,ProjectileList **list)
{
  ProjectileList *looper = *list;
  ProjectileList *last = NULL;
  Projectile *current = NULL;
  if(*list == NULL)
    return;
  while(looper!=NULL)
  {
    current = looper->projectile;
    if(current == projectile)
    {
        if(last)
          last->next = looper->next;
        else
          *list = NULL;
        free(current);
        free(looper);
        return;
    }
    last = looper;
    looper = looper->next;
  }
  return;
}
void FreeProjectileList(ProjectileList **list)
{
  Projectile *projectile;
  while(*list)
  {
    projectile = dequeueProjectile(list);
    free(projectile);
  }
}
void DrawProjectiles(ProjectileList *list)
{
  ProjectileList *looper = list;
  while(looper)
  {
    DrawModel(looper->projectile->model,looper->projectile->position,1.0f,YELLOW);
    looper = looper->next;
  }
}
void UpdateProjectiles(int deltatime,ProjectileList **list)
{
  ProjectileList *looper = *list;
  Projectile *current;
  while(looper)
  {
// do updates
    current = looper->projectile;
    current->ttl -= 1;
    if(current->ttl < 0)
    {
// expires
      looper = looper->next;
      RemoveProjectile(current,list);
    }
    else
    {
      Ray ray;
      RayHitInfo hitrayinfo;
      ray.position = current->position;
      ray.direction = current->movespeed;

// collision with units
UnitList *ulooper = ulist;
while(ulooper)
{
  if(ulooper->unit != current->caster)
  {
    BoundingBox bbox;
    bbox = CalculateBoundingBox(ulooper->unit->model.mesh);
    bbox.min = Vector3Add(bbox.min,ulooper->unit->position);
    bbox.max= Vector3Add(bbox.max,ulooper->unit->position);
    hitrayinfo = GetCollisionRayMesh(ray,&ulooper->unit->model.mesh);
    if(hitrayinfo.hit)
    {
      float hitdist = Vector3SquaredDistance(hitrayinfo.position,ray.position);
      float nextdist = Vector3SquaredDistance(current->position,Vector3Add(current->position,ray.direction));
      if(nextdist > hitdist)
      {
        DoEffect(*current->spell,ulooper->unit);
        current->ttl = 0;
      }
    }
  }
  else
  ulooper = ulooper->next;
}
      hitrayinfo = GetCollisionRayMesh(ray,&map.mesh);
      // hit terrain
      if(hitrayinfo.hit)
      {
        float hitdist = Vector3SquaredDistance(hitrayinfo.position,ray.position);
        float nextdist = Vector3SquaredDistance(current->position,Vector3Add(current->position,ray.direction));
        if(nextdist > hitdist)
        {
          looper = looper->next;
          RemoveProjectile(current,list);
        }
        // clear flightpath
        else
        {
            current->position = Vector3Add(current->position,current->movespeed);
            looper = looper->next;
        }
      }
      // clear flightpath
      else
      {
        current->position = Vector3Add(current->position,current->movespeed);
        looper = looper->next;
      }
    }
  }
}
