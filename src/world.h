#ifndef world_h
#define world_h

#include "platform/dos/bitplane_strip.h"
#include "image.h"
#include "dirty_background_strips.h"


typedef struct World {
	Image image;
	unsigned int frame;
	DirtyBackgroundStrips dirtyBackgroundStrips;

	int radius, posX, posY;
} World;


void makeWorld (World *world);
void freeWorld (World world);
void updateWorld (World *world);
void renderWorld(World world);

#endif
