#include "world.h"
#include "bmp.h"
#include "platform/dos/ega.h"
#include "sprite.h"
#include "sprite_instance.h"
#include "sprite_struct.h"
#include "map.h"
#include "pixel_coord.h"
#include "buffer.h"

#include <stdlib.h>
#include <math.h>
#include <string.h>


typedef struct WorldStruct {
	Renderer renderer;
	unsigned short int numSpriteInstances;
	SpriteInstance *spriteInstances;

	unsigned int frame;
	Map map;
	PixelCoord scroll;
} WorldStruct;


World makeWorld (Renderer renderer, char **errorMessage) {
	unsigned short int i;
	char *spritePaths[] =  {
		"../images/bunny3.bmp",
		"../images/bunny4.bmp",
		"../images/bunny1.bmp",
		"../images/bunny2.bmp"
	};
	unsigned int spritePathArrayLength = sizeof(spritePaths) / sizeof(spritePaths[0]);
	Sprite sprite;
    World world = malloc(sizeof(WorldStruct));
	world->renderer = renderer;
	world->frame = 0;

	world->numSpriteInstances = spritePathArrayLength;
	world->spriteInstances = malloc(sizeof(SpriteInstance) * world->numSpriteInstances);
	for (i=0; i<spritePathArrayLength; ++i) {
		if (!(sprite = rendererLoadSprite(world->renderer, spritePaths[i], errorMessage))) {
			return NULL;
		}
		world->spriteInstances[i] = makeSpriteInstance(sprite, 0, 0);
	}

	if (!(world->map = makeMap(errorMessage))) {
		return NULL;
	}

	return world;
}


void freeWorld (World world) {
	freeMap(world->map);
	free(world->spriteInstances);
	free(world);
}


void worldUpdate (World world) {
	unsigned int radius;
	unsigned short int i;
	unsigned int posX;
	unsigned int posY;

	world->frame++;

	world->scroll.x = world->frame * 5;
	world->scroll.y = world->frame * 5;

	radius = sin(world->frame/4.3435674)*20 + 50;
	posX = world->scroll.x + 200 + sin(world->frame/10.0) * radius;
	posY = world->scroll.y + 200 + cos(world->frame/10.0) * radius;

	for (i=0; i<world->numSpriteInstances; ++i) {
		world->spriteInstances[i].posX = posX + i*64;
		world->spriteInstances[i].posY = posY;
	}
}


void worldRender(World world) {
	rendererRender(world->renderer, world->numSpriteInstances, world->spriteInstances, world->map, world->scroll);
}
