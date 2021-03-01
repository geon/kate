#ifndef dirty_background_strips_h
#define dirty_background_strips_h

#include <stdbool.h>
#include "strip_coord.h"
#include <stdint.h>
#include "map.h"
#include "buffer.h"


typedef struct DirtyBackgroundStripsStruct *DirtyBackgroundStrips;

DirtyBackgroundStrips makeDirtyBackgroundStrips (bool alternateBuffer);
void freeDirtyBackgroundStrips (DirtyBackgroundStrips dirtyBackgroundStrips);

Uint16Vector dirtyBackgroundStripsGetDirtyBufferIndicesByStripIndex (DirtyBackgroundStrips dirtyBackgroundStrips);

uint16_t * dirtyBackgroundStripsBegin (DirtyBackgroundStrips dirtyBackgroundStrips);
uint16_t * dirtyBackgroundStripsEnd (DirtyBackgroundStrips dirtyBackgroundStrips);

void dirtyBackgroundStripsClear (DirtyBackgroundStrips dirtyBackgroundStrips);
void dirtyBackgroundStripsMark (DirtyBackgroundStrips dirtyBackgroundStrips, uint16_t bufferIndex,  uint16_t cleanStripIndex);
void dirtyBackgroundStripsMarkRectangle (DirtyBackgroundStrips dirtyBackgroundStrips, StripCoord topLeftWorldCoord, StripCoord bottomRightWorldCoord, Map map, Buffer buffer, bool alternateBuffer);

#endif
