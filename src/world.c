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
	SpriteInstanceVectorStruct spriteInstances;

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
		initializeSpriteInstanceVector(&world->spriteInstances, spritePathArrayLength);
		{
			PixelCoord pos = {.x=0, .y=0};
			uint16_t i;
			for (i=0; i<spritePathArrayLength; ++i) {
				Sprite sprite;
				if (!(sprite = rendererLoadSprite(world->renderer, spritePaths[i], errorMessage))) {
					return NULL;
				}
				spriteInstanceVectorPush(&world->spriteInstances, makeSpriteInstance(sprite, pos));
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
	destroySpriteInstanceVector(&world->spriteInstances);
	free(world);
}


void worldUpdate (World world) {
	SpriteInstance *spriteInstance;
	uint16_t i;

	world->frame++;

	world->scroll.x = world->frame * 3;
	world->scroll.y = world->frame * 3;

	vectorForeachIndex (spriteInstanceVectorBegin(&world->spriteInstances), spriteInstanceVectorEnd(&world->spriteInstances), spriteInstance, i) {
		spriteInstance->pos.x = world->frame + 150 + i*11;
		spriteInstance->pos.y = world->frame + 150;
	}
}


void worldRender(World world) {
	rendererRender(world->renderer, &world->spriteInstances, world->map, world->scroll);
}
