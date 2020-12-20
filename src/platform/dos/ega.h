#ifndef ega_h
#define ega_h

#include "bitplane_strip.h"

#include <stdbool.h>
#include <stdint.h>


void waitForFrame ();
void setVideoMode ();
void drawPoint (uint16_t x, uint16_t y, unsigned char color);
void drawStrip (uint16_t index, BitplaneStrip strip, unsigned char mask);
void copyStrip (uint16_t index);
void pasteStrip (uint16_t index, unsigned char mask);
void setPalette (unsigned char palette[16]);
void setVirtualScreenWidth (uint16_t numColumns);
void setBufferOffset (uint16_t offset, unsigned char restX);

#endif
