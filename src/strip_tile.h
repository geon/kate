#ifndef strip_tile_h
#define strip_tile_h

#include "platform/dos/bitplane_strip.h"


typedef struct StripTile {
    BitplaneStrip bitPlaneStrips[8];
} StripTile;

#endif
