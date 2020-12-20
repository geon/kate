#ifndef bitplane_strip_h
#define bitplane_strip_h

#include <stdint.h>


typedef struct BitplaneStrip {
    unsigned char planes[4];
} BitplaneStrip;

BitplaneStrip makeBitplaneStrip (uint32_t nibbleStrip);
uint32_t bitplaneStripAsInt (BitplaneStrip);

#endif
