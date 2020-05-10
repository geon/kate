#ifndef map_h
#define map_h

#include "platform/dos/bitplane_strip.h"
#include "sprite.h"

typedef struct MapStruct *Map;

Map makeMap (char **errorMessage);
void freeMap (Map map);
BitplaneStrip getStripAtWorldCoord (Map map, unsigned short int column, unsigned short int y);

#endif


