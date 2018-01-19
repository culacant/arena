#include "arena.h"

float Vector3SquaredDistance(Vector3 A, Vector3 B)
{
  float dx = B.x-A.x;
  float dy = B.y-A.y;
  float dz = B.z-A.z;
  return dx*dx+dy*dy+dz*dz;
}
Vector3 Vector3Round(Vector3 A)
{
  Vector3 rounded;
  rounded.x = floor(A.x+0.5f);
  rounded.y = floor(A.y+0.5f);
  rounded.z = floor(A.z+0.5f);
  return rounded;
}
Vector3 Vector3ProjectOnLine(Vector3 A, Vector3 B, Vector3 Q)
{
	float dotproduct;
	Vector3 projected;
	Vector3 AB = Vector3Subtract(B,A);
	Vector3 AQ = Vector3Subtract(Q,A);
	Vector3Normalize(&AB);
	dotproduct = Vector3DotProduct(AQ,AB);
	projected = Vector3Multiply(AB,dotproduct);
	projected = Vector3Add(projected,A);
	return projected;
}
float Vector3SquaredDistanceToLine(Vector3 A, Vector3 B, Vector3 Q)
{
	Vector3 projected = Vector3ProjectOnLine(A,B,Q);
	if(Vector3SquaredDistance(A,projected)+Vector3SquaredDistance(B,projected)>Vector3SquaredDistance(A,B)+DISTANCEMARGIN)
	{
		if(Vector3SquaredDistance(A,projected)>Vector3SquaredDistance(B,projected))
			projected = B;
		else
			projected = A;
	}
	return Vector3SquaredDistance(projected,Q);
}
Vector3 GetMouseHitModel(Camera camera, Model model)
{
  Ray ray;
  RayHitInfo hitinfo;
  BoundingBox bbox;

  ray = GetMouseRay(GetMousePosition(),camera);
  bbox = CalculateBoundingBox(model.mesh);
  if(CheckCollisionRayBox(ray,bbox))
  {
    hitinfo = GetCollisionRayMesh(ray,&model.mesh);
    if(hitinfo.hit)
      return hitinfo.position;
  }
  return (Vector3){-1.0f,-1.0f,-1.0f};
}
void enqueueNode(Node *node,NodeList **list)
{
  NodeList *newnode = malloc(sizeof(NodeList));
  newnode->node = node;
  newnode->next = *list;
  *list = newnode;
}
Node *dequeueNode(NodeList **list)
{
  NodeList *looper = NULL;
  NodeList *last = NULL;
  Node *node = NULL;
  if(*list == NULL)
    return node;
  looper = *list;
  while(looper->next != NULL)
  {
    last = looper;
    looper = looper->next;
  }
  node = looper->node;
  free(looper);
  if(last)
    last->next = NULL;
  else
    *list = NULL;
  return node;
}
Node *popNode(NodeList **list)
{
  NodeList *looper = NULL;
  Node *node = NULL;
  if(*list == NULL)
    return node;
  looper = *list;
  node = looper->node;
  if(looper->next)
  {
    *list = looper->next;
    free(looper);
  }
  else
    *list = NULL;
  return node;
}
void ClearNodeList(NodeList **list)
{
	while(*list)
	{
		dequeueNode(list);
	}
}
Node *dequeueNodeLowestf(NodeList **list)
{
  NodeList *looper = NULL;
  NodeList *last = NULL;
  Node *node = NULL;
  NodeList *lowestf = NULL;
  if(*list == NULL)
    return node;
  looper = *list;
  lowestf = *list;
  while(looper->next != NULL)
  {
    if(looper->next->node->fcost < lowestf->node->fcost)
    {
      last = looper;
      lowestf=looper->next;
    }
    looper = looper->next;
  }
  node = lowestf->node;
  if(last)
  {
    last->next = lowestf->next;
    free(lowestf);
  }
  else
    *list = NULL;
  return node;
}
Node *GetNodeFromQueueWithPosition(Vector3 position, NodeList *list)
{
  NodeList *looper = list;
  while(looper != NULL)
  {
		if(looper->node->position.x==position.x&&looper->node->position.z==position.z)
			return looper->node;
		looper = looper->next;
  }
  return 0;
}

NodeList *GetNavNodes(Node *start)
{
	NodeList *open = NULL;
	NodeList *closed = NULL;
	Node *current = NULL;

	enqueueNode(start,&open);

	while(open)
	{
		int i = 0;
		current = dequeueNode(&open);
		memset(&current->neighbors[0],0,sizeof(current->neighbors));
		for(int x=-1;x<2;x++)
		{
			for(int z=-1;z<2;z++)
			{
				Vector3 newpos = Vector3Add(current->position,(Vector3){(float)x*GRIDRESOLUTION,current->position.y,(float)z*GRIDRESOLUTION});
				if(!(x==0&&z==0))
				{
					Node *newnode;
					int isclosed = 0;
					if(!GetNodeFromQueueWithPosition(newpos,open)&&!GetNodeFromQueueWithPosition(newpos,closed))
					{
						Ray hitray;
						RayHitInfo hitrayinfo;
						hitray.position = Vector3Add(current->position,(Vector3){0.0f,2.1f * GRIDRESOLUTION,0.0f});
						hitray.direction = (Vector3){x,0.0f,z};
						hitrayinfo = GetCollisionRayMesh(hitray,&map.mesh);
						if(hitrayinfo.hit)
						{
								if(Vector3SquaredDistance(hitrayinfo.position,newpos)<RAYHITLENGTHHOR)
								{
									isclosed = 1;
								}
						}
						if(!isclosed)
						{
              hitray.position = newpos;
              hitray.direction = (Vector3){0.0f,-1.0f,0.0f};
              hitrayinfo = GetCollisionRayMesh(hitray,&map.mesh);
              if(hitrayinfo.hit)
              {
                newpos.y = hitrayinfo.position.y;
              }
              else
              {
                hitray.direction = (Vector3){0.0f,1.0f,0.0f};
                hitrayinfo = GetCollisionRayMesh(hitray,&map.mesh);
                newpos.y = hitrayinfo.position.y;
              }
							newnode = malloc(sizeof(Node));
							newnode->position = newpos;
							current->neighbors[i] = newnode;
							enqueueNode(newnode,&open);
            }
					}
					else
					{
						current->neighbors[i] = GetNodeFromQueueWithPosition(newpos,open);
						if(!current->neighbors[i])
							current->neighbors[i] = GetNodeFromQueueWithPosition(newpos,closed);
// add current as neighbor to the newpos node too
						Node *newposnode = current->neighbors[i];
						for(int i=0;i<NUMNEIGHBORS;i++)
						{
							if(newposnode->neighbors[i] == current)
								break;
							if(!newposnode->neighbors[i])
							{
									newposnode->neighbors[i] = current;
									break;
							}
						}
					}

					i++;
				}
			}
		}
		enqueueNode(current,&closed);
	}
	return closed;
}
NodeList *FindPath(Vector3 opos,Vector3 tpos,NodeList *NavNodes)
{
printf("finding path\n");
	NodeList *closed = NULL;
	NodeList *open = NULL;
	Node *start = GetNodeFromQueueWithPosition(opos,NavNodes);
	if(!start)
	{
printf("start not found\n");
		return 0;
	}
  opos = Vector3Round(opos);
  tpos = Vector3Round(tpos);
	enqueueNode(start,&open);
	while(open)
	{
		Node *current = dequeueNodeLowestf(&open);
		if(current->position.x == Vector3Round(tpos).x&&current->position.y == Vector3Round(tpos).y&&current->position.z == Vector3Round(tpos).z)
		{
// construct path
			NodeList *path = NULL;
			NodeList *looprev = NULL;
			while(current->position.x != Vector3Round(opos).x||current->position.y != Vector3Round(opos).y||current->position.z != Vector3Round(opos).z)
			{
				enqueueNode(current,&path);
				current = current->prev;
			}
printf("path found\n");
			return path;
		}
		enqueueNode(current,&closed);
		for(int i=0;i<NUMNEIGHBORS;i++)
		{
			Node *neighbor = current->neighbors[i];
			if(neighbor)
			{
				if(GetNodeFromQueueWithPosition(neighbor->position,closed))
					continue;
				if(!GetNodeFromQueueWithPosition(neighbor->position,open)||current->gcost+1<neighbor->gcost)
				{
					neighbor->prev = current;
					neighbor->gcost = current->gcost+1;
					neighbor->fcost = neighbor->gcost + Vector3SquaredDistance(neighbor->position,tpos);
					if(!GetNodeFromQueueWithPosition(neighbor->position,open))
						enqueueNode(neighbor,&open);
				}
			}
		}
	}
printf("path not found\n");
	return 0;
}
void FreeNodeList(NodeList **list)
{
  Node *node;
  while(*list)
  {
    node = dequeueNode(list);
    free(node);
  }
}

void PrintNodeList(NodeList *list)
{
  NodeList *looper = list;
  while(looper != NULL)
  {
    printf("node at: x:%f,z:%f,y:%f",looper->node->position.x,looper->node->position.z,looper->node->position.y);
    looper = looper->next;
  }
}

void LoadMap(const char *filename)
{
  Texture2D texture;
  map = LoadModel("./resources/map1.obj");;
  texture = LoadTexture("./resources/tileset.png");
  map.material.maps[MAP_DIFFUSE].texture = texture;
}
void UnloadMap()
{
  UnloadTexture(map.material.maps[MAP_DIFFUSE].texture);
  UnloadModel(map);
}
void DrawMap()
{
  DrawModel(map,(Vector3){0.0f,0.0f,0.0f},1.0f,WHITE);
}
void DrawNodeList(NodeList *list, Color color)
{
  NodeList *looper = list;

  while(looper != NULL)
  {
    DrawCube(looper->node->position,0.5,0.5f,0.5f,color);
    looper = looper->next;
  }
}
