#ifndef ega_h
#define ega_h

#include "bitplane_strip.h"

#include "stdbool.h"


void waitForFrame ();
void setVideoMode ();
void drawPoint (unsigned short int x, unsigned short int y, unsigned char color);
void drawStrip (unsigned short int index, BitplaneStrip strip, unsigned char mask);
void copyStrip (unsigned short int index);
void pasteStrip (unsigned short int index, unsigned char mask);
void setPalette (unsigned char palette[16]);
void setVirtualScreenWidth (unsigned short int numColumns);
void setBufferOffset (unsigned short int offset, unsigned char restX);

#endif
