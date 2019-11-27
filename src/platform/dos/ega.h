#ifndef ega_h
#define ega_h

#include "bitplane_strip.h"

#define EGA_BUFFER_NUM_COLUMNS 80


void waitForFrame ();
void setVideoMode ();
void drawPoint (unsigned short int x, unsigned short int y, unsigned char color);
void drawStrip (short int column, short int y, BitplaneStrip strip, unsigned char mask);
void copyStrip (short int column, short int y);
void pasteStrip (short int column, short int y, unsigned char mask);
void setPalette (unsigned char palette[16]);

#endif
