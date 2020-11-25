#ifndef buffer_h
#define buffer_h

#include "platform/dos/ega.h"
#include "strip_coord.h"
#include "ega_scroll_coord.h"
#include "pixel_coord.h"
#include "dirty_background_strips.h"

#include <stdlib.h>


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

void switchBuffer (Buffer buffer);
void setBufferScroll (Buffer buffer, PixelCoord scroll);

unsigned short int bufferMapWorldCoordToBufferIndex (Buffer buffer, StripCoord worldCoord);
StripCoord bufferStaticMapWorldCoordToBufferCoord (EgaScrollCoord bufferScroll, StripCoord worldCoord);
unsigned short int bufferStaticMapBufferCoordToBufferIndex (EgaScrollCoord bufferScroll, bool alternateBuffer, StripCoord bufferCoord);

StripCoord bufferMapBufferIndexToWorldCoord (Buffer buffer, unsigned short int bufferIndex);
StripCoord bufferStaticMapBufferIndexToBufferCoord (EgaScrollCoord bufferScroll, bool alternateBuffer, unsigned short int bufferIndex);
StripCoord bufferStaticMapBufferCoordToWorldCoord (EgaScrollCoord bufferScroll, StripCoord bufferCoord);

unsigned long int bufferGetDirtyBackgroundStripsNumCoords (Buffer buffer);
StripCoord *bufferGetDirtyBackgroundStripsCoords (Buffer buffer);
void bufferClearDirtyBackgroundStrips (Buffer buffer);
void bufferMarkDirtyBackgroundStrips(Buffer buffer, StripCoord coord);

#endif
