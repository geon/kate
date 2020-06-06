#ifndef coord_conversion_h
#define coord_conversion_h

#include "pixel_coord.h"
#include "strip_coord.h"
#include "ega_scroll_coord.h"

PixelCoord makePixelCoordFromStripCoord (StripCoord stripCoord);
StripCoord makeStripCoordFromPixelCoord (PixelCoord pixelCoord);
StripCoord makeStripCoordFromEgaScrollCoord (EgaScrollCoord egascrollCoord);
PixelCoord makePixelCoordFromEgaScrollCoord (EgaScrollCoord egascrollCoord);
EgaScrollCoord makeEgaScrollCoordFromPixelCoord (PixelCoord pixelCoord);

#endif
