#include "raylib.h"
#include "raymath.h"
#include "math.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#define MAXNODES 3000
#define WIDTH 10
#define NUMNEIGHBORS 8
#define GRIDRESOLUTION 1
#define RAYHITLENGTH GRIDRESOLUTION+0.1f
Model map;
Camera camera;
typedef struct Node Node;
struct Node
{
	Vector3 position;
	Node *neighbors[NUMNEIGHBORS];
	float fcost;
	float gcost;
	Node *prev;
};
typedef struct NodeList NodeList;
struct NodeList
{
	Node *node;
	NodeList *next;
};
void enqueue(Node *node,NodeList **list);
Node *dequeue(NodeList **list);
Node *GetFromQueueWithPos(Vector3 position, NodeList *list);
void PrintList(NodeList *list);
void DrawNodes(NodeList *list,Color color);
float Vector3FastDistance(Vector3 A,Vector3 B);
NodeList *GenNodes(Node *start);
Vector3 Vector3Round(Vector3 pos);

// path stuff
Node *dequeueLowestf(NodeList **list);
NodeList *FindPath(Vector3 opos,Vector3 tpos,NodeList *NavNodes);

NodeList *FindPath(Vector3 opos,Vector3 tpos,NodeList *NavNodes)
{
	NodeList *closed = NULL;
	NodeList *open = NULL;
	Node *start = GetFromQueueWithPos(opos,NavNodes);
	if(!start)
	{
		return 0;
	}
	enqueue(start,&open);
	while(open)
	{
		Node *current = dequeueLowestf(&open);
		if(current->position.x == Vector3Round(tpos).x&&current->position.y == Vector3Round(tpos).y&&current->position.z == Vector3Round(tpos).z)
		{
// construct path
			NodeList *path = NULL;
			NodeList *looprev = NULL;
			while(current->position.x != Vector3Round(opos).x||current->position.y != Vector3Round(opos).y||current->position.z != Vector3Round(opos).z)
			{
				enqueue(current,&path);
				current = current->prev;
			}
			return path;
		}
		enqueue(current,&closed);
		for(int i=0;i<NUMNEIGHBORS;i++)
		{
			Node *neighbor = current->neighbors[i];
			if(neighbor)
			{
				if(GetFromQueueWithPos(neighbor->position,closed))
					continue;
				if(!GetFromQueueWithPos(neighbor->position,open)||current->gcost+1<neighbor->gcost)
				{
					neighbor->prev = current;
					neighbor->gcost = current->gcost+1;
					neighbor->fcost = neighbor->gcost + Vector3FastDistance(neighbor->position,tpos);
					if(!GetFromQueueWithPos(neighbor->position,open))
						enqueue(neighbor,&open);
				}
			}
		}
	}
	return 0;
}

Node *dequeueLowestf(NodeList **list)
{
	NodeList *looper = NULL;
	NodeList *last = NULL;
	NodeList *lowestf = NULL;
	Node *node = NULL;
	if(*list == NULL)
		return NULL;
	looper = *list;
	lowestf = *list;
	while(looper->next != NULL)
	{
		if(looper->next->node->fcost < lowestf->node->fcost)
		{
			last = looper;
			lowestf = looper->next;
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
Vector3 Vector3Round(Vector3 pos)
{
	Vector3 rounded;
	rounded.x = floor(pos.x+0.5);
	rounded.y = floor(pos.y+0.5);
	rounded.z = floor(pos.z+0.5);
	return rounded;
}
Vector3 ShowNeighborOnMouseOver(NodeList *navnodes)
{
	Vector3 pos;
	Ray ray = GetMouseRay(GetMousePosition(),camera);
	RayHitInfo hitinfo = GetCollisionRayMesh(ray,&map.mesh);
	if(hitinfo.hit)
	{
		pos = Vector3Round(hitinfo.position);
		Node *node = GetFromQueueWithPos(pos,navnodes);
		if(node)
			{
				DrawCube(node->position, 0.5f, 0.5f, 0.5f, YELLOW);              // Draw cube
			for(int i=0;i<NUMNEIGHBORS;i++)
			{
				if(node->neighbors[i])
				{
				DrawCube(node->neighbors[i]->position, 0.5f, 0.5f, 0.5f, YELLOW);              // Draw cube
				}
			}
				return node->position;
		}
	}
	return (Vector3){0.0f,0.0f,0.0f};
}

void enqueue(Node *node,NodeList **list)
{
	NodeList *newNode = malloc(sizeof(NodeList));
	newNode->node = node;
	newNode->next = *list;
	*list = newNode;
}
Node *dequeue(NodeList **list)
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
Node *GetFromQueueWithPos(Vector3 position, NodeList *list)
{
	NodeList *looper = list;
	while(looper != NULL)
	{
		if(looper->node->position.x==position.x&&looper->node->position.y==position.y&&looper->node->position.z==position.z)
			return looper->node;
		looper = looper->next;
	}
	return 0;
}
void PrintList(NodeList *list)
{
	NodeList *looper = list;
	while(looper != NULL)
	{
		looper = looper->next;
	}
}
void DrawNodes(NodeList *list,Color color)
{
	NodeList *looper = list;

	Vector3 newpos;
	while(looper != NULL)
	{
		newpos = Vector3Add(looper->node->position,(Vector3){0.0f,0.0f,0.0f});
		looper = looper->next;
		DrawPlane(newpos,(Vector2){0.1f,0.1f},color);
		DrawCube(newpos, 0.5f, 0.5f, 0.5f, color);              // Draw cube
//		DrawPlane(newpos,(Vector2){0.1f,0.1f},color);
	}
}
float Vector3FastDistance(Vector3 A,Vector3 B)
{
	float dx = B.x-A.x;
	float dy = B.y-A.y;
	float dz = B.z-A.z;
	return dx*dx+dy*dy+dz*dz;
}
NodeList *GenNodes(Node *start)
{
	int nodescnt = 0;
	NodeList *open = NULL;
	NodeList *closed = NULL;
	Node *current = NULL;
	enqueue(start,&open);
	while(open&&nodescnt < MAXNODES)
	{
		int i = 0;
		current = dequeue(&open);
		memset(&current->neighbors[0],0,sizeof(current->neighbors));
		for(int x=-1;x<2;x++)
		{
			for(int y=-1;y<2;y++)
			{
				Vector3 newpos = Vector3Add(current->position,(Vector3){(float)x*GRIDRESOLUTION,0.0f,(float)y*GRIDRESOLUTION});
				if(!(x==0&&y==0))
				{
					Node *newnode;
					int isclosed = 0;
					if(!GetFromQueueWithPos(newpos,open)&&!GetFromQueueWithPos(newpos,closed))
					{
						nodescnt++;
						Ray hitray;
						RayHitInfo hitrayinfo;
						hitray.position = current->position;
						hitray.direction = (Vector3){x,0,y};
						hitrayinfo = GetCollisionRayMesh(hitray,&map.mesh);
						if(hitrayinfo.hit)
						{
								if(Vector3FastDistance(hitrayinfo.position,newpos)<RAYHITLENGTH)
								{
									isclosed = 1;
								}
						}
						if(!isclosed)
						{
							newnode = malloc(sizeof(Node));
							newnode->position = newpos;
							current->neighbors[i] = newnode;
							enqueue(newnode,&open);

						}
					}
					else
					{
						current->neighbors[i] = GetFromQueueWithPos(newpos,open);
						if(!current->neighbors[i])
							current->neighbors[i] = GetFromQueueWithPos(newpos,closed);
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
		enqueue(current,&closed);
	}
	return closed;
}

void ClearList(NodeList **list)
{
	while(*list)
	{
		dequeue(list);
	}
}
int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    int screenWidth = 800;
    int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [models] example - obj model loading");

    // Define the camera to look into our 3d world
    camera = (Camera){(Vector3){ 3.0f, 3.0f, 3.0f }, (Vector3){ 0.0f, 1.5f, 0.0f }, (Vector3){ 0.0f, 1.0f, 0.0f }, 45.0f };
		SetCameraMode(camera,CAMERA_FREE);

    map = LoadModel("../resources/map2.obj");                   // Load OBJ model
		Texture2D texture = LoadTexture("../resources/tileset.png");   // Load model texture
    map.material.maps[MAP_DIFFUSE].texture = texture;                     // Set map diffuse texture
    Vector3 position = { 0.0f, 0.0f, 0.0f };                                // Set model position

    SetTargetFPS(60);   // Set our game to run at 60 frames-per-second
// nodes
	Node *start = malloc(sizeof(Node));
	start->position = (Vector3){0.0f,0.0f,0.0f};
	NodeList *NavNodes;
	NavNodes = GenNodes(start);
    //--------------------------------------------------------------------------------------
Vector3 opos = {0.0f,0.0f,0.0f};
Vector3 tpos = {5.0f,0.0f,5.0f};
NodeList *path = NULL;



    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
				UpdateCamera(&camera);
        // Update
        //----------------------------------------------------------------------------------
        //...
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(BLACK);

            Begin3dMode(camera);
Vector3 gizmo = {0.0f,0.0f,0.0f};
//x,y,z
DrawGizmo(gizmo);

                DrawModel(map, position, 1.0f, WHITE);   // Draw 3d model with texture
DrawNodes(NavNodes,GREEN);

opos = ShowNeighborOnMouseOver(NavNodes);
if(IsKeyDown(KEY_M))
{
	printf("pathgen\n");
	path = FindPath(opos,tpos,NavNodes);
	ClearList(&path);
	PrintList(path);

}
DrawNodes(path,PURPLE);
            End3dMode();

            DrawFPS(10, 10);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
		UnloadTexture(texture);
    UnloadModel(map);         // Unload model

    CloseWindow();              // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
