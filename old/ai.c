#include "arenathing.h"
#include <string.h>
#include <math.h>

#define MAX_NODES  100000

Node closedset[MAX_NODES];
Node openset[MAX_NODES];

int closedsize;
int opensize;

Node *FindLowestF()
{
	float min = openset[0].f;
	int lowest = 0;
	for(int i=1;i<opensize;i++)
	{
		if(openset[i].f < min)
		{
			min = openset[i].f;
			lowest = i;
		}
	}
	return &openset[lowest];
}
int NodeClosed(Node *node)
{
	for(int i=0;i<closedsize;i++)
	{
		if(closedset[i].x == node->x && closedset[i].y == node->y)
			return 1;
	}
	return 0;
}
int NodeOpen(Node *node)
{
	for(int i=0;i<opensize;i++)
	{
		if(openset[i].x == node->x && openset[i].y == node->y)
			return 1;
	}
	return 0;
}
Node *CloseNode(Node *node)
{
	closedset[closedsize++] = *node;
	for(int i=0;i<opensize;i++)
	{
		if(openset[i].x == node->x && openset[i].y == node->y)
		{
				memmove(&openset[i],&openset[i+1], sizeof(Node)*(opensize-i-1));
				opensize--;
				break;
		}
	}
	return &closedset[closedsize-1];
}

Path *FindPath(Tile*map, int w, int h, int ox, int oy, int tx, int ty)
{
	if(tx<0||ty<0||tx>=w||ty>=h)
		return NULL;
	closedsize = 0;
	opensize = 0;
	openset[opensize++] = (Node){tx,ty,abs(tx-ox)+abs(ty-oy),0,0};
	while(opensize>0)
	{
		Node *current = FindLowestF();
		if(current->x == ox && current->y == oy)
		{
			Path *path = NULL;
			Path* looprev = NULL;
// construct path
			printf("found path\n");
			current = current->prev;
			path = malloc(sizeof(Path));
			looprev = path;
			while(current->x!=tx||current->y!=ty)
			{
				looprev->x = current->x;
				looprev->y = current->y;
				looprev->next = malloc(sizeof(Path));
				//Node *tmp = current;
				current = current->prev;
				//free(tmp);
				looprev = looprev->next;
			}
			looprev->x = current->x;
			looprev->y = current->y;
			looprev->next = NULL;

			return path;
		}
		current = CloseNode(current);
		for(int i=0;i<8;i++)
		{
			Node neighbor = {current->x,current->y,0,0,0};
			switch(i)
			{
				case 0:
					neighbor.y--;
					break;
				case 1:
					neighbor.x++;
					break;
				case 2:
					neighbor.y++;
					break;
				case 3:
					neighbor.x--;
					break;
				case 4:
					neighbor.x--;
					neighbor.y--;
					break;
				case 5:
					neighbor.x++;
					neighbor.y++;
					break;
				case 6:
					neighbor.x--;
					neighbor.y++;
					break;
				case 7:
					neighbor.x++;
					neighbor.y--;
					break;
			}

			if(NodeClosed(&neighbor)||neighbor.x<0||neighbor.y<0||neighbor.x>=w||neighbor.y>=h||map[neighbor.x+neighbor.y*w].canWalk == false)
				continue;
			if(!NodeOpen(&neighbor)||current->g+1<neighbor.g)
			{
				neighbor.prev = current;
				neighbor.g = current->g+1;
				neighbor.f = neighbor.g + abs(neighbor.x-ox)+abs(neighbor.y-oy);
				if(!NodeOpen(&neighbor))
					openset[opensize++] = neighbor;
			}
		}
	}
	printf("couldnt find path\n");
	return 0;
}

void RemovePath(Path *path)
{
	Path *last = NULL;
	while(path)
	{
		last = path;
		path = path->next;
		free(last);
		last = NULL;
	}
}

void DrawPath(Path *path)
{
	Path *looper = path;
	while(looper->next!=NULL)
	{
		looper = looper->next;
		DrawCube((Vector3){looper->x,0.0f,looper->y},1.0f,1.0f,1.0f,YELLOW);
	}
}
