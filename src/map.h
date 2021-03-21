#ifndef map_h
#define map_h

#include "platform/dos/bitplane_strip.h"
#include "strip_coord.h"

typedef struct MapStruct *Map;

Map makeMap (char **errorMessage);
void freeMap (Map map);
uint16_t getMapStripAtWorldCoord (Map map, StripCoord worldCoord);
BitplaneStrip *mapGetStrips (Map map);

#endif


