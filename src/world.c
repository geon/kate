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
#include <assert.h>
#include <stdint.h>


typedef struct WorldStruct {
	Renderer renderer;
	uint16_t numSpriteInstances;
	SpriteInstance *spriteInstances;

	uint16_t frame;
	Map map;
	PixelCoord scroll;
} WorldStruct;


World makeWorld (Renderer renderer, char **errorMessage) {
    World world = malloc(sizeof(WorldStruct));
	assert(world);
	world->renderer = renderer;
	world->frame = 0;

	{
		char *spritePaths[] =  {
			"../images/bunny3.bmp",
			"../images/bunny4.bmp",
			"../images/bunny1.bmp",
			"../images/bunny2.bmp"
		};
		uint16_t spritePathArrayLength = sizeof(spritePaths) / sizeof(spritePaths[0]);
		world->numSpriteInstances = spritePathArrayLength;
		world->spriteInstances = malloc(sizeof(SpriteInstance) * world->numSpriteInstances);
		assert(world->spriteInstances);
		{
			uint16_t i;
			for (i=0; i<spritePathArrayLength; ++i) {
				Sprite sprite;
				if (!(sprite = rendererLoadSprite(world->renderer, spritePaths[i], errorMessage))) {
					return NULL;
				}
				world->spriteInstances[i] = makeSpriteInstance(sprite, 0, 0);
			}
		}
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
	uint16_t i;

	world->frame++;

	world->scroll.x = world->frame * 12;
	world->scroll.y = world->frame * 12;

	for (i=0; i<world->numSpriteInstances; ++i) {
		world->spriteInstances[i].posX = world->scroll.x + 150 + i*64;
		world->spriteInstances[i].posY = world->scroll.y + 150;
	}
}


void worldRender(World world) {
	rendererRender(world->renderer, world->numSpriteInstances, world->spriteInstances, world->map, world->scroll);
}
