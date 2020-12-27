#ifndef dirty_background_strips_h
#define dirty_background_strips_h

#include <stdbool.h>
#include "strip_coord.h"
#include "map.h"
#include "indices_grouped_by_strip.h"


typedef struct DirtyBackgroundStripsStruct *DirtyBackgroundStrips;

DirtyBackgroundStrips makeDirtyBackgroundStrips (bool alternateBuffer);
void freeDirtyBackgroundStrips (DirtyBackgroundStrips dirtyBackgroundStrips);

IndicesByStripTableRow* dirtyBackgroundStripsBegin (DirtyBackgroundStrips dirtyBackgroundStrips);
IndicesByStripTableRow* dirtyBackgroundStripsEnd (DirtyBackgroundStrips dirtyBackgroundStrips);
void dirtyBackgroundStripsClear (DirtyBackgroundStrips dirtyBackgroundStrips);
void dirtyBackgroundStripsMark (DirtyBackgroundStrips dirtyBackgroundStrips, StripCoord coord, BitplaneStrip cleanStrip);
void dirtyBackgroundStripsMarkRectangle (DirtyBackgroundStrips dirtyBackgroundStrips, StripCoord topLeftWorldCoord, StripCoord bottomRightWorldCoord, Map map);

#endif
