#ifndef dirty_background_strips_h
#define dirty_background_strips_h

#include <stdbool.h>
#include "strip_coord.h"


typedef struct DirtyBackgroundStripsStruct *DirtyBackgroundStrips;

DirtyBackgroundStrips makeDirtyBackgroundStrips (bool alternateBuffer);
void freeDirtyBackgroundStrips (DirtyBackgroundStrips dirtyBackgroundStrips);
unsigned long int getDirtyBackgroundStripsNumCoords (DirtyBackgroundStrips dirtyBackgroundStrips);
StripCoord *getDirtyBackgroundStripsCoords (DirtyBackgroundStrips dirtyBackgroundStrips);
void dirtyBackgroundStripsClear (DirtyBackgroundStrips dirtyBackgroundStrips);
void dirtyBackgroundStripsMark (DirtyBackgroundStrips dirtyBackgroundStrips, StripCoord coord);

#endif
