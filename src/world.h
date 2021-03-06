#ifndef world_h
#define world_h

#include "renderer.h"


typedef struct WorldStruct *World;

World makeWorld (Renderer renderer, char **errorMessage);
void freeWorld (World world);
uint8_t * getWorldPalette(World world);
void worldUpdate (World world);
void worldRender(World world);

#endif
