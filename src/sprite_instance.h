#ifndef sprite_instance_h
#define sprite_instance_h

#include "sprite.h"
#include "pixel_coord.h"
#include "vector.h"

#include <stdint.h>
#include <stdlib.h>


typedef struct SpriteInstance {
    Sprite sprite;
	PixelCoord pos;
} SpriteInstance;

SpriteInstance makeSpriteInstance (Sprite sprite, PixelCoord pos);


declareVector(SpriteInstanceVector, SpriteInstance, spriteInstanceVector)
defineVectorStruct(SpriteInstanceVector, SpriteInstance, spriteInstanceVector)

#endif
