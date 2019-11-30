#ifndef ega_h
#define ega_h

#include "bitplane_strip.h"

#define EGA_BUFFER_NUM_COLUMNS 88
#define EGA_BUFFER_NUM_COLUMNS_DEFAULT 80
// Bytes per plane, or number of pixels/8.
#define EGA_PLANE_SIZE 65536


void waitForFrame ();
void setVideoMode ();
void drawPoint (unsigned short int x, unsigned short int y, unsigned char color);
unsigned short int stripCoordToIndex (unsigned short int column, unsigned short int y);
void drawStrip (unsigned short int index, BitplaneStrip strip, unsigned char mask);
void copyStrip (unsigned short int index);
void pasteStrip (unsigned short int index, unsigned char mask);
void setPalette (unsigned char palette[16]);
void setVirtualScreenWidth (unsigned short int numColumns);
void setScroll (unsigned short int x, unsigned short int y);

#endif
