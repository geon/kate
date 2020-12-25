#ifndef bitplane_strip_h
#define bitplane_strip_h

#include "../../vector.h"

#include <stdint.h>


typedef struct BitplaneStrip {
    uint8_t planes[4];
} BitplaneStrip;

BitplaneStrip makeBitplaneStrip (uint32_t nibbleStrip);
uint32_t bitplaneStripAsInt (BitplaneStrip);


declareVector(BitplaneStripVector, BitplaneStrip, bitplaneStripVector)
defineVectorStruct(BitplaneStripVector, BitplaneStrip, bitplaneStripVector)

#endif
