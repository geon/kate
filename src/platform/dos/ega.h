#ifndef ega_h
#define ega_h

#include "bitplane_strip.h"

#include <stdbool.h>
#include <stdint.h>


void waitForFrame ();
void setVideoMode ();
void drawPoint (uint16_t x, uint16_t y, uint8_t color);
void drawStrip (uint16_t index, BitplaneStrip strip, uint8_t mask);
void drawStrips (uint16_t* beginIndex, uint16_t* endIndex, BitplaneStrip bitplaneStrip);
void copyStrip (uint16_t index);
void pasteStrip (uint16_t index, uint8_t mask);
void setPalette (uint8_t palette[16]);
void setVirtualScreenWidth (uint16_t numColumns);
void setBufferOffset (uint16_t offset, uint8_t restX);

#endif
