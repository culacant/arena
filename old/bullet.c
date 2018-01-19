#include "arenathing.h"

#define GRAVITY 1

// projectile stuff

int FireProjectile(int ox, int oy, int tx, int ty)
{
	int distance = sqrt((tx-ox)*(tx-ox)+(ty-oy)*(ty-oy));
// out of range
	float vel = 10;
	if(distance > (vel*vel)/GRAVITY)
	{
		printf("out of range");
		return 0;
	}

	float accuracyz = (rand()%11-5)*DEG2RAD;
	float accuracy = (rand()%11-5)*DEG2RAD;
// indirect fire
//	float anglez = 90*DEG2RAD - 0.5*asin((GRAVITY*distance)/(vel*vel));
	float anglez = 0.5*asin((GRAVITY*distance)/(vel*vel))+accuracyz;
	float dirhor = cos(anglez)*vel;
	float angle = atan2(ty - oy, tx - ox)+accuracy;

	ProjectileList *looper = projectilelist;
	if(looper)
	{
		while(looper->next)
		{
			looper = looper->next;
		}
		looper->next = malloc(sizeof(ProjectileList));
		looper = looper->next;
	}
	else
	{
		projectilelist = malloc(sizeof(ProjectileList));
		looper = projectilelist;
	}
	looper->next = NULL;

	looper->projectile.supressionrange = 5;

	looper->projectile.x = ox;
	looper->projectile.y = oy;
	looper->projectile.z = 0.05f;

	looper->projectile.dirz= sin(anglez)*vel;
	looper->projectile.diry = sin(angle)*dirhor;
	looper->projectile.dirx = cos(angle)*dirhor;
	return 1;
}

void UpdateProjectiles(float deltatime)
{
	ProjectileList *looper = projectilelist;
	ProjectileList *last = NULL;
	while(looper)
	{
// gravity
		looper->projectile.dirz -= GRAVITY;

		looper->projectile.x += looper->projectile.dirx;
		looper->projectile.y += looper->projectile.diry;
		looper->projectile.z += looper->projectile.dirz;
// supression
		if(looper->projectile.z < 3)
		{
			int x = looper->projectile.x;
			int y = looper->projectile.y;
		}
// remove projectiles
		if(looper->projectile.z <= 0)
		{
				if(last)
				{
					last->next = looper->next;
					free(looper);
					looper = looper->next;
				}
				else
				{
					projectilelist = looper->next;
					free(looper);
					looper = projectilelist;
				}
		}
		else
		{
			last = looper;
			looper = looper->next;
		}
	}
}

void DrawProjectiles()
{
	ProjectileList *looper = projectilelist;
	while(looper)
	{
		DrawCube((Vector3){looper->projectile.x,looper->projectile.z,looper->projectile.y},0.5f,0.5f,0.5f,YELLOW);
		DrawLine3D((Vector3){looper->projectile.x-looper->projectile.dirx,looper->projectile.z-looper->projectile.dirz,looper->projectile.y-looper->projectile.diry},(Vector3){looper->projectile.x,looper->projectile.z,looper->projectile.y},BLUE);
		DrawLine3D((Vector3){looper->projectile.x+looper->projectile.dirx,(looper->projectile.z+looper->projectile.dirz-GRAVITY),looper->projectile.y+looper->projectile.diry},(Vector3){looper->projectile.x,looper->projectile.z,looper->projectile.y},BLUE);
		looper = looper->next;
	}
}

float DistancePointLine(Vector3 A, Vector3 B, Vector3 Q)
{
	Vector3 PP = ProjectOnSegment(A,B,Q);
	if(SquareDistance(A,PP)+SquareDistance(B,PP)>SquareDistance(A,B)+DISTANCEMARGIN)
	{
		if(SquareDistance(A,PP)>SquareDistance(B,PP))
			PP = B;
		else
			PP = A;
	}
	return Vector3Distance(PP,Q);
}

Vector3 ProjectOnSegment(Vector3 A, Vector3 B, Vector3 Q)
{
	float DotProduct;
	Vector3 PP;
	Vector3 AB = Vector3Subtract(B,A);
	Vector3 AQ = Vector3Subtract(Q,A);
	Vector3Normalize(&AB);
	DotProduct = Vector3DotProduct(AQ,AB);
	PP = Vector3Multiply(AB,DotProduct);
	PP = Vector3Add(PP,A);
	return PP;
}
float SquareDistance(Vector3 A, Vector3 B)
{
	float dx = B.x - B.x;
	float dy = B.y - B.y;
	float dz = B.z - B.z;
	return dx*dx+dy*dy+dz*dz;
}

int InSupressionRange(Projectile projectile, Vector3 pos)
{
/*
		if the distance < supression range, supress that mofo
*/
	if(DistancePointLine((Vector3){projectile.x,projectile.z,projectile.y},(Vector3){projectile.x+projectile.dirx,projectile.z+projectile.dirz-GRAVITY,projectile.y+projectile.diry},pos)<projectile.supressionrange)
		return true;
	else
		return false;
}
