#ifndef position_and_strip_h
#define position_and_strip_h

#include "platform/dos/bitplane_strip.h"
#include "vector.h"
#include <stdint.h>


typedef struct PositionAndStrip {
    uint16_t pos;
	BitplaneStrip strip;
} PositionAndStrip;


declareVector(PositionAndStripVector, PositionAndStrip, positionAndStripVector)
defineVectorStruct(PositionAndStripVector, PositionAndStrip, positionAndStripVector)

#endif
