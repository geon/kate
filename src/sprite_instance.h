#ifndef sprite_instance_h
#define sprite_instance_h

#include "sprite.h"
#include "vector.h"

#include <stdint.h>
#include <stdlib.h>


typedef struct SpriteInstance {
    Sprite sprite;
	int16_t posX;
	int16_t posY;
} SpriteInstance;

SpriteInstance makeSpriteInstance (Sprite sprite, int16_t posX, int16_t posY);


declareVector(SpriteInstanceVector, SpriteInstance, spriteInstanceVector)
defineVectorStruct(SpriteInstanceVector, SpriteInstance, spriteInstanceVector)

#endif
