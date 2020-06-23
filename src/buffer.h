#ifndef buffer_h
#define buffer_h

#include "platform/dos/ega.h"
#include "strip_coord.h"
#include "ega_scroll_coord.h"
#include "pixel_coord.h"

#include <stdlib.h>


#define EGA_BUFFER_NUM_COLUMNS 88
#define EGA_BUFFER_NUM_COLUMNS_DEFAULT 80
// Bytes per plane, or number of pixels/8.
#define EGA_PLANE_SIZE 65536
// Half of the 64 kB plane for each of the 2 buffers.
#define EGA_BUFFER_SIZE 32768

typedef struct BufferStruct *Buffer;


Buffer makeBuffer ();
void freeBuffer (Buffer buffer);

void updateBuffer (Buffer buffer);
void setBufferScroll (Buffer buffer, PixelCoord scroll);

StripCoord bufferMapWorldCoordToBufferCoord (Buffer buffer, StripCoord worldCoord);
unsigned short int bufferMapBufferCoordToBufferIndex (Buffer buffer, StripCoord bufferCoord);

StripCoord bufferMapBufferIndexToBufferCoord (Buffer buffer, unsigned short int bufferIndex);
StripCoord bufferMapBufferCoordToWorldCoord (Buffer buffer, StripCoord bufferCoord);

#endif
