#include "arenathing.h"

Tile *LoadMap(int *w, int *h)
{
	FILE*fp;
	fp = fopen("map.txt","r");
	if(!fp)
	{
		printf("error opening map");
		return 0;
	}

	fscanf(fp,"%i %i",w,h);
	fgetc(fp);

	int width = (*w);
	int height = (*h);

	Tile *map = malloc(width*height*sizeof(Tile));

	int x = 0;
	int y = 0;
	char i = fgetc(fp);

	while(i!= EOF)
	{
		if(i == '\n')
		{
			y++;
			x = 0;
		}
		else
		{
			switch((int)i)
			{
				case 49:
					map[x+y*width].graphic = 49;
					map[x+y*width].canWalk = true;
					map[x+y*width].canSee = true;
					break;
				case 50:
					map[x+y*width].graphic = 50;
					map[x+y*width].canWalk = false;
					map[x+y*width].canSee = false;
					break;
			}
			x++;
		}
		i = fgetc(fp);
	}
	fclose(fp);
	mapmodel = LoadModel("resources/map1.obj");
	maptexture = LoadTexture("resources/tileset.png");
	mapmodel.material.maps[MAP_DIFFUSE].texture = maptexture;
	return map;
}
void DrawMap(Tile *map, int w, int h)
{
	DrawModel(mapmodel,(Vector3){0.5f,0.0f,-0.5f},1.0f,WHITE);
	/*
	int x, y;
	Vector3 pos;
	Color color;
	for(y=0;y<h;y++)
	{
		for(x=0;x<w;x++)
		{
// draw
			pos = (Vector3){x,0,y};
			color;
			switch(map[x+y*w].graphic)
			{
				case 49:
					color = GREEN;
					break;
				case 50:
					color = BLUE;
					break;
			}
			DrawPlane(pos,(Vector2){1.0f,1.0f},color);
		}
	}
	*/
	return;
}
