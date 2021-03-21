#ifndef buffer_h
#define buffer_h

#include "platform/dos/ega.h"
#include "strip_coord.h"
#include "ega_scroll_coord.h"
#include "pixel_coord.h"
#include "map.h"
#include "uint16_vector.h"

#include <stdlib.h>
#include <stdint.h>


#define EGA_BUFFER_NUM_COLUMNS 88
#define EGA_BUFFER_NUM_COLUMNS_DEFAULT 80
#define EGA_BUFFER_NUM_ROWS_DEFAULT 350
// Bytes per plane, or number of pixels/8.
#define EGA_PLANE_SIZE 65536
// Half of the 64 kB plane for each of the 2 buffers.
#define EGA_BUFFER_SIZE 32768

typedef struct BufferStruct *Buffer;


Buffer makeBuffer ();
void freeBuffer (Buffer buffer);

void switchBuffer (Buffer buffer, PixelCoord scroll, Map map);

uint16_t bufferMapWorldCoordToBufferIndex (Buffer buffer, StripCoord worldCoord);
uint16_t bufferMapWorldCoordToBufferIndexInAlternateBuffer (StripCoord worldCoord, EgaScrollCoord bufferScroll, bool bufferAlternateBuffer);
StripCoord bufferStaticMapWorldCoordToBufferCoord (EgaScrollCoord bufferScroll, StripCoord worldCoord);
uint16_t bufferStaticMapBufferCoordToBufferIndex (EgaScrollCoord bufferScroll, bool alternateBuffer, StripCoord bufferCoord);

StripCoord bufferMapBufferIndexToWorldCoord (Buffer buffer, uint16_t bufferIndex);
StripCoord bufferStaticMapBufferIndexToBufferCoord (EgaScrollCoord bufferScroll, bool alternateBuffer, uint16_t bufferIndex);
StripCoord bufferStaticMapBufferCoordToWorldCoord (EgaScrollCoord bufferScroll, StripCoord bufferCoord);

Uint16Vector bufferGetDirtyBufferIndicesByStripIndex (Buffer buffer);
uint16_t * bufferDirtyBackgroundStripsBegin (Buffer buffer);
uint16_t * bufferDirtyBackgroundStripsEnd (Buffer buffer);
void bufferClearDirtyBackgroundStrips (Buffer buffer);
// void bufferMarkDirtyBackgroundStrips(Buffer buffer, StripCoord coord, BitplaneStrip cleanStrip);

void bufferMarkRectangleAsDirty (Buffer buffer, StripCoord topLeftWorldCoord, StripCoord bottomRightWorldCoord, Map map);

#endif
