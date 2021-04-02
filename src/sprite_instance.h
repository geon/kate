#ifndef sprite_instance_h
#define sprite_instance_h

#include "sprite_shift_cache.h"
#include "pixel_coord.h"
#include "vector.h"

#include <stdint.h>
#include <stdlib.h>


typedef struct SpriteInstance {
    SpriteShiftCache sprite;
	PixelCoord pos;
} SpriteInstance;


declareVector(SpriteInstanceVector, SpriteInstance, spriteInstanceVector)
defineVectorStruct(SpriteInstanceVector, SpriteInstance, spriteInstanceVector)


SpriteInstance makeSpriteInstance (Sprite sprite, PixelCoord pos);
void spriteInstanceDraw(SpriteInstance *spriteInstance, Map map, Buffer buffer, PositionAndStripVector stripBatch);

#endif
