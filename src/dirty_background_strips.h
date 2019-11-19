#ifndef dirty_background_strips_h
#define dirty_background_strips_h

#include <stdbool.h>


typedef bool *DirtyBackgroundStrips;

DirtyBackgroundStrips makeDirtyBackgroundStrips ();
void freeDirtyBackgroundStrips (DirtyBackgroundStrips dirtyBackgroundStrips);

#endif
