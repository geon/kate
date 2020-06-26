#ifndef dirty_background_strips_h
#define dirty_background_strips_h

#include <stdbool.h>


typedef struct DirtyBackgroundStripsStruct *DirtyBackgroundStrips;

DirtyBackgroundStrips makeDirtyBackgroundStrips (bool alternateBuffer);
void freeDirtyBackgroundStrips (DirtyBackgroundStrips dirtyBackgroundStrips);
unsigned long int getDirtyBackgroundStripsNumIndices (DirtyBackgroundStrips dirtyBackgroundStrips);
unsigned short int *getDirtyBackgroundStripsIndices (DirtyBackgroundStrips dirtyBackgroundStrips);
void dirtyBackgroundStripsClear (DirtyBackgroundStrips dirtyBackgroundStrips);
void dirtyBackgroundStripsMark (DirtyBackgroundStrips dirtyBackgroundStrips, unsigned short int index);

#endif
