#ifndef world_h
#define world_h

#include "platform/dos/bitplane_strip.h"
#include "dirty_background_strips.h"


#define EGA_BUFFER_NUM_COLUMNS 88
#define EGA_BUFFER_NUM_COLUMNS_DEFAULT 80
// Bytes per plane, or number of pixels/8.
#define EGA_PLANE_SIZE 65536
// Half of the 64 kB plane for each of the 2 buffers.
#define EGA_BUFFER_SIZE 32768


typedef struct WorldStruct *World;

World makeWorld (char **errorMessage);
void freeWorld (World world);
unsigned char * getWorldPalette(World world);
void updateWorld (World world);
void renderWorld(World world);

#endif
