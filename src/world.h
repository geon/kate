#ifndef world_h
#define world_h

#include "platform/dos/bitplane_strip.h"
#include "dirty_background_strips.h"


typedef struct WorldStruct *World;

World makeWorld (char **errorMessage);
void freeWorld (World world);
unsigned char * getWorldPalette(World world);
void updateWorld (World world);
void worldSetScroll (unsigned short int x, unsigned short int y, bool alternateBuffer);
void renderWorld(World world);

#endif
