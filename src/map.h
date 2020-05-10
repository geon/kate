#ifndef map_h
#define map_h

#include "platform/dos/bitplane_strip.h"
#include "sprite.h"

BitplaneStrip getStripAtWorldCoord(Sprite backgroundImage, unsigned short int column, unsigned short int y);

#endif


