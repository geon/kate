#ifndef world_h
#define world_h

#include "renderer.h"


typedef struct WorldStruct *World;

World makeWorld (Renderer renderer, char **errorMessage);
void freeWorld (World world);
unsigned char * getWorldPalette(World world);
void worldUpdate (World world);
void setWorldScroll (unsigned short int x, unsigned short int y, bool alternateBuffer);
void worldRender(World world);

#endif
